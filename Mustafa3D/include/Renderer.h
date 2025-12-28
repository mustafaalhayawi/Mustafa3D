#pragma once

class Renderer {
public:
	Renderer(int width, int height);
	~Renderer();

	void clear(int color);
	void render();
	int* getBuffer() { return m_frameBuffer; }

private:
	void drawPixel(int x, int y, int color);
	void drawLine(int x1, int y1, int x2, int y2, int color);
	
	int* m_frameBuffer;
	int m_width, m_height;
};