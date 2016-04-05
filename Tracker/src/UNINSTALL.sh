#/bin/bash

echo "Removing temporary directory"
rm /var/tmp/plebtrk/*
rmdir /var/tmp/plebtrk

echo "Removing binaries"
sudo rm /usr/bin/plebtrk
sudo rm /usr/bin/plebtrkdaemon
sudo rm /usr/bin/plebtrkraw

echo "Removing manuals"
sudo rm /usr/local/man/man1/plebtrk.1
sudo rm /usr/local/man/man1/plebtrkdaemon.1
sudo rm /usr/local/man/man1/plebtrkraw.1
