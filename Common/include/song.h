#ifndef SONG_H_
#define SONG_H_

#include "instrument.h"
#include "pattern.h"
#include <istream>
#include <ostream>

bool isJumpFunc(const unsigned short &wave);
const unsigned int SONGNAME_LENGTH = 29;

class Song{
    private:
        char songname[SONGNAME_LENGTH];
        unsigned char tracks;
        unsigned short  bytes_per_row;
        unsigned char interrow_resolution;

        unsigned char num_orders;
        unsigned char num_patterns;
        unsigned char *orders;
        Pattern **patterns;

        unsigned char num_instruments;
        Instrument **instruments;

        unsigned short waveEntries;
        unsigned short *waveTable;

        unsigned short pulseEntries;
        unsigned short *pulseTable;


/***\//////////////////////////////////////////////////////////////////////////        
Function: void copyCommutable(Song *other)
Description:
   Copies all fields from another song besides for pattern data
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        void copyCommutable(Song *other);

    public:

/***\//////////////////////////////////////////////////////////////////////////    
Function: Song()
Description:
    Creates a new song with default values.
    Name is set to "Name"

    instruments is initialized to an array of 256 instruments pointers
    and instrument 0 is set the default instrument (new Instrument())

    patterns is initialized to an array of 256 pattern pointers
    and pattern 0 is set to an empty pattern
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        Song();


/***\//////////////////////////////////////////////////////////////////////////        
Function: Song(std::istream &in)
Description:
   Reads a song from an input stream, overwrites and discards any old data
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        Song(std::istream &in);

        ~Song();



/***\//////////////////////////////////////////////////////////////////////////        
Function: output(std::ostream &out) const
Description:
    Write a song to an input stream
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        std::ostream &output(std::ostream &out) const;

/***\//////////////////////////////////////////////////////////////////////////        
Function: input(std::istream &in)
Description:
   Read a song from an input stream
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        std::istream &input(std::istream &in);

/***\//////////////////////////////////////////////////////////////////////////        
Function: makeExcerpt(unsigned char orderstart, unsigned char orderend, unsigned char rowstart, unsigned char rowend)
Description:
   Creates a new song that is a copy of this song, but the patterns are cut
   from the start(+rowstart) to end(+rowend) orders
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        Song *makeExcerpt(unsigned char orderstart, unsigned char orderend, unsigned char rowstart, unsigned char rowend);

/***\//////////////////////////////////////////////////////////////////////////        
Function: Song *makeExcerpt(unsigned int length, unsigned char patstart, unsigned char rowstart)
Description:
   Creates a new song that is a copy of this song, but the patterns are cut
   to the selected order, then `length` rows after the beginning of that pattern
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        Song *makeExcerpt(unsigned int length, unsigned char orderstart, unsigned char rowstart);


        inline char *getName(){return songname;}

/***\//////////////////////////////////////////////////////////////////////////        
Function: void setName(const char *name, int length)
Description:
   Set the name of this song
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        void setName(const char *name, int length);

/***\//////////////////////////////////////////////////////////////////////////        
Function: void setBytesPerRow(unsigned short bpr)
Description:
   Sets the byte samples size produces by one row
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        inline void setBytesPerRow(unsigned short bpr){bytes_per_row = bpr;}

/***\//////////////////////////////////////////////////////////////////////////        
Function: void setInterRowResolution(unsigned char res)
Description:
   Sets the subdivision of the bytes per row 
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\__///
        inline void setInterRowResolution(unsigned char res){interrow_resolution = res;}

/***\//////////////////////////////////////////////////////////////////////////        
Function: char getInterRowResolution()
Description:
   Get the number of segments in a row
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\_///
        inline unsigned char getInterRowResolution(){return interrow_resolution;}

/***\//////////////////////////////////////////////////////////////////////////        
Function: void setTrackNum(const unsigned newtracks)
Description:
    Set the number of tracks
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        void setTrackNum(const unsigned newtracks);


        inline unsigned char numInstruments() const {return num_instruments;}
        inline unsigned char numOrders() const {return num_orders;}
        inline unsigned char numTracks() const {return tracks;}
        inline unsigned char numPatterns() const {return num_patterns;}

        inline unsigned int getBytesPerRow() const {return bytes_per_row;}
        inline unsigned char getInterrowRes() const {return interrow_resolution;}
        inline unsigned short numWaveEntries() const {return waveEntries;}
        inline unsigned short numPulseEntries() const {return pulseEntries;}

        inline void setPatternIndexByOrder(unsigned const char &order, const unsigned char &pattern){orders[order] = pattern;}

        inline Pattern *getPattern(unsigned const char &ptrn) const {return patterns[ptrn];}
        inline Pattern *getPatternByOrder(unsigned const char &order) const {return patterns[orders[order]];}
        inline Instrument *getInstrument(unsigned const char &inst) const {if(inst >= num_instruments) return NULL; else return instruments[inst];}
        inline unsigned char getPatternIndexByOrder(unsigned const char &order) const {return orders[order];}


/***\//////////////////////////////////////////////////////////////////////////        
Function: bool insertOrder(unsigned char dest, unsigned char pattern)
Description:
   Insert a new order that points to a pattern at a certain position in the
   order list
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        bool insertOrder(unsigned char dest, unsigned char pattern);

/***\//////////////////////////////////////////////////////////////////////////        
Function: bool removeOrder(unsigned char ordr)
Description:
   Remove an order at a certain position in the order list
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\__///
        bool removeOrder(unsigned char ordr);


/***\//////////////////////////////////////////////////////////////////////////        
Function: bool newPattern(unsigned int tracks, unsigned int rows)
Description:
   Create a new pattern with the provided number of tracks and rows
   and add it to the end of the pattern list
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\_///
        bool newPattern(unsigned int tracks, unsigned int rows); //shouldn't this use tracks from local?

/***\//////////////////////////////////////////////////////////////////////////        
Function: bool clonePattern(unsigned char src)
Description:
    Clone the specified pattern and at it to the end of the pattern list
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        bool clonePattern(unsigned char src);


/***\//////////////////////////////////////////////////////////////////////////        
Function: bool clearPattern(unsigned char ptrn)
Description:
    Clear all pattern data from the specified pattern
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\__///
        bool clearPattern(unsigned char ptrn);

/***\//////////////////////////////////////////////////////////////////////////        
Function: bool removePattern(unsigned char ptrn)
Description:
    Remove the selected pattern from the pattern list
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\_///
        bool removePattern(unsigned char ptrn);

/***\//////////////////////////////////////////////////////////////////////////        
Function: void addPattern(Pattern *ptrn)
Description:
   Add a pattern onto the end of the pattern list
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\///
        void addPattern(Pattern *ptrn);


/***\//////////////////////////////////////////////////////////////////////////        
Function: bool newInstrument()
Description:
    Create a new instrument and add it to the instrument list
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        bool newInstrument();

/***\//////////////////////////////////////////////////////////////////////////        
Function: bool cloneInstrument(unsigned char inst)
Description:
    Clone the specified instrument and add it to the end of the instrument list
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\__///
        bool cloneInstrument(unsigned char inst);

/***\//////////////////////////////////////////////////////////////////////////        
Function: bool removeInstrument(unsigned char inst)
Description:
    Remove the specified instrument from the instrument list
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\_///
        bool removeInstrument(unsigned char inst);

/***\//////////////////////////////////////////////////////////////////////////        
Function: void addInstrument(Instrument *inst)
Description:
    Add an instrument to the end of the instrument list
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        void addInstrument(Instrument *inst);



        inline unsigned short *getWaveTable() {return waveTable;}
        inline unsigned short getWaveEntry(unsigned short index) {return waveTable[index];}
        inline void setWaveEntry(unsigned short index, unsigned short entry){waveTable[index] = entry;}

/***\//////////////////////////////////////////////////////////////////////////        
Function: bool insertWaveEntry(unsigned short index, unsigned short entry)
Description:
   Insert a new wave entry at the specified location.
   Pushed back entries that come after index
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        bool insertWaveEntry(unsigned short index, unsigned short entry);

/***\//////////////////////////////////////////////////////////////////////////        
Function: bool removeWaveEntry(unsigned short index)
Description:
    Remove a wave entry at the specified location
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        bool removeWaveEntry(unsigned short index);


/***\//////////////////////////////////////////////////////////////////////////        
Function: void fixWaveJumps(const unsigned short &index, short difference)
Description:
    Fixes indicies that point to positions in the wave table.
    Fired after rows are inserted or removed from the wave table
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        void fixWaveJumps(const unsigned short &index, short difference);




        inline unsigned short *getPulseTable() {return pulseTable;}
        inline unsigned short getPulseEntry(unsigned short index) {return pulseTable[index];}
        inline void setPulseEntry(unsigned short index, unsigned short entry){pulseTable[index] = entry;}

/***\//////////////////////////////////////////////////////////////////////////        
Function: bool insertPulseEntry(unsigned short index, unsigned short entry)
Description:
   Insert a new pulse entry at the specified location.
   Pushed back entries that come after index
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        bool insertPulseEntry(unsigned short index, unsigned short entry);

/***\//////////////////////////////////////////////////////////////////////////        
Function: bool removePulseEntry(unsigned short index)
Description:
    Remove a pulse entry at the specified location
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        bool removePulseEntry(unsigned short index);


/***\//////////////////////////////////////////////////////////////////////////        
Function: void fixPulseJumps(const unsigned short &index, short difference)
Description:
    Fixes indicies that point to positions in the pusle table.
    Fired after rows are inserted or removed from the pulse table
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        void fixPulseJumps(const unsigned short &index, short difference);




};

#endif
