#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <iostream>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/core/mat.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <type_traits>
#include <vector>
struct Component {
/* struct with cc info */
    int label;
    int area;
    cv::Rect bbox;
};

int main(int argc, char** argv){
    
    /* 1. 
     * load image */
    std::string imgpath = "assets/sketch-small.png";
    if (argc > 1)
        imgpath = argv[1];
    cv::Mat orig = cv::imread(imgpath);
    /*
    cv::resize(orig, orig, {}, 0.5, 0.5, cv::INTER_AREA);
    */
    if (orig.empty()) {
        std::cerr << "Error: could not load image at " << imgpath << std::endl;
        return 1;
    }

    /* 2.
     * convert to grayscale and apply binary threshold (invert + otsu)
     */
    cv::Mat gray;
    cv::cvtColor(orig, gray, cv::COLOR_BGR2GRAY);
    cv::Mat binary;
    cv::threshold(gray, binary, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

    /* `binary` being a single channel image
     */

    /* 3.
     * find connected components and stats
     */
    cv::Mat labels, stats, centroids;
    int nComponents = cv::connectedComponentsWithStats(
            binary, labels, stats, centroids, 8, CV_32S);

    /* 4.
     * store components (except 0 -> background)
     */
    std::vector<Component> components;
    components.reserve(nComponents - 1);
    for (int label=1; label < nComponents; ++label){
        Component comp;
        comp.label = label;

        /* rounding box of the cc */
        int left    = stats.at<int>(label, cv::CC_STAT_LEFT);
        int top     = stats.at<int>(label, cv::CC_STAT_TOP);
        int width   = stats.at<int>(label, cv::CC_STAT_WIDTH);
        int height  = stats.at<int>(label, cv::CC_STAT_HEIGHT);
        comp.area   = stats.at<int>(label, cv::CC_STAT_AREA);
        comp.bbox   = cv::Rect(left, top, width, height);
        components.push_back(comp);
    }
    std::cout << "Found " << components.size() << " connected components " 
        << "(excluding background)\n";

    /* 5.
     * setup SFML window and display binary image
     * need to convert that bin to RGBA pixel array */
    cv::Mat rgba;
    cv::cvtColor(binary, rgba, cv::COLOR_GRAY2RGBA);
    /* create SFML texture and update with pixel data
     */
    sf::Texture texture;
    if(!texture.resize({
                std::make_unsigned_t<int>(rgba.cols),
                std::make_unsigned_t<int>(rgba.rows)}))
        std::cerr << "Unable to resize to " << rgba.cols << ", " << rgba.rows
            << '\n';
    texture.update(rgba.ptr()); 

    /* use sprite to draw */
    sf::Sprite sprite(texture);
    /* SFML window to display */
    sf::RenderWindow window(sf::VideoMode({std::make_unsigned_t<int>(rgba.cols), 
                std::make_unsigned_t<int>(rgba.rows)}), "Binarized Image");
    while (window.isOpen()) {
        while(const std::optional event = window.pollEvent())
            if (event->is<sf::Event::Closed>() ||
                (event->is<sf::Event::KeyPressed>() &&
                 event->getIf<sf::Event::KeyPressed>()->code == 
                 sf::Keyboard::Key::Escape))
                window.close();
        
        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}

