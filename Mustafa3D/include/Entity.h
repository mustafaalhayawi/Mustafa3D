#pragma once
#include "Mesh.h"
#include <cmath>

struct WorldMesh {
	std::vector<Vertex> vertices;
};

struct Entity {
	const Mesh* mesh;

	Vector3 position;
	Vector3 rotation;

	WorldMesh worldMesh;

	Entity() : mesh(nullptr), position{ 0,0,0 }, rotation{ 0,0,0 } {}
	Entity(const Mesh* meshResource);

	void update(float deltaTime, std::pair<int, int> deltaMouse);
};