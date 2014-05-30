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
        int id; // prevents comparison of strings
        Location* location;
        double weight;
        std::vector<WeightedLocation*> possibleNextLocations; // simple graph representation
    public:
        WeightedLocation(){}
        WeightedLocation(int _id, Location* _location,double _weight):id(_id),location(_location),weight(_weight){}
        bool operator>(const WeightedLocation& l) const{return weight>l.weight;}
        bool operator<(const WeightedLocation& l) const{return weight<l.weight;}
        int getId() const{return id;}
        void setId(int id){this->id = id;}
        WeightedLocation& operator+(const double weight){this->weight += weight; return *this;}
        double getWeight(){return weight;}
        void setWeight(double weight){this->weight = weight;}
        Location* getLocation(){return location;}
        void setLocation(Location* location){this->location = location;}
        void addPossibleNextLocation(WeightedLocation* l){possibleNextLocations.push_back(l);}
        const std::vector<WeightedLocation*> getPossibleNextLocations(){return possibleNextLocations;}
    };
    bool ignoreCharacteristicWhenUnreachable;
    WeightedLocation* referenceLocation;
    std::vector<Location*> locations;
    std::vector<WeightedLocation*> weightedLocations;
    PairingHeap<WeightedLocation> refinedLocations;
    std::unordered_map<std::string,std::vector<Location*>> locationIndex;
    std::unordered_map<std::string,std::vector<typename PairingHeap<WeightedLocation>::Node*>> nodeIndex;
    void init(){
        ModelRepository models;
        std::vector<CharacteristicDefinition> ch11cs = {models.getGrassRightCharDef(),models.getAsphaltCharDef()};
        Location* PGebouw = new Location("P gebouw tot modderpad",ch11cs);
        locations.push_back(PGebouw);

        ch11cs = {models.getGrassLeftCharDef(),models.getGrassRightCharDef()};
        Location* modderpad = new Location("Modderpad tot sporthal",ch11cs);
        locations.push_back(modderpad);

        ch11cs = {models.getGrassLeftCharDef(),models.getBrickPaversHorizontalCharDef(),
                 models.getBrickPaversVerticalCharDef()};
        Location* sporthal = new Location("Sporthal tot straat",ch11cs);
        locations.push_back(sporthal);

        ch11cs = { models.getGrassLeftCharDef(), models.getBigSquarePebbledPaversCharDef() };
        Location* sporthal_pebble = new Location("Sporthal (grote witte stenen)",ch11cs);
        locations.push_back(sporthal_pebble);

        ch11cs = { models.getSquarePaversSidewalkCharDef(), models.getGrassLeftCharDef() };
        Location sportdenijs("Sporthal tot St Denijs", ch11cs);
        locations.push_back(sportdenijs);

        ch11cs = { models.getSquarePaversSidewalkCharDef(), models.getGrassLeftCharDef(), models.getGrassRightCharDef() };
        Location sportdenijs_2("Sporthal tot St Denijs (LR gras)", ch11cs);
        locations.push_back(sportdenijs_2);

        ch11cs = {models.getAsphaltCharDef(),models.getSquarePaversSidewalkCharDef()};
        Location* sintDenijsStraat = new Location("Sint-Denijs tot kruispunt",ch11cs);
        locations.push_back(sintDenijsStraat);

        ch11cs = {models.getAsphaltCharDef(),models.getSquarePaversCrossroadsCharDef()};
        Location* sintDenijsKruispunt = new Location("Kruispunt",ch11cs);
        locations.push_back(sintDenijsKruispunt);

        ch11cs = {models.getFenceStationCharDef()};
        Location* station = new Location("Station",ch11cs);
        locations.push_back(station);

        resetRefinement();
        buildIndex();
        if(ignoreCharacteristicWhenUnreachable){
            linkLocations();
            if(!weightedLocations.empty())
                referenceLocation = weightedLocations[0];
        }
    }
    void buildIndex(){
        for(Location* l : locations){
            for(const CharacteristicDefinition& c : l->getCharacteristics()){
                locationIndex[c.getName()].push_back(l);
            }
        }
    }

    void linkLocations(){
        if(weightedLocations.empty())
            return;
        WeightedLocation* current = weightedLocations[0];
        current->addPossibleNextLocation(current);
        if(weightedLocations.size() > 1){
            current->addPossibleNextLocation(weightedLocations[1]);
            size_t i = 1;
            for(; i<weightedLocations.size()-1; i++){
                current = weightedLocations[i];
                current->addPossibleNextLocation(current);
                current->addPossibleNextLocation(weightedLocations[i-1]);
                current->addPossibleNextLocation(weightedLocations[i+1]);
            }
            current = weightedLocations[i];
            current->addPossibleNextLocation(current);
            current->addPossibleNextLocation(weightedLocations[i-1]);
        }
    }
    bool characteristicReachable(const CharacteristicValue& characteristic){ // naïve implementation with single hop
        if(referenceLocation == nullptr)
            return true;
        for(WeightedLocation* location : referenceLocation->getPossibleNextLocations()){
            for(const CharacteristicDefinition &c : location->getLocation()->getCharacteristics()){
                if( c.getName() == characteristic.definition->getName())
                    return true;
            }
        }
        return false;
    }

public:
    LocationRepository( bool _ignoreCharacteristicWhenUnreachable = false)
        :ignoreCharacteristicWhenUnreachable(_ignoreCharacteristicWhenUnreachable){
        init();
    }
    ~LocationRepository(){
        for(Location* location : locations) delete location;
        for(WeightedLocation* wLocation: weightedLocations) delete wLocation;
    }
    std::vector<Location*>& getAllLocations(){
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
        if(ignoreCharacteristicWhenUnreachable && !characteristicReachable(characteristic))
            return;

        if(nodeIndex.find(characteristic.definition->getName()) != nodeIndex.end()){
            for(typename PairingHeap<WeightedLocation>::Node* node : nodeIndex[characteristic.definition->getName()]){
                refinedLocations.increasePriority(node,characteristic.weight);
            }
        }
    }

void resetRefinement(){
    int idCount = 1;
    refinedLocations = PairingHeap<WeightedLocation>();
    nodeIndex.clear();
    for(Location* l : locations){
        WeightedLocation* wl = new WeightedLocation(idCount,l,0.0);
        weightedLocations.push_back(wl);
        idCount++;
        typename PairingHeap<WeightedLocation>::Node* n = refinedLocations.push(*wl);
        for(const CharacteristicDefinition& c : l->getCharacteristics()){
            nodeIndex[c.getName()].push_back(n);
        }
    }
}
};

void play_classify(const char* fvid, int once_every_x_frames=1, int reset_location_every_x_frames=20, bool reset_on_skip=true);
void play_classify_mt(const char* fvid, int reset_location_every_x_frames=20, bool reset_on_skip=true);

#endif
