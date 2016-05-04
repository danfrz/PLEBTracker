#include "patternedtr.h"

#include <iostream>




//set muted state within the tracker, muteTracks() should be called soon afterward
void patternedtr::setMuteTracks(unsigned int *track, const unsigned int &tracks)
{
    for(int i = 0; i < editor::song->numTracks(); i++)
        editor::muted_tracks[i]= false;
    for(int i = 0; i < tracks; i ++)
        editor::muted_tracks[track[i]] = true;

}

//set muted state within the tracker, muteTracks() should be called soon afterward
void patternedtr::setUnmuteTracks(unsigned int *track, const unsigned int &tracks)
{
    for(int i = 0; i < tracks; i++)
        editor::muted_tracks[track[i]] = false;
}

//set muted state within the tracker, muteTracks() should be called soon afterward
void patternedtr::unmuteTrack(unsigned int track)
{
    editor::muted_tracks[track] = false;
}

//set muted state within the tracker, muteTracks() should be called soon afterward
void patternedtr::muteTrack(unsigned int track)
{
    editor::muted_tracks[track] = true;
}

void patternedtr::unmuteTracks()
{
    for(int i = 0; i < editor::song->numTracks(); i++)
        editor::muted_tracks[i] = false;
}

//toggle muted state within the tracker, muteTracks() should be called soon afterward
void patternedtr::toggleMuteTracks(unsigned int *track, const unsigned int &tracks)
{
    for(int i = 0; i < tracks; i ++)
        editor::muted_tracks[track[i]] = !editor::muted_tracks[track[i]];
}











const char *allkeys = "zsxdcvgbhnjm,l.;/q2w3er5t6y7ui9o0p[=]";
const int all_octswitch = 17;
const char *whitekeys = "zxcvbnm,./qwertyuiop[]\\";
const int white_octswitch = 10;


//Needs testing ---v
unsigned int patternedtr::toKey(const unsigned int &entry, const unsigned char *scale, const unsigned char &key)
{
    unsigned int notes_in_key[12];
    unsigned int note = entry & R_NOTE;
    unsigned int last;
    unsigned int lower = 0;
    unsigned int higher = -1;

    unsigned int base = key*0x02000000;
    int acc, entries;
    for(acc = 0, entries = 0; acc < 12 && entries < 11 && scale[entries] != 0; entries++)
        acc += scale[entries];
    if(entries > 11)
        entries = 11;
    entries-=1;

    last = addNotes(base, 0x00000000) & R_NOTE;
    notes_in_key[0]= last;

    for(int j  = 0; j < entries; j++)
    {
        last = addNotes(last, scale[j]*0x02000000) & R_NOTE;
        notes_in_key[j+1] = last;
    }

    for(int i = 0; i < entries; i++)
    {
        if(notes_in_key[i] == note)
            return entry;

        //Because note_in_key arent sorted. Sorting them would be helpful
        //and invalidate a lot of unneccessary validation in this function

        if(notes_in_key[i] > lower && notes_in_key[i] < note)
            lower = notes_in_key[i];
        else if(notes_in_key[i] < higher && notes_in_key[i] > note)
            higher = notes_in_key[i];
    }

    float dif = higher - lower;
    int x = note - lower;


    //clear note to set to higher or lower note in the key
    //std::cerr << "ENTRY:" << std::hex << entry << ", " << note << '\n';
    //Im a little dubious about this, it seems to only go low
    if( (x / dif) > .5f)
    {
        
        note = entry & R_OCTAVE;
        note |= higher;
        //Correct octave, note must be higher,
        //this is a consequence of omitting octaves in notes_in_key
        if(note < entry)
            note = (note + 0x20000000) & R_PITCHSEG;
        //std::cerr << "HIGH:" << std::hex << note <<'\n';
        return (entry & (~R_PITCHSEG)) | note;
    }
    else
    {
        note = entry & (~R_NOTE);
        note |= lower;
        //Correct octave, note must be lower
        if(note > entry)
            note = (note - 0x20000000) & R_PITCHSEG;
        //std::cerr << "LOW :" << std::hex << note <<'\n';
        return (entry & (~R_PITCHSEG)) | note;
    }

}   







