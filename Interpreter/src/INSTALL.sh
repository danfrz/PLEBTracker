#!/bin/bash

mkdir /var/tmp/plebtrk 2>/dev/null

echo "Dependencies include:"
echo "   ncurses"     #trk
echo "   aplay"       #itp
echo "   inotifywait" #trkdaemon
echo "   flac"        #rend
echo ""

touch /var/tmp/plebtrk/itplog.log
touch /var/tmp/plebtrk/trklog.log

echo "Copying executables into /usr/bin/..."
sudo cp ./plebplay /usr/bin/plebplay
echo "/usr/bin/plebplay"
sudo cp ./plebitp /usr/bin/plebitp
echo "/usr/bin/plebitp"
sudo cp ./plebrender /usr/bin/plebrender
echo "/usr/bin/plebrender"
echo ""

mkdir /usr/local/man/man1 2>/dev/null

echo "Copying man pages into /usr/local/man/man1/..."
sudo cp ../doc/plebitp.1 /usr/local/man/man1/
echo "/usr/local/man/man1/plebitp.1"
sudo cp ../doc/plebplay.1 /usr/local/man/man1/
echo "/usr/local/man/man1/plebplay.1"
sudo cp ../doc/plebrender.1 /usr/local/man/man1/
echo "/usr/local/man/man1/plebrender.1"

