// AnimationGeneration.cpp - backend animation generation functions
// Author: Sarah
// Date last updated: 03/05/2016
//

#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string.h>
#include <vector>
#include "AnimationDataSerializer/modeldata.pb.h"
#include "../point.h"

using namespace swellanimations;
using namespace std;

// returns an Animation object to send back to Unity
// TODO: get the spline to iterate
Animation* getFrames(ModelData* modelData) {
	Node model = modelData->model();
	Animation* animation = new Animation();
    // get hermite spline

    // call evaluateDLOA in loop for every generated control point in the spline
	for (int x = 0; x < modelData->controlpoints_size(); x++) {
		Node* node = animation->add_frames();
		node->CopyFrom(model);
		node->set_allocated_position(modelData->mutable_controlpoints(x));
	}
	return animation;
}

vector<struct pt> getSpline(ModelData* modelData) {
    for (int i = 0; i < modelData->controlpoints_size()+4; i++) {
        // TODO: call hermite for every 4 points to get spline
        modelData->mutable_controlpoints(i);
    }
    vector<struct pt> v;
    return v;
}

// returns a 1-frame animation of the model evaluated at a certain point along spline
// TODO going to need the time it takes the user to draw the LOA
Animation* evaluateDLOA(ModelData* modelData) {
    Animation* animation = new Animation();
    return animation;
}

// get the constant b:
// the ratio between spline length and model length
// TODO
double calculateB(ModelData* modelData) {
    
    return 0;
}
