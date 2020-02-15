# fblife

Version 1.0a

## What is this?

Yet another implementation of Conway's 'Game of Life' cellular
simulation, for use with the Linux framebuffer. This program
draws directly on the framebuffer, and is optimized for embedded
Linux systems like the Raspberry Pi. It doesn't require 
X or any desktop environment.
It has a large number of
configuration settings that can be used to tune the simulation.

## Building

The usual:

    $ make
    $ sudo make install 


## Command-line options

`--b-rule=digits`

Cell birth rule. See note 'Rules' below.

`-b`,`--border-color=color`

Sets the cell border to the specific colour. Colour can
be one of the basic HTML colour name:
black, white, red, lime, blue, yellow, cyan, magenta, silver, 
maroon, olive, green, purple, teal, navy;
or it can be a hexadecimal string of the form "#RRGGBB".

`-c`,`--color=color`

Cell body colour. See `--border-colour` for more details.

`-e`,`--erase`

Erase framebuffer (to black) before starting.

`-f`,`--fbdev=device`

Framebuffer device. Defaults to `/dev/fb0`.

`-h`,`--height=N`

Sets the height _in cells_ (not pixels) of the display. The
pixel size will be the height multiplied by the cell size.
Defaults to 20 cells.

--log-level=N`

Sets the logging verbosity from 0-5. Levels higher than 3
will probably only be comprehensible if read alongside the
source code.

`-i`,`--interval=N`

Time to wait between update cycles, in milliseconds Setting 
this to zero
can produce some very interesting displays on fast
harder, but will seriously tax the CPU. Default
value is 1000 msec. 

`-m`,`--max-cycles=N`

Maximum number of simulation cycles, before reseeding the cells.
Although the program will automatically reseed when the pattern
is static, it's possible to get into a repeated loop of
two or three patterns, which isn't very interesting. The program
can't detect this kind of thing automatically, so it's useful
to set a limit on the number of simulation cycles. 
The faster the simulation is running (small values of `--interval`)
the larger `--max-cycles` will need to be. The default is
60, which gives a one-minute run with the default interval of
one second.

`-p`,`--percent=N`

Percentage coverage with live cells when seeding the simulation.
The default is 30%; values less than 20% and greater the 70%
tend to result in very short-lived runs, at least with the
default rules.

`--s-rule=digits`

Cell survivorship rule. See note 'Rules' below.

`-w`,`--width=N`

Sets the width _in cells_ (not pixels) of the display. The
pixel size will be the width multiplied by the cell size.
Defaults to 20 cells.

`-s`,`--cell-size=pixels`

Size of a cell in pixels. Default is 20 pixels.

`-x`,`--x=X`

X position of the top-left corner of the display area, in pixels.
The default is to centre on the screen.

`-y`,`--y=Y`

R position of the top-left corner of the display area, in pixels.
The default is to centre on the screen.

## User and system RC files

All the command-line options can also be set in one or other
of the RC files: `/etc/fblife.rc` and `$HOME/.fblife.rc`.
The format is `key-value`, one entry on each line. Both
The keys correspond to the long form of the command line option. 

## Rules

The rule for cell lifetime in the original Game of Life
can be expressed as "B3/S23". That is, a cell is born into
an empty space if the space has (exactly) three live neighbours,
and survives if it has 2 or 3 neighbours. Otherwise, it dies.

There are many similar rules that can be used, although the
vast majority are uninteresting -- either the grid is filled,
or completely emptied, in a few cycles. 

`fblife` allows the birth rule and survivorship rule to be
set, using the `--b-rule` and `--s-rule` options. Each
option takes a string of digits, representing the number
of neighbouring cells. So, for example, `--b-rule 36`
means that a cell well be born into an space with 3 or 6
neighbours. This rule, combined with `--s-rule 23` produces
the B36/S23 "HighLife" behaviour, which can create interesting
behaviour, particularly with smaller initial coverage.   


## Notes

This program probably won't work under X, because the X server
will either disable the framebuffer, or compete for its 
attention. For testing purposes, it should work on a desktop 
system is you switch to a terminal console (e..g, ctrl+alt+F2).

To run `fblife` you'll need read/write access to the framebuffer. 
You can do this by running as `root` or as a user
which is in the same group as the `/dev/fbX` framebuffer
device.

`fblife` sends the control sequence to disable the flashing 
cursor to the current terminal, and re-enables it on exit.
This is only a helpful thing to do if you're running the utility
from an actual console, of course. If you're not, presumably
you will have other mechanisms to manage the cursor. 

Fun fact: `fblife` has nearly 7 000 lines of C, or which only
about 200 are for the actual simulation. Most of the rest are
for processing the command line and drawing on the framebuffer.


## Legal, etc

`fblife` is copyright (c)2020 Kevin Boone, and distributed under the
terms of the GNU Public Licence, version 3.0. Essentially that means
you may use the software however you wish, so long as the source
code continues to be made available, and the original author is
acknowledged. There is no warranty of any kind.

 
