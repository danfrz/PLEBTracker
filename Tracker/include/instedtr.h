//
//This header file contains UI prototype functions for instedtr.cpp
//



#ifndef INSTEDTR_H_
#define INSTEDTR_H_
#include <ncurses.h>
#include <cmath>
#include "tables.h"
#include "patternedtr.h"

namespace instedtr
{


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
////// UI Helpers for Instrument Editor windows
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////


/***\//////////////////////////////////////////////////////////////////////////    
Function: void chgSelWavRow(int i)

Description:
    Changes the selected row in the wave table by i, then verifies
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void chgSelWavRow(int i);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void chgSelWavSeg(int i)

Description:
    Changes the selected row segment in the wave table by i,
    if it overflows the row is increased: chgSelWavRow(1)
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void chgSelWavSeg(int i);


/***\//////////////////////////////////////////////////////////////////////////    
Function: void chgSelVolRow(int i)

Description:
   Changes the selected row in the volume table by i, then verifies
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void chgSelVolRow(int i);



/***\//////////////////////////////////////////////////////////////////////////    
Function: void chgSelVolSeg(int i)

Description:
   Changes the selected row segment in the volume table by i,
   if it overflows the row is changed: chgSelVolRow(1)
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void chgSelVolSeg(int i);



/***\//////////////////////////////////////////////////////////////////////////    
Function: void chgSelInstRow(int i)

Description:
   Moves the cursor along the Y axis in the instrument window, selecting
   the component on the next row
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void chgSelInstRow(int i);



/***\//////////////////////////////////////////////////////////////////////////    
Function: void chgSelInstObj(int i)

Description:
   Moves the cursor along the X axis in the instrument window, selecting
   the next component in the same row
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void chgSelInstObj(int i);


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
////// Render functions for instrument editor windows
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

/***\//////////////////////////////////////////////////////////////////////////    
Function: void display()

Description:
  Render the Instrument Edit screen 
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void display();

/***\//////////////////////////////////////////////////////////////////////////    
Function: void displayWav()

Description:
   Render the Wave Table
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void displayWav();


/***\//////////////////////////////////////////////////////////////////////////    
Function: void displayInst()

Description:
   Render the Instrument window
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void displayInst();

/***\//////////////////////////////////////////////////////////////////////////    
Function: void displayVol()

Description:
   Render the Volume table
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void displayVol();

/***\//////////////////////////////////////////////////////////////////////////    
Function: void displayEnvelope()

Description:
   Render the volume envelope for the Instrument window
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void displayEnvelope();


/***\//////////////////////////////////////////////////////////////////////////    
Function: bool setInstAttribs(unsigned char instrow, unsigned char instobj)
Description:
   Set ncurses attributes for this UI element of the instrument window
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    bool setInstAttribs(unsigned char instrow, unsigned char instobj);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void setWaveAttribs(unsigned short waverow, unsigned char waveseg)

Description:
   Set ncurses attributes for this UI element of the wave table
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void setWaveAttribs(unsigned short waverow, unsigned char waveseg);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void setVolAttribs(unsigned char volrow, unsigned char volseg)

Description:
   Set ncurses attributes for this UI element of the volume table
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void setVolAttribs(unsigned char volrow, unsigned char volseg);

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
////// User input processing for instrument editor windows
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////


/***\//////////////////////////////////////////////////////////////////////////    
Function: void processInput(int in)
Description:
  Diverges keyboard input to other processInput functions depending on
  selected window (editor::inputwin)
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void processInput(int in);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void processInputWav(int in)
Description:
   Process keyboard input for the wave table
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void processInputWav(int in);


/***\//////////////////////////////////////////////////////////////////////////    
Function: void processInputVol(int in)
Description:
   Process keyboard input for the volume table
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void processInputVol(int in);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void processInputInst(int in)
Description:
   Process keyboard input for the instrument editor window
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void processInputInst(int in);


/***\//////////////////////////////////////////////////////////////////////////    
Function: void startInstEditing()
Description:
   Happens after the user press enter on a UI element in the Instrument window.
   Performs two disctinct events for the instrument window depending on context:
     If the user is hoverring over a button
      --Perform that button's action
     If the user is hoverring over an editable field
       --Tell editor that the instrument editor should consume input until
         futher notice.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void startInstEditing();

/***\//////////////////////////////////////////////////////////////////////////    
Function: void instEdit(int in)
Description:
   This is the function that handles keyboard events while the input window
   is consuming input.
   Fields that are numeric use numBuffer while editing.
   Fields that are textual use charInputBuffer while editing.
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void instEdit(int in);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void doneInstEditing()
Description:
   This function is called after the user presses \n while editing a UI
   component. It commits the buffer to the actual field this UI element controls
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void doneInstEditing();

}


#endif
