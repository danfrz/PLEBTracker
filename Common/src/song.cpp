#include "song.h"
//#include <iostream>

Song::Song()
{
    bytes_per_row = 0x1CB0;
    interrow_resolution = 0x18;

    for(int i = 4; i < 29; i++)
        songname[i] = 0;

    songname[0]='N';
    songname[1]='a';
    songname[2]='m';
    songname[3]='e';

    num_instruments = 1;
    instruments = new Instrument*[256];
    Instrument *defaultInst = new Instrument();
    instruments[0] = defaultInst;

    tracks = 4;
    num_patterns = 1;
    patterns = new Pattern*[256];
    Pattern *defaultPat = new Pattern(tracks, 64);
    patterns[0] = defaultPat;

    for(int i = 1; i < 256; i++)
    {
        instruments[i] = NULL;
        patterns[i] = NULL;
    }

    num_orders = 1;
    orders = new unsigned char[256];
    orders[0] = 0;

    waveEntries = 0;
    waveTable = new unsigned short[256];
    for(int i = 0; i < waveEntries; i++)
        waveTable[i] = 0;

    pulseEntries = 0;
    pulseTable = new unsigned short[256];
    for(int i = 0; i < pulseEntries; i++)
        pulseTable[i] = 0;
}

Song::Song(std::istream &in)
{
    patterns = NULL;
    instruments = NULL;
    orders = new unsigned char[256];
    waveTable = new unsigned short[256];
    pulseTable = new unsigned short[256];

    pulseEntries = 0;

    input(in);
}

Song::~Song()
{
    delete [] instruments;
    delete [] patterns;
    delete [] orders;
    delete [] waveTable;
    delete [] pulseTable;

}



std::ostream &Song::output(std::ostream &out) const
{

    out.write(songname, 29);
    out.write((char*)&bytes_per_row, sizeof(short));
    out.write((char*)&interrow_resolution, 1);
    out.write((char*)&tracks, 1);

    out.write((char*)&num_orders, 1);
    out.write((char*)orders, num_orders);

    out.write((char*)&waveEntries, sizeof(short));
    out.write((char*)waveTable, waveEntries*sizeof(short));

    out.write((char*)&pulseEntries, sizeof(short));
    out.write((char*)pulseTable, pulseEntries*sizeof(short));

    out.write((char*)&num_instruments, 1);
    for(int i = 0; i < num_instruments; i++)
        (instruments[i])->output(out);
    

    out.write((char*)&num_patterns, 1);
    for(int i = 0; i < num_patterns; i++)
        (patterns[i])->output(out);
    return out;
}

std::istream &Song::input(std::istream &in)
{
    delete [] instruments;
    delete [] patterns;


    in.read(songname, 29);
    in.read((char*)&bytes_per_row, sizeof(short));
    in.read((char*)&interrow_resolution, 1);
    in.read((char*)&tracks, 1);

    in.read((char*)&num_orders, 1);
    in.read((char*)orders, num_orders);


    in.read((char*)&waveEntries, sizeof(short));
    in.read((char*)waveTable, waveEntries*sizeof(short));
    for(int i = waveEntries; i < 256; i++)
        waveTable[i] = 0;

    
    in.read((char*)&pulseEntries, sizeof(short));
    in.read((char*)pulseTable, pulseEntries*sizeof(short));
    for(int i = pulseEntries; i < 256; i++)
        pulseTable[i] = 0;
        

    instruments = new Instrument*[256];
    in.read((char*)&num_instruments, 1);
    for(int i = 0; i < num_instruments; i++)
        instruments[i] = new Instrument(in);	
    for(int i = num_instruments; i < 256; i++)
        instruments[i] = NULL;

    patterns = new Pattern*[256];
    in.read((char*)&num_patterns, 1);
    for(int i = 0; i < num_patterns; i++)
        patterns[i] = new Pattern(in);
    for(int i = num_patterns; i < 256; i++)
        patterns[i] = NULL;

    return in;
}

