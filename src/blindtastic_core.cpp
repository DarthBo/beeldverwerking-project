#include "blindtastic_core.h"


/*CHARACTERISTICTREE */
void CharacteristicTree::refine(Characteristic& characteristic){
    if(current == nullptr)
        return;
    if(current->getCharacteristic() == characteristic){
        if(characteristic.isDetected()){
            current = *current->getLeftChild();
        }else{
            current = *current->getRightChild();
        }
        traverseWithPool();
    }else{
        characteristicPool.push_back(characteristic);
    }
}

void CharacteristicTree::traverseWithPool(){
   for_each(characteristicPool.begin(),characteristicPool.end(),[=](Characteristic c){
       if(current->getCharacteristic() == c){
           if(c.isDetected()){
               current = *current->getLeftChild();
           }else{
               current = *current->getRightChild();
           }
           characteristicPool.remove(c);
           traverseWithPool();
       }
   });
}

void CharacteristicTree::addBreadthFirst(const Characteristic& characteristic,const std::vector<Location*>& possibleLocations){
    CharacteristicNode** curr = &root;
    std::queue<CharacteristicNode**> q;
    if((*curr) != nullptr){
        q.push((*curr)->getLeftChild());
        q.push((*curr)->getRightChild());
    }
    while((*curr) != nullptr){
        curr = q.front();
        q.pop();
        if((*curr) != nullptr){
            q.push((*curr)->getLeftChild());
            q.push((*curr)->getRightChild());
        }
    }
    *curr = new CharacteristicNode(characteristic,possibleLocations);
}

void CharacteristicTree::printBreadthFirst(){
    CharacteristicNode** curr = &root;
    std::queue<CharacteristicNode**> q;
    if((*curr) != nullptr){
        q.push(curr);
    }
    while(q.size() != 0){
        curr = q.front();
        q.pop();
        std::cout<<(*curr)->getCharacteristic().getName() <<std::endl;
        if((*(*curr)->getLeftChild()) != nullptr)
            q.push((*curr)->getLeftChild());
        if((*(*curr)->getRightChild()) != nullptr)
            q.push((*curr)->getRightChild());
    }
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
