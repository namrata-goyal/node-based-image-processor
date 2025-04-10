#include "ProcessingNodes.h"

BrightnessContrastNode::BrightnessContrastNode() : m_brightness(0), m_contrast(1.0f) {
    m_outputData.push_back(std::make_shared<cv::Mat>());
}

void BrightnessContrastNode::process() {
    auto inputData = getInputData(0);
    if (inputData) {
        auto inputImage = *std::static_pointer_cast<cv::Mat>(inputData);
        if (!inputImage.empty()) {
            cv::Mat output;
            inputImage.convertTo(output, -1, m_contrast, m_brightness);
            *std::static_pointer_cast<cv::Mat>(m_outputData[0]) = output;
            emit dataUpdated();
        }
    }
}

std::vector<Port> BrightnessContrastNode::getPorts() const {
    return {
        {0, "Input", PortType::Input, DataType::Image},
        {1, "Output", PortType::Output, DataType::Image}
    };
}

void BrightnessContrastNode::setBrightness(int value) {
    m_brightness = value;
    process();
}

void BrightnessContrastNode::setContrast(float value) {
    m_contrast = value;
    process();
}

BlurNode::BlurNode() : m_radius(5) {
    m_outputData.push_back(std::make_shared<cv::Mat>());
}

void BlurNode::process() {
    auto inputData = getInputData(0);
    if (inputData) {
        auto inputImage = *std::static_pointer_cast<cv::Mat>(inputData);
        if (!inputImage.empty()) {
            cv::Mat output;
            cv::GaussianBlur(inputImage, output, cv::Size(m_radius, m_radius), 0);
            *std::static_pointer_cast<cv::Mat>(m_outputData[0]) = output;
            emit dataUpdated();
        }
    }
}

std::vector<Port> BlurNode::getPorts() const {
    return {
        {0, "Input", PortType::Input, DataType::Image},
        {1, "Output", PortType::Output, DataType::Image}
    };
}

void BlurNode::setRadius(int radius) {
    m_radius = radius;
    process();
}

ThresholdNode::ThresholdNode() : m_threshold(127) {
    m_outputData.push_back(std::make_shared<cv::Mat>());
}

void ThresholdNode::process() {
    auto inputData = getInputData(0);
    if (inputData) {
        auto inputImage = *std::static_pointer_cast<cv::Mat>(inputData);
        if (!inputImage.empty()) {
            cv::Mat gray, output;
            if (inputImage.channels() > 1) {
                cv::cvtColor(inputImage, gray, cv::COLOR_BGR2GRAY);
            } else {
                gray = inputImage.clone();
            }
            cv::threshold(gray, output, m_threshold, 255, cv::THRESH_BINARY);
            *std::static_pointer_cast<cv::Mat>(m_outputData[0]) = output;
            emit dataUpdated();
        }
    }
}

std::vector<Port> ThresholdNode::getPorts() const {
    return {
        {0, "Input", PortType::Input, DataType::Image},
        {1, "Output", PortType::Output, DataType::Image}
    };
}

void ThresholdNode::setThreshold(int value) {
    m_threshold = value;
    process();
}

EdgeDetectionNode::EdgeDetectionNode() : m_method(0) {
    m_outputData.push_back(std::make_shared<cv::Mat>());
}

void EdgeDetectionNode::process() {
    auto inputData = getInputData(0);
    if (inputData) {
        auto inputImage = *std::static_pointer_cast<cv::Mat>(inputData);
        if (!inputImage.empty()) {
            cv::Mat gray, output;
            if (inputImage.channels() > 1) {
                cv::cvtColor(inputImage, gray, cv::COLOR_BGR2GRAY);
            } else {
                gray = inputImage.clone();
            }
            
            if (m_method == 0) { // Sobel
                cv::Mat grad_x, grad_y;
                cv::Sobel(gray, grad_x, CV_16S, 1, 0);
                cv::Sobel(gray, grad_y, CV_16S, 0, 1);
                
                cv::Mat abs_grad_x, abs_grad_y;
                cv::convertScaleAbs(grad_x, abs_grad_x);
                cv::convertScaleAbs(grad_y, abs_grad_y);
                
                cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, output);
            } else { // Canny
                cv::Canny(gray, output, 50, 150);
            }
            
            *std::static_pointer_cast<cv::Mat>(m_outputData[0]) = output;
            emit dataUpdated();
        }
    }
}

std::vector<Port> EdgeDetectionNode::getPorts() const {
    return {
        {0, "Input", PortType::Input, DataType::Image},
        {1, "Output", PortType::Output, DataType::Image}
    };
}

