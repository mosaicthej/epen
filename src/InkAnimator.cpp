#include "InkAnimator.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <iostream>
#include <optional>
#include <ostream>
#include <random>
#include <type_traits>

InkAnimator::InkAnimator(const ImageProcessor& processor, int numInkwells)
    : processor_(processor), numInkwells_(numInkwells),
      window_(sf::VideoMode({
                  std::make_unsigned_t<int>(processor.getBinaryImage().cols), 
                  std::make_unsigned_t<int>(processor.getBinaryImage().rows)}),
                  "Ink Animation"),
      texture_(),
      sprite_(texture_) /* use a default texture to init */

{
    canvas_.resize({
                  std::make_unsigned_t<int>(processor.getBinaryImage().cols), 
                  std::make_unsigned_t<int>(processor.getBinaryImage().rows)},
                  sf::Color::White);
    if(!texture_.resize({
            std::make_unsigned_t<int>(canvas_.getSize().x), 
            std::make_unsigned_t<int>(canvas_.getSize().y)}))
        std::cerr << "Failed to resize texture" << std::endl;
    sprite_.setTexture(texture_);
    initializeInkWells();

#ifdef DEBUG
    /* testing rendering */
    canvas_.setPixel({10,10}, sf::Color::Red);
    texture_.update(canvas_);
#endif
    
}

void InkAnimator::initializeInkWells() {
    auto& components = processor_.getComponents();
    std::mt19937 rng(std::random_device{}());
#ifdef DEBUG
    std::cout << "Initializing inkwells..." << std::endl;
#endif
    
    for (int i = 0; i < numInkwells_; ++i) {
        int compIdx = i % components.size();
        auto& comp = components[compIdx];
        std::uniform_int_distribution<int> distX(comp.bbox.x, 
                comp.bbox.x + comp.bbox.width - 1);
        std::uniform_int_distribution<int> distY(comp.bbox.y, 
                comp.bbox.y + comp.bbox.height - 1);

        sf::Vector2i pos(distX(rng), distY(rng));
        activeFronts_.push({pos, comp.label});
        filledPixels_.insert({pos.x, pos.y});
#ifdef DEBUG
        std::cout << "Selected starting point: (" << pos.x << ", " 
            << pos.y << ") for component " << comp.label << std::endl;
#endif
    }

}

int anic;
void InkAnimator::animateInkFlow() {
    const cv::Mat& labels = processor_.getLabelImage();
    const int dx[] = {-1, 1, 0, 0};
    const int dy[] = {0, 0, -1, 1};

    int stepsPerFrame = 10;  // speed of ink spreading

    for (int step = 0; step < stepsPerFrame && !activeFronts_.empty(); ++step) {
        InkFront current = activeFronts_.front();
        activeFronts_.pop();

        canvas_.setPixel({
                std::make_unsigned_t<int>(current.position.x),
                std::make_unsigned_t<int>(current.position.y)}, inkColor_);

        for (int d = 0; d < 4; ++d) {
            int nx = current.position.x + dx[d];
            int ny = current.position.y + dy[d];

            if (nx >= 0 && ny >= 0 && nx < labels.cols && ny < labels.rows) {
                if (labels.at<int>(ny, nx) == current.label && 
                    filledPixels_.insert({nx, ny}).second) {
                    activeFronts_.push({sf::Vector2i(nx, ny), current.label});
                }
            }
        }
#ifdef DEBUG
        std::cout << "Animating ink front at: (" 
            << current.position.x << ", " << current.position.y << ") " <<
            step << '/' << stepsPerFrame << " " << 
            anic++ << ':' << activeFronts_.size() << std::endl;
#endif
    }
        texture_.update(canvas_);
}

void InkAnimator::run() {
    while (window_.isOpen()) {
        while(const std::optional event = window_.pollEvent())
            if (event->is<sf::Event::Closed>() ||
                    (event->is<sf::Event::KeyPressed>() &&
                    (event->getIf<sf::Event::KeyPressed>()
                     -> code==sf::Keyboard::Key::Escape || 
                     event->getIf<sf::Event::KeyPressed>() 
                     -> code==sf::Keyboard::Key::Q )))
                window_.close();

        animateInkFlow();

        window_.clear(sf::Color::White);
        window_.draw(sprite_);
        window_.display();
    }
}

