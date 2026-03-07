#pragma once
#include <SDL3/SDL.h>
#include <utility>

class Window {
public:
	Window(const char* title, int width, int height);
	~Window();

	bool isOpen() const { return !m_closed; }
	void handleEvents(std::pair<int, int>& deltaMouse);
	void presentFrame(int* frameBuffer);

	int getWidth() const { return m_width; }
	int getHeight() const { return m_height; }

private:
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
	SDL_Texture* m_texture;
	int m_width, m_height;
	bool m_closed;
	bool m_isDragging;
};