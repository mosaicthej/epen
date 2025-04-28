#ifndef __INK_ANIMATOR_H__
#define __INK_ANIMATOR_H__
#include <SFML/Graphics.hpp>
#include "ImageProcessor.hpp"
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/core/mat.hpp>
#include <queue>
#include <random>
#include <set>
#include <type_traits>

#define CLOSE_SF_WIND_ON_CUE(w) \
    do{ \
        while(const std::optional event = w.pollEvent())      \
            if (event->is<sf::Event::Closed>() ||                   \
                    (event->is<sf::Event::KeyPressed>() &&          \
                    (event->getIf<sf::Event::KeyPressed>()          \
                     -> code==sf::Keyboard::Key::Escape ||          \
                     event->getIf<sf::Event::KeyPressed>()          \
                     -> code==sf::Keyboard::Key::Q )))              \
                w.close();                                    \
    } while(0)        

#define U(x) std::make_unsigned_t<int>((x))


class InkAnimator {
public:
    InkAnimator(const ImageProcessor& processor, int numInkwells);
    void run(uint stepsPerFrame);

private:
    struct InkFront { sf::Vector2i position; int label; };

    void initializeInkWells();
    bool spawnInkWell();
    void animateInkFlow(uint stepsPerFrame);

    const ImageProcessor& processor_;
    uint numInkwells_;
    bool hasremain_;
    sf::RenderWindow window_;

    // Our RGBA canvas as an OpenCV Mat  
    cv::Mat        canvasMat_;
    sf::Texture    texture_;
    sf::Sprite     sprite_;

    std::queue<InkFront> activeFronts_;
    std::set<std::pair<int,int>> filledPixels_;

    // —— NEW MEMBERS for respawning logic —— 
    std::unordered_map<int,int> componentAreas_;   
    // label -> total pixels
    std::unordered_map<int,int> filledCount_;      
    // label -> pixels filled so far
    std::set<int>               completedComponents_;
    std::mt19937                rng_;

    sf::Color inkColor_ = sf::Color::Black;
};


#endif
