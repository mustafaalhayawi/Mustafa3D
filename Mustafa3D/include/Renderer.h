#pragma once
#include "Entity.h"
#include "Primitives.h"
#include "MathUtils.h"
#include "ObjLoader.h"
#include "Camera.h"
#include <SDL3/SDL.h>
#include <cmath>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <utility>
#include <type_traits>

class Renderer {
public:
	Renderer(int width, int height, Camera camera, Vector3 light_source);
	~Renderer();

	void clear(int color);
	void render(float deltaTime, std::pair<int, int> deltaMouse, MovementKeys movementKeys);
	int* getBuffer() { return m_frameBuffer; }

private:
	void drawPixel(ScreenPosition pixel, uint32_t color);
	void drawLine(ScreenPosition pixel1, ScreenPosition pixel2, uint32_t color);
	void drawTriangle(Vertex vertexA, Vertex vertexB, Vertex vertexC, const Material* color);
	void drawWireMesh(const Entity& entity, uint32_t color);
	void drawMesh(const Entity& entity);
	uint32_t applyIntensity(uint32_t color, float intensity);
	uint32_t combineColours(uint32_t color1, uint32_t color2);
	uint32_t packColor(const Vector3& color);
	
	int* m_frameBuffer;
	std::vector<float> m_zBuffer;
	int m_width, m_height;
	Camera m_camera;
	Vector3 m_lightSource;
	float m_speed = 5.0f;
};