#pragma once
#include "Geometry.h"
#include <cmath>

namespace Math {
	constexpr float pi = 3.1415926535f;

	inline Vector3 rotateX(Vector3 vector, float angle) {
		Vector3 result = vector;

		float c = std::cos(angle);
		float s = std::sin(angle);

		result.y = vector.y * c - vector.z * s;
		result.z = vector.y * s + vector.z * c;

		return result;
	}

	inline Vector3 rotateY(Vector3 vector, float angle) {
		Vector3 result = vector;

		float c = std::cos(angle);
		float s = std::sin(angle);

		result.x = vector.x * c + vector.z * s;
		result.z = vector.x * -s + vector.z * c;

		return result;
	}

	inline Vector3 rotateZ(Vector3 vector, float angle) {
		Vector3 result = vector;

		float c = std::cos(angle);
		float s = std::sin(angle);

		result.x = vector.x * c - vector.y * s;
		result.y = vector.x * s + vector.y * c;

		return result;
	}

	inline Vector3 crossProduct(Vector3 vector1, Vector3 vector2) {
		Vector3 result;

		result.x = vector1.y * vector2.z - vector1.z * vector2.y;
		result.y = vector1.z * vector2.x - vector1.x * vector2.z;
		result.z = vector1.x * vector2.y - vector1.y * vector2.x;

		return result;
	}

	inline float dotProduct(Vector3 vector1, Vector3 vector2) {
		return vector1.x * vector2.x + vector1.y * vector2.y + vector1.z * vector2.z;
	}

	inline float getMagnitude(Vector3 vector) {
		return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
	}

	inline Vector3 normalise(Vector3 vector) {
		return vector / getMagnitude(vector);
	}
}