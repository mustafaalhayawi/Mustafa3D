#include "Window.h"

Window::Window(const char* title, int width, int height)
	: m_width(width),
	  m_height(height),
	  m_window(nullptr),
	  m_renderer(nullptr),
	  m_texture(nullptr),
	  m_closed(false)
{
	m_window = SDL_CreateWindow("Mustafa3D", m_width, m_height, 0);
	m_renderer = SDL_CreateRenderer(m_window, NULL);
	m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING,
		m_width, m_height);
}

Window::~Window() {
	if (m_texture) {
		SDL_DestroyTexture(m_texture);
	}
	if (m_renderer) {
		SDL_DestroyRenderer(m_renderer);
	}
	if (m_window) {
		SDL_DestroyWindow(m_window);
	}
}

void Window::handleEvents() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_EVENT_QUIT) m_closed = true;
		if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_ESCAPE) m_closed = true;
	}
}

void Window::presentFrame(int* frameBuffer) {
	char* pix;
	int pitch;

	SDL_LockTexture(m_texture, NULL, (void**)&pix, &pitch);
	for (int i = 0, sp = 0, dp = 0; i < m_height; i++, dp += m_width, sp += pitch)
		memcpy(pix + sp, frameBuffer + dp, m_width * 4);

	SDL_UnlockTexture(m_texture);
	SDL_RenderTexture(m_renderer, m_texture, NULL, NULL);
	SDL_RenderPresent(m_renderer);
}