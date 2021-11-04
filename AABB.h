#pragma once

#include "vec3.h"

class Ray;

class AABB
{
public:
	AABB()
		: mMin( 0.0f, 0.0f, 0.0f )
		, mMax( 0.0f, 0.0f, 0.0f )
	{}

	AABB( const vec3& min, const vec3& max )
		: mMin( min )
		, mMax( max )
	{}

	inline const vec3& getMin( void ) const
	{
		return mMin;
	}

	inline const vec3& getMax( void ) const
	{
		return mMax;
	}

	bool hit( const Ray& r, float tmin, float tmax ) const;

private:
	vec3 mMin;
	vec3 mMax;

}; // class AABB