void EdgeDetectionNode::setMethod(int method) {
    m_method = method;
    process();
}

BlendNode::BlendNode() : m_blendMode(0), m_opacity(0.5f) {
    m_outputData.push_back(std::make_shared<cv::Mat>());
}

void BlendNode::process() {
    auto input1 = getInputData(0);
    auto input2 = getInputData(1);
    
    if (input1 && input2) {
        auto image1 = *std::static_pointer_cast<cv::Mat>(input1);
        auto image2 = *std::static_pointer_cast<cv::Mat>(input2);
        
        if (!image1.empty() && !image2.empty()) {
            cv::Mat output;
            
            // Resize images to match if needed
            if (image1.size() != image2.size()) {
                cv::resize(image2, image2, image1.size());
            }
            
            switch (m_blendMode) {
                case 0: // Normal
                    cv::addWeighted(image1, 1.0 - m_opacity, image2, m_opacity, 0, output);
                    break;
                case 1: // Multiply
                    cv::multiply(image1, image2, output, 1.0/255.0);
                    break;
                case 2: // Screen
                    cv::Mat temp1, temp2;
                    cv::subtract(cv::Scalar::all(255), image1, temp1);
                    cv::subtract(cv::Scalar::all(255), image2, temp2);
                    cv::multiply(temp1, temp2, output, 1.0/255.0);
                    cv::subtract(cv::Scalar::all(255), output, output);
                    break;
                case 3: // Overlay
                    // Implementation of overlay blend mode
                    output = cv::Mat::zeros(image1.size(), image1.type());
                    for (int y = 0; y < image1.rows; y++) {
                        for (int x = 0; x < image1.cols; x++) {
                            for (int c = 0; c < image1.channels(); c++) {
                                double a = image1.at<cv::Vec3b>(y, x)[c] / 255.0;
                                double b = image2.at<cv::Vec3b>(y, x)[c] / 255.0;
                                double r = (a < 0.5) ? (2 * a * b) : (1 - 2 * (1 - a) * (1 - b));
                                output.at<cv::Vec3b>(y, x)[c] = cv::saturate_cast<uchar>(
                                    (1 - m_opacity) * a * 255 + m_opacity * r * 255);
                            }
                        }
                    }
                    break;
                case 4: // Difference
                    cv::absdiff(image1, image2, output);
                    break;
                default:
                    output = image1.clone();
            }
            
            *std::static_pointer_cast<cv::Mat>(m_outputData[0]) = output;
            emit dataUpdated();
        }
    }
}

std::vector<Port> BlendNode::getPorts() const {
    return {
        {0, "Input 1", PortType::Input, DataType::Image},
        {1, "Input 2", PortType::Input, DataType::Image},
        {2, "Output", PortType::Output, DataType::Image}
    };
}

void BlendNode::setBlendMode(int mode) {
    m_blendMode = mode;
    process();
}

void BlendNode::setOpacity(float opacity) {
    m_opacity = opacity;
    process();
}
// Add these implementations to ProcessingNodes.cpp

ColorChannelSplitterNode::ColorChannelSplitterNode() : m_outputGrayscale(true) {
    // Output ports for each channel (R, G, B, A)
    m_outputData.push_back(std::make_shared<cv::Mat>());
    m_outputData.push_back(std::make_shared<cv::Mat>());
    m_outputData.push_back(std::make_shared<cv::Mat>());
    m_outputData.push_back(std::make_shared<cv::Mat>());
}

