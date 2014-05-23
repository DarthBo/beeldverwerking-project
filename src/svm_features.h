#ifndef SVM_FEATURES_H
#define SVM_FEATURES_H

#include "opencv2/opencv.hpp"
#include "video_utils.h"

// Callback definition
typedef void (*featureCallback)(const cv::Mat& in_img, std::vector<double>& out_features);

/******* features *******/

/*Utility function to get all colour features as a std::vector<double>*/
void getColourFeatures(const cv::Mat& in, std::vector<double>& features);
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
void getWhiteFeatures(const cv::Mat& in, std::vector<double>& features);

/***** co-co-combos *****/

void getRectFeatures(const cv::Mat& img, std::vector<double>& features);
void getTextnColour(const cv::Mat& in, std::vector<double>& features);
void getTextnHSVColour(const cv::Mat& in, std::vector<double>& features);
void getColourAndContourFeatures(const cv::Mat& in, std::vector<double>& features);

//old ones
void getTextnColour_legacy(const cv::Mat& in, std::vector<double>& features);

#endif
