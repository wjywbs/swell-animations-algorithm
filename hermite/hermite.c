#include <stdio.h>

typedef struct pt Point;
struct pt {
	double x;
	double y;
};

Point minus(const Point *l, const Point *r)
{
	Point tmp = {
		.x = l->x - r->x,
		.y = l->y - r->y
	};
	return tmp;
}

Point mult(const Point *l, const Point *r)
{
	Point tmp = {
		.x = l->x * r->x,
		.y = l->y * r->y
	};
	return tmp;
}

Point multScalar(const double scalar, const Point *point) {
	Point tmp = {point->x * scalar, point->y * scalar};
	return tmp;
}

/* One sided diff */
Point forwardDiff(const Point *a, const Point *b)
{
	Point tmp = multScalar(3, minus(b, a));
	return tmp;
}

void displayPoint(const Point *point) {
	printf("(%f, %f)\n", point->x, point->y);
}

int main(int argc, char *argv[])
{
	Point a = {.x=1.0,.y=1.0};
	Point b = {.x=2.0,.y=3.0};
	Point c = minus(&a,&b);
	displayPoint(&c);
	return 0;
}
