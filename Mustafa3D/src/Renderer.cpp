#include "Renderer.h"
#include "Primitives.h"
#include "MathUtils.h"
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <utility>

namespace {
	struct EdgeHash {
		std::size_t operator()(const std::pair<int, int>& p) const {
			auto h1 = std::hash<int>{}(p.first);
			auto h2 = std::hash<int>{}(p.second);

			return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
		}
	};
}

Renderer::Renderer(int width, int height, float camera_distance, Vector3 light_source)
	: m_width(width),
	  m_height(height),
	  m_cameraDistance(camera_distance),
	  m_lightSource(light_source / Math::getMagnitude(light_source)), // normalise the light source vector
	  m_zBuffer(width * height, 0.0f)
{
	m_frameBuffer = new int[width * height];
}

Renderer::~Renderer()
{
	delete[] m_frameBuffer;
}

void Renderer::clear(int color) {
	for (int i = 0; i < m_width * m_height; i++) {
		m_frameBuffer[i] = color;
		m_zBuffer[i] = 0.0f;
	}
}

void Renderer::render(float deltaTime) {
	clear(0xff000000);

	static Mesh cubeMesh;
	if (cubeMesh.vertices.empty()) Primitives::createCube(cubeMesh, 10.0f);

	static Entity redCube(&cubeMesh);
	redCube.position = Vector3(2, -1, 0);
	redCube.update(deltaTime);
	drawMesh(redCube, 0xff0000ff);

	static Entity greenCube(&cubeMesh);
	greenCube.position = Vector3(-7, 9, 20);
	greenCube.update(deltaTime);
	drawMesh(greenCube, 0xff00ff00);
}

ScreenPosition Renderer::spaceToScreen(Vector3 position) {
	ScreenPosition screenPosition;

	float z_depth = position.z + m_cameraDistance;

	if (z_depth < 1.0f) {
		z_depth = 1.0f;
	}

	float FOV = 90.0f; // in degrees
	float fovRad = (FOV * 0.5f) * (Math::pi / 180.0f); // convert FOV to radians
	float projectionScale = 1.0f / tan(fovRad / 2);

	float aspectRatio = (float)m_height / (float)m_width;

	float normalizedX = (position.x / z_depth) * projectionScale * aspectRatio;
	float normalizedY = (position.y / z_depth) * projectionScale;

	screenPosition.x = static_cast<int>((normalizedX + 1.0f) * 0.5f * m_width);
	screenPosition.y = static_cast<int>((1.0f - normalizedY) * 0.5f * m_height);

	return screenPosition;
}

void Renderer::drawPixel(ScreenPosition pixel, uint32_t color) {
	if (pixel.x < 0 ||
		pixel.y < 0 ||
		pixel.x >= m_width ||
		pixel.y >= m_height)
	{
		return;
	}

 	m_frameBuffer[pixel.y * m_width + pixel.x] = color;
}

uint32_t Renderer::applyIntensity(uint32_t color, float intensity) {
	// keep alpha separate
	uint32_t a = color & 0xFF000000;

	// mask and multiply rgb
	uint32_t r = (uint32_t)(((color >> 16) & 0xFF) * intensity);
	uint32_t g = (uint32_t)(((color >> 8) & 0xFF) * intensity);
	uint32_t b = (uint32_t)((color & 0xFF) * intensity);

	return a | (r << 16) | (g << 8) | b;
}

// Bresenham's line algorithm 
void Renderer::drawLine(ScreenPosition pixel1, ScreenPosition pixel2, uint32_t color) {
	// check if coordinates are massive to prevent overflows
	if (std::abs(pixel1.x) > 10000 || std::abs(pixel1.y) > 10000 ||
		std::abs(pixel2.x) > 10000 || std::abs(pixel2.y) > 10000) {
		return;
	}

	// prevent clipping
	if ((pixel1.x < 0 && pixel2.x < 0) || (pixel1.x >= m_width && pixel2.x >= m_width) ||
		(pixel1.y < 0 && pixel2.y < 0) || (pixel1.y >= m_height && pixel2.y >= m_height)) {
		return;
	}

	int dx = std::abs(pixel2.x - pixel1.x);
	int dy = std::abs(pixel2.y - pixel1.y);
	int step_x = (pixel1.x < pixel2.x) ? 1 : -1;
	int step_y = (pixel1.y < pixel2.y) ? 1 : -1;

	int err = dx - dy;

	while (true) {
		drawPixel(pixel1, color);

		if (pixel1.x == pixel2.x && pixel1.y == pixel2.y) break;

		int e2 = 2 * err;
		if (e2 > -dy) {
			err -= dy;
			pixel1.x += step_x;
		}
		if (e2 < dx) {
			err += dx;
			pixel1.y += step_y;
		}
	}
}

