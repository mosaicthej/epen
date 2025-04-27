#include "InkAnimator.hpp"
#include "ImageProcessor.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <iostream>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <optional>
#include <ostream>
#include <random>

InkAnimator::InkAnimator(const ImageProcessor& proc, int nInkWells)
    : processor_(proc), numInkwells_(nInkWells),
    window_(sf::VideoMode(
                {U(proc.getBinaryImage().cols),
U(proc.getBinaryImage().rows)}, 32), "Ink Animation"),
   sprite_(texture_)   /* placeholder */
{
    unsigned int w { U(proc.getBinaryImage().cols) }, 
                 h { U(proc.getBinaryImage().rows) };

    /* cv::cvtColor(proc.getBinaryImage(), canvasMat_, cv::COLOR_GRAY2RGBA); */
    canvasMat_ = cv::Mat(h, w, CV_8UC4, cv::Scalar(255,255,255,255));
    /* canvasMat_ being rgba of all white */
    texture_.resize({w, h});
    /* sprite_.setTexture(texture_, true); */

    initializeInkWells();

    auto& testred = canvasMat_.at<cv::Vec4b>({15u, 15u});
    testred[0] = 0; testred[1] = 255; testred[2] = 0; testred[3] = 255;
    /* BGRA */

    if(!texture_.resize({w,h}))
        std::cerr << "Unable to resize texture_!" << std::endl;
    texture_.update(canvasMat_.ptr()); /* update the texture */

    sprite_.setTexture(texture_, true);
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

/*        canvasMat_.setPixel({U(current.position.x),U(current.position.y)}, 
                inkColor_); */
        auto& px = canvasMat_.at<cv::Vec4b>(current.position.y, current.position.x);
        px[0] = 0;   // B
        px[1] = 0;   // G
        px[2] = 0;   // R
        px[3] = 255; // A


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
    texture_.update(canvasMat_.ptr());
}

void InkAnimator::run() {
    while (window_.isOpen()) {
        while(const std::optional event = window_.pollEvent())
            CLOSE_SF_WIND_ON_CUE(window_);

        animateInkFlow();
        window_.clear(sf::Color::White);
        window_.draw(sprite_);
        window_.display();
    }
}

