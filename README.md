# PLEBTracker
PLEBTracker is an console based audio tracker program inspired by goattracker and milkytracker.
![ScreenShot](https://github.com/danfrz/PLEBTracker/blob/master/docs/images/plebtracker.png)

PLEBTracker is something I wrote because I wanted to have some experience with audio,
and wanted to implement what I've been learning by using other trackers and studying music theory with the help of my piano teacher.

With this program, like other trackers, you can compose music (.plb modules) and listen to them.
All of the sounds produced in this tracker are synthesized, meaning there are no samples.

##Requirements
   ncurses     (ncurses)
   aplay       (alsa-utils)
   inotifywait (inotify-tools)

##Installation
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


##Usage
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
Most importantly, see the plebitp and plebtrkraw manual pages.

The most efficient and effective way to perform actions in the tracker's interface is by using the vim-like command bar, which is shown by pressing ':'.
Those commands and other binds can be reviewed on the plebtrkraw(1) man page.

If you're interested, look through the docs folder, the manual provides a good introduction to the tracker (I hope) and wavetable.pdf details what synths/wave generators have been implemented so far.
