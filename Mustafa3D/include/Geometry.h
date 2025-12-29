#pragma once
#include <vector>

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

// currently contains only a position vector but will later contain a normal vector
struct Vertex {
	Vector3 position;
};

struct Triangle {
	unsigned int vertex1, vertex2, vertex3;
};

struct Mesh {
	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;
};