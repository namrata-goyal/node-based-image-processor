cmake_minimum_required(VERSION 3.5)
project(NodeBasedImageProcessor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED)
find_package(OpenCV REQUIRED)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.h")

add_executable(NodeBasedImageProcessor ${SOURCES} ${HEADERS})

target_include_directories(NodeBasedImageProcessor PRIVATE include)
target_link_libraries(NodeBasedImageProcessor Qt5::Widgets Qt5::OpenGL ${OpenCV_LIBS})