#include "stdafx.h"

#include "Rect.h"
#include "Material.h"
#include "AABB.h"

bool xyRect::hit( const Ray& r, float t_min, float t_max, HitRecord& hit ) const
{
	float t = ( mK - r.getOrigin().z ) / r.getDirection().z;
	if( ( t < t_min ) || ( t > t_max ) )
		return false;

	float x = r.getOrigin().x + t * r.getDirection().x;
	float y = r.getOrigin().y + t * r.getDirection().y;
	if( ( x < mX0 ) || ( x > mX1 ) || ( y < mY0 ) || ( y > mY1 ) )
		return false;

	hit.u = ( x - mX0 ) / ( mX1 - mX0 );
	hit.v = ( y - mY0 ) / ( mY1 - mY0 );
	hit.t = t;
	hit.material = mMaterial;
	hit.p = r.pointAtParameter( t );
	hit.normal = vec3( 0.0f, 0.0f, 1.0f );

	return true;
}

bool xyRect::getBoundingBox( float t0, float t1, AABB& box ) const
{
	box = AABB( vec3( mX0, mY0, mK - 0.0001f ), vec3( mX1, mY1, mK + 0.0001f ) );
	return true;
}
