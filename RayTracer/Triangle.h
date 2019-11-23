#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Hittable.h"
#include "Vector3.h"
#include "Ray.h"
#include "Material.h"

class Triangle : public Hittable {
public:
	Vector3 vPosition[3];
	Vector3 vNormal[3];
	Material * pMaterial;

	Triangle() {}
	Triangle(Vector3 *vpos, Vector3 *norm, Material * pMat) {
		this->vPosition[0] = vpos[0];
		this->vPosition[1] = vpos[1];
		this->vPosition[2] = vpos[2];
		this->vNormal[0] = norm[0];
		this->vNormal[1] = norm[1];
		this->vNormal[2] = norm[2];
		this->pMaterial = pMat;

		float x = fmin(vPosition[0].v[0], fmin(vPosition[1].v[0], vPosition[2].v[0]));
		float y = fmin(vPosition[0].v[1], fmin(vPosition[1].v[1], vPosition[2].v[1]));
		float z = fmin(vPosition[0].v[2], fmin(vPosition[1].v[2], vPosition[2].v[2]));

		this->aabb.m_min =  Vector3(x, y, z);

		x = fmax(vPosition[0].v[0], fmax(vPosition[1].v[0], vPosition[2].v[0]));
		y = fmax(vPosition[0].v[1], fmax(vPosition[1].v[1], vPosition[2].v[1]));
		z = fmax(vPosition[0].v[2], fmax(vPosition[1].v[2], vPosition[2].v[2]));

		this->aabb.m_max = Vector3(x, y, z);
	}

	~Triangle()
	{
		delete pMaterial;
	}
	virtual bool hit(const Ray& ray, float tMin, float tMax, HitInfo& hitInfo) const;
};

bool Triangle::hit(const Ray& ray, float tMin, float tMax, HitInfo& hitInfo) const {
	//radius^2 = dot((p(t) - c, p(t) - c) = (p(t)_x - c_x)^2 + (p(t)_y - c_y)^2 + (p(t)_z - c_z)^2
	//p(t) = origin + direction * t = A + B * t
	//0 = dot(A + B * t - C, A + B * t - C) = t^2 * dot(B, B) + 2t * dot(B, A - C) + (dot(A-C, A-C) - radius^2)

	Vector3 e10 = vPosition[1] - vPosition[0];
	Vector3 e20 = vPosition[2] - vPosition[0];
	Vector3 n = Vector3::cross(e10, e20);
	//if (Vector3::dot(ray.direction, n) > -0.00001f && Vector3::dot(ray.direction, n) < 0.00001f)
	//	return false;

	Vector3 p = vPosition[0] - ray.origin;
	float t = Vector3::dot(p, n) / Vector3::dot(ray.direction, n);
	if (t > tMin && t < tMax) {
		// get barrycentric coordinates
		hitInfo.t = t;
		hitInfo.position = ray.getPoint(hitInfo.t);
		//Vector3 p0 = hitInfo.position - vPosition[0];
		//float b = fabs(Vector3::dot(Vector3::cross(p0, e20), n)) / Vector3::dot(n, n);
		//float c = fabs(Vector3::dot(Vector3::cross(p0, e10), n)) / Vector3::dot(n, n);
		//float a = 1 - (b + c);
		//if (a >= 0.0f && a <= 1.0f && b > 0.0f && b <= 1.0f && c > 0.0f && c <= 1.0f)
		//{
		//	// inside triangle
		//	hitInfo.normal = a * vNormal[0] + b * vNormal[1] + c * vNormal[2];
		//	if (Vector3::dot(ray.direction, hitInfo.normal) > 0)
		//	{
		//		hitInfo.normal = -hitInfo.normal;
		//	}
		//	hitInfo.pMaterial = pMaterial;
		//	return true;
		//}

		Vector3 C;

		//edge 10
		Vector3 vp0 = hitInfo.position - vPosition[0];
		C = Vector3::cross(e10, vp0);

		if (Vector3::dot(n, C) < 0) return false;

		//edge 21
		Vector3 e21 = vPosition[2] - vPosition[1];
		Vector3 vp1 = hitInfo.position - vPosition[1];
		C = Vector3::cross(e21, vp1);
		float a;
		if ((a = Vector3::dot(n, C)) < 0) return false;

		//edge 02
		Vector3 e02 = vPosition[0] - vPosition[2];
		Vector3 vp2 = hitInfo.position - vPosition[2];
		C = Vector3::cross(e02, vp2);
		float b;
		if ((b = Vector3::dot(n, C)) < 0) return false;

		a /= Vector3::dot(n, n);
		b /= Vector3::dot(n, n);
		hitInfo.normal = a * vNormal[0] + b * vNormal[1] + (1 - a - b) * vNormal[2];
		//if (Vector3::dot(ray.direction, hitInfo.normal) > 0)
		//{
		//	hitInfo.normal = -hitInfo.normal;
		//}
		hitInfo.pMaterial = pMaterial;
		return true;
	}
	return false;
}

#endif
 