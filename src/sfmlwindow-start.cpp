#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
// #include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // std::cout << "OpenCV version: " << CV_VERSION << std::endl;

    sf::RenderWindow window(sf::VideoMode({800, 600}), "SFML window");

    while(window.isOpen()){
        while (const std::optional event = window.pollEvent()){
            if (event->is<sf::Event::Closed>() ||
                (event->is<sf::Event::KeyPressed>() &&
                 event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape))
                window.close();

            window.clear(sf::Color::Black);
            window.display();
        }
    }
}

