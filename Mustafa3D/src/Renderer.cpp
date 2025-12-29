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

Renderer::Renderer(int width, int height, float camera_distance)
	: m_width(width),
	  m_height(height),
	  m_cameraDistance(camera_distance)
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
	}
}

void Renderer::render() {
	clear(0xff000000);

	Mesh cubeMesh;
	Primitives::createCube(cubeMesh, 10.0f);

	drawMesh(cubeMesh, 0xff0000ff);
	drawWireMesh(cubeMesh, 0xffffffff);
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

void Renderer::drawPixel(ScreenPosition pixel, int color) {
	if (pixel.x < 0 ||
		pixel.y < 0 ||
		pixel.x >= m_width ||
		pixel.y >= m_height)
	{
		return;
	}

 	m_frameBuffer[pixel.y * m_width + pixel.x] = color;
}

// Bresenham's line algorithm 
void Renderer::drawLine(ScreenPosition pixel1, ScreenPosition pixel2, int color) {
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

void Renderer::drawTriangle(ScreenPosition A, ScreenPosition B, ScreenPosition C, int color) {
	ScreenPosition P = ScreenPosition(0, 0);

	// get bounding box of the triangle on the screen
	int minX = std::min({ A.x, B.x, C.x });
	int minY = std::min({ A.y, B.y, C.y });
	int maxX = std::max({ A.x, B.x, C.x });
	int maxY = std::max({ A.y, B.y, C.y });

	for (P.y = minY; P.y < maxY; P.y++) {
		for (P.x = minX; P.x < maxX; P.x++) {
			int APB = edgeFunction(A, P, B);
			int BPC = edgeFunction(B, P, C);
			int CPA = edgeFunction(C, P, A);

			if (APB >= 0 && BPC >= 0 && CPA >= 0) {
				drawPixel(P, color);
			}
		}
	}
}

void Renderer::drawWireMesh(Mesh mesh, int color) {
	std::unordered_set<std::pair<int, int>, EdgeHash> drawnEdges;

	static float angle = 0.0f;
	angle += 0.01f;

	std::vector<ScreenPosition> screenPositions;
	screenPositions.reserve(mesh.vertices.size());
	for (const auto& vertex : mesh.vertices) {
		Vector3 rotated = Math::rotateY(vertex.position, angle);
		rotated = Math::rotateX(rotated, angle / 2);
		screenPositions.push_back(spaceToScreen(rotated));
	}

	for (const auto& triangle : mesh.triangles) {
		int indices[3] = { triangle.vertex1, triangle.vertex2, triangle.vertex3 };

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

void Renderer::drawMesh(Mesh& mesh, int color) {
	static float angle = 0.0f;
	angle += 0.01f;

	std::vector<ScreenPosition> screenPositions;
	screenPositions.reserve(mesh.vertices.size());

	for (const auto& vertex : mesh.vertices) {
		Vector3 rotated = Math::rotateY(vertex.position, angle);
		rotated = Math::rotateX(rotated, angle / 2);
		screenPositions.push_back(spaceToScreen(rotated));
	}

	for (const auto& triangle : mesh.triangles) {
		// convert world position vectors of vertices of triangle into screen position vectors
		ScreenPosition A = screenPositions[triangle.vertex1];
		ScreenPosition B = screenPositions[triangle.vertex2];
		ScreenPosition C = screenPositions[triangle.vertex3];

		drawTriangle(A, B, C, color);
	}
}