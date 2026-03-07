#include "Renderer.h"
#include "Primitives.h"
#include "MathUtils.h"
#include "ObjLoader.h"
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <utility>
#include <type_traits>

namespace {
	struct EdgeHash {
		std::size_t operator()(const std::pair<int, int>& p) const {
			auto h1 = std::hash<int>{}(p.first);
			auto h2 = std::hash<int>{}(p.second);

			return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
		}
	};
}

Renderer::Renderer(int width, int height, float camera_distance, Vector3 light_source, float FOV)
	: m_width(width),
	  m_height(height),
	  m_cameraDistance(camera_distance),
	  m_lightSource(light_source / Math::getMagnitude(light_source)), // normalise the light source vector
	  m_zBuffer(width * height, 0.0f),
	  m_projectionScale(1.0f / tan(FOV * (Math::pi / 180.0f) / 2)),
	  m_aspectRatio((float)m_height / (float)m_width)
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

	//static Mesh cubeMesh;
	//if (cubeMesh.vertices.empty()) Primitives::createCube(cubeMesh, 10.0f);

	//static Entity redCube(&cubeMesh);
	//redCube.position = Vector3(2, -1, 10);
	//redCube.update(deltaTime);
	//drawMesh(redCube, 0xff0000ff);

	//static Entity greenCube(&cubeMesh);
	//greenCube.position = Vector3(-8, 10, 12);
	//greenCube.update(deltaTime);
	//drawMesh(greenCube, 0xff00ff00);

	static Mesh mesh;
	if (mesh.vertices.empty()) loadMesh(mesh, "assets/monkey.obj");

	static Entity myEntity(&mesh);
	myEntity.position = Vector3(0, 0, 0);
	myEntity.update(deltaTime);
	drawMesh(myEntity, 0xff0000ff);
}

