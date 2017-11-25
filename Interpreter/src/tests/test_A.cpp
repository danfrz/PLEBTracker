#include <iostream>
#include <fftw3.h>
#include <vector>
#include <stdio.h>
#include <math.h>

typedef unsigned char sample_res;
typedef unsigned char sample_res_unsigned;

fftw_complex *fft_in;
fftw_complex *fft_out;
fftw_complex *fft_result;
sample_res *fft_samples;
float *window;
double window_len;
double window_half;

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


fftw_complex *filter_lowpass(fftw_complex *in, unsigned int lowpass, const unsigned int &filter_len)
{
    //fftw_complex *out = (fftw_complex*)fftw_malloc ( sizeof ( fftw_complex ) * filter_len );
    fftw_complex *out = in;
    
    int righthalf=window_half+1;

    double flavor_lowpass = lowpass/window_half;
    flavor_lowpass*= flavor_lowpass;
    lowpass = flavor_lowpass*window_half;


    unsigned int minpass = std::min((unsigned int)(righthalf), lowpass);
    //OOOH. It never filters anything if minpass<2
    if(minpass < 2) //Yes 2
        minpass = 2;
    for(int i = 1; i < minpass; i++)
    {
        out[i][0] = 0;
        out[i][1] = 0;
    }

    return out;
}

fftw_complex *filter_highpass(fftw_complex *in, unsigned int highpass, const unsigned int &filter_len)
{
    fftw_complex *out = in;

    if(highpass > window_half)//Yes 2
        return out;

    //The higher end sounds nicer. So lets transform it by x=y^2, y=sqrt(x) [0,1]
    double flavor_highpass = highpass/window_half;
    flavor_highpass= std::sqrt(flavor_highpass);
    highpass = flavor_highpass*window_half;
    
    if(highpass < 2)//Yes 2
        highpass = 2;
    
    for(int i = highpass; i < window_half+1; i++)
    {
        out[i][0] = 0;
        out[i][1] = 0;
    }

    return out;
}

fftw_complex *filter_modulopass(fftw_complex *in, unsigned int mod, const unsigned int &filter_len)
{
    //fftw_complex *out = (fftw_complex*)fftw_malloc ( sizeof ( fftw_complex ) * filter_len );
    fftw_complex *out = in;
    if(mod == 0)
        mod = 1;
    

    for(int i = 1; i < window_half; i++)
    {
        if((i % mod) == 0)
        {
            out[i][0] = 0;
            out[i][1] = 0;
        }
    }

    return out;
}


fftw_complex *fourierTransform(sample_res *bfr, const unsigned int &len)
{
    //Copy sample data into input fft and window it with global window
    for(int i = 0; i < len; i++)
    {
        fft_in[i][0] = bfr[i]*window[i];
        fft_in[i][1] = 0.0;
    }

    fftw_plan plan_forward = fftw_plan_dft_1d (window_len, fft_in, fft_out, FFTW_FORWARD, FFTW_ESTIMATE );
    fftw_execute ( plan_forward );
    fftw_destroy_plan( plan_forward );
    return fft_out;
}

sample_res *backFourierTransform(sample_res *buffer, const unsigned int &len)
{
    //double *out = (double*)fftw_malloc ( sizeof ( double ) * len);
    fftw_plan plan_backward = fftw_plan_dft_1d (window_len, fft_out, fft_result, FFTW_BACKWARD, FFTW_ESTIMATE );
    fftw_execute ( plan_backward );

    fftw_destroy_plan( plan_backward);
    for(int i = 0; i < len; i++)
    {
        //std::cerr << fft_result[i][0]/window_len << '\n';
        //buffer[i] = out[i]/len_dbl;
        buffer[i] = ((fft_result[i][0]/window_len)/window[i]);
    }
    return buffer; 

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
void hamming() {

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
    int seconds = 8;
    int bytes = seconds*44100;
    int subdivisions = (bytes)/0x1CB0;
    int bytespersub = bytes/subdivisions;


    window_len = chooseWindowLength(bytespersub);
    window_half = window_len/2;
    fft_in = (fftw_complex *)fftw_malloc ( sizeof ( fftw_complex ) * window_len );
    fft_out = (fftw_complex *)fftw_malloc ( sizeof ( fftw_complex ) * window_len );
    fft_result = (fftw_complex *)fftw_malloc ( sizeof ( fftw_complex ) * window_len );
    fft_samples = new sample_res[bytespersub];
    window = new float[(long)window_len];
    //std::cerr << "ALLOCATED\n";
    for(int i = bytespersub; i < window_len; i++)
    {
        fft_in[i][0] = 0.0;
        fft_in[i][1] = 0.0;
    }
    //std::cerr << "INITIALIZED fft_in\n";
    hamming();
    //std::cerr << "HAMMED\n";

    //How many more of the available frequencies are we removing at each iteration:
    float filterstep = (1.25f/subdivisions)*(window_half);
    //float filterstep = 90;


    sample_res bfr[bytes];
    float phase = 0;
    for(int i = 0 ; i < bytes; i++)
        bfr[i] = 128;
    //genNoise_White(bfr, 256, 255, phase, bytes);
    genSqr(bfr, 256, 250, phase, bytes);

    //std::cerr << "GENERATED\n";


    for(int i = 0; i < subdivisions; i++)
    {
        int lstbfr = (i)*bytespersub;
        std::cerr << "Iteration " << i+1 << '/' << subdivisions << " filter=" << std::sqrt((filterstep*i)/window_half) << '\n';


        fourierTransform(bfr+lstbfr, bytespersub);
        filter_highpass(fft_out, filterstep*(i), bytespersub);
        backFourierTransform(bfr+lstbfr, bytespersub);
    }

    fftw_free(fft_in);
    fftw_free(fft_out);
    delete [] fft_result;
    delete [] window;

    play(bfr, bytes);
    return 0;
}





