#ifndef TYPES_H
#define TYPES_H

#include <opencv2/opencv.hpp>
#include <memory>
#include <vector>
#include <map>

enum class PortType {
    Input,
    Output
};

enum class DataType {
    Image,
    Scalar,
    Boolean,
    Integer
};

struct Port {
    int id;
    std::string name;
    PortType type;
    DataType dataType;
    std::shared_ptr<void> data;
};

using NodeID = int;
using Connection = std::pair<NodeID, int>; // node ID and port index

#endif // TYPES_H