#ifndef TEXTURES_H
#define TEXTURES_H

#include <stdint.h>
#include "defs.h"
#include "upng.h"

/* typedef struct { */
/*   int width; */
/*   int height; */
/*   color_t *texture_buffer; */
/*   upng_t* upngTexture; */
/* } texture_t; */

extern const char *textureFileNames[NUM_TEXTURES];

/* extern texture_t textures[NUM_TEXTURES]; */

extern upng_t* textures[NUM_TEXTURES];

void loadTextures(void);

void freeTextures(void);

#endif
