#include <iostream>
#include <fftw3.h>
#include <vector>
#include <stdio.h>
#include <math.h>

typedef unsigned char sample_res;
typedef unsigned char sample_res_unsigned;

void genSqr(sample_res *bfr,  const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len)
{
    if(period == 0 || amplitude == 0)
        return;

    long offset = phase*period;

    //unsigned long lowlen = len - (len % long(period));
    
    //Offset the length and initial phase by the phase
    const long prd = period;
    unsigned long i;
    for(i = 0; i < len; i++)
        bfr[i] += ((((i+offset)%prd) < (period/2))-0.5f)*amplitude;
    phase = ((i+offset)%prd) / period;
}

void genNoise_White(sample_res *bfr, const float &period, const sample_res_unsigned &height, float &phase, const unsigned long &len)
{
    if(height == 0)
        return;
    int halfamp = height/2;
    //Note, not actually white noise, for the record
    for(unsigned long i = 0; i < len; i++)
        bfr[i] += (rand() % height) - halfamp;
}


void genSine(sample_res *bfr,  const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len)
{
    if(period == 0 || amplitude == 0)
        return;

    const float tau = (2*M_PI);
    float halfamp = amplitude/2.0;
    unsigned long i;
    for(i = 0; i < len; i++)
        bfr[i] += halfamp*std::sin(phase + (tau*i)/period);
    phase = phase + (tau*i)/period;
}


fftw_complex *filter_highpass(fftw_complex *in, unsigned int cutoff, const unsigned long window_len)
{
    //fftw_complex *out = (fftw_complex*)fftw_malloc ( sizeof ( fftw_complex ) * filter_len );
    fftw_complex *out = in;
    
    unsigned int window_half = window_len/2L;

    double flavor_cutoff = cutoff/(double)window_half;
    flavor_cutoff*= flavor_cutoff;
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

fftw_complex *filter_lowpass(fftw_complex *in, unsigned int cutoff, const unsigned long window_len)
{
    std::cerr << "LOWPASS INPUT: " << cutoff << '\n';
    fftw_complex *out = in;

    unsigned int window_half = window_len/2L;

    double flavor_cutoff = cutoff/(double)window_half;
    flavor_cutoff*= flavor_cutoff;
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

//This sounds awful btw.
//Maybe if it filtered by octaves :o
//What if there was a filter operation that just filtered based on key signatures?
fftw_complex *filter_modulopass(fftw_complex *in, unsigned int mod, const unsigned int &window_len)
{
    //fftw_complex *out = (fftw_complex*)fftw_malloc ( sizeof ( fftw_complex ) * filter_len );
    fftw_complex *out = in;
    unsigned int window_half = window_len/2L;

    double flavor_cutoff = mod/(double)window_half;
    flavor_cutoff*= flavor_cutoff;
    flavor_cutoff*= flavor_cutoff;
    mod = flavor_cutoff*window_half;

    if(mod == 0)
        mod = 1;
 

    for(int i = 1; i < window_half; i++)
    {
        if((i % mod) == 0)
        {
            out[i][0] = 0;
            out[i][1] = 0;

            out[(long)window_len-i][0] = 0;
            out[(long)window_len-i][1] = 0;
        }
    }

    return out;
}


//IMPORTANT: Assumes that the entries in fft_sample_in are 0 at index bfr_len
//
//1. Assigns the bfr to fft_sample_in, centers the signal at 0 (assumes unsigned) and windows.
//2. Fourier transform is performed on the windowed, centered signal
//3. Result is stored in fft_transform. Returns pointer to fft_transform.
fftw_complex *fourierTransform(sample_res *bfr, const unsigned int &bfr_len, float *window, const unsigned long &window_len, fftw_complex *fft_transform, fftw_complex *fft_sample_in=NULL)
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

    //Copy sample data into input fft and window it with global window
    for(int i = 0; i < bfr_len; i++)
    {
        fft_sample_in[i][0] = (bfr[i]-128)*window[i];
        fft_sample_in[i][1] = 0.0;
    }

    fftw_plan plan_forward = fftw_plan_dft_1d (window_len, fft_sample_in, fft_transform, FFTW_FORWARD, FFTW_ESTIMATE );
    fftw_execute ( plan_forward );
    fftw_destroy_plan( plan_forward );

    if(temp_input_buffer)
        fftw_free(fft_sample_in);

    return fft_transform;
}

sample_res *backFourierTransform(sample_res *bfr, const unsigned int &bfr_len, float *window, const unsigned long &window_len, fftw_complex *fft_transform, fftw_complex *fft_out=NULL)
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
    for(int i = 0; i < bfr_len; i++)
    {
        double result = (fft_out[i][0]/window_len)/window[i]+128;
        if(result > 255)
        {
            std::cerr << "CLIPPED! " << result << '\n';
            result = 255;
        }
        else if( result < 0)
        {
            std::cerr << "CLIPPED! " << result << '\n';
            result = 0;
        }
        //buffer[i] = out[i]/len_dbl;
        bfr[i] = result; 
    }

    if(temp_output_buffer)
        fftw_free(fft_out);

    return bfr; 

}



