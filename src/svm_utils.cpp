#include <svm_utils.h>

#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "video_utils.h"
#include "blindtastic_core.h"
#include "svm_features.h"


int td::waitKey(int delay)
{
    int k = cv::waitKey(delay);
    if (k != -1) k &= 0xFF; //workaround for linux bug
    return k;
}

//show window, ask question
int train_askuser(const cv::Mat& img, const cv::Rect rect, const std::string& question);


/*****************************************************************************/

//Fucntion for training a frame (a single image) with grid
void manual_train_with_imagegrid(cv::Mat& frame, const std::string& q,
                                 featureCallback genFeatures,
                                 bool train,
                                 int f,
                                 int rows,
                                 int columns,
                                 std::ostream &os)
{
    ImageGrid grid(frame, rows, columns);

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

            genFeatures(ROI, features);

            if (train)
            {
                int hasCharacteristic = train_askuser(frame, window, q);
                if (hasCharacteristic < 0)
                {
                    if (hasCharacteristic == -1)
                        return;
                    col++;
                    continue;
                }
                os << ((hasCharacteristic == K_Y) ? "+1 " : "-1 ");
            }
            else
            {
                os << "0 ";
            }
            //set precision
            //os << std::setprecision(10);
            //print features
            for (size_t i=0 ; i < features.size() ; i++)
            {
                os << i+1 << ':' << features[i] << ' ';
            }
            os << "# frame " << f << std::endl; // frame[x,y]

            col++;
        }
        row++;
    }
}

//Function that starts playing the supplied video for manual training with a grid
void start_manual_train_with_imagegrid_video(const char* videoLocation,
                                             const std::string& q,
                                             featureCallback genFeatures,
                                             int rows,
                                             int columns)
{
    struct trackdata data;
    data.cap.open(videoLocation);
    cv::namedWindow(q);
    const char* track_class = "frame";

    //cv::Mat frame;
    int f = 0;

    cv::createTrackbar(track_class, q, &f, getFrameCount(videoLocation),&trackbar_moved, &data);

    while (data.cap.isOpened() && data.cap.read(data.img))
    {
        ++f;

        cv::imshow(q,data.img);
        cv::setTrackbarPos(track_class, q, f);
        int key = td::waitKey(100);

        if (key >= 0)
        {
            switch (key) {
            case K_Q:
                return;
            case K_ESC:
                return;
            case K_SPC:
                manual_train_with_imagegrid(data.img,q,genFeatures,true,f,rows,columns);
                break;
            default:
                break;
            }
        }
    }
    cv::destroyWindow(q);
}


void manual_train_full_frame(cv::Mat& frame,
                             const std::string& q,
                             featureCallback genFeatures,
                             bool train,
                             int f,
                             std::ostream& os = std::cout)
{
    std::vector<double> features;

    genFeatures(frame, features);

    /* remove me */
    std::vector<std::vector<cv::Point>> squares;
    findSquares(frame,squares);
    drawSquares(frame,squares);
    /*************/

    if (train)
    {
        int hasCharacteristic = train_askuser(frame, cv::Rect(0,0,frame.cols,frame.rows), q);
        if (hasCharacteristic < 0)
        {
            if (hasCharacteristic == -1)
                return;
        }
        os << ((hasCharacteristic == K_Y) ? "+1 " : "-1 ");
    }
    else
    {
        os << "0 ";
    }
    //set precision
    os << std::setprecision(10);
    //print features
    for (size_t i=0 ; i < features.size() ; i++)
    {
        os << i+1 << ':' << features[i] << ' ';
    }
    os << "# frame " << f << std::endl; // frame[x,y]
}

//Function that starts playing the supplied video for manual training
void start_manual_train_frame_video(const char* videoLocation,
                                    const std::string& q,
                                    featureCallback genFeatures)
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
                manual_train_full_frame(frame,q,genFeatures,true,f);
                break;
            default:
                break;
            }
        }
    }
}

//Function that prints all ch11cs in a video (per frame, with grid) to stdout
void print_imagegrid_features(const char* videoLocation, featureCallback genFeatures, int rows, int columns, int once_every_x_frames)
{
    cv::VideoCapture cap(videoLocation);
    cv::Mat frame;
    unsigned int f = 0;

    while (cap.isOpened() && cap.read(frame))
    {
        ++f;
        if (f%once_every_x_frames == 0)
            manual_train_with_imagegrid(frame, "", genFeatures, false, f, rows, columns);
    }
}

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

//Function that prints characteristics, adds +/-1 based on framerange
void auto_train_video (const char* vidloc,
                       featureCallback genFeatures,
                       int from_frame,
                       int to_frame,
                       int once_every_x_frames,
                       bool train)
{
    cv::VideoCapture cap(vidloc);
    assert(cap.isOpened());

    cv::Mat img;

    int counter = 0;

    while(cap.read(img))
    {
        ++counter;

        if (counter % once_every_x_frames == 1 || ( counter > from_frame && counter < to_frame && counter % 10 == 0))
        {
            std::vector<double> features;

            genFeatures(img, features);

            if(features.size()>1){
                //print
                if (train)
                    std::cout << (counter < from_frame || counter > to_frame ? "-1" : "+1");
                else
                    std::cout << "0";

                for (int i=0 ; i < features.size() ; i++)
                {
                    std::cout << " " << i+1 << ":" << features[i];
                }
                std::cout << " # frame: " << counter << std::endl;
            }
        }
    }
}

