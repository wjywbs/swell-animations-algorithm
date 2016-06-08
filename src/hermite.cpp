#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "point.h"

struct pt* createPoint(double x, double y, double z) {
  struct pt* newPoint = (pt*)malloc(sizeof(struct pt));
  newPoint->x = x;
  newPoint->y = y;
  newPoint->z = z;
  return newPoint;
}

/* Add two points */
struct pt* add(const struct pt* l, const struct pt* r) {
  struct pt* tmp = createPoint(l->x + r->x, l->y + r->y, l->z + r->z);
  return tmp;
}

/* Subtract two points */
struct pt* minusPt(const struct pt* l, const struct pt* r) {
  struct pt* tmp = createPoint(l->x - r->x, l->y - r->y, l->z - r->z);
  return tmp;
}

/* Multiple two points */
struct pt* mult(const struct pt* l, const struct pt* r) {
  struct pt* tmp = createPoint(l->x * r->x, l->y * r->y, l->z * r->z);
  return tmp;
}

/* Multiply a point by a scalar */
struct pt* multScalar(const double scalar, const struct pt* point) {
  struct pt* tmp =
      createPoint(point->x * scalar, point->y * scalar, point->z * scalar);
  return tmp;
}

/* Get the distance between two points */
double getDistance(const struct pt* l, const struct pt* r) {
  struct pt* tmp = minusPt(l, r);
  double x = tmp->x;
  double y = tmp->y;
  double z = tmp->z;
  return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

/* One sided diff */
struct pt* forwardDiff(const struct pt* a, const struct pt* b) {
  struct pt* tmp = minusPt(b, a);
  return multScalar(3, tmp);
}

/* Three point diff */
struct pt* midpointDiff(const struct pt* a,
                        const struct pt* b,
                        const struct pt* c) {
  // Define the offset to use
  const double offset = ((double)3) / 2;

  // Since this equation is rather large, we are splitting it up into smaller
  // components
  // Define the first and second term
  struct pt* term1 = multScalar(offset, minusPt(c, b));
  struct pt* term2 = multScalar(offset, minusPt(b, a));

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

struct pt* hermite(const double t,
                   const struct pt* p0,
                   const struct pt* m0,
                   const struct pt* p1,
                   const struct pt* m1) {
  return add(
      add(multScalar(hermiteBasis00(t), p0), multScalar(hermiteBasis10(t), m0)),
      add(multScalar(hermiteBasis01(t), p1),
          multScalar(hermiteBasis11(t), m1)));
}

/* Display a point to the screen */
void displayPoint(const struct pt* point) {
  printf("(%f, %f, %f)\n", point->x, point->y, point->z);
}
