#include "wall.h"

void renderWallProjection(void) {
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
      drawPixel(x, y, 0xFF333333);
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
      drawPixel(x, y, texelColor);
    }

    for (int y = wallBottomPixel; y < WINDOW_HEIGHT; y++) {
      drawPixel(x, y, 0xFF777777);
    }
  }
}