//Reads and shows gridpredictions (assumes 9x9 grid, 1 frame per 100)
//this code is awful, save your soul by not reading it
void play_grid_predictions(const char* fvid, const char* fpred, int rows, int columns, int once_every_x_frames)
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

        if (f%once_every_x_frames == 0)
        {
            double d;
            int pos = 0;

            ImageGrid g(img, rows, columns);
            ImageGrid::const_it_row row = g.begin();
            ImageGrid::const_it_col col = row->begin();

            for (int i=0 ; i<rows*columns ; i++)
            {
                pred >> d;
                cv::Rect win = (*col).getWindow();
                if (d > 0)
                {
                    pos++;
                    drawRect(img, win);
                }
                /*
                else
                {
                    drawRect(img, win, cv::Scalar(0,0,255));
                }*/

                col++;
                if (col == row->end())
                {
                    row++;
                    if (row != g.end())
                        col = row->begin();
                }
            }
            char buf[100] = {0};
            sprintf(buf, "%d/%d", pos,rows*columns);
            certainty = buf;

            printText(img, certainty);
            cv::imshow(winp, img);
            int k = td::waitKey();
            if (k == K_ESC || k == K_Q)
                cap.release();
        }
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

//Reads and shows predictions for full frame
void play_frame_predictions(const char* fvid, const char* fpred, int once_every_x_frames)
{
    const char* winp = "predictions";
    std::ifstream pred(fpred);
    cv::VideoCapture cap(fvid);

    double certainty = 0;
    cv::Mat img;

    int frame = 0;
    int diff = 0;
    char buff[64] = {0};

    while(cap.isOpened() && cap.read(img) && pred.is_open())
    {
        ++frame;

        if (frame % once_every_x_frames == 1)
        {
            pred >> certainty;
            diff = 0;
        }
        else
        {
            diff++;
        }

        if (certainty != 0)
        {
            sprintf(buff, "%s! (%d frames ago)",(certainty > 0 ? "Match" : "Nope"), diff);

            printText(img, std::string(buff));
        }

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



/************************************************************************************/


//Method to produce SVM input to train square or rectangle tile
void man_train_tile(cv::Mat& image,const std::string& q, bool train){
    char buff[32] = {0};
    cv::Rect window = cv::Rect(0, 0, image.cols, image.rows);
    std::vector<std::vector<cv::Point> > squares;
    double avgRatio;
    //cv::GaussianBlur(image,image,cv::Size(3,3),0);
    findSquares(image,squares);
    drawSquares(image,squares);
    //ratio
    getRatio(squares, avgRatio);

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
    double avgRatio;
    findSquares(image,squares);
    drawSquares(image,squares);
    //ratio
    getRatio(squares,avgRatio);

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



//method that calls a SVM input method used on an image
void start_manual_training_image(const char* imgLocation, const std::string& q, bool train){
   cv::Mat image;
   image = cv::imread(imgLocation, CV_LOAD_IMAGE_COLOR);   // Read the file

   if(! image.data )                              // Check for invalid input
   {
       std::cout <<  "Could not open or find the image" << std::endl ;
   }
   //train method:
   man_train_tile(image,q,train);
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
    /*
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
    */
}

bool classify_frame(cv::Mat frame, CharacteristicValue& c, bool skip_datacalc)
{
    FILE * fio;

    if (!skip_datacalc)
    {
        //calculate features
        std::ostringstream ss;
        manual_train_with_imagegrid(frame, "", c.definition->getFeature(), false, 0,
                                    c.definition->getRows(), c.definition->getColumns(), ss);
        std::string s = ss.str();

        //output to temp file
        fio = fopen(".tmp_feat", "w"); // create/overwrite
        if (fio == NULL)
        {
            std::cerr << "can't open .tmp_feat for writing" << std::endl;
            return false;
        }
        fwrite(s.c_str(),s.size(),sizeof(char),fio);
        fclose(fio);
    }
    //else: reuse last calculated features

    //call svmPerf
    #ifdef _WIN32
        const char* svmperf_class = "svm_perf_classify.exe";
        const char* nul = ">NUL";
    #else
        const char* svmperf_class = "./svm_perf_classify";
        const char* nul = ">/dev/null 2>/dev/null";
    #endif

    char buf[100] = {0};
    sprintf(buf, "%s .tmp_feat %s .tmp_result %s", svmperf_class, c.definition->getModel().c_str(), nul);

    if (system(buf) != 0)
    {
        std::cerr << "class failed" << std::endl;
        return false;
    }
    //read predictions
    fio = fopen(".tmp_result", "r");
    if (fio == NULL)
        return false;

    int pos_tally = 0;
    double pos_sum = 0.0;

    while (fgets (buf, 100, fio) != NULL)
    {
        double d = atof(buf);
        if (d > 0)
        {
            pos_tally++;
            pos_sum += d;
        }
    }
    fclose(fio);

    //save result
    if (pos_tally > 0 && pos_tally >= c.definition->getRows()*c.definition->getColumns()*c.definition->getRequiredRatio())
        c.weight = pos_sum/pos_tally;
    else
        c.weight = -1; // weight doesn't really matter here, we're not going to use it anyway

    return true;
}
