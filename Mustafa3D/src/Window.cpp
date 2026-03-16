#include "Window.h"
#include <iostream>

Window::Window(const char* title, int width, int height)
	: m_width(width),
	  m_height(height),
	  m_window(nullptr),
	  m_renderer(nullptr),
	  m_texture(nullptr),
	  m_closed(false),
	  m_isDragging(false)
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

void Window::handleEvents(std::pair<int, int>& deltaMouse, MovementKeys& movementKeys) {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_EVENT_QUIT) m_closed = true;
		if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) m_isDragging = true;
		else if (e.type == SDL_EVENT_MOUSE_BUTTON_UP) m_isDragging = false;

		if (m_isDragging && e.type == SDL_EVENT_MOUSE_MOTION) {
			deltaMouse.first += e.motion.xrel;
			deltaMouse.second += e.motion.yrel;
		}
	}

	const bool* state = SDL_GetKeyboardState(NULL);

	movementKeys.forward = state[SDL_SCANCODE_W];
	movementKeys.backward = state[SDL_SCANCODE_S];
	movementKeys.left = state[SDL_SCANCODE_A];
	movementKeys.right = state[SDL_SCANCODE_D];
	movementKeys.up = state[SDL_SCANCODE_E];
	movementKeys.down = state[SDL_SCANCODE_Q];

	if (state[SDL_SCANCODE_ESCAPE]) m_closed = true;
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