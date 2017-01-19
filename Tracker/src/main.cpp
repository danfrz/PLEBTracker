#include <ncurses.h>
#include "tables.h"
#include "instedtr.h"
#include "patternedtr.h"
#include "cmdbar.h"

#include "pattern.h"
#include "instrument.h"
#include "song.h"


#include <iostream>


bool parseParams(int argc, char *argv[])
{
    bool songset = false;
    for(int i = 1; i < argc; i++)
    {

        //This option isn't necesarry any more
        if(strcmp(argv[i],"") == 0)
        {
            //no params yet
        }
        else
        {
            int len = strlen(argv[i]);
            if(len > LASTSONG_SIZE-1)
                len = LASTSONG_SIZE-1;
            songset = true;
            editor::copy(argv[i], editor::lastSongPath, len);
            for(int i = len; i < LASTSONG_SIZE; i++)
                editor::lastSongPath[i] = 0;
        }
    
    }


    if(!songset)
    {
        //No song was set so just set it to song.plb
        editor::lastSongPath[8] = 0;
        editor::lastSongPath[0] = 's';
        editor::lastSongPath[1] = 'o';
        editor::lastSongPath[2] = 'n';
        editor::lastSongPath[3] = 'g';
        editor::lastSongPath[4] = '.';
        editor::lastSongPath[5] = 'p';
        editor::lastSongPath[6] = 'l';
        editor::lastSongPath[7] = 'b';
        editor::song = new Song();
    }
    else
    {
        //Search for said song, if it exists open it
        bool found = patternedtr::openSong(editor::lastSongPath);
        if(!found)
        {
            editor::song = new Song();
        }
    }

    return true;
}


