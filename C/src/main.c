#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "constants.h"
#include "textures.h"

const int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 2, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 5, 5, 5, 5, 5}
};

struct Player {
  float x;
  float y;
  float width;
  float height;
  int turnDirection; // -1 for left and +1 for right
  int walkDirection; // -1 for back and +1 for forward
  float rotationAngle;
  float walkSpeed;
  float turnSpeed;
} player;

struct Ray {
  float rayAngle;
  float wallHitX;
  float wallHitY;
  float distance;
  bool wasHitVertical;
  int wallHitContent;
} rays[NUM_RAYS];

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

bool isGameRunning = false;
int ticksLastFrame = 0;

uint32_t *colorBuffer = NULL;

SDL_Texture *colorBufferTexture;

bool InitializeWindow(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "Error initializing SDL.\n");
    return false;
  }

  window =
      SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_BORDERLESS
                       // 0
      );

  if (!window) {
    fprintf(stderr, "Error creating SDL window.\n");
    return false;
  }

  renderer = SDL_CreateRenderer(window,
                                -1, // get the default driver
                                0);

  if (!renderer) {
    fprintf(stderr, "Error creating SDL renderer.\n");
    return false;
  }

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  return true;
}

void destroyWindow(void) {
  freeWallTextures();
  free(colorBuffer);
  SDL_DestroyTexture(colorBufferTexture);
  colorBuffer = NULL;
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

bool mapHasWallAt(float x, float y) {
  if (x < 0 || x > WINDOW_WIDTH || y < 0 || y > WINDOW_HEIGHT) {
    return true;
  }

  int mapGridIndexX = floor(x / TILE_SIZE);
  int mapGridIndexY = floor(y / TILE_SIZE);
  return map[mapGridIndexY][mapGridIndexX] != 0;
}

void movePlayer(float deltaTime) {
  player.rotationAngle += player.turnDirection * player.turnSpeed * deltaTime;

  float moveStep = player.walkDirection * player.walkSpeed * deltaTime;

  float newPlayerX = player.x + cos(player.rotationAngle) * moveStep;
  float newPlayerY = player.y + sin(player.rotationAngle) * moveStep;

  if (!mapHasWallAt(newPlayerX, newPlayerY)) {
    player.x = newPlayerX;
    player.y = newPlayerY;
  }
}

void setup(void) {
  player.x = WINDOW_WIDTH / 2;
  player.y = WINDOW_HEIGHT / 2;
  player.width = 1;
  player.height = 1;
  player.turnDirection = 0;
  player.walkDirection = 0;
  player.rotationAngle = PI / 2;
  player.walkSpeed = 100;
  player.turnSpeed = 45 * (PI / 180);

  colorBuffer = (uint32_t *)malloc(sizeof(uint32_t) * (uint32_t)WINDOW_HEIGHT *
                                 (uint32_t)WINDOW_WIDTH);

  colorBufferTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
					 SDL_TEXTUREACCESS_STREAMING,
                                         WINDOW_WIDTH,
					 WINDOW_HEIGHT);

  loadWallTextures();
}

void renderPlayer(void) {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_Rect playerRect = {player.x * MINIMAP_SCALE_FACTOR,
                         player.y * MINIMAP_SCALE_FACTOR,
                         player.width * MINIMAP_SCALE_FACTOR,
                         player.height * MINIMAP_SCALE_FACTOR};
  SDL_RenderFillRect(renderer, &playerRect);

  SDL_RenderDrawLine(
      renderer, player.x * MINIMAP_SCALE_FACTOR,
      player.y * MINIMAP_SCALE_FACTOR,
      MINIMAP_SCALE_FACTOR * player.x + cos(player.rotationAngle) * 40,
      MINIMAP_SCALE_FACTOR * player.y + sin(player.rotationAngle) * 40);
}

float normalizeAngle(float angle) {
  angle = remainder(angle, TWO_PI);
  if (angle < 0) {
    angle = TWO_PI + angle;
  }

  return angle;
}

