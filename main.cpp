#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;
using namespace sf;

int main() {
    // Create a window
    RenderWindow window(VideoMode(600, 600), "SFML Test Window");

    // Create a circle
    CircleShape circle(100.f); // radius = 100
    circle.setFillColor(Color::Green);
    circle.setPosition(200.f, 150.f);

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }

        window.clear();              // Clear the window
        window.draw(circle);         // Draw the circle
        window.display();            // Display the window contents
    }

    return 0;
}
