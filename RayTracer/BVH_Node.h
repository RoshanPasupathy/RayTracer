#ifndef _BVH_NODE_H_
#define _BVH_NODE_H_

#include "Hittable.h"
#include "Vector3.h"
#include "Ray.h"
#include "Material.h"
#include "AxisAlignedBoundingBox.h"

class BVH_node : public Hittable {
public:
	//AABB aabb;
	Hittable* left;
	Hittable* right;

	BVH_node(Hittable* _left, Hittable* _right) {
		this->left = _left;
		this->right = _right;

		this->aabb = combineBoundingBoxes(left->aabb, right->aabb);
	}

	~BVH_node()
	{
		// delete children
		delete left;
		delete right;
	}

	virtual bool hit(const Ray& ray, float tMin, float tMax, HitInfo& hitInfo) const;
	virtual int drawBoundingBox(Rasterizer &rt, int level) const;
};

bool BVH_node::hit(const Ray& ray, float tMin, float tMax, HitInfo& hitInfo) const
{
	hitInfo.pMaterial = NULL;
	return (aabb.hit(ray, tMin, tMax));
}

int BVH_node::drawBoundingBox(Rasterizer &rt, int level) const
{
	int max_depth = left->drawBoundingBox(rt, level + 1);
	max_depth = MAX(right->drawBoundingBox(rt, level + 1), max_depth);

	rt.RasterizeBox(aabb, &getColor(level, max_depth));
	return max_depth;
}

#endif
 