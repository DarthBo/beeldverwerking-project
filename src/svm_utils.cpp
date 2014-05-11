#include <svm_utils.h>

#include <fstream>
#include "video_utils.h"
#include "blindtastic_core.h"
#include "svm_features.h"


inline int td::waitKey(int delay)
{
    int k = cv::waitKey(delay);
    if (k != -1) k &= 0xFF; //workaround for linux bug
    return k;
}

//show window, ask question
int train_askuser(const cv::Mat& img, const cv::Rect rect, const std::string& question);

//Method to produce SVM input to train specific pavement
void man_train_specific_paver(cv::Mat& image,const std::string& q, bool train);

//Method for SVM input for grass
void man_train_grass(cv::Mat& frame,const std::string& q, bool train,int f=0);

/*****************************************************************************/


//show window, ask question
int train_askuser(const cv::Mat& img, const cv::Rect rect, const std::string& question)
{
    cv::Mat ROI = img.clone();

    std::vector<cv::Point> corners(4);
    corners[0] = cv::Point(rect.x           , rect.y            );
    corners[1] = cv::Point(rect.x+rect.width, rect.y            );
    corners[2] = cv::Point(rect.x+rect.width, rect.y+rect.height);
    corners[3] = cv::Point(rect.x           , rect.y+rect.height);

    drawRect(ROI, corners, cv::Scalar(255,0,0));
    printText(ROI, question);

    cv::imshow(question, ROI);

    int key = td::waitKey();

    switch (key)
    {
        case K_Y:
            drawRect(ROI, corners, cv::Scalar(0,255,0));
            break;
        case K_N:
            drawRect(ROI, corners, cv::Scalar(0,0,255));
            break;
        case (int)'h':
            return -2;
        default:
            return -1;
    }

    cv::imshow(question, ROI);
    td::waitKey(100); //show choice for 100ms

    return key;
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
        bool square = (train_askuser(image, window, q) == K_Y);
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

//Improved training function for SVM input for grass
void man_train_grass(cv::Mat& frame,const std::string& q, bool train, int f){

    ImageGrid grid(frame, 9, 9);

    ImageGrid::const_it_row row = grid.begin();
    while (row != grid.end())
    {
        ImageGrid::const_it_col col = row->begin();
        while (col != row->end())
        {
            GridElement el = *col;

            cv::Mat ROI = el.getMat();
            cv::Rect window = el.getWindow();

            std::vector<double> features;

            getAverageColour(ROI, features);
            getTextureFeatures(ROI,features);

            if (train)
            {
                int green = train_askuser(frame, window, q);
                if (green < 0)
                {
                    if (green == -1)
                        return;
                    col++;
                    continue;
                }
                std::cout << ((green == K_Y) ? "+1 " : "-1 ");
            }
            else
            {
                std::cout << "0 ";
            }

            //print features
            for (size_t i=1 ; i <= features.size() ; i++)
            {
                std::cout << i << ':' << features[i] << ' ';
            }
            std::cout << "# frame " << f << std::endl; // frame[x,y]

            col++;
        }
        row++;
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
void man_train_video(const char* videoLocation, const std::string& q)
{
    cv::VideoCapture cap(videoLocation);
    cv::Mat frame;
    unsigned int f = 0;

    while (cap.isOpened() && cap.read(frame))
    {
        ++f;

        cv::imshow(q,frame);
        int key = td::waitKey(100);

        if (key >= 0)
        {
            switch (key) {
            case K_Q:
                return;
            case K_ESC:
                return;
            case K_SPC:
                man_train_grass(frame,q,true,f);
                break;
            default:
                break;
            }
        }
    }
}

void print_characteristics(const char* videoLocation)
{
    cv::VideoCapture cap(videoLocation);
    cv::Mat frame;
    unsigned int f = 0;

    while (cap.isOpened() && cap.read(frame))
    {
        ++f;
        if (f%100 == 0)
            man_train_grass(frame, "", false, f);
    }
}

//train (defaultVid) zones adhv hardgecodeerde grenzen (framenummers)
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

    while(cap.read(img))
    {
        ++counter;

        squares.clear();
        findSquares(img,squares);

        //colour + texture
        means.clear();

        getAvgColorTiles(img, squares, means);
        getTextureTiles(img, squares, means);

        //print
        if (train)
            std::cout << (counter < 735 || counter > 1205 ? "-1" : "+1");
        else
            std::cout << "0";
        for (int i=0 ; i < means.size() ; i++)
        {
            std::cout << " " << i << ":" << means[i];
        }
        std::cout << " # frame: " << counter << std::endl;
    }
}

void play_predictions(const char* fvid, const char* fpred)
{
    const char* winp = "predictions";
    std::ifstream pred(fpred);
    cv::VideoCapture cap(fvid);

    std::string certainty;
    cv::Mat img;

    while(cap.isOpened() && cap.read(img) && pred.is_open())
    {
        pred >> certainty;

        printText(img, certainty);

        cv::imshow(winp, img);

        if (td::waitKey(25) >= 0) //play at 4x speed
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

void play_grasspredictions(const char* fvid, const char* fpred)
{
    const char* winp = "predictions";
    std::ifstream pred(fpred);
    cv::VideoCapture cap(fvid);

    std::string certainty;
    cv::Mat img;

    int f = 0;

    while(cap.isOpened() && cap.read(img) && pred.is_open())
    {
        ++f;

        if (f%100 == 0)
        {
            double d;
            int pos = 0;
            for (int i=0 ; i<81 ; i++)
            {
                pred >> d;
                if (d > 0) pos++;
            }
            char buf[100] = {0};
            sprintf(buf, "%d/81", pos);
            certainty = buf;

            printText(img, certainty);
            cv::imshow(winp, img);
            td::waitKey();
        }


        //if (td::waitKey(25) >= 0) //play at 4x speed
        //    break;

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
