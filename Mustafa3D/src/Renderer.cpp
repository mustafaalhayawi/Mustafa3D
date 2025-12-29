#include "Renderer.h"
#include <algorithm>

Renderer::Renderer(int width, int height)
	: m_width(width),
	  m_height(height)
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
	drawLine(250, 387, 400, 127, 0xff0000ff);
	drawLine(400, 127, 550, 387, 0xff0000ff);
	drawLine(550, 387, 250, 387, 0xff0000ff);
}

void Renderer::drawPixel(int x, int y, int color) {
	if (x < 0 ||
		y < 0 ||
		x >= m_width ||
		y >= m_height)
	{
		return;
	}

 	m_frameBuffer[y * m_width + x] = color;
}

// Bresenham's line algorithm 
void Renderer::drawLine(int x1, int y1, int x2, int y2, int color) {
	int dx = std::abs(x2 - x1);
	int dy = std::abs(y2 - y1);
	int step_x = (x1 < x2) ? 1 : -1;
	int step_y = (y1 < y2) ? 1 : -1;

	int err = dx - dy;

	while (true) {
		drawPixel(x1, y1, color);

		if (x1 == x2 && y1 == y2) break;

		int e2 = 2 * err;
		if (e2 > -dy) {
			err -= dy;
			x1 += step_x;
		}
		if (e2 < dx) {
			err += dx;
			y1 += step_y;
		}
	}
}