void Song::copyCommutable(Song *other)
{
    unsigned short *otrwavebl = other->getWaveTable();
    for(int i = 0; i < waveEntries; i++)
        other->insertWaveEntry(i,waveTable[i]);

    unsigned short *otrpulsebl = other->getPulseTable();
    for(int i = 0; i < pulseEntries; i++)
        other->insertPulseEntry(i,pulseTable[i]);

    other->setInterRowResolution(interrow_resolution);
    other->setBytesPerRow(bytes_per_row);
    other->setTrackNum(tracks);
    
    for(int i = 0; i < num_instruments; i++)
        other->addInstrument(new Instrument(*instruments[i]));

}

Song *Song::makeExcerpt(unsigned char orderstart, unsigned char orderend, unsigned char rowstart, unsigned char rowend)
{
    unsigned int len = 0;
    for(int orderi = orderstart; orderi <= orderend; orderi++)
        len += getPatternByOrder(orderi)->numRows();
    len -= rowstart;
    len -= getPatternByOrder(orderend)->numRows() - orderend+1;
    //std::cerr << "Excerpt5\n";
    Song *out = new Song();
    copyCommutable(out);
    out->removeInstrument(0);
    //std::cerr << "Excerpt6\n";

    Pattern *first, *last;

    if(orderstart == orderend)
    {
        //std::cerr << "Excerpt7\n";
        first = new Pattern(*getPatternByOrder(orderstart)); 
        first->chop(rowstart, rowend);
        out->addPattern(first);
        out->insertOrder(0,1);
        //Clear the default order and patterns
        out->removeOrder(1);
        //out->removePattern(0);


        //std::cerr << "Excerpt8: " << int(out->numOrders()) << " " << int(out->numPatterns()) << " \n";
    }
    else
    {
        //std::cerr << "ExcerptA\n";
        first = new Pattern(*getPatternByOrder(orderstart));
        last = new Pattern(*getPatternByOrder(orderend));

        first->chop(rowstart, first->numRows()-1);
        last->chop(0,rowend);

        //std::cerr << "ExcerptB\n";
        out->addPattern(first);
        out->removePattern(0);
        out->insertOrder(0,0);
        out->removeOrder(1);//default order
        out->addPattern(last);
        out->insertOrder(1,1);
        for(int orderi = orderstart+1, i = 1; orderi < orderend; orderi++, i++)
        {
            //Have to create new patterns because Song will
            //run destructor
            out->addPattern(new Pattern(*getPatternByOrder(orderi)));
            out->insertOrder(i,i+1);
        }


        //std::cerr << "ExcerptC\n";

    }
    //std::cerr << "Excerpt Done\n";

    
    return out;

}


Song *Song::makeExcerpt(unsigned int length, unsigned char orderstart, unsigned char rowstart)
{

    unsigned char rowend, orderend;
    unsigned int acc = getPatternByOrder(orderstart)->numRows() - rowstart;
    unsigned char orderi = orderstart;

    //std::cerr << "Excerpt1\n";
    while (acc < length && ++orderi < num_orders)
    {
        acc += getPatternByOrder(orderi)->numRows();
        
    }
    //std::cerr << "Excerpt2\n";

    if(orderi >= num_orders)
        orderend = num_orders-1;
    else
        orderend = orderi;
    //std::cerr << "Excerpt3\n";

    if(orderstart == orderend)
    {
        rowend = rowstart + length;
        if(rowend >= getPatternByOrder(orderstart)->numRows())
            rowend = getPatternByOrder(orderstart)->numRows()-1;
    }
    else
    {
        rowend = length - (acc - getPatternByOrder(orderend)->numRows());
    }
    
    //std::cerr << "Excerpt4\n";
    //std::cerr << "The excerpt: " << int(orderstart) << " "<< int(orderend) << " "<< int(rowstart) << " " << int(rowend) << "\n";
    return makeExcerpt(orderstart, orderend, rowstart, rowend);
}




void Song::setName(const char *name, int length)
{
    if(length > 28)
        length = 28;
    for(int i = 0; i < length; i++)
        songname[i] = name[i];
}

bool Song::insertOrder(unsigned char dest, unsigned char pattern)
{
    if(pattern >= num_patterns)
        return false;
    if(dest >num_orders)
        return false;
    for(int i=(num_orders++)-1; i >= dest; i--)
        orders[i+1] = orders[i];
    orders[dest] = pattern;

    return true;
}

bool Song::removeOrder(unsigned char ordr)
{
    if(ordr >= num_orders || num_orders == 1)
        return false;
    num_orders--;
    for(int i=ordr; i < num_orders; i++)
        orders[i] = orders[i+1];

    return true;
}


