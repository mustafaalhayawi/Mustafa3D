#pragma once
#include <vector>

struct ScreenPosition {
	int x, y;
	float z;
};

int edgeFunction(ScreenPosition A, ScreenPosition B, ScreenPosition C);

struct Vector3 {
	float x, y, z;

	Vector3 operator+(const Vector3& vector) const {
		Vector3 result;
		result.x = this->x + vector.x;
		result.y = this->y + vector.y;
		result.z = this->z + vector.z;
		return result;
	}

	Vector3 operator-(const Vector3& vector) const {
		Vector3 result;
		result.x = this->x - vector.x;
		result.y = this->y - vector.y;
		result.z = this->z - vector.z;
		return result;
	}

	Vector3 operator*(const float scalar) const {
		Vector3 result;
		result.x = this->x * scalar;
		result.y = this->y * scalar;
		result.z = this->z * scalar;
		return result;
	}

	Vector3 operator/(const float scalar) const {
		Vector3 result;
		float inverse = 1 / scalar;
		result.x = this->x * inverse;
		result.y = this->y * inverse;
		result.z = this->z * inverse;
		return result;
	}
};

struct Vector2 {
	float x, y;

	Vector3 operator+(const Vector2& vector) const {
		Vector3 result;
		result.x = this->x + vector.x;
		result.y = this->y + vector.y;
		return result;
	}

	Vector3 operator-(const Vector2& vector) const {
		Vector3 result;
		result.x = this->x - vector.x;
		result.y = this->y - vector.y;
		return result;
	}

	Vector3 operator*(const float scalar) const {
		Vector3 result;
		result.x = this->x * scalar;
		result.y = this->y * scalar;
		return result;
	}

	Vector3 operator/(const float scalar) const {
		Vector3 result;
		float inverse = 1 / scalar;
		result.x = this->x * inverse;
		result.y = this->y * inverse;
		return result;
	}
};