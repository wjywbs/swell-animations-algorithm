/**
 * @author:  Chris Khedoo
 * Testing of functions 7 and on in hermite.cpp (forward_diff to end)
 */

#include <gtest/gtest.h>
#include "hermite.cpp"

TEST(hermite_forwardDiff, test1) {
	struct pt *point1 = createPoint(1, 1, 1);
	struct pt *point2 = createPoint(2, 3, 1);
	struct pt *point3 = minus(point2, point1);
	struct pt *point4 = multScalar(3, point3);

	ASSERT_EQ(3, point4->x);
	ASSERT_EQ(6, point4->y);
	ASSERT_EQ(0, point4->z);

	free(point1);
	free(point2);
	free(point3);
	free(point4);
}

TEST(hermite_forwardDiff, test2) {
	struct pt *point1 = createPoint(-5, 2, -9);
	struct pt *point2 = createPoint(11, 19, -26);
	struct pt *point3 = minus(point2, point1);
	struct pt *point4 = multScalar(3, point3);

	ASSERT_EQ(48, point4->x);
	ASSERT_EQ(51, point4->y);
	ASSERT_EQ(-51, point4->z);

	free(point1);
	free(point2);
	free(point3);
	free(point4);
}


int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
