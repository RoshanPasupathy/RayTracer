#ifndef _AXIS_ALIGNED_BOUNDING_BOX_H_
#define _AXIS_ALIGNED_BOUNDING_BOX_H_
#include "RTCommonDefines.h"
#include "Ray.h"
#include "Vector3.h"

class AABB
{
public:
	Vector3 m_min;
	Vector3 m_max;

	AABB() {}

	AABB(const Vector3 &_min, const Vector3 &_max)
	{
		m_min = _min;
		m_max = _max;
	}

	~AABB() {}

	//slab method for hit detection
	// TODO: branchless optimization. using MIN_SS for this this
	bool hit(const Ray& ray, float tmin, float tmax) const
	{
		Vector3 t0 = (m_min - ray.origin) / ray.direction;
		Vector3 t1 = (m_max - ray.origin) / ray.direction;

		float tmp;
		if (ray.direction.v[0] < 0)
		{
			tmp = t0.v[0];
			t0.v[0] = t1.v[0];
			t1.v[0] = tmp;
		}

		if (ray.direction.v[1] < 0)
		{
			tmp = t0.v[1];
			t0.v[1] = t1.v[1];
			t1.v[1] = tmp;
		}

		if (ray.direction.v[2] < 0)
		{
			tmp = t0.v[2];
			t0.v[2] = t1.v[2];
			t1.v[2] = tmp;
		}

		tmin = fmax(tmin, fmax(t0.v[0], fmax(t0.v[1], t0.v[2])));
		tmax = fmin(tmax, fmin(t1.v[0], fmin(t1.v[1], t1.v[2])));


		return (tmin <= tmax);
	}
};

#endif
