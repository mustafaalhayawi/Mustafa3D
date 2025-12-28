#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

int main(int argc, char* argv[]) {
	SDL_Window *window;
	bool done = false;

	SDL_Init(SDL_INIT_VIDEO);

	// create the window
	window = SDL_CreateWindow(
		"Mustafa3D",
		800, // width in pixels
		600, // height in pixels
		SDL_WINDOW_OPENGL
	);

	// check if computer is out of memory or drivers are broken
	if (window == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
		return 1;
	}

	while (!done) {
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				done = true;
			}
		}
	}

	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
}