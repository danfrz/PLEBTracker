#ifndef CMDBAR_H_
#define CMDBAR_H_
#include <ncurses.h>
#include <vector>
#include "tables.h"
#include "patternedtr.h"
#include "tables.h"
#include "instedtr.h"
#include "patternedtr.h"

#include "pattern.h"
#include "instrument.h"
#include "song.h"

namespace editor
{

    typedef void (*cmd_handler)(std::vector<char*>&);
    extern std::map<std::string, cmd_handler> cmdbar_cmdmap;
    extern std::map<std::string, unsigned int> scalenamemap;
    void populateCmdMap();
    void initializeScaleMap();

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
////// Command Bar UI and Helper prototypes
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////



/***\//////////////////////////////////////////////////////////////////////////    
Function: int parseSigned(char *str)

Description:
   Parses a signed hexadecimal string to integer
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    int parseSigned(char *str);

/***\//////////////////////////////////////////////////////////////////////////    
Function: int parseUnsigned(char *str)

Description:
   Parses a unsigned hexadecimal string to integer
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    unsigned int parseUnsigned(char *str);


/***\//////////////////////////////////////////////////////////////////////////    
Function: float parseFloat(char *str)

Description:
   Parses a decimal float string to float
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    float parseFloat(char *str);


/***\//////////////////////////////////////////////////////////////////////////    
Function: int parseKeySignature(const char *key)
Description:
   Parses a key signature to numerical representation
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
int parseKeySignature(char *key);




/***\//////////////////////////////////////////////////////////////////////////
Function: void parseScale(char *str)
Description:
    Tries to parse a string as either a scale number sequence
    or by name
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
bool parseScale(char *str, unsigned char &spinner, unsigned char *bfr);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void tokenize(std::vector<char*> &tokens, int begin)

Description:
   Seperates editor::charInputBuffer into tokens

Postcondition:
   Changes the contents of charInputBuffer so that whitespaces become \0.
   The character pointers in the vector do not have to be deleted because
   they point to indicies within the charInputBuffer.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void tokenize(std::vector<char*> &tokens, int begin);



/***\//////////////////////////////////////////////////////////////////////////    
Function: bool getCommand()

Description:
   Modal. Runs a UI loop for the vim command bar. Exits when \t or \n are input.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    bool getCommand(const char *autofilled);



///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
////// Command Bar Event Handlers
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////



/***\//////////////////////////////////////////////////////////////////////////    
Function: void doCommand()

Description:
   Tokenizes charInputBuffer, then performs the action corresponding to the
   command extracted from the charInputBuffer.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void doCommand();

    //command events

    //Quit
    void handle_q(std::vector<char*> &params);

    //Save
    void handle_w(std::vector<char*> &params);

    //Save and Quit
    void handle_wq(std::vector<char*> &params);

    //New song
    void handle_n(std::vector<char*> &params);

    //Open
    void handle_o(std::vector<char*> &params);

    //Refresh
    void handle_e(std::vector<char*> &params);


    //Delete
    void handle_d(std::vector<char*> &params);
    void handle_dd(std::vector<char*> &params);
    void handle_ddd(std::vector<char*> &params);


    //Copy
    void handle_y(std::vector<char*> &params);
    void handle_yy(std::vector<char*> &params);

    //
    void handle_step(std::vector<char*> &params);
    void handle_oct(std::vector<char*> &params);

    //Selection
    void handle_wave(std::vector<char*> &params);
    void handle_inst(std::vector<char*> &params);
    void handle_order(std::vector<char*> &params);
    void handle_ptrn(std::vector<char*> &params);
    void handle_row(std::vector<char*> &params);
    void handle_track(std::vector<char*> &params);

    
    //Playback 
    void handle_mark(std::vector<char*> &params);
    void handle_playlen(std::vector<char*> &params);

    void handle_mute(std::vector<char*> &params);
    void handle_unmute(std::vector<char*> &params);

    void handle_play(std::vector<char*> &params);
    void handle_stop(std::vector<char*> &params);
    void handle_render(std::vector<char*> &params);
    void handle_playamp(std::vector<char *> &params);


    //Amplify...
    void handle_amp(std::vector<char*> &params);
    //Amplify Instrument['s VolTable entries]
    void handle_ampinst(std::vector<char *> &params);
    //Amplify region between editor::playback_mark and patternedtr::selrow
    void handle_ampmark(std::vector<char *> &params);

    void handle_amplin(std::vector<char*> &params);


    void handle_resize(std::vector<char *> &params);
    void handle_resizesong(std::vector<char *> &params);

    //Set instrument of all in interval
    void handle_instset(std::vector<char *> &params);
    void handle_instsetmark(std::vector<char *> &params);

    //Set effect of all in interval
    void handle_fxset(std::vector<char *> &params);
    void handle_fxsetmark(std::vector<char *> &params);

    //Insert empty rows [into track of pattern] calls entryins
    void handle_ins(std::vector<char*> &params);

    //Insert empty rows [into track of pattern]
    void handle_entryins(std::vector<char*> &params);

    //Insert empty rows [into all tracks of pattern]
    void handle_rowins(std::vector<char*> &params);

    //Delete rows [of track of pattern]
    void handle_entrydel(std::vector<char*> &params);

    //Delete rows [of all tracks of pattern]
    void handle_rowdel(std::vector<char*> &params);


    //transpose region +/- semitones
    void handle_trans(std::vector<char*> &params);

    //transpose whole song +/- semitones
    void handle_transsong(std::vector<char*> &params);

    //transpose region +/- semitones then round to nearest notes of key signature
    void handle_transkey(std::vector<char*> &params);

    //transpose whole song +/- semitones then round to nearest notes of key signature
    void handle_transkeysong(std::vector<char*> &params);

    void handle_scale(std::vector<char*> &params);
    void handle_key(std::vector<char*> &params);

    
    //Set process ID to tether to
    void handle_pid(std::vector<char*> &params);


}

#endif
