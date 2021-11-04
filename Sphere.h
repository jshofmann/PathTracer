#pragma once

#include "Traceable.h"

class Sphere : public Traceable
{
public:
	Sphere()
		: mA( 0.0f, 0.0f, 0.0f )
		, mB( 0.0f, 0.0f, 0.0f )
		, mTime0( 0.0f )
		, mTime1( 0.0f )
		, mRadius( 1.0f )
		, mMaterial( nullptr )
	{}

	Sphere( const vec3& center, float radius, Material* material )
		: mA( center )
		, mB( center )
		, mTime0( 0.0f )
		, mTime1( 0.0f )
		, mRadius( radius )
		, mMaterial( material )
	{}

	// The sphere's center moves from a to b over the time span t1 - t0,
	// measured in seconds; the sphere is at a at t0 and b at t1.
	// If t0 == t1 the sphere is stationary.
	Sphere( const vec3& a, const vec3& b, float t0, float t1,
			float radius, Material* material )
		: mA( a )
		, mB( b )
		, mTime0( t0 )
		, mTime1( t1 )
		, mRadius( radius )
		, mMaterial( material )
	{}

	// Traceable interface implementation

	virtual bool hit( const Ray& r, float tmin, float tmax, HitRecord& hit ) const;

	virtual bool getBoundingBox( float t0, float t1, AABB& box ) const;

	// Sphere member functions

	vec3 getCenter( float time ) const
	{
		if( mTime0 == mTime1 ) // avoid divide-by-zero...
		{
			return mA;
		}
		else
		{
			return mA + ( ( time - mTime0 ) / ( mTime1 - mTime0 ) ) * ( mB - mA );
		}
	}

private:
	vec3		mA, mB; // The endpoints of the path the center follows
	float		mTime0, mTime1; // in seconds
	float		mRadius;
	Material*	mMaterial;
};
