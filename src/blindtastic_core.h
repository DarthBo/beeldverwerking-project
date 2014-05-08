#ifndef BLINDTASTIC_CORE_H
#define BLINDTASTIC_CORE_H

#include "opencv2/opencv.hpp"
#include <queue>

class Feature{
protected:
    std::string name;
    double value;
public:
    Feature(){}
    Feature(const std::string& _name,double _value):name(_name),value(_value){}
    bool operator==(const Feature& feature) const
    {
        return name == feature.name && value == feature.value;
    }
    bool operator!=(const Feature& feature) const
    {
        return !((*this).operator==(feature));
    }
    const std::string& getName() const{return name;}
    void setName(const std::string& name){this->name = name;}
    double getValue() const{return value;}
    void setValue(const double value){this->value = value;}
};

class Characteristic{
protected:
    std::string name;
    double weight;
    bool detected;
    std::vector<Feature> features;
public:
    Characteristic(){detected = true;}
    Characteristic(const std::string& _name):name(_name){}
    Characteristic(const std::string& _name, double _weight, bool _detected)
        :name(_name),weight(_weight),detected(_detected){}
    bool operator==(const Characteristic& characteristic) const
    {
        return name != characteristic.name && weight != characteristic.weight && features == characteristic.features && characteristic.detected == detected;
    }
    bool operator!=(const Characteristic& characteristic ) const
    {
        return !((*this).operator==(characteristic));
    }
    const std::string& getName() const{return name;}
    void setName(const std::string& name){this->name=name;}
    double getWeight(){return weight;}
    void setWeight(double weight){this->weight = weight;}
    const std::vector<Feature>& getFeatures() const{return features;}
    void setFeatures(const std::vector<Feature>& features){this->features = features;}
    bool isDetected(){return detected;}
    void setDetected(bool detected){this->detected = detected;}

};

class GridElement{
protected:
    cv::Mat element;
    std::vector<Characteristic> characteristics;
public:
    GridElement(){}
    GridElement(cv::Mat _element):element(_element){}
    const std::vector<Characteristic>& getCharacteristics() const{return characteristics;}
    void setCharacteristics(const std::vector<Characteristic>& characteristics){this->characteristics = characteristics;}
    const cv::Mat& getMat() const{return element;}
    void setMat(const cv::Mat& mat){this->element = mat;}
};

class Image{
protected:
    cv::Mat mat;
    std::vector<Characteristic> characteristics;
public:
    Image(cv::Mat _image):mat(_image){}
    const cv::Mat& getMat() const{return mat;}
    void setMat(const cv::Mat& mat){this->mat = mat;}
    const std::vector<Characteristic>& getCharacteristics() const{return characteristics;}
    void setCharacteristics(const std::vector<Characteristic>& characteristics){this->characteristics = characteristics;}
};

class ImageGrid{
protected:
    Image image;
    int elementHeight;
    int elementWidth;
    std::vector<std::vector<GridElement>> elements;
    void populate();
    void test();
public:
    //Elements of final column and row may not have given dimensions if height and width cannot be properly divided
    ImageGrid(cv::Mat _image,int _elementWidth, int _elementHeight):image(_image),elementHeight(_elementHeight),elementWidth(_elementWidth){
        populate();
    }

    void populateCharacteristics(){
        //move over grid and call SVM for every element to find the Characteristics
    }
};

class Location{
protected:
    std::string name;
    std::vector<Characteristic> characteristics;
public:
    Location(){}
    Location(const std::string& _name,std::vector<Characteristic> _characteristics):name(_name),characteristics(_characteristics){}
    const std::vector<Characteristic>& getCharacteristics() const{return characteristics;}
    void setCharacteristics(const std::vector<Characteristic>& characteristics){this->characteristics = characteristics;}
    const std::string& getName() const{return name;}
    void setName(const std::string& name){this->name = name;}
};