void Song::setTrackNum(const unsigned newtracks)
{
    tracks = newtracks;
    //loop through every pattern and change tracks
    for(int i = 0; i < num_patterns; i++)
        patterns[i]->setTrackNum(newtracks);
}

bool Song::newPattern(unsigned int tracks, unsigned int rows)
{
    if(num_patterns == 255)
        return false;
    Pattern *newPat = new Pattern(tracks,rows);
    patterns[num_patterns++] = newPat;
    return true;
}

bool Song::clonePattern(unsigned char src)
{
    if(num_patterns == 255)
        return false;

    Pattern *source = patterns[src];
    Pattern *newPat = new Pattern(*source);
    patterns[num_patterns++] = newPat;
    return true;
}

bool Song::clearPattern(unsigned char ptrn)
{
    patterns[ptrn]->clear();
}

bool Song::removePattern(unsigned char ptrn)
{
    if(ptrn >= num_patterns || num_patterns == 1)
        return false;

    delete patterns[ptrn];
    for(int i = ptrn+1; i < num_patterns; i++)
        patterns[i-1] = patterns[i];

    num_patterns--;

    patterns[num_patterns] = NULL;

    //then update orders to the new pattern indicies!

    //Set to the first pattern
    //so that it stands out and it would stand out looking through order list
    for(int i = 0; i < num_orders; i++)
        if(orders[i] == ptrn)
            orders[i] = 0;

    return true;
}


void Song::addPattern(Pattern *ptrn)
{
    if(num_patterns == 0xFF)
        return;
    patterns[num_patterns] = ptrn;
    num_patterns++;
}


bool Song::newInstrument()
{
    if(num_instruments == 255)
        return false;

    Instrument *defaultInst = new Instrument();
    instruments[num_instruments++] = defaultInst;

    return true;
}

bool Song::cloneInstrument(unsigned char inst)
{
    if(num_instruments == 255)
        return false;

    Instrument *srcInst = instruments[inst];
    Instrument *newInst = new Instrument(*srcInst);
    instruments[num_instruments++] = newInst;

    return true;
}

bool Song::removeInstrument(unsigned char inst)
{
    if(inst >= num_instruments || num_instruments == 1)
        return false;

    delete instruments[inst];
    inst+=1;
    while(inst < num_instruments)
        instruments[inst-1] = instruments[inst++];
    num_instruments--;
    //then update patterns with new instrument indicies
    for(int i = 0; i < num_patterns; i++)
        patterns[i]->purgeInstrument(inst);


    return true;
}


void Song::addInstrument(Instrument *inst)
{
    instruments[num_instruments++]=inst;
}




bool Song::insertWaveEntry(unsigned short index, unsigned short entry)
{
    if(waveEntries == 0xFFFF)
        return false;
    for(unsigned short last = ++waveEntries; last > index; last--)
        waveTable[last] = waveTable[last-1];

    fixWaveJumps(index, 1);
    
    waveTable[index] = entry;
    return true;
}

bool isJumpFunc(const unsigned short &wave)
{
    unsigned char func = (wave & 0xFF00) >> 8;
    switch(func)
    {
        case 0xFF:
        case 0xFC:
        case 0xFD:
            return true;
    }
    return false;
}

