#pragma once
#include "Geometry.h"
#include "Entity.h"
#include <SDL3/SDL.h>

class Renderer {
public:
	Renderer(int width, int height, float camera_distance, Vector3 light_source, float FOV);
	~Renderer();

	void clear(int color);
	void render(float deltaTime);
	int* getBuffer() { return m_frameBuffer; }

private:
	template<typename T = ScreenPosition>
	T spaceToScreen(Vector3 position);
	void drawPixel(ScreenPosition pixel, uint32_t color);
	void drawLine(ScreenPosition pixel1, ScreenPosition pixel2, uint32_t color);
	void drawTriangle(Vertex vertexA, Vertex vertexB, Vertex vertexC, uint32_t color);
	void drawWireMesh(const Entity& entity, uint32_t color);
	void drawMesh(const Entity& entity, uint32_t color);
	uint32_t applyIntensity(uint32_t color, float intensity);
	
	int* m_frameBuffer;
	std::vector<float> m_zBuffer;
	int m_width, m_height;
	float m_cameraDistance;
	Vector3 m_lightSource;
	float m_projectionScale;
	float m_aspectRatio;
};