#include "Window.h"
#include "Renderer.h"
#include <SDL3/SDL.h>
#include "Geometry.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float CAMERA_DISTANCE = 25.0f;
const int TARGET_FPS = 60;
const int FRAME_DELAY = 1000 / TARGET_FPS;
const Vector3 LIGHT_SOURCE = Vector3(1, 0, 1);

void runLoop(Window& window, Renderer& renderer) {
	Uint64 lastTime = SDL_GetTicks();

	while (window.isOpen())
	{
		Uint64 startTick = SDL_GetTicks();
		float deltaTime = (startTick - lastTime) / 1000.0f;
		lastTime = startTick;

		window.handleEvents();
		renderer.render(deltaTime);
		window.presentFrame(renderer.getBuffer());

		Uint64 endTick = SDL_GetTicks();
		Uint64 frameDuration = endTick - startTick;

		if (frameDuration < FRAME_DELAY) {
			SDL_Delay(FRAME_DELAY - (Uint32)frameDuration);
		}
	}
}

int main(int argc, char* argv[]) {
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
		return -1;
	}

	Window window("Mustafa3D", WINDOW_WIDTH, WINDOW_HEIGHT);
	Renderer renderer(WINDOW_WIDTH, WINDOW_HEIGHT, CAMERA_DISTANCE, LIGHT_SOURCE);

	runLoop(window, renderer);

	SDL_Quit();

	return 0;
}