template<typename T>
T Renderer::spaceToScreen(Vector3 position) {
	T screenPosition;

	float z_depth = position.z + m_cameraDistance;

	if (z_depth < 1.0f) {
		z_depth = 1.0f;
	}

	float normalizedX = (position.x / z_depth) * m_projectionScale * m_aspectRatio;
	float normalizedY = (position.y / z_depth) * m_projectionScale;

	float finalX = (normalizedX + 1.0f) * 0.5f * m_width;
	float finalY = (1.0f - normalizedY) * 0.5f * m_height;

	screenPosition.x = static_cast<decltype(screenPosition.x)>(finalX);
	screenPosition.y = static_cast<decltype(screenPosition.y)> (finalY);
	screenPosition.z = z_depth;

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

// previous rasteriser using scanline
//void Renderer::drawTriangle(Vector3 vertexA, Vector3 vertexB, Vector3 vertexC, uint32_t color) {
//	Vector3 v0 = spaceToScreen<Vector3>(vertexA);
//	Vector3 v1 = spaceToScreen<Vector3>(vertexB);
//	Vector3 v2 = spaceToScreen<Vector3>(vertexC);
//
//	v0.z = 1.0f / v0.z;
//	v1.z = 1.0f / v1.z;
//	v2.z = 1.0f / v2.z;
//
//	// sort the vertices in descending order of y values
//	if (v0.y > v1.y) std::swap(v0, v1);
//	if (v0.y > v2.y) std::swap(v0, v2);
//	if (v1.y > v2.y) std::swap(v1, v2);
//
//	float dy_total = (v2.y - v0.y); // total height of the triangle
//
//	if (std::abs(dy_total) < 0.0001f) return;
//
//	float long_dxdy = (v2.x - v0.x) / dy_total;
//	
//	float dy_middle = (v1.y - v0.y);
//
//	Vector3 vMidL = v1;
//	Vector3 vMidR;
//	vMidR.x = v0.x + (dy_middle * long_dxdy);
//	vMidR.y = v1.y;
//	vMidR.z = v0.z + (dy_middle * ((v2.z - v0.z) / dy_total));
//
//	if (vMidL.x > vMidR.x) std::swap(vMidL, vMidR);
//
//	float top_height = (float)(vMidL.y - v0.y);
//
//	if (top_height > 0.5f)
//	{
//		float dxdy_left = (vMidL.x - v0.x) / top_height;
//		float dxdy_right = (vMidR.x - v0.x) / top_height;
//
//		float dinvZdy_left = (vMidL.z - v0.z) / top_height;
//		float dinvZdy_right = (vMidR.z - v0.z) / top_height;
//
//		int yStart = std::max(0, (int)std::ceil(v0.y));
//		int yEnd = std::min(m_height, (int)std::ceil(vMidL.y));
//
//		float yPrestep = (float)(yStart - v0.y);
//
//		float x_left_cur = v0.x + (dxdy_left * yPrestep);
//		float x_right_cur = v0.x + (dxdy_right * yPrestep);
//		float invZ_left_cur = v0.z + (dinvZdy_left * yPrestep);
//		float invZ_right_cur = v0.z + (dinvZdy_right * yPrestep);
//
//		for (int y = yStart; y < yEnd; y++) {
//			int startX = std::max(0, std::min(m_width, (int)std::ceil(x_left_cur)));
//			int endX = std::max(0, std::min(m_width, (int)std::ceil(x_right_cur)));
//
//			float x_range = (float)(x_right_cur - x_left_cur);
//
//			float invZ_step = (x_range > 0.0f) ? ((invZ_right_cur - invZ_left_cur) / x_range) : 0.0f;
//
//			float xPrestep = (float)startX - x_left_cur;
//			float currentInvZ = invZ_left_cur + (invZ_step * xPrestep);
//
//			int idx = y * m_width;
//			for (int x = startX; x < endX; x++) {
//				// z-buffering
//				 // index of the current pixel in the z_buffer vector
//				if (currentInvZ > m_zBuffer[x+idx]) {
//					m_zBuffer[x+idx] = currentInvZ;
//					m_frameBuffer[x+idx] = color;
//				}
//
//				currentInvZ += invZ_step;
//			}
//
//			x_left_cur += dxdy_left;
//			x_right_cur += dxdy_right;
//			invZ_left_cur += dinvZdy_left;
//			invZ_right_cur += dinvZdy_right;
//		}
//	}
//
//	float bot_height = (float)(v2.y - vMidL.y);
//
//	if (bot_height > 0.5f)
//	{
//		float dxdy_left = (v2.x - vMidL.x) / bot_height;
//		float dxdy_right = (v2.x - vMidR.x) / bot_height;
//
//		float dinvZdy_left = (v2.z - vMidL.z) / bot_height;
//		float dinvZdy_right = (v2.z - vMidR.z) / bot_height;
//
//		int yStart = std::max(0, (int)std::ceil(vMidL.y));
//		int yEnd = std::min(m_height, (int)std::ceil(v2.y));
//
//		float yPrestep = (float)(yStart - vMidL.y);
//
//		float x_left_cur = vMidL.x + (dxdy_left * yPrestep);
//		float x_right_cur = vMidR.x + (dxdy_right * yPrestep);
//		float invZ_left_cur = vMidL.z + (dinvZdy_left * yPrestep);
//		float invZ_right_cur = vMidR.z + (dinvZdy_right * yPrestep);
//
//		for (int y = yStart; y < yEnd; y++) {
//			int startX = std::max(0, std::min(m_width, (int)std::ceil(x_left_cur)));
//			int endX = std::max(0, std::min(m_width, (int)std::ceil(x_right_cur)));
//
//			float x_range = (float)(x_right_cur - x_left_cur);
//
//			float invZ_step = (x_range > 0.0f) ? ((invZ_right_cur - invZ_left_cur) / x_range) : 0.0f;
//
//			float xPrestep = (float)startX - x_left_cur;
//			float currentInvZ = invZ_left_cur + (invZ_step * xPrestep);
//
//			int idx = y * m_width;
//			for (int x = startX; x < endX; x++) {
//				// z-buffering
//				 // index of the current pixel in the z_buffer vector
//				if (currentInvZ > m_zBuffer[x+idx]) {
//					m_zBuffer[x+idx] = currentInvZ;
//					m_frameBuffer[x+idx] = color;
//				}
//
//				currentInvZ += invZ_step;
//			}
//
//			x_left_cur += dxdy_left;
//			x_right_cur += dxdy_right;
//			invZ_left_cur += dinvZdy_left;
//			invZ_right_cur += dinvZdy_right;
//		}
//	}
//}

void Renderer::drawTriangle(Vertex A, Vertex B, Vertex C, uint32_t color) {
	ScreenPosition v0 = spaceToScreen(A.position);
	ScreenPosition v1 = spaceToScreen(B.position);
	ScreenPosition v2 = spaceToScreen(C.position);

	// create bounding box based on triangle
	int minX = std::min({ v0.x, v1.x, v2.x });
	int minY = std::min({ v0.y, v1.y, v2.y });
	int maxX = std::max({ v0.x, v1.x, v2.x });
	int maxY = std::max({ v0.y, v1.y, v2.y });

	// clip bounding box with screen boundaries
	minX = std::max(0, minX);
	minY = std::max(0, minY);
	maxX = std::min(maxX, m_width - 1);
	maxY = std::min(maxY, m_height - 1);

	// area of triangle
	int totalArea = edgeFunction(v0, v1, v2);
	if (totalArea == 0) return;
	float invTotalArea = 1.0f / totalArea;

	// get depths of vertices of triangle
	float A_invZ = 1.0f / v0.z;
	float B_invZ = 1.0f / v1.z;
	float C_invZ = 1.0f / v2.z;

	ScreenPosition P;
	for (P.y = minY; P.y <= maxY; P.y++) {
		int idx = P.y * m_width;
		for (P.x = minX; P.x <= maxX; P.x++) {
			int w0 = edgeFunction(v0, v1, P);
			int w1 = edgeFunction(v1, v2, P);
			int w2 = edgeFunction(v2, v0, P);

			// todo: create a top-left fill rule to prent overdraw
			if (totalArea < 0 && w0 <= 0 && w1 <= 0 && w2 <= 0) {
				float fw0 = (float)w1 * invTotalArea;
				float fw1 = (float)w2 * invTotalArea;
				float fw2 = (float)w0 * invTotalArea;

				float interpolatedInvZ = (A_invZ * fw0) + (B_invZ * fw1) + (C_invZ * fw2);

				Vector3 perspectiveNormal = (A.normal * A_invZ * fw0) + (B.normal * B_invZ * fw1) + (C.normal * C_invZ * fw2);
				perspectiveNormal = perspectiveNormal / interpolatedInvZ;
				Vector3 interpolatedNormal = Math::normalise(perspectiveNormal);

				if (interpolatedInvZ > m_zBuffer[idx + P.x]) {
					m_zBuffer[idx + P.x] = interpolatedInvZ;

					float lightIntensity = Math::dotProduct(interpolatedNormal, m_lightSource);
					float ambient = 0.2f;
					lightIntensity = std::min(1.0f, std::max(ambient, lightIntensity));

					uint32_t shadedColor = applyIntensity(color, lightIntensity);

					m_frameBuffer[idx + P.x] = shadedColor;
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
		const auto& triangle = entity.mesh->triangles[i];

		drawTriangle(entity.worldMesh.vertices[triangle.vertex1], entity.worldMesh.vertices[triangle.vertex2], entity.worldMesh.vertices[triangle.vertex3], color);
	}
}