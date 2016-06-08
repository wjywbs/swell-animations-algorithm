// AnimationGeneration.cpp - backend animation generation functions
// Author: Sarah
// Date last updated: 05/22/2016
//
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include "modeldata.pb.h"
#include "point.h"
#include "hermite.cpp"
#include "diff_hermite.h"
#include "AnimationLayering.cpp"

using namespace swellanimations;
using namespace std;

// Returns the length of the spline by adding the distances between all
// interpolated points. retthis is the length of the user drawn curve.
double getSplineLength(const vector<Point>& spline) {
  double length = 0;
  for (int i = 0; i < spline.size() - 1; i++) {
    double d = getDistance(spline.at(i), spline.at(i + 1));
    length += fabs(d);
  }
  return length;
}

// Returns the length of the longest path from the root.
// This is roughly the length of the model in the z direction.
double furthestNodeDistance(const Node& root) {
  double max = (double)fabs(root.position().z());
  for (int i = 0; i < root.children_size(); i++) {
    double n = furthestNodeDistance(root.children(i));
    if (n > max)
      max = n;
  }
  return max;
}

double getDistanceFromModelVectors(Vector* p1, Vector* p2) {
  float diffY = p1->y() - p2->y();
  float diffX = p1->x() - p2->x();
  float diffZ = p1->z() - p2->z();
  return sqrt((diffY * diffY) + (diffX * diffX) + (diffZ * diffZ));
}

// Returns the length of the model by adding the distances between joints.
double getModelLength(Node* model) {
  double length = 0;
  while (model->children_size() > 0) {
    // We are assuming only 1 child per node.  Currently that is all we send.
    Node* child = model->mutable_children(0);
    length += getDistanceFromModelVectors(model->mutable_position(),
                                          child->mutable_position());
    model = child;
  }
  return length;
}

// Returns the indices in the spline that correspond to joints of the model in
// the first frame. Maps the points in the spline to the joints in the model
// based on the length of the model and how many points in the spline will make
// up a single frame.
vector<int> mapPoints(Node* root,
                      int maxIndexOnStartFrame,
                      double modelLength) {
  vector<int> total;
  float distanceRatioFromFront = getModelLength(root) / modelLength;
  int correspondingIndex = round(distanceRatioFromFront * maxIndexOnStartFrame);
  total.push_back(correspondingIndex);
  for (int i = 0; i < root->children_size(); i++) {
    vector<int> c =
        mapPoints(root->mutable_children(i), maxIndexOnStartFrame, modelLength);
    for (int j = 0; j < c.size(); j++) {
      total.push_back(c.at(j));
    }
  }
  return total;
}

// Returns a new tree (frame) with new positions based on the calculated
// corresponding points in the spline. When called in succession, it moves the
// model and all of its joints along the spline.
Node jointsToSpline(Node* root,
                    const vector<Point>& spline,
                    vector<int> correspondingPoints,
                    int& index,
                    ofstream* myfile) {
  Node frame;
  frame.set_name(root->name());
  //*myfile << frame.name() << endl;

  int c = correspondingPoints.at(index);
  Point s = spline.at(c);

  double x0, x1, x2;
  double y0, y1, y2;
  double z0, z1, z2;

  Point s0;
  Point s2;
  Point m0;

  if (c == 0) {
    s2 = spline.at(c + 1);
    m0 = forwardDiff(s, s2);
  } else if (c == spline.size() - 1) {
    s0 = spline.at(c - 1);
    m0 = forwardDiff(s0, s);
  } else {
    s0 = spline.at(c - 1);
    s2 = spline.at(c + 1);
    m0 = midpointDiff(s0, s, s2);
  }

  frame.mutable_position()->set_x(s.x);
  frame.mutable_position()->set_y(s.y);
  frame.mutable_position()->set_z(s.z);

  frame.mutable_eularangles()->set_x(m0.x);
  frame.mutable_eularangles()->set_y(m0.y);
  frame.mutable_eularangles()->set_z(m0.z);

  //*myfile << "-- ";
  //*myfile << root.name() << endl;
  //*myfile << frame.mutable_position()->z() << endl;

  for (int i = 0; i < root->children_size(); i++) {
    Node tmp = jointsToSpline(root->mutable_children(i), spline,
                              correspondingPoints, ++index, myfile);
    tmp.set_name(root->children(i).name());
    Node* p = frame.add_children();
    p->CopyFrom(tmp);
  }
  return frame;
}

