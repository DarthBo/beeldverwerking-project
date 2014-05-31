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

    //const CharacteristicDefinition grass_full;
    const FakeCharacteristicDefinition grass_none;
    const FakeCharacteristicDefinition grass_left;
    const FakeCharacteristicDefinition grass_right;
    const FakeCharacteristicDefinition grass_leftright;

    const LRHelperCharacteristicDefinition grass_helper;
    const CharacteristicDefinition asphalt;
    const CharacteristicDefinition brick_pavers_vertical;
    const CharacteristicDefinition brick_pavers_horizontal;
    const CharacteristicDefinition big_square_pebbled_pavers;
    const CharacteristicDefinition fence_station;
    const CharacteristicDefinition square_pavers_sidewalk;
    const CharacteristicDefinition square_pavers_crossroads;
public:
    ModelRepository():
        grass_none("No Grass","../model/gras.model",&getTextnHSVColour,9,9),
        grass_left("Grass (left)","../model/gras.model",&getTextnHSVColour,9,9),
        grass_right("Grass (right)","../model/gras.model",&getTextnHSVColour,9,9),
        grass_leftright("Grass (left+right)","../model/gras.model",&getTextnHSVColour,9,9),
        grass_helper(&grass_none,&grass_left,&grass_right,&grass_leftright,
                     "-- dont't use --","../model/gras.model",&getTextnHSVColour,9,9),
        asphalt("Asphalt", "../model/asphalt.model",&getTextnHSVColour, asphalt_rows,asphalt_cols,asphalt_ratio),
        brick_pavers_vertical("Brick pavers (vertical)","../model/tegels1_sporthal.model",&getRectFeatures,1,1,0),
        brick_pavers_horizontal("Brick pavers (horizontal)","../model/tegels2_sporthal.model",&getRectFeatures,1,1,0),
        big_square_pebbled_pavers("Big square pebbled pavers","../model/tegelsGroot_sporthal.model",&getRectFeatures,1,1,0),
        fence_station("Fence station","../model/hek_station.model",&getRectFeatures,1,1,0),
        square_pavers_sidewalk("Square pavers sidewalk","../model/tegel_Denijs.model",&getRectFeatures,1,1,0),
        square_pavers_crossroads("Square pavers crossroads","../model/tegelx_Denijs.model",&getRectFeatures,1,1,0)
    {
        // features <-> characteristics
        CHARINSERT(grass_helper); //don't insert the ACTUAL grass definitions
        CHARINSERT(asphalt);
        CHARINSERT(brick_pavers_vertical);
        CHARINSERT(brick_pavers_horizontal);
        CHARINSERT(big_square_pebbled_pavers);
        CHARINSERT(fence_station);
        CHARINSERT(square_pavers_sidewalk);
        CHARINSERT(square_pavers_crossroads);
    }

    const std::multimap<featureCallback, const CharacteristicDefinition*>& getCharacteristics() const{return mCharacteristics;}
    const CharacteristicDefinition& getNoGrassCharDef() const{return grass_none;}
    const CharacteristicDefinition& getGrassLeftCharDef() const{return grass_left;}
    const CharacteristicDefinition& getGrassRightCharDef() const{return grass_right;}
    const CharacteristicDefinition& getGrassLeftRightCharDef() const{return grass_leftright;}
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
        double acceptableWeight; // minimum weight to accept this location
        Location* location;
        double weight;
        std::vector<WeightedLocation*> possibleNextLocations; // simple graph representation
    public:
        WeightedLocation(){}
        WeightedLocation(int _id, double _acceptableWeight, Location* _location,double _weight)
            :id(_id),acceptableWeight(_acceptableWeight),location(_location),weight(_weight){}
        bool operator>(const WeightedLocation& l) const{return weight>l.weight;}
        bool operator<(const WeightedLocation& l) const{return weight<l.weight;}
        int getId() const{return id;}
        void setId(int id){this->id = id;}
        WeightedLocation& operator+(const double weight){this->weight += weight; return *this;}
        double getWeight() const{return weight;}
        void setWeight(double weight){this->weight = weight;}
        double getAcceptableWeight() const{return acceptableWeight;}
        Location* getLocation() const{return location;}
        void setLocation(Location* location){this->location = location;}
        void addPossibleNextLocation(WeightedLocation* l){possibleNextLocations.push_back(l);}
        const std::vector<WeightedLocation*> getPossibleNextLocations() const{return possibleNextLocations;}
    };
    constexpr static double defaultMinimumWeight = 6.0;
    bool useRelativeWeights;
    bool ignoreCharacteristicWhenUnreachable;
    const WeightedLocation* referenceLocation;
    std::vector<Location*> locations;
    std::vector<WeightedLocation*> defaultWeightedLocations;
    std::vector<WeightedLocation*> weightedLocations;
    PairingHeap<WeightedLocation> refinedLocations;
    std::unordered_map<std::string,double> relativeCharacteristicImportance;
    std::unordered_map<std::string,std::vector<Location*>> locationIndex;
    std::unordered_map<std::string,std::vector<typename PairingHeap<WeightedLocation>::Node*>> nodeIndex;
    void init(){
        ModelRepository models;
        std::vector<CharacteristicDefinition> ch11cs = {models.getGrassRightCharDef(),models.getAsphaltCharDef()};
        Location* PGebouw = new Location("P gebouw tot grindpad",ch11cs);
        locations.push_back(PGebouw);
        WeightedLocation* wPGebouw = new WeightedLocation(0,INT_MAX,PGebouw,0.0);
        defaultWeightedLocations.push_back(wPGebouw);

        ch11cs = {models.getGrassLeftRightCharDef()};
        Location* modderpad = new Location("Grindpad tot sporthal",ch11cs);
        locations.push_back(modderpad);
        WeightedLocation* wModderpad = new WeightedLocation(1,7,modderpad,0.0);
        defaultWeightedLocations.push_back(wModderpad);

        ch11cs = {models.getGrassLeftCharDef(),models.getBrickPaversVerticalCharDef(),models.getBrickPaversHorizontalCharDef()};
        Location* sporthal_v = new Location("Sporthal",ch11cs);
        locations.push_back(sporthal_v);
        WeightedLocation* wSporthal_v = new WeightedLocation(2,8,sporthal_v,0.0);
        defaultWeightedLocations.push_back(wSporthal_v);

        /*ch11cs = {models.getGrassLeftCharDef(),models.getBrickPaversHorizontalCharDef()};
        Location* sporthal_h = new Location("Sporthal (horizontale stenen)",ch11cs);
        locations.push_back(sporthal_h);
        WeightedLocation* wSporthal_h = new WeightedLocation(3,2.5,sporthal_h,00.0);
        defaultWeightedLocations.push_back(wSporthal_h);*/

        ch11cs = { models.getGrassLeftCharDef(), models.getBigSquarePebbledPaversCharDef() };
        Location* sporthal_pebble = new Location("Sporthal (grote witte stenen)",ch11cs);
        locations.push_back(sporthal_pebble);
        WeightedLocation* wSporthal_pebble = new WeightedLocation(3,5,sporthal_pebble,0.0);
        defaultWeightedLocations.push_back(wSporthal_pebble);

        ch11cs = { models.getSquarePaversSidewalkCharDef(), models.getGrassLeftCharDef() };
        Location* sportdenijs = new Location("Sporthal tot St Denijs", ch11cs);
        locations.push_back(sportdenijs);
        WeightedLocation* wSportdenijs = new WeightedLocation(4,defaultMinimumWeight,sportdenijs,0.0);
        defaultWeightedLocations.push_back(wSportdenijs);

        ch11cs = { models.getSquarePaversSidewalkCharDef(), models.getGrassLeftRightCharDef() };
        Location* sportdenijs_2 = new Location("Sporthal (voorbij fietsen)", ch11cs);
        locations.push_back(sportdenijs_2);
        WeightedLocation* wSportdenijs_2 = new WeightedLocation(5,defaultMinimumWeight,sportdenijs_2,0.0);
        defaultWeightedLocations.push_back(wSportdenijs_2);

        /*ch11cs = { models.getSquarePaversSidewalkCharDef(), models.getGrassLeftCharDef() };
        Location* sportdenijs_3 = new Location("Sporthal tot St Denijs (struiken)", ch11cs);
        locations.push_back(sportdenijs_3);
        WeightedLocation* wSportdenijs_3 = new WeightedLocation(6,defaultMinimumWeight,sportdenijs_3,0.0);
        defaultWeightedLocations.push_back(wSportdenijs_3);*/

        ch11cs = {models.getAsphaltCharDef(),models.getSquarePaversSidewalkCharDef(), models.getBrickPaversHorizontalCharDef(),models.getBrickPaversVerticalCharDef(),models.getNoGrassCharDef()};
        Location* sintDenijsStraat = new Location("Sint-Denijs tot kruispunt",ch11cs);
        locations.push_back(sintDenijsStraat);
        WeightedLocation* wSintDenijsStraat = new WeightedLocation(6,defaultMinimumWeight,sintDenijsStraat,0.0);
        defaultWeightedLocations.push_back(wSintDenijsStraat);

        ch11cs = {models.getAsphaltCharDef(),models.getSquarePaversCrossroadsCharDef(), models.getNoGrassCharDef()};
        Location* sintDenijsKruispunt = new Location("Kruispunt",ch11cs);
        locations.push_back(sintDenijsKruispunt);
        WeightedLocation* wSintDenijsKruispunt = new WeightedLocation(7,defaultMinimumWeight,sintDenijsKruispunt,0.0);
        defaultWeightedLocations.push_back(wSintDenijsKruispunt);

        ch11cs = {models.getFenceStationCharDef(), models.getNoGrassCharDef()};
        Location* station = new Location("Station",ch11cs);
        locations.push_back(station);
        WeightedLocation* wStation = new WeightedLocation(8,defaultMinimumWeight,station,0.0);
        defaultWeightedLocations.push_back(wStation);

        resetRefinement(false);
        if(useRelativeWeights){
            calculateRelativeCharacteristicImportance();
        }
        buildIndex();
        if(ignoreCharacteristicWhenUnreachable && !weightedLocations.empty()){
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
    // Assumes lineair path
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
    bool characteristicReachable(const CharacteristicValue& characteristic,bool backtrack = true){ // naïve implementation with single hop
        if(referenceLocation == nullptr)
            return true;
        for(const WeightedLocation* location : referenceLocation->getPossibleNextLocations()){
            if(!backtrack && location->getId() < referenceLocation->getId())
                continue;
            for(const CharacteristicDefinition &c : location->getLocation()->getCharacteristics()){
                if( c.getName() == characteristic.definition->getName())
                    return true;
            }
        }
        return false;
    }
    bool locationReachable(const WeightedLocation* wLocation, bool backtrack = false){
        if(referenceLocation == nullptr)
            return true;
        for(const WeightedLocation* location : referenceLocation->getPossibleNextLocations()){
            if(!backtrack && location->getId() < referenceLocation->getId())
                continue;
            if(location->getId() == wLocation->getId())
                return true;
        }
        return false;
    }
    void calculateRelativeCharacteristicImportance(){
        for(const WeightedLocation* location:defaultWeightedLocations){
            for(const CharacteristicDefinition &characteristic : location->getLocation()->getCharacteristics()){
                relativeCharacteristicImportance[characteristic.getName()]++;
            }
        }
        for(auto it = relativeCharacteristicImportance.begin(); it != relativeCharacteristicImportance.end();it++){
            relativeCharacteristicImportance[it->first] = defaultWeightedLocations.size() / it->second;
        }
    }

public:
    LocationRepository(bool _useRelativeWeights = true, bool _ignoreCharacteristicWhenUnreachable = true)
        :useRelativeWeights(_useRelativeWeights),ignoreCharacteristicWhenUnreachable(_ignoreCharacteristicWhenUnreachable){
        init();
    }
    ~LocationRepository(){
        for(Location* location : locations) delete location;
        for(WeightedLocation* defaultWeightedLocation : defaultWeightedLocations) delete defaultWeightedLocation;
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

    //warning: resets current refinement, saves location when ignoring unreachable locations
    std::vector<std::pair<Location*,double>> getRefinedLocations(){
        std::vector<std::pair<Location*,double>> out;
        while(refinedLocations.size() > 0){
            WeightedLocation wl= refinedLocations.top();
            std::pair<Location*,double> p(wl.getLocation(),wl.getWeight());
            out.push_back(p);
            refinedLocations.pop();
        }
        resetRefinement(ignoreCharacteristicWhenUnreachable);
        return out;
    }

    void refine(const CharacteristicValue& characteristic){
        double weight = useRelativeWeights?
                characteristic.weight* relativeCharacteristicImportance[characteristic.definition->getName()]
                : characteristic.weight;
        bool charReachable = characteristicReachable(characteristic);
        if(ignoreCharacteristicWhenUnreachable && !charReachable)
            return;
        if(nodeIndex.find(characteristic.definition->getName()) != nodeIndex.end()){
            for(typename PairingHeap<WeightedLocation>::Node* node : nodeIndex[characteristic.definition->getName()]){
                refinedLocations.increasePriority(node,weight);
            }
        }
        //ADD THIS TO ONLY CHANGE LOCATIONS WHEN A MIN WEIGHT IS REACHED:
        // && refinedLocations.top().getWeight() >= refinedLocations.top().getAcceptableWeight()
        if(ignoreCharacteristicWhenUnreachable && charReachable
           && locationReachable(weightedLocations[refinedLocations.top().getId()])){
            referenceLocation =  weightedLocations[refinedLocations.top().getId()];
        }

    }
    //saves current location when true and ignoring unreachable locations, assumes non empty locations
    void resetRefinement(bool saveCurrentLocation){
        WeightedLocation previousReference;
        if(saveCurrentLocation)
            previousReference = *referenceLocation;
        refinedLocations = PairingHeap<WeightedLocation>();
        for(WeightedLocation* wLocation: weightedLocations) delete wLocation;
        weightedLocations.clear();
        nodeIndex.clear();
        int idCount = 0;
        for(Location* l : locations){
            WeightedLocation* wl = new WeightedLocation(*defaultWeightedLocations[idCount]);
            idCount++;
            weightedLocations.push_back(wl);
            if(saveCurrentLocation && wl->getId() == previousReference.getId())
                referenceLocation = wl;
            typename PairingHeap<WeightedLocation>::Node* n = refinedLocations.push(*wl);
            for(const CharacteristicDefinition& c : l->getCharacteristics()){
                nodeIndex[c.getName()].push_back(n);
            }
        }
        if(ignoreCharacteristicWhenUnreachable)
            linkLocations();
    }
};

void play_classify(const char* fvid, int once_every_x_frames=1, int reset_location_every_x_frames=5, bool reset_on_skip=true);
void play_classify_mt(const char* fvid, int reset_location_every_x_frames=5, bool reset_on_skip=true);

#endif
