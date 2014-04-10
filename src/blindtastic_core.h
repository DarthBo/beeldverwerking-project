#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include "opencv2/opencv.hpp"

class Feature{
protected:
    const char* name;
    double value;
public:
    Feature(const char* _name,double _value):name(_name),value(_value){}
    const char* getName(){return name;}
    double getValue(){return value;};
};

class Characteristic{
    const char* name;
    std::vector<Feature> features;
};

class GridElement{
protected:
    int width;
    int height;
    Characteristic characteristic;
public:
    GridElement(int _width,int _height):width(_width),height(_height){}
    Characteristic* getCharacteristic(){return &characteristic;}
};

class ImageGrid{
protected:
    int gridHeight;
    int gridWidth;
    int elementHeight;
    int elementWidth;
    std::vector<std::vector<GridElement>> elements;
public:
    ImageGrid(int _gridHeight,int _gridWidth,int _elementHeight,int _elementWidth)
        :gridHeight(_gridHeight),gridWidth(_gridWidth),elementHeight(_elementHeight),elementWidth(_elementWidth){}
};

class Image{
    cv::Mat image;
    ImageGrid grid;
    std::vector<Characteristic> characteristics;
};

#endif