//only adds upwards, no subtraction
unsigned int patternedtr::addNotes(const unsigned int &n1, const unsigned int &n2)
{
    unsigned int out = (n1 & R_OCTAVE) + (n2 & R_OCTAVE);
    unsigned int notes = (n1 & R_NOTE) + (n2 & R_NOTE);

    while(notes > 0x16000000)
    {
        out += 0x20000000;
        notes -= 0x18000000;
    }
    out |= notes;
    return out;
}

//only subtracts
unsigned int patternedtr::subNotes(const unsigned int &n1, const unsigned int &n2)
{
    unsigned int out;
    if(n2 == 0)
        return n1;

    if(n2 > n1)
        return 0;
    out = ((n1 & R_OCTAVE) - (n2 & R_OCTAVE)) & R_OCTAVE;
    std::cerr << "note1 " << std::hex <<  n1 << " note2 " << n2 << '\n';

    unsigned int note1 = (n1 & R_NOTE);
    unsigned int note2 = (n2 & R_NOTE);

    while(note2 > 0x16000000)
    {
        std::cerr << "subtracting: " << out << "--" << (out-0x20000000) << '\n';
        out -= 0x20000000;
        note2 =(note2 - 0x18000000);
    }

    std::cerr << "out=" << std::hex << out << " note1=" << note1 << " note2=" << note2 << '\n';

    if(note2 > note1) 
    {
        out -= 0x20000000;
        std::cerr << "case1, " << out;
        out += (12*0x02000000 - note2) + note1;
        std::cerr << ":" << out << '\n';
    }
    else
    {
        note1 -= note2;
        std::cerr << "case2, " << note1;
        out |= note1;
        std::cerr << ":" << out << '\n';
    }

    return out;
}


void patternedtr::populateNoteMap()
{
    //int numkeys = strlen(whitekeys);
    int acc, i;
    for(acc = 0, i = 0; acc < 12 && i < 11 && scaleconst[i] != 0; i++)
        acc += scaleconst[i];

    unsigned int base = 0x02000000*key,
                 last = base;

    //To control the modulus when
    //the octave changes
    unsigned int offset = 0;

    notemap.clear();

    //If there are more then 9 (10 or 11) notes in a scale,
    //use all keys to represent them
    //otherwise only use white keys
    if(i > 9)
    {
        int numkeys = strlen(allkeys);
        for(int j = 0; j < numkeys; j++)
        {
            if(j == all_octswitch)
            {
                //When the octave switches within a binding
                //move up one octave and reset
                last = addNotes(base, 0x20000000);
                offset = j;
            }
            notemap[allkeys[j]] = last;
            last = addNotes(last, scaleconst[(j- offset)%i]*0x02000000);
        }

    }
    else
    {
        int numkeys = strlen(whitekeys);
        for(int j = 0; j < numkeys; j++)
        {
            if(j == white_octswitch)
            {
                last = addNotes(base, 0x20000000);
                offset = j;
            }
            notemap[whitekeys[j]] = last;
            last = addNotes(last, scaleconst[(j- offset)%i]*0x02000000);
        }
    }

}

void patternedtr::chgSelTrack(int i)
{
    //Change track
    seltrack += i;
    if(seltrack >= selptrn->numTracks())
        if(i > 0)
            seltrack = selptrn->numTracks()-1;
        else
        {
            seltrack = 0;
            viewporttrack=0;
            return;
        }

    if(seltrack <= viewporttrack)
        viewporttrack=seltrack;
    else
    {
        if(seltrack >= viewporttrack + maxtracksviewport)
            viewporttrack = seltrack - maxtracksviewport+1;
    }
}

