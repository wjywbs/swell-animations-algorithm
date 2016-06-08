// AnimationGeneration.cpp - backend animation generation functions
// Author: Sarah
// Date last updated: 03/06/2016
//

#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string.h>
#include <vector>
#include "../include/modeldata.pb.h"
#include "../include/point.h"
#include "hermite.cpp"

using namespace swellanimations;
using namespace std;

// uses hermite.cpp to calculate a spline based on control points
// for rolling and keyframing (connecting separated LOAs)
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
  for (int i = 0; i < modelData->controlpoints_size(); i++) {
    Vector* tmp = modelData->mutable_controlpoints(i);
    struct pt* point =
        createPoint((double)tmp->x(), (double)tmp->y(), (double)tmp->z());
    v.push_back(point);
  }

  return v;
}

// returns the length of the spline by adding the distances between all
// interpolated points
double getSplineLength(vector<struct pt*> spline) {
  double length = 0;
  for (int i = 0; i < spline.size() - 1; i++) {
    double d = getDistance(spline.at(i), spline.at(i + 1));
    length += abs(d);
  }
  return length;
}

// return the length of the longest path from the root
double furthestNodeDistance(Node root) {
  double max = (double)abs(root.mutable_position()->z());
  for (int i = 0; i < root.children_size(); i++) {
    double n = furthestNodeDistance(root.children(i));
    if (n > max)
      max = n;
  }
  return max;
}

// returns the indices in the spline that correspond to joints of the model in
// the first frame
vector<int> mapPoints(Node root, double pointsPerFrame, double modelLength) {
  vector<int> total;
  int corresponding =
      ((double)abs(root.mutable_position()->z()) / modelLength) *
      pointsPerFrame;
  total.push_back(corresponding);
  for (int i = 0; i < root.children_size(); i++) {
    vector<int> c = mapPoints(root.children(i), pointsPerFrame, modelLength);
    for (int j = 0; j < c.size(); j++) {
      total.push_back(c.at(j));
    }
  }
  return total;
}

void printTree(Node frame, ofstream* myfile) {
  *myfile << "(";
  *myfile << fixed << setprecision(5) << frame.mutable_position()->x();
  *myfile << ", ";
  *myfile << fixed << setprecision(5) << frame.mutable_position()->y();
  *myfile << ", ";
  *myfile << fixed << setprecision(5) << frame.mutable_position()->z();
  *myfile << ")" << endl;
  for (int i = 0; i < frame.children_size(); i++) {
    printTree(frame.children(i), myfile);
  }
}

// returns a new tree (frame) with new positions based on the calculated
// corresponding points in the spline
Node jointsToSpline(Node root,
                    vector<struct pt*> spline,
                    vector<int> correspondingPoints,
                    int& index,
                    ofstream* myfile) {
  Node frame;
  frame.set_name(root.name());
  // Rotation will need to be calculated, for now we are just copying
  // frame.mutable_eularangles()->set_x(root.eularangles().x());
  // frame.mutable_eularangles()->set_y(root.eularangles().y());
  // frame.mutable_eularangles()->set_z(root.eularangles().z());

  int c = correspondingPoints.at(index);
  struct pt* s = spline.at(c);
  frame.mutable_position()->set_x(s->x);
  frame.mutable_position()->set_y(s->y);
  frame.mutable_position()->set_z(s->z);
  for (int i = 0; i < root.children_size(); i++) {
    //*myfile << index << endl;
    Node tmp = jointsToSpline(root.children(i), spline, correspondingPoints,
                              ++index, myfile);
    tmp.set_name(root.children(i).name());
    Node* p = frame.add_children();
    p->CopyFrom(tmp);
  }
  return frame;
}

// returns the length of the model by adding the distances between joints
double getModelLength(ModelData* modelData) {
  double length = 0;
  Node model = modelData->model();
  length = furthestNodeDistance(model);
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

// returns an animation of the model evaluated at a certain point along spline
// TODO: get the time it takes the user to draw the LOA, going to need the
// control points dropped at intervals
Animation* evaluateDLOA(ModelData* modelData, vector<struct pt*> spline) {
  Animation* animation = new Animation();
  double b = calculateB(modelData, spline);
  double modelLength = getModelLength(modelData);

  // calculate points in spline per frame
  double pointsPerFrame = spline.size() * b;

  // calculate which point goes with which joint
  Node root = modelData->model();
  Node frame;

  ofstream myfile;
  myfile.open("/home/psarahdactyl/Documents/aafunfunfun.txt");
  vector<int> correspondingPoints =
      mapPoints(root, pointsPerFrame, modelLength);
  for (int i = 0; i < spline.size() - pointsPerFrame; i++) {
    int index = 0;
    frame = jointsToSpline(root, spline, correspondingPoints, index, &myfile);
    printTree(frame, &myfile);
    myfile << "-----------------------" << endl;
    vector<int> newCorresponding;
    for (int j = 0; j < correspondingPoints.size(); j++) {
      newCorresponding.push_back(correspondingPoints.at(j) + 1);
    }
    correspondingPoints = newCorresponding;
    Node* a = animation->add_frames();
    a->CopyFrom(frame);
  }
  myfile.close();

  return animation;
}

// returns an Animation object to send back to Unity
Animation* getFrames(ModelData* modelData) {
  Node model = modelData->model();
  Animation* animation = new Animation();
  // get hermite spline
  vector<struct pt*> spline = getSpline(modelData);
  // evaluateDLOA
  animation = evaluateDLOA(modelData, spline);

  return animation;
}
