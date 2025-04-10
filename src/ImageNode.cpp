#include "ImageNode.h"
#include <QFileDialog>
#include <QMessageBox>

ImageInputNode::ImageInputNode() {
    m_outputData.push_back(std::make_shared<cv::Mat>());
}

void ImageInputNode::process() {
    if (!m_imagePath.empty()) {
        cv::Mat image = cv::imread(m_imagePath, cv::IMREAD_COLOR);
        if (!image.empty()) {
            m_image = image;
            auto output = std::static_pointer_cast<cv::Mat>(m_outputData[0]);
            *output = m_image.clone();
            emit dataUpdated();
        }
    }
}

std::vector<Port> ImageInputNode::getPorts() const {
    return {
        {0, "Output", PortType::Output, DataType::Image}
    };
}

void ImageInputNode::setImagePath(const std::string& path) {
    m_imagePath = path;
    process();
}

cv::Mat ImageInputNode::getImage() const {
    return m_image;
}

ImageOutputNode::ImageOutputNode() {
    // No output ports, only input
}

void ImageOutputNode::process() {
    auto inputData = getInputData(0);
    if (inputData) {
        auto inputImage = *std::static_pointer_cast<cv::Mat>(inputData);
        if (!inputImage.empty()) {
            m_outputImage = inputImage.clone();
            if (!m_outputPath.empty()) {
                cv::imwrite(m_outputPath, m_outputImage);
            }
        }
    }
}

std::vector<Port> ImageOutputNode::getPorts() const {
    return {
        {0, "Input", PortType::Input, DataType::Image}
    };
}

void ImageOutputNode::setOutputPath(const std::string& path) {
    m_outputPath = path;
    process();
}

cv::Mat ImageOutputNode::getOutputImage() const {
    return m_outputImage;
}