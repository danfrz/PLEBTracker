#include "generator.h"


generator GEN_MUX1 = genSilence;
generator GEN_MUX2 = genSilence;

void genSaw(unsigned char *bfr, unsigned char *ptbl, const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{
    if(period == 0 || amplitude == 0)
        return;

    unsigned char halfamp = amplitude/2;
    float acc = amplitude / period;

    unsigned long prd = period;
    unsigned long shift = phase*period;
    unsigned long ovr;
    unsigned long i;
    for(i = 0; i < len; i++)
    {
        ovr = (i+shift)%prd;
        bfr[i] += ovr*acc-halfamp;
    }
    phase = ((i + shift) % prd) / period;
}

void genSawPulse(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude,  float &phase,  const unsigned long &len)
{
    if(period == 0 || amplitude == 0)
        return;

    long prd = period;
    float halfamp = amplitude/2.0;
    //ratio*(amp / period)
    //(1 - p)*(amp / period)
    //(period - 0) = period
    //(period - 1*(period-1)) = 1
    //
    //(period - ratio*(period-1))*(amp / period)
    //Sounds good. But doesn't "iterate" fast enough
    //Need to find a transform that takes PULSE/0xFFFF to give more weight on
    //higher values. Bell curve?
    float prdmod = period/1;
    float ratio = prdmod - ((float)(((unsigned short*)ptbl)[PARAM_PULSE]) / 0xFFFF)*(prdmod-1);
    float acc = ratio*(amplitude / period);
    //unsigned long lowlen = len - (len % long(period));
    unsigned long increments = std::ceil(period);

    unsigned long shift = phase*period;
    unsigned long ovr;
    unsigned long i;
    for(i = 0; i < len; i++)
    {
        ovr = ((i+shift)%prd)*acc;

        if(ovr <= amplitude)
            bfr[i] += ovr - halfamp;

        //if(ovr > halfamp)
        //    ovr = halfamp;
        //bfr[i] += ovr-halfamp;
    }
    

    phase = ((i + shift) % prd) / period;
}


void genSqr(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
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


//Creates a Square wave where the center median shifts
void genSqrPulse2(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{
    if(period == 0 || amplitude == 0)
        return;


    unsigned long offset = phase*period;
    unsigned long lowlen = len;
    const long prd = period;
    float ratio = (float)(((unsigned short*)ptbl)[PARAM_PULSE]) / 0xFFFF;

    unsigned long i;
    for(i = 0; i < lowlen; i++)
    {
        bfr[i] += ((((i+offset) % prd) < (period*ratio)) - 0.5f)*amplitude;
    }
    phase = ((i+offset)%prd) / period;
}

//Creates a Square wave where the whole wave is scaled and silence is left for the rest of the period
void genSqrPulse(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{
    if(period == 0 || amplitude == 0)
        return;


    unsigned long offset = phase*period;
    unsigned long lowlen = len;
    const long prd = period;
    float ratio = ((float)(((unsigned short*)ptbl)[PARAM_PULSE])+1) / 0x10000;
    

    int roll;
    unsigned long i;
    for(i = 0; i < lowlen; i++)
    {
        roll = static_cast<unsigned long>((i+offset)) % prd;

        if(roll < ratio*period)
            bfr[i] += ((roll < ((period*ratio)/2)) - 0.5f)*amplitude;
    }

    phase = ((i+offset)%prd) / period;
}


//Creates a Square wave where the center median shifts
void genSqrPulseHybrid(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{
    if(period == 0 || amplitude == 0)
        return;


    unsigned long offset = phase*period;
    unsigned long lowlen = len;
    const long prd = period;
    float ratio2 = ((float)(((unsigned short*)ptbl)[PARAM_PULSE])+1) / 0x10000;
    float ratio = (float)(((unsigned short*)ptbl)[PARAM_PULSE2]) / 0xFFFF;
    

    int roll;
    unsigned long i;
    for(i = 0; i < lowlen; i++)
    {
        roll = static_cast<unsigned long>((i+offset)) % prd;

        if(roll < ratio2*period)
            bfr[i] += ((roll < ((period*ratio2*ratio))) - 0.5f)*amplitude;
    }

    phase = ((i+offset)%prd) / period;
}



void genSqrNES(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{
    if(period == 0 || amplitude == 0)
        return;

    long offset = phase*period;

    //unsigned long lowlen = len - (len % long(period));
    
    float strength = ptbl[PARAM_WAVE1] / 255.f;
    //Offset the length and initial phase by the phase
    const long prd = period;
    long saw;
    float wave;
    unsigned long i;
    for(i = 0; i < len; i++)
    {
        saw = (i+offset) % prd;
        wave = ((saw < (period/2.0f))-0.5f)*amplitude;
        wave *= 1.0f - (saw / period)*strength;
        bfr[i] += wave;
    }
    phase = ((i+offset)%prd) / period;
}


void genSqrPulseNES(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{
    if(period == 0 || amplitude == 0)
        return;

    float strength = ptbl[PARAM_WAVE1] / 256.f;
    unsigned long offset = phase*period;
    unsigned long lowlen = len;
    const long prd = period;
    long saw;
    float wave;
    float ratio = (float)(((unsigned short*)ptbl)[PARAM_PULSE]) / 0xFFFF;

    unsigned long i;
    for(i = 0; i < lowlen; i++)
    {//every 8 period 
        saw = (i + offset) % prd;
        wave = ((saw < (period*ratio)) - 0.5f)*amplitude;
        wave *= 1.0f - (saw / period)*strength;
        bfr[i] += wave;
        
    }
    phase = ((i+offset)%prd) / period;
}


void genSqrVTX(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{
    //Doesn't work how I want to yet.
    //Was trying to make it emulate the square in vortex tracker
    if(period == 0 || amplitude == 0)
        return;

    long offset = phase*period;

    //unsigned long lowlen = len - (len % long(period));
    
    float strength = ptbl[PARAM_WAVE1] / 255.f;
    //Offset the length and initial phase by the phase
    const long prd = period;
    long saw;
    float wave;
    unsigned long i;
    for(i = 0; i < len; i++)
    {
        saw = (i+offset) % prd;
        wave = ((saw < (period/2.0f))-0.5f)*amplitude;
        wave *= 1.0f - (saw / period)*std::pow(strength,1+2.f*saw/period);
        bfr[i] += wave;
    }
    phase = ((i+offset)%prd) / period;
}


void genSqrPulseVTX(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{
    //Doesn't work how I want to yet.
    //Was trying to make it emulate the square in vortex tracker
    if(period == 0 || amplitude == 0)
        return;

    float strength = ptbl[PARAM_WAVE1] / 255.f;
    unsigned long offset = phase*period;
    unsigned long lowlen = len;
    const long prd = period;
    long saw;
    float wave;
    float ratio = (float)(((unsigned short*)ptbl)[PARAM_PULSE]) / 0xFFFF;

    unsigned long i;
    for(i = 0; i < lowlen; i++)
    {//every 8 period 
        saw = (i + offset) % prd;
        wave = ((saw < (period*ratio)) - 0.5f)*amplitude;
        wave *= (1.0f - std::pow(saw,1.25) / std::pow(period,1.25))*strength;
        bfr[i] += wave;
        
    }
    phase = ((i+offset)%prd) / period;
}



void genTri(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{
    if(period == 0 || amplitude == 0)
        return;

    unsigned long shift = phase*period;

    unsigned long prd = period;
    float halfprd = prd/2;
    unsigned long fourthprd = prd/4;

    unsigned long ovr;
    unsigned long i;
    for(i = 0; i < len; i++)
    {
        ovr = (i+shift)%prd;
        ptbl[PARAM_LAST] = static_cast<float>(amplitude)*(fourthprd - std::abs(halfprd-ovr))/halfprd;
        bfr[i] += ptbl[PARAM_LAST] ;
        
    }
    phase = ((i+shift)%prd) / period;
}
 

void genTriPulse(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{
    if(period == 0 || amplitude == 0)
        return;

    unsigned long shift = phase*period;
    float halfamp = amplitude/2.0;

    unsigned short param = ((unsigned short*)ptbl)[0];
    float ratio = 4 - 3.0*(static_cast<float>(param) / 0xFFFF);

    unsigned long prd = period;
    float halfprd = prd/2;
    unsigned long fourthprd = prd/4;

    unsigned long ovr;
    unsigned long i;
    for(i = 0; i < len; i++)
    {
        ovr = (i+shift)%prd;
        if(ovr < period/ratio)
        {
            ptbl[PARAM_LAST] = amplitude*ratio*(fourthprd/ratio - std::abs(halfprd/ratio-ovr))/halfprd;
        }
        else
        {
            if(std::abs(char(ptbl[PARAM_LAST]) - halfamp) > 3)
            {
                int interp = (3*int((char)ptbl[PARAM_LAST]) - halfamp) / 4;
                ptbl[PARAM_LAST] = interp;

            }
        }
        bfr[i] += ptbl[PARAM_LAST];
    }

    phase = ((i+shift)%prd) / period;
}

void genSine(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
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


void genSinePulse(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{
    if(period == 0 || amplitude == 0)
        return;
    float ratio = ((float)(((unsigned short*)ptbl)[PARAM_PULSE])+1.0) / 0x10000 ;

    const float tau = (2*M_PI);

    float temp;
    float halfamp = amplitude/2.0;
    unsigned long i;
    for(i = 0; i < len; i++)
    {
        temp = fmod(phase + (tau*i)/period, tau);
        if(temp < tau*ratio)
            bfr[i] += halfamp*std::sin(temp/ratio);
    }

    temp = fmod(phase + (tau*i)/period, tau);
    phase = phase + (tau*i)/period;
}



void genSinePulse2(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{
    if(period == 0 || amplitude == 0)
        return;
    float trueratio = ((float)(((unsigned short*)ptbl)[PARAM_PULSE])+1.0) / 0x10000;


    float ratio; //ratio that modifies the upper sine
    float invratio; //ratio that modifies the lower sine

    if(trueratio <= 0.5)
    {
        ratio = trueratio*2;                //0 to 1
        invratio = 1.0 + (1.0 - ratio); //2 to 1
    }
    else
    {
        ratio = 1.0 + (trueratio - 0.5)*2; //1 to 2
        invratio = 1.0 - (trueratio-0.5)*2;//1 to 0
    }

    const float tau = (2*M_PI);

    float temp;
    float halfamp = amplitude/2.0;
    unsigned long i;
    for(i = 0; i < len; i++)
    {
        temp = fmod(phase + (tau*i)/period, tau);
        if(temp < M_PI*ratio)
            bfr[i] += halfamp*std::sin(temp/(ratio));
        else
            bfr[i] += halfamp*std::sin((temp-M_PI*ratio + M_PI*invratio)/(invratio));

    }

    temp = fmod(phase + (tau*i)/period, tau);
    phase = phase + (tau*i)/period;
}

//First step on the way to making genSinePulse2, these steps are going to stay
//here because some of them sounded kind of cool in their own right.
//I'll decide which ones stay later, if any.
void genSinePulse2_A(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{
    if(period == 0 || amplitude == 0)
        return;
    float ratio = ((float)(((unsigned short*)ptbl)[PARAM_PULSE])+1.0) / 0x10000 ;

    const float tau = (2*M_PI);

    float temp;
    float halfamp = amplitude/2.0;
    unsigned long i;
    for(i = 0; i < len; i++)
    {
        temp = fmod(phase + (tau*i)/period, tau);
        if(temp < tau*ratio/2.0)
            bfr[i] += halfamp*std::sin(temp/ratio);
        else
            bfr[i] += halfamp*std::sin((temp)/((1-ratio)*2.0));
    }

    temp = fmod(phase + (tau*i)/period, tau);
    phase = phase + (tau*i)/period;
}

//Second step on the way to making genSinePulse2
void genSinePulse2_B(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{
    if(period == 0 || amplitude == 0)
        return;
    float ratio = ((float)(((unsigned short*)ptbl)[PARAM_PULSE])+1.0) / 0x10000 ;

    const float tau = (2*M_PI);

    float temp;
    float halfamp = amplitude/2.0;
    unsigned long i;
    for(i = 0; i < len; i++)
    {
        temp = fmod(phase + (tau*i)/period, tau);
        if(temp < tau*ratio/2.0)
            bfr[i] += halfamp*std::sin(temp/ratio);
        else
            bfr[i] += halfamp*std::sin((temp-M_PI*ratio + M_PI*(1-ratio)*2)/((1-ratio)*2.0));
    }

    temp = fmod(phase + (tau*i)/period, tau);
    phase = phase + (tau*i)/period;
}

//Third step on the way to making genSinePulse2
void genSinePulse2_C(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{
    if(period == 0 || amplitude == 0)
        return;
    float ratio = ((float)(((unsigned short*)ptbl)[PARAM_PULSE])+1.0) / 0x10000 ;
    float invratio = 1.0 - ratio;

    const float tau = (2*M_PI);

    float temp;
    float halfamp = amplitude/2.0;
    unsigned long i;
    for(i = 0; i < len; i++)
    {
        temp = fmod(phase + (tau*i)/period, tau);
        if(temp < tau*ratio)
            bfr[i] += halfamp*std::sin(temp/ratio);
        else
            bfr[i] += halfamp*std::sin((temp-M_PI*ratio + M_PI*invratio*2)/(invratio*2.0));
    }

    temp = fmod(phase + (tau*i)/period, tau);
    phase = phase + (tau*i)/period;
}

//Fourth step on the way to making genSinePulse2
void genSinePulse2_D(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{
    if(period == 0 || amplitude == 0)
        return;
    float ratio = ((float)(((unsigned short*)ptbl)[PARAM_PULSE])+1.0) / 0x10000 ;
    float invratio = 0.5 + (1.0 - ratio)*0.5;

    const float tau = (2*M_PI);

    float temp;
    float halfamp = amplitude/2.0;
    unsigned long i;
    for(i = 0; i < len; i++)
    {
        temp = fmod(phase + (tau*i)/period, tau);
        if(temp < tau*ratio/2)
            bfr[i] += halfamp*std::sin(temp/ratio);
        else
            bfr[i] += halfamp*std::sin((temp-M_PI*ratio + M_PI*invratio*2)/(invratio*2.0));
    }

    temp = fmod(phase + (tau*i)/period, tau);
    phase = phase + (tau*i)/period;
}

void genSinePulseHybrid(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{

    if(period == 0 || amplitude == 0)
        return;
    //Pulse2 ratio
    float trueratio = ((float)(((unsigned short*)ptbl)[PARAM_PULSE2])+1.0) / 0x10000;
    //Pulse1 ratio
    float p1ratio = ((float)(((unsigned short*)ptbl)[PARAM_PULSE])+1.0) / 0x10000;


    float ratio; //ratio that modifies the upper sine
    float invratio; //ratio that modifies the lower sine

    if(trueratio <= 0.5)
    {
        ratio = trueratio*2;                //0 to 1
        invratio = 1.0 + (1.0 - ratio); //2 to 1
    }
    else
    {
        ratio = 1.0 + (trueratio - 0.5)*2; //1 to 2
        invratio = 1.0 - (trueratio-0.5)*2;//1 to 0
    }

    const float tau = (2*M_PI);

    float temp;
    float halfamp = amplitude/2.0;
    unsigned long i;
    for(i = 0; i < len; i++)
    {
        temp = fmod(phase + (tau*i)/period, tau);
        if(temp < tau*p1ratio)
        {
            if(temp < (M_PI*ratio*p1ratio))
                bfr[i] += halfamp*std::sin(temp/(p1ratio*ratio));
            else
                bfr[i] += halfamp*std::sin((temp-M_PI*p1ratio*ratio + M_PI*p1ratio*invratio)/(p1ratio*invratio));
        }

    }

    temp = fmod(phase + (tau*i)/period, tau);
    phase = phase + (tau*i)/period;

}


void genNSine(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{
    if(period == 0 || amplitude == 0)
        return;


    float n = ptbl[PARAM_WAVE1]; //Wave Param

    const float tau = (2*M_PI);

    float temp;
    float halfamp = amplitude/2.0;
    unsigned long i;
    for(i = 0; i < len; i++)
    {
        temp = fmod(phase + (tau*i)/period, tau);
        bfr[i] += halfamp*std::sin(temp*n);
    }

    temp = fmod(phase + (tau*i)/period, tau);
    phase = phase + (tau*i)/period;
}


void genNSinePulse(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{
    if(period == 0 || amplitude == 0)
        return;
    float ratio = ((float)(((unsigned short*)ptbl)[PARAM_PULSE])+1) / 0x10000 ;
    float n = ptbl[PARAM_WAVE1]; //Wave Param

    const float tau = (2*M_PI);

    float temp;
    float halfamp = amplitude/2.0;
    unsigned long i;
    for(i = 0; i < len; i++)
    {
        temp = fmod(phase + (tau*i)/period, tau);
        if(temp < tau*ratio)
            bfr[i] += halfamp*std::sin(temp/ratio*n);
    }

    temp = fmod(phase + (tau*i)/period, tau);
    phase = phase + (tau*i)/period;
}

void genNSinePulse2(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{
    if(period == 0 || amplitude == 0)
        return;

    float trueratio = ((float)(((unsigned short*)ptbl)[PARAM_PULSE])+1.0) / 0x10000;


    float ratio; //ratio that modifies the upper sine
    float invratio; //ratio that modifies the lower sine

    if(trueratio <= 0.5)
    {
        ratio = trueratio*2;                //0 to 1
        invratio = 1.0 + (1.0 - ratio); //2 to 1
    }
    else
    {
        ratio = 1.0 + (trueratio - 0.5)*2; //1 to 2
        invratio = 1.0 - (trueratio-0.5)*2;//1 to 0
    }

    float n = ptbl[PARAM_WAVE1]; //Wave Param

    const float tau = (2*M_PI);

    float temp;
    float halfamp = amplitude/2.0;
    unsigned long i;
    for(i = 0; i < len; i++)
    {
        temp = fmod(phase + (tau*i)/period, tau);
        //bfr[i] += halfamp*std::sin(temp/ratio*n);

        if(temp < tau*ratio/2)
            bfr[i] += halfamp*std::sin(temp/(ratio)*n);
        else
            bfr[i] += halfamp*std::sin((temp-M_PI*ratio + M_PI*invratio)/(invratio)*n);
    }

    temp = fmod(phase + (tau*i)/period, tau);
    phase = phase + (tau*i)/period;
}

void genNSinePulseHybrid(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{

    if(period == 0 || amplitude == 0)
        return;

    float trueratio = ((float)(((unsigned short*)ptbl)[PARAM_PULSE2])+1.0) / 0x10000;
    float p1ratio = ((float)(((unsigned short*)ptbl)[PARAM_PULSE])+1) / 0x10000 ;


    float ratio; //ratio that modifies the upper sine
    float invratio; //ratio that modifies the lower sine

    if(trueratio <= 0.5)
    {
        ratio = trueratio*2;                //0 to 1
        invratio = 1.0 + (1.0 - ratio); //2 to 1
    }
    else
    {
        ratio = 1.0 + (trueratio - 0.5)*2; //1 to 2
        invratio = 1.0 - (trueratio-0.5)*2;//1 to 0
    }

    float n = ptbl[PARAM_WAVE1]; //Wave Param

    const float tau = (2*M_PI);

    float temp;
    float halfamp = amplitude/2.0;
    unsigned long i;
    for(i = 0; i < len; i++)
    {
        temp = fmod(phase + (tau*i)/period, tau);
        //bfr[i] += halfamp*std::sin(temp/ratio*n);
        if(temp < tau*ratio)
            if(temp < (tau*ratio*p1ratio)/2)
                bfr[i] += halfamp*std::sin(temp/(p1ratio*ratio)*n);
            else
                bfr[i] += halfamp*std::sin((temp-M_PI*p1ratio*ratio + M_PI*p1ratio*invratio)/(p1ratio*invratio)*n);
    }

    temp = fmod(phase + (tau*i)/period, tau);
    phase = phase + (tau*i)/period;


}

void genHalfSine(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{
    if(period == 0 || amplitude == 0)
        return;

    const float tau = (2*M_PI);
    float temp;
    unsigned long i;

    for(i = 0; i < len; i++)
    {
        temp = fmod(phase + (tau*i)/period, tau);
        bfr[i] += amplitude*(std::abs(std::sin(temp/2.0)) - 0.5);
    }


    phase = phase + (tau*i)/period;
}

void genHalfSinePulse(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{
    if(period == 0 || amplitude == 0)
        return;
    float ratio = ((float)(((unsigned short*)ptbl)[PARAM_PULSE])+1) / 0x10000 ;

    float temp;
    const float tau = (2*M_PI);
    unsigned long i;
    for(i = 0; i < len; i++)
    {
        temp = fmod(phase + (tau*i)/period, tau);
        if(temp < tau*ratio)
            bfr[i] += amplitude*(std::abs(std::sin(temp/ratio/2.0)) - 0.5);
        else bfr[i] -=amplitude/2.0;
    }
    phase = phase + (tau*i)/period;
}


void genNHalfSine(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{
    if(period == 0 || amplitude == 0)
        return;

    float n = ptbl[PARAM_WAVE1] / 2.0; //Wave Param
    float temp;
    const float tau = (2*M_PI);
    unsigned long i;
    for(i = 0; i < len; i++)
    {
        temp = fmod(phase + (tau*i)/period, tau);
        bfr[i] += amplitude*(std::abs(std::sin(temp*n)) - 0.5);
    }
    phase = phase + (tau*i)/period;
}

void genNHalfSinePulse(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{
    if(period == 0 || amplitude == 0)
        return;
    float ratio = ((float)(((unsigned short*)ptbl)[PARAM_PULSE])+1) / 0x10000 ;
    float n = ptbl[PARAM_WAVE1] / 2.0; //Wave Param

    float temp;
    const float tau = (2*M_PI);
    unsigned long i;
    for(i = 0; i < len; i++)
    {
        temp = fmod(phase + (tau*i)/period, tau);
        if(temp < tau*ratio)
            bfr[i] += amplitude*(std::abs(std::sin(temp/ratio*n)) - 0.5);
        else bfr[i] -=amplitude/2.0;
    }
    phase = phase + (tau*i)/period;
}


void genSilence(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &height, float &phase, const unsigned long &len)
{
    return;
}

void genBongo(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &height, float &phase, const unsigned long &len)
{
    if(period == 0 || height == 0)
        return;

    unsigned long lowlen = len;
    //unsigned long lowlen = len - (len % long(period));
    float tau = (2*M_PI);
    float ratio = 14.0 /64.0;
    unsigned long i;
    for(i = 0; i < lowlen; i++)
    {
        float f = std::sin(phase+(tau*i)/period) * std::sin(phase/ratio + (tau*i)/(period*ratio))*0.5;
        bfr[i] += f*height;
    }
    phase = phase + (tau*i)/period;
}


void genNoise_White(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &height, float &phase, const unsigned long &len)
{
    if(height == 0)
        return;
    int halfamp = height/2;
    //Note, not actually white noise, for the record
    for(unsigned long i = 0; i < len; i++)
        bfr[i] += (rand() % height) - halfamp;
}

void genNoise0(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &height, float &phase, const unsigned long &len)
{
    if(height == 0)
        return;
    int halfamp = height/2;
    float x = 0;
    for(unsigned long i = 0; i < len;)
    {
        bfr[i] += (rand() % height) - halfamp;

        if(period < 1)
            x++;
        else
        {
            x += period;
            i = x;
        }
    }
}

void genNoise1(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &height, float &phase, const unsigned long &len)
{
    if(height == 0)
        return;
    if(period < 2)
    {
        genNoise_White(bfr, ptbl, period, height, phase, len);
        return;
    }

    unsigned long prd = period;
    int halfamp = height/2;
    char last = phase;
    for(unsigned long i = 0; i < len; i++)
    {
        if((i % prd) == 0)
            last =  ((rand()) % height) - halfamp;
        bfr[i] += last;
        
    }
    phase = last;
}


void genNoise2(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &height, float &phase, const unsigned long &len)
{
    if(height == 0)
        return;
    if(period < 2)
    {
        genNoise_White(bfr, ptbl, period, height, phase, len);
        return;
    }
    unsigned long prd = period;
    int halfamp = height/2;

    //Generate a waveform seeded by the wave param
    srand((((unsigned short*)ptbl)[PARAM_PULSE]));
    char timbre[prd];
    char last;
    for(int i = 0; i < prd; i++)
    {
        timbre[i] = (rand() % height) - halfamp;
    }

    last = phase;
    for(unsigned long i = 0; i < len; i++)
    {
        
        if(last >= prd)
            last = 0;
        bfr[i] += timbre[last];
        if((i %(prd/8)) == 0)
            last++;
        
    }
    phase = last;
}

void genNoise3(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &height, float &phase, const unsigned long &len)
{
    if(height == 0)
        return;
    if(period < 2)
    {
        genNoise_White(bfr, ptbl, period, height, phase, len);
        return;
    }
    //I wanted this to be cool but it doesn't actually sound that different.
    //On a different note however this generator is basically
    //a Noise wave with a rudimentary low pass filter
    
    unsigned long prd = period;
    int halfamp = height/2;
    unsigned short depth = ptbl[PARAM_WAVE1]+1;

    //Generate a waveform seeded by pulse param
    srand((((unsigned short*)ptbl)[PARAM_PULSE])+1);
    int timbre[prd];
    char last;
    for(int i = 0; i < prd; i++)
        timbre[i] = (rand() % height) - halfamp;
    for(int j = 1; j < depth; j++)
    {
        srand((((unsigned short*)ptbl)[PARAM_PULSE]) - j);
        for(int i = 0; i < prd; i++)
        timbre[i] += (rand() % height) - halfamp;
    }
    for(int i = 0; i < prd; i++)
        timbre[i] /= depth;
    

    last = phase;
    for(unsigned long i = 0; i < len; i++)
    {
        
        if(last >= prd)
            last = 0;
        bfr[i] += timbre[last];
        if((i %(prd/8)) == 0)
            last++;
        
    }
    phase = last;
}


//I don't know how to implement these
void genNoise_Brown(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &height, float &phase, const unsigned long &len)
{


}

void genNoise_Pink(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &height, float &phase, const unsigned long &len)
{
}

void genNoise_Blue(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &height, float &phase, const unsigned long &len)
{
}



void genNoise_Test(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &height, float &phase, const unsigned long &len)
{

}

void swapPulseParams(unsigned char *ptbl)
{
    unsigned short swap = (((unsigned short*)ptbl)[PARAM_PULSE]);
    (((unsigned short*)ptbl)[PARAM_PULSE]) = (((unsigned short*)ptbl)[PARAM_PULSE2]);
    (((unsigned short*)ptbl)[PARAM_PULSE2]) = swap;
}

void genMuxShared(unsigned char *bfr, unsigned char *ptbl, const float &period, const unsigned char &height, float &phase, const unsigned long &len)
{
    float diff = (ptbl[PARAM_WAVE1]/255.0) * period;
    float ampratio = (ptbl[PARAM_WAVE2]/255.0);
    float phase2 = phase + diff;
    GEN_MUX1(bfr, ptbl, period, height*ampratio, phase, len);
    GEN_MUX2(bfr, ptbl, period, height*(1.0-ampratio), phase2, len);

}

void genMuxSwap(unsigned char *bfr, unsigned char *ptbl, const float &period, const unsigned char &height, float &phase, const unsigned long &len)
{
    float diff = (ptbl[PARAM_WAVE1]/255.0) * period;
    float ampratio = (ptbl[PARAM_WAVE2]/255.0);
    float phase2 = phase + diff;
    GEN_MUX1(bfr, ptbl, period, height*ampratio, phase, len);
    swapPulseParams(ptbl);
    GEN_MUX2(bfr, ptbl, period, height*(1.0-ampratio), phase2, len);
    swapPulseParams(ptbl);

}
void genMuxSwap2(unsigned char *bfr, unsigned char *ptbl, const float &period, const unsigned char &height, float &phase, const unsigned long &len)
{
    float diff = (ptbl[PARAM_WAVE1]/255.0) * period;
    float ampratio = (ptbl[PARAM_WAVE2]/255.0);
    float phase2 = phase + diff;
    swapPulseParams(ptbl);
    GEN_MUX1(bfr, ptbl, period, height*ampratio, phase, len);
    swapPulseParams(ptbl);
    GEN_MUX2(bfr, ptbl, period, height*(1.0-ampratio), phase2, len);

}

void genBMrecurse(unsigned char *bfr, unsigned char *ptbl, const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len, int depth)
{
    if(period == 0 || amplitude == 0)
        return;
    float start_phase = phase/pow(2,depth);
    genTri(bfr, ptbl, period, amplitude, phase, len);
    float end_phase = phase;
    
    if(depth < ptbl[PARAM_WAVE1])
        genBMrecurse(bfr, ptbl, period/2.0, amplitude/2.0, start_phase, len, depth+1);

    if(depth == 0)
        phase = end_phase;
}

void genBlacmange(unsigned char *bfr, unsigned char *ptbl,  const float &period, const unsigned char &amplitude, float &phase, const unsigned long &len)
{
    if(period == 0 || amplitude == 0)
        return;
    
    genBMrecurse(bfr,ptbl,period,amplitude,phase,len, 0);
}

