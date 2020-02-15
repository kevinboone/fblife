/*==========================================================================

  fbclock 
  usage.c
  Copyright (c)2020 Kevin Boone
  Distributed under the terms of the GPL v3.0

==========================================================================*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "feature.h" 
#include "usage.h" 


/*==========================================================================
  usage_show
==========================================================================*/
void usage_show (FILE *fout, const char *argv0)
  {
  fprintf (fout, "Usage: %s [options]\n", argv0);
  fprintf (fout, "  -?,--help            show this message\n");
  fprintf (fout, "  -b,--border-colour=c border colour name or code (cyan)\n");
  fprintf (fout, "     --b-rule=NNN      cell birth rule (3)\n");
  fprintf (fout, "  -c,--colour=c        colour name or code (lime)\n");
  fprintf (fout, "  -e,--erase           clear framebuffer first\n");
  fprintf (fout, "  -f,--fbdev=device    framebuffer device (/dev/fb0)\n");
  fprintf (fout, "  -h,--height=N        height in cells (20)\n");
  fprintf (fout, "     --log-level=N     log level, 0-5 (default 2)\n");
  fprintf (fout, "  -i,--interval=N      msec between cycles (1000)\n");
  fprintf (fout, "  -m,--max-cycles=N    maximum number of cycles (60)\n");
  fprintf (fout, "  -p,--percent=N       initial percentage (30)\n");
  fprintf (fout, "  -s,--cell-size=N     cell size in pixels (20)  \n");
  fprintf (fout, "     --s-rule=NNN      cell survival rule (23)\n");
  fprintf (fout, "  -v,--version         show version\n");
  fprintf (fout, "  -w,--width=N         width in cells (20)\n");
  fprintf (fout, "  -x,--x=N             display x position (centre)\n");
  fprintf (fout, "  -y,--y=N             display y position (centre)\n");
  }

 
