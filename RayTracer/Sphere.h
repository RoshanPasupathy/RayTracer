#ifndef SPHERE_H
#define SPHERE_H

#include "Hittable.h"
#include "Vector3.h"
#include "Ray.h"
#include "Material.h"

class Sphere : public Hittable {
public:
	//AABB aabb;
	Vector3 center;
	float radius;
	Material * pMaterial;

	Sphere(){}

	Sphere(Vector3 center, float r, Material * pMat) {
		this->center = center;
		this->radius = r;
		this->pMaterial = pMat;

		this->aabb.m_min = Vector3(center.v[0] - r, center.v[1] - r, center.v[2] - r);
		this->aabb.m_max = Vector3(center.v[0] + r, center.v[1] + r, center.v[2] + r);
	}

	~Sphere()
	{
		delete pMaterial;
	}

	virtual bool hit(const Ray& ray, float tMin, float tMax, HitInfo& hitInfo) const;
};

bool Sphere::hit(const Ray& ray, float tMin, float tMax, HitInfo& hitInfo) const{
	//radius^2 = dot((p(t) - c, p(t) - c) = (p(t)_x - c_x)^2 + (p(t)_y - c_y)^2 + (p(t)_z - c_z)^2
	//p(t) = origin + direction * t = A + B * t
	//0 = dot(A + B * t - C, A + B * t - C) = t^2 * dot(B, B) + 2t * dot(B, A - C) + (dot(A-C, A-C) - radius^2)

	float a = Vector3::dot(ray.direction, ray.direction);
	float b = Vector3::dot(ray.origin - center, ray.direction);
	float c = Vector3::dot(ray.origin - center, ray.origin - center) - radius * radius;

	float d = b * b - a * c;

	if (d > 0) {
		float t = (-b - sqrt(d)) / (a);
		if (t > tMin && t < tMax) {
			hitInfo.t = t;
			hitInfo.position = ray.getPoint(hitInfo.t);
			hitInfo.normal = (hitInfo.position - center) / radius;
			hitInfo.pMaterial = pMaterial;
			return true;
		}
		else {
			t = (-b + sqrt(d)) / (a);
			if (t > tMin && t < tMax) {
				hitInfo.t = t;
				hitInfo.position = ray.getPoint(hitInfo.t);
				hitInfo.normal = (hitInfo.position - center) / radius;
				hitInfo.pMaterial = pMaterial;
				return true;
			}
		}
	}
	
	return false;
}

#endif
 