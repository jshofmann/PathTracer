#include "stdafx.h"

#include <cmath>
#include <cfloat>
#include <cassert>
#include <thread>

#include "PathTracer.h"
#include "TGAWriter.h"
#include "Math.h"
#include "vec3.h"
#include "Ray.h"
#include "Scene.h"
#include "Sphere.h"
#include "Camera.h"
#include "Material.h"
#include "Rect.h"

PathTracer::PathTracer()
	: mSampleCount( 100 )
	, mWidth( 0 )
	, mHeight( 0 )
	, mBytesPerPixel( 0 )
	, mPixels( nullptr )
	, mCamera( nullptr )
	, mScene( nullptr )
	, mProgressCallback( nullptr )
	, mProgressCallbackData( nullptr )
	, mCompleteCallback( nullptr )
	, mCompleteCallbackData( nullptr )
{
}

PathTracer::~PathTracer()
{
	if( mPixels != nullptr )
	{
		delete[] mPixels;
		mPixels = nullptr;
	}
}

bool PathTracer::initialize( uint16_t width, uint16_t height )
{
	mWidth = width;
	mHeight = height;
	mBytesPerPixel = 3; // RGB

	mPixels = new uint8_t[ mWidth * mHeight * mBytesPerPixel ];

	return mPixels != nullptr;
}

void PathTracer::setProgressCallback( ProgressCallback callback, void* data )
{
	mProgressCallback = callback;
	mProgressCallbackData = data;
}

void PathTracer::setCompleteCallback( CompleteCallback callback, void* data )
{
	mCompleteCallback = callback;
	mCompleteCallbackData = data;
}

void PathTracer::saveImage( const char* filename ) const
{
	TGAWriter::write( mPixels, mWidth, mHeight, mBytesPerPixel, filename );
}

void PathTracer::startTrace( void )
{
#if 1

#if 1
	vec3 eye( 23.0f, 2.0f, 3.0f );
	vec3 lookat( 0.0f, 0.0f, 0.0f );
	vec3 up( 0.0f, 1.0f, 0.0f );
	float verticalFOV = 20.0f; // degrees
	float aspect = float( mWidth ) / float( mHeight );
	float focalDistance = 10.0f; // ( eye - lookat ).length();
	float aperture = 0.1f;
#else
	vec3 eye( 278.0f, 278.0f, -800.0f );
	vec3 lookat( 278.0f, 278.0f, 0.0f );
	vec3 up( 0.0f, 1.0f, 0.0f );
	float verticalFOV = 40.0f; // degrees
	float aspect = float( mWidth ) / float( mHeight );
	float focalDistance = 10.0f; // ( eye - lookat ).length();
	float aperture = 0.0f;
#endif

	mCamera = new Camera( eye, lookat, up, verticalFOV, aspect, aperture, focalDistance, 0.0f, 1.0f );

//	mScene = createRandomScene();
	mScene = createTwoPerlinSpheres();

#else

#if 0

	const uint32_t kTraceableCount = 2;
	Traceable* list[ kTraceableCount ];

	float r = float( cos( M_PI / 4.0f ) );
	list[ 0 ] = new Sphere( vec3( -r, 0.0f, -1.0f ), r, new Lambertian( vec3( 0.0f, 0.0f, 1.0f ) ) );
	list[ 1 ] = new Sphere( vec3( r, 0.0f, -1.0f ), r, new Lambertian( vec3( 1.0f, 0.0f, 0.0f ) ) );

	vec3 eye( 3.0f, 3.0f, 2.0f );
	vec3 lookat( 0.0f, 0.0f, -1.0f );
	vec3 up( 0.0f, 1.0f, 0.0f );
	float aspect = float( mWidth ) / float( mHeight );
	float focalDistance = ( eye - lookat ).length();
	float aperture = 2.0f;

	mCamera  = new Camera( eye, lookat, up, 90.0f, aspect, aperture, focalDistance );

	mScene = new Scene;
	if( !mScene->initialize( list, kTraceableCount ) )
		return 0;

#else

	const uint32_t kTraceableCount = 5;
	Traceable* list[ kTraceableCount ];
	list[ 0 ] = new Sphere( vec3( 0.0f, 0.0f, -1.0f ), 0.5f, new Lambertian( vec3( 0.1f, 0.2f, 0.5f ) ) );
	list[ 1 ] = new Sphere( vec3( 0.0f, -100.5f, -1.0f ), 100.0f, new Lambertian( vec3( 0.8f, 0.8f, 0.0f ) ) );
	list[ 2 ] = new Sphere( vec3( 1.0f, 0.0f, -1.0f ), 0.5f, new Metal( vec3( 0.8f, 0.6f, 0.2f ), 0.0f ) );
	list[ 3 ] = new Sphere( vec3( -1.0f, 0.0f, -1.0f ), 0.5f, new Glass( 1.5f ) );
	list[ 4 ] = new Sphere( vec3( -1.0f, 0.0f, -1.0f ), -0.45f, new Glass( 1.5f ) );

	vec3 eye( 3.0f, 3.0f, 2.0f );
	vec3 lookat( 0.0f, 0.0f, -1.0f );
	vec3 up( 0.0f, 1.0f, 0.0f );
	float aspect = float( mWidth ) / float( mHeight );
	float focalDistance = ( eye - lookat ).length();
	float aperture = 1.0f / 2.0f;

	mCamera  = new Camera( eye, lookat, up, 20.0f, aspect, aperture, focalDistance );

	mScene = new Scene;
	if( !mScene->initialize( list, kTraceableCount ) )
		return;

#endif

#endif

	mProgressCounter.store( 0 );
}

