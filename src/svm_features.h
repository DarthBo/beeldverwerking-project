#ifndef SVM_FEATURES_H
#define SVM_FEATURES_H

#include "opencv2/opencv.hpp"

void getAverageColour(const cv::Mat& in, std::vector<double>& means);

/*Utility function to get all texture features as a std::vector<double>*/
void getTextureFeatures(const cv::Mat& in, std::vector<double>& features);

void getContourFeatures(const cv::Mat& in, std::vector<double>& features);


#endif
