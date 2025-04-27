#ifndef __INK_ANIMATOR_H__
#define __INK_ANIMATOR_H__
#include <SFML/Graphics.hpp>
#include "ImageProcessor.hpp"
#include <queue>
#include <set>

#define CLOSE_SF_WIND(w) {\
    while(const std::optional e = w.pollEvent()) \
        if (e->is<sf::Event::Closed>() || \
                (e->is<sf::Event::KeyPressed>() && \
                 (e->getIf<sf::Event::KeyPressed>() \
                 -> code==sf::Keyboard::Key::Escape || \
                 e->getIf<sf::Event::KeyPressed>() \
                 -> code==sf::Keyboard::Key::Q ))) \
            w.close()\
    }

class InkAnimator {
public:
    InkAnimator(const ImageProcessor& processor, int numInkwells);

    void run();

private:
    void initializeInkWells();
    void animateInkFlow();

    const ImageProcessor& processor_;
    int numInkwells_;
    sf::RenderWindow window_;
    sf::Image canvas_;
    sf::Texture texture_;
    sf::Sprite sprite_;

    struct InkFront {
        sf::Vector2i position;
        int label;
    };
    std::queue<InkFront> activeFronts_;
    std::set<std::pair<int, int>> filledPixels_;

    sf::Color inkColor_ = sf::Color::Black;
};

#endif
