#pragma once
#include "Geometry.h"
#include "Material.h"

struct Vertex {
	Vector3 position = { 0,0,0 };
	Vector3 normal = { 0, 0, 1 };
	Vector2 uv = { 0, 0 };
};

struct Triangle {
	unsigned int vertex1, vertex2, vertex3;
};

struct Mesh {
	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;

	Material* material;
};