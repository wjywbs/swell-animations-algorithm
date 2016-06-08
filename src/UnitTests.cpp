#include "hermite.cpp"
#include <gtest/gtest.h>

const double EPSILON = (double)1e-6;

TEST(hermite_create_Point, can_create) {
  Point p = createPoint(1, 2, 3);
  ASSERT_EQ(1, p.x);
  ASSERT_EQ(2, p.y);
  ASSERT_EQ(3, p.z);

  p = createPoint(7.9, 2.3, 198.54);
  ASSERT_EQ(7.9, p.x);
  ASSERT_EQ(2.3, p.y);
  ASSERT_EQ(198.54, p.z);
}

TEST(hermite_add, can_add) {
  Point p, q, r;
  p = createPoint(1, 2, 3);
  q = createPoint(7, 8, 9);
  r = add(p, q);
  ASSERT_EQ(8, p.x + q.x);
  ASSERT_EQ(10, p.y + q.y);
  ASSERT_EQ(12, p.z + q.z);

  p = createPoint(76.4, 2.44, 76.9);
  q = createPoint(12.8, 8.23, 9.1);
  r = add(p, q);
  ASSERT_EQ(89.2, p.x + q.x);
  ASSERT_EQ(10.67, p.y + q.y);
  ASSERT_EQ(86.0, p.z + q.z);
}

TEST(hermite_minusPoint, can_subtract) {
  Point p, q, r;
  p = createPoint(7, 8, 9);
  q = createPoint(1, 2, 3);
  r = minusPoint(p, q);
  ASSERT_TRUE(fabs(6 - (p.x - q.x)) < EPSILON);
  ASSERT_TRUE(fabs(6 - (p.y - q.y)) < EPSILON);
  ASSERT_TRUE(fabs(6 - (p.z - q.z)) < EPSILON);

  p = createPoint(76.4, 2.44, 76.9);
  q = createPoint(12.8, 8.23, 9.1);
  r = minusPoint(p, q);
  ASSERT_TRUE(fabs(63.6 - (p.x - q.x)) < EPSILON);
  ASSERT_TRUE(fabs(-5.79 - (p.y - q.y)) < EPSILON);
  ASSERT_TRUE(fabs(67.8 - (p.z - q.z)) < EPSILON);
}

TEST(hermite_mult, can_multiply) {
  Point p, q, r;
  p = createPoint(7, 8, 9);
  q = createPoint(1, 2, 3);
  r = mult(p, q);
  ASSERT_TRUE(fabs(7 - (r.x)) < EPSILON);
  ASSERT_TRUE(fabs(16 - (r.y)) < EPSILON);
  ASSERT_TRUE(fabs(27 - (r.z)) < EPSILON);

  p = createPoint(76.4, 2.44, 76.9);
  q = createPoint(12.8, 8.23, 9.1);
  r = mult(p, q);
  ASSERT_TRUE(fabs(977.92 - (r.x)) < EPSILON);
  ASSERT_TRUE(fabs(20.0812 - (r.y)) < EPSILON);
  ASSERT_TRUE(fabs(699.79 - (r.z)) < EPSILON);
}

TEST(hermite_multScalar, can_multiply_scalars) {
  Point p, r;
  double scalar = 5;
  p = createPoint(7, 8, 9);
  r = multScalar(scalar, p);
  ASSERT_EQ(35, r.x);
  ASSERT_EQ(40, r.y);
  ASSERT_EQ(45, r.z);

  p = createPoint(43.9, 53.87, 12.8);
  scalar = 11;
  r = multScalar(scalar, p);
  ASSERT_TRUE(fabs(482.9 - r.x) < EPSILON);
  ASSERT_TRUE(fabs(592.57 - r.y) < EPSILON);
  ASSERT_TRUE(fabs(140.8 - r.z) < EPSILON);
}