class LocationRepository {
    //TODO, stores all locations
private:
    std::vector<Location> locations;
    void init(){
        Characteristic grass("Grass");
        Characteristic paver_huge("Huge Pavers at P building");
        Characteristic paver_brick_grey_v("Brick style grey pavers");
        Characteristic asphalt("Asphalt (black)");
        Characteristic gravel_dirt("Brown gravel / dirt");
        Characteristic paver_brick_pink_v("Brick style pink pavers (vertical)");
        Characteristic paver_brick_pink_h("Brick style pink pavers (horizontal)");
        Characteristic paver_pebble_white("Big square pebbled pavers");
        Characteristic paver_square_dull("Dull coloured square pavers");
        Characteristic paver_train_station("Fancy (diamond) pavers");
        Characteristic zebra_crossing("Zebra crossing");
        Characteristic white_train_station("(avg colour) bland white");

        std::vector<Characteristic> ch11cs = {grass, paver_huge, paver_brick_grey_v};
        Location school("P gebouw",ch11cs);
        locations.push_back(school);

        ch11cs = {asphalt};
        Location p_road("P baan",ch11cs);
        locations.push_back(p_road);

        //missing: area inbetween

        ch11cs = {grass, gravel_dirt};
        Location dirt_path("Modderpad",ch11cs);
        locations.push_back(dirt_path);

        ch11cs = {grass, paver_brick_pink_v};
        Location gym_a("gym A",ch11cs);
        locations.push_back(gym_a);

        ch11cs = {grass, paver_brick_pink_h};
        Location gym_b("gym B",ch11cs);
        locations.push_back(gym_b);

        ch11cs = {grass, paver_pebble_white};
        Location gym_c("gym C",ch11cs);
        locations.push_back(gym_c);

        ch11cs = {grass, paver_square_dull};//voornamelijk zonder gras, ook stukken met fiets
        Location gym_d("gym D",ch11cs);
        locations.push_back(gym_d);

        ch11cs = {paver_square_dull};//af en toe ook paver_brick_grey_v, voornamelijk opritten
        Location stdenijs("St Denijs",ch11cs);
        locations.push_back(stdenijs);

        ch11cs = {zebra_crossing, asphalt};
        Location zebra("gym C",ch11cs);
        locations.push_back(zebra);

        ch11cs = {white_train_station};
        Location railw_white("werken station",ch11cs);
        locations.push_back(railw_white);

        ch11cs = {paver_train_station};
        Location railw_hall("Stationshal",ch11cs);
        locations.push_back(railw_hall);
    }

public:
    LocationRepository(){init();}
    std::vector<Location>& getAllLocations(){
        return locations;
    }
};

class CharacteristicTree {
protected:
    class CharacteristicNode{
    protected:
        Characteristic characteristic;
        std::vector<Location*> possibleLocations;
        CharacteristicNode* left;
        CharacteristicNode* right;
    public:
        CharacteristicNode(const Characteristic& _characteristic,const std::vector<Location*>& _possibleLocations)
            :characteristic(_characteristic),possibleLocations(_possibleLocations){}
        CharacteristicNode** getLeftChild(){return &left;}
        void SetLeftChild(CharacteristicNode * n){this->left = n;}
        CharacteristicNode** getRightChild(){return &right;}
        void SetRightChild(CharacteristicNode * n){this->right = n;}
        const Characteristic& getCharacteristic(){return characteristic;}
        const std::vector<Location*>& getPossibleLocations() const{return possibleLocations;}
    };
    std::list<Characteristic> characteristicPool;
    CharacteristicNode* root;
    CharacteristicNode* current;
    void traverseWithPool();
public:
    void refine(Characteristic& characteristic);
    void addBreadthFirst(const Characteristic& characteristic, const std::vector<Location*>& possibleLocations);
    void printBreadthFirst();
    const std::vector<Location*>* getPossibleLocations() const{return current == nullptr? nullptr : &current->getPossibleLocations();}
};

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
    cv::Mat mat = image.getMat();
    int rows = ceil((double)mat.rows/elementHeight);
    int cols = ceil((double)mat.cols/elementWidth);
    //init vectors
    elements.resize(rows);
    for(int row = 0; row<rows; row++){
        elements[row].resize(cols);
    }
    std::cout<<"Populating "<<rows<<"x"<<cols<<" grid..."<<std::endl;
    int count = 0; //debugging
    cv::Rect window = cv::Rect(0, 0, elementWidth, elementHeight);
    for(int row = 0; row<rows-1; row++){
        for(int col = 0; col<cols-1; col++){
            cv::Mat ROI(mat,window);
            GridElement element(ROI);
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
        GridElement element(ROI);
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
        GridElement element(ROI);
        elements[rows-1][col] = element;
        window.x += window.width;
        count++;
    }
    //final element can have a different width and height
    window.width = remainingX;
    cv::Mat ROI(mat,window);
    GridElement element(ROI);
    elements[rows-1][cols-1] = element;
    count++;

    std::cout<<"Done! Number of grid elements:"<<count<<std::endl;
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

#endif
