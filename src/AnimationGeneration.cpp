// AnimationGeneration.cpp - backend animation generation functions
// Author: Sarah
// Date last updated: 04/03/2016
//

#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string.h>
#include <vector>
#include "..\include\modeldata.pb.h"
#include "..\include\point.h"
#include "hermite.cpp"

using namespace swellanimations;
using namespace std;

/* uses hermite.cpp to calculate a spline based on control points */
/* for rolling and keyframing (connecting separated LOAs) */
std::vector<struct pt*> getSpline(ModelData* modelData) {
    std::vector<struct pt*> v;
    // TODO: check for self intersection of line
    // idea: check if any two control points have the exact same position
    // get indices of mutable_controlpoints for each of the 4 new control points
    int start, second, third, end;

    /*
    // point p0
    Vector* a = modelData->mutable_controlpoints(start);
    struct pt* p0 = createPoint((double)a->x(), (double)a->y(), (double)a->z());
    // point m0
    Vector* b = modelData->mutable_controlpoints(second);
    struct pt* m0 = createPoint((double)b->x(), (double)b->y(), (double)b->z());
    // point p1
    Vector* c = modelData->mutable_controlpoints(third);
    struct pt* p1 = createPoint((double)c->x(), (double)c->y(), (double)c->z());
    //point m1
    Vector* d = modelData->mutable_controlpoints(end);
    struct pt* m1 = createPoint((double)d->x(), (double)d->y(), (double)d->z());
    // call hermite with these points
    for (double t = 0; t < 1; t+=0.1) {
        struct pt* r = hermite(t, p0, m0, p1, m1);
        v.push_back(r);
    }
    */

    // if line contains no self intersections
    // get user drawn curve from frontend and store in a vector
    for (int i = 0; i < modelData->controlpoints_size(); i++) {
        Vector* tmp = modelData->mutable_controlpoints(i);
        struct pt* point = createPoint((double)tmp->x(), (double)tmp->y(), (double)tmp->z());
        v.push_back(point);
    }

    return v;
}


/* returns the length of the spline by adding the distances between all interpolated points */
/* retthis is the length of the user drawn curve */
double getSplineLength(vector<struct pt*> spline) {
    double length = 0;
    for (int i = 0; i < spline.size()-1; i++) {
        double d = getDistance(spline.at(i), spline.at(i+1));
        length += fabs(d);
    }
    return length;
}

/* returns the length of the longest path from the root */
/* this is roughly the length of the model in the z direction */
double furthestNodeDistance(Node root) {
    double max = (double)fabs(root.mutable_position()->z());
    for (int i = 0; i < root.children_size(); i++) {
        double n = furthestNodeDistance(root.children(i));
        if (n > max)
            max = n;
    }
    return max;
}

/* returns the indices in the spline that correspond to joints of the model in the first frame */
/* maps the points in the spline to the joints in the model based on the length of the model 
    and how many points in the spline will make up a single frame */
vector<int> mapPoints(Node root, double pointsPerFrame, double modelLength) {
    vector<int> total;
    int corresponding = ((double)fabs(root.mutable_position()->z()) / modelLength ) * pointsPerFrame;
    total.push_back(corresponding);
    for (int i = 0; i < root.children_size(); i++) {
        vector<int> c = mapPoints(root.children(i), pointsPerFrame, modelLength);
        for (int j = 0; j < c.size(); j++) {
            total.push_back(c.at(j));
        }
    }
    return total;
}

/* returns a new tree (frame) with new positions based on the calculated corresponding points in the spline */
/* when called in succession, it moves the model and all of its joints along the spline */
Node jointsToSpline(Node root, vector<struct pt*> spline, vector<int> correspondingPoints, int &index) {
    Node frame;
	frame.set_name(root.name());

    int c = correspondingPoints.at(index);
    struct pt* s = spline.at(c);
    frame.mutable_position()->set_x(s->x);
    frame.mutable_position()->set_y(s->y);
    frame.mutable_position()->set_z(s->z);
    for (int i = 0; i < root.children_size(); i++) {
        Node tmp = jointsToSpline(root.children(i), spline, correspondingPoints, ++index);
        tmp.set_name(root.children(i).name());
        Node* p = frame.add_children();
        p->CopyFrom(tmp);
    }
    return frame;
}

/* returns the length of the model by adding the distances between joints */
double getModelLength(ModelData* modelData) {
    double length = 0;
    Node model = modelData->model();
    // call furthestNodeDistance on the root node of the model
    length = furthestNodeDistance(model);
    return length;
}

/* computes the constant b:
    the ratio between spline length and model length */
double calculateB(ModelData* modelData, vector<struct pt*> spline) {
    double splineLength = getSplineLength(spline);
    double modelLength = getModelLength(modelData);
    double b = modelLength / splineLength;
    return b;
}

/* returns an animation of the model evaluated at a certain point along spline */
// TODO: get the time it takes the user to draw the LOA, going to need the control points dropped at intervals
Animation* evaluateDLOA(ModelData* modelData, vector<struct pt*> spline) {
    Animation* animation = new Animation();

    // calculate the constant b
    double b = calculateB(modelData, spline);
    double modelLength = getModelLength(modelData);

    // calculate points in spline per frame
    double pointsPerFrame = spline.size() * b;

    // calculate which point goes with which joint
    Node root = modelData->model();

    Node frame;

    // maps points from user drawn curve -- now a spline -- to the joints in the model
    vector<int> correspondingPoints = mapPoints(root, pointsPerFrame, modelLength);

    // go through every point in spline
    // iterating by 1 every time gives us frames overlapping points in the spline
    for (int i = 0; i < spline.size() - pointsPerFrame; i++) {
        int index = 0;
        // move model and its joints
        frame = jointsToSpline(root, spline, correspondingPoints, index);

        // go through the mapped joints on the model and move them up by 1
        // since we are on a new frame
        vector<int> newCorresponding;
        for (int j = 0; j < correspondingPoints.size(); j++) {
            newCorresponding.push_back(correspondingPoints.at(j)+1);
        }
        // copy for the next iteration 
        correspondingPoints = newCorresponding;

        // add frames to the animation
        Node* a = animation->add_frames();
        a->CopyFrom(frame);
    }

    return animation;
}

void copySplineToAnimation(vector<struct pt*> spline, Animation* animation){
	for (int x = 0; x < spline.size(); x++){
		struct pt* p = spline.at(x);
		Vector* vector = animation->add_spline();
		vector->set_x(p->x);
		vector->set_y(p->y);
		vector->set_z(p->z);
	}
}

/* returns an Animation object to send back to Unity */
Animation* getFrames(ModelData* modelData) {
	Node model = modelData->model();
	Animation* animation = new Animation();
    // get hermite spline
    vector<struct pt*> spline = getSpline(modelData);
    // evaluateDLOA
    animation = evaluateDLOA(modelData, spline);
	//set the spline in the return animation
	copySplineToAnimation(spline, animation);
	return animation;
}

