#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include "point.h"
#include "modeldata.pb.h"

using namespace std;
using namespace swellanimations;

/*
 * Disposable (for debug purposes): Reads "points"
 */
vector<pt> ReadPoints(string filename) {
  ifstream in(filename.c_str());
  // pt *points = (pt*) calloc(1, sizeof(struct pt));
  vector<pt> points;

  while (in) {
    string line;
    if (!getline(in, line)) {
      break;
    }

    istringstream ss(line);
    string temp;
    pt p;

    getline(ss, temp, ' ');
    p.x = atof(temp.c_str());

    getline(ss, temp, ' ');
    p.y = atof(temp.c_str());

    points.push_back(p);
  }
  return points;
}

double GetAngleBetweenVectors(pt a, pt b) {
  return atan2(b.y - a.y, b.x - a.x);
}

double GetAngleBetweenVectors(Vector a, Vector b) {
  return atan2(b.y() - a.y(), b.x() - a.x());
}

double DistanceBetweenPoints(pt a, pt b) {
  return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}

double DistanceBetweenPoints(Vector a, Vector b) {
  return sqrt(pow(b.x() - a.x(), 2) + pow(b.y() - a.y(), 2) +
              pow(b.z() - a.z(), 2));
}

pt MovePoint(pt a_spline, pt b_spline, pt a_model, pt b_model) {
  double theta_spline = GetAngleBetweenVectors(a_spline, b_spline);
  double dist_spline = DistanceBetweenPoints(a_spline, b_spline);
  double dist_model = DistanceBetweenPoints(a_model, b_model);

  double ratio = dist_model / dist_spline;

  double new_x = b_model.x + (cos(theta_spline));
  double new_y = b_model.y + (sin(theta_spline));

  pt new_variant = {new_x, new_y, 0};
  return new_variant;
}

/*
 * Moves a vector from sequence { a_model, b_model, c_model } to conform to the
 * curve given by { a_spline, b_spline, c_spline }
 */
void MoveVector(Vector* a_model,
                Vector* b_model,
                Vector* a_spline,
                Vector* b_spline,
                int dist_to_dloa) {
  /* Get normalized vector from point a_spline to b_spline */
  double v_spline_x = b_spline->x() - a_spline->x();
  double v_spline_y = b_spline->y() - a_spline->y();
  double v_spline_z = b_spline->z() - a_spline->z();
  double v_spline_magnitude =
      sqrt(pow(v_spline_x, 2) + pow(v_spline_y, 2) + pow(v_spline_z, 2));
  Vector* normal_spline = new Vector;

  normal_spline->set_x(v_spline_x / v_spline_magnitude);
  normal_spline->set_y(v_spline_y / v_spline_magnitude);
  normal_spline->set_z(v_spline_z / v_spline_magnitude);

  /* Get magnitude from point a_model to b_model */
  double m_spline_x = b_model->x() - a_model->x();
  double m_spline_y = b_model->y() - a_model->y();
  double m_spline_z = b_model->z() - a_model->z();
  double m_spline_magnitude =
      sqrt(pow(m_spline_x, 2) + pow(m_spline_y, 2) + pow(m_spline_z, 2));

  /* Get new vector from a_model to where b_model should move to */
  double new_point_x = v_spline_x + a_model->x();
  double new_point_y = v_spline_y + a_model->y();
  double new_point_z = v_spline_z + a_model->z();

  /* Scale the vector coordinates */
  new_point_x *= m_spline_magnitude;
  new_point_y *= m_spline_magnitude;
  new_point_z *= m_spline_magnitude;

  /* Set the points in the model */
  b_model->set_x(new_point_x);
  b_model->set_y(new_point_y);
  b_model->set_z(new_point_z);
}

/*vector<pt> Morph(vector<pt> detail, vector<pt> model) {
        vector<int>control;
        control.push_back(0);
        control.push_back(detail.size()*34/100);
        control.push_back(detail.size()*51/100);
        control.push_back(detail.size()*68/100);
        control.push_back(detail.size()*85/100);
        control.push_back(detail.size()-1);

        for (int i = 0; i < control.size() - 1; i++) {
                pt new_model_point = MovePoint(detail[control[i]],
detail[control[i+1]], model[i], model[i+1]);
                model[i+1].x = new_model_point.x;
                model[i+1].y = new_model_point.y;
        }

        return model;
}*/

