#include <svm_utils.h>

#include <fstream>
#include "image_utils.h"
#include "video_utils.h"
#include "blindtastic_core.h"

//show window, ask question
bool train_askuser(const cv::Mat& img, const cv::Rect rect, const std::string& question)
{
    cv::Mat ROI = img.clone();

    std::vector<cv::Point> corners(4);
    corners[0] = cv::Point(rect.x           , rect.y            );
    corners[1] = cv::Point(rect.x+rect.width, rect.y            );
    corners[2] = cv::Point(rect.x+rect.width, rect.y+rect.height);
    corners[3] = cv::Point(rect.x           , rect.y+rect.height);




    drawRect(ROI, corners, cv::Scalar(255,0,0));
    printText(ROI, question, rect.x + 50, rect.y+75);

    cv::imshow(question, ROI);

    int key = cv::waitKey();

    switch (key)
    {
        case YES:
            drawRect(ROI, corners, cv::Scalar(0,255,0));
            break;
        case NO:
            drawRect(ROI, corners, cv::Scalar(0,0,255));
            break;
        default:
            exit(1);
    }

    cv::imshow(question, ROI);
    cv::waitKey(100);

    return (key == YES);
}

//Method to produce SVM input to train square or rectangle tile
void man_train_tile(cv::Mat& image,const std::string& q, bool train){
    char buff[32] = {0};
    cv::Rect window = cv::Rect(0, 0, image.cols, image.rows);
    std::vector<std::vector<cv::Point> > squares;
    //cv::GaussianBlur(image,image,cv::Size(3,3),0);
    findSquares(image,squares);
    drawSquares(image,squares);
    //ratio
    double avgRatio=getRatio(squares);

    printText(image, std::string(buff));

    //svm format print
    if (train)
    {
        bool square = train_askuser(image, window, q);
        std::cout << (square ? "+1 " : "-1 ");
    }
    //OUTPUT: 1:tile_count 2:average_ratio
    std::cout <<"1:" << static_cast<unsigned int>(squares.size())<<" 2:"<<avgRatio<<std::endl;
}

//Method to produce SVM input to train specific pavement
void man_train_specific_paver(cv::Mat& image,const std::string& q, bool train){
    char buff[32] = {0};
    cv::Rect window = cv::Rect(0, 0, image.cols, image.rows);
    std::vector<std::vector<cv::Point> > squares;
    findSquares(image,squares);
    drawSquares(image,squares);
    //ratio
    double avgRatio=getRatio(squares);

    printText(image, std::string(buff));
    //colour
    std::vector<double> means;
    getAvgColorTiles(image, squares,means);
    double r=means[0];
    double g=means[1];
    double b=means[2];
    //texture
    std::vector<double>means2;
    getTextureTiles(image, squares,means2);
    double textureR=means2[0];
    double textureG=means2[1];
    double textureB=means2[2];
    //width
    std::vector<double> widthheight;
    widthheight=getAvgWidthHeight(squares);
    double avgWidth=widthheight[0];
    //height
    double avgHeight=widthheight[1];
    //svm format print
    if (train)
    {
        bool square = train_askuser(image, window, q);
        std::cout << (square ? "+1 " : "-1 ");
    }
    //OUTPUT: 1:tile_count 2:average_ratio 3:average_width 4:average_height 5:average_red 6:average:green 7:average_blue 8:average_texture_red 9:average_texture_green 10:average_texture_blue
    std::cout <<  "1:" << squares.size()
              << " 2:" << avgRatio
              << " 3:" << avgWidth
              << " 4:" << avgHeight
              << " 5:" << r
              << " 6:" << g
              << " 7:" << b
              << " 8:" << textureR
              << " 9:" << textureG
              << " 10:" << textureB
              << std::endl;

}

//Method for SVM input for grass
void man_train_grass(cv::Mat& frame,const std::string& q, bool train,int f){
    cv::Rect window = cv::Rect(0, 0, 640, 360); //deel frame in 4
    for(int row = 0; row< frame.rows; row+= window.height )
    {
        for(int col = 0; col < frame.cols;col += window.width)
        {
            cv::Mat ROI(frame,window); // region of interest
            double greenFeature = getAverageFilteredColour(ROI,GREEN_MIN,GREEN_MAX);
            std::vector<double> textureFeatures;
            getAverageTexture(ROI,textureFeatures);
            std::vector<double> features;
            features.push_back(greenFeature);
            features.insert(features.end(),textureFeatures.begin(),textureFeatures.end());

            if (train)
            {
                bool green = train_askuser(frame, window, q);
                std::cout << (green ? "+1 " : "-1 ");
            }

            //print features
            for (size_t i=1 ; i <= features.size() ; i++)
            {
                std::cout << i << ':' << features[i] << ' ';
            }
            std::cout << "# " << f << "[" << col << ',' << row << ']' << std::endl; // frame[x,y]

            window.x = window.x + window.width;
        }
        window.x = 0;
        window.y = window.y + window.height;
    }
}

