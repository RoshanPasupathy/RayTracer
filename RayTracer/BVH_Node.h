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
	virtual void drawBoundingBox(Rasterizer &rt, int depth, int max_depth) const;
	virtual int getTreeNodeHeight() const;
};

bool BVH_node::hit(const Ray& ray, float tMin, float tMax, HitInfo& hitInfo) const
{
	hitInfo.pMaterial = NULL;
	return (aabb.hit(ray, tMin, tMax));
}

void BVH_node::drawBoundingBox(Rasterizer &rt, int depth, int max_depth) const
{
	left->drawBoundingBox(rt, depth + 1, max_depth);
	right->drawBoundingBox(rt, depth + 1, max_depth);

	rt.RasterizeBox(aabb, &getColor(depth, max_depth));
}

int BVH_node::getTreeNodeHeight() const
{
	return MAX(right->getTreeNodeHeight(), left->getTreeNodeHeight()) + 1;
}

#endif
 