#pragma once

#include <stdint.h>

#include "Traceable.h"

// Called "hittable_list" in the "Ray Tracing in One Weekend" book
class Scene : public Traceable
{
public:
	Scene();
	~Scene();

	// Traceable interface implementation

	virtual bool hit( const Ray& r, float t_min, float t_max, HitRecord& rec ) const;

	virtual bool getBoundingBox( float t0, float t1, AABB& box ) const;

	// Scene member functions

	// This function takes ownership of the Traceable objects in list
	bool initialize( Traceable** list, uint32_t listSize );
	void shutdown( void );

	uint32_t getListSize( void ) const;

private:
	Traceable**	mList;
	uint32_t	mListSize;

}; // class Scene

inline Scene::Scene()
	: mList( nullptr )
	, mListSize( 0 )
{
}

inline Scene::~Scene()
{
	shutdown();
}

inline uint32_t Scene::getListSize( void ) const
{
	return mListSize;
}
