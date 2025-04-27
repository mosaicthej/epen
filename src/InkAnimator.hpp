#ifndef __INK_ANIMATOR_H__
#define __INK_ANIMATOR_H__
#include <SFML/Graphics.hpp>
#include "ImageProcessor.hpp"
#include <queue>
#include <set>
#include <type_traits>

#define CLOSE_SF_WIND_ON_CUE(w) \
    do{ \
        while(const std::optional event = window_.pollEvent())      \
            if (event->is<sf::Event::Closed>() ||                   \
                    (event->is<sf::Event::KeyPressed>() &&          \
                    (event->getIf<sf::Event::KeyPressed>()          \
                     -> code==sf::Keyboard::Key::Escape ||          \
                     event->getIf<sf::Event::KeyPressed>()          \
                     -> code==sf::Keyboard::Key::Q )))              \
                window_.close();                                    \
    } while(0)        

#define U(x) std::make_unsigned_t<int>((x))

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
