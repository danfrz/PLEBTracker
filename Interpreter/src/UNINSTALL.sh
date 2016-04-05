
#/bin/bash

echo "Removing temporary directory"
rm /var/tmp/plebtrk/*
rmdir /var/tmp/plebtrk

echo "Removing binaries"
sudo rm /usr/bin/plebitp
sudo rm /usr/bin/plebplay
sudo rm /usr/bin/plebrender

echo "Removing manuals"
sudo rm /usr/local/man/man1/plebitp.1
sudo rm /usr/local/man/man1/plebplay.1
sudo rm /usr/local/man/man1/plebrender.1
