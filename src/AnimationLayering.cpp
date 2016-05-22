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
	//pt *points = (pt*) calloc(1, sizeof(struct pt));
	vector<pt> points;

	while(in) {
		string line;
		if(!getline(in, line)) {
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
	return sqrt( pow(b.x() - a.x(), 2) + pow(b.y() - a.y(), 2) + pow(b.z() - a.z(), 2) );
}

pt MovePoint(pt a_spline, pt b_spline, pt a_model, pt b_model) {
	double theta_spline = GetAngleBetweenVectors(a_spline, b_spline);
	double dist_spline  = DistanceBetweenPoints(a_spline, b_spline);
	double dist_model   = DistanceBetweenPoints(a_model, b_model);

	double ratio = dist_model/dist_spline;

	double new_x = b_model.x + (cos(theta_spline));
	double new_y = b_model.y + (sin(theta_spline));

	pt new_variant = {new_x, new_y, 0};
	return new_variant;
}

/*
 * Moves a vector from sequence { a_model, b_model, c_model } to conform to the
 * curve given by { a_spline, b_spline, c_spline }
 */
void MoveVector(Vector* a_model, Vector* b_model, Vector* a_spline, Vector* b_spline, int dist_to_dloa) {
	double theta_spline = GetAngleBetweenVectors(*a_spline, *b_spline);
	double dist_spline  = DistanceBetweenPoints(*a_spline, *b_spline);
	double dist_model   = DistanceBetweenPoints(*a_model, *b_model);

	double ratio = dist_model/dist_spline;

	double new_x = b_model->x() + (cos(theta_spline)/(dist_to_dloa == 0? 1 : (double)dist_to_dloa));
	double new_y = b_model->y() + (sin(theta_spline)/(dist_to_dloa == 0? 1 : (double)dist_to_dloa));

//b_model->set_x(new_x);
//	b_model->set_y(new_y);
	b_model->set_x(999);
	b_model->set_y(999);
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
		pt new_model_point = MovePoint(detail[control[i]], detail[control[i+1]], model[i], model[i+1]);
		model[i+1].x = new_model_point.x;
		model[i+1].y = new_model_point.y;
	}

	return model;
}*/

void Morph(Node *frames, AnimationLayer* layer, int dloa_size, int dist_to_dloa) {
	Vector *current_position = frames->mutable_position();
	Node *nextchild = frames->mutable_children(0);
	int childcount = 0;
	int childrenleft = 0;

	// Find number of points from child root
	Node *temp = nextchild;
	while(temp->children_size() > 0) {
		childcount++;
		temp = temp->mutable_children(0);
	}
	childrenleft = childcount;
	temp = NULL;

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

	
}

void AddLayering(ModelData *modelData, Animation *animation) {
	ofstream out("/tmp/stufff");
	out << "OMG DO I GET HERE" << endl;
	for(int i=0; i < modelData->animationlayers_size(); i++) {
		out << "OMG DO I GET HERE NOW" << endl;
		for(int j = modelData->animationlayers(i).startframe();
		    j < modelData->animationlayers(i).startframe() +
		   	 modelData->animationlayers(i).numframes();
		    j++) {
			out << "WHAT ABOUT NOW?" << endl;
			Morph(	animation->mutable_frames(j),
				modelData->mutable_animationlayers(i),
				modelData->animationlayers(i).layerpoints_size(),
				modelData->animationlayers(i).startframe() + modelData->animationlayers(i).numframes() - j
			     );
			out << "NOW??" << endl;
		} // end frame counter
	} // end layer counter
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
