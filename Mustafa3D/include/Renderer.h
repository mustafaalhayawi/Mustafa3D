#pragma once
#include "Geometry.h"
#include "Entity.h"
#include <SDL3/SDL.h>

class Renderer {
public:
	Renderer(int width, int height, float camera_distance);
	~Renderer();

	void clear(int color);
	void render(float deltaTime);
	int* getBuffer() { return m_frameBuffer; }

private:
	ScreenPosition spaceToScreen(Vector3 position);
	void drawPixel(ScreenPosition pixel, int color);
	void drawLine(ScreenPosition pixel1, ScreenPosition pixel2, int color);
	void drawTriangle(ScreenPosition A, ScreenPosition B, ScreenPosition C, int color);
	void drawWireMesh(const Entity& entity, int color);
	void drawMesh(const Entity& entity, int color);
	
	int* m_frameBuffer;
	int m_width, m_height;
	float m_cameraDistance;
};