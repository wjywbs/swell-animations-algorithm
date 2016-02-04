#include <stdio.h>

struct pt {
	double x;
	double y;
};

struct pt minus(const struct pt *l, const struct pt *r)
{
	struct pt tmp = {
		.x = l->x - r->x,
		.y = l->y - r->y
	};
	return tmp;
}

struct pt mult(const struct pt *l, const struct pt *r)
{
	struct pt tmp = {
		.x = l->x * r->x,
		.y = l->y * r->y
	};
	return tmp;
}

struct pt diff1(const struct pt *a, const struct pt *b)
{
/* TODO(Make this work)
		return mult(3.0, minus(a,b));*/
}

int main(int argc, char *argv[])
{
	struct pt a = {.x=1.0,.y=1.0};
	struct pt b = {.x=2.0,.y=3.0};
	struct pt c = minus(&a,&b);
	printf("(%f,%f)\n",c.x,c.y);
}
