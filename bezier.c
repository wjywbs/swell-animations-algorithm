/*
 * Bezier Path Parametric Plotter
 *
 * Chris Khedoo
 * Carlo Rosati
*/

#define _GNU_SOURCE
#include <stdlib.h> /* exit, EXIT_FAILURE, strtod, free */
#include <stdio.h> /* fopen, perror, fclose, printf */
#include <stddef.h> /* size_t, NULL */
#include <math.h> /* pow */

typedef struct
{
	double x, y;
} point;

point control_points[4];

/* Cubic Bernstein basis polynomials */

double
cubic_bernstein_0(double x)
{
	// (1 - x)^3
	return pow((1 - x), 3);
}

double
cubic_bernstein_1(double x)
{
	// 3x(1 - x)^2
	return (3 * x) * pow((1 - x), 2);
}

double
cubic_bernstein_2(double x)
{
	// 3x^2(1 - x)
	return (3 * pow(x, 2)) * (1 - x);
}

double
cubic_bernstein_3(double x)
{
	// x^3
	return pow(x, 3);
}

double (*cubic_bernstein[4]) (double x) = {
	cubic_bernstein_0,
	cubic_bernstein_1,
	cubic_bernstein_2,
	cubic_bernstein_3
};

unsigned int
read_ctrl_pts(char const *file_name)
{
	FILE *fin = fopen(file_name, "r");
	if (fin == NULL)
	{
		perror("File not found.\n");
		exit(EXIT_FAILURE);
	}

	char *line = NULL;
	size_t len = 0;
	size_t read;

	unsigned int num_read = 0;

	while ((read = getline(&line, &len, fin)) != -1)
	{
		char *space;
		control_points[num_read].x = strtod(line, &space);
		control_points[num_read].y = strtod(space, NULL);
		++num_read;
	}

	free(line);
	fclose(fin);

	return num_read;
}

point
evaluate_parameter(double t)
{
	point p = {0, 0};

	for (int j = 0; j < 4; ++j)
	{
		p.x += cubic_bernstein[j](t) * control_points[j].x;
		p.y += cubic_bernstein[j](t) * control_points[j].y;
	}

	return p;
}

void
parametric_bezier_curve(int num_points)
{
	double t = 0;

	for (int i = 0; i < num_points; ++i)
	{
		t = i / (num_points - 1.0);
		point p = evaluate_parameter(t);
		printf("(%f, %f)\n", p.x, p.y);
	}
}

int
main(int argc, char const *argv[])
{
	if (argc < 2)
	{
		perror("Bro I need some points\n");
		exit(EXIT_FAILURE);
	}

	unsigned int n = read_ctrl_pts(argv[1]);
	parametric_bezier_curve(n);

	return 0;
}
