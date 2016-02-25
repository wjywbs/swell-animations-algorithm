#include<stdio.h>
#include<stdlib.h>
#include<math.h>

struct Direction {
	double x;
	double y;
	double z;
};

/*
 * This routine will use a pre-worked out differenciation on the output of the
 * hermite spline program in order to provide us with the direction in which
 * the object will be moving.
 * Arguments:
 * 	a : x0 or y0 or z0
 * 	b : x1 or y1 or z1
 * 	c : x2 or y2 or z2
 * 	t : contant
 * 	location : pointer to Direction{x, y, or z}
 * Usage:
 * 	struct Direction *d = calloc(1, sizeof(struct Direction));
 * 	Differenciate(x0, x1, x2, t, &(d->x));
 * 	Differenciate(y0, y1, y2, t, &(d->y));
 * 	Differenciate(z0, z1, z2, t, &(d->z));
 * Result: The direction of a point in relation to the ones preceeding and
 * 	succeeding it in 3D space.
 * 	d = <x, y, z>
 */
void Differenciate(double a, double b, double c, double t, double *location) {
	// Break it up so I don't get confused later
	double tempA = (6*pow(t, 2)*a) - (6*t*a);
	double tempB = (3*pow(t, 2)*(b-a)) - (4*t*(b-a)) - (b-a);
	double tempC = (-6*pow(t, 2)*b) + (6*t*b);
	double tempD = (3*pow(t, 2)*((c-a)/2)) - (2*t*((c-a)/2));

	*location = tempA + tempB + tempC + tempD;
}