void PathTracer::trace( std::atomic_uint32_t& progressCounter )
{
	const unsigned int threadCount = std::thread::hardware_concurrency();
	std::vector< std::thread > threads( threadCount );

	uint32_t stepCount = mWidth * mHeight;

	for( unsigned int t = 0; t < threadCount; ++t )
	{
		threads[ t ] = std::thread( std::bind( [&]( int start, int end, int t )
		{
			for( uint16_t y = start; y < end; ++y )
			{
				for( uint16_t x = 0; x < mWidth; ++x )
				{
					stepTrace( x, y );
					mProgressCounter++;
				}
			}
		},
		t * mHeight / threadCount,
		( t + 1 ) == threadCount ? mHeight : ( t + 1 ) * mHeight / threadCount,
		t ) );

	} // for( unsigned int t = 0; t < threadCount; ++t )

	if( mProgressCallback != nullptr )
	{
		uint32_t step;
		do
		{
			step = mProgressCounter.load();
			( *mProgressCallback )( uint16_t( step * 100 / stepCount ), mProgressCallbackData );
		}
		while( step < stepCount );
	}

	for( unsigned int i = 0; i < threadCount; ++i )
	{
		threads[ i ].join();
	}

	if( mCompleteCallback != nullptr )
	{
		( *mCompleteCallback )( *this, mCompleteCallbackData );
	}
}

void PathTracer::stepTrace( uint16_t x, uint16_t y )
{
	vec3 color( 0.0f, 0.0f, 0.0f );

	for( uint32_t s = 0; s < mSampleCount; ++s )
	{
		float u = float( x + RandomFloat() ) / float( mWidth );
		float v = float( y + RandomFloat() ) / float( mHeight );

		Ray ray = mCamera->getRay( u, v );
		color += getColor( ray, *mScene, 0 );
	}

	color /= float( mSampleCount );

	// gamma correct (2.0 gamma, not 2.2)
	color = vec3( sqrt( color[ 0 ] ), sqrt( color[ 1 ] ), sqrt( color[ 2 ] ) );

	int r = int( 255.99f * color[ 0 ] );
	int g = int( 255.99f * color[ 1 ] );
	int b = int( 255.99f * color[ 2 ] );

	uint32_t rowOffset = y * mWidth * mBytesPerPixel;
	uint32_t pixelOffset = x * mBytesPerPixel;

	assert( rowOffset + pixelOffset + 2 < uint32_t( mWidth * mHeight * mBytesPerPixel ) );

	// Both Windows and the TGA file format expect a BGR channel order
	// Note: This is thread safe because each thread writes to a different
	// part of the mPixels array
	mPixels[ rowOffset + pixelOffset     ] = b;
	mPixels[ rowOffset + pixelOffset + 1 ] = g;
	mPixels[ rowOffset + pixelOffset + 2 ] = r;
}

vec3 PathTracer::getColor( const Ray& r, Scene& scene, int depth ) const
{
	// 0.001f : Reject rays that are too close to 0 to fix shadow acne
	HitRecord hit;
	if( scene.hit( r, 0.001f, FLT_MAX, hit ) )
	{
		Ray scattered;
		vec3 attenuation;
		vec3 emitted = hit.material->emitted( hit.u, hit.v, hit.p );
		if( depth < 50 && hit.material->scatter( r, hit, attenuation, scattered ) )
		{
			return emitted + attenuation * getColor( scattered, scene, depth + 1 );
		}
		else
		{
			return emitted;
		}
	}

	// else the ray did not hit any scene object, return the background color

#if 0
	// a gradient between white at the bottom and light blue at the top
	vec3 direction = r.getDirection().getNormalized();
	float t = 0.5f * ( direction.y + 1.0f ); // [ -1, 1 ] -> [ 0, 1 ]

	const vec3 lightBlue( 0.5f, 0.7f, 1.0f );
	const vec3 white( 1.0f, 1.0f, 1.0f );
	return Lerp( white, lightBlue, t );
#else
	return vec3( 0.0f, 0.0f, 0.0f ); // black background
#endif
}

