#pragma once

#include <stdint.h>
#include <atomic>

#include "Camera.h"
#include "Scene.h"
#include "vec3.h"
#include "ray.h"

class PathTracer
{
public:
	typedef void( *ProgressCallback )( uint16_t step, void* data ); // step is in [0, 100]
	typedef void( *CompleteCallback )( const PathTracer& tracer, void* data );

	PathTracer();
	~PathTracer();

	bool initialize( uint16_t width, uint16_t height );

	void setProgressCallback( ProgressCallback callback, void* data );
	void setCompleteCallback( CompleteCallback callback, void* data );

	inline void getDimensions( uint16_t& width, uint16_t& height ) const;
	inline uint8_t getBytesPerPixel( void ) const;

	inline const uint8_t* getPixels( void ) const;

	void saveImage( const char* filename ) const;

	// To run the path tracer, call startTrace() to initialize the scene
	// and then trace() to run the actual path tracing loops
	void startTrace( void );

	// Single-threaded brute force tracer loop, will block the GUI
	void trace( std::atomic_uint32_t& progressCounter );

private:
	void stepTrace( uint16_t x, uint16_t y );
	vec3 getColor( const Ray& r, Scene& scene, int depth ) const;

	Scene* createRandomScene( void ) const;
	Scene* createTwoPerlinSpheres( void ) const;

	uint32_t				mSampleCount;
	uint16_t				mWidth, mHeight; // in pixels
	uint8_t					mBytesPerPixel;
	uint8_t*				mPixels;

	Camera*					mCamera;
	Scene*					mScene;

	std::atomic_uint32_t	mProgressCounter;

	ProgressCallback		mProgressCallback;
	void*					mProgressCallbackData;

	CompleteCallback		mCompleteCallback;
	void*					mCompleteCallbackData;

}; // class PathTracer

inline void PathTracer::getDimensions( uint16_t& width, uint16_t& height ) const
{
	width = mWidth;
	height = mHeight;
}

inline uint8_t PathTracer::getBytesPerPixel( void ) const
{
	return mBytesPerPixel;
}

inline const uint8_t* PathTracer::getPixels( void ) const
{
	return mPixels;
}
