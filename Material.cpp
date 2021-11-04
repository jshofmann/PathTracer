#include "stdafx.h"

#include "Material.h"

bool Glass::scatter( const Ray& ray, const HitRecord& hit,
					 vec3& attenuation, Ray& scattered ) const
{
	vec3 reflected = Reflect( ray.getDirection(), hit.normal );

	// Glass absorbs nothing
	attenuation = vec3( 1.0f, 1.0f, 1.0f );

	vec3 outwardNormal;
	float ni_over_nt, cosine;

	if( Dot( ray.getDirection(), hit.normal ) > 0.0f )
	{
		outwardNormal = -hit.normal;
		ni_over_nt = mRefractIndex;
		cosine = mRefractIndex * Dot( ray.getDirection(), hit.normal ) / ray.getDirection().length();
	}
	else
	{
		outwardNormal = hit.normal;
		ni_over_nt = 1.0f / mRefractIndex;
		cosine = -Dot( ray.getDirection(), hit.normal ) / ray.getDirection().length();
	}

	float reflectProbability;
	vec3 refracted;

	if( Refract( ray.getDirection(), outwardNormal, ni_over_nt, refracted ) )
	{
		reflectProbability = Schlick( cosine, mRefractIndex );
	}
	else
	{
		reflectProbability = 1.0f;
	}

	if( RandomFloat() < reflectProbability )
	{
		scattered = Ray( hit.p, reflected );
	}
	else
	{
		scattered = Ray( hit.p, refracted );
	}

	return true;
}
