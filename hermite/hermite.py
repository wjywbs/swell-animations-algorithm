#!/usr/bin/env python

import numpy as np
import matplotlib.pyplot as plt

def diff1(a,b):
  """One sided diff"""
  return 3.0 * (b - a)

def diff3(a,b,c):
  """Three point diff"""
  # Assume that the t diff is 1/3
  return 3.0 / 2.0 * (c - b) + 3.0 / 2.0 * (b - a)

def hermiteS(t,a,b,c):
  return (2 * t**3 - 3 * t**2 + 1) * a + \
  (t**3 - 2 * t**2 + t) * diff1(a,b) + \
  (-2 * t**3 + 3 * t**2) * b + \
  (t**3 - t**2) * diff3(a,b,c)

def hermiteE(t,b,c,d):
  return (2 * t**3 - 3 * t**2 + 1) * c + \
  (t**3 - 2 * t**2 + t) * diff3(b,c,d) + \
  (-2 * t**3 + 3 * t**2) * d + \
  (t**3 - t**2) * diff1(c,d)

def hermite(t,a,b,c,d):
  return (2 * t**3 - 3 * t**2 + 1) * b + \
  (t**3 - 2 * t**2 + t) * diff3(a,b,c) + \
  (-2 * t**3 + 3 * t**2) * c + \
  (t**3 - t**2) * diff3(b,c,d)

def plot(pts):
  """Plot list of (x,y) coordinates"""
  x, y = zip(*pts)
  plt.plot(x, y)
  plt.show()

def main():
  # Read in control points
  ctrl_pts = None
  with open('control_points.txt','r') as f:
    ctrl_pts = [np.array(map(float, line.split())) for line in f]

  resolution = 50 # Points per piece of piecewise spline (between 2 control points)
  spline = []

  # Interpolate first segment of spline using forward difference
  for t in range(resolution):
    dt = float(t)/resolution
    spline.append(hermiteS(dt, *ctrl_pts[:3]))

  # Interpolate interior segments of spline 
  for t in range(resolution+1):
    dt = float(t)/resolution
    spline.append(hermite(dt,*ctrl_pts))

  # Interpolate end segment of spline using backward difference
  for t in range(1,resolution+1):
    dt = float(t)/resolution
    spline.append(hermiteE(dt,*ctrl_pts[1:]))

  for l in spline:
	print l

if __name__ == "__main__":
  main()
