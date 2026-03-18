#include "Window.h"
#include "Renderer.h"
#include "Camera.h"
#include <SDL3/SDL.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float FOV = 60.0f;
const int TARGET_FPS = 30;
const int FRAME_DELAY = 1000 / TARGET_FPS;
const Vector3 CAMERA_POSITION = Vector3(-1.0f, 1.0f, -2.5f);
const Vector3 LIGHT_SOURCE = Vector3(-1.0f, 0.0f, -10.0f);

void runLoop(Window& window, Renderer& renderer) {
	Uint64 lastTime = SDL_GetTicks();

	while (window.isOpen())
	{
		Uint64 startTick = SDL_GetTicks();
		float deltaTime = (startTick - lastTime) / 1000.0f;
		std::pair<int, int> deltaMouse;
		MovementKeys movementKeys;
		lastTime = startTick;

		window.handleEvents(deltaMouse, movementKeys);
		renderer.render(deltaTime, deltaMouse, movementKeys);
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
	Camera camera(CAMERA_POSITION, 0.0f, 0.0f, FOV);
	Renderer renderer(WINDOW_WIDTH, WINDOW_HEIGHT, camera, LIGHT_SOURCE);

	runLoop(window, renderer);

	SDL_Quit();

	return 0;
}