#include "features.h"
#include "image_utils.h"

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
    for(int i : orientations){
        textureFilter(in,out,i);
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
