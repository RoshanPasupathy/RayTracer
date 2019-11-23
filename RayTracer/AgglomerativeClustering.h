#ifndef _AC_BVH_H_
#define _AC_BVH_H_
#include "RTCommonDefines.h"
#include "BVH_GenericNode.h"
#include <omp.h>


#define LEAFNODEPRIMS 16
#define INTRNODEPRIMS 2

float surface_area(Hittable* a, Hittable *b)
{
	AABB combined = combineBoundingBoxes(a->aabb, b->aabb);
	Vector3 sl = combined.m_max - combined.m_min; // sidelength
	return sl.v[0] * sl.v[1] + sl.v[0] * sl.v[2] + sl.v[1] * sl.v[2];
}

Hittable* AC_constructBVH(Hittable** shapeList, int numShapes, int numPrimitivesInLeaf, int branchingFactor)
{
	// assign each
	BVH_genericnode** clusters = new BVH_genericnode*[numShapes];
	for (int i = 0; i < numShapes; i++)
	{
		Hittable** nodelist = new Hittable*[numPrimitivesInLeaf];
		nodelist[0] = shapeList[i];
		clusters[i] = new BVH_genericnode(nodelist, 1, numPrimitivesInLeaf);
	}

	int numClusters = numShapes;
	//Vector3 maxsl = g_max - g_min;
	//float maxsa = maxsl.v[0] * maxsl.v[1] + maxsl.v[0] * maxsl.v[2] + maxsl.v[1] * maxsl.v[2];
	while (numClusters > 1)
	{
		int left = 0;
		int right = 0;
		float best = FLT_MAX;
		// iterate through clusters and get best distance function
#pragma omp parallel default(none) \
		shared(left, right, best, numClusters, clusters)
		{
			float loc_best = FLT_MAX;
			int loc_left = 0;
			int loc_right = 0;
			float dist;
#pragma omp for schedule(dynamic)
			for (int i = 0; i < numShapes; i++)
			{
				if (clusters[i] == NULL) continue;
				for (int j = i + 1; j < numShapes; j++)
				{
					if (clusters[j] == NULL) continue;

					dist = surface_area(clusters[i], clusters[j]);
					if (dist < loc_best)
					{
						loc_best = dist;
						loc_left = i;
						loc_right = j;
					}
				}
			}
			#pragma omp critical(best_reduction)
			{
				if (loc_best < best)
				{
					best = loc_best;
					left = loc_left;
					right = loc_right;
					
				}
			}
		}

		if (clusters[right]->maxChildren > clusters[left]->maxChildren)
		{
			int tmp = left;
			left = right;
			right = tmp;
		}
		if (clusters[left]->numChildren + clusters[right]->numChildren <= clusters[left]->maxChildren)
		{
			// copy over chidlren
			uint8_t i = clusters[left]->numChildren;
			for (int j = 0; j < clusters[right]->numChildren; j++)
			{
				clusters[left]->childList[i++] = clusters[right]->childList[j];
			}
			clusters[left]->numChildren = i; // update numchildren
			clusters[left]->aabb = combineBoundingBoxes(clusters[left]->aabb, clusters[right]->aabb);
			clusters[right]->numChildren = 0;
			delete (clusters[right]);// delete the node
		}
		else
		{
			Hittable** nodelist = new Hittable*[branchingFactor];
			nodelist[0] = clusters[left];
			nodelist[1] = clusters[right];
			BVH_genericnode *newnode = new BVH_genericnode(nodelist, 2, branchingFactor);
			clusters[left] = newnode;
		}
		clusters[right] = NULL;
		numClusters--;
	}
	

	// get the root node
	Hittable* ret = NULL;
	for (int i = 0; i < numShapes; i++)
	{
		if (clusters[i] != NULL)
		{
			ret = clusters[i];
		}
	}
	delete[] clusters;
	return ret;
}

