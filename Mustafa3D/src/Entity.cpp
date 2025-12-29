#include "Entity.h"
#include "MathUtils.h"

Entity::Entity(const Mesh* meshResource) {
	mesh = meshResource;

	position = Vector3(0, 0, 0);
	rotation = Vector3(0, 0, 0);
}

void Entity::update(float deltaTime) {
	if (!mesh) return;

	rotation.x += 1.0f * deltaTime;
	rotation.y += 1.0f * deltaTime;

	worldMesh.vertices.clear();
	worldMesh.normals.clear();
	worldMesh.vertices.reserve(mesh->vertices.size());
	worldMesh.normals.reserve(mesh->triangles.size());

	// precalculate values to prevent recalculating values
	float cosX = std::cos(rotation.x), sinX = std::sin(rotation.x);
	float cosY = std::cos(rotation.y), sinY = std::sin(rotation.y);
	float cosZ = std::cos(rotation.z), sinZ = std::sin(rotation.z);

	// rotate vertices
	for (const auto& vertex : mesh->vertices) {
		Vertex v;
		v.position = vertex.position;

		Vector3 temp;

		// rotate around x-axis
		temp.y = v.position.y * cosX - v.position.z * sinX;
		temp.z = v.position.y * sinX + v.position.z * cosX;
		v.position.y = temp.y; v.position.z = temp.z;

		// rotate around y-axis
		temp.x = v.position.x * cosY + v.position.z * sinY;
		temp.z = v.position.x * -sinY + v.position.z * cosY;
		v.position.x = temp.x; v.position.z = temp.z;

		// rotate around z-axis
		temp.x = v.position.x * cosZ - v.position.y * sinZ;
		temp.y = v.position.x * sinZ + v.position.y * cosZ;
		v.position.x = temp.x; v.position.y = temp.y;

		v.position = v.position + position;

		worldMesh.vertices.push_back(v);
	}

	// rotate normals which are currently stored in triangles
	for (const auto& triangle : mesh->triangles) {
		Vector3 n = triangle.normal;
		Vector3 temp;

		// rotate around x-axis
		temp.y = n.y * cosX - n.z * sinX;
		temp.z = n.y * sinX + n.z * cosX;
		n.y = temp.y; n.z = temp.z;

		// rotate around y-axis
		temp.x = n.x * cosY + n.z * sinY;
		temp.z = n.x * -sinY + n.z * cosY;
		n.x = temp.x; n.z = temp.z;

		// rotate around z-axis
		temp.x = n.x * cosZ - n.y * sinZ;
		temp.y = n.x * sinZ + n.y * cosZ;
		n.x = temp.x; n.y = temp.y;

		worldMesh.normals.push_back(n);
	}
}