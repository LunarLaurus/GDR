# Goblin Dice Rollaz

Goblin Dice Rollaz is a Doom engine game featuring dice-themed weapons and dwarf enemies.

Based on Chocolate Doom, which aims to accurately reproduce the original DOS version of
Doom and other games based on the Doom engine in a form that can be
run on modern computers.

Goblin Dice Rollaz's aims are:

 * To always be 100% Free and Open Source software.
 * Dice-themed gameplay with critical hits and percentile-based damage
 * Dwarf enemies replacing classic Doom monsters
 * Portability to as many different operating systems as possible.
 * Accurate reproduction of the original DOS versions of the games.

More information about the philosophy and design behind Chocolate Doom
can be found in the PHILOSOPHY file distributed with the source code.

## Setting up gameplay

For instructions on how to set up Goblin Dice Rollaz for play, see the
INSTALL file.

## Configuration File

Goblin Dice Rollaz is compatible with the DOS Doom configuration file
(normally named `default.cfg`). Existing configuration files for DOS
Doom should therefore simply work out of the box. However, Goblin
Dice Rollaz also provides some extra settings. These are stored in a
separate file named `goblin-doom.cfg`.

The configuration can be edited using the goblin-setup tool.

## Command line options

Goblin Dice Rollaz supports a number of command line parameters, including
some extras that were not originally suported by the DOS versions. For
binary distributions, see the CMDLINE file included with your
download; more information is also available on the Chocolate Doom
website.

## Playing TCs

With Vanilla Doom there is no way to include sprites in PWAD files.
Goblin Dice Rollaz's '-file' command line option behaves exactly the same
as Vanilla Doom, and trying to play TCs by adding the WAD files using
'-file' will not work.

Many Total Conversions (TCs) are distributed as a PWAD file which must
be merged into the main IWAD. Typically a copy of DEUSF.EXE is
included which performs this merge. Goblin Dice Rollaz includes a new
option, '-merge', which will simulate this merge. Essentially, the
WAD directory is merged in memory, removing the need to modify the
IWAD on disk.

To play TCs using Goblin Dice Rollaz, run like this:

```
goblin-doom -merge thetc.wad
```

Here are some examples:

```
goblin-doom -merge batman.wad -deh batman.deh vbatman.deh  (Batman Doom)
goblin-doom -merge aoddoom1.wad -deh aoddoom1.deh  (Army of Darkness Doom)
```

## Other information

 * Goblin Dice Rollaz includes a number of different options for music
   playback. See the README.Music file for more details.

 * More information, including information about how to play various
   classic TCs, is available on the Goblin Dice Rollaz wiki:

      https://github.com/chocolate-doom/chocolate-doom/wiki

 * Goblin Dice Rollaz is not perfect. Although it aims to accurately
   emulate and reproduce the DOS executables, some behavior can be very
   difficult to reproduce. Because of the nature of the project, you
   may also encounter Vanilla Doom bugs; these are intentionally
   present; see the NOT-BUGS file for more information.

 * Goblin Dice Rollaz is distributed under the GNU GPL. See the COPYING
   file for more information.
