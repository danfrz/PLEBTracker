# PLEBTracker
PLEBTracker is an ncurses based audio tracker program inspired by goattracker and milkytracker.
It produces noise by piping raw unsigned bytes into aplay.

PLEBTracker is something I wrote because I wanted to have some experience with audio,
and wanted to implement what I've been learning by using other trackers.

With this program, like other trackers, you can compose music (.plb modules) and listen to them.
All of the sounds produced in this tracker are synthesized, meaning there are no samples.

You can listen to some examples of (unfinished) songs written in this tracker in the examples/render directory.

The most efficient and effective way to perform actions in the interface, outside of the pattern editor is by using the vim-like command bar, which is shown by pressing ':'.
Those commands and other binds can be reviewed on the plebtrkraw(1) man page.
