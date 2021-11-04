#pragma once

#include <stdint.h>

#include "Traceable.h"
#include "AABB.h"

class BVHNode : public Traceable
{
public:
	BVHNode();
	BVHNode( Traceable** list, uint32_t listCount, float t0, float t1 );

	// Traceable interface implementation

	virtual bool hit( const Ray& r, float t_min, float t_max, HitRecord& rec ) const;

	virtual bool getBoundingBox( float t0, float t1, AABB& box ) const;

private:
	Traceable*	mLeft;
	Traceable*	mRight;
	AABB		mBounds;

}; // class BVHNode

inline BVHNode::BVHNode()
	: mLeft( nullptr )
	, mRight( nullptr )
{
}

inline bool BVHNode::getBoundingBox( float t0, float t1, AABB& box ) const
{
	box = mBounds;
	return true;
}
