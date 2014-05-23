#include "svm_features.h"
#include "image_utils.h"
#include "video_utils.h"

/*Utility function to get all colour features as a std::vector<double>*/
void getColourFeatures(const cv::Mat& in, std::vector<double>& features){
    std::vector<double> sums;
    squaredSum(in,sums);                   // local energy
    for(size_t i = 0; i< sums.size() ;i++){ // for every channel
        features.push_back(sums[i]/1000000);//normalise for SVM
    }
    sums.clear();
    absoluteSum(in,sums);                  //Mean Amplitude
    for(size_t i = 0; i< sums.size() ;i++){ // for every channel
        features.push_back(sums[i]/10000);  //normalise for SVM
    }
}


/* Utility feature that calls  cv::cvtColor(in,dst,CV_BGR2HSV_FULL) before calling getColourFeatures*/
void getHSVColourFeatures(const cv::Mat& in, std::vector<double>& features){
    cv::Mat dst;
    cv::cvtColor(in,dst,CV_BGR2HSV_FULL);
    getColourFeatures(dst,features);
}

/*Utility function to get all texture features as a std::vector<double>*/
void getTextureFeatures(const cv::Mat& in, std::vector<double>& features){
    int orientations[] = {0,45,90,135};
    cv::Mat out;
    for(int o : orientations){
        textureFilter(in,out,o);
        std::vector<double> sums;
        squaredSum(out,sums);                   // local energy
        for(size_t i = 0; i< sums.size() ;i++){ // for every channel
            features.push_back(sums[i]/1000000);//normalise for SVM
        }
        sums.clear();
        absoluteSum(out,sums);                  //Mean Amplitude
        for(size_t i = 0; i< sums.size() ;i++){ // for every channel
            features.push_back(sums[i]/10000);  //normalise for SVM
        }
    }
}

void getContourFeatures(const cv::Mat& in, std::vector<double>& features){
   cv::Mat out;
   contourFilter(in,out);
   std::vector<double> sums;
   squaredSum(out,sums);                   // local energy
   for(size_t i = 0; i< sums.size() ;i++){ // for every channel
       features.push_back(sums[i]);
   }
   sums.clear();
   absoluteSum(out,sums);                  //Mean Amplitude
   for(size_t i = 0; i< sums.size() ;i++){ // for every channel
       features.push_back(sums[i]);
   }
}

extern cv::Rect getrect(const std::vector<cv::Point> &square);

void getAvgColorTiles(const cv::Mat& in, const std::vector<std::vector<cv::Point>>& squares,std::vector<double>& means){
    if (squares.empty())
        return;

    std::vector<double> tmpA;
    std::vector<double> tmpB;
    int nrsqr = squares.size();

    for(size_t i=0; i< nrsqr;i++){
        tmpA.clear();
        cv::Mat ROI(in, getrect(squares[i]));
        getColourFeatures(ROI, tmpA);
        if (i == 0)
            tmpB = tmpA;
        else
            for (size_t j=0 ; j<tmpA.size() ;j++)
            {
                tmpB[j] += tmpA[j];
            }
    }
    for (size_t j=0 ; j<tmpB.size() ;j++)
    {
        means.push_back(tmpB[j]/nrsqr);
    }

}

void getTextureTiles(const cv::Mat& in, const std::vector<std::vector<cv::Point>>& squares,std::vector<double>& means){
    if (squares.empty())
        return;

    std::vector<double> tmpA;
    std::vector<double> tmpB;
    int nrsqr = squares.size();

    for(size_t i=0; i< nrsqr;i++){
        tmpA.clear();
        cv::Mat ROI(in, getrect(squares[i]));
        getTextureFeatures(ROI, tmpA);
        if (i == 0)
            tmpB = tmpA;
        else
            for (size_t j=0 ; j<tmpA.size() ;j++)
            {
                tmpB[j] += tmpA[j];
            }
    }
    for (size_t j=0 ; j<tmpB.size() ;j++)
    {
        means.push_back(tmpB[j]/nrsqr);
    }
}

void getRatioTiles(const std::vector<std::vector<cv::Point>>& squares, std::vector<double> & features){
    double ratio;
    getRatio(squares, ratio);
    features.push_back(ratio);
}

void getContourArea(const std::vector<std::vector<cv::Point>>& squares, std::vector<double>& features){
    double avgcontour;
    getAvgContourArea(squares,avgcontour);
    features.push_back(avgcontour/10000); //normalise
}

/***** co-co-combos *****/

void getTextnColour(const cv::Mat& in, std::vector<double>& features)
{
    getColourFeatures(in, features);
    getTextureFeatures(in, features);
}

void getTextnHSVColour(const cv::Mat& in, std::vector<double>& features){
    getHSVColourFeatures(in, features);
    getTextureFeatures(in, features);
}

void getColourAndContourFeatures(const cv::Mat& in, std::vector<double>& features){
    getColourFeatures(in, features);
    getContourFeatures(in,features);
}

void getTextnColour_legacy(const cv::Mat& in, std::vector<double>& features)
{
    getAverageColour(in, features);
    getTextureFeatures(in, features);
}

void getRectFeatures(const cv::Mat& img, std::vector<double>& features)
{
    std::vector<std::vector<cv::Point>> squares;
    findSquares(img,squares);

    features.push_back(squares.size());
    getAvgColorTiles(img, squares, features);
    getTextureTiles(img, squares, features);
    getRatioTiles(squares, features);
    getContourArea(squares,features);
}

void getWhiteFeatures(const cv::Mat& in, std::vector<double>& features){
    cv::Mat temp;
    std::vector<double> rows;
    std::vector<double> cols;
    whiteFilter(in,temp);
    reduce(temp,cols,0,CV_REDUCE_SUM);
    transpose(temp,temp);
    reduce(temp,rows,0,CV_REDUCE_SUM);
    for(unsigned int i=0; i<rows.size(); i++) features.push_back(rows[i]);
    for(unsigned int i=0; i<cols.size(); i++) features.push_back(cols[i]);
}
