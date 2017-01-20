//
// tables.h contains all of the global variables used for the ncurses interface
// as well as the headers for validation functions used throughout the interface
//
// the implementation of most of these fields can be found in externinit.cpp
//


#ifndef TABLES_H_
#define TABLES_H_
#include <map>
#include <cstring>
#include <ncurses.h>
#include <unistd.h>
#include <stdio.h>

#include "song.h"
#include "instrument.h"
#include "pattern.h"

#define TRACK_WIDTH 15
#define TABLE_WIDTH 11
#define HISTORY_SIZE 16
#define CHARBUFFER_SIZE 128
#define CMDBAR_SIZE 128
#define LASTSONG_SIZE 128

#define ALT_BACKSPACE 127

struct WIN
{
    public:
    int width, height, x, y;
    WIN(int H, int W, int Y, int X){width = W; height = H; x = X; y=Y;}
};


namespace editor
{

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
////// Editor fields 
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////


    const unsigned char
        SCALE_CHROMATIC = 0,
        SCALE_MAJOR = 1,
        SCALE_AEOLIAN = 2,
        SCALE_HARMONIC = 3,
        SCALE_MELODIC = 4,
        SCALE_DORIAN = 5,
        SCALE_PHYRGIAN = 6,
        SCALE_LYDIAN = 7,
        SCALE_MIXOLYDIAN = 8,
        SCALE_LOCRIAN = 9;



    extern int WIN_HEIGHT, WIN_WIDTH;
    //ncurses windows to render to and control input divergence
    extern WIN *metawin;
    extern WIN *ptrnwin;
    extern WIN *instwin;
    extern WIN *volwin;
    extern WIN *wavewin;
    extern WIN *pulsewin;
    extern WIN *dialog;


    //ptrnwin and instwin are windows that are the according
    //"group" windows. They are the primary focus of their
    //respective screens and so are used in code to more quickly
    //diverge input processing and display functions.
    extern WIN *wingroup;
    extern WIN *inputwin; //currently selected window, window that has focus
    extern WIN *lastwin;  //last window to have focus

    //controls the active state of the program
    //if false, the program will end after the main loop
    extern bool running;


    extern Song *song;
    extern Song *playback; //For use with the playback daemon
    extern int daemonpid;  //The process ID to link up with the daemon
    extern unsigned char playback_length; //How many rows to extract on key enter
    extern unsigned char playback_mark;  //Some row location in the current
                                         //pattern specified by the user.

    extern Instrument *selinst;     //The object representing the currently 
                                    //selected instrument. REMEMBER: If
                                    //patternedtr::selinstrument is ever
                                    //changed, change this to that inst.

    //Buffers for storing input before assigning to the destination
    extern int numBuffer;
    extern char charBuffer[CHARBUFFER_SIZE];

    //cursor position within the charBuffer
    extern char textCursorPos;
    extern char charInputBuffer[CMDBAR_SIZE];
    extern char charInputHistory[HISTORY_SIZE][CMDBAR_SIZE]; //circular list
    extern char history_head;
    extern char history_size;

    extern char lastSongPath[LASTSONG_SIZE];

    extern bool *muted_tracks;
    extern float playamp;

    char *makeUnderlines(char *string, int length);

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
////// Utility functions for metawin and ptrnwin
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////



    //Hex display functions
    const char hexnums[17] = "0123456789ABCDEF";
    char *byteString(char *string, const unsigned char byte);
    char *shortString(char *string, unsigned short shrt, const unsigned int &strlen);
    char *intString(char *string,  unsigned int integer, const unsigned int &strlen);

    //Implementations found in patternedtr.cpp
    char *noteString(char *string, unsigned int entry);
    char *instString(char *string, const unsigned int &entry);
    char *volString(char *string, const unsigned int &entry);
    char *fxString(char *string, const unsigned int &entry);
    char *fxpString(char *string, const unsigned int &entry);
    
