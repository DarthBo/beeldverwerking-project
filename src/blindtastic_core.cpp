#include "blindtastic_core.h"
#include "svm_utils.h"

CharacteristicValue CharacteristicDefinition::getValue(const cv::Mat& img, bool skip_datacalc) const
{
    CharacteristicValue val;
    val.definition = this;
    val.weight = 0.0;

    if (!classify_frame(img,val,skip_datacalc)) //this will update val.weight when succesful
        std::cerr << "UNKNOWN ERROR processing frame" << std::endl;

    return val;
}

/*IMAGEGRID */
void ImageGrid::populate(){

    //init vectors
    elements.resize(rows);
    for(int row = 0; row<rows; row++){
        elements[row].resize(cols);
    }

    cv::Mat mat = image.getMat();

    int elementWidth = static_cast<int>(ceil((double)mat.cols/cols));
    int elementHeight = static_cast<int>(ceil((double)mat.rows/rows));

    int count = 0; //debugging
    cv::Rect window = cv::Rect(0, 0, elementWidth, elementHeight);
    for(int row = 0; row<rows-1; row++){
        for(int col = 0; col<cols-1; col++){
            cv::Mat ROI(mat,window);
            GridElement element(ROI, window);
            elements[row][col] = element;
            window.x = window.x + window.width;
            count++;
        }
        window.x = 0;
        window.y += window.height;
    }
    window.x = window.width*(cols-1);
    //final row/column might have different dimensions
    int remainingX = mat.cols - window.x;
    int remainingY = mat.rows - window.y;

    //final column except for last element, this we will handle last, can have a different width
    window.width = remainingX;
    window.y = 0;
    for(int row = 0; row<rows-1; row++){
        cv::Mat ROI(mat,window);
        GridElement element(ROI, window);
        elements[row][cols-1] = element;
        window.y += window.height;
        count++;
    }
    //final row except for last element, this we will handle last, can have a different height
    window.width = elementWidth;
    window.height = remainingY;
    window.x = 0;
    for(int col = 0; col<cols-1; col++){
        cv::Mat ROI(mat,window);
        GridElement element(ROI, window);
        elements[rows-1][col] = element;
        window.x += window.width;
        count++;
    }
    //final element can have a different width and height
    window.width = remainingX;
    cv::Mat ROI(mat,window);
    GridElement element(ROI, window);
    elements[rows-1][cols-1] = element;
    count++;
}

//shows how to access and change pixels
void ImageGrid::test(){
    int count = 0;
    for(unsigned int i = 0; i< elements.size();i++){
        for(unsigned int j = 0; j< elements[i].size();j++){
            cv::Mat m = elements[i][j].getMat();
            for(int row = 0; row < (m).rows;row++){
                for(int col = 0; col < (m).cols;col++){
                    cv::Scalar s(1,0,0);
                    m.at<cv::Vec3b>(row,col)[0] =  count     %255;
                    m.at<cv::Vec3b>(row,col)[1] = (count+125)%255;
                    m.at<cv::Vec3b>(row,col)[2] = (count+250)%255;
                }
            count++;
            }
        }
    }
}
