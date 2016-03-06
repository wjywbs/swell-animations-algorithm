// AnimationGeneration.cpp - backend animation generation functions
// Author: Sarah
// Date last updated: 03/06/2016
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
// TODO: account for repeated points at the end of a spline
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


// returns the length of the spline by adding the distances between all interpolated points
double getSplineLength(vector<struct pt*> spline) {
    double length = 0;
    for (int i = 0; i < spline.size()-1; i++) {
        double d = getDistance(spline.at(i), spline.at(i+1));
        length += d;
    }
    return length;
}

// returns the length of the model by adding the distances between joints
double getModelLength(ModelData* modelData) {
    double length = 0;
    Node model = modelData->model();
    // TODO: understand model data structure better
    for (int i = 0; i < modelData->controlpoints_size()-1; i++) {
        Vector* l = modelData->mutable_controlpoints(i);
        Vector* r = modelData->mutable_controlpoints(i+1);
        struct pt* left = createPoint((double)l->x(), (double)l->y(), (double)l->z());
        struct pt* right = createPoint((double)r->x(), (double)r->y(), (double)r->z());
        double d = getDistance(left,right);
        length += d;
    }
    return length;
}

// computes the constant b:
// the ratio between spline length and model length
double calculateB(ModelData* modelData, vector<struct pt*> spline) {
    double splineLength = getSplineLength(spline);
    double modelLength = getModelLength(modelData);
    double b = modelLength / splineLength;
    return b;
}

// returns a 1-frame animation of the model evaluated at a certain point along spline
// TODO: can the return type be a frame??
// TODO: get the time it takes the user to draw the LOA, going to need the control points dropped at intervals
Animation* evaluateDLOA(ModelData* modelData, vector<struct pt*> spline) {
    Animation* animation = new Animation();
    double b = calculateB(modelData, spline);
    // TODO: calculate points in spline per model 
    // double pointsPerModel = spline.size() * b
    // TODO: calculate which point goes with which joint. this will be the number to increment by
    // double pointToJoint = (pointsPerModel) / ((number of joints in model) - 1)
    // TODO: create a frame with joints mapped to appropriate points
    return animation;
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

