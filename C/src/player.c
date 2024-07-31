#include "player.h"

player_t player = {
    .x = WINDOW_WIDTH / 2,
    .y = WINDOW_HEIGHT / 2,
    .width = 1,
    .height = 1,
    .turnDirection = 0,
    .walkDirection = 0,
    .rotationAngle = PI / 2,
    .walkSpeed = 100,
    .turnSpeed = 60 * (PI / 180),
};

void movePlayer(float deltaTime) {
  player.rotationAngle += player.turnDirection * player.turnSpeed * deltaTime;
  normalizeAngle(&player.rotationAngle);
  
  float moveStep = player.walkDirection * player.walkSpeed * deltaTime;

  float newPlayerX = player.x + cos(player.rotationAngle) * moveStep;
  float newPlayerY = player.y + sin(player.rotationAngle) * moveStep;

  if (!mapHasWallAt(newPlayerX, newPlayerY)) {
    player.x = newPlayerX;
    player.y = newPlayerY;
  }
}

void renderMapPlayer(void) {
  /* SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); */
  /* SDL_Rect playerRect = {player.x * MINIMAP_SCALE_FACTOR, */
  /*                        player.y * MINIMAP_SCALE_FACTOR, */
  /*                        player.width * MINIMAP_SCALE_FACTOR, */
  /*                        player.height * MINIMAP_SCALE_FACTOR}; */
  /* SDL_RenderFillRect(renderer, &playerRect); */

  /* SDL_RenderDrawLine( */
  /*     renderer, player.x * MINIMAP_SCALE_FACTOR, */
  /*     player.y * MINIMAP_SCALE_FACTOR, */
  /*     MINIMAP_SCALE_FACTOR * player.x + cos(player.rotationAngle) * 40, */
  /*     MINIMAP_SCALE_FACTOR * player.y + sin(player.rotationAngle) * 40); */

  drawRect(player.x * MINIMAP_SCALE_FACTOR, player.y * MINIMAP_SCALE_FACTOR,
           player.width * MINIMAP_SCALE_FACTOR,
           player.height * MINIMAP_SCALE_FACTOR, 0xFFFFFFFF);
}
