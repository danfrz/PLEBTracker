#ifndef FILTERS_H_
#define FILTERS_H_
#include <cmath>
#include <random>
#include <fftw3.h>
#include "track.h"



namespace itrp
{
    
    typedef fftw_complex *(*filter)(fftw_complex *in, unsigned int cutoff, const unsigned int &window_len, Track *seltrk);

    fftw_complex *filter_lowpass(fftw_complex *in, unsigned int cutoff, const unsigned int &window_len, Track *seltrk);

/***\//////////////////////////////////////////////////////////////////////////
Function: int chooseWindowLength(unsigned int a)

Description:
Returns an i such that i is greater than a and is a power of two >= 2048
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    unsigned int chooseWindowLength(unsigned int a);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void createHamming(float *window, unsigned long &window_len)

Description:
Creates a hamming window of specified size
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void createHamming(float *window, unsigned int &window_len);



    fftw_complex *fourierTransform(sample_res *bfr, const unsigned int &bfr_len, float *window, const unsigned int &window_len, fftw_complex *fft_transform, fftw_complex *fft_sample_in=NULL);


/***\//////////////////////////////////////////////////////////////////////////
Function: backFourierTransform(sample_res *bfr, const uint &bfr_len, float *window, const ulong &window_len, fftw_complex *fft_transform, fftw_complex *fft_out=NULL

Description:
1.  Takes the fft_transform, and back-transforms it into fft_out (or a temporary fft)
2.  bfr_len samples are taken from fft_out: are normalized, uncentered, and unwindowed
3.  Results are stored in bfr. Returns the pointer to bfr
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    sample_res *backFourierTransform(sample_res *bfr, const unsigned int &bfr_len, float *window, const unsigned int &window_len, fftw_complex *fft_transform, fftw_complex *fft_out=NULL);


/***\//////////////////////////////////////////////////////////////////////////    
Function: filter_lowpass(fftw_complex *in, uint cutoff, const uint &window_len)

Description:
Filter the input signal by the cutoff. Zeroes bins [cutoff,window_len/2]
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    fftw_complex *filter_lowpass(fftw_complex *in, unsigned int cutoff, const unsigned int &window_len, Track *seltrk);


/***\//////////////////////////////////////////////////////////////////////////
Function: filter_highpass(fftw_complex *in, unsigned int cutoff, const unsigned int &window_len)

Description:
Filter the input signal by the cutoff. Zeroes bins [cutoff,window_len/2] and [window/2,window_len-cutoff]
0 cutoff works as a special case.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    fftw_complex *filter_highpass(fftw_complex *in, unsigned int cutoff, const unsigned int &window_len, Track *seltrk);

}

#endif 
