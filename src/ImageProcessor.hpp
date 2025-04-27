#ifndef __IMAGE_PROCESSOR_H__
#define __IMAGE_PROCESSOR_H__

#include <opencv4/opencv2/opencv.hpp>
#include <vector>

struct Component {
    int label;
    cv::Rect bbox;
    int area;
};

class ImageProcessor {
public:
    explicit ImageProcessor(const std::string& imagePath);

    bool loadAndProcess();
    
    const cv::Mat& getBinaryImage() const;
    const cv::Mat& getLabelImage() const;
    const std::vector<Component>& getComponents() const;

private:
    std::string imagePath_;
    cv::Mat binaryImage_;
    cv::Mat labels_;
    std::vector<Component> components_;
};

#endif
