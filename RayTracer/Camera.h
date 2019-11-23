#ifndef CAMERA_H
#define CAMERA_H
#include "RTCommonDefines.h"
#include "Vector3.h"
#include "Random.h"

class Camera {
public:
	Vector3 position;
	Vector3 width;
	Vector3 height;
	Vector3 forward, right, up;
	float focusdistance;
	float xres;
	float yres;
	float lensRadius;

	Camera() {}
	Camera(Vector3 p, Vector3 lookAt, float verticalFOV, float aspect, float aperture, float focusDistance) {
		position = p;
		lensRadius = aperture / 2;

		//figure out dimensions
		float theta = verticalFOV * PI / 180;
		float halfHeight = tan(theta / 2);
		float halfWidth = aspect * halfHeight;
		xres = halfWidth * 2 * focusDistance;
		yres = halfHeight * 2 * focusDistance;
		//camera matrix
		
		forward = Vector3::normalized(position - lookAt);
		right = Vector3::normalized(Vector3::cross(Vector3(0, 1, 0), forward));
		up = Vector3::cross(forward, right);

		width = right * halfWidth * focusDistance * 2;
		height = up * halfHeight * focusDistance * 2;
		upperLeftCorner = p - halfWidth * focusDistance * right + halfHeight * focusDistance * up - forward * focusDistance;
		focusdistance = focusDistance;
	}

	Ray getRay(float u, float v) {
		Vector3 rd = lensRadius * randomUnitDiskVector();
		Vector3 offset = right * rd.x() + up * rd.y();
		return Ray(position + offset, upperLeftCorner + u * width - v * height - position - offset);
	}

protected:
	Vector3 upperLeftCorner;
};

#endif