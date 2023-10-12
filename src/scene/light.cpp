#include <cmath>

#include "light.h"
#include "../ui/TraceUI.h"

#include <iostream>
using namespace std;
extern TraceUI* traceUI;

double DirectionalLight::distanceAttenuation( const vec3f& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation( const vec3f& P ) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	vec3f to_light_dir = DirectionalLight::getDirection(P);
	ray to_light_ray(P, to_light_dir);
	isect i;
	if (scene->intersect(to_light_ray, i))
	{
		const Material& m = i.getMaterial();
		if (!m.kt.iszero())
			return vec3f(m.kt[0], m.kt[1], m.kt[2]);
		else
			return vec3f(0, 0, 0);
	}
	else
		return vec3f(1,1,1);
}

vec3f DirectionalLight::getColor() const
{
	// Color doesn't depend on P 
	return color;
}

vec3f DirectionalLight::getDirection( const vec3f& P ) const
{
	return -orientation;
}

double PointLight::distanceAttenuation( const vec3f& P ) const
{
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0

	double distance = vec3f(P - position).length();
	double a = traceUI->getConstAtten();
	double b = traceUI->getLinearAtten();
	double c = traceUI->getQuadAtten();

	//cout << distance << " : " << a << " : " << constAtten << " : " << b << " : " << linearAtten << " : " << c << " : " << quadAtten << endl;

	double dropOff = 1.0 / (a * constAtten + b * linearAtten * distance * 10 + c * quadAtten * distance * distance * 1000);
	double result = min(1.0, dropOff);
	//cout << result << " : " << dropOff << endl;
	return result;
}

vec3f PointLight::getColor() const
{
	// Color doesn't depend on P 
	return color;
}

vec3f PointLight::getDirection( const vec3f& P ) const
{
	return (position - P).normalize();
}


vec3f PointLight::shadowAttenuation(const vec3f& P) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	vec3f direction = getDirection(P);
	isect i;
	if (scene->intersect(ray(P, direction), i)) {

		const Material& m = i.getMaterial();
		if (!m.kt.iszero())
			return vec3f(m.kt[0], m.kt[1], m.kt[2]);
		else
			return vec3f(0, 0, 0);
	}

	//cout << i.t << endl;
    return vec3f(1,1,1);
}

vec3f AmbientLight::shadowAttenuation(const vec3f& P) const
{
	return vec3f(1, 1, 1);
}

double AmbientLight::distanceAttenuation(const vec3f& P) const
{
	return 1.0;
}

vec3f AmbientLight::getColor() const
{
	return color;
}
