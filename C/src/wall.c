#include "wall.h"

void changeColorIntensity(color_t *color, float factor) {
  color_t a = (*color & 0xFF000000);
  color_t r = (*color & 0x00FF0000) * factor;
  color_t g = (*color & 0x0000FF00) * factor;
  color_t b = (*color & 0x000000FF) * factor;

  *color = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);
}

void renderWallProjection(void) {
  for (int x = 0; x < NUM_RAYS; x++) {
    // calc perp dist to avoid fish-eye distortion
    float perpDistance =
        rays[x].distance * cos(rays[x].rayAngle - player.rotationAngle);
    // calc the projected wall height
    float wallHeight = (TILE_SIZE / perpDistance) * DIST_PROJ_PLANE;

    int wallTopY = (WINDOW_HEIGHT / 2) - (wallHeight / 2);
    wallTopY = wallTopY < 0 ? 0 : wallTopY;

    int wallBottomY = (WINDOW_HEIGHT / 2) + (wallHeight / 2);
    wallBottomY = wallBottomY > WINDOW_HEIGHT ? WINDOW_HEIGHT : wallBottomY;

    // draw ceiling
    for (int y = 0; y < wallTopY; y++) {
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

    int texture_width = upng_get_width(textures[texNum]);
    int texture_height = upng_get_height(textures[texNum]);

    // draw wall
    for (int y = wallTopY; y < wallBottomY; y++) {
      // calculate textureOffsetY
      int distanceFromTop = y + (wallHeight / 2) - (WINDOW_HEIGHT / 2);
      int textureOffsetY =
          distanceFromTop * ((float)texture_height / wallHeight);

      /* color_t texelColor = */
      /*     textures[texNum] */
      /*         .texture_buffer[texture_width * textureOffsetY +
       * textureOffsetX]; */

      color_t* wallTextureBuffer = (color_t*) upng_get_buffer(textures[texNum]);
      color_t texelColor = wallTextureBuffer[(texture_width * textureOffsetY) + textureOffsetX];

      if (rays[x].wasHitVertical) {
	changeColorIntensity(&texelColor, 0.7);
      }
      
      drawPixel(x, y, texelColor);
    }

    // draw floor
    for (int y = wallBottomY; y < WINDOW_HEIGHT; y++) {
      drawPixel(x, y, 0xFF777777);
    }
  }
}
