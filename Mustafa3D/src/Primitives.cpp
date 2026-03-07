#include "Primitives.h"

void Primitives::createCube(Mesh& outMesh, float size) {
	float h = size / 2.0f;

	std::vector<Vertex> vertices(24);
	std::vector<Triangle> triangles(12);

	struct FaceData {
		Vector3 normal;
		Vector3 positions[4];
	};

	FaceData faces[6] = {
		// front face
		{ Vector3(0, 0, -1), { Vector3(-h, -h, -h), Vector3(h, -h, -h), Vector3(h,  h, -h), Vector3(-h,  h, -h) } },
		// back face
		{ Vector3(0, 0,  1), { Vector3(h, -h,  h), Vector3(-h, -h,  h), Vector3(-h,  h,  h), Vector3(h,  h,  h) } },
		// left face
		{ Vector3(-1, 0, 0), { Vector3(-h,  h,  h), Vector3(-h, -h,  h), Vector3(-h, -h, -h), Vector3(-h,  h, -h) } },
		// right face
		{ Vector3(1, 0, 0), { Vector3(h,  h, -h), Vector3(h, -h, -h), Vector3(h, -h,  h), Vector3(h,  h,  h) } },
		// top face
		{ Vector3(0,  1, 0), { Vector3(-h,  h,  h), Vector3(-h,  h, -h), Vector3(h,  h, -h), Vector3(h,  h,  h) } },
		// bottom face
		{ Vector3(0, -1, 0), { Vector3(-h, -h, -h), Vector3(-h, -h,  h), Vector3(h, -h,  h), Vector3(h, -h, -h) } }
	};

	int vertexIndex = 0, triangleIndex = 0;

	for (int i = 0; i < 6; i++) {
		int startIdx = vertexIndex;

		for (int j = 0; j < 4; j++) {
			Vertex v;
			v.position = faces[i].positions[j];
			v.normal = faces[i].normal;
			vertices[vertexIndex++] = v;
		}

		triangles[triangleIndex].vertex1 = startIdx;
		triangles[triangleIndex].vertex2 = startIdx + 1;
		triangles[triangleIndex].vertex3 = startIdx + 2;
		triangleIndex++;

		triangles[triangleIndex].vertex1 = startIdx;
		triangles[triangleIndex].vertex2 = startIdx + 2;
		triangles[triangleIndex].vertex3 = startIdx + 3;
		triangleIndex++;
	}

	outMesh.vertices = vertices;
	outMesh.triangles = triangles;
}