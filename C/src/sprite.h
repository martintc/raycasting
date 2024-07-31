#ifndef SPRITE_H
#define SPRITE_H

#include <stdbool.h>

#include "defs.h"
#include "graphics.h"
#include "player.h"
#include "textures.h"
#include "math.h"

typedef struct {
  float x;
  float y;
  float distance;
  float angle;
  bool visible;
  int texture;
} sprite_t;

void renderSpriteProjection(void);
void renderMapSprites(void);

#endif
