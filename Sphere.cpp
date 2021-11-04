#include "stdafx.h"

#include "Sphere.h"
#include "AABB.h"
#include "Math.h"

bool Sphere::hit( const Ray& ray, float t_min, float t_max, HitRecord& hit ) const
{
	vec3 oc = ray.getOrigin() - getCenter( ray.getTime() );
	float a = Dot( ray.getDirection(), ray.getDirection() );
	float b = Dot( oc, ray.getDirection() );
	float c = Dot( oc, oc ) - mRadius * mRadius;

	// The 4 term in 4 * a * c was cancelled out by the 2 that was in the
	// b term above and the 2 that was in the temp term below
	float discriminant = b * b - a * c;
	if( discriminant > 0.0f )
	{
		float temp = ( -b - sqrtf( discriminant ) ) / a;
		if( temp > t_min && temp < t_max )
		{
			hit.t = temp;
			hit.p = ray.pointAtParameter( hit.t );
			hit.normal = ( hit.p - getCenter( ray.getTime() ) ) / mRadius;
			hit.material = mMaterial;
			return true;
		}

		temp = ( -b + sqrtf( discriminant ) ) / a;
		if( temp > t_min && temp < t_max )
		{
			hit.t = temp;
			hit.p = ray.pointAtParameter( hit.t );
			hit.normal = ( hit.p - getCenter( ray.getTime() ) ) / mRadius;
			hit.material = mMaterial;
			return true;
		}

	} // if( discriminant > 0.0f )

	return false;
}

bool Sphere::getBoundingBox( float t0, float t1, AABB& box ) const
{
	// If this sphere is stationary
	if( t0 == t1 )
	{
		box = AABB( mA - vec3( mRadius, mRadius, mRadius ), mA + vec3( mRadius, mRadius, mRadius ) );
	}
	else
	{
		// Return a region enclosing any place this sphere can be
		AABB boxa( mA - vec3( mRadius, mRadius, mRadius ), mA + vec3( mRadius, mRadius, mRadius ) );
		AABB boxb( mB - vec3( mRadius, mRadius, mRadius ), mB + vec3( mRadius, mRadius, mRadius ) );
		box = enclose( boxa, boxb );
	}

	return true;
}