void patternedtr::chgSelRow(int i)
{
    selrow += i;
    if(selrow >= selptrn->numRows())
        if(i > 0)
        {
            selrow = selptrn->numRows()-1;
        }
        else
        {
            selrow = 0;
        }
    if(selrow < viewportrow)
    {
        if(selrow < 4)
            viewportrow = 0;
        else
        {
            int dif = (viewportrow - selrow) /4 + 1;
            viewportrow-=4*dif;
        }
    }
    else
    {
        int w = editor::ptrnwin->width,h = editor::ptrnwin->height;

        if(selptrn->numRows() > h && (selptrn->numRows()-selrow) < 4)
            viewportrow = selptrn->numRows() - maxrowsviewport + 1;
        else
            if(selrow >= viewportrow + h-1)
            {
                int dif = (selrow-(viewportrow+h-1)) /4 + 1;
                viewportrow+=4*dif;
            }
    }

}


void patternedtr::chgSelRowSeg(int i)
{
    seltrackseg += i;
    if(seltrackseg > TRKSEG_FXP)
        if(i > 0)
        {
            if(seltrack < editor::song->numTracks() - 1)
            {
                seltrackseg = TRKSEG_NOTE;
                chgSelTrack(1);
            }
            else
                seltrackseg = TRKSEG_FXP;
        }
        else
        {
            if(seltrack > 0)
            {
                seltrackseg = TRKSEG_FXP;
                chgSelTrack(-1);
            }
            else
                seltrackseg = TRKSEG_NOTE;
        }
}






char *editor::noteString(char *string, unsigned int entry)
{
    if((entry & R_PITCHSEG) == R_PITCHSEG)
    {
        string[0] = ' ';
        string[1] = ' ';
        string[2] = ' ';
        return string;
    }
    unsigned char  oct  = ((entry & R_OCTAVE) >> RI_OCTAVE);
    unsigned char  note = (entry & R_NOTE) >> RI_NOTE;

    string[1]='-';
    string[2]='0' + oct;
    switch(note)
    {
        case 0:
            string[0]='C';
            break;
        case 1:
            string[0]='C';
            string[1]='#';
            break;
        case 2:
            string[0]='D';
            break;
        case 3:
            string[0]='D';
            string[1]='#';
            break;
        case 4:
            string[0]='E';
            break;
        case 5:
            string[0]='F';
            break;
        case 6:
            string[0]='F';
            string[1]='#';
            break;
        case 7:
            string[0]='G';
            break;
        case 8:
            string[0]='G';
            string[1]='#';
            break;
        case 9:
            string[0]='A';
            break;
        case 10:
            string[0]='A';
            string[1]='#';
            break;
        case 11:
            string[0]='B';
            break;
    }
    return string;
}

char *editor::instString(char *string, const unsigned int &entry)
{
    if((entry & R_INSTRUMENT)==R_INSTRUMENT)
    {
        string[0] = ' ';
        string[1] = ' ';
        return string;
    }
    unsigned char inst = (entry & R_INSTRUMENT) >> RI_INSTRUMENT;
    editor::byteString(string, inst);
    return string;
}

char *editor::volString(char *string, const unsigned int &entry)
{
    unsigned char  vol  = (entry & R_VOLUME) >> RI_VOLUME;
    if(vol == 0x3f && (entry & R_FULLSEG) == R_FULLSEG)
    {
        string[0] = ' ';
        string[1] = ' ';
    }
    else
        editor::byteString(string, vol);
    return string;
}

char *editor::fxString(char *string, const unsigned int &entry)
{
    unsigned char  fx   = (entry & R_EFFECT) >> (RI_EFFECT);

    unsigned char  fxp1 = (entry & R_FXPARAM1) >> RI_FXPARAM1;
    unsigned char  fxp2 = (entry & R_FXPARAM2);
    //fx should be validated before this point
    if(fx+fxp1+fxp2 == 0)
    {
        string[0] = ' ';
        return string;
    }

    string[0] = hexnums[fx];
    return string;
}
char *editor::fxpString(char *string, const unsigned int &entry)
{
    unsigned char  fx   = (entry & R_EFFECT) >> RI_EFFECT;

    unsigned char  fxp1 = (entry & R_FXPARAM1) >> RI_FXPARAM1;
    unsigned char  fxp2 = (entry & R_FXPARAM2);
    //fx should be validated before this point
    if(fx + fxp1 + fxp2 == 0)
    {
        string[0] = ' ';
        string[1] = ' ';
        return string;
    }
    string[0] = hexnums[fxp1];
    string[1] = hexnums[fxp2];
    return string;
}

