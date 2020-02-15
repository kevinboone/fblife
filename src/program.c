/*==========================================================================

  fblife
  program.c
  Copyright (c)2020 Kevin Boone
  Distributed under the terms of the GPL v3.0

  This file contains the main body of the program.

==========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <signal.h>
#include <math.h>
#include "program_context.h" 
#include "feature.h" 
#include "program.h" 
#include "string.h" 
#include "file.h" 
#include "list.h" 
#include "numberformat.h" 
#include "framebuffer.h"
#include "life.h"
#include "region.h"
#include "colour.h"

/* Defaults for command-line arguments */
#define DEF_WIDTH 20 
#define DEF_HEIGHT 20 
#define DEF_CELL_SIZE 20 
#define DEF_X -1
#define DEF_Y -1
#define DEF_TRANSPARENCY 50
#define DEF_PERCENT 30
#define DEF_MAX_CYCLES 60
#define DEF_INTERVAL 1000
#define DEF_COLOUR "green"
#define DEF_BORDER_COLOUR "cyan"
#define DEF_B_RULE "3"
#define DEF_S_RULE "23"

/*==========================================================================

  program_check_context

==========================================================================*/
BOOL program_check_context (const ProgramContext *context, 
      const FrameBuffer *fb)
  {
  LOG_IN
  BOOL ret = TRUE;
  int fb_w = framebuffer_get_width (fb);
  int fb_h = framebuffer_get_height (fb);
  int width = program_context_get_integer (context, "width", DEF_WIDTH);
  int height = program_context_get_integer (context, "height", DEF_HEIGHT);
  int cell_size = program_context_get_integer (context, "cell-size", 
           DEF_CELL_SIZE);
  int region_width = width * cell_size;
  int region_height = height * cell_size;
  int x = program_context_get_integer (context, "x", DEF_X);
  int y = program_context_get_integer (context, "y", DEF_Y);
  const char *colour = program_context_get (context, "colour");
  if (colour == NULL) colour = DEF_COLOUR;
  const char *border_colour = program_context_get (context, "border-colour");
  if (border_colour == NULL) border_colour = DEF_BORDER_COLOUR;

  if (x < 0)
    x = (framebuffer_get_width (fb) - region_width) / 2; 
  if (y < 0)
    y = (framebuffer_get_height (fb) - region_height) / 2; 
  if (ret)
    {
    BYTE r, g, b;
    if (colour_parse (colour, &r, &g, &b))
      {
      }
    else
      {
      log_error ("Couldn't parse colour: %s", colour);
      ret = FALSE;
      }
    }
  if (ret)
    {
    BYTE r, g, b;
    if (colour_parse (border_colour, &r, &g, &b))
      {
      }
    else
      {
      log_error ("Couldn't parse colour: %s", border_colour);
      ret = FALSE;
      }
    }
  if (ret)
    {
    if (region_width > fb_w)
      {
      log_error ("Display region width (%d) is wider than the screen (%d)",
            region_width, fb_w);
      ret = FALSE;
      }
    }
  if (ret)
    {
    if (region_height > fb_h)
      {
      log_error ("Display region height (%d) is taller than the screen (%d)",
            region_width, fb_w);
      ret = FALSE;
      }
    }
  if (ret)
    {
    if (region_width + x > fb_w || region_height + y > fb_h
         || x < 0 || y < 0)
      {
      log_error ("Display region lies partly outside the screen",
            region_width, fb_w);
      ret = FALSE;
      }
    }

  LOG_OUT
  return ret;
  }

/*======================================================================
  program_quit_signal 
  In response to a quit, or interrupt, we must unexport any pins that
  we exported
======================================================================*/
void program_quit_signal (int dummy)
  {
  // Show the cursor
  fputs ("\e[?25h", stdout); 
  fflush (stdout);
  exit (0);
  }


/*==========================================================================

  erase_region_background 

==========================================================================*/
void erase_region_background (Region *region)
  {
  region_fill_rect (region, 0, 0, region_get_width (region),
    region_get_height (region), 0, 0, 0);
  }


/*==========================================================================

  draw_life_on_region 

==========================================================================*/
void draw_life_on_region (Region *region, const Life *life, int cell_size,
       BYTE red, BYTE green, BYTE blue,  
       BYTE red_border, BYTE green_border, BYTE blue_border)
  {
  LOG_IN
  int w = life_get_width (life);
  int h = life_get_height (life);

  erase_region_background (region);

  for (int row = 0; row < h; row++)
    {
    int y = row * cell_size;
    for (int col = 0; col < w; col++)
      {
      int x = col * cell_size;

      int state = life_get_state (life, col, row);
      if (state)
        {
        region_draw_rect (region, x, y, x + cell_size - 2, 
          y + cell_size - 2, red_border, green_border, blue_border); 
        region_fill_rect (region, x + 1, y + 1, x + cell_size - 3, 
          y + cell_size - 3, red, green, blue); 
        }
      }
    }

  LOG_IN
  }

