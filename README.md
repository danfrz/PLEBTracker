# PLEBTracker
PLEBTracker is an ncurses based audio tracker program inspired by goattracker and milkytracker.
![ScreenShot](https://github.com/danfrz/PLEBTracker/blob/master/docs/screenshot/plebtracker.png)

PLEBTracker is something I wrote because I wanted to have some experience with audio,
and wanted to implement what I've been learning by using other trackers.

With this program, like other trackers, you can compose music (.plb modules) and listen to them.
All of the sounds produced in this tracker are synthesized, meaning there are no samples.

To install, git pull this repo, cd to ./Tracker/src and ./Interpreter/src and run make, then the install bashscripts.
To compile, you must have the ncurses library installed.
In order to have playback you must have inotifywait (on Archlinux included in package inotify-tools) and aplay (alsa-utils.)

You can listen to some examples of (unfinished) songs written in this tracker in the examples/render directory.

The most efficient and effective way to perform actions in the interface, outside of the pattern editor is by using the vim-like command bar, which is shown by pressing ':'.
Those commands and other binds can be reviewed on the plebtrkraw(1) man page.

If you're interested, look through the docs folder, the manual provides a good introduction to the tracker (I hope) and wavetable.svg details what synths/wave generators have been implemented so far.
