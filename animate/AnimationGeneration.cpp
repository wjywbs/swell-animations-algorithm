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
#include "../hermite/hermite.cpp"

using namespace swellanimations;
using namespace std;

// uses hermite.cpp to calculate a spline based on control points
std::vector<struct pt*> getSpline(ModelData* modelData) {
    std::vector<struct pt*> v;
    for (int i = 0; i < modelData->controlpoints_size()-3; i+=3) {
        // point p0
        Vector* a = modelData->mutable_controlpoints(i);
        struct pt* p0 = createPoint((double)a->x(), (double)a->y(), (double)a->z());
        // point m0
        Vector* b = modelData->mutable_controlpoints(i+1);
        struct pt* m0 = createPoint((double)b->x(), (double)b->y(), (double)b->z());
        // point p1
        Vector* c = modelData->mutable_controlpoints(i+2);
        struct pt* p1 = createPoint((double)c->x(), (double)c->y(), (double)c->z());
        //point m1
        Vector* d = modelData->mutable_controlpoints(i+3);
        struct pt* m1 = createPoint((double)d->x(), (double)d->y(), (double)d->z());
        // call hermite with these points
        for (double t = 0; t < 1; t+=0.1) {
            struct pt* r = hermite(t, p0, m0, p1, m1);
            v.push_back(r);
        }
    }
    return v;
}

// returns a 1-frame animation of the model evaluated at a certain point along spline
// TODO: get the time it takes the user to draw the LOA, going to need the control points dropped at intervals
Animation* evaluateDLOA(ModelData* modelData) {
    Animation* animation = new Animation();
    return animation;
}

// computes the constant b:
// the ratio between spline length and model length
double calculateB(ModelData* modelData) {
    
    return 0;
}

// returns an Animation object to send back to Unity
Animation* getFrames(ModelData* modelData) {
	Node model = modelData->model();
	Animation* animation = new Animation();
    // get hermite spline
    vector<struct pt*> spline = getSpline(modelData);
    // call evaluateDLOA in loop for every generated control point in the spline
	for (int x = 0; x < spline.size(); x++) {
		Node* node = animation->add_frames();
		node->CopyFrom(model);
		node->set_allocated_position(modelData->mutable_controlpoints(x));
	}
	return animation;
}

