/*============================================================================

  fblife
  life.h
  Copyright (c)2020 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

#include "defs.h"

struct _Life;
typedef struct _Life Life;

BEGIN_DECLS
Life        *life_create (int w, int h, const char *B, const char *S);
void        life_destroy (Life *self);
int         life_get_width (const Life *self);
int         life_get_height (const Life *self);
int         life_get_state (const Life *self, int col, int row);
BOOL        life_update (Life *self);
void        life_seed (Life *self, int percent);
END_DECLS



