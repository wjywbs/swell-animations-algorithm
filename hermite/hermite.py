#!/usr/bin/env python

import numpy as np

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

def main():
  with open('control_points.txt','r') as f:
    pts = []
    for line in f:
      pt = line.split()
      x = float(pt[0])
      y = float(pt[1])
      pts.append(np.array([x,y]))

    # Printing out the interpolating points
    resolution = 50

    for x in range(resolution):
      dt = float(x)/resolution
      print hermiteS(dt,*pts[:3])

    # Print the interior points
    for x in range(resolution+1):
      dt = float(x)/resolution
#      print dt,hermite(dt,*pts)
      print hermite(dt,*pts)

    # Print the end segment points
    for x in range(1,resolution+1):
      dt = float(x)/resolution
      print hermiteE(dt,*pts[1:])

if __name__ == "__main__":
  main()
