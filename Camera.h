#pragma once

#include <cmath>

#include "Math.h"
#include "vec3.h"
#include "Ray.h"

class Camera
{
public:
	Camera();

	// verticalFOV is the vertical field of view specified in degrees;
	// t0 is the time in seconds when the camera shutter opens and t1 is
	// when the shutter closes, so that motion blur can be calculated.
	// If t0 == t1 the object is stationary (the default).
	Camera( vec3 eye, vec3 direction, vec3 up,
			float verticalFOV, float aspectRatio,
			float aperture, float focalDistance,
			float t0 = 0.0f, float t1 = 0.0f );

	Ray getRay( float u, float v ) const;

private:
	vec3 mOrigin;
	vec3 mLowerLeftCorner;
	vec3 mHorizontal;
	vec3 mVertical;
	vec3 mU, mV, mW;
	float mLensRadius;
	float mTime0, mTime1; // seconds

}; // class Camera

inline Camera::Camera()
	: mOrigin( 0.0f, 0.0f, 0.0f )
	, mLowerLeftCorner( -2.0f, -1.0f, -1.0f )
	, mHorizontal( 4.0f, 0.0f, 0.0f )
	, mVertical( 0.0f, 2.0f, 0.0f )
	, mU( 0.0f, 0.0f, 0.0f )
	, mV( 0.0f, 0.0f, 0.0f )
	, mW( 0.0f, 0.0f, 0.0f )
	, mLensRadius( 0.0f )
	, mTime0( 0.0f )
	, mTime1( 0.0f )
{
}

inline Camera::Camera( vec3 eye, vec3 lookat, vec3 up,
					   float verticalFOV, float aspectRatio,
					   float aperture, float focalDistance,
					   float t0, float t1 )
	: mOrigin( eye )
	, mTime0( t0 )
	, mTime1( t1 )
{
	float theta = verticalFOV * float( M_PI / 180.0 ); // degrees to radians
	float halfHeight = tan( theta / 2.0f );
	float halfWidth = aspectRatio * halfHeight;

	// build an orthonormal basis from the eye parameters
	mW = ( eye - lookat ).getNormalized();
	mU = Cross( up, mW ).getNormalized();
	mV = Cross( mW, mU );

	mLowerLeftCorner = eye - halfWidth * focalDistance * mU
						   - halfHeight * focalDistance * mV
						   - focalDistance * mW;
	mHorizontal = 2.0f * halfWidth * focalDistance * mU;
	mVertical = 2.0f * halfHeight * focalDistance * mV;

	mLensRadius = aperture / 2.0f;
}

inline Ray Camera::getRay( float s, float t ) const
{
	if( mLensRadius == 0.0f )
	{
		return Ray( mOrigin, mLowerLeftCorner + s * mHorizontal + t * mVertical - mOrigin );
	}
	else
	{
		vec3 rd = mLensRadius * RandomInUnitDisk();
		vec3 offset = mU * rd.x + mV * rd.y;
		// Pick a random time in the interval the camera shutter is open
		float time = mTime0 + RandomFloat() * ( mTime1 - mTime0 );
		return Ray( mOrigin + offset,
					mLowerLeftCorner + s * mHorizontal + t * mVertical - mOrigin - offset,
					time );
	}
}
