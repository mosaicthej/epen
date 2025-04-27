#include "ImageProcessor.hpp"
#include <iostream>

ImageProcessor::ImageProcessor(const std::string& imagePath)
    : imagePath_(imagePath) {}

bool ImageProcessor::loadAndProcess() {
    cv::Mat img = cv::imread(imagePath_);
    if (img.empty()) {
        std::cerr << "Failed to load image: " << imagePath_ << "\n";
        return false;
    }

    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    cv::threshold(gray, binaryImage_, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

    cv::Mat stats, centroids;
    int nLabels = cv::connectedComponentsWithStats(binaryImage_, labels_, stats, centroids);

    components_.clear();
    for (int label = 1; label < nLabels; ++label) {
        components_.push_back({
            label,
            cv::Rect(
                stats.at<int>(label, cv::CC_STAT_LEFT),
                stats.at<int>(label, cv::CC_STAT_TOP),
                stats.at<int>(label, cv::CC_STAT_WIDTH),
                stats.at<int>(label, cv::CC_STAT_HEIGHT)
            ),
            stats.at<int>(label, cv::CC_STAT_AREA)
        });
    }
    return true;
}

const cv::Mat& ImageProcessor::getBinaryImage() const { return binaryImage_; }
const cv::Mat& ImageProcessor::getLabelImage() const { return labels_; }
const std::vector<Component>& ImageProcessor::getComponents() const { return components_; }

