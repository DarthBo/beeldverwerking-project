#include "blindtastic_core.h"
#include "svm_utils.h"

CharacteristicValue CharacteristicDefinition::getValue(const cv::Mat& img, bool skip_datacalc) const
{
    CharacteristicValue val;
    val.definition = this;
    val.weight = 0.0;

    //init result grid
    std::vector<std::vector<double> > grid(getRows());
    for (int r=0 ; r<getRows() ; r++)
    {
        grid[r] = std::vector<double>(getColumns());
    }

    //fill result grid
    if (!classify_frame(img, val, grid, skip_datacalc))
        std::cerr << "UNKNOWN ERROR processing frame" << std::endl;

    //process results
    int pos_tally = 0;
    int neg_tally = 0;
    double pos_sum = 0.0;
    double neg_sum = 0.0;

    for (int row=0 ; row<getRows() ; row++)
    {
        for (int col=0 ; col<getColumns() ; col++)
        {
            if (grid[row][col] > 0)
            {
                pos_tally++;
                pos_sum += grid[row][col];
            }
            else
            {
                neg_tally++;
                neg_sum += grid[row][col];
            }
        }
    }

    //save result
    if (pos_tally > 0 && pos_tally >= getRows()*getColumns()*getRequiredRatio())
        val.weight = pos_sum/pos_tally;
    else
        val.weight = neg_sum/neg_tally;

    return val;
}

CharacteristicValue LRHelperCharacteristicDefinition::getValue(const cv::Mat& img, bool skip_datacalc) const
{
    CharacteristicValue val;
    val.definition = this;
    val.weight = 0.0;
    double w_left = 0.0;
    double w_right = 0.0;

    //init result grid
    std::vector<std::vector<double> > grid(getRows());
    for (int r=0 ; r<getRows() ; r++)
    {
        grid[r] = std::vector<double>(getColumns());
    }

    //fill result grid
    if (!classify_frame(img, val, grid, skip_datacalc))
        std::cerr << "UNKNOWN ERROR processing frame" << std::endl;

    //process results
    int pos_tally = 0;
    int neg_tally = 0;
    double pos_sum = 0.0;
    double neg_sum = 0.0;

    /****** try left ******/

    for (int row=0 ; row<getRows() ; row++)
    {
        for (int col=0 ; col<getColumns()/2 ; col++)
        {
            if (grid[row][col] > 0)
            {
                pos_tally++;
                pos_sum += grid[row][col];
            }
            else
            {
                neg_tally++;
                neg_sum += grid[row][col];
            }
        }
    }
    if (pos_tally > 0 && pos_tally >= getRows()*getColumns()/2*getRequiredRatio())
        w_left = pos_sum/pos_tally;
    else
        w_left = neg_sum/neg_tally;


    /****** try right ******/

    pos_tally = 0;
    neg_tally = 0;
    pos_sum = 0.0;
    neg_sum = 0.0;

    for (int row=0 ; row<getRows() ; row++)
    {
        for (int col=getColumns()/2 ; col<getColumns() ; col++)
        {
            if (grid[row][col] > 0)
            {
                pos_tally++;
                pos_sum += grid[row][col];
            }
            else
            {
                neg_tally++;
                neg_sum += grid[row][col];
            }
        }
    }
    if (pos_tally > 0 && pos_tally >= getRows()*getColumns()/2*getRequiredRatio())
        w_right = pos_sum/pos_tally;
    else
        w_right = neg_sum/neg_tally;

    /**** determine definition type ****/

    if (w_left > 0)
    {
        if (w_right > 0)
        {
            val.definition = def_leftright;
            val.weight = (w_left + w_right)/2;
        }
        else
        {
            val.definition = def_left;
            val.weight = w_left;
        }
    }
    else
    {
        if (w_right > 0)
        {
            val.definition = def_right;
            val.weight = w_right;
        }
        else
        {
            val.definition = def_none;
            val.weight = (w_left + w_right)/(-2);
        }
    }

    return val;
}

CharacteristicValue FakeCharacteristicDefinition::getValue(const cv::Mat& img, bool skip_datacalc) const
{
    assert(false);
    return CharacteristicValue();
}

/*IMAGEGRID */
void ImageGrid::populate(){

    //init vectors
    elements.resize(rows);
    for(int row = 0; row<rows; row++){
        elements[row].resize(cols);
    }

    cv::Mat mat = image.getMat();

    int elementWidth = static_cast<int>(ceil((double)mat.cols/cols));
    int elementHeight = static_cast<int>(ceil((double)mat.rows/rows));

    int count = 0; //debugging
    cv::Rect window = cv::Rect(0, 0, elementWidth, elementHeight);
    for(int row = 0; row<rows-1; row++){
        for(int col = 0; col<cols-1; col++){
            cv::Mat ROI(mat,window);
            GridElement element(ROI, window);
            elements[row][col] = element;
            window.x = window.x + window.width;
            count++;
        }
        window.x = 0;
        window.y += window.height;
    }
    window.x = window.width*(cols-1);
    //final row/column might have different dimensions
    int remainingX = mat.cols - window.x;
    int remainingY = mat.rows - window.y;

    //final column except for last element, this we will handle last, can have a different width
    window.width = remainingX;
    window.y = 0;
    for(int row = 0; row<rows-1; row++){
        cv::Mat ROI(mat,window);
        GridElement element(ROI, window);
        elements[row][cols-1] = element;
        window.y += window.height;
        count++;
    }
    //final row except for last element, this we will handle last, can have a different height
    window.width = elementWidth;
    window.height = remainingY;
    window.x = 0;
    for(int col = 0; col<cols-1; col++){
        cv::Mat ROI(mat,window);
        GridElement element(ROI, window);
        elements[rows-1][col] = element;
        window.x += window.width;
        count++;
    }
    //final element can have a different width and height
    window.width = remainingX;
    cv::Mat ROI(mat,window);
    GridElement element(ROI, window);
    elements[rows-1][cols-1] = element;
    count++;
}

//shows how to access and change pixels
void ImageGrid::test(){
    int count = 0;
    for(unsigned int i = 0; i< elements.size();i++){
        for(unsigned int j = 0; j< elements[i].size();j++){
            cv::Mat m = elements[i][j].getMat();
            for(int row = 0; row < (m).rows;row++){
                for(int col = 0; col < (m).cols;col++){
                    cv::Scalar s(1,0,0);
                    m.at<cv::Vec3b>(row,col)[0] =  count     %255;
                    m.at<cv::Vec3b>(row,col)[1] = (count+125)%255;
                    m.at<cv::Vec3b>(row,col)[2] = (count+250)%255;
                }
            count++;
            }
        }
    }
}
