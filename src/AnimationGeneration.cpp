// AnimationGeneration.cpp - backend animation generation functions
// Author: Sarah
// Date last updated: 04/18/2016
//

#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string.h>
#include <vector>
#include "modeldata.pb.h"
#include "point.h"
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
    int numberOfFrames;

	numberOfFrames = modelData->numberofframes();

    // if line contains no self intersections
    // get user drawn curve from frontend and store in a vector
    //
    //
    for (int i = 0; i < modelData->controlpoints_size()-1; i++) {
        Vector* p0Vec = modelData->mutable_controlpoints(i);
        Vector* p1Vec = modelData->mutable_controlpoints(i+1);

        struct pt* p0 = createPoint((double)p0Vec->x(), (double)p0Vec->y(), (double)p0Vec->z()); 
        struct pt* p1 = createPoint((double)p1Vec->x(), (double)p1Vec->y(), (double)p1Vec->z()); 
        struct pt* m0;
        struct pt* m1;

        if(i == 0)
        {
            Vector* p2Vec = modelData->mutable_controlpoints(i+2);
            struct pt* p2 = createPoint((double)p2Vec->x(), (double)p2Vec->y(), (double)p2Vec->z()); 
            m0 = forwardDiff(p0, p1);
            m1 = midpointDiff(p0, p1, p2);
        }
        else if(i == modelData->controlpoints_size()-2)
        {
            Vector* p2Vec = modelData->mutable_controlpoints(i-2);
            struct pt* p2 = createPoint((double)p2Vec->x(), (double)p2Vec->y(), (double)p2Vec->z()); 
            m1 = forwardDiff(p0, p1);
            m0 = midpointDiff(p2, p0, p1);
        }
        else
        {
            Vector* p00Vec = modelData->mutable_controlpoints(i-1);
            struct pt* p00 = createPoint((double)p00Vec->x(), (double)p00Vec->y(), (double)p00Vec->z()); 
            Vector* p2Vec = modelData->mutable_controlpoints(i+2);
            struct pt* p2 = createPoint((double)p2Vec->x(), (double)p2Vec->y(), (double)p2Vec->z()); 
            m0 = midpointDiff(p00, p0, p1);
            m1 = midpointDiff(p0, p1, p2);
        }

        struct pt* r = hermite(0.5, p0, m0, p1, m1);
        v.push_back(r);
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

    //frame.mutable_eularangles()->set_x(root.eularangles().x());
    //frame.mutable_eularangles()->set_y(root.eularangles().y());
    //frame.mutable_eularangles()->set_z(root.eularangles().z());

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

