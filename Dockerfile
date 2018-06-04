FROM alpine:3.7

COPY . /usr/local/plebtrk
WORKDIR /usr/local/plebtrk

RUN apk add --no-cache alpine-sdk ncurses-dev fftw-dev alsa-utils inotify-tools
RUN cd ./Tracker/src && make && ./INSTALL.sh && cd ../../Interpreter/src && make && ./INSTALL.sh

ENTRYPOINT ["plebtrk"]