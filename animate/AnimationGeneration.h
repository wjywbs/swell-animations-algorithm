#ifndef ANIMATIONGENERATION_H
#define ANIMATIONGENERATION_H

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string.h>
#include <vector>
#include "AnimationDataSerializer/modeldata.pb.h"

class AnimationGeneration
{
    public:
        Animation* getFrames(ModelData* modelData);
    private:
        vector<double> getSpline(ModelData* modelData);
        Animation* evaluateDLOA(ModelData* modelData);
        double calculateB(ModelData* modelData);
};


#endif

