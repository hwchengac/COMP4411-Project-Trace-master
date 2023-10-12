#include "ray.h"
#include "material.h"
#include "light.h"
#include <typeinfo>
#include <iostream>
using namespace std;

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade( Scene *scene, const ray& r, const isect& i ) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
    // You will need to call both distanceAttenuation() and shadowAttenuation()
    // somewhere in your code in order to compute shadows and light falloff.
	vec3f colour = ke;
	vec3f one_minus_kt = vec3f(1.0, 1.0, 1.0) - kt;
	bool ambient_applied = false;

	for (list<Light*>::const_iterator iter = scene->beginLights(); iter != scene->endLights(); ++iter)
	{
		//cout << "light:" << endl;	//debug
		Light* current_light = *iter;
		vec3f light_colour = current_light->getColor();
		if (typeid(**iter) == typeid(AmbientLight) && !ambient_applied)
		{
			vec3f ambient_term(light_colour[0] * ka[0], light_colour[1] * ka[1], light_colour[2] * ka[2]);
			ambient_term = vec3f(ambient_term[0] * one_minus_kt[0], ambient_term[1] * one_minus_kt[1], ambient_term[2] * one_minus_kt[2]);
			colour += ambient_term;
			ambient_applied = true;
		}
		else
		{
			vec3f intersect = r.at(i.t);
			//cout << "intersect: " << intersect[0] << ' ' << intersect[1] << ' ' << intersect[2] << endl;	//debug
			vec3f light_direction = current_light->getDirection(intersect);
			//cout << "light_direction: " << light_direction[0] << ' ' << light_direction[1] << ' ' << light_direction[2] << endl;	//debug
			double dist_atten = minimum(1, current_light->distanceAttenuation(intersect));
			vec3f shadow_atten = current_light->shadowAttenuation(intersect);
			vec3f atten = shadow_atten * dist_atten;
			//cout << "normal: " << i.N[0] << ' ' << i.N[1] << ' ' << i.N[2] << endl;	//debug
			vec3f diffuse_term = kd * maximum(i.N.dot(light_direction), 0);
			diffuse_term = vec3f(diffuse_term[0] * one_minus_kt[0], diffuse_term[1] * one_minus_kt[1], diffuse_term[2] * one_minus_kt[2]);
			//cout << "kd: " << kd[0] << ' ' << kd[1] << ' ' << kd[2] << endl;	//debug
			//cout << "ke: " << ke[0] << ' ' << ke[1] << ' ' << ke[2] << endl;	//debug
			vec3f reflected_ray = 2 * light_direction.dot(i.N) * i.N - light_direction;
			//cout << "reflected_ray . normal: " << reflected_ray.dot(i.N) << endl;	//debug
			//cout << "incident_ray . normal: " << light_direction.dot(i.N) << endl;	//debug
			vec3f viewer_dir = -r.getDirection();
			vec3f specular_term = ks * pow(maximum(viewer_dir.dot(reflected_ray), 0), shininess * 128);
			vec3f temp_light_colour = diffuse_term + specular_term;
			temp_light_colour = vec3f(temp_light_colour[0] * light_colour[0], temp_light_colour[1] * light_colour[1], temp_light_colour[2] * light_colour[2]);
			temp_light_colour = vec3f(temp_light_colour[0] * atten[0], temp_light_colour[1] * atten[1], temp_light_colour[2] * atten[2]);
			colour += temp_light_colour;
			//cout << "diffuse: " << diffuse_term[0] << ' ' << diffuse_term[1] << ' ' << diffuse_term[2] << endl;	//debug
			//cout << "attenuation: " << atten[0] << ' ' << atten[1] << ' ' << atten[2] << endl;	//debug
			//cout << "specular: " << specular_term[0] << ' ' << specular_term[1] << ' ' << specular_term[2] << endl;	//debug
			//cout << "light: " << light_colour[0] << ' ' << light_colour[1] << ' ' << light_colour[2] << endl;	//debug
		}
	}

	colour.clamp();
	return colour;
}
