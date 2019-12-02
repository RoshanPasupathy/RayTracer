#ifndef HITTABLE_H
#define HITTABLE_H

#include "Vector3.h"
#include "Ray.h"
#include "AxisAlignedBoundingBox.h"
#include "AABB_Rasterizer.h"

class Material;

struct HitInfo {
	float t;
	Vector3 position;
	Vector3 normal;
	Material* pMaterial;
};

class Hittable {
public:
	virtual bool hit(const Ray& r, float t_min, float t_max, HitInfo& hitInfo) const = 0;
	virtual ~Hittable() = 0;
	virtual void drawBoundingBox(Rasterizer &rt, int depth, int max_depth) const;
	virtual int getTreeNodeHeight() const;

	AABB aabb;
};

Hittable::~Hittable() {}

void Hittable::drawBoundingBox(Rasterizer &rt,  int depth, int max_depth) const
{
	rt.RasterizeBox(aabb, &getColor(depth,max_depth));
}

int Hittable::getTreeNodeHeight() const
{
	return 0; // height is 0 for primitives
}

bool hitList(Hittable ** shapes, int numShapes, const Ray& r, float tMin, float tMax, HitInfo& hitInfo) {

	HitInfo tempInfo;
	bool hit = false;
	double minHitDist = tMax;
	for (int i = 0; i < numShapes; i++) {
		if (shapes[i]->hit(r, tMin, minHitDist, tempInfo)) {
			hit = true;
			minHitDist = tempInfo.t;
			hitInfo = tempInfo;
		}
	}
	return hit;
}

AABB combineBoundingBoxes(const AABB &a, const AABB &b)
{
	AABB res;
	res.m_min.v[0] = fmin(a.m_min.v[0], b.m_min.v[0]);
	res.m_min.v[1] = fmin(a.m_min.v[1], b.m_min.v[1]);
	res.m_min.v[2] = fmin(a.m_min.v[2], b.m_min.v[2]);

	res.m_max.v[0] = fmax(a.m_max.v[0], b.m_max.v[0]);
	res.m_max.v[1] = fmax(a.m_max.v[1], b.m_max.v[1]);
	res.m_max.v[2] = fmax(a.m_max.v[2], b.m_max.v[2]);
	return res;
}

AABB combineBoundingBoxes(Hittable** hittableList, int numHittables)
{
	AABB res = hittableList[0]->aabb;
	for (int i = 1; i < numHittables; i++)
	{
		res.m_min.v[0] = fmin(res.m_min.v[0], hittableList[i]->aabb.m_min.v[0]);
		res.m_min.v[1] = fmin(res.m_min.v[1], hittableList[i]->aabb.m_min.v[1]);
		res.m_min.v[2] = fmin(res.m_min.v[2], hittableList[i]->aabb.m_min.v[2]);

		res.m_max.v[0] = fmax(res.m_max.v[0], hittableList[i]->aabb.m_max.v[0]);
		res.m_max.v[1] = fmax(res.m_max.v[1], hittableList[i]->aabb.m_max.v[1]);
		res.m_max.v[2] = fmax(res.m_max.v[2], hittableList[i]->aabb.m_max.v[2]);
	}
	return res;
}

// needed to compute rshifts
Vector3 g_min;
Vector3 g_max;

#include "MortonCode.h"
void Hittable_Init(Hittable** shape_list, int numShapes)
{
	// get global min and max
	g_min = 0.5 * (shape_list[0]->aabb.m_max + shape_list[0]->aabb.m_min);
	g_max = g_min;
	for (int i = 1; i < numShapes; i++)
	{
		Vector3 l_center = 0.5 * (shape_list[i]->aabb.m_max + shape_list[i]->aabb.m_min);
		g_min.v[0] = fmin(l_center.v[0], g_min.v[0]);
		g_min.v[1] = fmin(l_center.v[1], g_min.v[1]);
		g_min.v[2] = fmin(l_center.v[2], g_min.v[2]);

		g_max.v[0] = fmax(l_center.v[0], g_max.v[0]);
		g_max.v[1] = fmax(l_center.v[1], g_max.v[1]);
		g_max.v[2] = fmax(l_center.v[2], g_max.v[2]);
	}

	// init morton code params
	_mortoncode_init_params(g_max - g_min);
}

#define mortoncode(a) (_mortoncode(0.5 * ((a)->aabb.m_max + (a)->aabb.m_min) - g_min))

#endif
