#include "Primitives.h"

Mesh Primitives::CreateCube(Mesh& outMesh, float size) {
	float half_size = size / 2;

	std::vector<Vertex> vertices;

	for (int i = 0; i < 8; i++) {
		Vertex newVertex;
		newVertex.position = Vector3(0, 0, 0);

		if (i & 1) newVertex.position = newVertex.position + Vector3(half_size, 0, 0);
		else newVertex.position = newVertex.position - Vector3(half_size, 0, 0);
		if (i & 2) newVertex.position = newVertex.position + Vector3(0, half_size, 0);
		else newVertex.position = newVertex.position - Vector3(0, half_size, 0);
		if (i & 4) newVertex.position = newVertex.position + Vector3(0, 0, half_size);
		else newVertex.position = newVertex.position - Vector3(0, 0, half_size);

		vertices.push_back(newVertex);
	}

	std::vector<int> indices = {
		0, 2, 1,	1, 2, 3,
		1, 3, 5,	3, 7, 5,
		4, 5, 7,	4, 6, 7,
		0, 4, 6,	0, 6, 2,
		0, 1, 5,	0, 5, 4,
		2, 7, 3,	2, 6, 7
	};

	std::vector<Triangle> triangles;

	for (int i = 0; i < 36; i += 3) {
		Triangle newTriangle;

		newTriangle.vertex1 = indices[i];
		newTriangle.vertex2 = indices[i + 1];
		newTriangle.vertex3 = indices[i + 2];

		triangles.push_back(newTriangle);
	}

	outMesh.vertices = vertices;
	outMesh.triangles = triangles;
}