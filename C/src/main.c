#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "defs.h"
#include "graphics.h"
#include "textures.h"
#include "map.h"
#include "ray.h"
#include "player.h"
#include "wall.h"
#include "sprite.h"

bool isGameRunning = false;
int ticksLastFrame = 0;

void setup(void) {
  loadTextures();
}

void processInput(void) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT: {
      isGameRunning = false;
      break;
    }
    case SDL_KEYDOWN: {
      if (event.key.keysym.sym == SDLK_ESCAPE) {
        isGameRunning = false;
      }

      if (event.key.keysym.sym == SDLK_UP) {
        player.walkDirection = +1;
      }

      if (event.key.keysym.sym == SDLK_DOWN) {
        player.walkDirection = -1;
      }

      if (event.key.keysym.sym == SDLK_RIGHT) {
        player.turnDirection = +1;
      }

      if (event.key.keysym.sym == SDLK_LEFT) {
        player.turnDirection = -1;
      }

      break;
    }
    case SDL_KEYUP: {
      if (event.key.keysym.sym == SDLK_UP) {
        player.walkDirection = 0;
      }

      if (event.key.keysym.sym == SDLK_DOWN) {
        player.walkDirection = 0;
      }

      if (event.key.keysym.sym == SDLK_RIGHT) {
        player.turnDirection = 0;
      }

      if (event.key.keysym.sym == SDLK_LEFT) {
        player.turnDirection = 0;
      }

      break;
    }
    }
  }
}

void update(void) {
  int timeToWait = FRAME_TIME_LENGTH - (SDL_GetTicks() - ticksLastFrame);

  if (timeToWait > 0 && timeToWait <= FRAME_TIME_LENGTH) {
    SDL_Delay(timeToWait);
  }

  float deltaTime = (SDL_GetTicks() - ticksLastFrame) / 1000.0f;

  ticksLastFrame = SDL_GetTicks();

  movePlayer(deltaTime);
  castAllRays();
}

void render(void) {
  clearColorBuffer(0xFF000000);

  renderWallProjection();
  renderSpriteProjection();
  
  // Display the minimap objects
  renderMapGrid();
  renderMapRays();
  renderMapSprites();
  renderMapPlayer();
  
  renderColorBuffer();
}

void releaseResources(void) {
  freeTextures();
  destroyWindow();
}

int main(void) {
  isGameRunning = initializeWindow();
  
  setup();

  while (isGameRunning) {
    processInput();
    update();
    render();
  }

  /* destroyWindow(); */
  releaseResources();

  return EXIT_SUCCESS;
}