char *patternedtr::makeSpaceBuffer(char *bfr, int length)
{

    switch(length)
    {
        case 1:
            bfr[0] = '.';
            break;
        case 2:
            bfr[0]='.';
            bfr[1]='.';
            break;
        case 3:
            bfr[0]='|';
            bfr[1]='.';
            bfr[2]='|';
            break;
        case 4:
            bfr[0]='|';
            bfr[1]='.';
            bfr[2]='.';
            bfr[3]='|';
            break;
        case 5:
            bfr[0]='|';
            bfr[1]=' ';
            bfr[2]='.';
            bfr[3]=' ';
            bfr[4]='|';
            break;
    }
    if(length > 5)
    {
        int half = (length)/2;
        bfr[0] = '|';
        bfr[1] = ' ';
        bfr[2] = '.';
        bfr[3] = '.';
        for(int i = 4; i < length-1; i++)
            bfr[i] = ' ';
        bfr[length-1] = '|';
    }
    bfr[length] = 0;

    return bfr;
}

void editor::copy(const char *src, char *dest, int len)
{
    for(int i = 0; i < len; i++)
        dest[i] = src[i];
}


void patternedtr::setPatternAttribs(unsigned char track, unsigned char row, unsigned char seg)
{
    attroff(-1);
    attron(A_BOLD);



    

    if(editor::inputwin == editor::ptrnwin)
    {
        const unsigned temp = 0;

        if(editor::muted_tracks[track])
        {
            if(row == selrow)
            {
                if(track == seltrack)
                {
                    if(seg == seltrackseg)
                        attron(COLOR_PAIR(COL_PTRN_MUTE_SS));
                    else
                        attron(COLOR_PAIR(COL_PTRN_MUTE_SU));
                    return;
                }
                else
                {
                    attron(COLOR_PAIR(COL_PTRN_MUTE_SU));
                }
            }
            else
            {
                attron(COLOR_PAIR(COL_PTRN_MUTE_US));
            }
        }
        else
        {
            if(row_underline > 0)
            {
                bool highlight_row = ((row+1) % row_underline) == 0;
                if(highlight_row)
                    attron(A_UNDERLINE);
            }
            if(row == selrow)
            {
                if(track == seltrack )
                    if(seg == seltrackseg)
                    {
                        attron(COLOR_PAIR(COL_PTRN_SSS));
                        return;
                    }
                    else
                    {
                        switch(seg)
                        {
                            case TRKSEG_NOTE:
                                attron(COLOR_PAIR(COL_PTRN_SSU_NOTE));
                                return;
                            case TRKSEG_INST:
                                attron(COLOR_PAIR(COL_PTRN_SSU_INST));
                                return;
                            case TRKSEG_VOL:
                                attron(COLOR_PAIR(COL_PTRN_SSU_VOL));
                                return;
                            case TRKSEG_FX:
                            case TRKSEG_FXP:
                                attron(COLOR_PAIR(COL_PTRN_SSU_FX));
                                return;
                        }
                    }
            }



            if(row == editor::playback_mark)
            {
                attron(COLOR_PAIR(COL_PTRN_MARK));
                return;
            }

            switch(seg)
            {
                case TRKSEG_NOTE:
                    attron(COLOR_PAIR(COL_PTRN_SU_NOTE));
                    return;
                case TRKSEG_INST:
                    attron(COLOR_PAIR(COL_PTRN_SU_INST));
                    return;
                case TRKSEG_VOL:
                    attron(COLOR_PAIR(COL_PTRN_SU_VOL));
                    return;
                case TRKSEG_FX:
                case TRKSEG_FXP:
                    attron(COLOR_PAIR(COL_PTRN_SU_FX));
                    return;
            }
            attron(COLOR_PAIR(COL_META_SU));
        }

    }
    else
    {

        if(editor::muted_tracks[track])
        {
            if(track == seltrack && row == selrow)
                attron(COLOR_PAIR(COL_PTRN_MUTE_US));
            else
                attron(COLOR_PAIR(COL_PTRN_MUTE_UU));
            
        }
        else
        {
            if(row_underline > 0)
            {
                bool highlight_row = ((row+1) % row_underline) == 0;
                if(highlight_row)
                    attron(A_UNDERLINE);
            }
            if(track == seltrack && row == selrow)
                attron(COLOR_PAIR(COL_PTRN_US));
            else
                attron(COLOR_PAIR(COL_PTRN_UU));
        }
    }
}


