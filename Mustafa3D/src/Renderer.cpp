#include "Renderer.h"
#include <iostream> // temp

namespace {
	struct EdgeHash {
		std::size_t operator()(const std::pair<int, int>& p) const {
			auto h1 = std::hash<int>{}(p.first);
			auto h2 = std::hash<int>{}(p.second);

			return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
		}
	};
}

Renderer::Renderer(int width, int height, Camera camera, Vector3 light_source)
	: m_width(width),
	  m_height(height),
	  m_camera(camera),
	  m_lightSource(Math::normalise(light_source)), // normalise the light source vector
	  m_zBuffer(width * height, 0.0f)
{
	m_frameBuffer = new int[width * height];
}

Renderer::~Renderer()
{
	delete[] m_frameBuffer;
}

void Renderer::clear(int color) {
	// clear the frame buffer (the screen) by making all pixels the color given
	// clear the zBuffer too
	for (int i = 0; i < m_width * m_height; i++) {
		m_frameBuffer[i] = color;
		m_zBuffer[i] = 0.0f;
	}
}

void Renderer::render(float deltaTime, std::pair<int, int> deltaMouse, MovementKeys movementKeys) {
	clear(0xff000000);
	static Mesh mesh;

	if (mesh.vertices.empty()) loadMesh(mesh, "model/model.obj", "model/texture.png");

	m_camera.update(movementKeys, deltaTime);

	static Entity myEntity(&mesh);
	myEntity.position = Vector3(0, -1, 4);
	myEntity.rotation = Vector3(0, 2, 0);
	myEntity.update(deltaTime);
	drawMesh(myEntity);
}

