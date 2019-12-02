#ifndef _BVH_LEAFNODE_H_
#define _BVH_LEAFNODE_H_

#include "Hittable.h"
#include "Vector3.h"
#include "Ray.h"
#include "Material.h"
#include "AxisAlignedBoundingBox.h"

class BVH_leafnode : public Hittable {
public:
	//AABB aabb;
	Hittable** primitiveList;
	int numPrimitives;

	BVH_leafnode(Hittable** plist, int np) {
		this->primitiveList = plist;
		this->numPrimitives = np;

		this->aabb = combineBoundingBoxes(plist, np);
	}

	~BVH_leafnode()
	{
		for (int i = 0; i < numPrimitives; i++)
		{
			delete primitiveList[i];
		}
	}

	virtual bool hit(const Ray& ray, float tMin, float tMax, HitInfo& hitInfo) const;
	virtual void drawBoundingBox(Rasterizer &rt, int depth, int max_depth) const;
	virtual int getTreeNodeHeight() const;
};

bool BVH_leafnode::hit(const Ray& ray, float tMin, float tMax, HitInfo& hitInfo) const
{
	if (!aabb.hit(ray, tMin, tMax)) return false;

	HitInfo tempInfo;
	bool hit = false;
	double minHitDist = tMax;
	for (int i = 0; i < numPrimitives; i++) {
		if (primitiveList[i]->hit(ray, tMin, minHitDist, tempInfo)) {
			hit = true;
			minHitDist = tempInfo.t;
			hitInfo = tempInfo;
		}
	}

	return hit;
}

void BVH_leafnode::drawBoundingBox(Rasterizer &rt, int depth, int max_depth) const
{
	for (int i = 0; i < numPrimitives; i++)
	{
		primitiveList[i]->drawBoundingBox(rt, depth + 1, max_depth);
	}

	// get my colour
	rt.RasterizeBox(aabb, &getColor(depth, max_depth));
}

int BVH_leafnode::getTreeNodeHeight() const
{
	int max_depth = 0;
	int depth;
	for (int i = 0; i < numPrimitives; i++)
	{
		if ((depth = primitiveList[i]->getTreeNodeHeight()) > max_depth)
		{
			max_depth = depth;
		}
	}
	return max_depth + 1;
}

#endif
 