//
//Contains prototypes for meta.cpp and patternedtr.cpp
//


#ifndef PATTERNEDTR_H_
#define PATTERNEDTR_H_
#include "instrument.h"
#include "pattern.h"
#include "song.h"
#include "tables.h"
#include "instedtr.h"
#include "daemoncomm.h"
#include "cmdbar.h"

namespace patternedtr
{
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
////// Note utility and arithmetic functions
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

    const unsigned int META_ROWS = 3,
          META_ROW0_DEPTH = 10,
          META_ROW1_DEPTH = 9,
          META_ROW2_DEPTH = 13;




/***\//////////////////////////////////////////////////////////////////////////    
Function: int toKey(const unsigned int &entry, const unsigned char &key, const unsigned char &sig)
Description:
   Returns the entry with its Note segment rounded to the nearest note available
   in the provided key signature
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    unsigned int toKey(const unsigned int &entry, const unsigned char &key);

/***\//////////////////////////////////////////////////////////////////////////    
Function: int addNotes(const unsigned int &n1, const unsigned int &n2)
Description:
   Adds one note to another, return result
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    unsigned int addNotes(const unsigned int &n1, const unsigned int &n2);

/***\//////////////////////////////////////////////////////////////////////////    
Function: int subNotes(const unsigned int &n1, const unsigned int &n2)
Description:
   Subtract one note from another, return the result
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    unsigned int subNotes(const unsigned int &n1, const unsigned int &n2);

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
////// Pattern traversal control functions
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

/***\//////////////////////////////////////////////////////////////////////////    
Function: void chgSelTrack(int i)
Description:
    Increases or decreases the current selected track,
    avoids overflow by comparing against editor::song->numTracks()
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void chgSelTrack(int i);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void chgSelRow(int i)
Description:
    Increases or decreases the current selected rot,
    avoids overflow by comparing against ptrnedtr::selptrn->numRows();
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void chgSelRow(int i);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void chgSelRowSeg(int i)
Description:
    Increases or decreases selected rowsegment,
    wraps around, calling chgSelRow if it overflows or underflows
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void chgSelRowSeg(int i);



/***\//////////////////////////////////////////////////////////////////////////    
Function: void chgSelMetaObj(int i)
Description:
    Cycles through objects in the respective rows of the metadata window
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void chgSelMetaObj(int i);


/***\//////////////////////////////////////////////////////////////////////////    
Function: void chgSelMetaRow(int i)
Description:
    Increases or decreases selected meta row with bounds checks
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void chgSelMetaRow(int i);



/***\//////////////////////////////////////////////////////////////////////////    
Function: void chgSelOrder(int i)
Description:
   Change selected order
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void chgSelOrder(int i);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void chgOrderPattern(int i)

Description:
   Change the pattern pointed to by the currently selected order
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void chgOrderPattern(int i);

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
////// Pattern Editor / Metadata display functions and utility
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////



/***\//////////////////////////////////////////////////////////////////////////    
Function: void getKeyChar(char *bfr, unsigned char note)
Description:
   Get the key string for the selected key:
   e.g. C_ C# CM Cm 
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void getKeyChar(char *bfr, unsigned char note);


/***\//////////////////////////////////////////////////////////////////////////    
Function: void display()
Description:
   Render the pattern editor and metadata window
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void display();

/***\//////////////////////////////////////////////////////////////////////////    
Function: void displayMeta()
Description:
   Renders the metadata window
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void displayMeta();

/***\//////////////////////////////////////////////////////////////////////////    
Function: void displayPattern()
Description:
   Renders the pattern editor window
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void displayPattern();



/***\//////////////////////////////////////////////////////////////////////////    
Function: bool setMetaAttribs(unsigned char objmeta, unsigned char rowmeta)
Description:
    Sets NCurses attributes of the provided UI element by index
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    bool setMetaAttribs(unsigned char objmeta, unsigned char rowmeta);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void setPatternAttribs(unsigned char track, unsigned char row, unsigned char seg)
Description:
    Sets NCurses attributes of the provided rowsegment
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void setPatternAttribs(unsigned char track, unsigned char row, unsigned char seg);


/***\//////////////////////////////////////////////////////////////////////////    
Function: makeSpaceBuffer(char *bfr, int length)
Description:
   I uhh formatting crap
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    char *makeSpaceBuffer(char *bfr, int length);




///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
////// UI Key processing functions
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////




/***\//////////////////////////////////////////////////////////////////////////    
Function: void processMetaInput(int in)
Description:
   Interprets input as input intended for the metadata window
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void processMetaInput(int in);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void processPatternInput(int in)
Description:
   Interprets input as input for the pattern editor
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void processPatternInput(int in);



/***\//////////////////////////////////////////////////////////////////////////    
Function: void entryInput(int in)
Description:
   Evaluates the input when intended for an entry in the pattern editor
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void entryInput(int in);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void processInput(int in)
Description:
    Sends the input to the appropriate window
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void processInput(int in);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void startMetaEditing()
Description:
    Responsible for performing or beginning an event for the objects in the metawindow
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void startMetaEditing();

/***\//////////////////////////////////////////////////////////////////////////    
Function: void metaEdit(int in)
Description:
    An input event performed on objects in the meta window after
    a startEditing event has been fired and before doneEditing.

    Only buffers are modified, so that the user can back out of an edit.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void metaEdit(int in);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void doneMetaEditing()
Description:
   Confirms changes to the selected meta window
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void doneMetaEditing();

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
////// Song management functions
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

/***\//////////////////////////////////////////////////////////////////////////    
Function: void newSong()
Description:
   Sets the song to a new song, deleting all old data
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void newSong();

/***\//////////////////////////////////////////////////////////////////////////    
Function: bool saveSong(const char *file)
Description:
   Saves editor::song to the file at the path provided
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    bool saveSong(const char *file);

/***\//////////////////////////////////////////////////////////////////////////    
Function: bool openSong(const char *file)
Description:
   Loads a song from the file provided to editor::song and updates all
   relevant UI fields
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    bool openSong(const char *file);


/***\//////////////////////////////////////////////////////////////////////////    
Function: void exit()
Description:
   Shutdown the program
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void exit();




/***\//////////////////////////////////////////////////////////////////////////    
Function: void newInstrument()
Description:
   Creates a new instrument, and point 
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void newInstrument();


/***\//////////////////////////////////////////////////////////////////////////    
Function: void cloneInstrument()

Description:
   Creates a new instrument with the same data as the currently selected
   instrument.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void cloneInstrument(unsigned char inst);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void removeInstrument(unsigned char inst)
Description:
   Removes the instrument as long as it's not the last instrument
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void removeInstrument(unsigned char inst);


/***\//////////////////////////////////////////////////////////////////////////    
Function: void newPattern()
Description:
   Creates a new empty pattern and sets the currently selected pattern to
   this new pattern.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void newPattern();

/***\//////////////////////////////////////////////////////////////////////////    
Function: void clonePattern(unsigned char ptrn)
Description:
   Creates a new pattern with the same data as the currently provided pattern.
   Sets the currently selected pattern to this new pattern.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void clonePattern(unsigned char ptrn);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void clearPattern()
Description:
   Removes all data within this pattern
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void clearPattern(unsigned char ptrn);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void removePattern(unsigned char ptrn)
Description:
   Deletes the specified pattern as long as this pattern isn't the last.
   Sets the selected pattern of the selected order to the nearest lower pattern.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void removePattern(unsigned char ptrn);


/***\//////////////////////////////////////////////////////////////////////////    
Function: void clearTrack(unsigned char track)
Description:
   Remove all pattern data within one track of the currently selected pattern.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void clearTrack(unsigned char track);


/***\//////////////////////////////////////////////////////////////////////////    
Function: void newOrder()
Description:
   Create a new order with a new empty pattern.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void newOrder();

/***\//////////////////////////////////////////////////////////////////////////    
Function: void cloneOrder(unsigned char order)
Description:
   Create a new order that points to the same pattern as the provided order.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void cloneOrder(unsigned char order);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void removeOrder(unsigned char order)
Description:
   Removes this order from the order list, as long is it isn't the last.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void removeOrder(unsigned char order);


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
////// Track mute controls
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////


/***\//////////////////////////////////////////////////////////////////////////    
Function: void unmuteTracks()
Description:
   Set all tracks to unmuted. editor::refreshMutedTracks() should be
   called soon afterwards if the tracker is connected to a daemon.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void unmuteTracks();

/***\//////////////////////////////////////////////////////////////////////////    
Function: void setMuteTracks(unsigned int *track, const unsigned int &tracks)
Description:
   Sets multiple tracks to muted, editor::refreshMutedTracks() should be
   called soon afterwards if the tracker is connected to a daemon.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void setMuteTracks(unsigned int *track, const unsigned int &tracks);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void setUnmuteTracks(unsigned int *track, const unsigned int &tracks)
Description:
   Sets multiple tracks to unmute. editor::refreshMutedTracks() should be
   called soon afterwards if the tracker is connected to a daemon.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void setUnmuteTracks(unsigned int *track, const unsigned int &tracks);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void toggleMuteTracks(unsigned int *track, const unsigned int &tracks)
Description:
   Toggles multiple tracks. editor::refreshMutedTracks() should be
   called soon afterwards if the tracker is connected to a daemon.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void toggleMuteTracks(unsigned int *track, const unsigned int &tracks);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void unmuteTrack(unsigned int track)
Description:
   Unmutes a single track. editor::refreshMutedTracks() should be
   called soon afterwards if the tracker is connected to a daemon.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void unmuteTrack(unsigned int track);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void muteTrack(unsigned int track)
Description:
   Mutes a signle track. editor::refreshMutedTracks() should be
   called soon afterwards if the tracker is connected to a daemon.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void muteTrack(unsigned int track);



    
}





#endif
