#include "tables.h"

///////////////INITIALIZIING////////////////
////////////////////////////////////////////
namespace editor
{
    int WIN_HEIGHT, WIN_WIDTH;
    WINDOW *ptrnwin;
    WINDOW *instwin;
    WINDOW *metawin;
    WINDOW *volwin;
    WINDOW *wavewin;
    WINDOW *dialog;

    WINDOW *wingroup;
    WINDOW *inputwin;
    WINDOW *lastwin;

    bool running;


    int numBuffer;
    char charBuffer[CHARBUFFER_SIZE];
    char charInputBuffer[CMDBAR_SIZE];

    char charInputHistory[HISTORY_SIZE][CMDBAR_SIZE];
    char history_head;
    char history_size;

    char lastSongPath[LASTSONG_SIZE];
    char textCursorPos;
    bool *muted_tracks;
    float playamp;

    Song *song;
    int daemonpid;
    Song *playback;
    unsigned char playback_length;
    unsigned char playback_mark;
    Instrument *selinst;
}

//Pattern Editor
namespace patternedtr
{
    std::map<int,unsigned int> notemap;
    Pattern *selptrn; 
    unsigned char viewporttrack;
    unsigned char viewportrow;

    unsigned char maxtracksviewport;
    unsigned char maxrowsviewport;

    unsigned char selrow;
    unsigned char seltrack;
    unsigned char seltrackseg;
    unsigned char selorder;

    unsigned char selinstrument;
    unsigned char edit_step;
    unsigned char row_underline;
    unsigned char octave;
    unsigned char key;
    unsigned int entryclipboard;


    //METADATA (Main controls)
    unsigned char metaobjindex;
    bool metaobjedit;

    unsigned char selobjmeta;//x
    unsigned char selrowmeta;//y

}

//Instrument Editor
namespace instedtr
{
    unsigned short waveclipboard;
    unsigned short volclipboard;

    bool instobjedit;
    unsigned char selinstrow;
    unsigned char selinstobj;
    unsigned short selwavrow;
    unsigned char selwavseg;
    unsigned short viewportwave;
    unsigned char viewportvol;


    unsigned char selvolrow;
    unsigned char selvolseg;
}
/////////////DONE INITIALIZING/////////////
///////////////////////////////////////////