void ColorChannelSplitterNode::process() {
    auto inputData = getInputData(0);
    if (inputData) {
        auto inputImage = *std::static_pointer_cast<cv::Mat>(inputData);
        if (!inputImage.empty()) {
            std::vector<cv::Mat> channels;
            cv::split(inputImage, channels);
            
            // If input is grayscale, just duplicate it for all channels
            if (channels.size() == 1) {
                channels.push_back(channels[0].clone());
                channels.push_back(channels[0].clone());
            }
            
            // Ensure we have at least 3 channels
            while (channels.size() < 3) {
                channels.push_back(cv::Mat::zeros(inputImage.size(), CV_8UC1));
            }
            
            // Alpha channel if available
            cv::Mat alpha;
            if (channels.size() > 3) {
                alpha = channels[3];
            } else {
                alpha = cv::Mat(inputImage.size(), CV_8UC1, cv::Scalar(255));
            }
            
            if (m_outputGrayscale) {
                *std::static_pointer_cast<cv::Mat>(m_outputData[0]) = channels[2]; // R
                *std::static_pointer_cast<cv::Mat>(m_outputData[1]) = channels[1]; // G
                *std::static_pointer_cast<cv::Mat>(m_outputData[2]) = channels[0]; // B
                *std::static_pointer_cast<cv::Mat>(m_outputData[3]) = alpha;       // A
            } else {
                cv::Mat red, green, blue;
                cv::Mat zeros = cv::Mat::zeros(inputImage.size(), CV_8UC1);
                
                // Red channel
                std::vector<cv::Mat> redChannels = {zeros, zeros, channels[2]};
                cv::merge(redChannels, red);
                
                // Green channel
                std::vector<cv::Mat> greenChannels = {zeros, channels[1], zeros};
                cv::merge(greenChannels, green);
                
                // Blue channel
                std::vector<cv::Mat> blueChannels = {channels[0], zeros, zeros};
                cv::merge(blueChannels, blue);
                
                *std::static_pointer_cast<cv::Mat>(m_outputData[0]) = red;
                *std::static_pointer_cast<cv::Mat>(m_outputData[1]) = green;
                *std::static_pointer_cast<cv::Mat>(m_outputData[2]) = blue;
                *std::static_pointer_cast<cv::Mat>(m_outputData[3]) = alpha;
            }
            
            emit dataUpdated();
        }
    }
}

std::vector<Port> ColorChannelSplitterNode::getPorts() const {
    return {
        {0, "Input", PortType::Input, DataType::Image},
        {1, "Red", PortType::Output, DataType::Image},
        {2, "Green", PortType::Output, DataType::Image},
        {3, "Blue", PortType::Output, DataType::Image},
        {4, "Alpha", PortType::Output, DataType::Image}
    };
}

void ColorChannelSplitterNode::setOutputGrayscale(bool grayscale) {
    m_outputGrayscale = grayscale;
    process();
}

NoiseGenerationNode::NoiseGenerationNode() : 
    m_type(Perlin), m_scale(0.1f), m_octaves(4), 
    m_persistence(0.5f), m_useAsDisplacement(false) {
    m_outputData.push_back(std::make_shared<cv::Mat>());
}

// Simple Perlin noise implementation (simplified for this example)
float NoiseGenerationNode::perlinNoise(float x, float y, float z) {
    // This is a very basic implementation - a real implementation would be more complex
    // and might use a proper noise library like FastNoise or similar
    float n = x * 0.1f + y * 0.1f + z * 0.1f;
    return std::sin(n * 10.0f) * 0.5f + 0.5f;
}

void NoiseGenerationNode::process() {
    // Create a noise texture
    const int width = 512;
    const int height = 512;
    cv::Mat noise(height, width, CV_32FC1);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float value = 0.0f;
            float amplitude = 1.0f;
            float frequency = 1.0f;
            
            for (int o = 0; o < m_octaves; ++o) {
                float nx = x * frequency * m_scale;
                float ny = y * frequency * m_scale;
                
                float noiseValue = 0.0f;
                switch (m_type) {
                    case Perlin:
                        noiseValue = perlinNoise(nx, ny);
                        break;
                    case Simplex:
                        // Simplex would be similar but with different coordinates
                        noiseValue = perlinNoise(nx * 0.5f, ny * 0.5f);
                        break;
                    case Worley:
                        // Worley noise would need a different implementation
                        noiseValue = std::fmod(nx * 0.1f + ny * 0.1f, 1.0f);
                        break;
                }
                
                value += noiseValue * amplitude;
                amplitude *= m_persistence;
                frequency *= 2.0f;
            }
            
            noise.at<float>(y, x) = value;
        }
    }
    
    // Normalize to 0-1 range
    cv::normalize(noise, noise, 0.0f, 1.0f, cv::NORM_MINMAX);
    
    if (m_useAsDisplacement) {
        // Convert to displacement map (3-channel)
        cv::Mat displacement;
        cv::cvtColor(noise, displacement, cv::COLOR_GRAY2BGR);
        *std::static_pointer_cast<cv::Mat>(m_outputData[0]) = displacement;
    } else {
        // Convert to grayscale image
        cv::Mat output;
        noise.convertTo(output, CV_8UC1, 255.0f);
        *std::static_pointer_cast<cv::Mat>(m_outputData[0]) = output;
    }
    
    emit dataUpdated();
}

std::vector<Port> NoiseGenerationNode::getPorts() const {
    return {
        {0, "Output", PortType::Output, DataType::Image}
    };
}

void NoiseGenerationNode::setNoiseType(NoiseType type) {
    m_type = type;
    process();
}

void NoiseGenerationNode::setScale(float scale) {
    m_scale = scale;
    process();
}

