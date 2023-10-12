#ifndef __BOX_H__
#define __BOX_H__

#include "../scene/scene.h"

class Box
	: public MaterialSceneObject
{
public:
	Box( Scene *scene, Material *mat )
		: MaterialSceneObject( scene, mat )
	{
	}

	virtual bool intersectLocal( const ray& r, isect& i ) const;
	virtual bool hasBoundingBoxCapability() const { return true; }
    virtual BoundingBox ComputeLocalBoundingBox() const
    {
        BoundingBox localbounds;
        localbounds.max = vec3f(0.5, 0.5, 0.5);
		localbounds.min = vec3f(-0.5, -0.5, -0.5);
        return localbounds;
    }
private:
	bool intersectOnPlane(double intersectCoordinate, double planeCoordinate) const {
		return abs(intersectCoordinate - planeCoordinate) <= RAY_EPSILON;
	}
};

#endif // __BOX_H__
