#pragma once
#include "Geometry.h"

struct WorldMesh {
	std::vector<Vertex> vertices;
	std::vector<Vector3> normals;
};

struct Entity {
	const Mesh* mesh;

	Vector3 position;
	Vector3 rotation;

	WorldMesh worldMesh;

	Entity() : mesh(nullptr), position{ 0,0,0 }, rotation{ 0,0,0 } {}
	Entity(const Mesh* meshResource);

	void update(float deltaTime);
};