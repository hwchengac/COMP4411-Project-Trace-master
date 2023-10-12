// The main ray tracer.

#include <Fl/fl_ask.h>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"
#include "ui/TraceUI.h"

#include <iostream>
using namespace std;

extern TraceUI* traceUI;

ray RayTracer::reflection(const isect& i, const ray& incident_ray)
{
	vec3f ray_direction = -incident_ray.getDirection();
	vec3f normal;
	if (incident_ray.getEnteringObject())
		normal = i.N;
	else
		normal = -i.N;
	//cout << "incident_ray . normal: " << ray_direction.dot(i.N) << endl;	//debug
	ray_direction = 2 * normal.dot(ray_direction) * normal - ray_direction;
	//cout << "reflected_ray . normal: " << ray_direction.dot(i.N) << endl;	//debug
	ray reflected_ray(incident_ray.at(i.t), ray_direction);
	reflected_ray.toggleEnteringObject(incident_ray.getEnteringObject());
	return reflected_ray;
}

bool RayTracer::TIRCheck(const double n_i, const double n_t, const vec3f& normal, const vec3f incident)
{
	if (n_t > n_i)	//check if it is possible to happen
		return false;
	
	double critical_angle = asin(n_t / n_i);
	//cout << "critical_angle: " << critical_angle << endl;	//debug
	double incident_angle = acos(normal.dot(incident));
	//cout << "incident_angle: " << incident_angle << endl;	//debug
	if (incident_angle > critical_angle)
		return true;
	else
		return false;
}

ray RayTracer::refraction(const double n_i, const double n_t, const isect& i, const ray& incident_ray)
{
	//if (n_i == n_t )
	//{
	//	ray refracted_ray(incident_ray.at(i.t), incident_ray.getDirection());
	//	bool entering_object = !incident_ray.getEnteringObject();
	//	refracted_ray.toggleEnteringObject(entering_object);
	//	return refracted_ray;
	//}
	double index = n_i / n_t;
	vec3f ray_direction = -incident_ray.getDirection();
	vec3f normal;
	if (incident_ray.getEnteringObject())
		normal = i.N;
	else
		normal = -i.N;
	double cos_incident_angle = normal.dot(ray_direction);
	double cos_trans_angle = sqrt(1 - index * index * (1 - cos_incident_angle * cos_incident_angle));
	ray_direction = (index * cos_incident_angle - cos_trans_angle) * normal - index * ray_direction;
	ray refracted_ray(incident_ray.at(i.t), ray_direction);
	bool entering_object = !incident_ray.getEnteringObject();
	refracted_ray.toggleEnteringObject(entering_object);
	return refracted_ray;
}

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace( Scene *scene, double x, double y )
{
	ray r(vec3f(0, 0, 0), vec3f(0, 0, 0));
    scene->getCamera()->rayThrough( x,y,r );
	return traceRay( scene, r, vec3f(1.0,1.0,1.0), 0 ).clamp();
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay( Scene *scene, const ray& r, 
	const vec3f& thresh, int depth )
{
	isect i;

	if( scene->intersect( r, i ) ) {
		// YOUR CODE HERE

		// An intersection occured!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.
		
		vec3f colour;
		const Material& m = i.getMaterial();
		colour = m.shade(scene, r, i);
		if (depth < traceUI->getDepth())
		{
			//reflection
			const ray reflected_ray = reflection(i, r);
			vec3f reflected_colour = traceRay(scene, reflected_ray, thresh, depth + 1);
			if (!m.kr.iszero())
				reflected_colour = vec3f(reflected_colour[0] * m.kr[0], reflected_colour[1] * m.kr[1], reflected_colour[2] * m.kr[2]);
			else
				reflected_colour = vec3f(reflected_colour[0] * m.ks[0], reflected_colour[1] * m.ks[1], reflected_colour[2] * m.ks[2]);
			colour += reflected_colour;

			//refraction
			if (!m.kt.iszero() || abs(m.index - 1.0) < RAY_EPSILON)
			{
				double n_i;
				double n_t;
				if (r.getEnteringObject())
				{
					n_i = 1;
					n_t = m.index;
				}
				else
				{
					n_i = m.index;
					n_t = 1;
				}
				//cout.precision(10);
				//cout << "n_i:" << n_i << endl;	//debug
				//cout.precision(10);
				//cout << "n_t:" << n_t << endl;	//debug
				vec3f ray_direction = -r.getDirection();
				bool tir_check;
				if (r.getEnteringObject())
					tir_check = TIRCheck(n_i, n_t, i.N, ray_direction);
				else
				{
					vec3f normal = -i.N;
					tir_check = TIRCheck(n_i, n_t, normal, ray_direction);
				}
				//cout << "TIR: " << tir_check << endl;	//debug
				if (!tir_check)
				{
					const ray refracted_ray = refraction(n_i, n_t, i, r);
					vec3f refracted_colour = traceRay(scene, refracted_ray, thresh, depth + 1);
					refracted_colour = vec3f(refracted_colour[0] * m.kt[0], refracted_colour[1] * m.kt[1], refracted_colour[2] * m.kt[2]);
					colour += refracted_colour;
				}
			}
		}
		colour.clamp();
		return colour;
	
	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.

		return vec3f( 0.0, 0.0, 0.0 );
	}
}

RayTracer::RayTracer()
{
	buffer = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;

	m_bSceneLoaded = false;
}


RayTracer::~RayTracer()
{
	delete [] buffer;
	delete scene;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return scene ? scene->getCamera()->getAspectRatio() : 1;
}

bool RayTracer::sceneLoaded()
{
	return m_bSceneLoaded;
}

bool RayTracer::loadScene( char* fn )
{
	try
	{
		scene = readScene( fn );
	}
	catch( ParseError pe )
	{
		fl_alert( "ParseError: %s\n", pe );
		return false;
	}

	if( !scene )
		return false;
	
	buffer_width = 256;
	buffer_height = (int)(buffer_width / scene->getCamera()->getAspectRatio() + 0.5);

	bufferSize = buffer_width * buffer_height * 3;
	buffer = new unsigned char[ bufferSize ];
	
	// separate objects into bounded and unbounded
	scene->initScene();
	
	// Add any specialized scene loading code here
	
	m_bSceneLoaded = true;

	return true;
}

void RayTracer::traceSetup( int w, int h )
{
	if( buffer_width != w || buffer_height != h )
	{
		buffer_width = w;
		buffer_height = h;

		bufferSize = buffer_width * buffer_height * 3;
		delete [] buffer;
		buffer = new unsigned char[ bufferSize ];
	}
	memset( buffer, 0, w*h*3 );
}

void RayTracer::traceLines( int start, int stop )
{
	vec3f col;
	if( !scene )
		return;

	if( stop > buffer_height )
		stop = buffer_height;

	for( int j = start; j < stop; ++j )
		for( int i = 0; i < buffer_width; ++i )
			tracePixel(i,j);
}

void RayTracer::tracePixel( int i, int j )
{
	vec3f col;

	if( !scene )
		return;

	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);

	col = trace( scene,x,y );

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}