void Song::fixWaveJumps(const unsigned short &index, short difference)
{

    if(difference == 0) return;

    unsigned char instwav;
    if(difference > 0)
    {
        for(unsigned char i = 0; i < num_instruments; i++)
        {
            instwav = instruments[i]->getWaveIndex();
            // > 0 because the first instrument's wave pointer shouldn't
            // realistically change due to insertions at index 0
            if(instwav > 0 && instwav >= index && instwav < 0xFFFF-difference)
            {
                instruments[i]->setWaveIndex(instwav+difference);
            }
        }
    }
    else
    {
        for(unsigned char i = 0; i < num_instruments; i++)
        {
            instwav = instruments[i]->getWaveIndex();
            if(instwav >= index)
            {
                if(instwav >= -difference)
                    instruments[i]->setWaveIndex(instwav +difference);
                else
                    instruments[i]->setWaveIndex(0);
            }
        }
    }

    unsigned short jumptype;
    unsigned short dest;
    if(difference > 0)
    {
        for(unsigned short i = 0; i < waveEntries; i++)
        {
            if( isJumpFunc(waveTable[i]))//is jump, correct it
            {
                jumptype = waveTable[i] & 0xFF00;
                if( (i < waveEntries-1) && ((waveTable[i+1]&0xFF00) == jumptype))//Long jump
                {
                    dest = ((waveTable[i] & 0xFF) << 8) | (waveTable[i+1] & 0xFF);
                    if(dest >= index && dest < 0xFFFF-difference) 
                    {
                        dest += difference;
                        waveTable[i] = jumptype | ((dest & 0xFF00) >> 8);
                        waveTable[i+1] = jumptype | (dest & 0xFF);
                    }
                    i++;
                }
                else
                {
                    dest = waveTable[i] & 0xFF;
                    if(dest >= index)
                    {
                        if(dest >= 0xFF - difference)
                        {

                            if(waveEntries < 0xFFFF)
                            {
                                dest += difference;
                                waveTable[i] &= 0xFF00;
                                waveTable[i] |= ((dest & 0xFF00) >> 8);
                                insertWaveEntry(i+1,jumptype | (dest & 0xFF));
                            }
                        }
                        else
                            waveTable[i]+=difference;
                    }
                }
            }
        }
    }

    else //difference negative
    {
        for(unsigned short i = 0; i < waveEntries; i++)
        {
            if( isJumpFunc(waveTable[i]))//is jump, correct it
            {
                jumptype = waveTable[i] & 0xFF00;
            
                if( (i < waveEntries-1) && ((waveTable[i+1] & 0xFF00) == jumptype) )//Long jump
                {
                    dest = ((waveTable[i] & 0xFF) << 8) | (waveTable[i+1] & 0xFF);
                    if(dest >= index && dest >= -difference) 
                    {
                        dest +=  difference;

                        waveTable[i] = jumptype | ((dest & 0xFF00) >> 8);
                        waveTable[i+1] = jumptype | (dest & 0xFF);
                    }
                    i++;
                }
                else
                {
                    dest = waveTable[i] & 0xFF;
                    if(dest >= index)
                    {
                        if(dest >= -difference)
                            waveTable[i]+= difference;
                        else
                            waveTable[i] &= 0xFF00;
                    }
                }

            }
        }
    }

    Pattern *p;
    for(int i = 0; i < num_patterns; i++)
    {
        p = patterns[i];
        for(int trk = 0; trk < p->numTracks(); trk++)
        {
            for(int row = 0; row < p->numRows(); row++)
            {
                unsigned int entry = p->at(trk,row);
                if((entry & 0xF00) == 0x900)
                {
                    unsigned char wavejump = entry & 0xFF;

                    if(difference > 0)
                    {
                        if(wavejump > 0 &&wavejump >= index && wavejump < 0xFF-difference)
                        {
                            wavejump +=difference;
                        }
                    }
                    else
                    {
                        if(wavejump >= index)
                        {
                            if(wavejump >= -difference)
                                wavejump +=difference;
                            else
                                wavejump = 0;
                        }
                    }
                    entry &= ~0xFF;
                    entry |= wavejump;
                    p->setAt(trk,row,entry);
                }
            }
        }
    }

}

bool Song::removeWaveEntry(unsigned short index)
{
    if(waveEntries == 0)
        return false;

    for(unsigned short i = index+1; i < waveEntries; i++)
        waveTable[i-1] = waveTable[i];
    waveTable[--waveEntries] = 0;
    fixWaveJumps(index, -1);
    return true;
}


bool Song::insertPulseEntry(unsigned short index, unsigned short entry)
{
    if(pulseEntries == 0xFFFF)
        return false;
    for(unsigned short last = ++pulseEntries; last > index; last--)
        pulseTable[last] = pulseTable[last-1];

    fixPulseJumps(index, 1);
    
    pulseTable[index] = entry;
    return true;
}

