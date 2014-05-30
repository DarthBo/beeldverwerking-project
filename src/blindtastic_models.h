#ifndef BLINDTASTIC_MODELS_H
#define BLINDTASTIC_MODELS_H

#include <vector>

#include "blindtastic_core.h"
#include "svm_features.h"

#define CHARINSERT(x) mCharacteristics.insert(std::pair<featureCallback, const CharacteristicDefinition*>(x.getFeature(),&x))

class ModelRepository
{
protected:
    std::multimap<featureCallback, const CharacteristicDefinition*> mCharacteristics;
    constexpr static double asphalt_rows = 9;
    constexpr static double asphalt_cols = 9;
    constexpr static double asphalt_ratio = 5/(asphalt_rows*asphalt_cols);

    const CharacteristicDefinition grass_full;
    const LeftHalfCharacteristicDefinition grass_left;
    const RightHalfCharacteristicDefinition grass_right;
    const CharacteristicDefinition asphalt;
    const CharacteristicDefinition brick_pavers_vertical;
    const CharacteristicDefinition brick_pavers_horizontal;
    const CharacteristicDefinition big_square_pebbled_pavers;
    const CharacteristicDefinition fence_station;
    const CharacteristicDefinition square_pavers_sidewalk;
    const CharacteristicDefinition square_pavers_crossroads;
public:
    ModelRepository():
        grass_full("Grass (full frame)","../model/gras.model",&getTextnHSVColour,9,9),
        grass_left("Grass (left)","../model/gras.model",&getTextnHSVColour,9,9),
        grass_right("Grass (right)","../model/gras.model",&getTextnHSVColour,9,9),
        asphalt("Asphalt", "../model/asphalt.model",&getTextnHSVColour, asphalt_rows,asphalt_cols,asphalt_ratio),
        brick_pavers_vertical("Brick pavers (vertical)","../model/tegels1_sporthal.model",&getRectFeatures,1,1,0),
        brick_pavers_horizontal("Brick pavers (horizontal)","../model/tegels2_sporthal.model",&getRectFeatures,1,1,0),
        big_square_pebbled_pavers("Big square pebbled pavers","../model/tegelsGroot_sporthal.model",&getRectFeatures,1,1,0),
        fence_station("Fence station","../model/hek_station.model",&getRectFeatures,1,1,0),
        square_pavers_sidewalk("Square pavers sidewalk","../model/tegel_Denijs.model",&getRectFeatures,1,1,0),
        square_pavers_crossroads("Square pavers crossroads","../model/tegelx_Denijs.model",&getRectFeatures,1,1,0)
    {
        // features <-> characteristics
        //CHARINSERT(grass_full);
        CHARINSERT(grass_left);
        CHARINSERT(grass_right);
        CHARINSERT(asphalt);
        CHARINSERT(brick_pavers_vertical);
        CHARINSERT(brick_pavers_horizontal);
        CHARINSERT(big_square_pebbled_pavers);
        CHARINSERT(fence_station);
        CHARINSERT(square_pavers_sidewalk);
        CHARINSERT(square_pavers_crossroads);
    }

    const std::multimap<featureCallback, const CharacteristicDefinition*>& getCharacteristics() const{return mCharacteristics;}
    const CharacteristicDefinition& getGrassFullCharDef() const{return grass_full;}
    const CharacteristicDefinition& getGrassLeftCharDef() const{return grass_left;}
    const CharacteristicDefinition& getGrassRightCharDef() const{return grass_right;}
    const CharacteristicDefinition& getAsphaltCharDef() const{return asphalt;}
    const CharacteristicDefinition& getBrickPaversVerticalCharDef() const{return brick_pavers_vertical;}
    const CharacteristicDefinition& getBrickPaversHorizontalCharDef() const{return brick_pavers_horizontal;}
    const CharacteristicDefinition& getBigSquarePebbledPaversCharDef() const{return big_square_pebbled_pavers;}
    const CharacteristicDefinition& getFenceStationCharDef() const{return fence_station;}
    const CharacteristicDefinition& getSquarePaversSidewalkCharDef() const{return square_pavers_sidewalk;}
    const CharacteristicDefinition& getSquarePaversCrossroadsCharDef() const{return square_pavers_crossroads;}

};