void patternedtr::displayPattern()
{
    using namespace editor;
    attroff(-1);

    int w = editor::ptrnwin->width, h = editor::ptrnwin->height;
    int tracks_visible = w/TRACK_WIDTH;


    unsigned char minrow = viewportrow;
    unsigned char maxrow = viewportrow+h - 1;
    unsigned char mintrack = viewporttrack;
    unsigned char maxtrack = viewporttrack + tracks_visible;
    if(selptrn != NULL)
    {
        if(maxrow > selptrn->numRows())
            maxrow = selptrn->numRows();   
        if(maxtrack > selptrn->numTracks())
            maxtrack = selptrn->numTracks();
    }
    tracks_visible = maxtrack - mintrack;

    //std::cerr << "DISPLAYING PATTERN  mintrack=" << (int)mintrack << " maxtrack =" << (int)maxtrack << "tracksvis=" << tracks_visible <<  "\n";

    char name[4];
    name[2] = 0;
    unsigned char visiblerows = maxrow - minrow;
    mvprintw(3, 0, "  ", ptrnwin);
    for(int i = 0; i < visiblerows; i++)
    {
        byteString(name, viewportrow+i);
        mvprintw(4+i, 0, name, ptrnwin);
    }

    //clear spot where rows arent
    for(int i = visiblerows; i < viewportrow+h-1; i++)
        mvprintw(4+i, 0, "  ", ptrnwin);

    for(int i = 0; i < tracks_visible; i++)
    {
        unsigned char xoff = 2+TRACK_WIDTH*i;
        attroff(-1);
        if(muted_tracks[viewporttrack + i])
            mvprintw(3,xoff, "|TRACK_  __<x_|",ptrnwin);
        else
            mvprintw(3,xoff, "|TRACK_  __<)_|",ptrnwin);
        name[2] = 0;
        byteString(name, viewporttrack+i);
        mvprintw(3, xoff + 7, name, ptrnwin);
        int j;
        for(j = 0; j < visiblerows; j++)
        {
            //std::cerr << "   neato0:" << int(selptrn->numRows());
            unsigned int entry = selptrn->at(viewporttrack+i,viewportrow+j);
            //std::cerr << "   neato1\n";
            if(entry == R_EMPTY)
            {
                //Print empty entry
                setPatternAttribs(viewporttrack+i,viewportrow+j, TRKSEG_NOTE);
                mvprintw(4+j, xoff, "|   ",ptrnwin);
                setPatternAttribs(viewporttrack+i,viewportrow+j, TRKSEG_INST);
                mvprintw(4+j, xoff + 4, "   ", ptrnwin);
                setPatternAttribs(viewporttrack+i,viewportrow+j, TRKSEG_VOL);
                mvprintw(4+j, xoff + 7, "   ", ptrnwin);
                setPatternAttribs(viewporttrack+i,viewportrow+j, TRKSEG_FX);
                mvprintw(4+j, xoff + 10, "  ", ptrnwin);
                setPatternAttribs(viewporttrack+i,viewportrow+j, TRKSEG_FXP);
                mvprintw(4+j, xoff + 12, "  |", ptrnwin);
            }
            else
            {
                //print note!
                setPatternAttribs(viewporttrack+i,viewportrow+j, TRKSEG_NOTE);
                // name="|XXX"
                //the X portion is modified by noteString
                noteString(name+1, entry);
                name[0] = '|';
                name[4] = 0;
                mvprintw(4+j, xoff, name, ptrnwin);

                //print instrument
                setPatternAttribs(viewporttrack+i,viewportrow+j, TRKSEG_INST);
                // name=" XX"
                name[0] = ' ';
                instString(name+1, entry);
                mvprintw(4+j, xoff + 4, name, ptrnwin);


                //print volume
                setPatternAttribs(viewporttrack+i,viewportrow+j, TRKSEG_VOL);
                // name=" XX"
                volString(name+1, entry);
                mvprintw(4+j, xoff + 7, name, ptrnwin);


                //print effect
                setPatternAttribs(viewporttrack+i,viewportrow+j, TRKSEG_FX);
                // name=" X"
                name[2] = 0;
                fxString(name+1, entry);
                mvprintw(4+j, xoff + 10, name, ptrnwin);

                //print effect parameters
                setPatternAttribs(viewporttrack+i,viewportrow+j, TRKSEG_FXP);
                // name="XX|"
                name[2] = '|';
                name[3] = 0;
                fxpString(name, entry);
                mvprintw(4+j, xoff + 12, name, ptrnwin);       
            }
        }

        attroff(-1);
        while(j < viewportrow+h - 1)
        {
            mvprintw(4+j, xoff, "|             |",ptrnwin);
            j++;
        }
    }
    //std::cerr << "PRINTING SPACER maxtracksviewport=" << maxtracksviewport << " w=" << w << '\n';



    {//Print Spacer "|_..______" 

        unsigned char xoff; 
        if(song->numTracks() < maxtracksviewport)
            xoff = 3+TRACK_WIDTH*song->numTracks()-1;
        else
            xoff = 3+TRACK_WIDTH*maxtracksviewport-1;

        int spacerwidth = w - xoff;
        char *spacebuffer = new char[spacerwidth+1];
        makeSpaceBuffer(spacebuffer, spacerwidth);
        attroff(-1);
        attron(COLOR_PAIR(COL_PTRN_UU));

        
        for(int j = 0; j < h; j++)
            mvprintw(4+j, xoff, spacebuffer, ptrnwin);


        for(int j = 0; j < spacerwidth; j++)
            if(spacebuffer[j] == ' ')
                spacebuffer[j] = '_';
        mvprintw(3,xoff, spacebuffer, ptrnwin);

        delete [] spacebuffer;
    }//End Print Spacer
    //std::cerr << "DISPLAYED PATTERN\n";
}

