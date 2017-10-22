#ifndef GENERATOR_H_
#define GENERATOR_H_
#include <cmath>
#include <random>
#include "instrument.h"

struct paramtable
{
    unsigned short PULSE1;
    unsigned short PULSE2;
    unsigned short CUSTOM_JUMP_WAV, CUSTOM_JUMP_PLS, CUSTOM_JUMP_FLT;
    unsigned char LOOP_WAVE, WAVE1, WAVE2, LOOP_PULSE, LOOP_FA_WAVE, LOOP_FA_PULSE, LOOP_FILTER;
    sample_res CHAIN, LAST;

    unsigned char FILTER[0x10];
    short         FILTERP[0x10];

};

//Form of a wave generator function 
typedef void (*generator)(sample_res *bfr, paramtable *ptbl, const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);

//Two generator functions are used for multiplexing
extern generator GEN_MUX1, GEN_MUX2;


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//SAW WAVES||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void genSaw(sample_res *bfr, paramtable *ptbl, const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);

void genSawPulse(sample_res *bfr, paramtable *ptbl,  const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);




//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//SQUARE WAVES|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

/***\//////////////////////////////////////////////////////////////////////////    
Function: void genSqr(uchar *bfr, uint period, uint amplitude, const ulong &len

Description:
   Populate a buffer with a square wave
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
void genSqr(sample_res *bfr, paramtable *ptbl, const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);

void genSqrPulse(sample_res *bfr,  paramtable *ptbl,const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);

void genSqrPulse2(sample_res *bfr, paramtable *ptbl,  const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);

void genSqrPulseHybrid(sample_res *bfr, paramtable *ptbl,  const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);

void genSqrNES(sample_res *bfr, paramtable *ptbl, const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);

void genSqrPulseNES(sample_res *bfr,  paramtable *ptbl,const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);

void genSqrVTX(sample_res *bfr, paramtable *ptbl, const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);

void genSqrPulseVTX(sample_res *bfr, paramtable *ptbl, const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//TRIANGLE WAVES|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


/***\//////////////////////////////////////////////////////////////////////////    
Function: void genTri(uchar *bfr, uint period, uint amplitude, const ulong &len

Description:
   Populate a buffer with a triangle wave
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
void genTri(sample_res *bfr, paramtable *ptbl, const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);

void genTriPulse(sample_res *bfr, paramtable *ptbl,  const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//SINE WAVES|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void genSine(sample_res *bfr, paramtable *ptbl, const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);

void genSinePulse(sample_res *bfr, paramtable *ptbl,  const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);

void genSinePulse2(sample_res *bfr, paramtable *ptbl,  const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);

void genSinePulseHybrid(sample_res *bfr, paramtable *ptbl,  const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);

void genHalfSine(sample_res *bfr, paramtable *ptbl,  const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);

void genHalfSinePulse(sample_res *bfr, paramtable *ptbl,  const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);

void genNSine(sample_res *bfr, paramtable *ptbl, const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);

void genNSinePulse(sample_res *bfr, paramtable *ptbl,  const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);

void genNSinePulse2(sample_res *bfr, paramtable *ptbl,  const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);

void genNSinePulseHybrid(sample_res *bfr, paramtable *ptbl,  const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);

void genNHalfSine(sample_res *bfr, paramtable *ptbl,  const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);

void genNHalfSinePulse(sample_res *bfr, paramtable *ptbl,  const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//NOISE WAVES||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void genSilence(sample_res *bfr, paramtable *ptbl, const float &period, const sample_res_unsigned &height, float &phase, const unsigned long &len);

void genNoise_White(sample_res *bfr, paramtable *ptbl, const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);
void genNoise0(sample_res *bfr, paramtable *ptbl, const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);
void genNoise1(sample_res *bfr, paramtable *ptbl, const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);
void genNoise2(sample_res *bfr, paramtable *ptbl, const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);
void genNoise3(sample_res *bfr, paramtable *ptbl, const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);



void genBongo(sample_res *bfr, paramtable *ptbl, const float &period, const sample_res_unsigned &height, float &phase, const unsigned long &len);

void genMuxShared(sample_res *bfr, paramtable *ptbl, const float &period, const sample_res_unsigned &height, float &phase, const unsigned long &len);
void genMuxSwap(sample_res *bfr, paramtable *ptbl, const float &period, const sample_res_unsigned &height, float &phase, const unsigned long &len);
void genMuxSwap2(sample_res *bfr, paramtable *ptbl, const float &period, const sample_res_unsigned &height, float &phase, const unsigned long &len);
void genMuxPCTRL(sample_res *bfr, paramtable *ptbl, const float &period, const sample_res_unsigned &height, float &phase, const unsigned long &len);
void genMuxP2CTRL(sample_res *bfr, paramtable *ptbl, const float &period, const sample_res_unsigned &height, float &phase, const unsigned long &len);
void genMuxHPCTRL(sample_res *bfr, paramtable *ptbl, const float &period, const sample_res_unsigned &height, float &phase, const unsigned long &len);

void genWavePiecewise(sample_res *bfr, paramtable *ptbl,  const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);
void genWavePiecewisePCTRL(sample_res *bfr, paramtable *ptbl,  const float &period, const sample_res_unsigned &amplitude, float &phase, const unsigned long &len);
void genBlacmange(sample_res *bfr, paramtable *ptbl, const float &period, const sample_res_unsigned &height, float &phase, const unsigned long &len);

#endif
