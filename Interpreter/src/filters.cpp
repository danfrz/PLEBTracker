#include "filters.h"
#include <iostream>




unsigned int itrp::chooseWindowLength(unsigned int a)
{
    unsigned int i = 1;
    while ((i<<=1)<=a);
    if (i < 2048)
        i=2048;
    return i;
}

//This was from some example somewhere, I should have recorded where
void itrp::createHamming(float *window, unsigned int &window_len) {

    for(int i = 0; i < window_len; i++) {

        window[i] = 0.54 - (0.46 * std::cos( 2 * M_PI * (i / ((window_len - 1) * 1.0))));
    }
}


//IMPORTANT: Assumes that the entries in fft_sample_in are 0 at index bfr_len
//
//1. Assigns the bfr to fft_sample_in, centers the signal at 0 (assumes unsigned) and windows.
//2. Fourier transform is performed on the windowed, centered signal
//3. Result is stored in fft_transform. Returns pointer to fft_transform.
fftw_complex *itrp::fourierTransform(sample_res *bfr, const unsigned int &bfr_len, float *window, const unsigned int &window_len, fftw_complex *fft_transform, fftw_complex *fft_sample_in)
{
    bool temp_input_buffer = false;
    if(fft_sample_in == NULL)
    {
        temp_input_buffer = true;
        fft_sample_in = (fftw_complex *)fftw_malloc ( sizeof ( fftw_complex ) * window_len );
        for(int i = bfr_len; i < window_len; i++)
        {
            fft_sample_in[i][0] = 0.0;
            fft_sample_in[i][1] = 0.0;
        }
    }

#if SAMPLE_RES_IS_UNSIGNED
    //Copy sample data into input fft and window it with global window
    const unsigned long maximum = std::pow(2,sizeof(sample_res)*8);
    const sample_res middle = maximum/2;

    for(int i = 0; i < bfr_len; i++)
    {
        fft_sample_in[i][0] = (bfr[i]-middle)*window[i];
        fft_sample_in[i][1] = 0.0;
    }
#else
    
    for(int i = 0; i < bfr_len; i++)
    {
        fft_sample_in[i][0] = (bfr[i])*window[i];
        fft_sample_in[i][1] = 0.0;
    }
#endif


    fftw_plan plan_forward = fftw_plan_dft_1d (window_len, fft_sample_in, fft_transform, FFTW_FORWARD, FFTW_ESTIMATE );
    fftw_execute ( plan_forward );
    fftw_destroy_plan( plan_forward );

    if(temp_input_buffer)
        fftw_free(fft_sample_in);

    return fft_transform;
}


sample_res *itrp::backFourierTransform(sample_res *bfr, const unsigned int &bfr_len, float *window, const unsigned int &window_len, fftw_complex *fft_transform, fftw_complex *fft_out)
{
    bool temp_output_buffer = false;
    if(fft_out == NULL)
    {
        temp_output_buffer = true;
        fft_out = (fftw_complex *)fftw_malloc ( sizeof ( fftw_complex ) * window_len );
    }
    //double *out = (double*)fftw_malloc ( sizeof ( double ) * len);
    fftw_plan plan_backward = fftw_plan_dft_1d (window_len, fft_transform, fft_out, FFTW_BACKWARD, FFTW_ESTIMATE );
    fftw_execute ( plan_backward );

    fftw_destroy_plan( plan_backward);

#if SAMPLE_RES_IS_UNSIGNED
    const unsigned long maximum = std::pow(2,sizeof(sample_res)*8);
    const sample_res middle = maximum/2;

    for(int i = 0; i < bfr_len; i++)
    {
        double result;
        result = (fft_out[i][0]/window_len)/window[i]+middle;
        if(result > maximum)
        {
            std::cerr << "CLIPPED! " << result << '\n';
            result = maximum-1;
        }
        else if( result < 0)
        {
            std::cerr << "CLIPPED! " << result << '\n';
            result = 0;
        }
        bfr[i] = result; 
    }
#else
    //[-128,127] [-1-(2^8/2-1), 2^8/2-1] [-2^8/2, 2^8/2-1]
    const unsigned long maximum = std::pow(2,sizeof(sample_res)*8)/2 - 1;
    const unsigned long minimum = -1-maximum;
    for(int i = 0; i < bfr_len; i++)
    {
        double result;
        result = (fft_out[i][0]/window_len)/window[i];
        if(result > middle-1)
        {
            std::cerr << "CLIPPED! " << result << '\n';
            result = middle-1;
        }
        else if( result < -middle)
        {
            std::cerr << "CLIPPED! " << result << '\n';
            result = -middle;
        }
        bfr[i] = result; 
    }
#endif

    if(temp_output_buffer)
        fftw_free(fft_out);

    return bfr; 

}

fftw_complex *itrp::filter_highpass(fftw_complex *in, unsigned int cutoff, const unsigned int &window_len, Track *seltrk)
{
    //fftw_complex *out = (fftw_complex*)fftw_malloc ( sizeof ( fftw_complex ) * filter_len );
    fftw_complex *out = in;
    
    unsigned int window_half = window_len/2L;

    double flavor_cutoff = cutoff/(double)window_half;
    //flavor_cutoff*= flavor_cutoff;
    cutoff = flavor_cutoff*window_half;
    std::cerr << "HIGHPASS CUTOFF: " << cutoff << '\n';

    unsigned int minpass = std::min((window_half + 1), cutoff);
    if(minpass < 1) 
        minpass = 1;
    out[0][0] = 0;
    out[0][1] = 0;
    for(int i = 1; i < minpass; i++)
    {
        out[i][0] = 0;
        out[i][1] = 0;

        out[window_len-i][0] = 0;
        out[window_len-i][1] = 0;

    }

    return out;
}

fftw_complex *itrp::filter_lowpass(fftw_complex *in, unsigned int cutoff, const unsigned int &window_len, Track *seltrk)
{
    std::cerr << "LOWPASS INPUT: " << cutoff << '\n';
    fftw_complex *out = in;

    unsigned int window_half = window_len/2L;

    double flavor_cutoff = cutoff/(double)window_half;
    //flavor_cutoff*= flavor_cutoff;
    cutoff = flavor_cutoff*window_half;

    if(cutoff > window_half)
        cutoff = window_half;

    std::cerr << "LOWPASS CUTOFF: " << cutoff << '\n';

    if(cutoff == 0)
    {
        out[0][0] = 0;
        out[0][1] = 0;
        cutoff=1;

    }

    for(int i = cutoff; i <= window_half; i++)
    {
        out[i][0] = 0;
        out[i][1] = 0;

        out[window_len-i][0] = 0;
        out[window_len-i][1] = 0;
    }

    return out;
}



