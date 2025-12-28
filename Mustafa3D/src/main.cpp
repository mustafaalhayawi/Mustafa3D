#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

int* gFrameBuffer;
SDL_Window* gWindow;
SDL_Renderer* gRenderer;
SDL_Texture* gTexture;
static int gDone;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

bool handleEvents() {
	SDL_Event e;
	if (SDL_PollEvent(&e)) {
		if (e.type == SDL_EVENT_QUIT) return false;
		if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_ESCAPE) return false;
	}

	return true;
}

void drawPixel(int x, int y, int color) {
	if (x < 0 ||
		y < 0 ||
		x >= WINDOW_WIDTH ||
		y >= WINDOW_HEIGHT)
	{
		return;
	}

	gFrameBuffer[y * WINDOW_WIDTH + x] = color;
}

void render() {
	for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++) {
		gFrameBuffer[i] = 0xFF;
	}

	for (int i = 0; i < 100; i++) {
		for (int j = 0; j < 100; j++) {
			drawPixel(j + 50, i + 50, 0xff0000ff);
		}
	}
}

void presentFrame() {
	char* pix;
	int pitch;

	SDL_LockTexture(gTexture, NULL, (void**)&pix, &pitch);
	for (int i = 0, sp = 0, dp = 0; i < WINDOW_HEIGHT; i++, dp += WINDOW_WIDTH, sp += pitch)
		memcpy(pix + sp, gFrameBuffer + dp, WINDOW_WIDTH * 4);

	SDL_UnlockTexture(gTexture);
	SDL_RenderTexture(gRenderer, gTexture, NULL, NULL);
	SDL_RenderPresent(gRenderer);
}

void loop() {
	if (!handleEvents()) {
		gDone = 1;
	}
	else {
		render();
		presentFrame();
		SDL_Delay(1);
	}
}

int main(int argc, char* argv[]) {
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
		return -1;
	}

	gFrameBuffer = new int[WINDOW_WIDTH * WINDOW_HEIGHT];
	gWindow = SDL_CreateWindow("Mustafa3D", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	gRenderer = SDL_CreateRenderer(gWindow, NULL);
	gTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING,
									WINDOW_WIDTH, WINDOW_HEIGHT);

	if (!gFrameBuffer || !gWindow || !gRenderer || !gTexture) {
		return -1;
	}

	gDone = 0;
	while (!gDone) {
		loop();
	}

	SDL_DestroyTexture(gTexture);
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	SDL_Quit();

	return 0;
}