/*==========================================================================

  program_run

==========================================================================*/
int program_run (ProgramContext *context)
  {
  log_set_level (program_context_get_integer (context, "log-level", 
      LOG_WARNING));
  const char *fbdev = "/dev/fb0";
  const char *arg_fbdev = program_context_get (context, "fbdev");
  if (arg_fbdev) fbdev = arg_fbdev;

  FrameBuffer *fb = framebuffer_create (fbdev);
  char *error = NULL;
  framebuffer_init (fb, &error);
  if (error == NULL)
    {
    if (program_check_context (context, fb))
      {
      BOOL erase = program_context_get_boolean (context, "erase", FALSE);
      int width = program_context_get_integer (context, "width", DEF_WIDTH);
      int height = program_context_get_integer (context, "height", DEF_HEIGHT);
      int cell_size = program_context_get_integer (context, "cell-size", 
           DEF_CELL_SIZE);
      int region_width = width * cell_size;
      int region_height = height * cell_size;
      int x = program_context_get_integer (context, "x", DEF_X);
      int y = program_context_get_integer (context, "y", DEF_Y);
      int percent = program_context_get_integer 
            (context, "percent", DEF_PERCENT);
      int max_cycles = program_context_get_integer 
            (context, "max-cycles", DEF_MAX_CYCLES);
      int interval = program_context_get_integer 
            (context, "interval", DEF_INTERVAL);
      int usecs = interval * 1000;
      const char *colour = program_context_get (context, "colour");
      if (colour == NULL) colour = DEF_COLOUR;
      BYTE r, g, b;
      colour_parse (colour, &r, &g, &b);
      const char *b_rule = program_context_get (context, "b-rule");
      if (b_rule == NULL) b_rule = DEF_B_RULE;
      const char *s_rule = program_context_get (context, "s-rule");
      if (s_rule == NULL) s_rule = DEF_S_RULE;

      const char *border_colour = program_context_get 
         (context, "border-colour");
      if (border_colour == NULL) border_colour = DEF_BORDER_COLOUR;
      BYTE rb, gb, bb;
      colour_parse (border_colour, &rb, &gb, &bb);

      if (x < 0)
        x = (framebuffer_get_width (fb) - region_width) / 2; 
      if (y < 0)
        y = (framebuffer_get_height (fb) - region_height) / 2; 

      signal (SIGQUIT, program_quit_signal);
      signal (SIGTERM, program_quit_signal);
      signal (SIGHUP, program_quit_signal);
      signal (SIGINT, program_quit_signal);
      srand (time (NULL));
      
      // Hide cursor
      fputs("\e[?25l", stdout);
      fflush (stdout);

      log_debug ("Display region is %d x %d", region_width, region_height);
      log_debug ("TL corner is %d x %d", x, y); 
      log_debug ("Percent coverage is %d", percent); 
      log_debug ("Maximum cycles is %d", max_cycles); 

      Life *life = life_create (width, height, b_rule, s_rule);
      life_seed (life, percent); 

      if (erase) framebuffer_clear (fb);

      Region *region = region_create (region_width, region_height);

      int cycle = 1;
      while (TRUE)
        {
        log_debug ("Starting cycle %d", cycle); 
        draw_life_on_region (region, life, cell_size, r, g, b,
           rb, gb, bb);
        region_to_fb (region, fb, x, y); 
        usleep (usecs); 
        if (cycle >= max_cycles)
          {
          log_debug ("Restarting with new seed");
          life_seed (life, percent); 
          cycle = 0;
          }
         else
          {
          BOOL viable = life_update (life);
          if (!viable)
            {
            log_debug ("Restarting with new seed");
            life_seed (life, percent); 
            cycle = 0;
            }
          }
        cycle++;
        }

      life_destroy (life);
      region_destroy (region);
      // Show the cursor
      printf("\e[?25h"); 
      fflush (stdout);
      }
    else
      {
      // Do nothing -- error already reported
      }
    framebuffer_deinit (fb);
    framebuffer_destroy (fb);
    
    }
  else
    {
    log_error (error);
    free (error);
    }

  return 0;
  }