void Morph(Node* frames,
           AnimationLayer* layer,
           int dloa_size,
           int dist_to_dloa) {
  Vector* current_position = frames->mutable_position();
  Node* nextchild = frames->mutable_children(0);
  int childcount = 1;
  int childrenleft = 0;

  // Find number of points from child root
  Node* temp = nextchild;
  while (temp->children_size() > 0) {
    childcount++;
    temp = temp->mutable_children(0);
  }
  // Get mid point child
  Node* midpoint = nextchild;
  for (int c = 0; c < childcount / 2; c++) {
    midpoint = midpoint->mutable_children(0);
  }
  childrenleft = childcount;
  temp = NULL;

  ofstream o;
  o.open("debug", ofstream::app);
  o << "CHILDREN = " << childcount << endl;
  o << "DLOAD SIZE = " << dloa_size << endl;
  o.close();

  /* Move points on model spine from center out towards the head while
   * trying to mirror the detail LOA */
  temp = midpoint;
  for (int x = childcount / 2; x > 0; x--) {
    int first_bound = (dloa_size / 100) * (100 * (x) / childcount);
    int next_bound = (dloa_size / 100) * (100 * (x - 1) / childcount);

    // make sure we don't go out of bounds
    if (next_bound > dloa_size) {
      next_bound = dloa_size;
    }

    MoveVector(temp->mutable_position(),
               temp->mutable_parent()->mutable_position(),
               layer->mutable_layerpoints(first_bound),
               layer->mutable_layerpoints(next_bound), dist_to_dloa);

    if (!temp->has_parent()) {
      break;
    }
    temp = temp->mutable_parent();
  }
  /* Since the head of the list is disjointed, we need to manually move
   * it as well */
  MoveVector(
      nextchild->mutable_position(), current_position,
      layer->mutable_layerpoints((dloa_size / 100) * (100 * (1) / childcount)),
      layer->mutable_layerpoints(0), dist_to_dloa);

  temp = midpoint;
  for (int x = childcount / 2 + 1; x < childcount; x++) {
    // establish bounds
    int first_bound = (dloa_size / 100) * (100 * (x) / childcount);
    int next_bound = (dloa_size / 100) * (100 * (x + 1) / childcount);

    // make sure we don't go out of bounds
    if (next_bound > dloa_size) {
      next_bound = dloa_size;
    }

    MoveVector(temp->mutable_position(),
               temp->mutable_children(0)->mutable_position(),
               layer->mutable_layerpoints(first_bound),
               layer->mutable_layerpoints(next_bound), dist_to_dloa);
    temp = temp->mutable_children(0);

    if (!temp->children_size()) {
      break;
    }
  }

  /****************
          // Morph first point from root position
          MoveVector(	current_position,
                          nextchild->mutable_position(),
                          layer->mutable_layerpoints(0),
                          layer->mutable_layerpoints((dloa_size/100)*(100*1/(childcount))),
                          dist_to_dloa
                          );

          current_position = nextchild->mutable_position();
          nextchild = nextchild->mutable_children(0);

          // Next couple points on the model's spine
          for(int x = 1; nextchild->children_size() > 0; x++) {
                  // establish bounds
                  int first_bound = (dloa_size/100)*(100*(x)/childcount);
                  int next_bound = (dloa_size/100)*(100*(x+1)/childcount);

                  // make sure we don't go out of bounds
                  if(next_bound > dloa_size) {
                          next_bound = dloa_size;
                  }

                  MoveVector(	current_position,
                                  nextchild->mutable_position(),
                                  layer->mutable_layerpoints(first_bound),
                                  layer->mutable_layerpoints(next_bound),
                                  dist_to_dloa
                                  );
                  current_position = nextchild->mutable_position();
                  nextchild = nextchild->mutable_children(0);

                  // Just in case we have a infinite loop:
                  if(x>20)
                          break;
          }
  ******************/
}

void AddLayering(ModelData* modelData, Animation* animation) {
  ofstream o;
  o.open("debug", ofstream::app);
  o << "nlayers: " << modelData->animationlayers_size() << endl;
  o << "layer(0).start: " << modelData->animationlayers(0).startframe() << endl;
  o << "layer(0).numframe: " << modelData->animationlayers(0).numframes()
    << endl;
  o << "layer(0).layerpointsize: "
    << modelData->animationlayers(0).layerpoints_size() << endl;
  o.close();
  for (int i = 0; i < modelData->animationlayers_size(); i++) {
    for (int j = modelData->animationlayers(i).startframe();
         j < modelData->animationlayers(i).startframe() +
                 modelData->animationlayers(i).numframes();
         j++) {
      Morph(animation->mutable_frames(j), modelData->mutable_animationlayers(i),
            modelData->animationlayers(i).layerpoints_size(),
            modelData->animationlayers(i).startframe() +
                modelData->animationlayers(i).numframes() - j);
    }  // end frame counter
  }    // end layer counter
}

/*
int main() {
        vector<pt> splinepoints = ReadPoints(string("spline.out"));
        vector<pt> modelpoints = ReadPoints(string("current"));

        vector<pt> result = Morph(splinepoints, modelpoints);
        for(int i=0 ; i<result.size(); i++) {
                printf("%f %f\n", result[i].x, result[i].y);
        }

        return 0;
}
*/
