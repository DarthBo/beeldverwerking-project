#ifndef BLINDTASTIC_CORE_H
#define BLINDTASTIC_CORE_H

#include "opencv2/opencv.hpp"

class Feature{
protected:
    std::string name;
    double value;
public:
    Feature(const char* _name,double _value):name(_name),value(_value){}
    const char* getName(){return name.c_str();}
    double getValue(){return value;}
};

class Characteristic{
    std::string name;
    double weight;
    std::vector<Feature> features;
};

class GridElement{
protected:
    cv::Mat element;
    std::vector<Characteristic> characteristics;
public:
    GridElement(){}
    GridElement(cv::Mat _element):element(_element){}
    //Characteristic* getCharacteristic(){return &characteristic;}
    cv::Mat* getMat(){return &element;}
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
    void test();
public:
    //Elements of final column and row may not have given dimensions if height and width cannot be properly divided
    ImageGrid(cv::Mat _image,int _elementWidth, int _elementHeight):image(_image),elementHeight(_elementHeight),elementWidth(_elementWidth){
        populate();
    }

    void populateCharacteristics(){
        //move over grid and call SVM for every element to find the Characteristics
    }
};

void ImageGrid::populate(){
    cv::Mat* mat = image.getMat();
    int rows = ceil((double)mat->rows/elementHeight);
    int cols = ceil((double)mat->cols/elementWidth);
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
    window.x = window.width*(cols-1);
    //final row/column might have different dimensions
    int remainingX = mat->cols - window.x;
    int remainingY = mat->rows - window.y;

    //final column except for last element, this we will handle last, can have a different width
    window.width = remainingX;
    window.y = 0;
    for(int row = 0; row<rows-1; row++){
        cv::Mat ROI(*mat,window);
        GridElement element(ROI);
        elements[row][cols-1] = element;
        window.y += window.height;
        count++;
    }
    //final row except for last element, this we will handle last, can have a different height
    window.width = elementWidth;
    window.height = remainingY;
    window.x = 0;
    for(int col = 0; col<cols-1; col++){
        cv::Mat ROI(*mat,window);
        GridElement element(ROI);
        elements[rows-1][col] = element;
        window.x += window.width;
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

//shows how to access and change pixels
void ImageGrid::test(){
    int count = 0;
    for(unsigned int i = 0; i< elements.size();i++){
        for(unsigned int j = 0; j< elements[i].size();j++){
            cv::Mat* m = elements[i][j].getMat();
            for(int row = 0; row < (*m).rows;row++){
                for(int col = 0; col < (*m).cols;col++){
                    cv::Scalar s(1,0,0);
                    m->at<cv::Vec3b>(row,col)[0] =  count     %255;
                    m->at<cv::Vec3b>(row,col)[1] = (count+125)%255;
                    m->at<cv::Vec3b>(row,col)[2] = (count+250)%255;
                }
            count++;
            }
        }
    }
}

#endif
