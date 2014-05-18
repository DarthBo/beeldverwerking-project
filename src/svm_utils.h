#ifndef SVM_SHIT_H
#define SVM_SHIT_H

#include "opencv2/opencv.hpp"
#include "svm_features.h"

#define K_ESC 27
#define K_SPC 32
#define K_Q (int)'q'
#define K_Y (int)'y'
#define K_N (int)'n'

#define DEFAULT_FRAMESKIP 100

namespace td {
    int waitKey(int delay = 0);
}

//Method for training a frame with grid
void manual_train_with_imagegrid(cv::Mat& frame,
                                 const std::string& q,
                                 featureCallback genFeatures,
                                 bool train,
                                 int f=0,
                                 int rows=9,
                                 int columns=9);

//method that calls a SVM input method used on an image
void start_manual_training_video(const char* videoLocation,
                                 const std::string& q,
                                 featureCallback genFeatures,
                                 int rows=9,
                                 int columns=9);

//method that prints all ch11cs in a video (per frame) to stdout
void print_imagegrid_features(const char* videoLocation,
                              featureCallback genFeatures,
                              int rows=9,
                              int columns=9,
                              int once_every_x_frames = DEFAULT_FRAMESKIP);

void auto_train_video(const char* vidloc,
                      featureCallback genFeatures,
                      int from_frame,
                      int to_frame,
                      int once_every_x_frames = DEFAULT_FRAMESKIP,
                      bool train=true);

//shows gridpredictions (assumes 9x9 grid, 1 frame per 100)
void play_grid_predictions(const char* fvid,
                           const char* fpred,
                           int rows=9,
                           int colums=9,
                           int once_every_x_frames = DEFAULT_FRAMESKIP);

//shows predictions for full frame
void play_frame_predictions(const char* fvid,
                            const char* fpred,
                            int once_every_x_frames = DEFAULT_FRAMESKIP);

////////////////// pruts /////////////////////

//method that calls a SVM input method used on an image
void start_manual_training_image(const char* imgLocation, const std::string& q, bool train = true);

void hardTrainSchool2Station();

#endif // SVM_SHIT_H
