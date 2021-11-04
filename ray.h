#pragma once

#include "vec3.h"

class Ray
{
public:
	Ray() {}
	Ray( const vec3& origin, const vec3& direction, float time = 0.0f );

	inline const vec3& getOrigin( void ) const;
	inline const vec3& getDirection( void ) const;
	inline float getTime( void ) const;

	inline vec3 pointAtParameter( float t ) const;

private:
	vec3	mOrigin;
	vec3	mDirection;
	float	mTime;

}; // class Ray

inline Ray::Ray( const vec3& origin, const vec3& direction, float time )
	: mOrigin( origin )
	, mDirection( direction )
	, mTime( time )
{
}

inline const vec3& Ray::getOrigin( void ) const
{
	return mOrigin;
}

inline const vec3& Ray::getDirection( void ) const
{
	return mDirection;
}

inline float Ray::getTime( void ) const
{
	return mTime;
}

inline vec3 Ray::pointAtParameter( float t ) const
{
	return mOrigin + t * mDirection;
}
