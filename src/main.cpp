#include <type_traits>
#include <vector>
#include <opencv4/opencv2/imgcodecs.hpp>   // cv::imread
#include <opencv4/opencv2/imgproc.hpp>     // cv::cvtColor, cv::threshold, cv::connectedComponentsWithStats
#include <SFML/Graphics.hpp>       // SFML Graphics (includes Window and System modules)

#include <iostream>

// Structure to hold connected component info
struct Component {
    int label;
    cv::Rect bbox;
    int area;
    // (Centroid or other properties can be added here in the future)
};

int main(int argc, char** argv) {
    // 1. Load the input image
    std::string imagePath = "assets/scan.png";
    if (argc > 1) {
        imagePath = argv[1];
    }
    cv::Mat orig = cv::imread(imagePath);

    cv::resize(orig, orig, {}, 0.5, 0.5, cv::INTER_AREA);
    if (orig.empty()) {
        std::cerr << "Error: Could not load image at " << imagePath << std::endl;
        return 1;
    }

    // 2. Convert to grayscale and apply binary threshold (invert + Otsu)
    cv::Mat gray;
    cv::cvtColor(orig, gray, cv::COLOR_BGR2GRAY);
    cv::Mat binary;
    cv::threshold(gray, binary, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
    // After this, 'binary' is a single-channel image: pixels are 255 for original black lines, 0 for background&#8203;:contentReference[oaicite:13]{index=13}.

    // 3. Find connected components and their stats
    cv::Mat labels, stats, centroids;
    int nComponents = cv::connectedComponentsWithStats(
        binary, labels, stats, centroids, 8, CV_32S);
    // nComponents includes the background as label 0&#8203;:contentReference[oaicite:14]{index=14}.

    // 4. Store components (skip label 0 which is background)
    std::vector<Component> components;
    components.reserve(nComponents - 1);
    for (int label = 1; label < nComponents; ++label) {
        Component comp;
        comp.label = label;
        // Retrieve bounding box and area from stats matrix (columns 0-4)&#8203;:contentReference[oaicite:15]{index=15}:
        int left   = stats.at<int>(label, cv::CC_STAT_LEFT);
        int top    = stats.at<int>(label, cv::CC_STAT_TOP);
        int width  = stats.at<int>(label, cv::CC_STAT_WIDTH);
        int height = stats.at<int>(label, cv::CC_STAT_HEIGHT);
        comp.area  = stats.at<int>(label, cv::CC_STAT_AREA);
        comp.bbox  = cv::Rect(left, top, width, height);
        components.push_back(comp);
    }
    std::cout << "Found " << components.size() << " connected components (excluding background).\n";

    // 5. Set up SFML window and display the binary image
    // Convert binary Mat to RGBA pixel array for SFML
    cv::Mat rgba;
    cv::cvtColor(binary, rgba, cv::COLOR_GRAY2RGBA);  // result has 4 channels (R,G,B,A)
    // Create SFML texture and update it with pixel data
    sf::Texture texture;
    if(!texture.resize({std::make_unsigned_t<int>(rgba.cols), 
            std::make_unsigned_t<int>(rgba.rows)}))
        std::cerr << "Unable to resize to" << rgba.cols << ", " << rgba.rows <<'\n';
    texture.update(rgba.ptr());  // update texture with RGBA pixels&#8203;:contentReference[oaicite:16]{index=16}

    // Create a sprite to draw the texture
    sf::Sprite sprite(texture);
    // Open an SFML window to display the image
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

