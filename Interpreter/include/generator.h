#ifndef GENERATOR_H_
#define GENERATOR_H_
#include <cmath>
#include <random>

#define PARAM_PULSE 0
#define PARAM_CUSTOM_JUMP 2
#define PARAM_LOOP 4
#define PARAM_WAVE1 5
#define PARAM_WAVE2 6
#define PARAM_CHAIN 7
#define PARAM_LAST 8

//Form of a wave generator function 
typedef void (*generator)(unsigned char *bfr, unsigned char *ptbl, const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//SAW WAVES||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void genSaw(unsigned char *bfr, unsigned char *ptbl, const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);

void genSawPulse(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);




//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//SQUARE WAVES|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

/***\//////////////////////////////////////////////////////////////////////////    
Function: void genSqr(uchar *bfr, uint period, uint amplitude, const ulong &len

Description:
   Populate a buffer with a square wave
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
void genSqr(unsigned char *bfr, unsigned char *ptbl, const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);

void genSqrPulse(unsigned char *bfr,  unsigned char *ptbl,const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);

void genSqrPulse2(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);

void genSqrPulseHybrid(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);

void genSqrNES(unsigned char *bfr, unsigned char *ptbl, const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);

void genSqrPulseNES(unsigned char *bfr,  unsigned char *ptbl,const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);

void genSqrVTX(unsigned char *bfr, unsigned char *ptbl, const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);

void genSqrPulseVTX(unsigned char *bfr, unsigned char *ptbl, const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//TRIANGLE WAVES|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


/***\//////////////////////////////////////////////////////////////////////////    
Function: void genTri(uchar *bfr, uint period, uint amplitude, const ulong &len

Description:
   Populate a buffer with a triangle wave
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
void genTri(unsigned char *bfr, unsigned char *ptbl, const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);

void genTriPulse(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);

void genTriPulse2(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);

void genTriPulseHybrid(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//SINE WAVES|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void genSine(unsigned char *bfr, unsigned char *ptbl, const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);

void genSinePulse(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);

void genSinePulse2(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);

void genSinePulseHybrid(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);

void genHalfSine(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);

void genHalfSinePulse(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);

void genNSine(unsigned char *bfr, unsigned char *ptbl, const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);

void genNSinePulse(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);

void genNSinePulse2(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);

void genNSinePulseHybrid(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);

void genNHalfSine(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);

void genNHalfSinePulse(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//NOISE WAVES||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void genSilence(unsigned char *bfr, unsigned char *ptbl, const float &period, const unsigned char &height, float &phase, const unsigned long &len);

void genNoise_White(unsigned char *bfr, unsigned char *ptbl, const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);
void genNoise0(unsigned char *bfr, unsigned char *ptbl, const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);
void genNoise1(unsigned char *bfr, unsigned char *ptbl, const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);
void genNoise2(unsigned char *bfr, unsigned char *ptbl, const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);
void genNoise3(unsigned char *bfr, unsigned char *ptbl, const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len);



void genBongo(unsigned char *bfr, unsigned char *ptbl, const float &period, const unsigned char &height, float &phase, const unsigned long &len);


#endif
