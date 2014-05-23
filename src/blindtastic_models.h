#ifndef BLINDTASTIC_MODELS_H
#define BLINDTASTIC_MODELS_H

#include <vector>

#include "blindtastic_core.h"
#include "svm_features.h"

#define CHARINSERT(x,y) mCharacteristics.insert(std::pair<featureCallback, CharacteristicDefinition>(x,y))

class models
{
public:
    std::multimap<featureCallback, CharacteristicDefinition> mCharacteristics;

    models()
    {
        //characteristics
        CHARINSERT(&getTextnHSVColour, CharacteristicDefinition("Grass (full frame)",
                                                                "../model/gras.model",
                                                                &getTextnHSVColour,
                                                                9,9));
        CHARINSERT(&getTextnHSVColour, CharacteristicDefinition("Asphalt",
                                                                "../model/asphalt.model",
                                                                &getTextnHSVColour,
                                                                9,9));
        CHARINSERT(&getRectFeatures, CharacteristicDefinition("Brick pavers (vertical)",
                                                              "../model/tegels1_sporthal.model",
                                                              &getRectFeatures,
                                                              1,1,0));
        CHARINSERT(&getRectFeatures, CharacteristicDefinition("Brick pavers (horizontal)",
                                                              "../model/tegels2_sporthal.model",
                                                              &getRectFeatures,
                                                              1,1,0));
        CHARINSERT(&getRectFeatures, CharacteristicDefinition("Big square pebbled pavers",
                                                              "../model/tegelsGroot_sporthal.model",
                                                              &getRectFeatures,
                                                              1,1,0));
    }

};

void play_classify(const char* fvid, int once_every_x_frames=1);

#endif
