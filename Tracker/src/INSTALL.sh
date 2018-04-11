#!/bin/bash

if [[ $EUID -eq 0 ]]; then
echo "Unless you only want to run the tracker as root" 1>&2
echo "don't run this script as root." 1>&2
echo "This script will prompt with sudo." 1>&2
echo "" 1>&2

read -p "Continue anyway [not recommended] y/n: " answer
case $answer in
[nN]* ) exit;;
esac

fi

echo "Dependencies include:"
echo "   ncurses"     #trk
echo "   aplay"       #itp
echo "   inotifywait" #trkdaemon
echo ""
suffix=""
if [ $# -eq 1 ]
then
suffix=$1
fi

echo "Creating log files"
userName=$USER
primaryGrp=`id -gn`

sudo mkdir /var/tmp/plebtrk 2>/dev/null
sudo chmod 755 /var/tmp/plebtrk
sudo chgrp $primaryGrp /var/tmp/plebtrk
sudo chown $userName /var/tmp/plebtrk

touch /var/tmp/plebtrk/itplog.log
touch /var/tmp/plebtrk/trklog.log

echo "Copying executables into /usr/bin/..."
sudo cp ./plebtrk "/usr/bin/plebtrk$1"
echo "/usr/bin/plebtrk$1"
sudo cp ./plebtrkdaemon "/usr/bin/plebtrkdaemon$1"
echo "/usr/bin/plebtrkdaemon$1"
sudo cp ./plebtrkraw "/usr/bin/plebtrkraw$1"
echo "/usr/bin/plebtrkraw$1"
echo ""

if [ ! -d /usr/local/man ]; then
	sudo mkdir /usr/local/man	
fi
if [ ! -d /usr/local/man/man1 ]; then
	sudo mkdir /usr/local/man/man1/
fi

echo "Copying man pages into /usr/local/man/man1/..."
sudo cp ../doc/plebtrk.1 /usr/local/man/man1/
echo "/usr/local/man/man1/plebtrk.1"
sudo cp ../doc/plebtrkdaemon.1 /usr/local/man/man1/
echo "/usr/local/man/man1/plebtrkdaemon.1"
sudo cp ../doc/plebtrkraw.1 /usr/local/man/man1/
echo "/usr/local/man/man1/plebtrkraw.1"

