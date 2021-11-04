#pragma once

#include <functional>
#include <random>

#include "vec3.h"
#include "AABB.h"

inline float ffmin( float a, float b )
{
	return ( a < b ) ? a : b;
}

inline float ffmax( float a, float b )
{
	return ( a > b ) ? a : b;
}

// Return the union of the two axis-aligned bounding boxes
// ("union" is a C++ reserved keyword, of course)
inline AABB enclose( const AABB& box0, const AABB& box1 )
{
	// "min" and "max" are of course #defined by Microsoft....
	vec3 mins( ffmin( box0.getMin().x, box1.getMin().x ),
			   ffmin( box0.getMin().y, box1.getMin().y ),
			   ffmin( box0.getMin().z, box1.getMin().z ) );

	vec3 maxs( ffmax( box0.getMax().x, box1.getMax().x ),
			   ffmax( box0.getMax().y, box1.getMax().y ),
			   ffmax( box0.getMax().z, box1.getMax().z ) );

	return AABB( mins, maxs );
}

inline float RandomFloat( void )
{
	static thread_local std::uniform_real_distribution< float > distribution( 0.0, 1.0 );
	static thread_local std::mt19937 generator;
	static thread_local std::function< float() > randomGenerator = std::bind( distribution, generator );
	return randomGenerator();
}

inline vec3 RandomInUnitSphere( void )
{
	vec3 p;
	do
	{
		// [ 0, 1 ] -> [ -1, 1 ]
		p = 2.0f * vec3( RandomFloat(), RandomFloat(), RandomFloat() ) - vec3( 1.0f, 1.0f, 1.0f );
	}
	while( p.lengthSquared() >= 1.0f );

	return p;
}

inline vec3 RandomInUnitDisk( void )
{
	vec3 p;

	do
	{
		p = 2.0f * vec3( RandomFloat(), RandomFloat(), 0.0f ) - vec3( 1.0f, 1.0f, 0.0f );
	}
	while( Dot( p, p ) >= 1.0f );

	return p;
}

inline vec3 Reflect( const vec3& v, const vec3& n )
{
	return v - 2.0f * Dot( v, n ) * n;
}

inline bool Refract( const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted )
{
	vec3 uv = v.getNormalized();
	float dt = Dot( uv, n );
	float discriminant = 1.0f - ni_over_nt * ni_over_nt * ( 1.0f - dt * dt );
	if( discriminant > 0.0f )
	{
		refracted = ni_over_nt * ( uv - n * dt ) - n * sqrt( discriminant );
		return true;
	}

	return false;
}

inline float Schlick( float cosine, float refractIndex )
{
	float r0 = ( 1.0f - refractIndex ) / ( 1.0f + refractIndex );
	r0 = r0 * r0;
	return r0 + ( 1.0f - r0 ) * pow( 1.0f - cosine, 5.0f );
}

inline void GetSphereUV( const vec3& p, float& u, float& v )
{
	float phi = atan2( p.z, p.x );
	float theta = asin( p.y );
	u = 1.0f - ( phi + float( M_PI ) ) / ( 2.0f * float( M_PI ) );
	v = ( theta + float( M_PI ) / 2.0f ) / float( M_PI );
}
