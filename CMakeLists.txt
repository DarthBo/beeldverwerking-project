cmake_minimum_required(VERSION 2.4)

SET(PROJECT_NAME "blindtastic")

PROJECT(${PROJECT_NAME})

# paths 
INCLUDE_DIRECTORIES(src)
find_package(OpenCV REQUIRED)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
SET(CMAKE_CXX_FLAGS "-o3 -Wall -g -std=c++0x")
SET(CMAKE_CXX_LINK_FLAGS "-pg")
SET(OpenCV_LIBRARIES opencv_core opencv_highgui opencv_imgproc opencv_gpu)

SET(SRC src/main.cpp
        src/image_utils.h
        src/image_utils.cpp
        src/svm_features.h
        src/svm_features.cpp
        src/file_utils.h
        src/video_utils.h
        src/video_utils.cpp
        src/svm_utils.h
        src/svm_utils.cpp
        src/blindtastic_core.h
)

ADD_EXECUTABLE(${PROJECT_NAME} ${SRC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${OpenCV_LIBRARIES})

SET(CMAKE_BUILD_TYPE Debug)
