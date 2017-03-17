#include <iostream>
#include <fftw3.h>
#include <vector>
#include <stdio.h>

typedef unsigned char sample_res;
typedef unsigned char sample_res_unsigned;

std::vector<sample_res> song_buffer;
std::vector<sample_res> *track_buffers;


void mix(int index, int tracks, std::vector<sample_res> *track_buffers)
{

    for(int t = 0; t < tracks; t++)
    {
        sample_res sum = 0;
        for(std::vector<sample_res>::iterator it = track_buffers[t].begin() + index;
                it != track_buffers[t].end();
                it++)
            sum += *it;
        song_buffer.push_back(sum);
    }
}

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


fftw_complex *fourierTransform(sample_res *bfr, unsigned int &filter_len, const unsigned int &len)
{
    double *in = (double*)fftw_malloc (  sizeof ( double ) * len );
    for(int i = 0; i < len; i++)
        in[i] = bfr[i];

    filter_len = (len/2) + 1;
    fftw_complex *out = (fftw_complex *)fftw_malloc ( sizeof ( fftw_complex ) * filter_len );
    fftw_plan plan_forward = fftw_plan_dft_r2c_1d ( len, in, out, FFTW_MEASURE );
    fftw_execute ( plan_forward );
    fftw_destroy_plan( plan_forward );
    delete [] in;
    return out;
}

fftw_complex *filter_lowpass(fftw_complex *in, const unsigned int &lowpass, const unsigned int &filter_len)
{
    //fftw_complex *out = (fftw_complex*)fftw_malloc ( sizeof ( fftw_complex ) * filter_len );
    fftw_complex *out = in;
    
    unsigned int minpass = std::min(filter_len, lowpass);

    //index 0 contains important information
    //so skip it
    for(int i = 1; i < minpass; i++)
    {
        out[i][0] = 0;
        out[i][1] = 0;
    }

    return out;
}

fftw_complex *filter_highpass(fftw_complex *in, const unsigned int &highpass, const unsigned int &filter_len)
{
    //fftw_complex *out = (fftw_complex*)fftw_malloc ( sizeof ( fftw_complex ) * filter_len );
    fftw_complex *out = in;
    

    for(int i = highpass; i < filter_len; i++)
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
    

    for(int i = 1; i < filter_len; i++)
    {
        if((i % mod) == 0)
        {
            out[i][0] = 0;
            out[i][1] = 0;
        }
    }

    return out;
}

sample_res *backFourierTransform(fftw_complex *in, const unsigned int &filter_len, const unsigned int &len)
{
    double *out = (double*)fftw_malloc ( sizeof ( double ) * len);
    fftw_plan plan_backward = fftw_plan_dft_c2r_1d ( len, in, out, FFTW_MEASURE);
    fftw_execute ( plan_backward );

    fftw_destroy_plan( plan_backward);
    sample_res *aout = new sample_res[len];
    for(int i = 0; i < len; i++)
        aout[i] = out[i]/len;
    fftw_free(out);
    return aout; 

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







void performFilter(sample_res *bfr, int bytes)
{
    int bytespersub = 8192;
    int subdivisions = bytes / bytespersub;

    float filterstep = 32;

    

    sample_res *outbfr;

    for(int i = 0; i < subdivisions; i++)
    {
        for(int j = 0; j <1 ; j++)
        {
            unsigned int filter_len = 0;
            fftw_complex *transform = fourierTransform(bfr+i*bytespersub, filter_len, bytespersub);
            filter_highpass(transform, filterstep*i, filter_len);
            outbfr = backFourierTransform(transform, filter_len, bytespersub);
            for(int j = 0; j < bytespersub; j++)
                bfr[bytespersub*i+j] = outbfr[j];

            fftw_free(transform);
            fftw_free(outbfr);
        }
    }
}

void test_ProgressiveFilter()
{
    int bytes = 262144;
    int subd = bytes/32;
    int subdd = subd/3;
    sample_res bfr[bytes];

    float phase = 0;
    for(int i = 0; i < 32; i++)
    {
        for(int j = 0; j < 3; j++)
            genNoise_White(bfr+ i*subd + j*subdd, 100+j*240, 0x80-i*2, phase, subdd);
    }

    performFilter(bfr, bytes);

    play(bfr, bytes);

}


int main()
{
    test_ProgressiveFilter();
    return 0;
}


int main2()
{
    int bytes = 256000;
    int subdivisions = 256000/44100;
    int bytespersub = bytes/subdivisions;

    float filterstep = 100.f;

    sample_res bfr[bytes];
    float phase = 0;
    genSqr(bfr, 256, 0x02, phase, bytes);


    sample_res *outbfr;

    for(int i = 0; i < subdivisions; i++)
    {
        int lstbfr = (i-1)*bytespersub;
        int modify = 2;
        if(lstbfr < 0)
        {
            lstbfr = 0;
            modify = 1;
        }


        unsigned int filter_len = 0;
        fftw_complex *transform = fourierTransform(bfr+lstbfr, filter_len, modify*bytespersub);
        filter_lowpass(transform, filterstep*i, filter_len);
        outbfr = backFourierTransform(transform, filter_len, modify*bytespersub);
        for(int j = 0; j < bytespersub; j++)
            bfr[bytespersub*i+j] = outbfr[j+(modify-1)*bytespersub];

        fftw_free(transform);
        fftw_free(outbfr);
    }

    play(bfr, bytes);
    return 0;
}