void patternedtr::display()
{
    using namespace editor;

    displayPattern();
    
    instedtr::displayWav();
    instedtr::displayPulse();
    
    //print meta after pattern so that meta tooltipts overlap pattern
    displayMeta();
    attroff(-1);

}

bool alphanumeric(int c)
{
    return(c >='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9');
}

void patternedtr::processPatternInput(int in)
{
    switch(in)
    {
        case KEY_UP:
            chgSelRow(-1);
            break;
        case KEY_DOWN:
            chgSelRow(1);
            break;
        case KEY_LEFT:
            chgSelRowSeg(-1);
            break;
        case KEY_RIGHT:
            chgSelRowSeg(1);
            break;
        case KEY_PPAGE:
            chgSelRow(-8);
            break;
        case KEY_NPAGE:
            chgSelRow(8);
            break;
        case KEY_HOME:
            chgSelTrack(-1);
            break;
        case KEY_END:
            chgSelTrack(1);
            break;
        case KEY_IC:
            patternedtr::selptrn->insertRow(seltrack, selrow);
            break;
        case KEY_SHOME:
            chgSelTrack(-seltrack);
            //doesnt seem to work
            break;
        case KEY_SEND:
            chgSelTrack(editor::song->numTracks() - seltrack - 1);
            //doesnt seem to work
            break;
        case '\n':
            {
                patternedtr::entryclipboard = selptrn->at(seltrack, selrow);
                unsigned char inst = (patternedtr::entryclipboard & R_INSTRUMENT) >> RI_INSTRUMENT;
                if(inst < editor::song->numInstruments())
                {
                    patternedtr::selinstrument = inst;
                    editor::selinst   = editor::song->getInstrument(patternedtr::selinstrument);
                    instedtr::selwavrow = editor::selinst->getWaveIndex(); 
                    if(editor::selinst->getPulseIndex() != 0xFFFF)
                        instedtr::selpulrow = editor::selinst->getPulseIndex(); 
                }
                if((patternedtr::entryclipboard & R_PITCHSEG) != R_PITCHSEG)
                {
                    patternedtr::octave = ((patternedtr::entryclipboard & R_OCTAVE) >> RI_OCTAVE) - 1;
                    if(patternedtr::octave > 7)
                        patternedtr::octave = 0;
                }
                if((patternedtr::entryclipboard & 0xF00) == 0x700)
                {
                    if((patternedtr::entryclipboard & 0xFF) < editor::song->numWaveEntries())
                    {
                        instedtr::selwavrow = patternedtr::entryclipboard & 0xFF; 
                    }

                }
                else if((patternedtr::entryclipboard & 0xF00) == 0x900)
                {
                    if((patternedtr::entryclipboard & 0xFF) < editor::song->numPulseEntries())
                    {
                        instedtr::selpulrow = patternedtr::entryclipboard & 0xFF; 
                    }

                }
            }
            break;
        case ' ':
            selptrn->setAt(seltrack, selrow, patternedtr::entryclipboard);
            chgSelRow(patternedtr::edit_step);
            break;
        case '!':
            octave = 0;
            break;
        case '@':
            octave = 1;
            break;
        case '#':
            octave = 2;
            break;
        case '$':
            octave = 3;
            break;
        case '%':
            octave = 4;
            break;
        case '^':
            octave = 5;
            break;
        case '&':
            octave = 6;
            break;

        case '{'://Decrement Order
            patternedtr::chgSelOrder(-1);
            break;
        case '}'://Increment Order
            patternedtr::chgSelOrder(1);
            break;
        case '>'://Decrement Pattern
            patternedtr::chgOrderPattern(-1);
            break;
        case '?'://Increment Pattern
            patternedtr::chgOrderPattern(1);
            break;


        case 'M'://set mark

            if( editor::playback_mark != patternedtr::selrow || editor::playback_mark == 0xFF)
                editor::playback_mark = patternedtr::selrow;
            else
                editor::playback_mark = 0xFF;
            break;
        default:
            //process as input for an entry of the pattern editor
            entryInput(in);
            break;
    }

}

