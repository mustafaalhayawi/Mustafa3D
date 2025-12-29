#pragma once
#include "Geometry.h"
#include <cmath>

namespace Math {
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
}