Hittable* AC_constructBVH(Hittable** shapeList, int numShapes, int numPrimitivesInLeaf)
{
	// assign each
	BVH_genericnode** clusters = new BVH_genericnode*[numShapes];
	for (int i = 0; i < numShapes; i++)
	{
		Hittable** nodelist = new Hittable*[numPrimitivesInLeaf];
		nodelist[0] = shapeList[i];
		clusters[i] = new BVH_genericnode(nodelist, 1, numPrimitivesInLeaf);
	}

	int numClusters = numShapes;
	//Vector3 maxsl = g_max - g_min;
	//float maxsa = maxsl.v[0] * maxsl.v[1] + maxsl.v[0] * maxsl.v[2] + maxsl.v[1] * maxsl.v[2];
	while (numClusters > 1)
	{
		int left = 0;
		int right = 0;
		float best = FLT_MAX;
		// iterate through clusters and get best distance function
#pragma omp parallel default(none) \
		shared(left, right, best, numClusters, clusters)
		{
			float loc_best = FLT_MAX;
			int loc_left = 0;
			int loc_right = 0;
			float dist;
#pragma omp for schedule(dynamic)
			for (int i = 0; i < numShapes; i++)
			{
				if (clusters[i] == NULL) continue;
				for (int j = i + 1; j < numShapes; j++)
				{
					if (clusters[j] == NULL) continue;

					dist = surface_area(clusters[i], clusters[j]);
					if (dist < loc_best)
					{
						loc_best = dist;
						loc_left = i;
						loc_right = j;
					}
				}
			}
#pragma omp critical(best_reduction)
			{
				if (loc_best < best)
				{
					best = loc_best;
					left = loc_left;
					right = loc_right;

				}
			}
		}

		if (clusters[right]->maxChildren > clusters[left]->maxChildren)
		{
			int tmp = left;
			left = right;
			right = tmp;
		}
		if (clusters[left]->numChildren + clusters[right]->numChildren <= clusters[left]->maxChildren)
		{
			// copy over chidlren
			uint8_t i = clusters[left]->numChildren;
			for (int j = 0; j < clusters[right]->numChildren; j++)
			{
				clusters[left]->childList[i++] = clusters[right]->childList[j];
			}
			clusters[left]->numChildren = i; // update numchildren
			clusters[left]->aabb = combineBoundingBoxes(clusters[left]->aabb, clusters[right]->aabb);
			clusters[right]->numChildren = 0;
			delete (clusters[right]);// delete the node
		}
		else
		{
			uint8_t branchingFactor = MAX(MIN(clusters[left]->maxChildren, clusters[right]->maxChildren) / 2, 2);
			Hittable** nodelist = new Hittable*[branchingFactor];
			nodelist[0] = clusters[left];
			nodelist[1] = clusters[right];
			BVH_genericnode *newnode = new BVH_genericnode(nodelist, 2, branchingFactor);
			clusters[left] = newnode;
		}
		clusters[right] = NULL;
		numClusters--;
	}


	// get the root node
	Hittable* ret = NULL;
	for (int i = 0; i < numShapes; i++)
	{
		if (clusters[i] != NULL)
		{
			ret = clusters[i];
		}
	}
	delete[] clusters;
	return ret;
}

static Hittable* ac_stack[MAX_THREADS][MAX_SHAPES];

// hit information
bool AC_bvh_hit(Hittable *bvh_root, const Ray& r, float tMin, float tMax, HitInfo& hitInfo)
{
	int stack_size = 1;
	Hittable** mystack = ac_stack[omp_get_thread_num()];
	mystack[0] = bvh_root;
	Hittable* cur_node;
	HitInfo tempInfo;
	bool hit = false;
	double minHitDist = tMax;

	int hitcount = 0;
	while (stack_size != 0)
	{
		cur_node = mystack[stack_size - 1];
		stack_size--;
		//if (cur_node == NULL) continue;
		if (cur_node->hit(r, tMin, minHitDist, tempInfo))
		{
			hitcount++;
			if (tempInfo.pMaterial == NULL)
			{
				// it is a generic node. add all children
				Hittable** childlist = ((BVH_genericnode *)cur_node)->childList;
				for (uint8_t i = 0; i < ((BVH_genericnode *)cur_node)->numChildren; i++)
				{
					mystack[stack_size++] = childlist[i];
				}
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