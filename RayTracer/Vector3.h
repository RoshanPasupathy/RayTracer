#ifndef VECTOR3_H
#define VECTOR3_H

#include <math.h>

class Vector3 {
public:
	float v[3];

	Vector3(){}
	Vector3(float x, float y, float z) { v[0] = x; v[1] = y; v[2] = z; }
	
	inline float x() const { return v[0]; }
	inline float y() const { return v[1]; }
	inline float z() const { return v[2]; }
	inline float r() const { return v[0]; }
	inline float g() const { return v[1]; }
	inline float b() const { return v[2]; }


	inline Vector3& operator+=(const Vector3& v2) {
		v[0] += v2.v[0];
		v[1] += v2.v[1];
		v[2] += v2.v[2];
		return *this;
	}
	inline Vector3& operator-=(const Vector3& v2) {
		v[0] -= v2.v[0];
		v[1] -= v2.v[1];
		v[2] -= v2.v[2];
		return *this;
	}
	inline Vector3& operator*=(const Vector3& v2) {
		v[0] *= v2.v[0];
		v[1] *= v2.v[1];
		v[2] *= v2.v[2];
		return *this;
	}
	inline Vector3& operator/=(const Vector3& v2) {
		v[0] /= v2.v[0];
		v[1] /= v2.v[1];
		v[2] /= v2.v[2];
		return *this;
	}
	inline Vector3& operator*=(const float f) {
		v[0] *= f;
		v[1] *= f;
		v[2] *= f;
		return *this;
	}
	inline Vector3& operator/=(const float f) {
		v[0] /= f;
		v[1] /= f;
		v[2] /= f;
		return *this;
	}
	inline const Vector3& operator+() const { return *this; }
	inline Vector3 operator-() const { return Vector3(-v[0], -v[1], -v[2]); }
	inline float operator[](int i) const { return v[i]; }
	inline float& operator[](int i) { return v[i]; }

	inline float lengthSquared() const { return v[0] * v[0] + v[1] * v[1] + v[2] * v[2]; }
	inline float length() const { return sqrt(lengthSquared()); }
	inline void normalize() {
		float l = length();
		v[0] /= l; 
		v[1] /= l;
		v[2] /= l;
	}

	inline static Vector3 normalized(Vector3 v) {
		float l = v.length();
		return Vector3(v[0] / l, v[1] / l, v[2] / l);
	}
	inline static float dot(Vector3 v1, Vector3 v2) {
		return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
	}
	inline static Vector3 cross(Vector3 v1, Vector3 v2) {
		return Vector3(v1[1] * v2[2] - v1[2] * v2[1],
					v1[2] * v2[0] - v1[0] * v2[2],
					v1[0] * v2[1] - v1[1] * v2[0]);
	}
	inline static Vector3 lerp(const Vector3& v1, const Vector3& v2, float t);
	inline static Vector3 reflect(const Vector3& v, const Vector3& n);
	static bool refract(const Vector3& v, const Vector3& n, float dn, Vector3& refracted);
};

inline Vector3 operator*(const float& f, const Vector3& v) {
	return Vector3(v.v[0] * f, v.v[1] * f, v.v[2] * f);
}
inline Vector3 operator*(const Vector3& v, const float& f) {
	return Vector3(v.v[0] * f, v.v[1] * f, v.v[2] * f);
}
inline Vector3 operator/(const Vector3& v, const float& f) {
	return Vector3(v[0] / f, v[1] / f, v[2] / f);
}
inline Vector3 operator+(const Vector3& v1, const Vector3& v2) {
	return Vector3(v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]);
}
inline Vector3 operator-(const Vector3& v1, const Vector3& v2) {
	return Vector3(v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]);
}
inline Vector3 operator*(const Vector3& v1, const Vector3& v2) {
	return Vector3(v1[0] * v2[0], v1[1] * v2[1], v1[2] * v2[2]);
}
inline Vector3 operator/(const Vector3& v1, const Vector3& v2) {
	return Vector3(v1[0] / v2[0], v1[1] / v2[1], v1[2] / v2[2]);
}

inline Vector3 Vector3::lerp(const Vector3& v1, const Vector3& v2, float t) {
	Vector3 r = ((1 - t) * v1) + t * v2;
	return r;
}
inline Vector3 Vector3::reflect(const Vector3& v, const Vector3& n) {
	return v - 2 * n * dot(v, n);
}
bool Vector3::refract(const Vector3& v, const Vector3& n, float dn, Vector3& refracted) {
	Vector3 vNorm = Vector3::normalized(v);
	float dt = Vector3::dot(vNorm, n);
	float d = 1.0 - dn * dn * (1 - dt * dt);

	if (d > 0) {
		refracted = dn * (vNorm - n * dt) - n * sqrt(d);
		refracted.normalize();
		return true;
	}

	return false;
}

#endif