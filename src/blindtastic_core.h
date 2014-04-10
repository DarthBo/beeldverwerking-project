#ifndef BLINDTASTIC_CORE_H
#define BLINDTASTIC_CORE_H

#include "opencv2/opencv.hpp"

class Feature{
protected:
    const char* name;
    double value;
public:
    Feature(const char* _name,double _value):name(_name),value(_value){}
    const char* getName(){return name;}
    double getValue(){return value;}
};

class Characteristic{
    const char* name;
    std::vector<Feature> features;
};

class GridElement{
protected:
    cv::Mat element;
    Characteristic characteristic;
public:
    GridElement(){}
    GridElement(cv::Mat _element):element(_element){}
    Characteristic* getCharacteristic(){return &characteristic;}
};

class Image{
protected:
    cv::Mat mat;
    std::vector<Characteristic> characteristics;
public:
    Image(cv::Mat _image):mat(_image){}
    cv::Mat* getMat(){return &mat;}
};

class ImageGrid{
protected:
    Image image;
    int elementHeight;
    int elementWidth;
    std::vector<std::vector<GridElement>> elements;
    void populate();
public:
    ImageGrid(cv::Mat _image,int _elementWidth, int _elementHeight):image(_image),elementHeight(_elementHeight),elementWidth(_elementWidth){
        populate();
    }
};

void ImageGrid::populate(){
    cv::Mat* mat = image.getMat();
    int rows = ceil(mat->rows/elementHeight);
    int cols = ceil(mat->cols/elementWidth);
    //init vectors
    elements.resize(rows);
    for(int row = 0; row<rows; row++){
        elements[row].resize(cols);
    }
    std::cout<<"Populating "<<rows<<"x"<<cols<<" grid..."<<std::endl;
    int count = 0; //debugging
    cv::Rect window = cv::Rect(0, 0, elementWidth, elementHeight);
    for(int row = 0; row<rows-1; row++){
        for(int col = 0; col<cols-1; col++){
            cv::Mat ROI(*mat,window);
            GridElement element(ROI);
            elements[row][col] = element;
            window.x = window.x + window.width;
            count++;
        }
        window.x = 0;
        window.y += window.height;
    }
    //final row/column might have different dimensions
    int remainingX = mat->cols - window.x;
    int remainingY = mat->rows - window.y;

    //final column except for last element, this we will handle last, can have a different width
    window.width = remainingX;
    for(int row = 0; row<rows-1; row++){
        cv::Mat ROI(*mat,window);
        GridElement element(ROI);
        elements[row][cols-1] = element;
        count++;
    }
    //final row except for last element, this we will handle last, can have a different height
    window.width = elementWidth;
    window.height = remainingY;
    for(int col = 0; col<cols-1; col++){
        cv::Mat ROI(*mat,window);
        GridElement element(ROI);
        elements[rows-1][col] = element;
        count++;
    }
    //final element can have a different width and height
    window.width = remainingX;
    cv::Mat ROI(*mat,window);
    GridElement element(ROI);
    elements[rows-1][cols-1] = element;
    count++;

    std::cout<<"Done! Number of grid elements:"<<count<<std::endl;
}

#endif
