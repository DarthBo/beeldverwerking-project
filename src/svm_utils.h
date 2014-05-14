#ifndef SVM_SHIT_H
#define SVM_SHIT_H

#include "opencv2/opencv.hpp"
#include "svm_features.h"

#define K_ESC 27
#define K_SPC 32
#define K_Q (int)'q'
#define K_Y (int)'y'
#define K_N (int)'n'

namespace td {
    int waitKey(int delay = 0);
}

//Method for training a frame with grid
void manual_train_with_imagegrid(cv::Mat& frame, const std::string& q, featureCallback genFeatures, bool train, int f=0);


//method that calls a SVM input method used on an image
void start_manual_training_video(const char* videoLocation,
                                 const std::string& q,
                                 featureCallback genFeatures);

//method that prints all ch11cs in a video (per frame) to stdout
void print_imagegrid_features(const char* videoLocation,
                           featureCallback genFeatures,
                           int once_every_x_frames = 1);

void auto_train_video(const char* vidloc,
                      featureCallback genFeatures,
                      int from_frame,
                      int to_frame,
                      int once_every_x_frames = 1,
                      bool train=true);

////////////////// pruts /////////////////////

//method that calls a SVM input method used on an image
void start_manual_training_image(const char* imgLocation, const std::string& q, bool train = true);

void hardTrainSchool2Station();


void train_paver_pebble_white(const char* vidloc, bool train=true);

void play_predictions(const char* fvid, const char* fpred);

//shows gridpredictions (assumes 9x9 grid, 1 frame per 100)
void play_grasspredictions(const char* fvid, const char* fpred);


#endif // SVM_SHIT_H
