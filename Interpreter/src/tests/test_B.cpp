#include <iostream>
#include <fftw3.h>
#include <vector>
#include <stdio.h>
#include <math.h>

typedef int sample_res;
typedef unsigned int sample_res_unsigned;
#define SAMPLE_RES_IS_UNSIGNED 0

//TODO Use rate 48000

std::vector<sample_res> *track_buffers;


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
    fftw_plan plan_forward = fftw_plan_dft_r2c_1d ( len, in, out, FFTW_PATIENT );
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
    fftw_plan plan_backward = fftw_plan_dft_c2r_1d ( len, in, out, FFTW_PATIENT);
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
        else if(sizeof(sample_res) == 4)
        {
            putchar((char)(buffer[i] &0x000000FF)); 
            putchar((char)((buffer[i]&0x0000FF00) >> 8));
            putchar((char)((buffer[i]&0x00FF0000) >> 16));
            putchar((char)((buffer[i]&0xFF000000) >> 24));
        }
    }
}







void performFilter(sample_res *bfr, float parameter, int bytes)
{
    sample_res *outbfr;

    unsigned int filter_len = 0;
    fftw_complex *transform = fourierTransform(bfr, filter_len, bytes);
    filter_highpass(transform, parameter, filter_len);
    outbfr = backFourierTransform(transform, filter_len, bytes);
    for(int j = 0; j < bytes; j++)
        bfr[j] = outbfr[j];

    fftw_free(transform);
    fftw_free(outbfr);
}

void test_ProgressiveFilter()
{

    /*OKAY SO Here's what's happenning here.
     *We're making a mock-interpreter where we have
     *  BYT(BPR, Bytes per row) SEG (Subdivisions in a row)
     * 
     *In the normal interpreter each segment is made one after
     *another in a pretty straight-forward manner. Each segment
     *was rendered into a row into a pattern which for each track
     *was simply a stream of sample_res that it mixed together.
     *
     * When it played it just played each of those mixed sample_res buffers
     * in succession
     *
     *So that would be things as normal without filtering, but as expected
     *Filter doesn't make things easy.
     *
     * For one, the input to the discrete fourier transform needs to be a power
     * of 2 so you can't just put those subdivision buffers into dft and expect
     * to get nice output.
     *
     * Therefore you need to make the input the size of a power of two. 
     * The buffer_size here (8192 recommended) is a power of two and
     * initialized to silence. The segment is written into the last indicies of
     * the buffer [offset, buffer_size) . The first part of the buffer [0, offset)
     * is silence. The DFT, filter, and backwards DFT is performed on the whole
     * buffer. Then the last bytes [offset, buffer_size) are copied into the final,
     * filtered song buffer. I have a concern that that could cause clicks at boundaries
     * but we'll see. Before the computation of the next segment, the bytes are shifted
     * left such that the end of the last produced segment is immediately preceding where
     * the next segment will be placed. Hopefully this will work and avoid clicking!
     */




    int BPR = 0x1cb0;
    int subdivisions = 0x18;
    int rows = 0x80;

    int subd_size = BPR/subdivisions; //bytes per row
    int bytes = rows*BPR;

    int tracks = 1;
    
    //Fourier transform likes powers of two.
    int buffer_size = 0b1000000000000;
    while(buffer_size < subd_size) //If buffer size is less then the
        buffer_size <<= 1;          //smallest subdivision, then find the
                                    //next power of two that is greater.

    //The offset where new bytes will be introduced into the buffer
    int offset = buffer_size - subd_size;

    //
    sample_res bfr[buffer_size];

#if SAMPLE_RES_IS_UNSIGNED
        unsigned long middle = std::pow(2, sizeof(sample_res)*8) / 2;
        for(int i = 0; i < buffer_size; i++)
            bfr[i]=middle;
#else
        for(int i = 0; i < buffer_size; i++)
            bfr[i]=0;
#endif

    std::cerr << "GENERATING AND FILTERING TRACKS\n";
    
    track_buffers = new std::vector<sample_res>[tracks];
    float phase[tracks];
    for(int t = 0; t < tracks; t++)
    {
        track_buffers[t].reserve(bytes);
        phase[t] = 0;
    }

    for(int row = 0; row < rows; row++)
    {
        for(int seg = 0; seg < subdivisions; seg++)
        {
            for(int t = 0; t < tracks; t++)
            {
                //Shift the input such that the buffer is empty
                //and the previous part of the song is in the beginning of the buffer.
                //The normal assumption that "bytes" is the length of bfr is incorrect here./
                //buffer_size is the size of the buffer.
                for(int i = bytes; i < buffer_size; i++)
                    bfr[i-subd_size] = bfr[i];

                //Reset the part of the buffer to be used for the new segment
#if SAMPLE_RES_IS_UNSIGNED
                {
                    unsigned long middle = std::pow(2, sizeof(sample_res)*8) / 2;
                    for(int i = offset; i < buffer_size; i++)
                        bfr[i]=middle;
                }
#else
                {
                    for(int i = offset; i < buffer_size; i++)
                        bfr[i]=0;
                }
#endif

                //genSqr(bfr+offset, 50+40*t+3*row, std::pow(2,sizeof(sample_res)*8)/2, phase[t], subd_size);
                genSqr(bfr+offset, 600, std::pow(2,sizeof(sample_res)*8)/2.5, phase[t], subd_size);

                performFilter(bfr, 8*row,  buffer_size);

                for(int i = offset; i < buffer_size; i++)
                    track_buffers[t].push_back(bfr[i]);
            }
        }
    }

    std::cerr << "MADE FILTERED TRACKS\n";
    std::cerr << "track length: " << track_buffers[0].size() << '\n';
    std::cerr << "predicted length: " << bytes << '\n';

    sample_res *song_buffer = new sample_res[bytes];

    for(int i = 0; i < bytes; i++)
    {
        sample_res tmp = 0;
        for(int t = 0; t < tracks; t++)
            tmp += track_buffers[t].at(i);
        //IMPORTANTE:
#if SAMPLE_RES_IS_UNSIGNED
        if( (tracks % 2) == 0)
            tmp += std::pow(2,sizeof(sample_res)*8)/2;
#endif
        song_buffer[i] = tmp;
    }


    std::cerr << "PLAYING\n";
    play(song_buffer, bytes);
    std::cerr << "YOU GOT PLAYED\n";

}


int main()
{
    test_ProgressiveFilter();
    return 0;
}




