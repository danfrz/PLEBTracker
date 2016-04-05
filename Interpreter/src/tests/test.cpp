#include <cmath>
#include <cstdio>
#include <unistd.h>
#include <iostream>

const unsigned char INST_SILENCE = 0, INST_SQR = 1, INST_SAW = 2, INST_TRI = 3, INST_SINE=4;



void play(unsigned char *buffer, const unsigned long &len)
{
    for(long i = 0; i < len; i++)
        putchar(buffer[i]);
}

/***\//////////////////////////////////////////////////////////////////////////    
Function: void add(char *to, char *from, long len

Description:
   Add one char buffer to another
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
void add(unsigned char *to, unsigned char *from, const unsigned long &len)
{
    for(long i = 0; i < len; i++)
        to[i] += from[i];
}


/***\//////////////////////////////////////////////////////////////////////////    
Function: void avg(char *to, char *from, long len

Description:
   Average two bytes
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
void avg(unsigned char *to, const unsigned char *from, const unsigned long &len)
{
    for(long i = 0; i < len; i++)
    {
        int sum = to[i] + from[i];
        to[i] = sum/2;
    }
}



/***\//////////////////////////////////////////////////////////////////////////    
Function: void printByte(uchar &b, const char &character, const uchar &res

Description:
   Prints a single byte to cout
   using the specified character
   where each character represents 'res' units
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
void printByte(unsigned char &b, const char &character, const unsigned char &res)
{
    while(b > res)
    {
        std::cout << character;
        b-=res;
    }
    std::cout << character << '\n';
}


/***\//////////////////////////////////////////////////////////////////////////    
Function: void printBuffer(unsigned char *bfr, const unsigned long &len

Description:
   Prints a whole buffer to the console
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
void printBuffer(unsigned char *bfr, const unsigned long &len)
{
    for(unsigned long i = 0; i < len; i++)
        printByte(bfr[i], '#', 8);
}


/***\//////////////////////////////////////////////////////////////////////////    
Function: unsigned long ceil(const float &a)

Description:
   only for positive values
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
unsigned long ceil(const float &a)
{
    unsigned long out = a;
    if(a - out > 0)
        return out + 1;
    return out;
}


/***\//////////////////////////////////////////////////////////////////////////    
Function: void genSaw(uchar *bfr, uchar period, uchar amplitude, const ulong &len

Description:
   
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
void genSaw(unsigned char *bfr, const unsigned char &period, const unsigned char &amplitude, const unsigned long &len)
{
    int ovr = 128;
    char acc = ceil(amplitude / static_cast<float>(period));

    for(long i = 0; i < len; i++)
    {
        bfr[i] = ovr;
        ovr += acc;
        if(ovr > 255)
            ovr = 0;
    }
}


/***\//////////////////////////////////////////////////////////////////////////    
Function: void genSqr(uchar *bfr, uint period, uint amplitude, const ulong &len

Description:
   Populate a buffer with a square wave
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
void genSqr(unsigned char *bfr, const unsigned int &period, const unsigned char &amplitude, const unsigned long &len)
{
    for(long i = 0; i < len; i++)
        bfr[i] = ((i%period) < (period/2))*amplitude;
}

void genSqrPwm(unsigned char *bfr, unsigned int period, const unsigned char &amplitude, const unsigned long &len)
{

    for(long i = 0; i < len; i++)
    {//every 8 period 
        bfr[i] = ((i % period) < (period/2 + 32*std::sin((2*M_PI*i)/(128*period))))*amplitude;
    }
}

/***\//////////////////////////////////////////////////////////////////////////    
Function: void genTri(uchar *bfr, uint period, uint amplitude, const ulong &len

Description:
   Populate a buffer with a triangle wave
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
void genTri(unsigned char *bfr, unsigned int period, const unsigned char &amplitude, const unsigned long &len)
{
    int ovr = 128;
    period/=2;
    char acc = ceil(amplitude / static_cast<float>(period));

    for(long i = 0; i < len; i++)
    {
        bfr[i] = ovr;
        ovr +=acc;
        if(ovr > 255)
        {
            ovr = 255;
            acc = -acc;
        }
        else if(ovr < 0)
        {
            ovr = 0;
            acc = -acc;
        }
    }
}


void genSilence(unsigned char *bfr, const unsigned char &height, const unsigned long &len)
{
    for(unsigned long i = 0; i < len; i++)
        bfr[i] = height;
}

void genSine(unsigned char *bfr, const unsigned int &period, unsigned char amplitude, const unsigned long &len)
{
    amplitude/=2;
    for(unsigned long i = 0; i < len; i++)
        bfr[i] = amplitude*std::sin((2*M_PI*i)/period) + 128;
}

#define R_OCTAVE 0x70000000
#define R_NOTE 0x0F000000
#define R_INSTRUMENT 0x00F60000
#define R_VOLUME 0x0003F000
#define R_EFFECT 0x00000F00
#define R_FXPARAM1 0x000000F0
#define R_FXPARAM2 0x0000000F



void interpret(unsigned char *bfr, unsigned int in, const unsigned long &len)
{
    //0000 0000 0000 0000 0000 0000 0000 0000

    //1000 0000 0000 0000 0000 0000 0000 0000 active
    //0111 0000 0000 0000 0000 0000 0000 0000 octave
    //0000 1111 0000 0000 0000 0000 0000 0000 note C C# D D# E F F# G G# A A# B
    //0000 0000 1111 1100 0000 0000 0000 0000 instrument
    //0000 0000 0000 0011 1111 0000 0000 0000 volume 64
    //0000 0000 0000 0000 0000 1111 1111 1111 effect

    const bool on             = (in & 0x80000000) >> (4*7)+3;
    if(!on)
        return;

    unsigned char  oct  = ((in & R_OCTAVE) >> (4*7)) + 1;
    unsigned char  note = (in & R_NOTE) >> (4*6);
    unsigned char  inst = (in & R_INSTRUMENT) >> (4*4+2);
    unsigned char  vol  = (in & R_VOLUME) >> (4*3);

    unsigned char  fx   = (in & R_EFFECT) >> (4*2);
    unsigned char  fxp1 = (in & R_FXPARAM1) >> (4);
    unsigned char  fxp2 = (in & R_FXPARAM2);

    unsigned int period = 128 / (oct);
    //period = period - note*(period - 128/(oct+1))/12.0;
    period = period - note*(period/24.0);
    unsigned char amplitude = (vol/63.0f)*255;

    switch(inst)
    {
        case INST_SILENCE: //silence
            genSilence(bfr, 128, len);
            std::cerr << "Silence length=" << len << '\n';
            break;
        case INST_SQR: //sqr
            genSqr(bfr, period, amplitude, len);
            std::cerr << "Square [" << int(oct) << ',' << int(note) << "] period=" << period << " amplitude=" << static_cast<int>(amplitude) << " length=" << len << '\n';
            break;
        case INST_SAW: //saw
            genSaw(bfr, period, amplitude, len);
            std::cerr << "Saw [" << int(oct) << ',' << int(note) << "] period=" << period << " amplitude=" << static_cast<int>(amplitude) << " length=" << len << '\n';
            break;
        case INST_TRI: //tri
            genTri(bfr, period, amplitude, len);
            std::cerr << "Triangle [" << int(oct) << ',' << int(note) << "] period=" << period << " amplitude=" << static_cast<int>(amplitude) << " length=" << len << '\n';
            break;
        case INST_SINE: // sine
            genSine(bfr, period, amplitude, len);
            std::cerr << "Sine [" << int(oct) << ',' << int(note) << "] period=" << period << " amplitude=" << static_cast<int>(amplitude) << " length=" << len << '\n';
            break;
        default:
            std::cerr << "Instrument " << inst << " does not exist!\n";
    }
}
    

int main()
{
    unsigned long length = 1<<15;
    unsigned char *temp = new unsigned char[length];
    unsigned char *out = new unsigned char[length]{0};
    unsigned int ld4 = length/2;

    int rowlen = 12;
    unsigned int rows[rowlen]
    {
            0b10010000000001111111000000000000,
            0b10010001000001111111000000000000,
            0b10010010000001111111000000000000,
            0b10010011000001111111000000000000,
            0b10010100000001111111000000000000,
            0b10010101000001111111000000000000,
            0b10010110000001111111000000000000,
            0b10010111000001111111000000000000,
            0b10011000000001111111000000000000,
            0b10011001000001111111000000000000,
            0b10011010000001111111000000000000,
            0b10011011000001111111000000000000
    }; 

    int rowduration = 1400;

    for(int i = 0; i < rowlen; i++)
        interpret(temp+i*rowduration, rows[i], rowduration);

    add(out, temp, length);

    play(out, length);
    //printBuffer(out, length);

    delete [] temp;
    delete [] out;

    return 0;
}
