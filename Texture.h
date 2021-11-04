#pragma once

#include "vec3.h"
#include "Perlin.h"

class Texture
{
public:
	virtual vec3 getValue( float u, float v, const vec3& p ) const = 0;

}; // class Texture

class ConstantTexture : public Texture
{
public:
	ConstantTexture()
		: mColor( 0.0f, 0.0f, 0.0f )
	{}

	ConstantTexture( const vec3& color )
		: mColor( color )
	{}

	// Texture interface implementation

	virtual vec3 getValue( float u, float v, const vec3& p ) const
	{
		return mColor;
	}

private:
	vec3 mColor;
};

class CheckerTexture : public Texture
{
public:
	CheckerTexture()
		: mOdd( nullptr )
		, mEven( nullptr )
	{}

	CheckerTexture( const vec3& oddColor, const vec3& evenColor )
	{
		mOdd = new ConstantTexture( oddColor );
		mEven = new ConstantTexture( evenColor );
	}

	// Takes ownership of the given Texture pointers
	CheckerTexture( Texture* oddTexture, Texture* evenTexture )
	{
		mOdd = oddTexture;
		mEven = evenTexture;
	}

	virtual ~CheckerTexture()
	{
		if( mOdd != nullptr )
			delete mOdd;
		if( mEven != nullptr )
			delete mEven;
	}

	// Texture interface implementation

	virtual vec3 getValue( float u, float v, const vec3& p ) const
	{
		float sines = sin( 10 * p.x ) * sin( 10 * p.y ) * sin( 10 * p.z );
		if( sines < 0.0f )
		{
			return mOdd->getValue( u, v, p );
		}
		else
		{
			return mEven->getValue( u, v, p );
		}
	}

private:
	Texture* mOdd;
	Texture* mEven;

}; // class CheckerTexture

class NoiseTexture : public Texture
{
public:
	NoiseTexture()
		: mScale( 1.0f )
	{}

	NoiseTexture( float scale )
		: mScale( scale )
	{}

	// Texture interface implementation

	virtual vec3 getValue( float u, float v, const vec3& p ) const
	{
//		return vec3( 1.0f, 1.0f, 1.0f ) * mNoise.noise( mScale * p );
		return vec3( 1.0f, 1.0f, 1.0f ) * 0.5f * ( 1.0f + sinf( mScale * p.z + 10.0f * mNoise.turbulence( p ) ) );
	}

private:
	Perlin	mNoise;
	float	mScale;

}; // class NoiseTexture

class ImageTexture : public Texture
{
public:
	ImageTexture()
		: mWidth( 0 )
		, mHeight( 0 )
		, mBytesPerPixel( 0 )
		, mPixels( nullptr )
	{}

	ImageTexture( uint16_t width, uint16_t height, uint16_t bytesPerPixel,
				  const uint8_t* pixels )
		: mWidth( width )
		, mHeight( height )
		, mBytesPerPixel( bytesPerPixel )
		, mPixels( pixels )
	{}

	ImageTexture( const char* filename );

	virtual ~ImageTexture();

	// Texture interface implementation

	virtual vec3 getValue( float u, float v, const vec3& p ) const;

private:
	uint16_t mWidth, mHeight, mBytesPerPixel;
	const uint8_t* mPixels;
	void* mFile;

}; // class ImageTexture
