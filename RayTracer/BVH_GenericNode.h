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

	virtual void drawBoundingBox(Rasterizer &rt, int depth, int max_depth) const;
	virtual int getTreeNodeHeight() const;
};

bool BVH_genericnode::hit(const Ray& ray, float tMin, float tMax, HitInfo& hitInfo) const
{
	hitInfo.pMaterial = NULL;
	return (aabb.hit(ray, tMin, tMax));
}

void BVH_genericnode::drawBoundingBox(Rasterizer &rt, int depth, int max_depth) const
{
	for (int i = 0; i < numChildren; i++)
	{
		childList[i]->drawBoundingBox(rt, depth + 1, max_depth);
	}

	rt.RasterizeBox(aabb, &getColor(depth, max_depth));
}

int BVH_genericnode::getTreeNodeHeight() const
{
	int max_depth = 0;
	int depth;
	for (int i = 0; i < numChildren; i++)
	{
		if ((depth = childList[i]->getTreeNodeHeight()) > max_depth)
		{
			max_depth = depth;
		}
	}
	return max_depth + 1;
}
#endif
 