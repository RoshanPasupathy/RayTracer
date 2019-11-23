#ifndef _BOUNDING_VOLUME_HEIRACHY_H_
#define _BOUNDING_VOLUME_HEIRACHY_H_

#include "RTCommonDefines.h"
#include "AxisAlignedBoundingBox.h"
#include "Hittable.h"
#include "BVH_Node.h"
#include "BVH_LeafNode.h"
#include "omp.h"

int compare_hittable(const void *_a, const void *_b)
{
	Hittable* a = *(Hittable **)_a;
	Hittable* b = *(Hittable **)_b;
	if (a->aabb.m_min.x() < b->aabb.m_min.x())
	{
		return -1;
	}
	else
	{
		return 1;
	}
}
int compare_hittable_morton(const void *_a, const void *_b)
{
	Hittable* a = *(Hittable **)_a;
	Hittable* b = *(Hittable **)_b;
	if (mortoncode(a) < mortoncode(b))
	{
		return -1;
	}
	else
	{
		return 1;
	}
}
////////////////////////////////////////////////////////////
// functions for constructing BVH
////////////////////////////////////////////////////////////


BVH_leafnode** getLeafNodes(Hittable** shapeList, int numShapes, int maxPrimitivesInLeaf, int& numLeaves, bool use_morton)
{
	// sort shapes along an axis
	if (use_morton)
	{
		qsort(shapeList, numShapes, sizeof(Hittable *), compare_hittable_morton);
	}
	else
	{
		qsort(shapeList, numShapes, sizeof(Hittable *), compare_hittable);
	}
	numLeaves = numShapes / maxPrimitivesInLeaf;
	if (numShapes % maxPrimitivesInLeaf > 0)
	{
		numLeaves++;
	}
	BVH_leafnode** res = new BVH_leafnode*[numShapes];
	int curLeafIndex = 0;
	int i = 0;
	for (; i < numLeaves - 1; i++)
	{ 
		res[i] = new BVH_leafnode(&shapeList[i  * maxPrimitivesInLeaf], maxPrimitivesInLeaf);
		// debug
		//float r = fmax(0.0f, 1 - 2.0f*i / (float)(numLeaves - 1));
		//float g = (i <= (numLeaves - 1)/ 2) ? 2.0f * i / (float)(numLeaves - 1): 2.0f * (1 - (float)i / (float)(numLeaves - 1));
		//float b = fmax(0.0f, -1.0f + 2.0f * (float)i / (float)(numLeaves - 1));
		//Material *pMat = new Lambertian(Vector3(r * r * r, g * g, b * b * b));
		//for (int j = 0; j < maxPrimitivesInLeaf; j++)
		//{
		//	delete(((Triangle *)shapeList[i * maxPrimitivesInLeaf + j])->pMaterial);
		//	((Triangle *)shapeList[i * maxPrimitivesInLeaf + j])->pMaterial = pMat;
		//}
	}
	if (numShapes % maxPrimitivesInLeaf > 0)
	{
		res[i] = new BVH_leafnode(&shapeList[i  * maxPrimitivesInLeaf], numShapes % maxPrimitivesInLeaf);
		//Material *pMat = new Lambertian(Vector3(0.0f, 0.0f, 1.0f));
		//for (int j = 0; j < numShapes % maxPrimitivesInLeaf; j++)
		//{
		//	delete(((Triangle *)shapeList[i * maxPrimitivesInLeaf + j])->pMaterial);
		//	((Triangle *)shapeList[i * maxPrimitivesInLeaf + j])->pMaterial = pMat;
		//}
	}
	else
	{
		res[i] = new BVH_leafnode(&shapeList[i  * maxPrimitivesInLeaf], maxPrimitivesInLeaf);
		//Material *pMat = new Lambertian(Vector3(0.0f, 0.0f, 1.0f));
		//for (int j = 0; j < maxPrimitivesInLeaf; j++)
		//{
		//	delete(((Triangle *)shapeList[i * maxPrimitivesInLeaf + j])->pMaterial);
		//	((Triangle *)shapeList[i * maxPrimitivesInLeaf + j])->pMaterial = pMat;
		//}
	}
	return res;
}

// construct tree from leaf nodes
Hittable* constructBVH(Hittable** shapeList, int numShapes, int maxPrimitivesInLeaf, bool use_morton)
{
	int numnodes;
	Hittable** level = (Hittable **)getLeafNodes(shapeList, numShapes, maxPrimitivesInLeaf, numnodes, use_morton);
	Hittable** parentLevel = new Hittable*[numShapes];
	while (numnodes != 1)
	{
		// construct parent level
		int j = 0;

		for (j; j < numnodes / 2; j++)
		{
			parentLevel[j] = (Hittable *) new BVH_node(level[j * 2], level[j * 2 + 1]);
		}

		if (j * 2 < numnodes)
		{
			// one extra node: pass it up as is
			parentLevel[j] = level[numnodes - 1];
			j++;
		}

		numnodes = j;
		// swap pointers
		Hittable** tmp = parentLevel;
		parentLevel = level;
		level = tmp;
	}

	Hittable* rootNode = level[0];
	// you can delete these because this wont delte the allocated hittable objects
	delete[] level;
	delete[] parentLevel;
	return rootNode;
}

static Hittable* bvh_stack[MAX_THREADS][MAX_SHAPES];
// hit information
bool bvh_hit(Hittable *bvh_root, int numPrimitives, const Ray& r, float tMin, float tMax, HitInfo& hitInfo)
{
	int stack_size = 1;
	Hittable** mystack = bvh_stack[omp_get_thread_num()];
	mystack[0] = bvh_root;
	Hittable* cur_node;
	HitInfo tempInfo;
	bool hit = false;
	double minHitDist = tMax;
	while (stack_size != 0)
	{
		cur_node = mystack[stack_size - 1];
		stack_size--;
		//if (cur_node == NULL) continue;
		if (cur_node->hit(r, tMin, minHitDist, tempInfo))
		{
			if (tempInfo.pMaterial == NULL)
			{
				// it is an interior node. add left and right children
				mystack[stack_size] = ((BVH_node *)cur_node)->left;
				stack_size++;
				mystack[stack_size] = ((BVH_node *)cur_node)->right;
				stack_size++;
			}
			else
			{
				// primitive hit
				hit = true;
				minHitDist = tempInfo.t;
				hitInfo = tempInfo;
			}
		}
	}
	return hit;
}



#endif 
