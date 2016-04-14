#ifndef INSTRUMENT_H_
#define INSTRUMENT_H_

#include<istream>
#include<ostream>

class Instrument
{
    private:
        char name[23]{0};
        unsigned short waveIndex;
        unsigned short pulseIndex;

        
        unsigned char volEntries;
        unsigned short *volTable;
    public:

        ~Instrument();
        Instrument();


/***\//////////////////////////////////////////////////////////////////////////        
Function: Instrument(std::istream &in)
Description:
    Creates a new instrument from an input stream
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        Instrument(std::istream &in);

/***\//////////////////////////////////////////////////////////////////////////        
Function: Instrument(const Instrument &other)
Description:
   Copy constructor
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        Instrument(const Instrument &other);


/***\//////////////////////////////////////////////////////////////////////////        
Function: output(std::ostream &out) const
Description:
    Write an instrument to an input stream
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        std::ostream &output(std::ostream &out) const;

/***\//////////////////////////////////////////////////////////////////////////        
Function: input(std::istream &in)
Description:
   Read an instrument from an input stream
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        std::istream &input(std::istream &in);

        inline void setWaveIndex(short i){waveIndex = i;}
        inline void setPulseIndex(short i){pulseIndex = i;}
        

        inline void setVolEntry(unsigned char index, unsigned short entry){volTable[index] = entry;}

/***\//////////////////////////////////////////////////////////////////////////        
Function: void setVolTable(unsigned short *table)
Description:
   Copies over another volume table to this instrument's
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        void setVolTable(unsigned short *table);
        inline char *getName(){return name;}

        inline unsigned short getWaveIndex() const{return waveIndex;}
        inline unsigned short getPulseIndex() const{return pulseIndex;}
        inline unsigned short getVolEntry(unsigned char index) const {return volTable[index];}
        inline unsigned char numVolEntries() const {return volEntries;}
        inline unsigned short *getVolTable() const {return volTable;}


/***\//////////////////////////////////////////////////////////////////////////        
Function: bool insertVolEntry(unsigned char index, unsigned short entry)
Description:
    Inserts a new entry to the volume table at an index, then fixes jumps
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        bool insertVolEntry(unsigned char index, unsigned short entry);

/***\//////////////////////////////////////////////////////////////////////////        
Function: bool removeVolEntry(unsigned char index)
Description:
   Removes the specified entry in the vol table, then fixes jumps
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        bool removeVolEntry(unsigned char index);

/***\//////////////////////////////////////////////////////////////////////////        
Function: void fixVolJumps(const unsigned char &index, short difference)
Description:
   If an entry of the volume table is deleted then fix references to
   indicies that have moved
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        void fixVolJumps(const unsigned char &index, short difference);


/***\//////////////////////////////////////////////////////////////////////////        
Function: char getVolume(unsigned char &cur, unsigned char &seg, unsigned char &last)
Description
   Interpolates what volume this instrument should be at for the specified index.
   seg should be an accumulation of the number of row segments that have passed
   since the last time the inst changed current vol table index (cur)
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        unsigned char getVolume(unsigned char &cur, unsigned char &seg, unsigned char &jump, unsigned char &last);

};

#endif