void play(sample_res *buffer, unsigned int bytes)
{
    //Output all of the data from one byte buffer to stdout
    for(unsigned int i = 0; i < bytes; i++)
    {
        //TODO will need to be revised if sample_res ever changes
        if(sizeof(sample_res) ==1)
            putchar(buffer[i]);
        else if(sizeof(sample_res) == 2)
        {
            putchar((char)(buffer[i]&0xFF)); 
            putchar((char)((buffer[i]&0xFF00) >> 8));
        }
    }
}

// Create a hamming window of windowLength samples in buffer
void hamming(float *window, unsigned long &window_len) {

    for(int i = 0; i < window_len; i++) {

        window[i] = 0.54 - (0.46 * std::cos( 2 * M_PI * (i / ((window_len - 1) * 1.0))));
    }
}

unsigned int chooseWindowLength(unsigned int a)
{
    unsigned int i = 1;
    while ((i<<=1)<=a);
    if (i < 2048)
        i=2048;
    return i;
}

int main()
{
    //Play at a rate of 44100
    //GOAL: Filter chunks of signal by different filter values at each subdivision
    int seconds = 8;
    int bytes = seconds*44100;
    int subdivisions = (bytes)/0x1CB0;
    int bytespersub = bytes/subdivisions; //Bytes in a subdivision

    fftw_complex *fft_sample_in;
    fftw_complex *fft_transform;
    fftw_complex *fft_out;
    float *window;
    unsigned long window_len;
    unsigned int window_half;

    window_len = chooseWindowLength(bytespersub);
    window_half = window_len/2L;

    fft_sample_in = (fftw_complex *)fftw_malloc ( sizeof ( fftw_complex ) * window_len );
    fft_transform = (fftw_complex *)fftw_malloc ( sizeof ( fftw_complex ) * window_len );
    fft_out = (fftw_complex *)fftw_malloc ( sizeof ( fftw_complex ) * window_len );
    window = new float[(long)window_len];
    //std::cerr << "ALLOCATED\n";

    //Zero out the section of the input fft that will never ve written by a subdivision to 0
    //(The subdivisions are ALL the same size, even the final one)
    for(int i = bytespersub; i < window_len; i++)
    {
        fft_sample_in[i][0] = 0.0;
        fft_sample_in[i][1] = 0.0;
    }
    hamming(window, window_len);
    //std::cerr << "COOKED UP A HAM FOR LATER USE\n";

    //How many more of the available frequencies are we removing at each iteration:
    float filterstep = (1.0f/subdivisions)*(window_half);
    //float filterstep = 45;

    sample_res bfr[bytes];
    sample_res bfr2[bytes];
    for(int i = 0 ; i < bytes; i++)
    {
        bfr[i] = 128;
        bfr2[i] = 128;
    }

    //Generate a waveform in a `bytes` long sample_res buffer
    float phase = 0;
    genSqr(bfr, 256, 34, phase, bytes);
    genSine(bfr2, 127, 34, phase, bytes);
    genSine(bfr2, 60, 60, phase, bytes);

    for(int i = 0 ; i < bytes; i++)
    {
        bfr[i] = bfr[i] + bfr2[i] - 128;
    }
    //std::cerr << "GENERATED\n";

    //Loop through each subdivision cutting out the appropriate bytes
    //  Create a fourierTransform which populates fft_sample_in and forward transforms it into fft_transform
    //  Filter the fourier transform by zeroing terms
    //  Convert transform back into sample_res buffer
    for(int i = 0; i < subdivisions; i++)
    {
        int lstbfr = (i)*bytespersub;
        std::cerr << "Iteration " << i+1 << '/' << subdivisions << '\n';


        fourierTransform(bfr+lstbfr, bytespersub, window, window_len, fft_transform, fft_sample_in);
        
        
        filter_highpass(fft_transform, i*filterstep, bytespersub);
        if(i == 29)
        {
            for(int j = 0; j < window_len; j++)
            {
                std::cerr << '[' <<j << "] " << fft_transform[j][0] << '\n';
            }
        }

        backFourierTransform(bfr+lstbfr, bytespersub, window, window_len, fft_transform, fft_out);
    }

    //Free memory and play filterred buffer
    fftw_free(fft_sample_in);
    fftw_free(fft_transform);
    fftw_free(fft_out);
    delete [] window;

    play(bfr, bytes);
    return 0;
}






