#ifndef _BVH_GENERICNODE_H_
#define _BVH_GENERICNODE_H_

#include "Hittable.h"
#include "Vector3.h"
#include "Ray.h"
#include "Material.h"
#include "AxisAlignedBoundingBox.h"

class BVH_genericnode : public Hittable {
public:
	//AABB aabb;
	Hittable** childList;
	uint8_t numChildren;
	uint8_t maxChildren;

	BVH_genericnode(Hittable** plist, uint8_t numprivitives, uint8_t maxprimitives) {
		this->childList = plist;
		this->numChildren = numprivitives;
		this->maxChildren = maxprimitives;

		this->aabb = combineBoundingBoxes(plist, numChildren);
	}

	~BVH_genericnode()
	{
		for (int i = 0; i < numChildren; i++)
		{
			delete childList[i];
		}
		delete[] childList;
	}

	bool BVH_addnode(Hittable* newnode)
	{
		if (numChildren == maxChildren) return false;

		this->childList[numChildren] = newnode;
		numChildren++;
		this->aabb = combineBoundingBoxes(this->aabb, newnode->aabb);
		return true;
	}

	virtual bool hit(const Ray& ray, float tMin, float tMax, HitInfo& hitInfo) const;

	virtual int drawBoundingBox(Rasterizer &rt, int level) const;
};

bool BVH_genericnode::hit(const Ray& ray, float tMin, float tMax, HitInfo& hitInfo) const
{
	hitInfo.pMaterial = NULL;
	return (aabb.hit(ray, tMin, tMax));
}

int BVH_genericnode::drawBoundingBox(Rasterizer &rt, int level) const
{
	int max_depth = 0;
	int depth;
	for (int i = 0; i < numChildren; i++)
	{
		if ((depth = childList[i]->drawBoundingBox(rt, level + 1)) > max_depth)
		{
			max_depth = depth;
		}
	}

	rt.RasterizeBox(aabb, &getColor(level, max_depth));
	return max_depth;
}


#endif
 