#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <vector>
#include <thread>
#include <algorithm>

using namespace sf;

const int size = 3000;
const float prob = 0.1f;

int get_neighbours(int k, int l, const std::vector<std::vector<bool>>& map) {
    int output = 0;
    int iStart = std::max(k - 1, 0);
    int jStart = std::max(l - 1, 0);
    int iEnd = std::min(k + 1, size - 1);
    int jEnd = std::min(l + 1, size - 1);

    for (int i = iStart; i <= iEnd; i++) {
        for (int j = jStart; j <= jEnd; j++) {
            if ((i != k || j != l) && map[i][j]) {
                output++;
                if (output == 4) return output; // Optimization
                // cock

            }
        }
    }
    return output;
}

int main()
{
    RenderWindow window(VideoMode(size, size), "Game of Life", Style::Close | Style::Titlebar);

    std::vector<std::vector<bool>> cells(size, std::vector<bool>(size));
    VertexArray pixels(Points, size * size);

    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    // Initialize cells and pixel positions
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            cells[i][j] = dist(rng) < prob;
            pixels[i * size + j].position = Vector2f(j, i);
        }
    }

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed) window.close();
            if (Keyboard::isKeyPressed(Keyboard::R)) {
                // Reset game
                for (int i = 0; i < size; i++) {
                    for (int j = 0; j < size; j++) {
                        cells[i][j] = dist(rng) < prob;
                    }
                }
            }
        }

        // Prepare a vector to hold the threads
        const int num_threads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads(num_threads);

        // Prepare the newCells grid
        std::vector<std::vector<bool>> newCells(size, std::vector<bool>(size));

        // Divide the grid into chunks and assign each to a thread
        int chunk_size = size / num_threads;
        for (int t = 0; t < num_threads; t++) {
            threads[t] = std::thread([&, t]() {
                int start = t * chunk_size;
                int end = (t == num_threads - 1) ? size : start + chunk_size;

                for (int i = start; i < end; i++) {
                    for (int j = 0; j < size; j++) {
                        int n = get_neighbours(i, j, cells);
                        newCells[i][j] = cells[i][j] ? (n == 2 || n == 3) : (n == 3);
                        pixels[i * size + j].color = newCells[i][j] ? Color::White : Color::Black;
                    }
                }
            });
        }

        // Wait for all threads to finish
        for (auto& thread : threads) {
            thread.join();
        }

        // Swap cell grids
        std::swap(cells, newCells);

        // Draw the updated grid
        window.clear(Color::Black);
        window.draw(pixels);
        window.display();
    }
    window.close();
    return 0;
}