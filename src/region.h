/*============================================================================

  fblife
  region.h
  Copyright (c)2020 Kevin Boone, GPL v3.0

============================================================================*/

#pragma once

#include "defs.h"
#include "framebuffer.h"

struct _Region;
typedef struct _Region Region;

BEGIN_DECLS

Region     *region_create (int w, int h);
void        region_set_pixel (Region *self, int x, int y, 
               BYTE r, BYTE g, BYTE b);
void        region_fill_rect (Region *self, int x1, int y1,
               int x2, int y2, BYTE r, BYTE g, BYTE b);
void        region_draw_rect (Region *self, int x1, int y1,
               int x2, int y2, BYTE r, BYTE g, BYTE b);
void        region_destroy (Region *self);
void        region_to_fb (const Region *r, FrameBuffer *fb, int x, int y);
void        region_from_fb (Region *self, const FrameBuffer *fb, int x, int y);
void        region_darken (Region *self, int percent);
Region     *region_clone (const Region *other);
int         region_get_height (const Region *self);
int         region_get_width (const Region *self);
END_DECLS


