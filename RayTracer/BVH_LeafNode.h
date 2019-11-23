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
	virtual int drawBoundingBox(Rasterizer &rt, int level) const;
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

int BVH_leafnode::drawBoundingBox(Rasterizer &rt, int level) const
{
	int max_depth = 0;
	int depth;
	for (int i = 0; i < numPrimitives; i++)
	{
		if ((depth = primitiveList[i]->drawBoundingBox(rt, level + 1)) > max_depth)
		{
			max_depth = depth;
		}
	}

	// get my colour
	rt.RasterizeBox(aabb, &getColor(level, max_depth));
	return max_depth;
}

#endif
 