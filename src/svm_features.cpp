#include "svm_features.h"
#include "image_utils.h"
#include "video_utils.h"

void getAverageColour(const cv::Mat& in,std::vector<double>& means){
    cv::Scalar s = cv::mean(in);
    means.push_back(s[0]);
    means.push_back(s[1]);
    means.push_back(s[2]);
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

void getAvgColorTiles(const cv::Mat& in, const std::vector<std::vector<cv::Point>>& squares,std::vector<double>& means){
    if (squares.empty())
        return;

    std::vector<double> R;
    std::vector<double> G;
    std::vector<double> B;

    for(size_t i=0; i< squares.size();i++){
        getAvgColorSingleTile(in,R,G,B,squares[i]);
    }
    double r = 0.0, g = 0.0, b = 0.0;
    for(size_t j=0; j<R.size(); j++){
        r+=R[j];
        g+=G[j];
        b+=B[j];
    }
    r=r/R.size();
    g=g/G.size();
    b=b/B.size();
    means.push_back(r);
    means.push_back(g);
    means.push_back(b);
}

void getTextureTiles(const cv::Mat& in, const std::vector<std::vector<cv::Point>>& squares,std::vector<double>& means){
    if (squares.empty())
        return;

    std::vector<double> R;
    std::vector<double> G;
    std::vector<double> B;

    for(size_t i=0; i< squares.size();i++){
        getAvgTextureTile(in, R, G, B, squares[i]);
    }

    double r=0.0, g=0.0, b=0.0;
    for(size_t j=0; j<R.size(); j++){
        r+=R[j];
        g+=G[j];
        b+=B[j];
    }
    r=r/R.size();
    g=g/G.size();
    b=b/B.size();
    means.push_back(r);
    means.push_back(g);
    means.push_back(b);
}


/***** co-co-combos *****/

void getTextnColour(const cv::Mat& in, std::vector<double>& features)
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
}
