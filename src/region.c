/*============================================================================

  fbclock 
  region.c
  Copyright (c)2020 Kevin Boone, GPL v3.0

============================================================================*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include "string.h" 
#include "defs.h" 
#include "log.h" 
#include "framebuffer.h" 
#include "region.h" 

// Bytes per pixel
#define BPP 3

struct _Region
  {
  int w;
  int h;
  BYTE *data;
  }; 

/*==========================================================================
  region_create
*==========================================================================*/
Region *region_create (int w, int h)
  {
  LOG_IN
  Region *self = malloc (sizeof (Region));
  self->w = w;
  self->h = h;
  self->data = malloc (w * h * BPP);
  LOG_OUT 
  return self;
  }

/*==========================================================================
  region_clone
*==========================================================================*/
Region *region_clone (const Region *other)
  {
  LOG_IN
  Region *self = region_create (other->w, other->h);

  int size = self->w * self->h * BPP;
  memcpy (self->data, other->data, size); 
 
  LOG_OUT
  return self;
  }

/*==========================================================================
  region_set_pixel
*==========================================================================*/
void region_set_pixel (Region *self, int x, int y, 
      BYTE r, BYTE g, BYTE b)
  {
  if (x >= 0 && x < self->w && y >= 0 && y < self->h)
    {
    int index24 = (y * self->w + x) * BPP;
    self->data [index24++] = b;
    self->data [index24++] = g;
    self->data [index24] = r;
    }
  }

/*==========================================================================
  region_set_pixel_t

  Set a pixel with a specific 'brightness'. This is just a helper to
  void reproducing all the per-channel brightness code repeatedly 
  elsewhere
*==========================================================================*/
void region_set_pixel_t (Region *self, int x, int y, 
      BYTE r, BYTE g, BYTE b, float t)
  {
  b = (BYTE) (t * (float)b);
  g = (BYTE) (t * (float)g);
  r = (BYTE) (t * (float)r);
  if (x > 0 && x < self->w && y > 0 && y < self->h)
    {
    int index24 = (y * self->w + x) * BPP;
    self->data [index24 + 0] = b;
    self->data [index24 + 1] = g;
    self->data [index24 + 2] = r;
    }
  }


/*==========================================================================
  region_fill_rect
  x2,y2 point is _excluded_
*==========================================================================*/
void region_fill_rect (Region *self, int x1, int y1,
      int x2, int y2, BYTE r, BYTE g, BYTE b)
  {
  LOG_IN
  if (x1 > x2) { int t = x1; x2 = x1; x1 = t; }
  if (y1 > y2) { int t = y1; y2 = y1; y1 = t; }
  for (int y = y1; y < y2; y++)
    {
    for (int x = x1; x < x2; x++)
      {
      region_set_pixel (self, x, y, r, g, b);
      }
    }
  LOG_OUT
  }

/*==========================================================================
  region_draw_rect
  x2,y2 point is _excluded_
*==========================================================================*/
void region_draw_rect (Region *self, int x1, int y1,
      int x2, int y2, BYTE r, BYTE g, BYTE b)
  {
  LOG_IN
  if (x1 > x2) { int t = x1; x2 = x1; x1 = t; }
  if (y1 > y2) { int t = y1; y2 = y1; y1 = t; }

  for (int x = x1; x < x2; x++)
    {
    region_set_pixel (self, x, y1, r, g, b);
    region_set_pixel (self, x, y2 - 1, r, g, b);
    }

  for (int y = y1; y < y2; y++)
    {
    region_set_pixel (self, x1, y, r, g, b);
    region_set_pixel (self, x2 - 1, y, r, g, b);
    }

  LOG_OUT
  }

/*==========================================================================
  region_destroy
*==========================================================================*/
void region_destroy (Region *self)
  {
  LOG_IN
  if (self)
    {
    if (self->data) free (self->data);
    free (self); 
    }
  LOG_OUT
  }


/*==========================================================================
  region_to_fb
*==========================================================================*/
void region_to_fb (const Region *self, FrameBuffer *fb, int x1, int y1)
  {
  LOG_IN
  int w_in = self->w;
  int h_in = self->h;

  // We can do the copy to FB either in this method, or we can
  //   just let the framebuffer do the math. If the FB is linear
  //   we do it here, directly on the FB's mapped data, because
  //   this is much faster. But if the FB is non-linear, leave
  //   the FB to do it, rather than duplicating all the fiddly 
  //   logic in this class  

  if (!framebuffer_is_linear (fb))
    {
    for (int y = 0; y < h_in; y++)
      {
      int xp = x1;
      int linestart24 = y * w_in;
      for (int x = 0; x < w_in; x++)
	{
	int index24 = (linestart24 + x) * BPP;
	BYTE b = self->data [index24++];
	BYTE g = self->data [index24++];
	BYTE r = self->data [index24];

	framebuffer_set_pixel (fb, x + x1, y + y1, 
	  r, g, b);

	xp++;
	}
      }
    }
  else
    {
    BYTE *data = framebuffer_get_data (fb);
    int w_out = framebuffer_get_width (fb);
    for (int y = 0; y < h_in; y++)
      {
      int xp = x1;
      int linestart24 = y * w_in;
      int linestart32 = (y + y1) * w_out;
      for (int x = 0; x < w_in; x++)
	{
	int index24 = (linestart24 + x) * BPP;
	int index32 = (linestart32 + x + x1) * 4;
	BYTE b = self->data [index24++];
	BYTE g = self->data [index24++];
	BYTE r = self->data [index24];
	data [index32] = b;
	data [index32+1] = g;
	data [index32+2] = r;
	xp++;
	}
      }
    }
  LOG_OUT
  }


/*==========================================================================

  region_from_fb

  The region should already be intialized, and have the desired
  sizes

*==========================================================================*/
void region_from_fb (Region *self, const FrameBuffer *fb, int x1, int y1)
  {
  LOG_IN
  int w_in = self->w;
  int h_in = self->h;
  for (int y = 0; y < h_in; y++)
    {
    int yp = y + y1;
    int xp = x1;
    int linestart = y * w_in;
    for (int x = 0; x < w_in; x++)
      {
      BYTE r, g, b;
      framebuffer_get_pixel (fb, xp, yp, &r, &g, &b);
      int index24 = (linestart + x) * BPP;
      self->data [index24++] = b;
      self->data [index24++] = g;
      self->data [index24] = r;
      xp++;
      }
    }
  LOG_OUT
  }

/*==========================================================================

  region_darken

  Darken to the specified percentage of original value

*==========================================================================*/
void region_darken (Region *self, int percent)
  {
  LOG_IN
  int l = self->w * self->h * BPP;
  for (int i = 0; i < l; i++)
    {
    self->data[i] = self->data[i] * percent / 100;
    }
  LOG_OUT
  }

/*==========================================================================
  region_get_width
*==========================================================================*/
int region_get_width (const Region *self)
  {
  return self->w;
  }

/*==========================================================================
  region_get_height
*==========================================================================*/
int region_get_height (const Region *self)
  {
  return self->h;
  }


