#ifndef TEXTURES_C
#define TEXTURES_C
#include "textures.h"
#include <stdio.h>

const char *textureFileNames[NUM_TEXTURES] = {
    "./images/redbrick.png", "./images/purplestone.png",
    "./images/mossystone.png", "./images/graystone.png",
    "./images/colorstone.png", "./images/bluestone.png", "./images/wood.png",
    "./images/eagle.png", "./images/pikuma.png", "./images/barrel.png",
    "./images/light.png", "./images/table.png", "./images/guard.png", "./images/armor.png"
};

/* texture_t textures[NUM_TEXTURES]; */
upng_t* textures[NUM_TEXTURES];

void loadTextures(void) {
  for (int i = 0; i < NUM_TEXTURES; i++) {
    upng_t *upng;

    upng = upng_new_from_file(textureFileNames[i]);
    if (upng != NULL) {
      upng_decode(upng);
      if (upng_get_error(upng) == UPNG_EOK) {
	/* textures[i].upngTexture = upng; */
        /* textures[i].width = upng_get_width(upng); */
        /* textures[i].height = upng_get_height(upng); */
        /* textures[i].texture_buffer = (color_t *)upng_get_buffer(upng); */
	textures[i] = upng;
      } else {
        printf("Error decoding texture file %s\n", textureFileNames[i]);
      }
    } else {
      printf("Error loading texture %s\n", textureFileNames[i]);
    }
  }
}

void freeTextures(void) {
  for (int i = 0; i < NUM_TEXTURES; i++) {
    /* upng_free(textures[i].upngTexture); */
    upng_free(textures[i]);
  }
}

#endif
