#pragma once

#include "ray.h"

class Material;
class AABB;

struct HitRecord
{
	float		t;
	float		u;
	float		v;
	vec3		p;
	vec3		normal;
	Material*	material;
};

class Traceable
{
public:
	virtual bool hit( const Ray& r, float t_min, float t_max, HitRecord& rec ) const = 0;

	// Returns true if this object has a bounding box, and initializes box
	// to contain that bounding box's parameters; objects with infinite extent
	// (e.g. planes) should return false, and the box argument is not touched.
	virtual bool getBoundingBox( float t0, float t1, AABB& box ) const = 0;

}; // class Traceable
