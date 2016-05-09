// AnimationGeneration.cpp - backend animation generation functions
// Author: Sarah
// Date last updated: 05/03/2016
//
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string.h>
#include <vector>
#include <algorithm>
#include "modeldata.pb.h"
#include "point.h"
#include "hermite.cpp"
#include "diff_hermite.c"

using namespace swellanimations;
using namespace std;

/* uses hermite.cpp to calculate a spline based on control points */
/* for rolling and keyframing (connecting separated LOAs) */
std::vector<struct pt*> getSpline(ModelData* modelData) {
    std::vector<struct pt*> v;
    // TODO: check for self intersection of line
    // idea: check if any two control points have the exact same position
    // get indices of mutable_controlpoints for each of the 4 new control points
	float frameIncs = modelData->controlpoints_size() / (float) modelData->numberofframes();
	float currentInc = frameIncs;
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

		if (currentInc < i + 1){
			for (double t = (currentInc - i + 1); t < 1; t += frameIncs) {
				struct pt* r = hermite(t, p0, m0, p1, m1);
				v.push_back(r);
				currentInc += frameIncs;
			}
		}
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
Node jointsToSpline(Node root, vector<struct pt*> spline, vector<int> correspondingPoints, int &index){ //, ofstream *myfile) {
    Node frame;
	frame.set_name(root.name());
    
    int c = correspondingPoints.at(index);
    struct pt* s = spline.at(c);

    double x0, x1, x2;
    double y0, y1, y2;
    double z0, z1, z2;

    struct pt* s0;
    struct pt* s2;

    if (c == 0)
    {
        x0 = s->x; 
        y0 = s->y; 
        z0 = s->z; 
        s2 = spline.at(c+1);
        x1 = (s->x + s2->x) / 2; 
        y1 = (s->y + s2->y) / 2; 
        z1 = (s->z + s2->z) / 2; 
        x2 = s2->x; 
        y2 = s2->y; 
        z2 = s2->z; 
    }
    else if (c == spline.size()-1)
    {
        s0 = spline.at(c-1);
        x0 = s0->x; 
        y0 = s0->y; 
        z0 = s0->z; 
        x1 = (s->x + s0->x) / 2; 
        y1 = (s->y + s0->y) / 2; 
        z1 = (s->z + s0->z) / 2; 
        x2 = s->x; 
        y2 = s->y; 
        z2 = s->z; 
    }
    else
    {
        s0 = spline.at(c-1);
        s2 = spline.at(c+1);
        x0 = s0->x; 
        y0 = s0->y; 
        z0 = s0->z; 
        x1 = s->x; 
        y1 = s->y; 
        z1 = s->z; 
        x2 = s2->x; 
        y2 = s2->y; 
        z2 = s2->z; 
    
    }
    double t = 0.5;
    struct Direction *d = (Direction*)calloc(1, sizeof(struct Direction));
    Differentiate(x0, x1, x2, t, &(d->x));
    Differentiate(y0, y1, y2, t, &(d->y));
    Differentiate(z0, z1, z2, t, &(d->z));

    frame.mutable_position()->set_x(s->x);
    frame.mutable_position()->set_y(s->y);
    frame.mutable_position()->set_z(s->z);

    frame.mutable_eularangles()->set_x(d->x);
    frame.mutable_eularangles()->set_y(d->y);
    frame.mutable_eularangles()->set_z(d->z);

    //*myfile << "-- ";
    //*myfile << root.name() << endl;
    //*myfile << frame.mutable_position()->z() << endl;

    for (int i = 0; i < root.children_size(); i++) {
        Node tmp = jointsToSpline(root.children(i), spline, correspondingPoints, ++index); //, myfile);
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
    //ofstream myfile;
    //myfile.open ("/home/psarahdactyl/Documents/bbfunfunfun.txt");

    // calculate the constant b
    double b = calculateB(modelData, spline);
    double modelLength = getModelLength(modelData);

    // calculate points in spline per frame
    double pointsPerFrame = spline.size() * b;

    // calculate which point goes with which joint
    Node root = modelData->model();

    // maps points from user drawn curve -- now a spline -- to the joints in the model
    vector<int> correspondingPoints = mapPoints(root, pointsPerFrame, modelLength);
    
    //they are backwards, don't know why but I fixed it
    reverse(correspondingPoints.begin(), correspondingPoints.end());

    // go through every point in spline
    // iterating by 1 every time gives us frames overlapping points in the spline
    for (int i = 0; i < spline.size() - pointsPerFrame; i++) {
        int index = 0;
        // move model and its joints
        Node frame = jointsToSpline(root, spline, correspondingPoints, index); //, &myfile);

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

/* May need someone to double check that I'm using ModelData appropriately since I've never messed around with it before */
void applyRotationPoints(ModelData* modelData) {
	Node model = modelData->model();

	// Store the rotation angle values in a vector for later use
	vector<struct pt*> rotationAngles;
	struct pt *point = new struct pt;
	RotationPoint rp;

	// Iterate over each rotation point and calculate the rotation angle for adjustment use in the x, y, and z dimensions
	for (int i = 0; i < modelData->rotationpoints_size(); i++) {
		rp = modelData->rotationpoints(i);

		struct pt *point = new struct pt;

		// The model will begin to rotate some specified number of frames before the rotation point- this number is retrieved in the numframes() method
		point->x = rp.mutable_rotation()->x() / rp.numframes();
		point->y = rp.mutable_rotation()->y() / rp.numframes();
		point->z = rp.mutable_rotation()->z() / rp.numframes();
		rotationAngles.push_back(point);
	}

	int startframe;
	int endframe;

	// Iterate over each rotation point, calculating the start frame and end frame
	for (int i = 0; i < modelData->rotationpoints_size(); i++) {
		rp = modelData->rotationpoints(i);
		startframe = rp.startframe();
		endframe = rp.startframe() + rp.numframes();

		// For each frame between the start and end frame, modify the eular angles by the appropriate value stored in the rotationAngles vector
		for (int j = startframe; j <= endframe; j++) {
			model.mutable_eularangles()->set_x(model.mutable_eularangles()->x() + rotationAngles.at(i)->x);
			model.mutable_eularangles()->set_y(model.mutable_eularangles()->y() + rotationAngles.at(i)->y);
			model.mutable_eularangles()->set_z(model.mutable_eularangles()->z() + rotationAngles.at(i)->z);
		}
	}
}
