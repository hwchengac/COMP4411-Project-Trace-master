#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

// The main ray tracer.

#include "scene/scene.h"
#include "scene/ray.h"

class RayTracer
{
public:
    RayTracer();
    ~RayTracer();

    vec3f trace( Scene *scene, double x, double y );
	vec3f traceRay( Scene *scene, const ray& r, const vec3f& thresh, int depth );


	void getBuffer( unsigned char *&buf, int &w, int &h );
	double aspectRatio();
	void traceSetup( int w, int h );
	void traceLines( int start = 0, int stop = 10000000 );
	void tracePixel( int i, int j );

	bool loadScene( char* fn );

	bool sceneLoaded();

private:
	unsigned char *buffer;
	int buffer_width, buffer_height;
	int bufferSize;
	Scene *scene;

	bool m_bSceneLoaded;

	//reflection: accept intersection point and incident ray, return reflected ray
	ray reflection(const isect& i, const ray& incident_ray);
	//check total interal reflection: True for TIR, False for not TIR
	bool TIRCheck(const double n_i, const double n_t, const vec3f& normal, const vec3f incident);
	//refraction: accept intersection point and incident ray, return refracted ray
	ray refraction(const double n_i, const double n_t, const isect& i, const ray& incident_ray);
};

#endif // __RAYTRACER_H__
