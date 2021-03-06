#ifndef BLINDTASTIC_CORE_H
#define BLINDTASTIC_CORE_H

#include "opencv2/opencv.hpp"
#include <queue>
#include <unordered_map>
#include "svm_features.h"


struct CharacteristicValue;
class  CharacteristicDefinition {
protected:
    std::string name;
    std::string model;
    featureCallback feature;
    int rows;
    int columns;
    double req_ratio;
    constexpr static double default_ratio = 1.0/9;
public:
    CharacteristicDefinition(  const std::string& _name,
                               const std::string& _model,
                               featureCallback _feature,
                               int _rows=1,
                               int _columns=1,
                               double _req_ratio = default_ratio  )
        : name(_name), model(_model), feature(_feature), rows(_rows),
          columns(_columns), req_ratio(_req_ratio){}

    CharacteristicDefinition(const CharacteristicDefinition& ch)
        : name(ch.name), model(ch.model), feature(ch.feature), rows(ch.rows),
          columns(ch.columns), req_ratio(ch.req_ratio){}

    bool operator==(const CharacteristicDefinition& characteristic) const
    {
        return  name == characteristic.name &&
                model == characteristic.model &&
                feature == characteristic.feature &&
                rows == characteristic.rows &&
                columns == characteristic.columns &&
                req_ratio == characteristic.req_ratio;
    }
    bool operator!=(const CharacteristicDefinition& characteristic ) const
    {
        return !((*this).operator==(characteristic));
    }

    const std::string& getName() const {return name;}
    const std::string& getModel() const {return model;}
    featureCallback getFeature() const {return feature;}
    int getRows() const {return rows;}
    int getColumns() const {return columns;}
    double getRequiredRatio() const {return req_ratio;}

    virtual CharacteristicValue getValue(const cv::Mat& img, bool skip_datacalc = false) const;
};

class FakeCharacteristicDefinition : public CharacteristicDefinition
{
public:
    FakeCharacteristicDefinition(const std::string& _name)
        : CharacteristicDefinition(_name, "", NULL){}

    FakeCharacteristicDefinition(const FakeCharacteristicDefinition& ch)
        : CharacteristicDefinition(ch){}
    //seriously, don't use this
    CharacteristicValue getValue(const cv::Mat& img, bool skip_datacalc = false) const;
};

class LRHelperCharacteristicDefinition : public CharacteristicDefinition
{
private:
    const CharacteristicDefinition* def_none;
    const CharacteristicDefinition* def_left;
    const CharacteristicDefinition* def_right;
    const CharacteristicDefinition* def_leftright;
public:
    LRHelperCharacteristicDefinition(const FakeCharacteristicDefinition* _none,
                                  const FakeCharacteristicDefinition* _left,
                                  const FakeCharacteristicDefinition* _right,
                                  const FakeCharacteristicDefinition* _leftright,
                                  const std::string& _model,
                                  featureCallback _feature,
                                  int _rows=1,
                                  int _columns=1,
                                  double _req_ratio = default_ratio)
        : CharacteristicDefinition("<helper - don't show>", _model, _feature, _rows, _columns, _req_ratio),
          def_none(_none), def_left(_left), def_right(_right), def_leftright(_leftright) {}

    LRHelperCharacteristicDefinition(const LRHelperCharacteristicDefinition& ch)
        : CharacteristicDefinition(ch), def_none(ch.def_none), def_left(ch.def_left),
          def_right(ch.def_right), def_leftright(ch.def_leftright){}
    CharacteristicValue getValue(const cv::Mat& img, bool skip_datacalc = false) const;
};

struct CharacteristicValue
{
    const CharacteristicDefinition* definition;
    double weight;
};

class GridElement{
protected:
    cv::Mat element;
    //std::vector<Characteristic> characteristics;
    cv::Rect window;
public:
    GridElement(){}
    GridElement(cv::Mat _element, cv::Rect _window)
        : element(_element), window(_window) {}
    //const std::vector<Characteristic>& getCharacteristics() const{return characteristics;}
    //void setCharacteristics(const std::vector<Characteristic>& characteristics){this->characteristics = characteristics;}
    const cv::Mat& getMat() const{return element;}
    const cv::Rect& getWindow() const {return window;}
    void setMat(const cv::Mat& mat){this->element = mat;}
};

class Image{
protected:
    cv::Mat mat;
    //std::vector<Characteristic> characteristics;
public:
    Image(cv::Mat _image):mat(_image){}
    const cv::Mat& getMat() const{return mat;}
    void setMat(const cv::Mat& mat){this->mat = mat;}
    //const std::vector<Characteristic>& getCharacteristics() const{return characteristics;}
    //void setCharacteristics(const std::vector<Characteristic>& characteristics){this->characteristics = characteristics;}
};

class ImageGrid{
protected:
    Image image;
    int rows;
    int cols;
    std::vector<std::vector<GridElement>> elements; //[row][column]
    void populate();
    void test();
public:
    typedef std::vector<std::vector<GridElement>>::const_iterator const_it_row;
    typedef std::vector<GridElement>::const_iterator const_it_col;