    //should be called after the editor changes the number of tracks within a song
    //one purpose of this is to keep editor::muted_tracks valid
    void trackNumChanged(unsigned int from, unsigned int to);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void setPID(int PID)

Description:
   Set the PID to append to the signal and excerpt song files.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void setPID(int PID);
   

/***\//////////////////////////////////////////////////////////////////////////    
Function: void inform(const char *message)

  Description:
Shows an information message to the user
This method blocks processing until Space is pressed
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void inform(const char *message);



/***\//////////////////////////////////////////////////////////////////////////    
Function: bool confirm(const char *message)

  Description:
Thows a confirmation message to the user
This method blocks processing until y/n is pressed
and returns the result y(1), n(0)
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    bool confirm(const char *message);


/***\//////////////////////////////////////////////////////////////////////////    
Function: void displayAbout()

Description:
   Displays the about window to the user until Space is pressed
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void displayAbout();


/***\//////////////////////////////////////////////////////////////////////////    
Function: void copy(const char *src, char *dest, int len)

Description:
    copies len characters from src to dest
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void copy(const char *src, char *dest, int len);


/***\//////////////////////////////////////////////////////////////////////////    
Function: bool validateHexChar(char a)

Description:
   determines a valid hexadecimal character
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    bool validateHexChar(char a);

/***\//////////////////////////////////////////////////////////////////////////    
Function: bool validateByte(char str[2])

Description:
   validates two hexadecimal characters 
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    bool validateByte(char str[2]);

/***\//////////////////////////////////////////////////////////////////////////    
Function: bool validate64(char str[2])

Description:
   validates that the input string is a byte less than 64, 0x40
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    bool validate64(char str[2]);

/***\//////////////////////////////////////////////////////////////////////////    
Function: char charHex(char c)

Description:
   parses one hexadecimal character, return as byte
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    unsigned char charHex(char c);

/***\//////////////////////////////////////////////////////////////////////////    
Function: char parseHexChar(char str[2])

Description:
   parses two hex characters and returns as a byte
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    unsigned char parseHexChar(char str[2]);

}

// Editor controls are split into patternedtr and instedtr namespaces
// and the namespace that a control is contained in corresponds to
// that control's window's group:
//
// patternedtr
//    {metawin, ptrnwin} controls
// instedtr
//    {instwin, volwin, wavewin} controls

namespace patternedtr
{

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
////// Pattern editor constants and fields
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////



    //PATTERN TABLE
    const unsigned char
        TRKSEG_NOTE = 0,
        TRKSEG_INST = 1,
        TRKSEG_VOL = 2,
        TRKSEG_FX = 3,
        TRKSEG_FXP = 4;

    //Window, Object, Segment selected
    //[S]elected or [U]nselected
    //For example
    //
    //COL_META_SSU:
    //   Window Selected, Object Selected, But not the row segment
    const unsigned char
        COL_META_SSS = 1,
        COL_META_SSS_ERR  = 21,
        COL_META_SSSE = 6,
        COL_META_SSU = 2,
        COL_META_SU  = 3,
        COL_META_US  = 4,
        COL_META_UU  = 5,
        COL_META_ERR  = 7,

        COL_TABLE_SSS = 1,
        COL_TABLE_SSU = 2,
        COL_TABLE_SU  = 3,
        COL_TABLE_SU_JUMP  = 29,
        COL_TABLE_US  = 4,
        COL_TABLE_UU  = 5,
        COL_TABLE_UU_JUMP  = 30,

        COL_CMDBAR_S  = 28;

    const unsigned char
        COL_PTRN_SSS = 8,
        COL_PTRN_SSU_NOTE = 9,
        COL_PTRN_SSU_INST = 10,
        COL_PTRN_SSU_VOL = 11,
        COL_PTRN_SSU_FX = 12,
        COL_PTRN_SSU_SYSFX = 13,
        COL_PTRN_SU_NOTE  = 14,
        COL_PTRN_SU_INST  = 15,
        COL_PTRN_SU_HIGH  = 22,
        COL_PTRN_SU_VOL  = 16,
        COL_PTRN_SU_FX  = 17,
        COL_PTRN_SU_SYSFX  = 18,
        COL_PTRN_US  = 19,
        COL_PTRN_UU  = 20,
        COL_PTRN_MARK  = 23,
        COL_PTRN_MUTE_SS  = 24,
        COL_PTRN_MUTE_SU  = 25,
        COL_PTRN_MUTE_US  = 26,
        COL_PTRN_MUTE_UU  = 27;


/***\//////////////////////////////////////////////////////////////////////////    
Function: void populateNoteMap()

Description:
   clears and populates patternedtr::notemap with key bindings according to the
   current key signature (patternedtr::key)
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void populateNoteMap();
    extern std::map<int, unsigned int> notemap;



    //PATTERN EDITOR controls
    extern Pattern *selptrn;
    extern unsigned char viewporttrack;
    extern unsigned char viewportrow;

    extern unsigned char maxorderlistviewport;
    extern unsigned char maxtracksviewport;
    extern unsigned char maxrowsviewport;

    extern unsigned char selrow;
    extern unsigned char seltrack;
    extern unsigned char seltrackseg;
    extern unsigned char selorder;

    extern unsigned char selinstrument;
    extern unsigned char edit_step;
    extern unsigned char row_underline;
    extern unsigned char octave;
    extern unsigned char key;
    extern unsigned char scalespinner;
    extern unsigned char scaleconst[CHROMATIC_NOTES];//Scale construction pattern


    //PATTERN EDITOR CLIPBOARD
    extern unsigned int entryclipboard;

    //METADATA controls
    extern unsigned char metaobjindex;
    extern  bool metaobjedit;
    extern unsigned char selobjmeta;//x
    extern unsigned char selrowmeta;//y



}

//Instrument Editor
namespace instedtr
{ 
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
////// Instrument editor controls
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////



    //WAVE TABLE
    const unsigned char
        WAVSEG_WAVE = 0,
        WAVSEG_TONE = 1,
        WAVSEG_NUM = 2;

    //VOL TABLE
    const unsigned char
        VOLSEG_VOL = 0,
        VOLSEG_DUR = 1,
        VOLSEG_NUM = 2;
    extern unsigned short waveclipboard;
    extern unsigned short pulseclipboard;
    extern unsigned short volclipboard;

    extern unsigned short selwavrow;
    extern unsigned char selwavseg;

    extern unsigned short selpulrow;
    extern unsigned char selpulseg;



    extern  bool instobjedit;
    extern unsigned char selinstrow; //control Y selection in instwin
    extern unsigned char selinstobj; //control X selection in instwin

    extern unsigned short viewportwave;
    extern unsigned short viewportpulse;
    extern unsigned char viewportvol;



    extern unsigned char selvolrow;
    extern unsigned char selvolseg;
}




#endif