// Uses hermite.cpp to calculate a spline based on control points
// for rolling and keyframing (connecting separated LOAs).
std::vector<Point> getSpline(ModelData* modelData) {
  std::vector<Point> v;
  // TODO: check for self intersection of line
  // idea: check if any two control points have the exact same position

  // Get indices of mutable_controlpoints for each of the 4 new control points.
  float frameIncs =
      modelData->controlpoints_size() / (float)modelData->numberofframes();
  float currentInc = frameIncs;
  // if line contains no self intersections
  // get user drawn curve from frontend and store in a vector
  //
  //
  for (int i = 0; i < modelData->controlpoints_size() - 1; i++) {
    Vector* p0Vec = modelData->mutable_controlpoints(i);
    Vector* p1Vec = modelData->mutable_controlpoints(i + 1);

    Point p0 = createPoint(p0Vec->x(), p0Vec->y(), p0Vec->z());
    Point p1 = createPoint(p1Vec->x(), p1Vec->y(), p1Vec->z());
    Point m0;
    Point m1;

    if (i == 0) {
      Vector* p2Vec = modelData->mutable_controlpoints(i + 2);
      Point p2 = createPoint(p2Vec->x(), p2Vec->y(), p2Vec->z());
      m0 = forwardDiff(p0, p1);
      m1 = midpointDiff(p0, p1, p2);
    } else if (i == modelData->controlpoints_size() - 2) {
      Vector* p2Vec = modelData->mutable_controlpoints(i - 2);
      Point p2 = createPoint(p2Vec->x(), p2Vec->y(), p2Vec->z());
      m1 = forwardDiff(p0, p1);
      m0 = midpointDiff(p2, p0, p1);
    } else {
      Vector* p00Vec = modelData->mutable_controlpoints(i - 1);
      Point p00 = createPoint(p00Vec->x(), p00Vec->y(), p00Vec->z());
      Vector* p2Vec = modelData->mutable_controlpoints(i + 2);
      Point p2 = createPoint(p2Vec->x(), p2Vec->y(), p2Vec->z());
      m0 = midpointDiff(p00, p0, p1);
      m1 = midpointDiff(p0, p1, p2);
    }

    if (currentInc < i + 1) {
      for (double t = (currentInc - i + 1); t < 1; t += frameIncs) {
        Point r = hermite(t, p0, m0, p1, m1);
        v.push_back(r);
        currentInc += frameIncs;
      }
    }
  }

  return v;
}

// Returns an animation of the model evaluated at a certain point along spline.
// TODO: get the time it takes the user to draw the LOA, going to need the
// control points dropped at intervals
Animation* evaluateDLOA(ModelData* modelData,
                        const vector<Point>& spline,
                        int* modelFrames) {
  Animation* animation = new Animation();
  ofstream myfile;
  // myfile.open ("/home/psarahdactyl/Documents/ccfunfunfun.txt");

  // calculate the constant b
  double modelLength = getModelLength(modelData->mutable_model());
  double splineLength = getSplineLength(spline);
  double b = modelLength / (splineLength);

  // calculate points in spline per frame
  double pointsPerFrame = spline.size() * b;
  *modelFrames = pointsPerFrame;
  // myfile << pointsPerFrame << endl;

  // calculate which point goes with which joint
  Node* root = modelData->mutable_model();

  // maps points from user drawn curve -- now a spline -- to the joints in the
  // model
  vector<int> correspondingPoints =
      mapPoints(root, pointsPerFrame, modelLength);
  for (int h = 0; h < correspondingPoints.size(); h++) {
    myfile << correspondingPoints.at(h) << endl;
  }

  // End animation on curve unless there are not enough points on spline.
  vector<Point> extra;
  if (spline.size() < pointsPerFrame) {
    Point last = spline.at(spline.size() - 1);
    Point secondLast = spline.at(spline.size() - 2);

    double x = last.x - secondLast.x;
    double y = last.y - secondLast.y;
    double z = last.z - secondLast.z;
    Point difference = createPoint(x, y, z);

    for (double t = 1; t <= pointsPerFrame; t++) {
      Point diff = multScalar(t, difference);
      Point r = add(last, diff);
      extra.push_back(r);
    }
  }

  vector<Point> newSpline;
  newSpline.reserve(spline.size() + extra.size());  // preallocate memory
  newSpline.insert(newSpline.end(), spline.begin(), spline.end());
  newSpline.insert(newSpline.end(), extra.begin(), extra.end());

  // go through every point in spline
  // iterating by 1 every time gives us frames overlapping points in the spline
  for (int i = 0; i <= newSpline.size() - pointsPerFrame; i++) {
    int index = 0;
    // move model and its joints
    Node frame =
        jointsToSpline(root, newSpline, correspondingPoints, index, &myfile);
    frame.set_name(root->name());

    // go through the mapped joints on the model and move them up by 1
    // since we are on a new frame
    vector<int> newCorresponding;
    for (int j = 0; j < correspondingPoints.size(); j++) {
      newCorresponding.push_back(correspondingPoints.at(j) + 1);
    }
    // copy for the next iteration
    correspondingPoints = newCorresponding;

    // add frames to the animation
    Node* a = animation->add_frames();
    a->CopyFrom(frame);
    a->set_name(root->name());
  }

  return animation;
}

