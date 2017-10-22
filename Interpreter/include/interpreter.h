#ifndef INTERPRETER_H_
#define INTERPRETER_H_
#include <istream>
#include <ostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <stdio.h>
#include <ctype.h>
#include <ctgmath> //logb
#include <fftw3.h>
#include <chrono>

#include "song.h"
#include "instrument.h"
#include "pattern.h"
#include "generator.h"

#define ARPEGGIO_SPEED 6

namespace itrp
{


    struct Track
    {
        paramtable *ptbl;
        float lastfrq;
        float frq;
        float nextfrq;
        float phase;
        unsigned char filters_active;

        unsigned short segments;
        unsigned char fx;
        unsigned char fxparam;
        Instrument *inst;

        unsigned short wavei;
        unsigned short pulsei;
        unsigned short filteri;
        unsigned char lastwave;
        unsigned char waveduracc;
        unsigned char pulseduracc;

        //VOLUME
        unsigned char ptrnvol;
        unsigned char ptrnlastvol;
        unsigned char voljump;

        sample_res_unsigned lastvol;
        unsigned char voli;
        unsigned char volduracc;
        sample_res *fourier_buffer;
    };

    //not implemented yet
    char **songpaths;
    int numsongs;
    

    Song *song;
    Track *tracks;
    Pattern *curpattern;
    unsigned char order;
    generator *generators;
    bool trackmute[256];
    float amplifyall;
    unsigned int fourier_buffer_size;
    std::chrono::system_clock::time_point begin_time;



/***\//////////////////////////////////////////////////////////////////////////    
Function: void initializeWaveTable()
Description:
    Populates the wave table with wave generator functions
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void initializeWaveTable();

/***\//////////////////////////////////////////////////////////////////////////    
Function: void initializeRender()
Description:
    Prepares a song to render.
Precondition :
    itrp::song must be initialized
Postcondition:
    The interpreter is ready to begin rendering the song
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void initializeRender();




/***\//////////////////////////////////////////////////////////////////////////    
    Function: ::linearize(sample_res **buffer, const unsigned int orders, unsigned int *bytes, unsigned int &total_bytes

Description:
   Linearizes all play buffers.
   Returns a dynamic array of samples
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
sample_res *linearize(sample_res **buffer, const unsigned int orders, unsigned int *bytes, unsigned int &total_bytes);

fftw_complex *fourierTransform(sample_res *bfr, unsigned int &filter_len, const unsigned int &len);


fftw_complex *filter_lowpass(fftw_complex *in, const unsigned int &lowpass, const unsigned int &filter_len);
fftw_complex *filter_highpass(fftw_complex *in, const unsigned int &highpass, const unsigned int &filter_len);


sample_res *backFourierTransform(fftw_complex *in, const unsigned int &filter_len, const unsigned int &len);


void performFilter(sample_res *bfr, paramtable *ptbl, int bytes);

/***\//////////////////////////////////////////////////////////////////////////    
Function: bool resetsPhaseOnWave1Set(const unsigned char &wave)
Description:
    For certain wave functions that sound strange if the phase is not reset
    after wave 1 parameter is set
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    bool resetsPhaseOnWave1Set(const unsigned char &wave);



/***\//////////////////////////////////////////////////////////////////////////    
Function: void renderTick(unsigned char *buffer, const unsigned char &track, const unsigned int &bytes)
Description:
   Render one segment (row subdivision)
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void renderTick(sample_res *buffer, const unsigned char &track, const unsigned int &bytes);



/***\//////////////////////////////////////////////////////////////////////////    
Function: unsigned char renderPattern(int start, int end, unsigned int &bytes)
Description:
    Render a pattern from row start to end.
    Returns a new unsigned byte array and sets bytes to the number of bytes initialized
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
   sample_res *renderPattern(int start, int end, unsigned int &bytes);



/***\//////////////////////////////////////////////////////////////////////////    
Function: unsigned char **renderSong(unsigned int *bytes)
Description:
    Renders itrp::song, returns a new 2D unsigned 8 bit array, and remembers the
    amount of bytes in each one by storing them in `bytes`
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    sample_res **renderSong(unsigned int *bytes);

/***\//////////////////////////////////////////////////////////////////////////    
Function: unsigned char **renderSong(unsigned int *bytes, int start_order, int end_order, int start_row, int end_row)
Description:
    Renders itrp::song within the region provided, returns a new 2D unsigned 8 bit array,
    and remembers the amount of bytes in each one by storing them in `bytes`
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    sample_res **renderSong(unsigned int *bytes, int start_order, int end_order, int start_row, int end_row);



/***\//////////////////////////////////////////////////////////////////////////    
Function: bool load(const char *file)
Description:
   Load a song from a file
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    bool load(const char *file);



/***\//////////////////////////////////////////////////////////////////////////    
Function: void play(unsigned char **buffer, const unsigned int orders, unsigned int *bytes)
Description:
   Outputs the buffer to stdout
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void play(sample_res **buffer, const unsigned int orders, unsigned int *bytes);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void play(unsigned char *buffer, unsigned int bytes)
Description:
    Outputs the buffer to stdout
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void play(sample_res *buffer, unsigned int bytes);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void print(unsigned char **buffer, const unsigned int orders, unsigned int *bytes)
Description:
    Outputs a visualization of each resulting byte to stdout, recommend
    piping the data into less.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void print(sample_res **buffer, const unsigned int orders, unsigned int *bytes);


/***\//////////////////////////////////////////////////////////////////////////    
Function: void printByte(uchar &b, const char &character, const uchar &res

Description:
   Prints a single byte to cout
   using the specified character
   where each character represents 'res' units
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void printSample(sample_res &b, const char &character, const unsigned int &res);


/***\//////////////////////////////////////////////////////////////////////////    
Function: void printBuffer(unsigned char *bfr, const unsigned long &len

Description:
   Prints a whole buffer to the console
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void printBuffer(sample_res *bfr, const unsigned long &len);


/***\//////////////////////////////////////////////////////////////////////////    
Function: void purgeSong()
Description:
   Delete song data after interpreter
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void purgeSong();




}

#endif