void NoiseGenerationNode::setOctaves(int octaves) {
    m_octaves = octaves;
    process();
}

void NoiseGenerationNode::setPersistence(float persistence) {
    m_persistence = persistence;
    process();
}

void NoiseGenerationNode::setUseAsDisplacement(bool useAsDisplacement) {
    m_useAsDisplacement = useAsDisplacement;
    process();
}

ConvolutionFilterNode::ConvolutionFilterNode() : m_kernelSize(3) {
    // Initialize with identity kernel
    m_kernel.resize(m_kernelSize, std::vector<float>(m_kernelSize, 0.0f));
    m_kernel[m_kernelSize/2][m_kernelSize/2] = 1.0f;
    
    m_outputData.push_back(std::make_shared<cv::Mat>());
}

void ConvolutionFilterNode::applyKernel(const cv::Mat& input, cv::Mat& output) {
    cv::Mat kernel(m_kernelSize, m_kernelSize, CV_32F);
    for (int i = 0; i < m_kernelSize; ++i) {
        for (int j = 0; j < m_kernelSize; ++j) {
            kernel.at<float>(i, j) = m_kernel[i][j];
        }
    }
    
    cv::filter2D(input, output, -1, kernel, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
}

void ConvolutionFilterNode::process() {
    auto inputData = getInputData(0);
    if (inputData) {
        auto inputImage = *std::static_pointer_cast<cv::Mat>(inputData);
        if (!inputImage.empty()) {
            cv::Mat output;
            applyKernel(inputImage, output);
            *std::static_pointer_cast<cv::Mat>(m_outputData[0]) = output;
            emit dataUpdated();
        }
    }
}

std::vector<Port> ConvolutionFilterNode::getPorts() const {
    return {
        {0, "Input", PortType::Input, DataType::Image},
        {1, "Output", PortType::Output, DataType::Image}
    };
}

void ConvolutionFilterNode::setKernelSize(int size) {
    if (size % 2 == 1 && size >= 3 && size <= 5) { // Only odd sizes 3x3 or 5x5
        m_kernelSize = size;
        m_kernel.resize(m_kernelSize, std::vector<float>(m_kernelSize, 0.0f));
        m_kernel[m_kernelSize/2][m_kernelSize/2] = 1.0f; // Reset to identity
        process();
    }
}

void ConvolutionFilterNode::setKernelValue(int row, int col, float value) {
    if (row >= 0 && row < m_kernelSize && col >= 0 && col < m_kernelSize) {
        m_kernel[row][col] = value;
        process();
    }
}

void ConvolutionFilterNode::setPreset(int preset) {
    // Reset to identity first
    for (auto& row : m_kernel) {
        std::fill(row.begin(), row.end(), 0.0f);
    }
    m_kernel[m_kernelSize/2][m_kernelSize/2] = 1.0f;
    
    switch (preset) {
        case 1: // Sharpen
            if (m_kernelSize == 3) {
                m_kernel = {
                    { 0, -1,  0},
                    {-1,  5, -1},
                    { 0, -1,  0}
                };
            } else { // 5x5
                m_kernel = {
                    { 0,  0, -1,  0,  0},
                    { 0, -1, -1, -1,  0},
                    {-1, -1, 25, -1, -1},
                    { 0, -1, -1, -1,  0},
                    { 0,  0, -1,  0,  0}
                };
            }
            break;
        case 2: // Edge detection
            if (m_kernelSize == 3) {
                m_kernel = {
                    {-1, -1, -1},
                    {-1,  8, -1},
                    {-1, -1, -1}
                };
            } else { // 5x5
                m_kernel = {
                    {-1, -1, -1, -1, -1},
                    {-1, -1, -1, -1, -1},
                    {-1, -1, 24, -1, -1},
                    {-1, -1, -1, -1, -1},
                    {-1, -1, -1, -1, -1}
                };
            }
            break;
        case 3: // Emboss
            if (m_kernelSize == 3) {
                m_kernel = {
                    {-2, -1,  0},
                    {-1,  1,  1},
                    { 0,  1,  2}
                };
            } else { // 5x5
                m_kernel = {
                    {-2, -2, -1,  0,  0},
                    {-2, -1,  0,  1,  0},
                    {-1,  0,  1,  2,  1},
                    { 0,  1,  2,  1,  0},
                    { 0,  0,  1,  0,  0}
                };
            }
            break;
        case 4: // Box blur
            float val = 1.0f / (m_kernelSize * m_kernelSize);
            for (auto& row : m_kernel) {
                std::fill(row.begin(), row.end(), val);
            }
            break;
    }
    
    process();
}