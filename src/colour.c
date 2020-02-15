/*==========================================================================

  fblife
  colour.c
  Copyright (c)2020 Kevin Boone
  Distributed under the terms of the GPL v3.0

  This file contains functions for working with colours 

==========================================================================*/
#include <stddef.h> 
#include <stdint.h> 
#include <string.h> 
#include <ctype.h> 
#include "feature.h" 
#include "defs.h" 
#include "log.h" 
#include "colour.h"

typedef struct _Col
  {
  const char *name;
  BYTE r;
  BYTE g;
  BYTE b;
  } Col;

static Col colour_table[] = 
  {
  { "black",   0,   0,   0    },
  { "white",   255, 255, 255  },
  { "red",     255, 0,   0    },
  { "lime",    0,   255, 0    },
  { "blue",    0,   0,   255  },
  { "yellow",  255, 255, 0    },
  { "cyan",    0,   255, 255  },
  { "magenta", 255, 0,   255  },
  { "silver",  192, 192, 192  },
  { "maroon",  192, 0,   0    },
  { "olive",   128, 128, 0    },
  { "green",   0,   128, 0    },
  { "purple",  128, 0,   128  },
  { "teal",    0,   128, 128  },
  { "navy",    0,   0,   128 },
  { NULL,      0,   0,   0   }
  };

static int hd (char d)
  {
  if (d >= '0' && d <= '9') return d - '0';
  return toupper (d) - 'A' + 10;
  }

BOOL colour_parse (const char *colour_name, BYTE *r, BYTE *g, BYTE *b) 
  {
  LOG_IN
  BOOL ret = FALSE;

  if (colour_name[0] == '#' && strlen (colour_name) == 7)
    {
    *r = hd(colour_name[1]) * 16 + hd(colour_name[2]);
    *g = hd(colour_name[3]) * 16 + hd(colour_name[4]);
    *b = hd(colour_name[5]) * 16 + hd(colour_name[6]);
    ret = TRUE;
    }
  else
    {
    BOOL got = FALSE;
    int i = 0;
    Col *t = &colour_table[i];
    while (t->name)
      {
      if (strcmp (colour_name, t->name) == 0)
	{
	got = TRUE;
	*r = t->r; *g = t->g; *b = t->b;
	}
      i++;
      t = &colour_table[i];
      } 
    ret = got;
    }
  LOG_OUT
  return ret;
  }


