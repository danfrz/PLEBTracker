#ifndef INSTRUMENT_H_
#define INSTRUMENT_H_

#include<istream>
#include<ostream>

#define INST_NAME_SIZE 23
#define INST_VOL_SIZE 256
#define INST_VOL_LOOP_MAX_JUMPS 64

typedef short sample_res; //The sample resolution used for most of the interpreter
typedef unsigned short sample_res_unsigned; //The resolution that is gauranteed to be unsigned
typedef short sample_res_signed; //The resolution that is gauranteed to be signed

#define SAMPLE_RES_IS_UNSIGNED 0

class Instrument
{
    private:
        char name[INST_NAME_SIZE]{0};
        unsigned short waveIndex;
        unsigned short pulseIndex;
        unsigned short filterIndex;

        
        unsigned char volEntries;
        unsigned short *volTable;
    public:

        ~Instrument();
        Instrument();


/***\//////////////////////////////////////////////////////////////////////////        
Function: Instrument(std::istream &in)
Description:
    Read a new instrument from an input stream
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


/***\//////////////////////////////////////////////////////////////////////////        
Function: void setWaveIndex(short i)

Description:
   Set the wave index for this instrument
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        inline void setWaveIndex(short i){waveIndex = i;}

/***\//////////////////////////////////////////////////////////////////////////        
Function: void setPulseIndex(short i)

Description:
   Set the pulse index for this instrument
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        inline void setPulseIndex(short i){pulseIndex = i;}
        

/***\//////////////////////////////////////////////////////////////////////////        
Function: void setFilterIndex(short i)

Description:
   Set the pulse index for this instrument
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        inline void setFilterIndex(short i){filterIndex = i;}


/***\//////////////////////////////////////////////////////////////////////////        
Function: void setVolEntry(unsigned char index, unsigned short entry){volTable[index] = entry;

Description:
   Set the volume at the specified index of this instrument's volume table
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        inline void setVolEntry(unsigned char index, unsigned short entry){volTable[index] = entry;}

/***\//////////////////////////////////////////////////////////////////////////        
Function: void setVolTable(unsigned short *table)
Description:
   Copies over another volume table to this instrument's
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        void setVolTable(unsigned short *table);

/***\//////////////////////////////////////////////////////////////////////////        
Function: getName(){return name;

Description:
   Get the pointer to this instrument's name.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        inline char *getName(){return name;}


/***\//////////////////////////////////////////////////////////////////////////        
Function: short getWaveIndex()

Description:
   Get the wave index for this instrument.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        inline unsigned short getWaveIndex() const{return waveIndex;}

/***\//////////////////////////////////////////////////////////////////////////        
Function: short getPulseIndex()

Description:
   Get the pulse index for this instrument.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        inline unsigned short getPulseIndex() const{return pulseIndex;}


/***\//////////////////////////////////////////////////////////////////////////        
Function: short getPulseIndex() 

Description:
   Get the filter index for this instrument
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        inline unsigned short getFilterIndex() const{return filterIndex;}


/***\//////////////////////////////////////////////////////////////////////////        
Function: short getVolEntry(unsigned char index) const {return volTable[index];

Description:
   Get the volume a the specified index of this instrument's volume table
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        inline unsigned short getVolEntry(unsigned char index) const {return volTable[index];}

/***\//////////////////////////////////////////////////////////////////////////        
Function: char numVolEntries() const {return volEntries;

Description:
   Get the logical size of the volume table.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        inline unsigned char numVolEntries() const {return volEntries;}

/***\//////////////////////////////////////////////////////////////////////////        
Function: getVolTable() const {return volTable;

Description:
   Get a pointer to this instrument's volume table.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        inline unsigned short *getVolTable() const {return volTable;}


/***\//////////////////////////////////////////////////////////////////////////        
Function: bool insertVolEntry(unsigned char index, unsigned short entry)
Description:
    Inserts a new entry to the volume table at an index, then fixes jumps
    
    Returns false if another entry can not be added to this instrument's
    volume table
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        bool insertVolEntry(unsigned char index, unsigned short entry);

/***\//////////////////////////////////////////////////////////////////////////        
Function: bool removeVolEntry(unsigned char index)
Description:
   Removes the specified entry in the vol table, then fixes jumps

   Returns false if there is already only one entry in the volume table
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
Function: char getVolume(unsigned char &index, unsigned char &seg, unsigned char &last)
Description
   Interpolates what volume this instrument should be at for the specified index.
   seg should be an accumulation of the number of row segments that have passed
   since the last time the inst changed current vol table index (cur)
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        sample_res_unsigned getVolume(unsigned char &index, unsigned char &seg, unsigned char &jump_param, sample_res_unsigned &last);

};

#endif