// todo: update algorithm to be more efficient by using line scanning
void Renderer::drawTriangle(Vector3 vectorA, Vector3 vectorB, Vector3 vectorC, uint32_t color) {
	ScreenPosition A = spaceToScreen(vectorA);
	ScreenPosition B = spaceToScreen(vectorB);
	ScreenPosition C = spaceToScreen(vectorC);
	ScreenPosition P = ScreenPosition(0, 0);

	// get bounding box of the triangle on the screen
	int minX = std::min({ A.x, B.x, C.x });
	int minY = std::min({ A.y, B.y, C.y });
	int maxX = std::max({ A.x, B.x, C.x });
	int maxY = std::max({ A.y, B.y, C.y });

	// clamp to the screen boundaries
	minX = std::max(0, minX);
	minY = std::max(0, minY);
	maxX = std::min(m_width - 1, maxX);
	maxY = std::min(m_height - 1, maxY);

	float area = (float)edgeFunction(A, B, C);

	for (P.y = minY; P.y <= maxY; P.y++) {
		for (P.x = minX; P.x <= maxX; P.x++) {
			float ABP = edgeFunction(A, B, P) / area;
			float BCP = edgeFunction(B, C, P) / area;
			float CAP = edgeFunction(C, A, P) / area;

			// backface culling
			if (ABP >= 0 && BCP >= 0 && CAP >= 0) {
				float zWorld = (BCP * vectorA.z) + (CAP * vectorB.z) + (ABP * vectorC.z) + m_cameraDistance;
				
				// don't render if too close to or behind the camera
				if (zWorld < 0.1f) continue;

				float pixelDepth = 1.0f / zWorld;

				// z-buffering
				int idx = P.x + P.y * m_width; // index of the current pixel in the z_buffer vector
				if (pixelDepth > m_zBuffer[idx]) {
					m_zBuffer[idx] = pixelDepth;

					drawPixel(P, color);
				}
			}
		}
	}
}

void Renderer::drawWireMesh(const Entity& entity, uint32_t color) {
	std::unordered_set<std::pair<int, int>, EdgeHash> drawnEdges;

	std::vector<ScreenPosition> screenPositions;
	screenPositions.reserve(entity.worldMesh.vertices.size());
	for (const auto& vertex : entity.worldMesh.vertices) {
		screenPositions.push_back(spaceToScreen(vertex.position));
	}

	for (const auto& triangle : entity.mesh->triangles) {
		unsigned int indices[3] = { triangle.vertex1, triangle.vertex2, triangle.vertex3 };

		for (int j = 0; j < 3; j++) {
			int index1 = indices[j];
			int index2 = indices[(j + 1) % 3];

			std::pair<int, int> edge = std::minmax(index1, index2);

			if (drawnEdges.insert(edge).second) {
				drawLine(screenPositions[index1], screenPositions[index2], color);
			}
		}
	}
}

void Renderer::drawMesh(const Entity& entity, uint32_t color) {
	for (size_t i = 0; i < entity.mesh->triangles.size(); i++) {
		float dot = Math::dotProduct(entity.worldMesh.normals[i], m_lightSource); // the normal and m_lightSource are already unit vectors (normalised)
		float ambient = 0.2f;
		float lightIntensity = std::min(1.0f, std::max(0.0f, dot) + ambient);

		const auto& triangle = entity.mesh->triangles[i];

		drawTriangle(entity.worldMesh.vertices[triangle.vertex1].position, entity.worldMesh.vertices[triangle.vertex2].position, entity.worldMesh.vertices[triangle.vertex3].position, applyIntensity(color, lightIntensity));
	}
}