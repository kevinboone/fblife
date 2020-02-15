/*============================================================================

  fblife
  life.c
  Copyright (c)2020 Kevin Boone, GPL v3.0

  Life is a class for carrying out the evolution of a game-of-life
  simulation.

============================================================================*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include "defs.h" 
#include "log.h" 
#include "life.h" 


struct _Life
  {
  int w; // Width
  int h; // Height
  char *B; // Birth rule
  char *S; // Survival rule
  BYTE *cells;
  }; 


/*==========================================================================
  life_create
*==========================================================================*/
Life *life_create (int w, int h, const char *B, const char *S)
  {
  LOG_IN
  Life *self = malloc (sizeof (Life));
  self->w = w;
  self->h = h;
  self->cells = malloc (w * h * sizeof (BYTE));
  memset (self->cells, 0, w * h * sizeof (BYTE)); 
  self->B = strdup (B);
  self->S = strdup (S);
  LOG_OUT
  return self;
  }

/*==========================================================================

  life_set_cell

  set a specific cell alive or dead  

*==========================================================================*/
void life_set_cell (Life *self, int x, int y, BOOL alive)
  {
  self->cells [y *self->w + x] = alive;
  }


/*==========================================================================

  life_seed

  randomly set the cells alive or dead, according to some supplied
  percentage occupancy

*==========================================================================*/
void life_seed (Life *self, int percent)
  {
  for (int i = 0; i < self->w * self->h; i++)
    self->cells[i] = (rand() * 100.0 / RAND_MAX < percent ? 8 : 0);
  //  self->cells[i] = 0;
  
/*
  life_set_cell (self, 10, 10, 1);
  life_set_cell (self, 11, 10, 1);
  life_set_cell (self, 8, 9, 1);
  life_set_cell (self, 9, 9, 1);
  life_set_cell (self, 11, 9, 1);
  life_set_cell (self, 12, 9, 1);
  life_set_cell (self, 11, 8, 1);
  life_set_cell (self, 10, 8, 1);
  life_set_cell (self, 9, 8, 1);
  life_set_cell (self, 8, 8, 1);
  life_set_cell (self, 9, 7, 1);
  life_set_cell (self, 10, 7, 1);
 */ 
  }


/*==========================================================================
  life_destroy
*==========================================================================*/
void  life_destroy (Life *self)
  {
  LOG_IN
  if (self)
    {
    if (self->cells) free (self->cells);
    if (self->B) free (self->B);
    if (self->S) free (self->S);
    free (self);
    }
  LOG_OUT
  }


/*==========================================================================
  life_get_width
*==========================================================================*/
int life_get_width (const Life *self)
  {
  LOG_IN
  int ret = self->w;
  LOG_OUT
  return ret;
  }


/*==========================================================================
  life_get_height
*==========================================================================*/
int life_get_height (const Life *self)
  {
  LOG_IN
  int ret = self->h;
  LOG_OUT
  return ret;
  }

/*==========================================================================
  life_get_state

  In most case, the state can be taken to be a boolean value, indicating
  whether the cell is alive or dead. However, the update() method 
  actually sets the state to a number between 0 and 8, indicating the
  number of live neighbours. This may be useful for display purposes,
  although it does not affect the algorithm.

*==========================================================================*/
int  life_get_state (const Life *self, int col, int row)
  {
  LOG_IN
  BOOL state = self->cells [row * self->w + col];
  LOG_OUT
  return state;
  }


/*==========================================================================

  life_get_live_neighbours

  Get the number of live neighbouring cells -- a number between 0 and
  8. The cell itself is not counted.

*==========================================================================*/
int life_get_live_neighbours (const Life *self, int row, int col)
  {
  LOG_IN
  int ret = 0; 
  int h = self->h;
  int w = self->w;
   
  for (int r = row - 1; r <= row + 1; r++)
    {
    int _r = r;
    if (_r >= h) _r = 0;
    if (_r < 0) _r = h - 1;
    for (int c = col - 1; c <= col + 1; c++)
      {
      int _c = c;
      if (_c >= w) _c = 0;
      if (_c < 0) _c = w - 1;
      if (_c >= 0 && _c < w && _r >= 0 && _r < h 
           && (!(_r == row && _c == col)))
        {
        if (self->cells [_r * self->w + _c]) ret++;
        }
      }
    } 
 
  LOG_OUT;
  return ret;
  }

/*==========================================================================

  life_new_cell_state

  Given that a current cell is alive or dead, work out whether it
  should change to being alive or dead, given how many neighbouring
  cells are alive

*==========================================================================*/
BOOL life_new_cell_state (const Life *self, BOOL alive, int neighbours,
    const char *B, const char *S)
  {
  LOG_IN
  BOOL new = FALSE;

  if (strchr (B, neighbours + '0')) new = TRUE;
  else if (alive && strchr (S, neighbours + '0')) new = TRUE;

  LOG_OUT
  return new;
  }

/*==========================================================================

  life_update

  Update the entire grid to new cell states, based on existing
  cell states.

*==========================================================================*/
BOOL life_update (Life *self)
  {
  LOG_IN
  BOOL ret = TRUE;
  BOOL at_least_one = FALSE;
  char *B = self->B; 
  char *S = self->S; 

  
  // Note -- we must write the results int a new array,
  //  and then copy it to self->cells. Otherwise, the
  //  calculation is biased because of the scan
  //  direction
  BYTE *new_cells = malloc (self->w * self->h * sizeof (BOOL));
  for (int row = 0; row < self->h; row++)
    {
    int stride = row * self->w;
    for (int col = 0; col < self->w; col++)
      {
      int n = life_get_live_neighbours (self, row, col);
      BOOL current = (self->cells [stride + col] != 0);
      BOOL new = life_new_cell_state (self, current, n, B, S); 
      if (new)
        {
        new_cells [stride + col] = n;
        at_least_one = TRUE;
        }
      else
        new_cells [stride + col] = 0;
      }
    }

  if (memcmp (self->cells, new_cells, 
       self->w * self->h * sizeof (BYTE)) == 0) 
    {
    log_debug ("Update did not change state -- pattern is stable");
      ret = FALSE;
    }

  if (!at_least_one)
    {
    log_debug ("All cells dead -- pattern is stable");
      ret = FALSE;
    }

  memcpy (self->cells, new_cells, self->w * self->h * sizeof (BYTE));
  free (new_cells);
  LOG_OUT
  return ret;
  }


