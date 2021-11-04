#pragma once

#include <cmath>

#include "Math.h"

class Perlin
{
public:
	float noise( const vec3& p ) const;

	float turbulence( const vec3& p, int depth = 7 ) const;

private:

	static vec3*	mRandomVec3;
	static int*		mPermuteX;
	static int*		mPermuteY;
	static int*		mPermuteZ;

}; // class Perlin
