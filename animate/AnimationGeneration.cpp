// Backend Animation class
// Author: Sarah
// Date late updated: 03/05/2016
//

#include <iostream>
#include <vector>
#include "AnimationGeneration.h"

using namespace swellanimations;
using namespace std;

AnimationGeneration::AnimationGeneration() { }

// returns an Animation object to send back to Unity
// TODO: get the spline to iterate
AnimationGeneration::Animation* getFrames(ModelData* modelData) {
	Node model = modelData->model();
	Animation* animation = new Animation();
    // call evaluateDLOA in loop for every generated control point in the spline
	for (int x = 0; x < modelData->controlpoints_size(); x++) {
		Node* node = animation->add_frames();
		node->CopyFrom(model);
		node->set_allocated_position(modelData->mutable_controlpoints(x));
	}
	return animation;
}

vector<double> AnimationGeneration::getSpline(ModelData* modelData) {
    for (int i = 0; i < modelData->controlpoints_size()+4; i++) {
        // TODO: call hermite for every 4 points to get spline
        modelData->mutable_controlpoints(i);
    }
    vector<double> v;
    return v
}

// returns a 1-frame animation of the model evaluated at a certain point along spline
// TODO going to need the time it takes the user to draw the LOA
Animation* AnimationGeneration::evaluateDLOA(ModelData* modelData) {
    Animation* animation = new Animation();
    return animation;
}

// get the constant b:
// the ratio between spline length and model length
// TODO
double AnimationGeneration::calculateB(ModelData* modelData) {
    
    return 0;
}
