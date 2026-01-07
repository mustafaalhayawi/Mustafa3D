#include "Primitives.h"

void Primitives::createCube(Mesh& outMesh, float size) {
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
		0, 1, 2,	2, 1, 3,
		5, 4, 7,	7, 4, 6,
		1, 5, 3,	3, 5, 7,
		4, 0, 6,	6, 0, 2,
		2, 3, 6,	3, 7, 6,
		4, 5, 0,	0, 5, 1
	};

	std::vector<Triangle> triangles;

	for (int i = 0; i < 36; i += 3) {
		Triangle newTriangle;

		int index1 = indices[i];
		int index2 = indices[i + 1];
		int index3 = indices[i + 2];

		newTriangle.vertex1 = index1;
		newTriangle.vertex2 = index2;
		newTriangle.vertex3 = index3;

		Vector3 vectorA = vertices[index2].position - vertices[index1].position;
		Vector3 vectorB = vertices[index3].position - vertices[index1].position;

		newTriangle.normal = Math::normalise(Math::crossProduct(vectorA, vectorB));

		triangles.push_back(newTriangle);
	}

	outMesh.vertices = vertices;
	outMesh.triangles = triangles;
}