TEST(hermite_getDistance, can_get_distance) {
  Point p, q;
  p = createPoint(1, 2, 3);
  q = createPoint(7, 8, 9);
  double dist = getDistance(p, q);
  ASSERT_TRUE(fabs((double)10.392305 - dist) < EPSILON);
}

TEST(hermite_forwardDiff, can_get_forward_diff) {
  Point point1 = createPoint(1, 1, 1);
  Point point2 = createPoint(2, 3, 1);
  Point point3 = minusPoint(point2, point1);
  Point point4 = multScalar(3, point3);
  ASSERT_EQ(3, point4.x);
  ASSERT_EQ(6, point4.y);
  ASSERT_EQ(0, point4.z);

  point1 = createPoint(-5, 2, -9);
  point2 = createPoint(11, 19, -26);
  point3 = minusPoint(point2, point1);
  point4 = multScalar(3, point3);
  ASSERT_EQ(48, point4.x);
  ASSERT_EQ(51, point4.y);
  ASSERT_EQ(-51, point4.z);
}

TEST(hermite_midpointDiff, can_get_midpoint_diff) {
  Point point1 = createPoint(11.5, 7, 9);
  Point point2 = createPoint(4, 1.75, 2);
  Point point3 = createPoint(6.4, 13, 2.1);
  Point point4 = midpointDiff(point1, point2, point3);
  ASSERT_TRUE(fabs(-7.65 - (point4.x)) < EPSILON);
  ASSERT_TRUE(fabs(9 - (point4.y)) < EPSILON);
  ASSERT_TRUE(fabs(-10.35 - (point4.z)) < EPSILON);

  point1 = createPoint(-4, 1, -2);
  point2 = createPoint(3.6, 9.12, 4);
  point3 = createPoint(7.65, 11, 1.15);
  point4 = midpointDiff(point1, point2, point3);
  ASSERT_TRUE(fabs(17.475 - (point4.x)) < EPSILON);
  ASSERT_TRUE(fabs(15 - (point4.y)) < EPSILON);
  ASSERT_TRUE(fabs(4.725 - (point4.z)) < EPSILON);
}

TEST(hermite_hermiteBasis00, can_get_hermite_basis_00) {
  double t = 6;
  double result = hermiteBasis00(t);
  ASSERT_EQ(325, result);

  t = 7.5;
  result = hermiteBasis00(t);
  ASSERT_EQ(676, result);
}

TEST(hermite_hermiteBasis10, can_get_hermite_basis_10) {
  double t = 12;
  double result = hermiteBasis10(t);
  ASSERT_EQ(1452, result);

  t = 3.5;
  result = hermiteBasis10(t);
  ASSERT_EQ(21.875, result);
}

TEST(hermite_hermiteBasis01, can_get_hermite_basis_01) {
  double t = 4;
  double result = hermiteBasis01(t);
  ASSERT_EQ(-80, result);

  t = 9.5;
  result = hermiteBasis01(t);
  ASSERT_EQ(-1444, result);
}

TEST(hermite_hermiteBasis11, can_get_hermite_basis_11) {
  double t = 8;
  double result = hermiteBasis11(t);
  ASSERT_EQ(448, result);

  t = 9.7;
  result = hermiteBasis11(t);
  ASSERT_TRUE(fabs(818.583 - result) < EPSILON);
}

TEST(hermite_hermite, can_get_hermite_spline) {
  double t = 6.3;
  Point p0 = createPoint(16, 2.7, -4.6);
  Point m0 = createPoint(-9, -7.8, 19.45);
  Point p1 = createPoint(4.5, 6, 9.8);
  Point m1 = createPoint(18, 27.6, 22.9);

  Point result = hermite(t, p0, m0, p1, m1);
  ASSERT_TRUE(fabs(6591.499 - (result.x)) < EPSILON);
  ASSERT_TRUE(fabs(3170.8314 - (result.y)) < EPSILON);
  ASSERT_TRUE(fabs(2767.83785 - (result.z)) < EPSILON);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
