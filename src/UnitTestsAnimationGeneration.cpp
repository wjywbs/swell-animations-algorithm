#include "AnimationGeneration.cpp"
#include <gtest/gtest.h>

const double EPSILON = (double)1e-6;

TEST(animationGeneration_getSplineLength, can_get_spline_length) {
  Point point1 = createPoint(11, 6, 8);
  Point point2 = createPoint(-12, 5, -7);
  Point point3 = createPoint(1, 5, -10);
  Point point4 = createPoint(4, 8, 9);
  vector<Point> spline;
  spline.push_back(point1);
  spline.push_back(point2);
  spline.push_back(point3);
  spline.push_back(point4);
  double splineLength = getSplineLength(spline);
  ASSERT_TRUE(fabs(60.286850 - splineLength) < EPSILON);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
