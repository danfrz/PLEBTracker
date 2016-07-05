#include "interpreter.h"
#include <iostream>



double logbase(double base, double val)
{
    //calculate a value to the log of base _base_
    double b = log2(base);
    if(b == 0)
    {
        std::cerr << "Log base(" << base << ") evaluated " << b << "\n";
        throw 10; 
    }
    return log2(val) / b;

}

/***\//////////////////////////////////////////////////////////////////////////    
Function: void printByte(uchar &b, const char &character, const uchar &res

Description:
   Prints a graphic for the height of a single byte to cout
   using the specified character.
   Each character represents 'res' (resolution) units.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
void itrp::printByte(unsigned char &b, const char &character, const unsigned char &res)
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
   Prints a graphic for an entire byte buffer to the stdout
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
void itrp::printBuffer(unsigned char *bfr, const unsigned long &len)
{
    for(unsigned long i = 0; i < len; i++)
        printByte(bfr[i], '#', 4);
}


/***\//////////////////////////////////////////////////////////////////////////    
Function: void printWaveTable(unsigned short *tbl, unsigned short entries

Description:
   Prints the wave table loaded from the song to stderr
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
void printWaveTable(unsigned short *tbl, unsigned short entries)
{
    for(unsigned int i = 0; i < entries; i++)
        std::cerr << std::hex << tbl[i] << '\n';

}

void itrp::play(unsigned char **buffer, const unsigned int orders, unsigned int *bytes)
{
    //std::cerr << "playing\n";
    
    //Play each of the orders in the song
    //buffer stores all of the byte buffers for each order.
    for(unsigned int orderi = 0; orderi < orders; orderi++)
    {
        
        unsigned char *order = buffer[orderi];
        //std::cerr << "rows=" << int(curpattern->numRows()) << '\n';
        //std::cerr << "playing buffer " << orderi << ' ' << bytes[orderi] << " bytes\n";

        //Export each of the bytes from the byte buffers to stdout
        for(unsigned int i = 0; i < bytes[orderi]; i++)
        {
            putchar(order[i]);
            //std::cerr << order[i];
        }
        //std::cerr << "gothere "<< '\n';
        
    }

}

void itrp::print(unsigned char **buffer, const unsigned int orders, unsigned int *bytes)
{
    //std::cerr << "printing\n";

    //Prints a graphic for an entires song

    for(unsigned int orderi = 0; orderi < orders; orderi++)
    {
        
        unsigned char *order = buffer[orderi];
        curpattern = song->getPatternByOrder(orderi);
        //std::cerr << int(curpattern->numRows()) << " ? " << song->getBytesPerRow() <<'\n';
        //std::cerr << "playing order " << orderi << ' ' << bytes << " bytes\n";

        printBuffer(order, bytes[orderi]);
        //for(unsigned int i = 0; i < bytes; i++)
        //    std::cerr << int(order[i]) << ' 
        
    }

}


void itrp::play(unsigned char *buffer, unsigned int bytes)
{
    //Output all of the data from one byte buffer to stdout
    for(unsigned int i = 0; i < bytes; i++)
        putchar(buffer[i]);
}


void itrp::renderTick(unsigned char *buffer, const unsigned char &track, const unsigned int &bytes)
{
    //std::cerr << "renderTick0\n";
    Track *seltrk = &tracks[track];
    //std::cerr << "renderTick1 voli=" << std::hex << int(seltrk->voli) << " volduracc=" << int(seltrk->volduracc) << " lastvol=" << int(seltrk->lastvol) <<  "\n";
    if(seltrk->inst == NULL)
        return;

    //defer to Instrument::getVolume(...) for determining instrument volume at
    //this point in time
    unsigned char amp = seltrk->inst->getVolume(seltrk->voli, seltrk->volduracc, seltrk->voljump, seltrk->lastvol);
    seltrk->volduracc++;

    //std::cerr << "renderTick2 getVolume=" << int(amp) << " ptrnvol=" << int(seltrk->ptrnvol) <<"\n";

    
    float frq = seltrk->frq;
    unsigned char depth = 0;//this counter is used to make sure the user doesn't
                            //make the interpreter get stuck in an infinite
                            //loop of wave functions.

    ///////////////////////////////////////////////////////
    ///// WAVE TABLE FUNCTION IMPLEMENTATION///////////////
    ///////////////////////////////////////////////////////

    //the wave entry
    unsigned short _wav = song->getWaveEntry(seltrk->wavei);

    //Check if the wave entry is a function
    if((_wav & 0xF000) == 0xF000)
    {
        //Wave functions don't produce a sound and do not continue the wave loop
        //so this loop will process all wave functions until the next
        //non-function wave table entry
        while((_wav & 0xF000) == 0xF000)
        {
            depth++;
            if(depth > 64)
            {
                std::cerr << "Oh dear! Infinite loop! Waveform set to silence.\n";
                seltrk->wavei--;
                _wav = 0;//silence
                break;
            }

            //Get which wave function it is
            unsigned char cmd = (_wav & 0x0F00) >> 8;
            if(cmd == 0xF) //FF, JUMP
            {
                //If there is a Jump instruction immediately following this wave
                //entry then concatenate the destinations of these wave entries
                if(seltrk->wavei < 0xFFFF) //edge case
                {
                    unsigned short nxtwav = song->getWaveEntry(seltrk->wavei + 1);
                    if((nxtwav & 0xFF00) == 0xFF00)
                    {
                        //Next entry IS the FF function, so concat the
                        //destinations
                        seltrk->wavei = ((_wav & 0x00FF) << 8) | (nxtwav & 0x00FF);
                        _wav = song->getWaveEntry(seltrk->wavei);
                    }
                    else
                    {
                        //Next entry wasn't the FF function
                        seltrk->wavei = _wav & 0x00FF;
                        _wav = song->getWaveEntry(seltrk->wavei);
                    }
                }
                else
                {
                    //Just jump to where it says
                    seltrk->wavei = _wav & 0x00FF;
                    _wav = song->getWaveEntry(seltrk->wavei);
                }
                
            } else if(cmd == 0xA) //Repeat Counter
            {
                if((_wav & 0xFF) == 0)
                    seltrk->ptbl[PARAM_LOOP3] = 1;
                else
                    seltrk->ptbl[PARAM_LOOP3] = _wav & 0xFF;
                seltrk->waveduracc = seltrk->ptbl[PARAM_LOOP3];
                //waveduracc will control how many ticks have passed
                //until the wave table index should iterate

                seltrk->wavei++;
                _wav = song->getWaveEntry(seltrk->wavei);

            } else if(cmd == 0xB) //Set loop Counter
            {
                seltrk->ptbl[PARAM_LOOP] = _wav & 0xFF;
                seltrk->wavei++;
                _wav = song->getWaveEntry(seltrk->wavei);
            } else if(cmd == 0xC) //decrement loop ctr, jump if != 0
            {
                seltrk->ptbl[PARAM_LOOP]--;
                if(seltrk->ptbl[PARAM_LOOP]!=0)
                {

                    //jump 
                    if(seltrk->wavei < 0xFFFF)
                    {
                        //concat destinations if next line is also FC function
                        unsigned short nxtwav = song->getWaveEntry(seltrk->wavei + 1);
                        if((nxtwav & 0xFF00) == 0xFC00)
                        {
                            seltrk->wavei = ((_wav & 0x00FF) << 8) | (nxtwav & 0x00FF);
                            _wav = song->getWaveEntry(seltrk->wavei);
                        }
                        else
                        {
                            seltrk->wavei = _wav & 0x00FF;
                            _wav = song->getWaveEntry(seltrk->wavei);
                        }
                    }
                    else
                    {
                        seltrk->wavei = _wav & 0x00FF;
                        _wav = song->getWaveEntry(seltrk->wavei);
                    }
                }
                else
                {
                    //Concatenate destinations if adjacent to another FC function
                    if(seltrk->wavei < 0xFFFF)
                    {
                        unsigned short nxtwav = song->getWaveEntry(seltrk->wavei + 1);
                        if((nxtwav & 0xFF) == 0xFC)
                            seltrk->wavei+=2;
                        else
                            seltrk->wavei++;
                    }
                    else
                        seltrk->wavei++;

                    _wav = song->getWaveEntry(seltrk->wavei);
                }
            
            } else if(cmd == 0x2) //F2, SET MISC WAVE PARAM 1
            {
                if(((_wav & 0xFF) != seltrk->ptbl[PARAM_WAVE1]) && resetsPhaseOnWave1Set(seltrk->lastwave))
                    seltrk->phase = 0;

                seltrk->ptbl[PARAM_WAVE1] = _wav; 
                seltrk->wavei++;
                _wav = song->getWaveEntry(seltrk->wavei);

            } else if(cmd == 0x3) //F3, SET MISC WAVE PARAM 2
            {
                seltrk->ptbl[PARAM_WAVE2] = _wav; 
                seltrk->wavei++;
                _wav = song->getWaveEntry(seltrk->wavei);
                
            } else if(cmd == 0x4) //F4, SET MULTIPLEX WAVE GENERATOR 1
            {
                GEN_MUX1 = generators[_wav & 0x00FF];

                seltrk->wavei++;
                _wav = song->getWaveEntry(seltrk->wavei);

            } else if(cmd == 0x5) //F5, SET MULTIPLEX WAVE GENERATOR 2
            {
                GEN_MUX2 = generators[_wav & 0x00FF];
                seltrk->wavei++;
                _wav = song->getWaveEntry(seltrk->wavei);
            } else if(cmd == 0x6) //F6, Fine Tune
            {
                char params = (_wav & 0x00FF);
                if(params == 0)
                    params = seltrk->ptbl[PARAM_WAVE1];

                frq *= std::pow(NOTEMULT, (params / 128.0f));
                //Count F6 as a note by breaking from the loop
                break;

            } else if(cmd == 0xD) //FD, SET CUSTOM JUMP
            {
                if(seltrk->wavei < 0xFFFF)
                {
                    //If the next wave function is also FD concat destinations
                    unsigned short nxtwav = song->getWaveEntry(seltrk->wavei + 1);
                    if((nxtwav & 0xFF00) == 0xFD00)
                    {
                        ((unsigned short*)seltrk->ptbl)[PARAM_CUSTOM_JUMP] = ((_wav & 0x00FF) << 8) | (nxtwav & 0x00FF);
                        seltrk->wavei++;
                    }
                    else
                        ((unsigned short*)seltrk->ptbl)[PARAM_CUSTOM_JUMP] = (_wav & 0x00FF);
                }
                else
                    ((unsigned short*)seltrk->ptbl)[PARAM_CUSTOM_JUMP] = (_wav & 0x00FF);
                std::cerr << " SET=" << std::hex << ((unsigned short*)seltrk->ptbl)[PARAM_CUSTOM_JUMP] << " FROM=" << _wav;

                seltrk->wavei++;
                _wav = song->getWaveEntry(seltrk->wavei);
            } else if(cmd == 0xE) //FE, PERFORM CUSTOM JUMP
            {
                seltrk->wavei = ((unsigned short*)seltrk->ptbl)[PARAM_CUSTOM_JUMP] + (_wav & 0xFF);
                std::cerr << " JUMP=" << seltrk->wavei;
                _wav = song->getWaveEntry(seltrk->wavei);
            }
            else
            {
                //Skip it until it's implemented
                seltrk->wavei++;
                _wav = song->getWaveEntry(seltrk->wavei);
            }


        }
    }
    unsigned char waveform = (_wav & 0xFF00) >> 8;

    //After leaving that loop, a waveform is gauranteed to be selected

    //if not portamento, set last frq
    if(seltrk->fx != 0x3)
    {
        seltrk->lastfrq = seltrk->frq;
    }

    if(waveform == 0xF6) 
        waveform = seltrk->lastwave;

    //////////////////////////////////////////
    //////PULSE TABLE IMPLEMENTATION//////////
    //////////////////////////////////////////

    //All of the jumps operate identically to the wave table jumps but these
    //operations perform on pulse related fields
    if(seltrk->pulsei != 0xFFFF)
    {
        unsigned short _pulse = song->getPulseEntry(seltrk->pulsei);

        if(_pulse > 0xEFFF)
        {
            while((_pulse & 0xF000) == 0xF000)
            {
                depth++;
                if(depth > 64)
                {
                    std::cerr << "Oh dear! Infinite loop! Pulse set to 0.\n";
                    seltrk->pulsei--;
                    ((unsigned short*)seltrk->ptbl)[PARAM_PULSE] = 0;
                    break;
                }

                //Get which pulse function it is
                unsigned char cmd = (_pulse & 0x0F00) >> 8;
                if(cmd == 0xF) //FF, JUMP
                {
                    //If there is a Jump instruction immediately following this pulse
                    //entry then concatenate the destinations of these wave entries
                    if(seltrk->pulsei < 0xFFFF) //edge case
                    {
                        unsigned short nxtpls = song->getPulseEntry(seltrk->pulsei + 1);
                        if((nxtpls & 0xFF00) == 0xFF00)
                        {
                            seltrk->pulsei = ((_pulse & 0x00FF) << 8) | (nxtpls & 0x00FF);
                            _pulse = song->getPulseEntry(seltrk->pulsei);
                        }
                        else
                        {
                            //Next entry wasn't the FF function
                            seltrk->pulsei = _pulse & 0x00FF;
                            _pulse = song->getPulseEntry(seltrk->pulsei);
                        }
                    }
                    else
                    {
                        //Just jump to where it says
                        seltrk->pulsei = _pulse & 0x00FF;
                        _pulse = song->getPulseEntry(seltrk->pulsei);
                    }
                } else if(cmd == 0xA) //Repeat Counter
                {

                    if((_pulse & 0xFF) == 0)
                        seltrk->ptbl[PARAM_LOOP4] = 1;
                    else
                        seltrk->ptbl[PARAM_LOOP4] = _pulse & 0xFF;
                    seltrk->pulseduracc = seltrk->ptbl[PARAM_LOOP4];

                    seltrk->pulsei++;
                    _pulse = song->getPulseEntry(seltrk->pulsei);

                } else if(cmd == 0xB) //Set loop Counter
                {
                    seltrk->ptbl[PARAM_LOOP2] = _pulse & 0xFF;
                    seltrk->pulsei++;
                    _pulse = song->getPulseEntry(seltrk->pulsei);
                } else if(cmd == 0xC) //decrement loop ctr, jump if != 0
                {
                    seltrk->ptbl[PARAM_LOOP2]--;
                    if(seltrk->ptbl[PARAM_LOOP2]!=0)
                    {

                        //jump 
                        if(seltrk->pulsei < 0xFFFF)
                        {

                            unsigned short nxtpls = song->getPulseEntry(seltrk->pulsei + 1);
                            if((nxtpls & 0xFF00) == 0xFC00)
                            {
                                seltrk->pulsei = ((_pulse & 0x00FF) << 8) | (nxtpls & 0x00FF);
                                _pulse = song->getPulseEntry(seltrk->pulsei);
                            }
                            else
                            {
                                seltrk->pulsei = _pulse & 0x00FF;
                                _pulse = song->getPulseEntry(seltrk->pulsei);
                            }
                        }
                        else
                        {
                            seltrk->pulsei = _pulse & 0x00FF;
                            _pulse = song->getPulseEntry(seltrk->pulsei);
                        }
                    }
                    else
                    {
                        //Concatenate destinations if adjacent to another FC function
                        if(seltrk->pulsei < 0xFFFF)
                        {
                            unsigned short nxtpls = song->getPulseEntry(seltrk->pulsei + 1);
                            if((nxtpls & 0xFF) == 0xFC)
                                seltrk->pulsei+=2;
                            else
                                seltrk->pulsei++;
                        }
                        else
                            seltrk->pulsei++;

                        _pulse = song->getPulseEntry(seltrk->pulsei);
                    }

                } else if(cmd == 0x0) //F0, SET PULSE2 
                {
                    ((unsigned short*)seltrk->ptbl)[PARAM_PULSE2] = (_pulse & 0x00FF) << 8;

                    seltrk->pulsei++;
                    _pulse = song->getPulseEntry(seltrk->pulsei);
                } else if(cmd == 0x1) //F1, ADD PULSE2
                {
                    ((short*)seltrk->ptbl)[PARAM_PULSE2] += static_cast<char>(_pulse & 0x00FF)*0x10; //shift once to the left

                    seltrk->pulsei++;
                    _pulse = song->getPulseEntry(seltrk->pulsei);

                }
                else if(cmd == 0xD) //FD, SET CUSTOM JUMP
                {
                    if(seltrk->pulsei < 0xFFFF)
                    {
                        unsigned short nxtpls = song->getPulseEntry(seltrk->pulsei + 1);
                        if((nxtpls & 0xFF00) == 0xFD00)
                        {
                            ((unsigned short*)seltrk->ptbl)[1] = ((_pulse & 0x00FF) << 8) | (nxtpls & 0x00FF);
                            seltrk->pulsei++;
                        }
                        else
                            ((unsigned short*)seltrk->ptbl)[1] = (_pulse & 0x00FF);
                    }
                    else
                        ((unsigned short*)seltrk->ptbl)[PARAM_CUSTOM_JUMP2] = (_pulse & 0x00FF);

                    seltrk->pulsei++;
                    _pulse = song->getPulseEntry(seltrk->pulsei);
                } else if(cmd == 0xE) //FE, JUMP CUSTOM JUMP
                {
                    seltrk->pulsei = ((unsigned short*)seltrk->ptbl)[PARAM_CUSTOM_JUMP2] + (_pulse & 0xFF);
                    _pulse = song->getPulseEntry(seltrk->pulsei);
                }
                else
                {
                    seltrk->pulsei++;
                    _pulse = song->getPulseEntry(seltrk->pulsei);
                }


            }

 
        }

        if(_pulse < 0xE000) //0x0000 to 0xDFFF add pulse
        {
            if(_pulse >= 0x7000) //Negative
            {
                unsigned short offset = _pulse + 0x2000; // this would make 0xDFFF into 0xFFFF which is -1
                ((unsigned short*)seltrk->ptbl)[PARAM_PULSE] += static_cast<short>(offset);
            }
            else
                ((short*)seltrk->ptbl)[PARAM_PULSE] += _pulse; //shift once to the left
        }
        else if(_pulse < 0xF000) // 0xE___, set pulse
        {
            ((unsigned short*)seltrk->ptbl)[PARAM_PULSE] = (_pulse & 0x0FFF) << 4;

        }
        //if(seltrk->pulsei != 0xFFFF) std::cerr << " pulse=" <<  ((unsigned short*)seltrk->ptbl)[PARAM_PULSE] << " " << seltrk->pulsei << "pulseval=" << song->getPulseEntry(seltrk->pulsei ) <<'\n';
    
    
    
    }

    ////////////////////////////////////////////////
    //////APPLY ON-SEGMENT EFFECTS TO WAVEFORM//////
    ////////////////////////////////////////////////
    if(!(seltrk->fx == seltrk->fxparam && seltrk->fx == 0))
    {

        //0 FRQ Arpeggio    <<per seg
        //1 FRQ Slide up    <<per seg
        //2 FRQ Slide dn    <<per seg
        //3 FRQ Slide note  <<per seg
        //4 FRQ Vibrato     <<per seg
        //
        //  5 Set Wave Param1 ||on row
        //  6 Set Wave Param2 ||on row
        //
        //  8 Set Pulse       ||on row
        //
        //A VOL Slide up    <<per seg
        //B VOL Slide dn    <<per seg

        switch(seltrk->fx)
        {
            case 0x0: //arpeggio
                {
                    int section;
                    section = (ARPEGGIO_SPEED*seltrk->segments / song->getInterrowRes()) % 3;
                    if(section == 1)
                        frq/=std::pow(NOTEMULT, (seltrk->fxparam & 0xF0) >> 4);
                    else if(section == 2)
                    {
                        frq/=std::pow(NOTEMULT, (seltrk->fxparam & 0x0F));
                    }
                    if(frq < 1)
                        frq = 1;
                }
                break;
            case 0x1: //frq slide up
                {
                    //the constant 21.33333 clearly has some significance
                    //I wish I could remember what it was.
                    //But it works fine so yay.
                    frq/=std::pow(NOTEMULT, (1.0*seltrk->fxparam)/(song->getInterrowRes()*21.33333));
                    if(frq < 1)
                        frq = 1;
                    seltrk->frq = frq;
                }
                break;
            case 0x2: //frq slide dn
                {
                    frq*=std::pow(NOTEMULT,  (1.0*seltrk->fxparam)/(song->getInterrowRes()*21.33333));
                    if(frq < 1)
                        frq = 1;
                    seltrk->frq = frq;
                }
                break;
            case 0x3: //frq slide note
                {

                    
                    //std::cerr << "INTERPOLATING " << seltrk->frq << " to " << seltrk->nextfrq << "seg=" << seltrk->segments  << '\n';
                    //std::cerr << seltrk->lastfrq << " , " << logbase(NOTEMULT, seltrk->lastfrq) <<   ", " << std::pow(NOTEMULT, logbase(NOTEMULT,seltrk->lastfrq)) << '\n';
                    //Will exponentially approach nextnote asymptotically
                    //not sure if this is the correct way to implement this, but it seems to work
                    float lastnote = logbase(NOTEMULT, seltrk->frq);
                    float nextnote = logbase(NOTEMULT, seltrk->nextfrq);
                    float dif = 12.0*(nextnote - lastnote)/(256-seltrk->fxparam); //the higher the number on the right, faster portamento
                    frq = std::pow(NOTEMULT, lastnote+dif);
                    


                    if(dif > 0)
                        if(frq > seltrk->nextfrq)
                            frq = seltrk->nextfrq;
                    if(dif < 0)
                        if(frq < 1)
                            frq = seltrk->nextfrq;
                    
                    //std::cerr << "dif=" << dif << " result=" << frq << '\n';
                    seltrk->frq = frq;
                }
                break;
            case 0x4: //vibrato
                {
                    //4 stage vibrato UP DOWN, DOWN UP, like this picture: [^v]
                    unsigned char durparam = 0x10 - ((seltrk->fxparam & 0xF0) >> 4);
                    unsigned char duration = durparam*4;
                    float depth = (std::pow(NOTEMULT,(((seltrk->fxparam & 0xF) +
                                        1)/(durparam*16.0))));
                    unsigned char part = (seltrk->segments % (duration)) / durparam;
                    switch(part)
                    {
                        case 0:
                        case 3:
                            frq*=depth; //floating point error galore but 
                                        //shhhhhh it's okay                           
                            
                            break;
                        case 1:
                        case 2:
                            frq/=depth;
                            break;
                    }
                    seltrk->frq = frq;
                    
                
                }
                break;
            case 0xA: //vol slide up
                {
                    seltrk->segments %=  song->getInterrowRes();
                    seltrk->ptrnvol = seltrk->ptrnlastvol+ (0x3f*seltrk->segments*(seltrk->fxparam / 255.0)) / song->getInterrowRes();
                    if(seltrk->ptrnvol > 0x3f)
                        seltrk->ptrnvol = 0x3f;


                
                }
                break;
            case 0xB: //vol slide dn
                {
                    seltrk->segments %=  song->getInterrowRes();
                    seltrk->ptrnvol = (0x3f*seltrk->segments*(seltrk->fxparam / 255.0)) / song->getInterrowRes();
                    if(seltrk->ptrnvol > seltrk->ptrnlastvol)
                        seltrk->ptrnvol = 0;
                    else
                        seltrk->ptrnvol = -int(seltrk->ptrnvol) + seltrk->ptrnlastvol;
                    if(seltrk->ptrnvol > 0x3f)
                        seltrk->ptrnvol = 0x3f;
                
                }
                break;
        }

    }


    //Change frequency based on the parameters of the wave entry
    //as long as the entry wasn't the vibrato effect
    if(waveform != 0xF4)
    {
        unsigned char params = (_wav & 0x00FF);
        if(params)
        {
            if(params < 0x40)
                frq /= std::pow(NOTEMULT, params);
            else
            {
                params = ~params + 1;
                frq *= std::pow(NOTEMULT, params);
            }
        }
    }


    if(seltrk->pulsei != 0xFFFF)
    {
        //Do pulseduracc handling
        if((unsigned char)seltrk->ptbl[PARAM_LOOP4] < 2)
        {
            seltrk->pulsei++;
            seltrk->pulseduracc = 1;
        }
        else
        {
            //if pulse duration step is now 0, increase pulse 
            if(seltrk->pulseduracc == 0)
            {
                seltrk->pulsei++;
                seltrk->pulseduracc = seltrk->ptbl[PARAM_LOOP4];
            }
        }
        seltrk->pulseduracc--;
    }
    if(frq < 1)
        frq = 1;
    if((unsigned char)seltrk->ptbl[PARAM_LOOP3] < 2)
    {
        seltrk->wavei++;
        seltrk->waveduracc = 1;
    }
    else
    {
        if(seltrk->waveduracc == 0)
        {
            seltrk->wavei++;
            seltrk->waveduracc = seltrk->ptbl[PARAM_LOOP3];
        }
    }
    seltrk->waveduracc--; 

    amp *= seltrk->ptrnvol / 63.0f;
    amp *= amplifyall;


    //Maintain phase only in similar waveforms
    //because the different generators use phase differently
    //at the moment
    if(seltrk->lastwave != waveform)
        seltrk->phase = 0;

    //DEBUG OUTPUT
    //std::cerr << "renderTick 3 amp=" << std::hex << int(amp) << ", wav="  << _wav << "h frq=" << frq <<  " bytes=" << bytes << " phase=" << seltrk->phase << " pos=" << (int*)buffer;
    //if(seltrk->pulsei != 0xFFFF) std::cerr << " pulse=" << (unsigned short)song->getPulseEntry(seltrk->pulsei) << '\n';


    //Generate the actual sound
    generators[waveform](buffer, seltrk->ptbl,  frq, amp, seltrk->phase, bytes);
    seltrk->ptbl[PARAM_CHAIN] = buffer[bytes-1];

    seltrk->lastwave =  waveform;
    seltrk->segments++;

    //std::cerr << "renderTick END\n";
}

void itrp::initializeWaveTable()
{
    generators = new generator[256];
    generators[0] = genSilence;

    for(int i = 4; i < 256; i++)
        generators[i] = genSilence;

    //BASIC WAVE FORMS!
    generators[0x01] = genSqr;
    generators[0x02] = genTri;
    generators[0x03] = genSaw;
    generators[0x04] = genSine;

    //NOISE
    generators[0x05] = genNoise_White;

    //PULSE WAVE FORMS
    generators[0x06] = genSqrPulse;
    generators[0x07] = genTriPulse;
    generators[0x08] = genSawPulse;
    generators[0x09] = genSinePulse;


    generators[0x0A] = genSqrNES;
    generators[0x0B] = genSqrPulseNES;

    generators[0x0C] = genSqrVTX;
    generators[0x0D] = genSqrPulseVTX;

    generators[0x0E] = genSqrPulse2;
    generators[0x0F] = genSqrPulseHybrid;

    //save room for more squares (how many could there be?)

    generators[0x14] = genBlacmange;
    //generators[0x15] = genTriPulseHybrid;

    generators[0x16] = genSinePulse2;
    generators[0x17] = genSinePulseHybrid;

    generators[0x18] = genHalfSine;
    generators[0x19] = genHalfSinePulse;


    generators[0x1a] = genNSine;
    generators[0x1b] = genNSinePulse;

    generators[0x1c] = genNSinePulse2;
    generators[0x1d] = genNSinePulseHybrid;

    generators[0x1e] = genNHalfSine;
    generators[0x1f] = genNHalfSinePulse;

    generators[0x20] = genMuxShared;
    generators[0x21] = genMuxSwap;
    generators[0x22] = genMuxSwap2;


    //PERCUSSION
    generators[0x60] = genNoise0;
    generators[0x61] = genNoise1;
    generators[0x62] = genNoise2;
    generators[0x63] = genNoise3;
    generators[0x64] = genBongo;

    
}


bool itrp::resetsPhaseOnWave1Set(const unsigned char &wave)
{
    switch(wave)
    {
        case 0x1A:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
        case 0x1F:
            return true;
    }
    return false;
}




void itrp::initializeRender()
{
    order = 0;
    curpattern = song->getPatternByOrder(0);
    for(int i = 0; i < song->numTracks(); i++)
    {
        unsigned int firstrow = curpattern->at(i,0);
        tracks[i].frq = 1;
        tracks[i].lastfrq = 1;
        tracks[i].nextfrq = 1;
        tracks[i].phase = 0;

        if(((firstrow & R_INSTRUMENT) >> RI_INSTRUMENT) < song->numInstruments())
            tracks[i].inst = song->getInstrument((firstrow & R_INSTRUMENT) >> RI_INSTRUMENT);
        else
            tracks[i].inst = NULL;
        if(tracks[i].inst != NULL)
        {
            tracks[i].wavei = tracks[i].inst->getWaveIndex();
            tracks[i].pulsei = tracks[i].inst->getPulseIndex();
            tracks[i].voli = tracks[i].inst->getVolEntry(0);
        }
        else
        {
            tracks[i].wavei = 0;
            tracks[i].pulsei = 0;
            tracks[i].voli = 0;

        }

        tracks[i].fx = (firstrow & R_EFFECT) >> RI_EFFECT;
        tracks[i].fxparam = firstrow & R_FXPARAM;
        tracks[i].segments = 0;
        tracks[i].ptbl = new unsigned char[32];
        tracks[i].lastwave = 0;


        tracks[i].ptbl[2*PARAM_PULSE  ] = 0x00; //Pulse param (part 1)
        tracks[i].ptbl[2*PARAM_PULSE+1] = 0x10; //Pulse param (part 2), a small value so you can hear it, but doesnt sound correct. better than nothing

        tracks[i].ptbl[2*PARAM_PULSE2  ] = 0x00; //Pulse param (part 1)
        tracks[i].ptbl[2*PARAM_PULSE2+1] = 0x10; //Pulse param (part 1)

        tracks[i].ptbl[2] = 0; //Custom jump (part 1)
        tracks[i].ptbl[3] = 0; //Custom jump (part 2)

        tracks[i].ptbl[PARAM_LOOP] = 0; //Wave Loop counter
        tracks[i].ptbl[PARAM_WAVE1] = 127; //Wave misc param 1
        tracks[i].ptbl[PARAM_WAVE2] = 0x80; //Wave misc param 2
        tracks[i].ptbl[PARAM_CHAIN] = 127; //Chain
        tracks[i].ptbl[PARAM_LAST] = 127; //Track Interp
        tracks[i].ptbl[PARAM_LOOP2] = 0; //Pulse loop counter
        tracks[i].ptbl[PARAM_LOOP3] = 1; //Wave repeat counter
        tracks[i].ptbl[PARAM_LOOP4] = 1; //Pulse repeat counter

        tracks[i].ptrnvol = (firstrow & R_VOLUME) >> RI_VOLUME;
        tracks[i].ptrnlastvol = tracks[i].ptrnvol;

        tracks[i].waveduracc = 0;
        tracks[i].pulseduracc = 0;
        tracks[i].volduracc = 0;
        tracks[i].lastvol = 0;
        tracks[i].voljump = 1;
    }
    //std::cerr << "initialized render\n";
}

unsigned char *itrp::renderPattern(int start, int end, unsigned int &bytes)
{

    unsigned int row;
    const unsigned short bytesperrow = song->getBytesPerRow();
    const unsigned char  subdiv = song->getInterrowRes();
    const unsigned short segment = bytesperrow / subdiv;

    //std::cerr << "renderSong0 orders:" << int(song->numOrders()) <<" \n";

    unsigned char _oct;
    unsigned char _note;
    unsigned char _inst;

    unsigned char _vol;
    unsigned char _fx;
    unsigned char _fxp1;
    unsigned char _fxp2;
    unsigned short _wav;
    Track *seltrk;


    if(end < start)
        end = curpattern->numRows();
    int rows = end - start;
    bytes = rows*bytesperrow;

    unsigned char *buffer = new unsigned char[bytes];
    for(int i = 0; i < bytes; i++)
        buffer[i]=127;

    //Loop through tracks
    for(unsigned int tracki = 0; tracki < song->numTracks(); tracki++)
    {
        //if track muted, go to next track
        if(trackmute[tracki])
            continue;

        //Track not muted:

        seltrk = &tracks[tracki];

        //std::cerr << "renderSong2 track=" << tracki << '\n';
        for(unsigned int rowi = start; rowi < end; rowi++)
        {
            //std::cerr << "renderSong3    row=" << rowi << '\n';
            row = curpattern->at(tracki, rowi);
            _fx = (row & R_EFFECT) >> RI_EFFECT;

            if((row & R_EMPTY) != R_EMPTY) //FULLSEG AND VOLUME
            {
                if((row & R_FULLSEG) != R_FULLSEG) // PITCHSEG AND INST
                {
                    if((row & R_PITCHSEG) != R_PITCHSEG) //OCTAVE AND NOTE
                    {
                        //std::cerr << "pitch";
                        _oct  = (row & R_OCTAVE) >> RI_OCTAVE;
                        _note = (row & R_NOTE) >> RI_NOTE;
                        if(_fx != 0x3)
                            seltrk->frq = BASEFRQ/std::pow(NOTEMULT, 12*_oct+_note);
                        else
                            seltrk->nextfrq = BASEFRQ/std::pow(NOTEMULT, 12*_oct+_note);
                    }

                    //If instrument changes
                    if((row & R_INSTRUMENT) != R_INSTRUMENT)
                    {
                        //std::cerr << " inst";

                        _inst = (row & R_INSTRUMENT) >> RI_INSTRUMENT;
                        seltrk->inst = song->getInstrument(_inst);
                        _wav = seltrk->inst->getWaveIndex();
                        seltrk->wavei = _wav;
                        if (seltrk->inst->getPulseIndex() < song->numPulseEntries())
                        {
                            seltrk->pulsei = seltrk->inst->getPulseIndex();
                            //std::cerr << "INSTSET pulsei " << seltrk->pulsei << '\n';
                        }
                        seltrk->waveduracc = 0;
                        seltrk->ptbl[PARAM_LOOP3] = 1;

                        if(seltrk->inst->getPulseIndex() < 0xFFFF)
                        {
                            seltrk->pulseduracc = 0;
                            seltrk->ptbl[PARAM_LOOP4] = 1;
                        }
                        
                        seltrk->voli = 0;
                        seltrk->volduracc = 0;
                        seltrk->lastvol = (seltrk->inst->getVolEntry(0) & 0xFF00) >> 8;

                    }

                    _vol  = (row & R_VOLUME) >> RI_VOLUME;
                    seltrk->ptrnvol = _vol;
                    //std::cerr << " vol lastvol=" << (int)seltrk->lastvol;
                }
                else
                {
                    if((row & R_VOLUME) != R_VOLUME)
                    {
                        _vol  = (row & R_VOLUME) >> RI_VOLUME;
                        seltrk->ptrnvol = _vol;
                        //std::cerr << "vol lastvol=" << (int)(seltrk->lastvol);
                    }
                }
                //std::cerr << '\n';

            }
            seltrk->ptrnlastvol = seltrk->ptrnvol;

            //Handle instant effects that happen once immediately per row
            if(row & R_EFFECTSEG) // Handle effects seperately
            {
                //std::cerr << "Effect " << std::hex << (row & 0xFFF) << '\n';

                //  0 FRQ Arpeggio    ||per seg
                //  1 FRQ Slide up    ||per seg
                //  2 FRQ Slide dn    ||per seg
                //  3 FRQ Slide note  ||per seg
                //  4 FRQ Vibrato     ||per seg
                //5 Set Wave Param1 <<on row
                //6 Set Wave Param2 <<on row
                //
                //8 Set Pulse       <<on row
                //
                //  A VOL Slide up    ||per seg
                //  B VOL Slide dn    ||per seg
                if(_fx == 5)
                { 
                    _fxp1 = row & R_FXPARAM;
                    seltrk->ptbl[5] = _fxp1;
                }
                else if(_fx == 6)
                { 
                    _fxp1 = row & R_FXPARAM;
                    seltrk->ptbl[6] = _fxp1; 
                }
                else if(_fx == 8)
                { 
                    _fxp1 = row & R_FXPARAM;
                   ((unsigned short*)seltrk->ptbl)[0] = (((unsigned short)_fxp1)<< 8); //oh dear.
                }
                else if(_fx == 7)
                { 
                    seltrk->wavei = row & R_FXPARAM;
                }
                else if(_fx == 0x9)
                { 
                    seltrk->pulsei = row & R_FXPARAM;
                }

            }

            if(seltrk->fx != _fx)
            {
                seltrk->fx = _fx;
                seltrk->segments = 0;
            }
            seltrk->fxparam = row & R_FXPARAM;

            //std::cerr << "renderSong4 starting segloop subdiv=" << int(subdiv) << " segment=" << segment << '\n';
            for(unsigned int subi = 0; subi < subdiv; subi++)
            {
                renderTick(buffer + (rowi - start)*bytesperrow + subi*segment,  tracki, segment);
            }
        }
    }
    return buffer;
}

unsigned char **itrp::renderSong(unsigned int *bytes)
{
    
    //std::cerr << "renderSong creating buffers \n";

    unsigned char **buffers = new unsigned char*[song->numOrders()];
    //std::cerr << "renderSong1\n";
    for(unsigned int orderi = 0; orderi < song->numOrders(); orderi++)
    {
        curpattern = song->getPatternByOrder(orderi);
        unsigned char *buffer = renderPattern(0, curpattern->numRows(), bytes[orderi]);
        buffers[orderi] = buffer;
    }
    //std::cerr << "renderSong END\n";
    return buffers;
}


bool itrp::load(const char *file)
{
    std::ifstream infile;

    infile.open(file);
    if(!infile)
        return false;
    song = new Song(infile);
    infile.close();
    
    tracks = new Track[song->numTracks()];
    return true;
}


void itrp::purgeSong()
{
    for(int i = 0; i < song->numTracks(); i++)
        delete [] tracks[i].ptbl;
    delete song;
    delete [] tracks;
    delete [] generators;
    //delete buffer
}




void testWaveTable()
{
    itrp::initializeWaveTable();

    int waves = 5;
    int bytes = 100000;
    int step = bytes / waves / 16;


    unsigned char *bfr = new unsigned char[bytes];
    for(int i = 0; i < waves; i++)
    {
        float phase = 0;
        generator g = itrp::generators[i];
        g(bfr+i*16*step         , NULL,  64, 75, phase, step);
        g(bfr+i*16*step + step  , NULL, 32, 75, phase, step);
        g(bfr+i*16*step + step*2, NULL, 16, 75, phase, step);
        g(bfr+i*16*step + step*3, NULL, 64, 75, phase, step);
        g(bfr+i*16*step + step*4, NULL, 64, 75, phase, step);
        g(bfr+i*16*step + step*5, NULL, 32, 75, phase, step);
        g(bfr+i*16*step + step*6, NULL, 16, 75, phase, step);
        g(bfr+i*16*step + step*7, NULL, 64, 75, phase, step);
        g(bfr+i*16*step + step*8, NULL, 64, 75, phase, step);
        g(bfr+i*16*step + step*9, NULL, 32, 75, phase, step);
        g(bfr+i*16*step + step*10, NULL, 16, 75, phase, step);
        g(bfr+i*16*step + step*11, NULL, 64, 75, phase, step);
        g(bfr+i*16*step + step*12, NULL, 64, 75, phase, step);
        g(bfr+i*16*step + step*13, NULL, 32, 75, phase, step);
        g(bfr+i*16*step + step*14, NULL, 16, 75, phase, step);
        g(bfr+i*16*step + step*15, NULL, 64, 75, phase, step);
    }
    itrp::play(bfr, bytes);
    //itrp::printBuffer(bfr, bytes);
    delete [] bfr;

}




void testLoadWholeSong()
{
    //Loads song.plb
    if(!itrp::load("song.plb"))
    {
        //Didn't load
        std::cerr << "song.plb could not be found.";
    }

    //printWaveTable(itrp::song->getWaveTable(), 10);

    itrp::initializeWaveTable();
    //std::cerr << "main1 initialized wave table\n";
    itrp::initializeRender();
    //std::cerr << "main2 initialized render\n";

    unsigned int *bytes = new unsigned int[itrp::song->numOrders()];
    unsigned char **bfrs = itrp::renderSong(bytes);
    //std::cerr << "main3 finished render!\n";

    itrp::play(bfrs, itrp::song->numOrders(),  bytes);
    //itrp::print(bfrs, itr[::song->numOrders(), bytes);
    for(int i = 0; i < itrp::song->numOrders(); i++)
        delete [] bfrs[i];
    delete [] bfrs;

    //std::cerr << "main4 deleted buffers, deleting song\n";

    itrp::purgeSong();
}

unsigned char **itrp::renderSong(unsigned int *bytes, int start_order, int end_order, int start_row, int end_row)
{
    if(end_row >= 0)
    {
        //Only play start order
        unsigned char **buffers = new unsigned char*[1];
        curpattern = song->getPatternByOrder(start_order);
        unsigned char *buffer = renderPattern(start_row, end_row, bytes[0]);
        buffers[0] = buffer;
        return buffers;
    }
    else
    {
        //play through order
        int orders = end_order - start_order + 1;
        unsigned char **buffers = new unsigned char*[orders];

        int bfri = 0;
        for(unsigned int orderi = start_order; orderi <= end_order; orderi++)
        {
            curpattern = song->getPatternByOrder(orderi);
            unsigned char *buffer = renderPattern(start_row, curpattern->numRows(), bytes[bfri]);
            start_row = 0;
            buffers[bfri] = buffer;
            bfri++;
        }
        return buffers;
    }

}

bool parseParams(int argc, const char* argv[])
{


    int start_order = 0;
    int start_row = 0;
    int end_order = itrp::song->numOrders()-1;
    int end_row = -1;
    for(int i = 0; i < 256; i++)
        itrp::trackmute[i] = 0;

    for(int i = 0; i < argc; i++)
    {
        if(strcmp(argv[i], "order") == 0)
        {
            if(++i < argc)
            {
                start_order = atoi(argv[i]);
            }
            else
            {
                std::cerr << "which order?\n";
                return false;
            }
        }
        else if(strcmp(argv[i], "endorder") == 0)
        {
            if(++i < argc)
            {
                end_order = atoi(argv[i]);
            }
            else
            {
                std::cerr << "which order?\n";
                return false;
            }
        }
        else if(strcmp(argv[i], "row") == 0)
        {
            if(++i < argc)
            {
                start_row = atoi(argv[i]);
            }
            else
            {
                std::cerr << "which row?\n";
                return false;
            }
        }
        else if(strcmp(argv[i], "endrow") == 0)
        {
            if(++i < argc)
            {
                end_row = atoi(argv[i]);
            }
            else
            {
                std::cerr << "which row?\n";
                return false;
            }
        }
        else if(strcmp(argv[i], "amp") == 0     ||
                strcmp(argv[i], "amplify") == 0 ||
                strcmp(argv[i], "gain") == 0)
        {
            if(++i < argc)
            {
                sscanf( argv[i], "%f", &itrp::amplifyall);
            }
            else 
            {
                std::cerr << "amplify how much?\n";
                return false;
            };
        }
        else if(strcmp(argv[i], "mute") == 0)
        {
            if(++i < argc)
            {
                std::stringstream ss(argv[i]);
                int trk;
                std::string str;
                while(std::getline(ss, str, ','))
                {
                    trk = atoi(str.c_str());
                    itrp::trackmute[trk] = true;
                    std::cerr << "Muting track " << trk << '\n';
                }
            }
            else
            {
                std::cerr << "mute what tracks?\n";
                return false;
            }
        }



    }
    



    unsigned int *bytes;
    int orders;

    if(start_order < 0)
        start_order = 0;
    if(start_order >= itrp::song->numOrders())
        start_order = itrp::song->numOrders() - 1;

    if(end_row > 1)
    {
        orders = 1;
        bytes = new unsigned int[1];
        end_order = start_order;
    }
    else
    {
        if(end_order < start_order)
            end_order = start_order;
        else if(end_order >= itrp::song->numOrders())
            end_order = itrp::song->numOrders()-1;

        orders = end_order - start_order+1;
        bytes = new unsigned int[orders];
    }

    
    unsigned char **bfrs = itrp::renderSong(bytes, start_order, end_order, start_row, end_row);
    itrp::play(bfrs, orders, bytes);
    //itrp::print(bfrs, orders, bytes);

    for(int i = 0; i < orders; i++)
        delete [] bfrs[i];
    delete [] bytes; //Why does thia cause double free corruption??


    itrp::purgeSong();
    return true;

}




int main(int argc, const char* argv[])
{ 
    itrp::songpaths = new char*[64]; //TODO
    itrp::amplifyall = 1;
    if(argc > 1)
    {
        itrp::load(argv[1]);

        itrp::initializeWaveTable();
        itrp::initializeRender();

        //Has parameters
        parseParams(argc, argv);
    }
    delete [] itrp::songpaths;



}