    //Elements of final column and row may not have given dimensions if height and width cannot be properly divided
    ImageGrid(cv::Mat _image, cv::Rect celSize) : image(_image)
    {
        rows = ceil((double)image.getMat().rows/celSize.height);
        cols = ceil((double)image.getMat().cols/celSize.width);

        populate();
    }

    ImageGrid(cv::Mat _image, int _rows, int _cols) : image(_image), rows(_rows), cols(_cols)
    {
        populate();
    }

    const_it_row begin() const
    {
        return elements.begin();
    }

    const_it_row end() const
    {
        return elements.end();
    }


    void populateCharacteristics(){
        //move over grid and call SVM for every element to find the Characteristics
    }
};

class Location{
protected:
    std::string name;
    std::vector<CharacteristicDefinition> characteristics;
public:
    Location(){}
    Location(const std::string& _name,std::vector<CharacteristicDefinition> _characteristics):name(_name),characteristics(_characteristics){}
    const std::vector<CharacteristicDefinition>& getCharacteristics() const{return characteristics;}
    void setCharacteristics(const std::vector<CharacteristicDefinition>& characteristics){this->characteristics = characteristics;}
    const std::string& getName() const{return name;}
    void setName(const std::string& name){this->name = name;}
};

template <typename T>
class PairingHeap{
public:
    class Node{
    public:
        T t;
        Node* previous; // previous* of first in linked list points to the parent
        Node* next;     //next sibling in linked list
        Node* child;    //first child
        Node(const T& _t):t(_t),previous(nullptr),next(nullptr),child(nullptr){}
        ~Node(){
            if(next != nullptr)
                delete next;
            if(child != nullptr)
                delete child;
        }
    }; // end of Node interface
    PairingHeap():_size(0),root(nullptr){}
    ~PairingHeap(){
        delete root;
    }
    int size() const{return _size;}
    bool empty() const {return _size == 0;}
    Node* push(const T& t);
    void pop();
    const T& top() const;
    void increasePriority(Node* node, const double val);
private:
    int _size;
    Node* root;
    Node* combineSiblings(Node* first);
    void combine(Node*&,Node*);
};

/*PAIRINGHEAP */

template <typename T>
void PairingHeap<T>::combine(Node*& first,Node* second){
    if (second == nullptr)
        return;
    if (first->t < second->t)
    {
        second->previous = first->previous;
        first->previous = second;
        first->next = second->child;
        if (first->next != nullptr)
            first->next->previous = first;
        second->child = first;
        first = second;
    }
    else
    {
        second->previous = first;
        first->next = second->next;
        if (first->next != nullptr)
            first->next->previous = first;
        second->next = first->child;
        if (second->next != nullptr)
            second->next->previous = second;
        first->child = second;
    }
}

template <typename T>
typename PairingHeap<T>::Node* PairingHeap<T>::push(const T& t){
    Node *newNode = new Node(t);
    if (root == nullptr)
        root = newNode;
    else
        combine(root, newNode);
    _size++;
    return newNode;
}

template <typename T>
void PairingHeap<T>::increasePriority(Node* node,const double val){
        node->t = node->t + val;
        if (node != root)
        {
            if (node->next != nullptr)
                node->next->previous = node->previous;
            if (node->previous->child == node)
                node->previous->child = node->next;
            else
                node->previous->next = node->next;
            node->next = nullptr;
            combine(root, node);
        }
}

template <typename T>
const T& PairingHeap<T>::top() const{
    return root->t;
}

template <typename T>
typename PairingHeap<T>::Node* PairingHeap<T>::combineSiblings(Node* first){
    if(first == nullptr)
        return nullptr;
    if(first->next == nullptr)
        return first;
    std::vector<Node*> subheaps;
    //first pass combines in pairs creating separate heaps
    Node* second = first->next;
    while(first != nullptr){
        Node* nextFirst = nullptr;
        Node* nextSecond = nullptr;
        first->next = nullptr; // decouple
        if(second != nullptr){
            nextFirst = second->next;
            if(nextFirst != nullptr)
                nextSecond = nextFirst->next;
            second->next = nullptr;
        }
        combine(first,second);
        subheaps.push_back(first);
        first = nextFirst;
        second = nextSecond;
    }
    //second pass combines in 1 tree
    second = subheaps.back();
    if(subheaps.size() > 1){
        for(int i = subheaps.size()-2; i >= 0; i--){
            Node* first = subheaps[i];
            combine(second,first);
        }
    }
    return second;
}

template <typename T>
void PairingHeap<T>::pop(){
    //delete current root and merge siblings
    if(root == nullptr)
        return;
    Node *oldRoot = root;
    if (root->child == nullptr)
        root = nullptr;
    else
        root = combineSiblings(root->child);
    if(root != nullptr)
        root->previous = nullptr; // cleaning up previous links
    oldRoot->child = nullptr;
    delete oldRoot;
    _size--;
}



#endif
