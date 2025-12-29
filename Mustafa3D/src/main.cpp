#include "Window.h"
#include "Renderer.h"
#include <SDL3/SDL.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float CAMERA_DISTANCE = 25.0f;
const int TARGET_FPS = 60;
const int FRAME_DELAY = 1000 / TARGET_FPS;

void runLoop(Window& window, Renderer& renderer) {

	while (window.isOpen())
	{
		Uint64 frameStart = SDL_GetTicks();

		window.handleEvents();

		renderer.clear(0xff);
		renderer.render();
		window.presentFrame(renderer.getBuffer());

		Uint64 frameTime = SDL_GetTicks() - frameStart;

		if (frameTime < FRAME_DELAY) {
			SDL_Delay(FRAME_DELAY - (Uint32)frameTime);
		}
	}
}

int main(int argc, char* argv[]) {
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
		return -1;
	}

	Window window("Mustafa3D", WINDOW_WIDTH, WINDOW_HEIGHT);
	Renderer renderer(WINDOW_WIDTH, WINDOW_HEIGHT, CAMERA_DISTANCE);

	runLoop(window, renderer);

	SDL_Quit();

	return 0;
}