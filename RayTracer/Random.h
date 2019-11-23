#ifndef RANDOM_H
#define RANDOM_H

#include <cstdlib>

inline double random() {
	return rand() / (RAND_MAX + 1.0);
}

inline Vector3 randomUnitSphereVector() {
	Vector3 v(10, 1, 1);
	while (v.lengthSquared() >= 1) {
		v = 2 * Vector3(random(), random(), random()) - Vector3(1, 1, 1);
	}
	return v;
}

inline Vector3 randomUnitDiskVector() {
	Vector3 v(10, 1, 0);
	while (Vector3::dot(v, v) >= 1) {
		v = 2 * Vector3(random(), random(), 0) - Vector3(1, 1, 0);
	}
	return v;
}

#endif