void Song::fixPulseJumps(const unsigned short &index, short difference)
{

    if(difference == 0) return;

    //Fix instrument pulse index references
    unsigned char instpls;
    if(difference > 0)
    {
        for(unsigned char i = 0; i < num_instruments; i++)
        {
            instpls = instruments[i]->getPulseIndex();
            // > 0 because the first instrument's wave pointer shouldn't
            // realistically change due to insertions at index 0
            if(instpls > 0 && instpls >= index && instpls < 0xFFFF-difference)
            {
                instruments[i]->setPulseIndex(instpls+difference);
            }
        }
    }
    else
    {
        for(unsigned char i = 0; i < num_instruments; i++)
        {
            instpls = instruments[i]->getPulseIndex();
            if(instpls >= index)
            {
                if(instpls >= -difference)
                    instruments[i]->setPulseIndex(instpls +difference);
                else
                    instruments[i]->setPulseIndex(0);
            }
        }
    }

    //Fix Pulse jumps
    unsigned short jumptype;
    unsigned short dest;
    if(difference > 0)
    {
        for(unsigned short i = 0; i < pulseEntries; i++)
        {
            if( isJumpFunc(pulseTable[i]))//is jump, correct it
            {
                jumptype = pulseTable[i] & 0xFF00;
                if( (i < pulseEntries-1) && ((pulseTable[i+1]&0xFF00) == jumptype))//Long jump
                {
                    dest = ((pulseTable[i] & 0xFF) << 8) | (pulseTable[i+1] & 0xFF);
                    if(dest >= index && dest < 0xFFFF-difference) 
                    {
                        dest += difference;
                        pulseTable[i] = jumptype | ((dest & 0xFF00) >> 8);
                        pulseTable[i+1] = jumptype | (dest & 0xFF);
                    }
                    i++;
                }
                else
                {
                    dest = pulseTable[i] & 0xFF;
                    if(dest >= index)
                    {
                        if(dest >= 0xFF - difference)
                        {

                            if(waveEntries < 0xFFFF)
                            {
                                dest += difference;
                                pulseTable[i] &= 0xFF00;
                                pulseTable[i] |= ((dest & 0xFF00) >> 8);
                                insertPulseEntry(i+1,jumptype | (dest & 0xFF));
                            }
                        }
                        else
                            pulseTable[i]+=difference;
                    }
                }
            }
        }
    }

    else //difference negative
    {
        for(unsigned short i = 0; i < pulseEntries; i++)
        {
            if( isJumpFunc(pulseTable[i]))//is jump, correct it
            {
                jumptype = pulseTable[i] & 0xFF00;
            
                if( (i < pulseEntries-1) && ((pulseTable[i+1] & 0xFF00) == jumptype) )//Long jump
                {
                    dest = ((pulseTable[i] & 0xFF) << 8) | (pulseTable[i+1] & 0xFF);
                    if(dest >= index && dest >= -difference) 
                    {
                        dest +=  difference;

                        pulseTable[i] = jumptype | ((dest & 0xFF00) >> 8);
                        pulseTable[i+1] = jumptype | (dest & 0xFF);
                    }
                    i++;
                }
                else
                {
                    dest = pulseTable[i] & 0xFF;
                    if(dest >= index)
                    {
                        if(dest >= -difference)
                            pulseTable[i]+= difference;
                        else
                            pulseTable[i] &= 0xFF00;
                    }
                }

            }
        }
    }

    Pattern *p;
    for(int i = 0; i < num_patterns; i++)
    {
        p = patterns[i];
        for(int trk = 0; trk < p->numTracks(); trk++)
        {
            for(int row = 0; row < p->numRows(); row++)
            {
                unsigned int entry = p->at(trk,row);
                if((entry & 0xF00) == 0xC00)
                {
                    unsigned char plsjump = entry & 0xFF;

                    if(difference > 0)
                    {
                        if(plsjump > 0 &&plsjump >= index && plsjump < 0xFF-difference)
                        {
                            plsjump +=difference;
                        }
                    }
                    else
                    {
                        if(plsjump >= index)
                        {
                            if(plsjump >= -difference)
                                plsjump +=difference;
                            else
                                plsjump = 0;
                        }
                    }
                    entry &= ~0xFF;
                    entry |= plsjump;
                    p->setAt(trk,row,entry);
                }
            }
        }
    }

}

bool Song::removePulseEntry(unsigned short index)
{
    if(pulseEntries == 0)
        return false;

    for(unsigned short i = index+1; i < pulseEntries; i++)
        pulseTable[i-1] = pulseTable[i];
    pulseTable[--pulseEntries] = 0;
    fixPulseJumps(index, -1);
    return true;
}