float distanceBetweenPoints(float x1, float y1, float x2, float y2) {
  return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void castRay(float rayAngle, int stripId) {
  rayAngle = normalizeAngle(rayAngle);

  int isRayFacingDown = rayAngle > 0 && rayAngle < PI;
  int isRayFacingUp = !isRayFacingDown;
  int isRayFacingRight = rayAngle < (0.5 * PI) || rayAngle > (1.5 * PI);
  int isRayFacingLeft = !isRayFacingRight;

  float xintercept, yintercept;
  float xstep, ystep;

  ///////////////////////////////////////////
  // HORIZONTAL RAY-GRID INTERSECTION CODE
  ///////////////////////////////////////////
  bool foundHorzWallHit = false;
  float horzWallHitX = 0;
  float horzWallHitY = 0;
  int horzWallContent = 0;

  // Find the y-coordinate of the closest horizontal grid intersection
  yintercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
  yintercept += isRayFacingDown ? TILE_SIZE : 0;

  // Find the x-coordinate of the closest horizontal grid intersection
  xintercept = player.x + (yintercept - player.y) / tan(rayAngle);

  // Calculate the increment xstep and ystep
  ystep = TILE_SIZE;
  ystep *= isRayFacingUp ? -1 : 1;

  xstep = TILE_SIZE / tan(rayAngle);
  xstep *= (isRayFacingLeft && xstep > 0) ? -1 : 1;
  xstep *= (isRayFacingRight && xstep < 0) ? -1 : 1;

  float nextHorzTouchX = xintercept;
  float nextHorzTouchY = yintercept;

  // Increment xstep and ystep until we find a wall
  while (nextHorzTouchX >= 0 && nextHorzTouchX <= WINDOW_WIDTH &&
         nextHorzTouchY >= 0 && nextHorzTouchY <= WINDOW_HEIGHT) {
    float xToCheck = nextHorzTouchX;
    float yToCheck = nextHorzTouchY + (isRayFacingUp ? -1 : 0);

    if (mapHasWallAt(xToCheck, yToCheck)) {
      horzWallHitX = nextHorzTouchX;
      horzWallHitY = nextHorzTouchY;
      horzWallContent = map[(int)floor(yToCheck / TILE_SIZE)]
                           [(int)floor(xToCheck / TILE_SIZE)];
      foundHorzWallHit = true;
      break;
    } else {
      // I am gonna do something else
      nextHorzTouchX += xstep;
      nextHorzTouchY += ystep;
    }
  }

  ///////////////////////////////////////////
  // VERTICAL RAY-GRID INTERSECTION CODE
  ///////////////////////////////////////////
  bool foundVertWallHit = false;
  float vertWallHitX = 0;
  float vertWallHitY = 0;
  int vertWallContent = 0;

  // Find the x-coordinate of the closest vertical grid intersenction
  xintercept = floor(player.x / TILE_SIZE) * TILE_SIZE;
  xintercept += isRayFacingRight ? TILE_SIZE : 0;

  // Find the y-coordinate of the closest vertical grid intersection
  yintercept = player.y + (xintercept - player.x) * tan(rayAngle);

  // Calculate the increment xstep and ystep
  xstep = TILE_SIZE;
  xstep *= isRayFacingLeft ? -1 : 1;

  ystep = TILE_SIZE * tan(rayAngle);
  ystep *= (isRayFacingUp && ystep > 0) ? -1 : 1;
  ystep *= (isRayFacingDown && ystep < 0) ? -1 : 1;

  float nextVertTouchX = xintercept;
  float nextVertTouchY = yintercept;

  // Increment xstep and ystep until we find a wall
  while (nextVertTouchX >= 0 && nextVertTouchX <= WINDOW_WIDTH &&
         nextVertTouchY >= 0 && nextVertTouchY <= WINDOW_HEIGHT) {
    float xToCheck = nextVertTouchX + (isRayFacingLeft ? -1 : 0);
    float yToCheck = nextVertTouchY;

    if (mapHasWallAt(xToCheck, yToCheck)) {
      vertWallHitX = nextVertTouchX;
      vertWallHitY = nextVertTouchY;
      vertWallContent = map[(int)floor(yToCheck / TILE_SIZE)]
                           [(int)floor(xToCheck / TILE_SIZE)];
      foundVertWallHit = true;
      break;
    } else {
      // I am gonna do something else
      nextVertTouchX += xstep;
      nextVertTouchY += ystep;
    }
  }

  // Calculate both horizontal and vertical distances and choose the smallest
  // value
  float horzHitDistance =
      (foundHorzWallHit) ? distanceBetweenPoints(player.x, player.y,
                                                 horzWallHitX, horzWallHitY)
                         : FLT_MAX;
  float vertHitDistance =
      (foundVertWallHit) ? distanceBetweenPoints(player.x, player.y,
                                                 vertWallHitX, vertWallHitY)
                         : FLT_MAX;

  // only store the smallest of the distances
  if (vertHitDistance < horzHitDistance) {
    rays[stripId].distance = vertHitDistance;
    rays[stripId].wallHitX = vertWallHitX;
    rays[stripId].wallHitY = vertWallHitY;
    rays[stripId].wallHitContent = vertWallContent;
    rays[stripId].wasHitVertical = true;
    rays[stripId].rayAngle = rayAngle;
  } else {
    rays[stripId].distance = horzHitDistance;
    rays[stripId].wallHitX = horzWallHitX;
    rays[stripId].wallHitY = horzWallHitY;
    rays[stripId].wallHitContent = horzWallContent;
    rays[stripId].wasHitVertical = false;
    rays[stripId].rayAngle = rayAngle;
  }

}

void castAllRays(void) {
  // start first ray subtracting half FOV
  /* float rayAngle = player.rotationAngle - (FOV_ANGLE / 2); */

  for (int col = 0; col < NUM_RAYS; col++) {
    float rayAngle = player.rotationAngle + atan((col - NUM_RAYS / 2) / DIST_PROJ_PLANE);
    castRay(rayAngle, col);

    /* rayAngle += FOV_ANGLE / NUM_RAYS; */
  }
}

void renderMap(void) {
  for (int i = 0; i < MAP_NUM_ROWS; i++) {
    for (int j = 0; j < MAP_NUM_COLS; j++) {
      int tileX = j * TILE_SIZE;
      int tileY = i * TILE_SIZE;
      int tileColor = map[i][j] != 0 ? 255 : 0;

      SDL_SetRenderDrawColor(renderer, tileColor, tileColor, tileColor, 255);
      SDL_Rect mapTileRect = {
          tileX * MINIMAP_SCALE_FACTOR, tileY * MINIMAP_SCALE_FACTOR,
          TILE_SIZE * MINIMAP_SCALE_FACTOR, TILE_SIZE * MINIMAP_SCALE_FACTOR};
      SDL_RenderFillRect(renderer, &mapTileRect);
    }
  }
}

void renderRays(void) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  for (int i = 0; i < NUM_RAYS; i++) {
    SDL_RenderDrawLine(renderer, MINIMAP_SCALE_FACTOR * player.x,
                       MINIMAP_SCALE_FACTOR * player.y,
                       MINIMAP_SCALE_FACTOR * rays[i].wallHitX,
                       MINIMAP_SCALE_FACTOR * rays[i].wallHitY);
  }
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

void generate3DProjection(void) {
  for (int x = 0; x < NUM_RAYS; x++) {
    float perpDistance =
        rays[x].distance * cos(rays[x].rayAngle - player.rotationAngle);
    float projectedWallHeight = (TILE_SIZE / perpDistance) * DIST_PROJ_PLANE;

    int wallStripHeight = (int)projectedWallHeight;

    int wallTopPixel = (WINDOW_HEIGHT / 2) - (wallStripHeight / 2);
    wallTopPixel = wallTopPixel < 0 ? 0 : wallTopPixel;

    int wallBottomPixel = (WINDOW_HEIGHT / 2) + (wallStripHeight / 2);
    wallBottomPixel = wallBottomPixel > WINDOW_HEIGHT ? WINDOW_HEIGHT : wallBottomPixel;

    for (int y = 0; y < wallTopPixel; y++) {
      colorBuffer[WINDOW_WIDTH * y + x] = 0xFF333333;
    }

    int textureOffsetX;
    // calculate textureOffsetX
    if (rays[x].wasHitVertical) {
      textureOffsetX = (int)rays[x].wallHitY % TILE_SIZE;
    } else {
      textureOffsetX = (int)rays[x].wallHitX % TILE_SIZE;
    }

    // get the correct texture if number from the map content
    int texNum = rays[x].wallHitContent - 1;

    int texture_width = wallTextures[texNum].width;
    int texture_height = wallTextures[texNum].height;
    
    for (int y = wallTopPixel; y < wallBottomPixel; y++) {
      // calculate textureOffsetY
      int distanceFromTop = y + (wallStripHeight / 2) - (WINDOW_HEIGHT / 2);
      int textureOffsetY =
          distanceFromTop * ((float)texture_height / wallStripHeight);

      uint32_t texelColor = wallTextures[texNum].texture_buffer[texture_width * textureOffsetY + textureOffsetX];
      colorBuffer[WINDOW_WIDTH * y + x] = texelColor;
    }

    for (int y = wallBottomPixel; y < WINDOW_HEIGHT; y++) {
      colorBuffer[WINDOW_WIDTH * y + x] = 0xFF777777;
    }
  }
}

void clearColorBuffer(uint32_t color) {
  for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++) {
      colorBuffer[i] = color;
  }
}

void renderColorBuffer(void) {
  SDL_UpdateTexture(colorBufferTexture, NULL, colorBuffer,
                    (int)(uint32_t)WINDOW_WIDTH * sizeof(uint32_t));
  SDL_RenderCopy(renderer, colorBufferTexture, NULL, NULL);
}

void render(void) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  generate3DProjection();

  renderColorBuffer();
  clearColorBuffer(0xFF000000);

  // Display the minimap
  renderMap();
  renderRays();
  renderPlayer();

  SDL_RenderPresent(renderer);
}

int main(void) {
  isGameRunning = InitializeWindow();
  
  setup();

  while (isGameRunning) {
    processInput();
    update();
    render();
  }
  
  destroyWindow();

  return 0;
}
