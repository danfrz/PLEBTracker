#include "instrument.h"

Instrument::Instrument()
{
    //Initial name "instrument name"
    //not going to bother with a strcpy
    name[0] = 'i';
    name[1] = 'n';
    name[2] = 's';
    name[3] = 't';
    name[4] = 'r';
    name[5] = 'u';
    name[6] = 'm';
    name[7] = 'e';
    name[8] = 'n';
    name[9] = 't';
    name[10] = ' ';
    name[11] = 'n';
    name[12] = 'a';
    name[13] = 'm';
    name[14] = 'e';
    name[15] = 0;

    //Initial values for instrument
    waveIndex = 0;
    volEntries=1;
    volTable = new unsigned short[INST_VOL_SIZE];//64 integers worth
    for(int i = 0; i < INST_VOL_SIZE; i++)
        volTable[i]=0;
    volTable[0] = 0x3F00;
    pulseIndex = 0xFFFF;
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
    pulseIndex       = other.pulseIndex;
    volEntries       = other.volEntries;
    volTable = new unsigned short[INST_VOL_SIZE];//64 integers worth
    for(int i = 0; i < INST_VOL_SIZE; i++) //Why was this 255 and not 256?
        volTable[i] = other.volTable[i];

}


std::ostream &Instrument::output(std::ostream &out) const
{
    out.write(name, INST_NAME_SIZE);
    out.write((char*)&waveIndex, sizeof(short));
    out.write((char*)&pulseIndex, sizeof(short));

    out.write((char*)&volEntries, sizeof(char));
    out.write((char*)volTable, volEntries*sizeof(short));

    return out;	
}

std::istream &Instrument::input(std::istream &in)
{
    if(volTable)
        delete [] volTable;

    in.read(name, INST_NAME_SIZE);
    in.read((char*)&waveIndex, sizeof(short));
    in.read((char*)&pulseIndex, sizeof(short));
    in.read((char*)&volEntries, sizeof(char));
    volTable = new unsigned short[INST_VOL_SIZE];
    in.read((char*)volTable, volEntries*sizeof(short));
    for(int i = volEntries; i < INST_VOL_SIZE; i++)
        volTable[i] = 0;

    return in;
}

void Instrument::setVolTable(unsigned short *table)
{
    for(int i = 0; i < INST_VOL_SIZE; i++)
        volTable[i] = table[i];
}

sample_res_unsigned Instrument::getVolume(unsigned char &index, unsigned char &seg, unsigned char &jump_param, sample_res_unsigned &last)
{
    //Destination time, the total amount of time it should take to get to the destination volume
    float desttime = (volTable[index] & 0x00FF);
    //Detination volume to interpolate toward
    unsigned char destvol = (volTable[index] & 0xFF00) >> 8;

    //If this volume entry is the FB function (set loop counter) or FD (Set custom jump) set the
    //jump variable to desttime, the right side of 0xFBXX
    if(destvol==0xFB || destvol == 0xFD)
    {
        jump_param = desttime;
        if(index < volEntries-1)
        {
            index++;
            desttime = (volTable[index] &0x00FF);
            destvol = (volTable[index] &0xFF00) >> 8;
        }
    }

    //Depth is used to make sure that the volume table doesn't enter an infinite loop of jumps
    unsigned char depth = 0;

    while(destvol==0xFF || destvol == 0xFC || destvol == 0xFE)
    {
        
        depth++;
        if(depth > INST_VOL_LOOP_MAX_JUMPS)
        {
            last = 0;
            return 0;
        }

        if(destvol == 0xFF) //Jump function
        {
            index = desttime;
        } else if (destvol == 0xFC) //Decrement `jump`, jump to desttime if `jump` != 0
        {
            jump_param--;
            if(jump_param == 0 && index < volEntries-1)
                index++;
            else
                index = desttime;
        } else if (destvol == 0xFE) //Jump to custom jump (offset by desttime, most cases 0)
        {
            index = jump_param + desttime;
        }

        //Prepare for the next iteration
        //Continue to make sure `index` doens't point to 
        desttime = (volTable[index] &0x00FF);
        destvol = (volTable[index] &0xFF00) >> 8;
        seg = 0;
    }

    //It's possible for FB or FD to occur directly after a loop.
    //Without this extra if here, it would treat FB or FD as a
    //regular volume (very loud)
    if(destvol==0xFB || destvol == 0xFD)
    {
        jump_param = desttime;
        if(index < volEntries-1)
        {
            index++;
            desttime = (volTable[index] &0x00FF);
            destvol = (volTable[index] &0xFF00) >> 8;
        }
    }

    //If the appropriate amount of segments have passed
    //proceed to the next index of the volume table.
    if(seg >= desttime)
    {
        seg= 0;
        if(index < volEntries-1)
            index++;
        last = ((sample_res_unsigned) -1)*(destvol/255.0);
        return last;
    }
    

    //Interpolate between the previous volume and destination volume over the duration
    if(index >0)
    {

        //last = last + ((0xFFFF*(dest/255.0) - last) * (seg/desttime));
        //return  last; 

        return last + ((((sample_res_unsigned)-1)*(destvol/255.0)-last)*(seg/desttime));

        //interpolate
        //return last + ((static_cast<float>(destvol) - last) * (seg/desttime));
    }

    last = ((sample_res_unsigned) -1)*(destvol/255.0);
    return last;
}



void Instrument::fixVolJumps(const unsigned char &index, short difference)
{
    //If there was no insertion or delection from the volume table, return.
    if(difference == 0)
        return;

    //Loop through the table locating jump functions whose jump destinations
    //lie after or on the index where the insertion or deletion happened.
    //Change the jump destination according to the difference.
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

    //Push back all entries on or after this index
    for(unsigned short last = ++volEntries; last > index; last--)
        volTable[last] = volTable[last-1];
    fixVolJumps(index, 1);

    //Set the new entry
    volTable[index] = entry;
    return true;
}

bool Instrument::removeVolEntry(unsigned char index)
{
    if(volEntries <= 1) //Dont let you remove the last one
        return false;

    //Pop back all entries on or after this index
    for(unsigned short i = index+1; i < volEntries; i++)
        volTable[i-1] = volTable[i];
    fixVolJumps(index, -1);
    volTable[--volEntries] = 0;

    //Make sure that if something is pushed into the first entry the inst doesn't try to interpolate garbage data
    volTable[0] &= 0xFF00;
    return true;
}






