#include <stdio.h>

struct pt {
	double x;
	double y;
};

struct pt minus(struct pt *l, struct pt *r)
{
	struct pt diff = {
		.x = l->x - r->x,
		.y = l->y - r->y
	};
	return diff;
}

struct pt mult(struct pt *l, struct pt *r)
{
	struct pt mult = {
		.x = l->x * r->x,
		.y = l->y * r->y
	};
	return mult;
}

struct pt diff1(struct pt *a, struct pt *b)
{
/* TODO(Make this work)
		return mult(3.0, minus(a,b));*/
}

int main(int argc, char **argv)
{
	struct pt a = {.x=1.0,.y=1.0};
	struct pt b = {.x=2.0,.y=3.0};
	struct pt c = minus(&a,&b);
	printf("(%f,%f)\n",c.x,c.y);
}
