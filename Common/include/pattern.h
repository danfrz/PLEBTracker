#ifndef PATTERN_H_
#define PATTERN_H_

#include<istream>
#include<ostream>

#define NOTEMULT 1.05946309435929526456f //12th root of 2 (12 note chromatic scale)
//#define NOTEMULT 1.04427378242741384032f //16th root of 2 (16 note chromatic scale)
#define CHROMATIC_NOTES 12
//more or less arbitrary
//44100 (hz) / 10
static float BASEFRQ;

//Integer segment masks
#define R_OCTAVE      0xE0000000
#define R_NOTE        0x1E000000
#define R_INSTRUMENT  0x01FC0000
#define R_VOLUME      0x0003F000
#define R_EFFECT      0x00000F00
#define R_FXPARAM1    0x000000F0
#define R_FXPARAM2    0x0000000F
#define R_FXPARAM     0x000000FF

#define R_PITCHSEG    0xFE000000
#define R_FULLSEG     0xFFFC0000
#define R_EFFECTSEG   0x00000FFF

#define RI_OCTAVE     29
#define RI_NOTE       25
#define RI_PITCHSEG   25
#define RI_INSTRUMENT 18
#define RI_FULLSEG    18
#define RI_VOLUME     12
#define RI_EFFECT     8
#define RI_FXPARAM1   4

//For empty cells in pattern editor
#define R_EMPTY 0xFFFFF000

//Holds data for one pattern of a song
class Pattern
{
    private:
        unsigned int **data;
        unsigned char tracks;
        unsigned char rows;

    public:
        Pattern();
        Pattern(std::istream &in);
        Pattern(unsigned int track, unsigned int length);
        Pattern(const Pattern &other);
        ~Pattern();



        std::ostream &output(std::ostream &out) const;
        std::istream &input(std::istream &in);

        unsigned char numTracks() const {return tracks;}
        unsigned char numRows() const {return rows;};


/***\//////////////////////////////////////////////////////////////////////////        
Function: void setSize(const unsigned char &newtrack, const unsigned char &newrow)
Description:
    Change the number of tracks and rows to new sizes.
    New entries are empty, and chopped entries are deleted
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        void setSize(const unsigned char &newtrack, const unsigned char &newrow);

        void setRowNum(const unsigned char &newrow);
        void setTrackNum(const unsigned char &newtrack);
        void setAt(const unsigned char &track, const unsigned char &row, const unsigned int &cell);
        void setTrack(const unsigned char &which, unsigned int *track);

/***\//////////////////////////////////////////////////////////////////////////        
Function: void setRow(const unsigned char &which, unsigned int *row)
Description:
    Set the specified row of the pattern
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        void setRow(const unsigned char &which, unsigned int *row);




/***\//////////////////////////////////////////////////////////////////////////        
Function: void insertRow(const unsigned char &track, const unsigned char &row, const unsigned int &entry=R_EMPTY)
Description:
    Insert a new row at the specified row in only one track
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        void insertRow(const unsigned char &track, const unsigned char &row, const unsigned int &entry=R_EMPTY);

/***\//////////////////////////////////////////////////////////////////////////        
Function: void insertRow(const unsigned char &row, const unsigned int *entry)
Description:
    Insert a new row into all tracks of the pattern with the specified entries
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        void insertRow(const unsigned char &row, const unsigned int *entry);

/***\//////////////////////////////////////////////////////////////////////////        
Function: void insertRow(const unsigned char &row, const unsigned int &entry=R_EMPTY)
Description:
    Insert a new row into the pattern
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        void insertRow(const unsigned char &row, const unsigned int &entry=R_EMPTY);


/***\//////////////////////////////////////////////////////////////////////////        
Function: void deleteRow(const unsigned char &track, const unsigned char &row, unsigned char len)
Description:
    Delete `len` rows from one track of the pattern
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        void deleteRow(const unsigned char &track, const unsigned char &row, unsigned char len);

/***\//////////////////////////////////////////////////////////////////////////        
Function: void deleteRow(const unsigned char &row, unsigned char len=1)
Description:
    delete `len` rows from all tracks of the pattern
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        void deleteRow(const unsigned char &row, unsigned char len=1);



/***\//////////////////////////////////////////////////////////////////////////        
Function: void copyTrack(const unsigned char &srctrk, const unsigned char &desttrk)
Description:
    copy a track to another
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        void copyTrack(const unsigned char &srctrk, const unsigned char &desttrk);



/***\//////////////////////////////////////////////////////////////////////////        
Function: void removeTracks(const unsigned char &srctrk, const unsigned char &endtrk)
Description:
   Remove an interval of tracks
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        void removeTracks(const unsigned char &srctrk, const unsigned char &endtrk);

/***\//////////////////////////////////////////////////////////////////////////        
Function: void chop(unsigned char start, unsigned char end)
Description:
   cut this pattern from row start to end
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        void chop(unsigned char start, unsigned char end);


/***\//////////////////////////////////////////////////////////////////////////        
Function: void clearAt(const unsigned char &track, const unsigned char &row)
Description:
   Clear an entry of this pattern
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        void clearAt(const unsigned char &track, const unsigned char &row);

/***\//////////////////////////////////////////////////////////////////////////        
Function: void clearTrack(const unsigned char &which)
Description:
    Clear one track of entries from this pattern
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        void clearTrack(const unsigned char &which);

/***\//////////////////////////////////////////////////////////////////////////        
Function: void clearRow(const unsigned char &which)
Description:
    Clear a row of entries from all tracks this pattern
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        void clearRow(const unsigned char &which);
        void clear();


/***\//////////////////////////////////////////////////////////////////////////        
Function: int at(const unsigned char &track, const unsigned char &row)
Description:
   get the entry at this position in the pattern
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        unsigned int at(const unsigned char &track, const unsigned char &row);

/***\//////////////////////////////////////////////////////////////////////////        
Function: trackAt(const unsigned char &track)
Description:
    get a pointer to a track of this pattern
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        unsigned int *trackAt(const unsigned char &track);


/***\//////////////////////////////////////////////////////////////////////////        
Function: void purgeInstrument(const unsigned char &inst)
Description:
    remove all references to this instrument from all of existence. damn.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
        void purgeInstrument(const unsigned char &inst);

};

#endif