int main(int argc, char *argv[])
{

    //Make sure to tie with the daemon
    editor::daemonpid = getppid();
    editor::setPID(editor::daemonpid);

    //Parse params
    if (!parseParams(argc, argv))
        return 1;


    //Initilize ncurses
    initscr();
    raw();
    start_color();
    noecho();

    clear();
    printw("Welcome to PLEBTracker!  [Press any key to continue]  20160114-20160705\n\n");
    printw("To view logs produced by plebtrk see\n");
    printw("    /var/tmp/plebtrk/trklog.log and\n");
    printw("    /var/tmp/plebtrk/itplog.log");
    curs_set(0);
    getch();
    clear();

    {
        using namespace patternedtr;
        //META color mappings for metadata window
        init_pair(COL_META_SSS, COLOR_YELLOW, COLOR_CYAN);
        init_pair(COL_META_SSSE, COLOR_YELLOW, COLOR_MAGENTA);
        init_pair(COL_META_SSU, COLOR_BLACK, COLOR_CYAN);
        init_pair(COL_META_SU , COLOR_WHITE , COLOR_BLACK);
        init_pair(COL_META_US , COLOR_WHITE  , COLOR_BLUE);
        init_pair(COL_META_UU , COLOR_CYAN  , COLOR_BLACK);

        init_pair(COL_META_ERR, COLOR_MAGENTA, COLOR_BLUE);
        init_pair(COL_META_SSS_ERR, COLOR_MAGENTA, COLOR_CYAN);

        //PATTERN color mappings for pattern editor
        init_pair(COL_PTRN_SSS      , COLOR_YELLOW, COLOR_CYAN);

        init_pair(COL_PTRN_SSU_NOTE , COLOR_BLACK, COLOR_BLUE);
        init_pair(COL_PTRN_SSU_INST , COLOR_BLACK, COLOR_BLUE);
        init_pair(COL_PTRN_SSU_VOL  , COLOR_GREEN, COLOR_BLUE);
        init_pair(COL_PTRN_SSU_FX   , COLOR_MAGENTA, COLOR_BLUE);
        init_pair(COL_PTRN_SSU_SYSFX, COLOR_RED, COLOR_BLUE);

        init_pair(COL_PTRN_MARK, COLOR_BLACK, COLOR_RED);

        init_pair(COL_PTRN_SU_NOTE  , COLOR_WHITE, COLOR_BLACK);
        init_pair(COL_PTRN_SU_INST  , COLOR_WHITE, COLOR_BLACK);
        init_pair(COL_PTRN_SU_VOL   , COLOR_GREEN, COLOR_BLACK);
        init_pair(COL_PTRN_SU_FX    , COLOR_YELLOW, COLOR_BLACK);
        init_pair(COL_PTRN_SU_SYSFX , COLOR_RED, COLOR_BLACK);

        init_pair(COL_PTRN_US       , COLOR_CYAN, COLOR_BLACK);
        init_pair(COL_PTRN_UU       , COLOR_BLUE, COLOR_BLACK);


        init_pair(COL_PTRN_MUTE_SS , COLOR_BLACK, COLOR_WHITE);
        init_pair(COL_PTRN_MUTE_SU , COLOR_BLUE, COLOR_BLACK);
        init_pair(COL_PTRN_MUTE_US  , COLOR_BLUE, COLOR_BLACK);
        init_pair(COL_PTRN_MUTE_UU  , COLOR_BLUE, COLOR_BLACK);

        init_pair(COL_CMDBAR_S       , COLOR_BLACK, COLOR_WHITE);

        init_pair(COL_TABLE_SU_JUMP , COLOR_GREEN  , COLOR_BLACK);
        init_pair(COL_TABLE_UU_JUMP , COLOR_GREEN  , COLOR_BLACK);



    }


    
        

    










    //INITIALIZE EXTERNAL IDENTIFIERS
    getmaxyx(stdscr, editor::WIN_HEIGHT, editor::WIN_WIDTH);
    editor::metawin = new WIN(3, editor::WIN_WIDTH, 0, 0);
    editor::ptrnwin = new WIN(editor::WIN_HEIGHT-3, editor::WIN_WIDTH-11*2, 3, 0);
    editor::instwin = new WIN(editor::WIN_HEIGHT, editor::WIN_WIDTH,      0, 0);
    editor::wavewin = new WIN(editor::WIN_HEIGHT-5, 10,                   3, editor::WIN_WIDTH-20);
    editor::pulsewin = new WIN(editor::WIN_HEIGHT-5, 10,                   3, editor::WIN_WIDTH-10);
    editor::dialog  = new WIN(8, 60,             editor::WIN_HEIGHT/3, editor::WIN_WIDTH/3); //good enough
    editor::selinst = editor::song->getInstrument(0);

    editor::wingroup = editor::ptrnwin;
    editor::inputwin = editor::metawin;
    editor::lastwin = editor::metawin;

    editor::playback_length = 4;
    editor::playback_mark = 0xFF;


    editor::muted_tracks = new bool[editor::song->numTracks()];
    for(int i = 0; i < editor::song->numTracks(); i++)
        editor::muted_tracks[i] = 0;

    editor::playamp = 1;

    editor::populateCmdMap();
    editor::initializeScaleMap();

    patternedtr::key = 0;
    patternedtr::scalespinner = 0;
    for(int i = 0; i < CHROMATIC_NOTES-1; i++)
        patternedtr::scaleconst[i] = 1;
    patternedtr::scaleconst[CHROMATIC_NOTES-1] = 0;

    patternedtr::populateNoteMap();
    patternedtr::selptrn = editor::song->getPatternByOrder(0);
    patternedtr::viewporttrack = 0;
    patternedtr::viewportrow = 0;

    patternedtr::maxtracksviewport = (editor::WIN_WIDTH-11*2 - 3) / 15;
    patternedtr::maxrowsviewport = (editor::WIN_HEIGHT-3);

    patternedtr::selrow = 0;
    patternedtr::seltrack = 0;
    patternedtr::seltrackseg = 0;
    patternedtr::selorder = 0;

    patternedtr::selinstrument = 0;
    patternedtr::edit_step = 1;
    patternedtr::row_underline = 0;
    patternedtr::octave = 3;
    patternedtr::entryclipboard = R_EMPTY;

    //editor::charInputHistory[HISTORY_SIZE][64];
    editor::history_head = 0;
    editor::history_size = 0;

    instedtr::selinstrow = 0;
    instedtr::selinstobj = 0;
    instedtr::instobjedit = false;
    instedtr::selwavrow = 0;
    instedtr::selwavseg = 0;
    instedtr::viewportwave = 0;

    instedtr::selvolrow = 0;
    instedtr::selvolseg = 0;
    instedtr::viewportvol = 0;


    patternedtr::metaobjindex = 0;
    patternedtr::metaobjedit = false;
    patternedtr::selobjmeta = 3;
    patternedtr::selrowmeta = 0;

    keypad(stdscr, 1);

    //Show the tracker
    patternedtr::display();

    //Start the tracker's loop
    int ch;
    editor::running = true;
    while(editor::running)
    {
        ch = getch();

        //Update window geometry here
        //If the user resizes the terminal, plebtrk shouldn't crash
        //from invalid rendering operations
        getmaxyx(stdscr, editor::WIN_HEIGHT, editor::WIN_WIDTH);

        editor::metawin->width = editor::WIN_WIDTH;
        editor::ptrnwin->height = editor::WIN_HEIGHT-3;
        editor::ptrnwin->width = editor::WIN_WIDTH - TABLE_WIDTH*2;
        editor::instwin->height = editor::WIN_HEIGHT;
        editor::instwin->width = editor::WIN_WIDTH;
        editor::wavewin->height = editor::WIN_HEIGHT-5;
        editor::wavewin->x = editor::WIN_WIDTH-20;
        editor::pulsewin->height = editor::WIN_HEIGHT-5;
        editor::pulsewin->x = editor::WIN_WIDTH-10;


        patternedtr::maxtracksviewport = (editor::WIN_WIDTH-TABLE_WIDTH*2 - 3) / TRACK_WIDTH;
        patternedtr::maxrowsviewport = (editor::WIN_HEIGHT-3);

        //First thing's first, process input
        if(ch == '\t')
        {
            //Tab acts as a sentinel and to switch
            //windows
            using namespace editor;

            //If any of the fields are being edited stop it
            //and continue on with the normal tracker state
            if(patternedtr::metaobjedit)
            {
                patternedtr::metaobjedit = false;
                patternedtr::display();
                continue;
            } if(instedtr::instobjedit)
            {
                instedtr::instobjedit = false;
                instedtr::display();
                continue;
            }
            else
            {

                mvprintw(WIN_HEIGHT-1, 0, "[q]meta [w]ptrn [e/d]wave [a]vol [s]inst", stdscr);
                while(ch == '\t')
                    ch = getch();
                //TAB + Character changes the window
                switch(ch)
                {
                    case 's':
                    case 'S':
                        //Instrument view

                        inputwin = instwin;
                        wingroup = instwin;
                        instedtr::display();

                        break;
                    case 'q':
                    case 'Q':
                        //Meta / Menu : Pattern View

                        inputwin = metawin;
                        wingroup = ptrnwin;
                        patternedtr::display();

                        break;
                    case 'w':
                    case 'W':
                        //Pattern View

                        inputwin = ptrnwin;
                        wingroup = ptrnwin;
                        patternedtr::display();

                        break;
                        //Wave : Pattern View -- Wave : Instrument View
                    case 'e':
                    case 'E':
                        inputwin = wavewin;
                        wingroup = ptrnwin;
                        patternedtr::display();

                        break;
                    case 'D':
                    case 'd':
                        inputwin = wavewin;
                        wingroup = instwin;
                        instedtr::display();

                        break;
                    case 'r':
                    case 'R':
                        //Pulse : Pattern View
                        inputwin = pulsewin;
                        wingroup = ptrnwin;
                        patternedtr::display();

                        break;
                    case 'f':
                    case 'F':
                        //Pulse : Instrument View
                        inputwin = pulsewin;
                        wingroup = instwin;
                        instedtr::display();

                        break;
                    case 'a':
                    case 'A':
                        //Volume : Instrument View
                        inputwin = volwin;
                        wingroup = instwin;
                        instedtr::display();
                        break;
                }
            }

        }
        //Command bar
        else if(ch == ':' && (!patternedtr::metaobjedit|| !instedtr::instobjedit) )
        {
            //If command not cancelled attempt to do it
            if(editor::getCommand(""))
                editor::doCommand();

            if(editor::wingroup == editor::ptrnwin)
                patternedtr::display();
            else if(editor::wingroup == editor::instwin)
                instedtr::display();
        }
        else
        {

            //MISC binds
            if(!patternedtr::metaobjedit && !instedtr::instobjedit)
            {
                using namespace editor;
                bool confirmation;
                switch(ch)
                {
                    
                    case 'P'://Play
                        editor::playExcerptImplicit();

                        continue;
                    case 'J'://Quick Change Pattern

                        editor::wingroup = editor::ptrnwin;
                        editor::inputwin = editor::metawin;
                        patternedtr::selobjmeta = 1;
                        patternedtr::selrowmeta = 2;
                        patternedtr::startMetaEditing();
                        patternedtr::display();
                        continue;
                    case 'K'://Quick Change Instrument
                        if(editor::wingroup == editor::ptrnwin)
                        {
                            editor::inputwin = editor::metawin;
                            patternedtr::selobjmeta = 4;
                            patternedtr::selrowmeta = 1;
                            patternedtr::startMetaEditing();
                            patternedtr::display();
                        }
                        else if(editor::wingroup == editor::instwin)
                        {
                            editor::inputwin = editor::instwin;
                            instedtr::selinstrow = 0;
                            instedtr::selinstobj = 6;
                            instedtr::startInstEditing();
                            instedtr::display();
                        }

                        continue;
                    case 'L'://Quick Change Wave Index

                        editor::inputwin = editor::instwin;
                        editor::wingroup = editor::instwin;
                        instedtr::selinstrow = 1;
                        instedtr::selinstobj = 0;
                        instedtr::startInstEditing();
                        instedtr::display();
                        continue;
                    case '_'://previous instrument
                        if(patternedtr::selinstrument!= 0)
                            patternedtr::selinstrument--;
                        editor::selinst = editor::song->getInstrument(patternedtr::selinstrument);
                        if(editor::wingroup == editor::ptrnwin)
                            patternedtr::display();
                        else if(editor::wingroup == editor::instwin)
                            instedtr::display();
                        continue;
                    case '+'://Next instrument
                        if(patternedtr::selinstrument < editor::song->numInstruments()-1)
                            patternedtr::selinstrument++;
                        editor::selinst = editor::song->getInstrument(patternedtr::selinstrument);
                        if(editor::wingroup == editor::ptrnwin)
                            patternedtr::display();
                        else if(editor::wingroup == editor::instwin)
                            instedtr::display();
                        continue;

                    /*case 'K'://Quick Change Order

                        editor::wingroup = editor::ptrnwin;
                        editor::inputwin = editor::metawin;
                        patternedtr::selobjmeta = 0;
                        patternedtr::selrowmeta = 2;
                        patternedtr::startMetaEditing();
                        patternedtr::display();
                        continue;*/

                }
            }

            using namespace editor;

            //Yes in this order and dont combine the if groups


            if(wingroup == ptrnwin)
                patternedtr::processInput(ch);
            else if(wingroup == instwin)
                instedtr::processInput(ch);

            if(wingroup == ptrnwin)
                patternedtr::display();
            else if(wingroup == instwin)
                instedtr::display();
        }



    }
    delete editor::song;

    //shutdown ncurses
    endwin();
    return 0;
}
