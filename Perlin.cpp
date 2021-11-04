#include "stdafx.h"

#include "Perlin.h"

static float interpolate( vec3 c[ 2 ][ 2 ][ 2 ], float u, float v, float w )
{
	// Use Hermite interpolation to reduce the amount of Mach banding
	float uu = u * u * ( 3.0f - 2.0f * u );
	float vv = v * v * ( 3.0f - 2.0f * v );
	float ww = w * w * ( 3.0f - 2.0f * w );

	float accumulator = 0.0f;
	for( int i = 0; i < 2; ++i )
	{
		for( int j = 0; j < 2; ++j )
		{
			for( int k = 0; k < 2; ++k )
			{
				vec3 weight( u - i, v - j, w - k );
				accumulator += ( i * uu + ( 1.0f - i ) * ( 1.0f - uu ) ) *
					( j * vv + ( 1.0f - j ) * ( 1.0f - vv ) ) *
					( k * ww + ( 1.0f - k ) * ( 1.0f - ww ) ) *
					Dot( c[ i ][ j ][ k ], weight );
			}
		}
	}
	return accumulator;
}

float Perlin::noise( const vec3& p ) const
{
	float u = p.x - floorf( p.x );
	float v = p.y - floorf( p.y );
	float w = p.z - floorf( p.z );
	int i = int( floorf( p.x ) );
	int j = int( floorf( p.y ) );
	int k = int( floorf( p.z ) );
	vec3 c[ 2 ][ 2 ][ 2 ];
	for( int di = 0; di < 2; ++di )
	{
		for( int dj = 0; dj < 2; ++dj )
		{
			for( int dk = 0; dk < 2; ++dk )
			{
				c[ di ][ dj ][ dk ] = mRandomVec3[
					mPermuteX[ ( i + di ) & 255 ] ^
					mPermuteY[ ( j + dj ) & 255 ] ^
					mPermuteZ[ ( k + dk ) & 255 ] ];
			}
		}
	}
	return interpolate( c, u, v, w );
}

float Perlin::turbulence( const vec3& p, int depth /* = 7 */ ) const
{
	float accumulator = 0.0f;
	vec3 temp = p;
	float weight = 1.0f;
	for( int i = 0; i < depth; ++i )
	{
		accumulator += weight * noise( temp );
		weight *= 0.5f;
		temp *= 2.0f;
	}

	return fabs( accumulator );
}

// Callers will need to delete[] the returned array when done
static vec3* PerlinGenerate( void )
{
	static const int gTableSize = 256;
	vec3* p = new vec3[ gTableSize ];

	for( int i = 0; i < gTableSize; ++i )
	{
		p[ i ] = vec3( 2.0f * RandomFloat() - 1.0f,
					   2.0f * RandomFloat() - 1.0f,
					   2.0f * RandomFloat() - 1.0f ).getNormalized();
	}

	return p;
}

static void Permute( int* p, int n )
{
	for( int i = n - 1; i > 0; i-- )
	{
		int target = int( RandomFloat() * ( i + 1 ) );
		int temp = p[ i ];
		p[ i ] = p[ target ];
		p[ target ] = temp;
	}
}

// Callers will need to delete[] the returned array
static int* PerlinGeneratePermutation( void )
{
	static const int gTableSize = 256;
	int* p = new int[ gTableSize ];
	for( int i = 0; i < gTableSize; ++i )
	{
		p[ i ] = i;
	}
	Permute( p, 256 );
	return p;
}

vec3* Perlin::mRandomVec3 = PerlinGenerate();
int* Perlin::mPermuteX = PerlinGeneratePermutation();
int* Perlin::mPermuteY = PerlinGeneratePermutation();
int* Perlin::mPermuteZ = PerlinGeneratePermutation();
