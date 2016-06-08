#include "AnimationLayering.cpp"
#include <gtest/gtest.h>

const double EPSILON = (double)1e-6;

TEST(GetAngleBetweenVectors, can_get_angle) {
  Point a;
  a.x = 7;
  a.y = 10;

  Point b;
  b.x = 5;
  b.y = 6;

  ASSERT_TRUE(fabs(GetAngleBetweenVectors(a, b) - (-2.0344444)) < EPSILON);
}

TEST(DistanceBetweenPoints, gets_correct_distance) {
  Point a;
  a.x = 7;
  a.y = 10;

  Point b;
  b.x = 5;
  b.y = 6;

  ASSERT_TRUE(fabs(DistanceBetweenPoints(a, b) - 4.4721368) < EPSILON);
}

TEST(MovePoint, moves_point_correctly) {
  Point a = {7, 10, 0};
  Point b = {5, 6, 0};
  Point c = {0, 3, 0};
  Point d = {0, 2, 0};

  Point result = MovePoint(a, b, c, d);

  ASSERT_TRUE(fabs(result.x - (-0.447214)) < EPSILON);
  ASSERT_TRUE(fabs(result.y - (1.105573)) < EPSILON);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
