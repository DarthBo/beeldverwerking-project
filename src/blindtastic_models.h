#ifndef BLINDTASTIC_MODELS_H
#define BLINDTASTIC_MODELS_H

#include <vector>

#include "blindtastic_core.h"
#include "svm_features.h"

#define CHARINSERT(x,y) mCharacteristics.insert(std::pair<featureCallback, CharacteristicDefinition>(x,y))

class ModelRepository
{
protected:
    std::multimap<featureCallback, CharacteristicDefinition> mCharacteristics;
    constexpr static double asphalt_rows = 9;
    constexpr static double asphalt_cols = 9;
    constexpr static double asphalt_ratio = 5/(asphalt_rows*asphalt_cols);

    const CharacteristicDefinition grass_full;
    const CharacteristicDefinition asphalt;
    const CharacteristicDefinition brick_pavers_vertical;
    const CharacteristicDefinition brick_pavers_horizontal;
    const CharacteristicDefinition big_square_pebbled_pavers;
    const CharacteristicDefinition fence_station;
    const CharacteristicDefinition square_pavers_sidewalk;
    const CharacteristicDefinition square_pavers_crossroads;
public:
    ModelRepository():grass_full("Grass (full frame)","../model/gras.model",&getTextnHSVColour,9,9),
        asphalt("Asphalt", "../model/asphalt.model",&getTextnHSVColour, asphalt_rows,asphalt_cols,asphalt_ratio),
        brick_pavers_vertical("Brick pavers (vertical)","../model/tegels1_sporthal.model",&getRectFeatures,1,1,0),
        brick_pavers_horizontal("Brick pavers (horizontal)","../model/tegels2_sporthal.model",&getRectFeatures,1,1,0),
        big_square_pebbled_pavers("Big square pebbled pavers","../model/tegelsGroot_sporthal.model",&getRectFeatures,1,1,0),
        fence_station("Fence station","../model/hek_station.model",&getRectFeatures,1,1,0),
        square_pavers_sidewalk("Square pavers sidewalk","../model/tegel_Denijs.model",&getRectFeatures,1,1,0),
        square_pavers_crossroads("Square pavers crossroads","../model/tegelx_Denijs.model",&getRectFeatures,1,1,0)
    {
        // features <-> characteristics
        CHARINSERT(&getTextnHSVColour, grass_full);
        CHARINSERT(&getTextnHSVColour, asphalt);
        CHARINSERT(&getRectFeatures, brick_pavers_vertical);
        CHARINSERT(&getRectFeatures, brick_pavers_horizontal);
        CHARINSERT(&getRectFeatures, big_square_pebbled_pavers);
        CHARINSERT(&getRectFeatures, fence_station);
        CHARINSERT(&getRectFeatures, square_pavers_sidewalk);
        CHARINSERT(&getRectFeatures, square_pavers_crossroads);
    }

    const std::multimap<featureCallback, CharacteristicDefinition>& getCharacteristics() const{return mCharacteristics;}
    const CharacteristicDefinition& getGrassFullCharDef() const{return grass_full;}
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
        */
        ModelRepository models;
        std::vector<CharacteristicDefinition> ch11cs = {models.getGrassFullCharDef(),models.getAsphaltCharDef()};
        Location PGebouw("P gebouw tot sporthal",ch11cs);
        locations.push_back(PGebouw);

        ch11cs = {models.getGrassFullCharDef(),models.getBigSquarePebbledPaversCharDef(),models.getBrickPaversHorizontalCharDef(),
                 models.getBrickPaversVerticalCharDef()};
        Location sporthal("Sporthal tot straat",ch11cs);
        locations.push_back(sporthal);

        ch11cs = {models.getAsphaltCharDef(),models.getSquarePaversSidewalkCharDef()};
        Location sintDenijsStraat("Sporthal tot kruispunt",ch11cs);
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

    void refine(CharacteristicValue& characteristic){
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

void play_classify(const char* fvid, int once_every_x_frames=1);

#endif