void copySplineToAnimation(const vector<Point>& spline, Animation* animation) {
  for (int x = 0; x < spline.size(); x++) {
    const Point& p = spline.at(x);
    Vector* vector = animation->add_spline();
    vector->set_x(p.x);
    vector->set_y(p.y);
    vector->set_z(p.z);
  }
}

// Normalize angles greater than 180 degree to negative angle.
double normalizeAngle(double angle) {
  if (angle > 180)
    return angle - 360;
  return angle;
}

void applyRotationPoints(ModelData* modelData,
                         Animation* animation,
                         int modelFrames) {
  // Iterate over each rotation point and calculate rotation angle for
  // adjustment use in the x, y, and z dimensions, and the start frame and end
  // frame.
  for (int i = 0; i < modelData->rotationpoints_size(); i++) {
    // The model will begin to rotate some specified number of frames before the
    // rotation point- this number is retrieved in the numframes() method
    const RotationPoint& rp = modelData->rotationpoints(i);
    // Consider model length to start earlier.
    int startframe = max(rp.startframe() - modelFrames, 0);
    int endframe =
        min(startframe + rp.numframes(), animation->frames_size() - 1);

    double x = normalizeAngle(rp.rotation().x()) / rp.numframes(),
           y = normalizeAngle(rp.rotation().y()) / rp.numframes(),
           z = normalizeAngle(rp.rotation().z()) / rp.numframes();

    // For each frame between the start and end frame, modify the eular angles
    // by the appropriate value stored in the rotationAngles vector
    for (int j = startframe; j <= endframe; j++) {
      animation->mutable_frames(j)->mutable_rotation()->set_x(x);
      animation->mutable_frames(j)->mutable_rotation()->set_y(y);
      animation->mutable_frames(j)->mutable_rotation()->set_z(z);
    }

    // Reverse for the frames after
    int end = endframe + rp.numframes();
    int frameEndForReverse = min(animation->frames_size() - 1, end);
    for (int j = endframe + 1; j <= frameEndForReverse; j++) {
      animation->mutable_frames(j)->mutable_rotation()->set_x(-x);
      animation->mutable_frames(j)->mutable_rotation()->set_y(-y);
      animation->mutable_frames(j)->mutable_rotation()->set_z(-z);
    }
  }
}

float CalculateDistance(const Point& p1, const Vector& p2) {
  float diffY = p1.y - p2.y();
  float diffX = p1.x - p2.x();
  float diffZ = p1.z - p2.z();
  return sqrt((diffY * diffY) + (diffX * diffX) + (diffZ * diffZ));
}

void adjustAnimationLayers(ModelData* modelData, const vector<Point>& spline) {
  for (int x = 0; x < modelData->animationlayers_size(); x++) {
    AnimationLayer* animLayer = modelData->mutable_animationlayers(x);
    int startFrame = animLayer->startframe();
    int numFrames = animLayer->numframes();
    Vector* location = modelData->mutable_controlpoints(startFrame + numFrames);
    float closestDistance = CalculateDistance(spline.at(0), *location);
    int closestFrame = 0;
    for (int i = 1; i < spline.size(); i++) {
      float distance = CalculateDistance(spline.at(i), *location);
      if (distance < closestDistance) {
        closestDistance = distance;
        closestFrame = i;
      }
    }
    animLayer->set_numframes(min(numFrames, closestFrame + 1));
    animLayer->set_startframe(max(closestFrame - numFrames, 0));
  }
}

// Returns an Animation object to send back to Unity.
Animation* getFrames(ModelData* modelData) {
  Node model = modelData->model();
  // get hermite spline
  vector<Point> spline = getSpline(modelData);
  // evaluateDLOA
  int modelFrames;
  Animation* animation = evaluateDLOA(modelData, spline, &modelFrames);
  // apply rotation points to the model data
  applyRotationPoints(modelData, animation, modelFrames);

  if (modelData->animationlayers_size() > 0) {
    // Call layering
    adjustAnimationLayers(modelData, spline);
    AddLayering(modelData, animation);
  }
  // set the spline in the return animation
  copySplineToAnimation(spline, animation);
  return animation;
}
