#!/usr/bin/env python

import numpy as np
import matplotlib.pyplot as plt

# mathplotlib: http://matplotlib.org/users/installing.html

def diff1(a,b):
	"""One sided diff"""
	return 3.0 * (b - a)

def diff3(a,b,c):
	"""Three point diff"""
	# Assume that the t diff is 1/3
	return 3.0 / 2.0 * (c - b) + 3.0 / 2.0 * (b - a)

def h00(t):
	return ( 2 * t ** 3 ) - ( 3 * t ** 2 ) + 1

def h10(t):
	return ( t ** 3 ) - ( 2 * t ** 2 ) + t

def h01(t):
	return ( -2 * t ** 3 ) + ( 3 * t ** 2 )

def h11(t):
	return ( t ** 3 ) - ( t ** 2 )

def hermite(t, p0, m0, p1, m1):
	return ( h00(t) * p0 ) + ( h10(t) * m0 ) + ( h01(t) * p1 ) + ( h11(t) * m1 )

def hermite_spline(p0, m0, p1, m1, n):
	return [hermite(float(t)/n, p0, m0, p1, m1) for t in range(1, n+1)]

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
	n = len(ctrl_pts)

	resolution = 50 # Points per piece of piecewise spline (between 2 control points)
	spline = ctrl_pts[:1]

	# Interpolate first segment of spline using forward difference
	p0 = ctrl_pts[0]
	m0 = diff1(*ctrl_pts[:2])
	p1 = ctrl_pts[1]
	m1 = diff3(*ctrl_pts[:3])
	spline += hermite_spline(p0, m0, p1, m1, resolution)

	# Interpolate interior segments of spline 
	for i in range(1, n-2):
		p0 = ctrl_pts[i]
		m0 = diff3(*ctrl_pts[i-1:i+2])
		p1 = ctrl_pts[i+1]
		m1 = diff3(*ctrl_pts[i:i+3])
		spline += hermite_spline(p0, m0, p1, m1, resolution)

	# Interpolate end segment of spline using backward difference
	p0 = ctrl_pts[n-2]
	m0 = diff3(*ctrl_pts[n-3:])
	p1 = ctrl_pts[n-1]
	m1 = diff1(*ctrl_pts[n-2:])
	spline += hermite_spline(p0, m0, p1, m1, resolution)

        for i in range(len(spline)):
            print spline[i][0], spline[i][1]

	# Plot that shit!
	plot(spline)

if __name__ == "__main__":
	main()
