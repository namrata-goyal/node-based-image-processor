#ifndef IMAGENODE_H
#define IMAGENODE_H

#include "Node.h"
#include <opencv2/opencv.hpp>
#include <string>

class ImageInputNode : public Node {
public:
    ImageInputNode();
    ~ImageInputNode() override = default;
    
    void process() override;
    std::string name() const override { return "Image Input"; }
    std::vector<Port> getPorts() const override;
    
    void setImagePath(const std::string& path);
    cv::Mat getImage() const;
    
private:
    std::string m_imagePath;
    cv::Mat m_image;
};

class ImageOutputNode : public Node {
public:
    ImageOutputNode();
    ~ImageOutputNode() override = default;
    
    void process() override;
    std::string name() const override { return "Image Output"; }
    std::vector<Port> getPorts() const override;
    
    void setOutputPath(const std::string& path);
    cv::Mat getOutputImage() const;
    
private:
    std::string m_outputPath;
    cv::Mat m_outputImage;
};

#endif // IMAGENODE_H