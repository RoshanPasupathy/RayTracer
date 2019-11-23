#ifndef MATERIAL_H
#define MATERIAL_H

#include "Vector3.h"
#include "Ray.h"
#include "Hittable.h"
#include "Random.h"


float schlick(float cosine, float refIndex) {
	float r0 = (1 - refIndex) / (1 + refIndex);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5);
}


class Material {
public:
	virtual bool scatter(const Ray& ray, const HitInfo& hitInfo, Vector3& attenuation, Ray& scatteredRay) const = 0;
	virtual Vector3 emitt(float u, float v,const Vector3 &p) const {
		return Vector3(0, 0, 0);
	}
};

class Lambertian : public Material {
public: 
	Vector3 albedo;

	Lambertian(const Vector3& a) {
		albedo = a;
	}
	virtual bool scatter(const Ray& ray, const HitInfo& hitInfo, Vector3& attenuation, Ray& scatteredRay)const {
		Vector3 target = hitInfo.normal + randomUnitSphereVector();
		target.normalize();
		scatteredRay = Ray(hitInfo.position, target);
		attenuation = albedo;
		return true;
	}
};

class Metal : public Material {
public:
	Vector3 albedo;
	float fuzz;

	Metal(const Vector3& a, float f) {
		albedo = a;
		fuzz = f < 1 ? fuzz > 0 ? f : 0 : 1;
	}
	virtual bool scatter(const Ray& ray, const HitInfo& hitInfo, Vector3& attenuation, Ray& scatteredRay)const {
		Vector3 scatterDir = Vector3::reflect(ray.direction, hitInfo.normal) + fuzz * randomUnitSphereVector();
		scatterDir.normalize();
		scatteredRay = Ray(hitInfo.position, scatterDir);
		attenuation = albedo;
		return Vector3::dot(scatteredRay.direction, hitInfo.normal) > 0;
	}
};

class Dialectric : public Material {
public:
	float refIndex;

	Dialectric(float r) {
		refIndex = r;
	}
	virtual bool scatter(const Ray& ray, const HitInfo& hitInfo, Vector3& attenuation, Ray& scatteredRay)const {
		Vector3 normalOut, refracted;
		Vector3 reflected = Vector3::reflect(ray.direction, hitInfo.normal);
		float dn, reflectProb, cos;
		attenuation = Vector3(1, 1, 1);

		if (Vector3::dot(ray.direction, hitInfo.normal) > 0) {
			normalOut = -hitInfo.normal;
			dn = refIndex;
			cos = refIndex * Vector3::dot(ray.direction, hitInfo.normal) / ray.direction.length();
		}
		else {
			normalOut = hitInfo.normal;
			dn = 1 / refIndex;
			cos = -Vector3::dot(ray.direction, hitInfo.normal) / ray.direction.length();
		}

		if (Vector3::refract(ray.direction, normalOut, dn, refracted)) {
			reflectProb = schlick(cos, refIndex);
		}
		else {
			reflectProb = 1.0;
		}

		if (random() < reflectProb) {
			scatteredRay = Ray(hitInfo.position, reflected);
		}
		else {
			scatteredRay = Ray(hitInfo.position, refracted);
		}


		return true;
	}
};

class Diffuse_Light : public Material{
public:
	Vector3 light_color;

	Diffuse_Light(Vector3 p) {
		light_color = p;
	}

	virtual bool scatter(const Ray& ray, const HitInfo& hitInfo, Vector3& attenuation, Ray& scatteredRay)const {
		return false;
	}

	virtual Vector3 emitt(float u, float v, const Vector3 &p) const
	{
		return light_color;
	}
};

#endif
