#ifndef SVM_FEATURES_H
#define SVM_FEATURES_H

#include "opencv2/opencv.hpp"
#include "video_utils.h"

// Callback definition
typedef void (*featureCallback)(const cv::Mat& in_img, std::vector<double>& out_features);

/******* features *******/

/*Utility function to get all texture features as a std::vector<double>*/
void getTextureFeatures(const cv::Mat& in, std::vector<double>& features);
void getAverageColour(const cv::Mat& in, std::vector<double>& means);
void getContourFeatures(const cv::Mat& in, std::vector<double>& features);

void getAvgColorTiles(const cv::Mat& in,
                      const std::vector<std::vector<cv::Point>>& squares,
                      std::vector<double>& means);


void getTextureTiles(const cv::Mat& in,
                     const std::vector<std::vector<cv::Point>>& squares,
                     std::vector<double>& means);

/***** co-co-combos *****/

void getTextnColour(const cv::Mat& in, std::vector<double>& features);

void getRectFeatures(const cv::Mat& img, std::vector<double>& features);

#endif