void patternedtr::entryInput(int in)
{
    bool ishex = editor::validateHexChar(in);
    bool resolved = false;
    unsigned char val;
    switch(seltrackseg)
    {
        case TRKSEG_NOTE:
            {
                try
                {
                    unsigned int note = notemap.at(in);
                    unsigned int entry = selptrn->at(seltrack, selrow);
                    //if((entry & R_INSTRUMENT) == R_INSTRUMENT)
                    if(entry == R_EMPTY)
                    {
                        //If it was an empty note, set to
                        //selected 
                        entry &= ~R_INSTRUMENT;
                        entry |= selinstrument << RI_INSTRUMENT;
                    }

                    //Copy the last volume used in the track
                    unsigned int prevvolmask = R_VOLUME;
                    for(int i = patternedtr::selrow; i >= 0; i--)
                    {
                        if((selptrn->at(seltrack,i) & ~R_EFFECTSEG) != R_EMPTY)
                        {
                            prevvolmask = selptrn->at(seltrack,i) & R_VOLUME;
                             break;
                        }
                    }
                    if(prevvolmask == 0)
                        prevvolmask = R_VOLUME;

                    entry &= ~(R_PITCHSEG | R_VOLUME);
                    entry += note;
                    entry += patternedtr::octave << RI_OCTAVE;
                    entry |= prevvolmask; 

                    selptrn->setAt(seltrack,selrow,entry);
                    editor::playExcerptImplicit();

                    resolved=true;

                    chgSelRow(patternedtr::edit_step);
                }
                catch(std::out_of_range &o) //notemap.at causes this if not found
                {
                    if(in == KEY_DC)
                    {
                        unsigned int entry = selptrn->at(seltrack, selrow);
                        if((entry & R_INSTRUMENT) != R_INSTRUMENT)
                            entry = R_EMPTY;
                        else
                            entry |= R_FULLSEG;
                        selptrn->setAt(seltrack,selrow,entry);
                        chgSelRow(patternedtr::edit_step);

                        resolved = true;
                    }
                }

            }
            break;
        case TRKSEG_INST:
            if(ishex)
            {
                unsigned int entry = selptrn->at(seltrack, selrow);
                unsigned char inst = (entry & R_INSTRUMENT) >> RI_INSTRUMENT;
                val = editor::charHex(in);
                entry &= ~(R_INSTRUMENT | R_VOLUME);
                inst *= 0x10;

                if(inst >= editor::song->numInstruments())
                    inst = 0;

                inst |= val;

                if(inst >= editor::song->numInstruments())
                    inst = editor::song->numInstruments()-1;

                //Copy the last volume used in the track
                unsigned int prevvolmask = 0x3f;
                for(int i = patternedtr::selrow; i >= 0; i--)
                {
                    if((selptrn->at(seltrack,i) & ~R_EFFECTSEG) != R_EMPTY)
                    {
                        prevvolmask = selptrn->at(seltrack,i) & R_VOLUME;
                        break;
                    }
                }
                if(prevvolmask == 0)
                    prevvolmask = 0x3f;

                entry |= prevvolmask;
                entry |= inst << RI_INSTRUMENT;
                selptrn->setAt(seltrack,selrow,entry);
                resolved=true;

            }
            else
            {
                if(in == KEY_DC)//DELETE
                {
                    unsigned int entry = selptrn->at(seltrack, selrow);
                    entry |= R_INSTRUMENT;
                    selptrn->setAt(seltrack,selrow,entry);
                    resolved = true;
                }

            }
            break;
        case TRKSEG_VOL:
            if(ishex)
            {
                unsigned int entry = selptrn->at(seltrack, selrow);
                unsigned char vol;
                if((entry & R_EMPTY) == R_EMPTY)
                    vol = 0;
                else
                    vol = (entry & R_VOLUME) >> RI_VOLUME;
                val = editor::charHex(in);
                entry &= ~R_VOLUME;
                vol *= 0x10;
                if(vol>=0x40)
                    vol = 0x30;
                vol |= val;
                entry |= vol << RI_VOLUME;
                selptrn->setAt(seltrack,selrow,entry);
                resolved=true;
            }
            else
            {
                if(in == KEY_DC)//DELETE
                {
                    unsigned int entry = selptrn->at(seltrack, selrow);
                    entry = R_EMPTY;
                    selptrn->setAt(seltrack,selrow,entry);
                    resolved = true;
                }

            }
            break;
        case TRKSEG_FX:
            if(ishex)
            {
                unsigned int entry = selptrn->at(seltrack, selrow);
                val = editor::charHex(in);
                entry &= ~R_EFFECT;
                entry |= val << RI_EFFECT;
                selptrn->setAt(seltrack,selrow,entry);
                resolved=true;
            }
            else
            {
                if(in == KEY_DC) //DELETE
                {
                    unsigned int entry = selptrn->at(seltrack, selrow);
                    val = editor::charHex(in);
                    entry &= ~(R_EFFECT | R_FXPARAM);
                    selptrn->setAt(seltrack,selrow,entry);
                    resolved = true;
                }

            }
            break;
        case TRKSEG_FXP:
            if(ishex)
            {
                unsigned int entry = selptrn->at(seltrack, selrow);
                unsigned char  fxp = entry & (R_FXPARAM);
                val = editor::charHex(in);
                entry &= ~R_FXPARAM;
                fxp *= 0x10;
                fxp |= val;
                entry |= fxp;
                selptrn->setAt(seltrack,selrow,entry);
                resolved=true;
            }
            else
            {
                if(in == KEY_DC)//DELETE
                {
                    unsigned int entry = selptrn->at(seltrack, selrow);
                    val = editor::charHex(in);
                    entry &= ~R_FXPARAM;
                    selptrn->setAt(seltrack,selrow,entry);
                    resolved = true;
                }
            }
            break;
    }

}


void patternedtr::processInput(int in)
{

    if(editor::inputwin == editor::metawin)
        processMetaInput(in);    
    else if(editor::inputwin == editor::ptrnwin)
        processPatternInput(in);
    else if(editor::inputwin == editor::wavewin)
        instedtr::processInputWav(in);
    else if(editor::inputwin == editor::pulsewin)
        instedtr::processInputPulse(in);

}

