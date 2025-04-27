#include "ImageProcessor.hpp"
#include "InkAnimator.hpp"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    std::string imgpath {"assets/sketch-small.png"};
    if (argc > 1)
        imgpath = argv[1];
    ImageProcessor processor(imgpath);
    if (!processor.loadAndProcess()) {
        std::cerr << "Image processing failed.\n";
        return 1;
    }

    int numInkwells = 10;  // Customize number of ink wells as needed
    InkAnimator animator(processor, numInkwells);

    animator.run();

    return 0;
}