class LocationRepository {
private:
    class WeightedLocation{
    private:
        Location* location;
        double weight;
    public:
        WeightedLocation(){}
        WeightedLocation(Location* _location,double _weight):location(_location),weight(_weight){}
        bool operator>(const WeightedLocation& l) const{return weight>l.weight;}
        bool operator<(const WeightedLocation& l) const{return weight<l.weight;}
        WeightedLocation& operator+(const double weight){this->weight += weight; return *this;}
        double getWeight(){return weight;}
        void setWeight(double weight){this->weight = weight;}
        Location* getLocation(){return location;}
        void setLocation(Location* location){this->location = location;}
    };
    std::vector<Location> locations;
    PairingHeap<WeightedLocation> refinedLocations;
    std::unordered_map<std::string,std::vector<Location*>> locationIndex;
    std::unordered_map<std::string,std::vector<typename PairingHeap<WeightedLocation>::Node*>> nodeIndex;
    void init(){
        ModelRepository models;
        std::vector<CharacteristicDefinition> ch11cs = {models.getGrassRightCharDef(),models.getAsphaltCharDef()};
        Location PGebouw("P gebouw tot modderpad",ch11cs);
        locations.push_back(PGebouw);

        ch11cs = {models.getGrassLeftCharDef(),models.getGrassRightCharDef()};
        Location modderpad("Modderpad tot sporthal",ch11cs);
        locations.push_back(modderpad);

        ch11cs = {models.getGrassLeftCharDef(),models.getBrickPaversHorizontalCharDef(),
                 models.getBrickPaversVerticalCharDef()};
        Location sporthal("Sporthal tot straat",ch11cs);
        locations.push_back(sporthal);

        ch11cs = { models.getGrassLeftCharDef(), models.getBigSquarePebbledPaversCharDef() };
        Location sporthal_pebble("Sporthal (grote witte stenen)",ch11cs);
        locations.push_back(sporthal_pebble);

        ch11cs = {models.getAsphaltCharDef(),models.getSquarePaversSidewalkCharDef()};
        Location sintDenijsStraat("Sint-Denijs tot kruispunt",ch11cs);
        locations.push_back(sintDenijsStraat);

        ch11cs = {models.getAsphaltCharDef(),models.getSquarePaversCrossroadsCharDef()};
        Location sintDenijsKruispunt("Kruispunt",ch11cs);
        locations.push_back(sintDenijsKruispunt);

        ch11cs = {models.getFenceStationCharDef()};
        Location station("Station",ch11cs);
        locations.push_back(station);

        resetRefinement();
        buildIndex();
    }
    void buildIndex(){
        for(Location& l : locations){
            for(const CharacteristicDefinition& c : l.getCharacteristics()){
                locationIndex[c.getName()].push_back(&l);
            }
        }
    }

public:
    LocationRepository(){init();}
    std::vector<Location>& getAllLocations(){
        return locations;
    }

    std::pair<Location*,double> getTopLocation(){
        WeightedLocation wl = refinedLocations.top();
        std::pair<Location*,double> p(wl.getLocation(),wl.getWeight());
        return p;
    }

    //warning: resets current refinement
    std::vector<std::pair<Location*,double>> getRefinedLocations(){
        std::vector<std::pair<Location*,double>> out;
        while(refinedLocations.size() > 0){
            WeightedLocation wl= refinedLocations.top();
            std::pair<Location*,double> p(wl.getLocation(),wl.getWeight());
            out.push_back(p);
            refinedLocations.pop();
        }
       resetRefinement();
    return out;
    }

    void refine(const CharacteristicValue& characteristic){
        if(nodeIndex.find(characteristic.definition->getName()) != nodeIndex.end()){
            for(typename PairingHeap<WeightedLocation>::Node* node : nodeIndex[characteristic.definition->getName()]){
                refinedLocations.increasePriority(node,characteristic.weight);
            }
        }
    }

void resetRefinement(){
    refinedLocations = PairingHeap<WeightedLocation>();
    nodeIndex.clear();
    for(Location& l : locations){
        WeightedLocation wl(&l,0.0);
        typename PairingHeap<WeightedLocation>::Node* n = refinedLocations.push(wl);
        for(const CharacteristicDefinition& c : l.getCharacteristics()){
            nodeIndex[c.getName()].push_back(n);
        }
    }
}
};

void play_classify(const char* fvid, int once_every_x_frames=1, int reset_location_every_x_frames=20, bool reset_on_skip=true);
void play_classify_mt(const char* fvid, int reset_location_every_x_frames=20, bool reset_on_skip=true);

#endif
