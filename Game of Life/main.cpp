#include <SFML/Graphics.hpp>
#include <time.h>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>

using namespace sf;

const int size = 1000;
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
			}
		}
	}
	return output;
}

int main()
{
    RenderWindow window(VideoMode(size, size), "Game of Life", Style::Close | Style::Titlebar);

    Clock deltaClock;

    // Using vectors instead of stack-allocated arrays
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

        // Using vectors instead of stack-allocated arrays
        std::vector<std::vector<bool>> newCells(size, std::vector<bool>(size));

        // Update cell states and pixel colors
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                int n = get_neighbours(i, j, cells);
                newCells[i][j] = cells[i][j] ? (n == 2 || n == 3) : (n == 3);
                pixels[i * size + j].color = newCells[i][j] ? Color::White : Color::Black;
            }
        }
        std::swap(cells, newCells);

        window.clear(Color::Black);
        window.draw(pixels);
        window.display();
    }
    window.close();
    return 0;
}