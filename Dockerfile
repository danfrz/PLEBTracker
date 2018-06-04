FROM base/archlinux

COPY . /usr/local/plebtrk
WORKDIR /usr/local/plebtrk

RUN pacman --noconfirm -Sy base-devel fftw alsa-utils inotify-tools
RUN cd ./Tracker/src && make && ./INSTALL.sh && cd ../../Interpreter/src && make && ./INSTALL.sh

ENTRYPOINT ["./Tracker/src/plebtrk"]