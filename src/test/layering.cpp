#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include "../../include/point.h"

using namespace std;

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

void testgabv() {
	pt a;
	a.x = 7;
	a.y = 10;

	pt b;
	b.x = 5;
	b.y = 6;

	printf("%f\n", GetAngleBetweenVectors(a, b)); //2.03
}

double DistanceBetweenPoints(pt a, pt b) {
	    return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}

void testdbp() {
	pt a;
	a.x = 7;
	a.y = 10;

	pt b;
	b.x = 5;
	b.y = 6;

	printf("%f\n", DistanceBetweenPoints(a, b)); //4.47
}

pt MovePoint(pt a_spline, pt b_spline, pt a_model, pt b_model) {
	double theta_spline = GetAngleBetweenVectors(a_spline, b_spline);
	double dist_spline  = DistanceBetweenPoints(a_spline, b_spline);
	double dist_model   = DistanceBetweenPoints(a_model, b_model);

	double ratio = dist_model/dist_spline;

	double new_x = b_model.x + (cos(theta_spline)*ratio);
	double new_y = b_model.y + (sin(theta_spline)*ratio);

	pt new_variant = {new_x, new_y, 0};
	return new_variant;

}

void testmp() {
	pt a = {7, 10, 0};
	pt b = {5, 6, 0};
	pt c = {0, 3, 0};
	pt d = {0, 2, 0};

	pt result = MovePoint(a, b, c, d);
	printf("%f, %f\n", result.x, result.y); // -0.1, 1.79
}

vector<pt> Morph(vector<pt> detail, vector<pt> model) {
	vector<int>control;
	control.push_back(0);
	control.push_back(detail.size()*25/100);
	control.push_back(detail.size()*50/100);
	control.push_back(detail.size()*75/100);
	control.push_back(detail.size()-1);

	for (int i = 0; i < control.size() - 1; i++) {
		pt new_model_point = MovePoint(detail[control[i]], detail[control[i+1]], model[i], model[i+1]);
		model[i+1].x = new_model_point.x;
		model[i+1].y = new_model_point.y;
	}

	return model;
}

int main() {
	vector<pt> splinepoints = ReadPoints(string("spline.out"));
	vector<pt> modelpoints = ReadPoints(string("current"));

	vector<pt> result = Morph(splinepoints, modelpoints);
	for(int i=0 ; i<result.size(); i++) {
		printf("%f, %f\n", result[i].x, result[i].y);
	}

	return 0;
}
