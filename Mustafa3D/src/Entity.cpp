#include "Entity.h"
#include "MathUtils.h"

Entity::Entity(const Mesh* meshResource) {
	mesh = meshResource;

	position = Vector3(0, 0, 0);
	rotation = Vector3(0, 0, 0);
}

void Entity::update(float deltaTime, std::pair<int, int> deltaMouse) {
	if (!mesh) return;

	rotation.x += deltaMouse.second / 100.0f;
	rotation.y -= deltaMouse.first / 100.0f;

	worldMesh.vertices.clear();
	worldMesh.vertices.reserve(mesh->vertices.size());

	// precalculate values to prevent recalculating values
	float cosX = std::cos(rotation.x), sinX = std::sin(rotation.x);
	float cosY = std::cos(rotation.y), sinY = std::sin(rotation.y);
	float cosZ = std::cos(rotation.z), sinZ = std::sin(rotation.z);

	// rotate vertices
	for (const auto& vertex : mesh->vertices) {
		Vertex v;

		v.position = vertex.position;
		v.normal = vertex.normal;

		Vector3 tempPosition, tempNormal;

		// rotate around x-axis
		tempPosition.y = v.position.y * cosX - v.position.z * sinX;
		tempPosition.z = v.position.y * sinX + v.position.z * cosX;
		v.position.y = tempPosition.y; v.position.z = tempPosition.z;

		tempNormal.y = v.normal.y * cosX - v.normal.z * sinX;
		tempNormal.z = v.normal.y * sinX + v.normal.z * cosX;
		v.normal.y = tempNormal.y; v.normal.z = tempNormal.z;

		// rotate around y-axis
		tempPosition.x = v.position.x * cosY + v.position.z * sinY;
		tempPosition.z = v.position.x * -sinY + v.position.z * cosY;
		v.position.x = tempPosition.x; v.position.z = tempPosition.z;

		tempNormal.x = v.normal.x * cosY + v.normal.z * sinY;
		tempNormal.z = v.normal.x * -sinY + v.normal.z * cosY;
		v.normal.y = tempNormal.y; v.normal.z = tempNormal.z;

		// rotate around z-axis
		tempPosition.x = v.position.x * cosZ - v.position.y * sinZ;
		tempPosition.y = v.position.x * sinZ + v.position.y * cosZ;
		v.position.x = tempPosition.x; v.position.y = tempPosition.y;

		tempNormal.x = v.normal.y * cosZ - v.normal.y * sinZ;
		tempNormal.y = v.normal.y * sinZ + v.normal.y * cosZ;
		v.normal.x = tempNormal.x; v.normal.y = tempNormal.y;

		v.position = v.position + position;

		worldMesh.vertices.push_back(v);
	}
}