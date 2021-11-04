#pragma once

#include "vec3.h"
#include "Ray.h"
#include "Math.h"
#include "Traceable.h"
#include "Texture.h"

class Material
{
public:
	virtual bool scatter( const Ray& ray, const HitRecord& hit,
						  vec3& attenuation, Ray& scattered ) const = 0;

	virtual vec3 emitted( float u, float v, const vec3& p ) const
	{
		return vec3( 0.0f, 0.0f, 0.0f );
	}

}; // class Material

class Lambertian : public Material
{
public:
	Lambertian( const vec3& albedo )
	{
		mAlbedo = new ConstantTexture( albedo );
	}

	// Takes ownership of the given Texture pointer
	Lambertian( Texture* albedo )
		: mAlbedo( albedo )
	{}

	virtual ~Lambertian()
	{
		if( mAlbedo != nullptr )
			delete mAlbedo;
	}

	// Material interface implementation

	virtual bool scatter( const Ray& ray, const HitRecord& hit,
						  vec3& attenuation, Ray& scattered ) const
	{
		vec3 target = hit.p + hit.normal + RandomInUnitSphere();
		scattered = Ray( hit.p, target - hit.p, ray.getTime() );
		attenuation = mAlbedo->getValue( 0.0f, 0.0f, hit.p );
		return true;
	}

private:
	Texture* mAlbedo;

}; // class Lambertian

class Metal : public Material
{
public:
	Metal( const vec3& albedo, float fuzziness )
		: mAlbedo( albedo )
	{
		if( fuzziness > 1.0f )
			mFuzziness = 1.0f;
		else
			mFuzziness = fuzziness;
	}

	// Material interface implementation

	virtual bool scatter( const Ray& ray, const HitRecord& hit,
						  vec3& attenuation, Ray& scattered ) const
	{
		vec3 reflected = Reflect( ray.getDirection().getNormalized(), hit.normal );
		scattered = Ray( hit.p, reflected + mFuzziness * RandomInUnitSphere() );
		attenuation = mAlbedo;
		return Dot( scattered.getDirection(), hit.normal ) > 0.0f;
	}

private:
	vec3	mAlbedo;
	float	mFuzziness;

}; // class Metal

// This class is called dialetric in "Ray Tracing in One Weekend"
class Glass : public Material
{
public:
	Glass( float index )
		: mRefractIndex( index )
	{}

	// Material interface implementation

	virtual bool scatter( const Ray& ray, const HitRecord& hit,
						  vec3& attenuation, Ray& scattered ) const;

private:
	float mRefractIndex;

}; // class Glass

class DiffuseLight : public Material
{
public:
	DiffuseLight() : mEmitter( nullptr ) {}
	DiffuseLight( Texture* emitter ) : mEmitter( emitter ) {}

	// Material interface implementation

	virtual bool scatter( const Ray& ray, const HitRecord& hit,
						  vec3& attenuation, Ray& scattered ) const
	{
		return false;
	}

	virtual vec3 emitted( float u, float v, const vec3& p ) const
	{
		return mEmitter->getValue( u, v, p );
	}

private:
	Texture* mEmitter;

}; // DiffuseLight
