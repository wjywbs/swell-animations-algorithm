#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "point.h"

Point createPoint(double x, double y, double z) {
  Point newPoint;
  newPoint.x = x;
  newPoint.y = y;
  newPoint.z = z;
  return newPoint;
}

/* Add two points */
Point add(const Point l, const Point r) {
  Point tmp = createPoint(l.x + r.x, l.y + r.y, l.z + r.z);
  return tmp;
}

/* Subtract two points */
Point minusPoint(const Point l, const Point r) {
  Point tmp = createPoint(l.x - r.x, l.y - r.y, l.z - r.z);
  return tmp;
}

/* Multiple two points */
Point mult(const Point l, const Point r) {
  Point tmp = createPoint(l.x * r.x, l.y * r.y, l.z * r.z);
  return tmp;
}

/* Multiply a point by a scalar */
Point multScalar(const double scalar, const Point point) {
  Point tmp =
      createPoint(point.x * scalar, point.y * scalar, point.z * scalar);
  return tmp;
}

/* Get the distance between two points */
double getDistance(const Point l, const Point r) {
  Point tmp = minusPoint(l, r);
  double x = tmp.x;
  double y = tmp.y;
  double z = tmp.z;
  return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

/* One sided diff */
Point forwardDiff(const Point a, const Point b) {
  Point tmp = minusPoint(b, a);
  return multScalar(3, tmp);
}

/* Three point diff */
Point midpointDiff(const Point a,
                        const Point b,
                        const Point c) {
  // Define the offset to use
  const double offset = ((double)3) / 2;

  // Since this equation is rather large, we are splitting it up into smaller
  // components
  // Define the first and second term
  Point term1 = multScalar(offset, minusPoint(c, b));
  Point term2 = multScalar(offset, minusPoint(b, a));

  // Define the result value
  return add(term1, term2);
}

double hermiteBasis00(double t) {
  return (2 * pow(t, 3)) - (3 * pow(t, 2)) + 1;
}

double hermiteBasis10(double t) {
  return pow(t, 3) - (2 * pow(t, 2)) + t;
}

double hermiteBasis01(double t) {
  return (-2 * pow(t, 3)) + (3 * pow(t, 2));
}

double hermiteBasis11(double t) {
  return pow(t, 3) - pow(t, 2);
}

Point hermite(const double t,
                   const Point p0,
                   const Point m0,
                   const Point p1,
                   const Point m1) {
  return add(
      add(multScalar(hermiteBasis00(t), p0), multScalar(hermiteBasis10(t), m0)),
      add(multScalar(hermiteBasis01(t), p1),
          multScalar(hermiteBasis11(t), m1)));
}

/* Display a point to the screen */
void displayPoint(const Point& point) {
  printf("(%f, %f, %f)\n", point.x, point.y, point.z);
}
