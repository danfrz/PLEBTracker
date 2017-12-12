# PLEBTracker
PLEBTracker is a linux console based audio tracker program inspired by goattracker and milkytracker.
![ScreenShot](https://github.com/danfrz/PLEBTracker/blob/master/docs/images/PLEBTracker_MainScreen.png)

*Pattern editor, main song editor window*

On the right are the *Wave*, *Pulse*, and *Filter tables*. (described in *docs/manual.txt*). The sound an instrument makes is determined by the indicies it points to in these tables.
* The **WAVE** table determines the waveform used for the instrument. 
* The **PULSE** table determines *pulse width modulation* (described in *docs/wavetable.pdf* and *docs/manual.txt*) of waveforms that use pulse width modulation.
* The **FILTER** table allows tracks to filter out certain frequencies. How this works is not described in documentation yet. (I'll get to it soon!)

![ScreenShot](https://github.com/danfrz/PLEBTracker/blob/master/docs/images/PLEBTracker_InstrumentEditor.png)

*Instrument Editor, with volume envelope visualization. The Volume table on the left determines the shape of the envelope*

PLEBTracker is something I wrote because I wanted to have some experience with audio,
and wanted to implement what I've been learning by using other trackers and studying music theory with the help of my piano teacher.

With this program, like other trackers, you can compose music (.plb modules) and listen to them.
All of the sounds produced in this tracker are synthesized, meaning there are no samples.

## Requirements
In order to run and build PLEBTracker the following programs and libraries are required.
   program/library... (package...   )


On Arch and Debian (at least) these packages are sufficient to get it working
* ncurses     (**ncurses**)
* fftw3        (**fftw**)
* aplay       (**alsa-utils**) 
* inotifywait (**inotify-tools**)
* sudo        (**sudo**)

Some distributions may not have these exact package names but
these names should work for at least Archlinux and Debian.

Right now, to get this working on **ubuntu 16.04** (and probably variants?) you need
* inotifywait (**inotify-tools**)
* ncurses (**libncursesw5** and **libncursesw5-dev**)
* fftw3 (**libfftw3** and **libfftw3-dev**)

(sudo and aplay are already installed, or were when I tested them)

Unfortunately, when **libncursesw5** is installed on ubuntu, the headers are placed within */usr/include/ncursesw/*, 
therefore each of the header files within *Tracker/include/* and *Tracker/src/main.cpp* need to be modified so that

> #include <ncurses.h>

becomes

> #include <ncursesw/ncurses.h>

and in the *Tracker/src/Makefile* change the LIBS line so that

> LIBS=-lncurses

becomes

> LIBS=-lncursesw

Sorry about this badness, hopefully in a future (or current?) version of Ubuntu **get_wch** will be included within regular (*ncurses.h*)


## Installation
Clone this project from git
```
git clone https://github.com/danfrz/PLEBTracker
```
cd to ./Tracker/src then as your user (not root) run
```
make ; ./INSTALL.sh;
```
cd to ./Interpreter/src 
```
make ; ./INSTALL.sh;
```


## Usage
To open a song within the tracker, the easiest way is from the commandline:
```
   plebtrk song.plb
```

To simply play a song without openning the tracker run:
```
   plebplay song.plb [OPTIONS...]
```

To render a song directly to flac run:
```
   plebrender song.plb [OPTIONS...]
```


And probably not a very popular option but to have the raw byte data go directly to stdout instead of aplay run:
```
   plebitp song.plb [OPTIONS...] 2>/dev/null
```


Example songs can be found in ./examples and their respective flac renderings in ./examples/render.

All of the programs that are installed have manual pages with important information on using the tracker.
Most importantly, see the plebtrk manual page.

The most efficient and effective way to perform actions in the tracker's interface is by using the vim-like command bar, which is shown by pressing ':'.
Those commands and other binds can be reviewed on the plebtrk man page.

If you're interested, look through the docs folder, the manual provides a good introduction to the tracker (I hope) and wavetable.pdf details what synths/wave generators have been implemented so far. patternedtr.pdf has information on the effects that are available in the pattern editor as well as going over some of its' quirks.
