#include "instrument.h"


Instrument::Instrument()
{
    name[0] = 'i';
    name[1] = 'n';
    name[2] = 's';
    name[3] = 't';
    waveIndex = 0;
    volEntries=1;
    volTable = new unsigned short[256];//64 integers worth
    for(int i = 0; i < 256; i++)
        volTable[i]=0;
    volTable[0] = 0x3F00;
    pulseIndex = -1;
}

Instrument::~Instrument()
{
    delete [] volTable;
}

Instrument::Instrument(std::istream &in)
{
    volTable = NULL;
    input(in);
}
Instrument::Instrument(const Instrument &other)
{
    waveIndex        = other.waveIndex;
    volEntries       = other.volEntries;
    volTable = new unsigned short[256];//64 integers worth
    for(int i = 0; i < 255; i++)
        volTable[i] = other.volTable[i];

}


std::ostream &Instrument::output(std::ostream &out) const
{
    out.write(name, 23);
    out.write((char*)&waveIndex, sizeof(short));
    out.write((char*)&pulseIndex, sizeof(short));

    out.write((char*)&volEntries,1);
    out.write((char*)volTable, volEntries*2);

    return out;	
}

std::istream &Instrument::input(std::istream &in)
{
    if(volTable)
        delete [] volTable;

    in.read(name,23);
    in.read((char*)&waveIndex, sizeof(short));
    in.read((char*)&pulseIndex, sizeof(short));
    in.read((char*)&volEntries, 1);
    volTable = new unsigned short[256];
    in.read((char*)volTable, volEntries*2);
    for(int i = volEntries; i < 256; i++)
        volTable[i] = 0;

    return in;
}

void Instrument::setVolTable(unsigned short *table)
{
    for(int i = 0; i < 256; i++)
        volTable[i] = table[i];
}

unsigned char Instrument::getVolume(unsigned char &cur, unsigned char &seg, unsigned char &jump, unsigned char &last)
{
    float desttime = (volTable[cur] &0x00FF);
    unsigned char dest = (volTable[cur] &0xFF00) >> 8;

    if(dest==0xFB || dest == 0xFD)
    {
        jump = desttime;
        if(cur < volEntries-1)
        {
            cur++;
            desttime = (volTable[cur] &0x00FF);
            dest = (volTable[cur] &0xFF00) >> 8;
        }
    }

    unsigned char depth = 0;

    while(dest==0xFF || dest == 0xFC || dest == 0xFE)
    {
        depth++;
        if(depth > 64)
        {
            last = 0;
            return 0;
        }
        if(dest == 0xFF)
        {
            cur = desttime;
        } else if (dest == 0xFC)
        {
            jump--;
            if(jump == 0 && cur < volEntries-1)
                cur++;
            else
                cur = desttime;
        } else if (dest == 0xFE)
        {
            cur = desttime+jump;
        }

        desttime = (volTable[cur] &0x00FF);
        dest = (volTable[cur] &0xFF00) >> 8;
        seg = 0;
    }


    if(seg >= desttime)
    {
        seg= 0;
        if(cur < volEntries-1)
            cur++;
        last = dest;
        return last;
    }
    


    //Interpolate between the last volume and next over the duration
    if(cur >0)
    {
        //interpolate
        last = last + ((static_cast<float>(dest) - last) * (seg/desttime));
        return  last; 
    }
    last = dest;
    return last;
}



void Instrument::fixVolJumps(const unsigned char &index, short difference)
{
    if(difference == 0)
        return;
    for(unsigned short i = 0; i < volEntries; i++)
        if((volTable[i] & 0xFF00) == 0xFF00)//is jump, correct it
        {
            if(difference > 0)
            {
                unsigned char dest = volTable[i] & 0x00FF;
                if(dest >= index)
                {
                    if(dest < 0xFF-difference)
                        volTable[i] += difference;

                }
            }
            else
            {
                unsigned char dest = volTable[i] & 0xFF;
                if(dest >= index)
                {
                    if(dest >= -difference)
                        volTable[i] += difference;
                    else volTable[i] = 0xFF00;

                }


            }
        }
}


bool Instrument::insertVolEntry(unsigned char index, unsigned short entry)
{
    if(volEntries == 0xFFFF)
        return false;
    for(unsigned short last = ++volEntries; last > index; last--)
        volTable[last] = volTable[last-1];
    fixVolJumps(index, 1);
    volTable[index] = entry;
    return true;
}

bool Instrument::removeVolEntry(unsigned char index)
{
    if(volEntries == 0)
        return false;

    for(unsigned short i = index+1; i < volEntries; i++)
        volTable[i-1] = volTable[i];
    fixVolJumps(index, -1);
    volTable[--volEntries] = 0;
    return true;
}






