/**
 * To compile, type the following:
 * gcc hermite.c -o hermite -lm
 *
 * We must manually link the math library to make use of the pow function
 */

#include <stdio.h>
#include <math.h>

struct pt {
	double x;
	double y;
};

/* Add two points */
struct pt add(const struct pt *l, const struct pt *r) {
	struct pt tmp = {
		.x = l->x + r->x,
		.y = l->y + r->y
	};
	return tmp;
}

/* Subtract two points */
struct pt minus(const struct pt *l, const struct pt *r)
{
	struct pt tmp = {
		.x = l->x - r->x,
		.y = l->y - r->y
	};
	return tmp;
}

/* Multiple two points */
struct pt mult(const struct pt *l, const struct pt *r)
{
	struct pt tmp = {
		.x = l->x * r->x,
		.y = l->y * r->y
	};
	return tmp;
}

/* Multiply a point by a scalar */
struct pt multScalar(const double scalar, const struct pt *point) {
	struct pt tmp = {point->x * scalar, point->y * scalar};
	return tmp;
}

/* One sided diff */
struct pt forwardDiff(const struct pt *a, const struct pt *b)
{
	struct pt tmp = minus(b, a);
	tmp = multScalar(3, &tmp);
	return tmp;
}

/* Three point diff */
struct pt midpointDiff(const struct pt *a, const struct pt *b, const struct pt *c) {
	// TODO: Implement this function
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

/* Display a point to the screen */
void displayPoint(const struct pt *point) {
	printf("(%f, %f)\n", point->x, point->y);
}

int main(int argc, char *argv[])
{
	struct pt a = {.x=1.0,.y=1.0};
	struct pt b = {.x=2.0,.y=3.0};
	struct pt c = minus(&a,&b);
	displayPoint(&c);
	return 0;
}