//method that calls a SVM input method used on an image
void man_train_img(const char* imgLocation, const std::string& q, bool train){
   cv::Mat image;
   image = cv::imread(imgLocation, CV_LOAD_IMAGE_COLOR);   // Read the file

   if(! image.data )                              // Check for invalid input
   {
       std::cout <<  "Could not open or find the image" << std::endl ;
   }
   //train method:
   man_train_tile(image,q,train);
}


//method that calls a SVM input method used on an image
void man_train_video(const char* videoLocation, const std::string& q, bool train)

{
    cv::VideoCapture cap(videoLocation);
    assert(cap.isOpened());

    unsigned int frames = getFrameCount(cap);
    unsigned int step = (30 * 10); //elke 30s aan 10fps

    cv::Mat frame;
    for (unsigned int f=1 ; f < frames ; f+=step)
    {
        getFrameByNumber(cap,f,frame);
        //train method:
        man_train_grass(frame,q, train,f);

    }
}

void svm_trainGrass(const char* video)
{
    man_train_video(video, std::string("Contains grass? Y/N"));
}

void print_characteristics(const char* videoLocation)
{
    man_train_video(videoLocation, "", false);
}

void check_classification(const char* videoLocation, const char* classification)
{

}

void hardTrainSchool2Station()
{
    /*
     *  0: start (grote stenen)
     *  1: straat
     *  2: modderpad
     *  3: vert steentjes
     *  4: hor steentjes
     *  5: witte stenen
     *  6: vknte stenen
     *  7: pad beide kanten terug gras
     *  8: nu rechts struiken
     *  9: geen struiken meer
     * 10: alles vknte stenen
     * 11: terug gras rechts
     * 12: geen gras meer
     */
    std::vector<int> borders = {1,85,427,565,640,735,1205,1390,1570,2050,2100,2130,2200};
    Characteristic grass("Grass");
    Characteristic paver_huge("Huge Pavers at P building");
    Characteristic paver_brick_grey_v("Brick style grey pavers");
    Characteristic asphalt("Asphalt (black)");
    Characteristic gravel_dirt("Brown gravel / dirt");
    Characteristic paver_brick_pink_v("Brick style pink pavers (vertical)");
    Characteristic paver_brick_pink_h("Brick style pink pavers (horizontal)");
    Characteristic paver_pebble_white("Big square pebbled pavers");
    Characteristic paver_square_dull("Dull coloured square pavers");

    //0
    std::vector<Characteristic> ch11cs = {grass, paver_huge, paver_brick_grey_v};
    Location school("P gebouw",ch11cs);

    //1
    ch11cs = {asphalt};
    Location p_road("P baan",ch11cs);

    //2
    ch11cs = {grass, gravel_dirt};
    Location dirt_path("Modderpad",ch11cs);

    //3
    ch11cs = {grass, paver_brick_pink_v};
    Location gym_a("gym A",ch11cs);

    //4
    ch11cs = {grass, paver_brick_pink_h};
    Location gym_b("gym B",ch11cs);

    //5
    ch11cs = {grass, paver_pebble_white};
    Location gym_c("gym C",ch11cs);

    //6
    ch11cs = {grass, paver_square_dull};//voornamelijk zonder gras, ook stukken met fiets
    Location gym_d("gym D",ch11cs);

    //7
    ch11cs = {paver_square_dull};//af en toe ook paver_brick_grey_v, voornamelijk opritten
    Location stdenijs("St Denijs",ch11cs);
}

void train_paver_pebble_white(const char* vidloc, bool train)
{
    cv::VideoCapture cap(vidloc);
    assert(cap.isOpened());

    cv::Mat img;

    std::vector<std::vector<cv::Point>> squares;
    std::vector<double> means;
    int counter = 0;

    while(cap.isOpened())//getFrameByNumber(cap, counter, img))
    {
        cap.read(img);
        ++counter;

        squares.clear();
        findSquares(img,squares);

        if (train)
            std::cout << (counter < 735 || counter > 1205 ? "-1" : "+1");
        else
            std::cout << "0";

        //colour + texture
        means.clear();
        getAvgColorTiles(img, squares, means);
        getTextureTiles(img, squares, means);

        //print
        for (int i=0 ; i < means.size() ; i++)
        {
            std::cout << " " << i << ":" << means[i];
        }
        std::cout << std::endl;
    }
}

void play_predictions(const char* fvid, const char* fpred)
{
    const char* winp = "predictions";
    std::ifstream pred(fvid);
    cv::VideoCapture cap(fvid);

    std::string certainty;
    cv::Mat img;

    while(cap.isOpened() && pred.is_open())
    {
        pred >> certainty;
        cap.read(img);

        printText(img, certainty);

        cv::imshow(winp, img);

        if (cv::waitKey(25) >= 0) //play at 4x speed
            break;
    }

    if (pred.is_open())
    {
        std::cout << "not all predictions shown" << std::endl;
        pred.close();
    }
    if (cap.isOpened())
    {
        std::cout << "not all frames shown" << std::endl;
        cap.release();
    }
}
