#ifndef RAY_H
#define RAY_H

#include "Vector3.h"

class Ray {
public:
	Vector3 origin;
	Vector3 direction;

	Ray(){}
	Ray(const Vector3& origin, const Vector3& direction) {
		this->origin = origin;
		this->direction = direction;
	}
	Vector3 getPoint(float t) const { return origin + t * direction; }
};



#endif
