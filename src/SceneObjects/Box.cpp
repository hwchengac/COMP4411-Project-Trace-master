#include <cmath>
#include <assert.h>
#include <iostream>

#include "Box.h"

using namespace std;

bool Box::intersectLocal( const ray& r, isect& i ) const
{
	// YOUR CODE HERE:
    // Add box intersection code here.
	// it currently ignores all boxes and just returns false.
	// 
	// tNear and tFar
	double tNear, tFar;
	bool intersect = ComputeLocalBoundingBox().intersect(r, tNear, tFar);
	if (intersect && (tNear > RAY_EPSILON)) {
		//cout << "hit box" << endl;
		// Set intersected object as this
		i.obj = this;
		// Modify i
		i.t = tNear;
		vec3f isectPoint = r.at(tNear);
		//cout << isectPoint[0] << " : " << isectPoint[1] << " : " << isectPoint[2] << endl;
		// Set the normal of the intersected point
		if (intersectOnPlane(isectPoint[0], 0.5)) {		// if the intersect x is positive, intersect normal should be positive
			i.N = vec3f(1, 0, 0);
		}
		else if (intersectOnPlane(isectPoint[0], -0.5)) {
			i.N = vec3f(-1, 0, 0);
		} 
		else if (intersectOnPlane(isectPoint[1], 0.5)) {		// if the intersect y is positive, intersect normal should be positive
			i.N = vec3f(0, 1, 0);
		}
		else if (intersectOnPlane(isectPoint[1], -0.5)) {
			i.N = vec3f(0, -1, 0);
		}
		else if (intersectOnPlane(isectPoint[2], 0.5)) {		// if the intersect z is positive, intersect normal should be positive
			i.N = vec3f(0, 0, 1);
		}
		else if (intersectOnPlane(isectPoint[2], -0.5)) {
			i.N = vec3f(0, 0, -1);
		}

		return true;
	}
	//cout << "no hit box" << endl;
	return false;



}