Scene* PathTracer::createRandomScene( void ) const
{
	uint32_t n = 500; // # of objects to create

	Traceable** list = new Traceable* [ n + 1 ]; // add one for the floor

	Texture* checker = new CheckerTexture( new ConstantTexture( vec3( 0.2f, 0.3f, 0.1f ) ),
										   new ConstantTexture( vec3( 0.9f, 0.9f, 0.9f ) ) );
	list[ 0 ] = new Sphere( vec3( 0.0f, -1000.0f, 0.0f ), 1000.0f,
							new Lambertian( checker ) );

	uint32_t i = 1;

	for( int a = -10; a < 10; ++a )
	{
		for( int b = -10; b < 10; ++b )
		{
			float materialChoice = RandomFloat();
			vec3 center( a + 0.9f * RandomFloat(), 0.2f, b + 0.9f * RandomFloat() );
			if( ( center - vec3( 4.0f, 0.2f, 0.0f ) ).length() > 0.9f )
			{
				if( materialChoice < 0.8f ) // 80% chance of a diffuse material
				{
					Lambertian* material = new Lambertian( vec3( RandomFloat() * RandomFloat(),
																 RandomFloat() * RandomFloat(),
																 RandomFloat() * RandomFloat() ) );
					list[ i++ ] = new Sphere( center, center + vec3( 0.0f, 0.5f * RandomFloat(), 0.0f ),
											  0.0f, 1.0f, 0.2f, material );
				}
				else if( materialChoice < 0.95f ) // 15% chance of Metal
				{
					Metal* material = new Metal( vec3( 0.5f * ( 1.0f + RandomFloat() ),
													   0.5f * ( 1.0f + RandomFloat() ),
													   0.5f * ( 1.0f + RandomFloat() ) ),
												 0.5f * RandomFloat() );

					list[ i++ ] = new Sphere( center, 0.2f, material );
				}
				else // 5% chance of Glass
				{
					list[ i++ ] = new Sphere( center, 0.2f, new Glass( 1.5f ) );
				}

			} // if( ( center - vec3( 4.0f, 0.2f, 0.0f ) ).length() > 0.9f )

		} // for( int b = -11; b < 11; ++b )

	} // for( int a = -11; a < 11; ++a )

	// Add three big "landmark" sphere in the center,
	// showcasing the three different material types

	list[ i++ ] = new Sphere( vec3( 0.0f, 1.0f, 0.0f ), 1.0f, new Glass( 1.5f ) );
	list[ i++ ] = new Sphere( vec3( -4.0f, 1.0f, 0.0f ), 1.0f, new Lambertian( vec3( 0.4f, 0.2f, 0.1f ) ) ); // brown
	list[ i++ ] = new Sphere( vec3( 4.0f, 1.0f, 0.0f ), 1.0f, new Metal( vec3( 0.7f, 0.6f, 0.5f ), 0.0f ) );

	Scene* scene = new Scene;
	if( scene == nullptr )
		return nullptr;

	if( !scene->initialize( list, i ) )
	{
		delete scene;
		return nullptr;
	}

	return scene;
}

Scene* PathTracer::createTwoPerlinSpheres( void ) const
{
	static const float scale = 4.0f;
	static const size_t kListCount = 4;

	Traceable** list = new Traceable* [ kListCount ];
	list[ 0 ] = new Sphere( vec3( 0.0f, -1000.0f, 0.0f ), 1000.0f, new Lambertian( new NoiseTexture( scale ) ) );
	list[ 1 ] = new Sphere( vec3( 0.0f, 2.0f, 0.0f ), 2.0f, new Lambertian( new NoiseTexture( scale ) ) );
	list[ 2 ] = new Sphere( vec3( 0.0f, 7.0f, 0.0f ), 2.0f, new DiffuseLight( new ConstantTexture( vec3( 4.0f, 4.0f, 4.0f ) ) ) );
	list[ 3 ] = new xyRect( 3.0f, 5.0f, -1.0f, 3.0f, -2.0f, new DiffuseLight( new ConstantTexture( vec3( 4.0f, 4.0f, 4.0f ) ) ) );

	Scene* scene = new Scene;
	if( scene == nullptr )
		return nullptr;

	if( !scene->initialize( list, kListCount ) )
	{
		delete scene;
		return nullptr;
	}

	return scene;
}