void Renderer::drawPixel(ScreenPosition pixel, uint32_t color) {
	// outdated function because I realised that updating the frame buffer directly is more efficient
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

uint32_t Renderer::combineColours(uint32_t color1, uint32_t color2) {
	int r = ((color1 >> 16) & 0xFF) + ((color2 >> 16) & 0xFF);
	int g = ((color1 >> 8) & 0xFF) + ((color2 >> 8) & 0xFF);
	int b = (color1 & 0xFF) + (color2 & 0xFF);

	// Clamp to 255
	r = std::min(255, r);
	g = std::min(255, g);
	b = std::min(255, b);

	return (0xFF000000) | (r << 16) | (g << 8) | b;
}

uint32_t Renderer::packColor(const Vector3& color) {
	// turns a Vector3 with floating values 0-1 for rgb into a uint32_t
	uint32_t r = (uint32_t)(std::max(0.0f, std::min(1.0f, color.x)) * 255.0f);
	uint32_t g = (uint32_t)(std::max(0.0f, std::min(1.0f, color.y)) * 255.0f);
	uint32_t b = (uint32_t)(std::max(0.0f, std::min(1.0f, color.z)) * 255.0f);
	uint32_t a = 255;

	return (a << 24) | (r << 16) | (g << 8) | b;
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

void Renderer::drawTriangle(Vertex A, Vertex B, Vertex C, const Material* material) {
	// get the positions of each vertex on the screen
	ScreenPosition v0 = m_camera.spaceToScreen(A.position, m_width, m_height);
	ScreenPosition v1 = m_camera.spaceToScreen(B.position, m_width, m_height);
	ScreenPosition v2 = m_camera.spaceToScreen(C.position, m_width, m_height);

	if (v0.z == -1.0f || v1.z == -1.0f || v2.z == -1.0f) return;

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

	bool useTexture = (material != nullptr && material->diffuseMap != nullptr);

	// calculate the step of each weight along x
	int dw0_dx = v0.y - v1.y;
	int dw1_dx = v1.y - v2.y;
	int dw2_dx = v2.y - v0.y;

	ScreenPosition P;
	for (P.y = minY; P.y <= maxY; P.y++) {
		int idx = P.y * m_width;

		P.x = minX;

		// calculate the weights, i.e. w0 is the area of the subtriangle opposite vertex 0
		int w0 = edgeFunction(v0, v1, P);
		int w1 = edgeFunction(v1, v2, P);
		int w2 = edgeFunction(v2, v0, P);
		for (P.x = minX; P.x <= maxX; P.x++) {
			// todo: create a top-left fill rule to prent overdraw
			if (totalArea < 0 && w0 <= 0 && w1 <= 0 && w2 <= 0) {
				float fw0 = (float)w1 * invTotalArea;
				float fw1 = (float)w2 * invTotalArea;
				float fw2 = (float)w0 * invTotalArea;

				float interpolatedInvZ = (A_invZ * fw0) + (B_invZ * fw1) + (C_invZ * fw2);

				if (interpolatedInvZ > m_zBuffer[idx + P.x]) {
					m_zBuffer[idx + P.x] = interpolatedInvZ;

					Vector3 perspectiveNormal = (A.normal * A_invZ * fw0) + (B.normal * B_invZ * fw1) + (C.normal * C_invZ * fw2);
					perspectiveNormal = perspectiveNormal / interpolatedInvZ;
					Vector3 interpolatedNormal = Math::normalise(perspectiveNormal);

					Vector3 perspectivePos = (A.position * A_invZ * fw0) + (B.position * B_invZ * fw1) + (C.position * C_invZ * fw2);
					Vector3 interpolatedWorldPos = perspectivePos / interpolatedInvZ;
					Vector3 viewVector = Math::normalise(m_camera.position - interpolatedWorldPos);

					Vector3 lightDirection = Math::normalise(m_lightSource - interpolatedWorldPos);

					float specularIntensity = 0.0f;

					float dotNL = Math::dotProduct(interpolatedNormal, lightDirection);
					if (dotNL > 0.0f) {
						Vector3 reflectionVector = interpolatedNormal * 2 * dotNL - lightDirection;
						specularIntensity = (float)std::pow(std::max(0.0f, Math::dotProduct(reflectionVector, viewVector)), material->shininess);
					}
					
					float ambient = 0.2f;
					float diffuseIntensity = std::min(1.0f, ambient + std::max(0.0f, Math::dotProduct(interpolatedNormal, lightDirection)));

					Vector3 color;
					
					if (useTexture) {
						float interpolatedU = (A.uv.x * A_invZ * fw0) + (B.uv.x * B_invZ * fw1) + (C.uv.x * C_invZ * fw2);
						float interpolatedV = (A.uv.y * A_invZ * fw0) + (B.uv.y * B_invZ * fw1) + (C.uv.y * C_invZ * fw2);

						float finalU = interpolatedU / interpolatedInvZ;
						float finalV = interpolatedV / interpolatedInvZ;

						color = material->diffuseMap->getColor(finalU, finalV);
					}
					else {
						color = material->diffuse;
					}

					uint32_t shadedColor = applyIntensity(packColor(color), diffuseIntensity);
					uint32_t specularColor = applyIntensity(0xFFFFFFFF, specularIntensity);

					uint32_t finalColor = combineColours(shadedColor, specularColor);
 
					m_frameBuffer[idx + P.x] = finalColor;
				}
			}

			w0 += dw0_dx;
			w1 += dw1_dx;
			w2 += dw2_dx;
		}
	}
}

void Renderer::drawWireMesh(const Entity& entity, uint32_t color) {
	// function may not work anymore
	std::unordered_set<std::pair<int, int>, EdgeHash> drawnEdges;

	std::vector<ScreenPosition> screenPositions;
	screenPositions.reserve(entity.worldMesh.vertices.size());
	for (const auto& vertex : entity.worldMesh.vertices) {
		screenPositions.push_back(m_camera.spaceToScreen(vertex.position, m_width, m_height));
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

void Renderer::drawMesh(const Entity& entity) {
	for (size_t i = 0; i < entity.mesh->triangles.size(); i++) {
		const auto& triangle = entity.mesh->triangles[i];

		drawTriangle(entity.worldMesh.vertices[triangle.vertex1], entity.worldMesh.vertices[triangle.vertex2], entity.worldMesh.vertices[triangle.vertex3], entity.mesh->material);
	}
}