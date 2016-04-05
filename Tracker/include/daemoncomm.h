//
//Functions to communicate between the tracker and the trackdaemon
//


#ifndef DAEMONCOMM_H_
#define DAEMONCOMM_H_

#include <fstream>
#include <sstream>
#include "tables.h"
namespace editor
{

    extern char *signalpath;
    extern char *playbackpath;

/***\//////////////////////////////////////////////////////////////////////////    
Function: void playExcerptImplicit()
Description:
   Plays a portion of the song depending on where the cursor and playback_mark are
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void playExcerptImplicit();

/***\//////////////////////////////////////////////////////////////////////////    
Function: void playExcerptExplicit(unsigned char orderstart, unsigned char orderend, unsigned char rowstart, unsigned char rowend)
Description:
   Plays an explicitly stated portion of the song
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void playExcerptExplicit(unsigned char orderstart, unsigned char orderend, unsigned char rowstart, unsigned char rowend);

/***\//////////////////////////////////////////////////////////////////////////    
Function: void playSong()
Description:
   Play the whole song
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void playSong();

/***\//////////////////////////////////////////////////////////////////////////    
Function: void renderSong()
Description:
   Write the whole song to a file
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void renderSong();


/***\//////////////////////////////////////////////////////////////////////////    
Function: void stopPlayback()
Description:
  Tells the daemon to stop playing 
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void stopPlayback();

/***\//////////////////////////////////////////////////////////////////////////    
Function: void refreshMutedTracks()
Description:
   Send the daemon which tracks should be muted
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void refreshMutedTracks();


/***\//////////////////////////////////////////////////////////////////////////    
Function: void setPlaybackAmplitude()
Description:
   Tell the daemon what amplitude to play at
*////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___///
    void setPlaybackAmplitude();
}

#endif
