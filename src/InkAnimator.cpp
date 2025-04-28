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
   sprite_(texture_),   /* placeholder */
   rng_(std::random_device{}())
{
    unsigned int w { U(proc.getBinaryImage().cols) }, 
                 h { U(proc.getBinaryImage().rows) };

    /* cv::cvtColor(proc.getBinaryImage(), canvasMat_, cv::COLOR_GRAY2RGBA); */
    canvasMat_ = cv::Mat(h, w, CV_8UC4, cv::Scalar(255,255,255,255));
    /* canvasMat_ being rgba of all white */

    if(!texture_.resize({w,h}))
        std::cerr << "Unable to resize texture_!" << std::endl;
    texture_.update(canvasMat_.ptr()); /* update the texture */

    sprite_.setTexture(texture_, true);

    
    /* build components area map */
    for (auto& comp : proc.getComponents()) {
        componentAreas_[comp.label] = comp.area;
        filledCount_[comp.label]  = 0;
    }


    initializeInkWells();
}

void InkAnimator::initializeInkWells() {
    auto& components = processor_.getComponents();
    std::uniform_int_distribution<> pickComp(0, components.size()-1);
    std::mt19937 rng(std::random_device{}());
#ifdef DEBUG
    std::cout << "Initializing inkwells..." << std::endl;
#endif
    
    for (int i = 0; i < numInkwells_; ++i) {
        auto& comp = components[i % components.size()];

        std::uniform_int_distribution<> dx(comp.bbox.x, 
                comp.bbox.x+comp.bbox.width - 1);
        std::uniform_int_distribution<> dy(comp.bbox.y,
                comp.bbox.y+comp.bbox.height - 1);

        sf::Vector2i pos;
        do {
            pos.x = dx(rng_); pos.y = dy(rng_); 
        } while (processor_.getLabelImage().at<int>(pos.y, pos.x) != comp.label);

        activeFronts_.push({pos, comp.label});
        filledPixels_.insert({pos.x, pos.y});
#ifdef DEBUG
                std::cout << "Seeded well #" << i
                  << " at (" << pos.x << "," << pos.y
                  << ") label=" << comp.label
                  << " filled=" << filledCount_[comp.label]
                  << "/" << componentAreas_[comp.label] << "\n";
#endif
    }

}

/* spawn new inkwell on random incomplete cc */
void InkAnimator::spawnInkWell() {
#ifdef DEBUG
    std::cout << "spawning new..." << std::endl;
#endif
    /* gather candidates */
    std::vector<int> cand;
    cand.reserve(componentAreas_.size());
    for (auto& [lbl, area] : componentAreas_)
        if (!completedComponents_.count(lbl))
            cand.push_back(lbl);

    if (cand.empty()) return; /* all done */

    std::uniform_int_distribution<> pick(0, cand.size()-1);
    int label = cand[pick(rng_)];

    /* get the bbox */
    auto& comps = processor_.getComponents();
    auto it = std::find_if(comps.begin(), comps.end(),
            [&](auto& c){ return c.label == label; });
    auto& comp = *it;

    std::uniform_int_distribution<>
        dx(comp.bbox.x, comp.bbox.x + comp.bbox.width -1),
        dy(comp.bbox.y, comp.bbox.y + comp.bbox.height - 1);

    sf::Vector2i pos;
    do {
        pos.x = dx(rng_); pos.y = dy(rng_);
    } while (processor_.getLabelImage().at<int>(pos.y, pos.x) != label);

    activeFronts_.push({pos, label});
    filledPixels_.insert({pos.x, pos.y});
    // filledCount_[label]++;

#ifdef DEBUG
    std::cout << "Respawned well on label=" << label
              << " at (" << pos.x << "," << pos.y << ")"
              << " now filled=" << filledCount_[label]
              << "/" << componentAreas_[label] << "\n";
#endif
}

void InkAnimator::animateInkFlow() {
    const cv::Mat& labels = processor_.getLabelImage();
    static constexpr int dx[] = {-1, 1, 0, 0}, dy[] = {0, 0, -1, 1};

    int stepsPerFrame = 10;  // speed of ink spreading

    for (int step = 0; step < stepsPerFrame && !activeFronts_.empty(); ++step) {
        auto cur = activeFronts_.front();
        activeFronts_.pop();

        /* paint */
        auto& px = canvasMat_.at<cv::Vec4b>(cur.position.y, cur.position.x);
        px = {0,0,0,255};

        /* record fill count and check completion */
        int cnt = ++filledCount_[cur.label];
#ifdef DEBUG
        std::cout << "filling cc # " << cur.label << ' ' 
            << filledCount_[cur.label] << '/' << componentAreas_[cur.label]
            << std::endl;
#endif
        if (activeFronts_.empty()) spawnInkWell();
        if (cnt == componentAreas_[cur.label]) {
#ifdef DEBUG
            std::cout << "completed cc #" << cur.label << '\n';
#endif
            completedComponents_.insert(cur.label);
            spawnInkWell();
        }

        /* 


        InkFront current = activeFronts_.front();
        activeFronts_.pop();

        canvasMat_.setPixel({U(current.position.x),U(current.position.y)}, 
                inkColor_); */

        /* spread to 4 neighbours */
        for (int d = 0; d < 4; ++d) {
            int nx = cur.position.x + dx[d];
            int ny = cur.position.y + dy[d];

            if (nx>=0 && ny>=0 && nx<labels.cols && ny<labels.rows) 
                if (labels.at<int>(ny, nx) == cur.label &&
                        filledPixels_.insert({nx, ny}).second){
                    activeFronts_.push({{nx, ny}, cur.label});
                }
            }
        }
    texture_.update(canvasMat_.ptr());
}

void InkAnimator::run() {
    while (window_.isOpen()) {
        CLOSE_SF_WIND_ON_CUE(window_);

        window_.clear(sf::Color::White);
        animateInkFlow();
        window_.draw(sprite_);
        window_.display();
    }
}

