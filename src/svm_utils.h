#ifndef SVM_SHIT_H
#define SVM_SHIT_H

#include "opencv2/opencv.hpp"

#define K_ESC 27
#define K_SPC 32
#define K_Q (int)'q'
#define K_Y (int)'y'
#define K_N (int)'n'

namespace td {
    int waitKey(int delay = 0);
}

//Method to produce SVM input to train square or rectangle tile
void man_train_tile(cv::Mat& image,const std::string& q, bool train);

//method that calls a SVM input method used on an image
void man_train_img(const char* imgLocation, const std::string& q, bool train = true);

//method that calls a SVM input method used on an image
void man_train_video(const char* videoLocation, const std::string& q);

//method that prints all ch11cs in a video (per frame) to stdout
void print_characteristics(const char* videoLocation);

//shows gridpredictions (assumes 9x9 grid, 1 frame per 100)
void play_grasspredictions(const char* fvid, const char* fpred);

////////////////// pruts /////////////////////

void hardTrainSchool2Station();

void train_paver_pebble_white(const char* vidloc, bool train=true);

void play_predictions(const char* fvid, const char* fpred);




#endif // SVM_SHIT_H
