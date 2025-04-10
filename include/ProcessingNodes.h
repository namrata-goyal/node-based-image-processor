#ifndef PROCESSINGNODES_H
#define PROCESSINGNODES_H

#include "Node.h"
#include <opencv2/opencv.hpp>

class BrightnessContrastNode : public Node {
public:
    BrightnessContrastNode();
    ~BrightnessContrastNode() override = default;
    
    void process() override;
    std::string name() const override { return "Brightness/Contrast"; }
    std::vector<Port> getPorts() const override;
    
    void setBrightness(int value);
    void setContrast(float value);
    
private:
    int m_brightness;
    float m_contrast;
};

class BlurNode : public Node {
public:
    BlurNode();
    ~BlurNode() override = default;
    
    void process() override;
    std::string name() const override { return "Blur"; }
    std::vector<Port> getPorts() const override;
    
    void setRadius(int radius);
    
private:
    int m_radius;
};

class ThresholdNode : public Node {
public:
    ThresholdNode();
    ~ThresholdNode() override = default;
    
    void process() override;
    std::string name() const override { return "Threshold"; }
    std::vector<Port> getPorts() const override;
    
    void setThreshold(int value);
    
private:
    int m_threshold;
};

class EdgeDetectionNode : public Node {
public:
    EdgeDetectionNode();
    ~EdgeDetectionNode() override = default;
    
    void process() override;
    std::string name() const override { return "Edge Detection"; }
    std::vector<Port> getPorts() const override;
    
    void setMethod(int method); // 0 = Sobel, 1 = Canny
    
private:
    int m_method;
};

class BlendNode : public Node {
public:
    BlendNode();
    ~BlendNode() override = default;
    
    void process() override;
    std::string name() const override { return "Blend"; }
    std::vector<Port> getPorts() const override;
    
    void setBlendMode(int mode);
    void setOpacity(float opacity);
    
private:
    int m_blendMode;
    float m_opacity;
};

#endif // PROCESSINGNODES_H
// Add these class declarations to ProcessingNodes.h

class ColorChannelSplitterNode : public Node {
    public:
        ColorChannelSplitterNode();
        ~ColorChannelSplitterNode() override = default;
        
        void process() override;
        std::string name() const override { return "Channel Splitter"; }
        std::vector<Port> getPorts() const override;
        
        void setOutputGrayscale(bool grayscale);
        
    private:
        bool m_outputGrayscale;
    };
    
    class NoiseGenerationNode : public Node {
    public:
        enum NoiseType { Perlin, Simplex, Worley };
        
        NoiseGenerationNode();
        ~NoiseGenerationNode() override = default;
        
        void process() override;
        std::string name() const override { return "Noise Generator"; }
        std::vector<Port> getPorts() const override;
        
        void setNoiseType(NoiseType type);
        void setScale(float scale);
        void setOctaves(int octaves);
        void setPersistence(float persistence);
        void setUseAsDisplacement(bool useAsDisplacement);
        
    private:
        NoiseType m_type;
        float m_scale;
        int m_octaves;
        float m_persistence;
        bool m_useAsDisplacement;
        
        float perlinNoise(float x, float y, float z = 0.0f);
    };
    
    class ConvolutionFilterNode : public Node {
    public:
        ConvolutionFilterNode();
        ~ConvolutionFilterNode() override = default;
        
        void process() override;
        std::string name() const override { return "Convolution Filter"; }
        std::vector<Port> getPorts() const override;
        
        void setKernelSize(int size);
        void setKernelValue(int row, int col, float value);
        void setPreset(int preset);
        
    private:
        int m_kernelSize;
        std::vector<std::vector<float>> m_kernel;
        
        void applyKernel(const cv::Mat& input, cv::Mat& output);
    };