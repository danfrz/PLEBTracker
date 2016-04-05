#!/bin/bash

mkdir /var/tmp/plebtrk 2>/dev/null

echo "Dependencies include:"
echo "   ncurses"     #trk
echo "   aplay"       #itp
echo "   inotifywait" #trkdaemon
echo ""

touch /var/tmp/plebtrk/itplog.log
touch /var/tmp/plebtrk/trklog.log

echo "Copying executables into /usr/bin/..."
sudo cp ./plebtrk /usr/bin/plebtrk
echo "/usr/bin/plebtrk"
sudo cp ./plebtrkdaemon /usr/bin/plebtrkdaemon
echo "/usr/bin/plebtrkdaemon"
sudo cp ./plebtrkraw /usr/bin/plebtrkraw
echo "/usr/bin/plebtrkraw"
echo ""
echo "Copying man pages into /usr/local/man/man1/..."
sudo cp ../doc/plebtrk.1 /usr/local/man/man1/
echo "/usr/local/man/man1/plebtrk.1"
sudo cp ../doc/plebtrkdaemon.1 /usr/local/man/man1/
echo "/usr/local/man/man1/plebtrkdaemon.1"
sudo cp ../doc/plebtrkraw.1 /usr/local/man/man1/
echo "/usr/local/man/man1/plebtrkraw.1"

