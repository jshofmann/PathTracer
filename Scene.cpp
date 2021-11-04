#include "stdafx.h"

#include "Scene.h"
#include "AABB.h"
#include "Math.h"

// This function takes ownership of the Traceable objects in list
bool Scene::initialize( Traceable** list, uint32_t listSize )
{
	if( ( list == nullptr ) || ( listSize == 0 ) )
		return false;

	shutdown();

	mListSize = listSize;

	mList = new Traceable* [ mListSize ];
	if( mList == nullptr )
		return false;

	memcpy( mList, list, listSize * sizeof( Traceable* ) );

	return true;
}

void Scene::shutdown( void )
{
	if( mList == nullptr )
		return; // already shut down

	for( uint32_t i = 0; i < mListSize; ++i )
	{
		delete mList[ i ];
	}

	delete[] mList;
}

bool Scene::hit( const Ray& r, float t_min, float t_max, HitRecord& rec ) const
{
	HitRecord tempRecord;
	bool hitAnything = false;
	float closest = t_max;

	for( uint32_t i = 0; i < mListSize; ++i )
	{
		if( mList[ i ]->hit( r, t_min, closest, tempRecord ) )
		{
			hitAnything = true;
			closest = tempRecord.t;
			rec = tempRecord;
		}
	}

	return hitAnything;
}

bool Scene::getBoundingBox( float t0, float t1, AABB& box ) const
{
	if( mListSize == 0 )
	{
		return false;
	}

	AABB temp;
	bool hasBox = mList[ 0 ]->getBoundingBox( t0, t1, temp );
	if( !hasBox )
	{
		return false;
	}
	else
	{
		box = temp;
	}

	if( mListSize > 1 )
	{
		for( uint32_t i = 1; i < mListSize; ++i )
		{
			if( mList[ i ]->getBoundingBox( t0, t1, temp ) )
			{
				box = enclose( box, temp );
			}
			else
			{
				return false;
			}
		}
	}

	return true;
}
