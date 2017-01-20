#include "cmdbar.h"


std::map<std::string, editor::cmd_handler> editor::cmdbar_cmdmap;
std::map<std::string, unsigned int> editor::scalenamemap;

void editor::populateCmdMap()
{
    cmdbar_cmdmap["q"] =  handle_q;
    cmdbar_cmdmap["w"] =  handle_w;
    cmdbar_cmdmap["wq"] =  handle_wq;
    cmdbar_cmdmap["n"] =  handle_n;
    cmdbar_cmdmap["o"] =  handle_o;
    cmdbar_cmdmap["e"] =  handle_e;

    cmdbar_cmdmap["d"] =  handle_d;
    cmdbar_cmdmap["dd"] =  handle_dd;
    cmdbar_cmdmap["ddd"] =  handle_ddd;

    cmdbar_cmdmap["y"] =  handle_y;
    cmdbar_cmdmap["yy"] =  handle_yy;

    cmdbar_cmdmap["amp"] =  handle_amp;
    cmdbar_cmdmap["ampinst"] =  handle_ampinst;
    cmdbar_cmdmap["ampinstall"] =  handle_ampinstall;
    cmdbar_cmdmap["ampmark"] =  handle_ampmark;
    cmdbar_cmdmap["amplin"] = handle_amplin;

    cmdbar_cmdmap["resize"] =  handle_resize;
    cmdbar_cmdmap["resizesong"] =  handle_resizesong;
    
    cmdbar_cmdmap["instset"] = handle_instset;
    cmdbar_cmdmap["instsetmark"] = handle_instsetmark;

    cmdbar_cmdmap["fxset"] = handle_fxset;
    cmdbar_cmdmap["fxsetmark"] = handle_fxsetmark;

    cmdbar_cmdmap["ins"] = handle_ins;
    cmdbar_cmdmap["entryins"] = handle_entryins;
    cmdbar_cmdmap["rowins"] = handle_rowins;
    cmdbar_cmdmap["entrydel"] = handle_entrydel;
    cmdbar_cmdmap["rowdel"] = handle_rowdel;

    cmdbar_cmdmap["trans"] = handle_trans;
    cmdbar_cmdmap["transsong"] = handle_transsong;
    cmdbar_cmdmap["transkey"] = handle_transkey;
    cmdbar_cmdmap["transkeysong"] = handle_transkeysong;
    cmdbar_cmdmap["key"] = handle_key;
    cmdbar_cmdmap["scale"] = handle_scale;

    cmdbar_cmdmap["step"] = handle_step;
    cmdbar_cmdmap["oct"] = handle_oct;

    cmdbar_cmdmap["wave"] = handle_wave;
    cmdbar_cmdmap["inst"] = handle_inst;
    cmdbar_cmdmap["order"] = handle_order;
    cmdbar_cmdmap["ptrn"] = handle_ptrn;
    cmdbar_cmdmap["track"] = handle_track;
    cmdbar_cmdmap["row"] = handle_row;
    cmdbar_cmdmap["r"] = handle_row;
    cmdbar_cmdmap["t"] = handle_track;

    cmdbar_cmdmap["mark"] = handle_mark;
    cmdbar_cmdmap["playlen"] = handle_playlen;
    cmdbar_cmdmap["render"] = handle_render;
    cmdbar_cmdmap["mute"] = handle_mute;
    cmdbar_cmdmap["unmute"] = handle_unmute;
    cmdbar_cmdmap["play"] = handle_play;
    cmdbar_cmdmap["stop"] = handle_stop;
    cmdbar_cmdmap["playamp"] = handle_playamp;
    cmdbar_cmdmap["pid"] = handle_pid;

    cmdbar_cmdmap["fxchange"] = handle_fxchange;

}

int editor::parseKeySignature(char *key)
{
    if(key[0] >= 'a' && key[0] <= 'z')
        key[0] -= 32;

    if(strcmp(key,"C") == 0)
        return 0;
    else if(strcmp(key,"C#") == 0)
        return 1;
    else if(strcmp(key,"D") == 0)
        return 2;
    else if(strcmp(key,"D#") == 0)
        return 3;
    else if(strcmp(key,"E") == 0)
        return 4;
    else if(strcmp(key,"F") == 0)
        return 5;
    else if(strcmp(key,"F#") == 0)
        return 6;
    else if(strcmp(key,"G") == 0)
        return 7;
    else if(strcmp(key,"G#") == 0)
        return 8;
    else if(strcmp(key,"A") == 0)
        return 9;
    else if(strcmp(key,"A#") == 0)
        return 10;
    else if(strcmp(key,"B") == 0)
        return 11;
    return 0;
}


bool editor::getCommand(const char * autocmd)
{
    //Command bar displayed at the bottom
    int ypos = WIN_HEIGHT -1;
    wint_t in;      //to grab characters
    char sel[2]; //to display one character
    sel[1] = 0;

    //use the numBuffer to cycle through the history
    numBuffer = history_head;

    //copy the input into the input buffer
    for(in = 0; autocmd[in]; in++)
        charInputBuffer[in] = autocmd[in];

    //put the cursor at the end of the charInputBuffer
    //and zero out the rest of the input buffer
    textCursorPos = in;
    for(;in < CMDBAR_SIZE; in++)
        charInputBuffer[in] = 0;

    //use the charBuffer to prepare and display the
    //charInputBuffer
    editor::copy(charInputBuffer, charBuffer, CMDBAR_SIZE);
    editor::makeUnderlines(charBuffer, CMDBAR_SIZE-1);

    //prepare cmdbar line  and charBuffer
    attroff(-1);
    attron(COLOR_PAIR(patternedtr::COL_CMDBAR_U));
    mvprintw(ypos, 0, std::string(WIN_WIDTH, ' ').c_str(), stdscr);
    mvprintw(ypos, 0, ":", stdscr);
    mvprintw(ypos, 1, charBuffer, stdscr);

    //print cursor as the character with the BOLD attribute
    sel[0] = charBuffer[textCursorPos];
    attron(COLOR_PAIR(patternedtr::COL_CMDBAR_S));
    attron(A_BOLD);
    mvprintw(ypos, 1+textCursorPos, sel, stdscr);

    //loop until newline or tab character
    get_wch(&in);
    while(in  != '\n' && in != '\t')
    {
        switch(in)
        {
            case KEY_LEFT: //select prev character
                if(textCursorPos > 0)
                    textCursorPos--;

                break;
            case KEY_RIGHT://select next character
                if(textCursorPos < strlen(charInputBuffer))
                    textCursorPos++;

                break;
            case KEY_UP://History up
                numBuffer--;
                if(numBuffer < 0)
                    numBuffer = history_size - 1;
                copy(charInputHistory[numBuffer], charInputBuffer, CMDBAR_SIZE);
                textCursorPos = strlen(charInputBuffer);

                break;
            case KEY_DOWN://History down

                numBuffer++;
                if(numBuffer >= history_size)
                    numBuffer = 0;
                copy(charInputHistory[numBuffer], charInputBuffer, CMDBAR_SIZE);
                textCursorPos = strlen(charInputBuffer);
                break;
            case KEY_HOME://Select beginning of cmdbar
                textCursorPos = 0;
                break;
            case KEY_END://Select end of cmdbar
                textCursorPos = strlen(charInputBuffer);
                break;
            case KEY_IC://Move the rest of the characters in the input buffer upwards
                {
                    int length = strlen(charInputBuffer);
                    for(int i = length; i > textCursorPos; i--)
                        charInputBuffer[i] = charInputBuffer[i-1];
                    charInputBuffer[length+1] = 0;
                    charInputBuffer[textCursorPos] = ' ';
                }
            case KEY_DC://Move the rest of the characters in the inout buffer downwrds
            case ALT_BACKSPACE:
                {
                    int length = strlen(charInputBuffer);
                    for(int i = textCursorPos+1; i < length; i++)
                        charInputBuffer[i-1] = charInputBuffer[i];
                    charInputBuffer[length-1] = 0;
                    if(textCursorPos >= length && length > 0)
                        textCursorPos--;
                }
                break;
            default:
                //All other characters are written into the input
                //buffer to be parsed as a commadn
                {
                    if(in <= 'z' && in >= ' ')
                    {
                        charInputBuffer[textCursorPos] = in;
                        if(textCursorPos < strlen(charInputBuffer))
                            textCursorPos++;           
                    }
                }

        }
        //after input, prepare to display the output again
        editor::copy(charInputBuffer, charBuffer, CMDBAR_SIZE);
        editor::makeUnderlines(charBuffer, CMDBAR_SIZE-1);
        attroff(-1);
        attron(COLOR_PAIR(patternedtr::COL_CMDBAR_U));
        mvprintw(ypos, 1, charBuffer, stdscr);

        //print cursor
        sel[0] = charBuffer[textCursorPos];
        attron(COLOR_PAIR(patternedtr::COL_CMDBAR_S));
        attron(A_BOLD);
        mvprintw(ypos, 1+textCursorPos, sel, stdscr);
        get_wch(&in);

    }

    //Dont do any command if tab is caught
    if(in == '\t')
        return false;

    //Store the inputBuffer into the history
    copy(charInputBuffer, charInputHistory[history_head], CMDBAR_SIZE);

    //increment the head of the circular vector
    history_head++;
    if(history_head >= HISTORY_SIZE)
        history_head = 0;

    //increment the amount of object in history as long
    history_size++;
    if(history_size > HISTORY_SIZE)

        history_size = HISTORY_SIZE;
    //perform action
    return true;

}

void editor::tokenize(std::vector<char*> &tokens, int i)
{
    //seperate words based on whether the last character was a space
    //All delimiting spaces are replaced with null character
    bool wasspace = true;
    for(; i < CMDBAR_SIZE-1; i++)
    {
        if(wasspace)
        {
            if(charInputBuffer[i] != 0 && charInputBuffer[i] != ' ')
            {
                tokens.push_back(charInputBuffer + i);
                wasspace = false;
            }
        }
        else
        {
            if(charInputBuffer[i] == 0 || charInputBuffer[i] == ' ')
            {
                charInputBuffer[i] = 0;
                wasspace = true;
            }
        }
    }
}

void editor::initializeScaleMap()
{
    scalenamemap["CHROMATIC"] = SCALE_CHROMATIC;
    scalenamemap["CHR"] = SCALE_CHROMATIC;
    scalenamemap["ALL"] = SCALE_CHROMATIC;


    scalenamemap["IONIAN"] = SCALE_MAJOR;
    scalenamemap["MAJOR"] = SCALE_MAJOR;
    scalenamemap["MJR"] = SCALE_MAJOR;
    scalenamemap["MAJ"] = SCALE_MAJOR;
    scalenamemap["MJ"] = SCALE_MAJOR;

    scalenamemap["AEOLIAN"] = SCALE_AEOLIAN;
    scalenamemap["MINOR"] = SCALE_AEOLIAN;
    scalenamemap["MIN"] = SCALE_AEOLIAN;
    scalenamemap["NMINOR"] = SCALE_AEOLIAN;
    scalenamemap["NATMIN"] = SCALE_AEOLIAN;
    scalenamemap["NATURAL_MINOR"] = SCALE_AEOLIAN;
    scalenamemap["AEOLIAN_MINOR"] = SCALE_AEOLIAN;

    scalenamemap["HARMONIC_MINOR"] = SCALE_HARMONIC;
    scalenamemap["HRM_MINOR"] = SCALE_HARMONIC;
    scalenamemap["HMIN"] = SCALE_HARMONIC;
    scalenamemap["HAR"] = SCALE_HARMONIC;
    scalenamemap["HMINOR"] = SCALE_HARMONIC;
    scalenamemap["MINOR_HRMON"] = SCALE_HARMONIC;
    scalenamemap["MINOR_HARMONIC"] = SCALE_HARMONIC;

    scalenamemap["MELODIC_MINOR"] = SCALE_MELODIC;
    scalenamemap["MEL_MINOR"] = SCALE_MELODIC;
    scalenamemap["MMIN"] = SCALE_MELODIC;
    scalenamemap["MEL"] = SCALE_MELODIC;
    scalenamemap["MMINOR"] = SCALE_MELODIC;
    scalenamemap["MINOR_MELOD"] = SCALE_MELODIC;
    scalenamemap["MINOR_MELODIC"] = SCALE_MELODIC;

    scalenamemap["DORIAN"] = SCALE_DORIAN;
    scalenamemap["DOR"] = SCALE_DORIAN;

    scalenamemap["PHYRGIAN"] = SCALE_PHYRGIAN;
    scalenamemap["PHY"] = SCALE_PHYRGIAN;

    scalenamemap["LYDIAN"] = SCALE_LYDIAN;
    scalenamemap["LYD"] = SCALE_LYDIAN;

    scalenamemap["MIXOLYDIAN"] = SCALE_MIXOLYDIAN;
    scalenamemap["MIXO"] = SCALE_MIXOLYDIAN;
    scalenamemap["MIX"] = SCALE_MIXOLYDIAN;

    scalenamemap["LOCRIAN"] = SCALE_LOCRIAN;
    scalenamemap["LOC"] = SCALE_LOCRIAN;
    scalenamemap["LOCR"] = SCALE_LOCRIAN;
}


bool editor::parseScale(char *str, unsigned char &spinner, unsigned char *bfr)
{
    if(str[0] >= '1' && str[0] <= '9')
    {
        //parse as sequence
        int acc, i;

        for(acc = 0, i = 0; acc < 12 && i < 11 && str[i] != 0; i++)
        {
            bfr[i] = str[i]- '0';
            acc += bfr[i];
        }
        if(acc > 12)
            bfr[i-1] -= acc - 12;
        else if(acc < 12)
        {
            bfr[i] = 12 - acc;
            acc += bfr[i];
            i++;
        }
        for(;i < 12; i++)
            bfr[i] = 0;
        patternedtr::inferScaleType(bfr);
    }
    else
    {
        //Parse by name
        int len = strlen(str);
        for(int i = 0; i < len; i++)
        {
            if(str[i]>='a' && str[i] <= 'z')
                str[i]-=32;        
        }
       
        try
        {
            int num = scalenamemap[std::string(str)];
            spinner = num;
            patternedtr::generateScaleFromType(bfr, spinner);
        }
        catch(const std::out_of_range &e)
        {
            return false;
        }


    }
    return true;
}

unsigned int editor::parseUnsigned(char *str)
{
    int len = strlen(str);
    unsigned int acc =0;
    for(int i = 0; i < len; i++)
    {
        acc *= 0x10; //Horners method!
        if(str[i] >= 'A')
        {
            if(str[i] >= 'a' && str[i] <= 'f')
                acc += (str[i] - 'a' + 0xa);
            else if(str[i] <= 'F')
                acc += (str[i] - 'A' + 0xA);
        }
        else
            if(str[i] >= '0' && str[i] <= '9')
                acc += (str[i] - '0');
    }
    return acc;
}

int editor::parseSigned(char *str)
{
    int acc = 0;
    int len = strlen(str);
    int start = 0;

    bool negative = false;
    if(str[0] == '-')
    {
        negative = true;
        start = 1;
    }
    else if(str[0] == '+')
        start = 1;

    for(int i = start; i < len; i++)
    {
        acc *= 0x10; //Horners!
        if(str[i] >= 'A')
            if(str[i] >= 'a')
                acc += (str[i] - 'a' + 0xa);
            else
                acc += (str[i] - 'A' + 0xA);
        else
            acc += (str[i] - '0');
    }
    if(negative)
        return -acc;
    return acc;
}

float editor::parseFloat(char *str)
{
    float out;
    sscanf( str, "%f", &out);
    return out;
}

void editor::doCommand()
{
    int tokeni = 0;
    int tokenend = 0;
    int len = strlen(charInputBuffer);
    if(len < 1)
        return;

    //find first token index (first non whitespace)
    while(charInputBuffer[tokeni] == ' ' && tokeni < len)
        tokeni++;
    if(tokeni == len)
        return;

    tokenend = tokeni;
    while(charInputBuffer[tokenend] != ' ' && tokenend < len)
        tokenend++;

    charInputBuffer[tokenend] = 0;
    std::vector<char *> tokens;

    //Populate `tokens` with pointers to the tokens, starting at the end
    //of the first token
    editor::tokenize(tokens, tokenend);



    try
    {
        cmd_handler command = cmdbar_cmdmap.at(charInputBuffer+tokeni);
        command(tokens);
    }
    catch(const std::out_of_range &e)
    {
        editor::inform("Input misunderstood              :(");
    }


}

void editor::handle_q(std::vector<char*> &params)
{
    bool ans = editor::confirm("Exit?");
    if(ans)
        patternedtr::exit();

}

void editor::handle_w(std::vector<char*> &params)
{

    //get extra parameter

    //confirm save

    
    if(params.size() > 0)
    {
        if(params.size() == 1)
        {
            bool ans = editor::confirm("Save to File?");
            if(ans)
            {
                bool scc = patternedtr::saveSong(params.at(0));
                if(scc)
                {
                    int len = strlen(params.at(0));
                    editor::copy(params.at(0), lastSongPath, len);
                }
                else
                    editor::inform("Couldn't save file");
            }
        }
        else
            editor::inform("Too many parameters for Save");
    }
    else
    {
        bool ans = editor::confirm("Save?");
        patternedtr::saveSong(lastSongPath);
    }
}


void editor::handle_ins(std::vector<char*> &params)
{
    handle_entryins(params);
}


//insert a row into one track of a pattern
void editor::handle_entryins(std::vector<char*> &params)
{
    //insert at seltrk:selrow
    unsigned char trk = patternedtr::seltrack,
                  row = patternedtr::selrow;
    unsigned int  entry = R_EMPTY;
    if(params.size() > 0)
    {
        trk = parseUnsigned(params.at(0));
        if(params.size() > 1)
        {
            row = parseUnsigned(params.at(1));
        }
    }
    patternedtr::selptrn->insertRow(trk, row, entry);

}

//insert a row into all tracks of a pattern
void editor::handle_rowins(std::vector<char*> &params)
{
    //insert at selrow
    unsigned char row = patternedtr::selrow;
    unsigned int  entry = R_EMPTY;
    if(params.size() > 0)
    {
        row = parseUnsigned(params.at(0));
    }
    patternedtr::selptrn->insertRow(row, entry);

}


//delete a row in one track of a pattern
void editor::handle_entrydel(std::vector<char*> &params)
{
    //insert at seltrk:selrow
    unsigned char trk = patternedtr::seltrack,
                  row = patternedtr::selrow,
                  num = 1;
    if(params.size() > 0)
    {
        trk = parseUnsigned(params.at(0));
        if(params.size() > 1)
        {
            row = parseUnsigned(params.at(1));

            if(params.size() > 2)
            {
                num = parseUnsigned(params.at(2));
            }
        }
    }
    patternedtr::selptrn->deleteRow(trk, row, num);

}

//insert a row into all tracks of a pattern
void editor::handle_rowdel(std::vector<char*> &params)
{
    //insert at selrow
    unsigned char row = patternedtr::selrow,
                  num = 1;
    if(params.size() > 0)
    {
        row = parseUnsigned(params.at(0));
        if(params.size() > 1)
            num = parseUnsigned(params.at(0));
    }
    patternedtr::selptrn->deleteRow(row, num);
}



void editor::handle_wq(std::vector<char*> &params)
{
    //confirm save and quit
    //get extra parameter

    if(params.size() > 0)
    {
        if(params.size() == 1)
        {
            bool ans = editor::confirm("Save to File then Quit?");
            if(ans)
            {
                bool scc = patternedtr::saveSong(params.at(0));
                if(scc)
                {
                    patternedtr::exit();
                }
                else
                    editor::inform("Couldn't save file");
            }
        }
        else
            editor::inform("Too many parameters for Save");
    }
    else
    {
        bool ans = editor::confirm("Save and Quit?");

        if(ans)
        {
            bool scc = patternedtr::saveSong(lastSongPath);
            if(scc)
                patternedtr::exit();
            else
                editor::inform("Too many parameters for Save");
        }
    }
}

void editor::handle_o(std::vector<char*> &params)
{
        //get parameter
    if(params.size() > 0)
    {
        if(params.size() == 1)
        {
            bool ans = editor::confirm("Open from File?");
            if(ans)
            {
                bool scc = patternedtr::openSong(params.at(0));
                if(scc)
                {
                    int len = strlen(params.at(0));
                    editor::copy(params.at(0), lastSongPath, len);
                }
            }
        }
        else
            editor::inform("Too many parameters for Open");
    }
    else
    {
        bool ans = editor::confirm("Restore Song?");
        if(ans)
            patternedtr::openSong(lastSongPath);
    }
}

void editor::handle_d(std::vector<char*> &params)
{
    //get type by parameter
    //    no type: ...
    //            get region for pattern editor
    //          trk: clear track
    //          inst: instrument
    //          order: order 
    //          ptrn: delete pattern
    //         

    if(params.size() > 0)
    {
        char *p = params.at(0);
        if(strcmp(p, "track") == 0)
        {
            unsigned char track = patternedtr::seltrack;
            if(params.size() > 1)
            {
                track = parseUnsigned(params.at(1));
                //patternedtr::clearTrack(track);
            }
            for(int i = 0; i < editor::song->numPatterns(); i++)
                editor::song->getPattern(i)->removeTracks(track, track);
            editor::song->setTrackNum(editor::song->numTracks()-1);

            //Changed on the road
            if(patternedtr::seltrack >= editor::song->numTracks())
                patternedtr::seltrack = editor::song->numTracks()-1;
        }
        else if(strcmp(p, "inst") == 0)
            if(params.size() > 1)
            {
                unsigned char inst = parseUnsigned(params.at(1));
                if(inst < editor::song->numInstruments())
                {
                    patternedtr::removeInstrument(inst);
                }
                else
                    editor::inform("This Instrument does not exist.");
            }
            else
                patternedtr::removeInstrument(patternedtr::selinstrument);
        else if(strcmp(p, "order") == 0)
            if(params.size() > 1)
            {
                unsigned char ordr = parseUnsigned(params.at(1));
                if(ordr < editor::song->numOrders())
                {
                    patternedtr::removeOrder(ordr);
                }
                else editor::inform("This Order does not exist.");
            }
            else
                patternedtr::removeOrder(patternedtr::selorder);
        else if(strcmp(p, "ptrn") == 0)
        {
            if(params.size() > 1)
            {
                unsigned char ptrn = parseUnsigned(params.at(1));
                if(ptrn < editor::song->numPatterns())
                {
                    patternedtr::removePattern(ptrn);
                }
                else editor::inform("This Pattern does not exist.");

            }
            else
                patternedtr::removePattern(editor::song->getPatternIndexByOrder(patternedtr::selorder));
        }
        else if(strcmp(p, "row") == 0)
        {
            unsigned char len = 1;
            unsigned char row = patternedtr::selrow;
            if(params.size() > 1)
            {
                len = parseUnsigned(params.at(1));
                if(params.size() > 2)
                {
                    row = parseUnsigned(params.at(2));
                }
            }
            patternedtr::selptrn->deleteRow(row, len);
        }
        else if(strcmp(p, "mark") == 0)
        {
            if(playback_mark == 0xFF)
            {
                inform("Delete Mark (d mark) playback mark not set.");
                return;
            }
            unsigned char len;
            unsigned char row = patternedtr::selrow;
            if(row < editor::playback_mark)
            { 
                len = editor::playback_mark - row + 1;
            }
            else
            {
                len = row - editor::playback_mark + 1;
                row = editor::playback_mark;
            }

            unsigned char trkbegin = patternedtr::seltrack, trkend = trkbegin;
            if(params.size() > 1)
            {
                trkbegin = parseUnsigned(params.at(1));
                if(params.size() > 2)
                {
                    trkend = parseUnsigned(params.at(2));
                }
            }
            for(; trkbegin <= trkend; trkbegin++)
                patternedtr::selptrn->deleteRow(trkbegin, row, len);

        }
        else
            inform("Delete (d) parameter 1 not recognized.");

    }
    else
        patternedtr::selptrn->deleteRow(patternedtr::seltrack, patternedtr::selrow, 1);
}

void editor::handle_ampinst(std::vector<char *> &params)
{
    if(params.size() > 0)
    {
        float amp, out;
        unsigned short *vtbl;
        unsigned char entries;
        unsigned short outchar;

        amp = parseFloat(params.at(0));

        if(amp == 1)
            return;
        if(params.size() > 1)
        {
            unsigned char inst;
            for(int i = 1; i < params.size(); i++)
            {
                inst = parseUnsigned(params.at(i));
                vtbl = editor::song->getInstrument(inst)->getVolTable();
                entries = editor::song->getInstrument(inst)->numVolEntries();

                for(int j = 0; j < entries; j++)
                {
                    out = (vtbl[j] & 0xFF00) >> 8;
                    if(out < 0xFA)
                    {
                        out*=amp;
                        outchar = out;
                        vtbl[j] &= 0x00FF;
                        vtbl[j] |= (outchar << 8);
                    }
                }

            }

        }
        else
        {
            unsigned char inst = patternedtr::selinstrument;
            vtbl = editor::song->getInstrument(inst)->getVolTable();
            entries = editor::song->getInstrument(inst)->numVolEntries();

            for(int j = 0; j < entries; j++)
            {
                out = (vtbl[j] & 0xFF00) >> 8;
                if(out < 0xFA)
                {
                    out*=amp;
                    outchar = out;
                    vtbl[j] &= 0x00FF;
                    vtbl[j] |= (outchar << 8);
                }
            }

        }
    }
}



void editor::handle_ampinstall(std::vector<char *> &params)
{
    if(params.size() > 0)
    {
        float amp, out;
        unsigned short *vtbl;
        unsigned char entries;
        unsigned short outchar;

        amp = parseFloat(params.at(0));

        if(amp == 1)
            return;
        for(int i = 0; i < editor::song->numInstruments(); i++)
        {
            vtbl = editor::song->getInstrument(i)->getVolTable();
            entries = editor::song->getInstrument(i)->numVolEntries();

            for(int j = 0; j < entries; j++)
            {
                out = (vtbl[j] & 0xFF00) >> 8;
                if(out < 0xFA)
                {
                    out*=amp;
                    outchar = out;
                    vtbl[j] &= 0x00FF;
                    vtbl[j] |= (outchar << 8);
                }
            }

        }
    }
}

void editor::handle_instset(std::vector<char *> &params)
{
    if(params.size() > 0)
    {
        unsigned char inst;
        unsigned char begin=0, end=patternedtr::selptrn->numRows()-1;

        if(params.size() == 3)
        {
            inst = parseUnsigned(params.at(0));
            begin = parseUnsigned(params.at(1));
            end = parseUnsigned(params.at(2));

        }
        else if(params.size() == 2)
        {
            inst = patternedtr::selinstrument;
            begin = parseUnsigned(params.at(0));
            end = parseUnsigned(params.at(1));

        }
        else
        {
            inform("Instrument Set(instset): requires 2 to 3 params.");
            return;
        }

        if(inst >= editor::song->numInstruments())
        {
            inform("Instrument Set(instset): that instrument doesnt exist.");
            return;
        }

        if(begin >=patternedtr::selptrn->numRows())
        {
            inform("Instrument Set(instset): Beginning row too high.");
            return;
        }

        if(end >= patternedtr::selptrn->numRows())
            end = patternedtr::selptrn->numRows() - 1;

        unsigned int entry;
        unsigned int mask = int(inst) << RI_INSTRUMENT; 
        for(int i = begin; i <= end; i++)
        {
            entry = patternedtr::selptrn->at(patternedtr::seltrack, i);
            if((entry & R_INSTRUMENT) != R_INSTRUMENT)
            {
                entry &= ~R_INSTRUMENT;
                entry |= mask;
                patternedtr::selptrn->setAt(patternedtr::seltrack, i, entry);
            }
            
        }

    }
    else
        inform("Instrument Set(instset) requires 1 to 3 params");
}




void editor::handle_instsetmark(std::vector<char *> &params)
{

    if(editor::playback_mark >= patternedtr::selptrn->numRows())
    {
        inform("Instrument Set Mark(fxsetmark): No playback mark set");
        return;
    }
    unsigned int inst = 0;
    unsigned char begin, end;
    if(patternedtr::selrow > editor::playback_mark)
    {
        begin = editor::playback_mark;
        end = patternedtr::selrow;
    }
    else
    {
        begin = patternedtr::selrow;
        end = editor::playback_mark;
    }

    if(params.size() > 0)
        inst = (parseUnsigned(params.at(0)) & 0xFF) << RI_INSTRUMENT;


    if(begin >= patternedtr::selptrn->numRows())
        return;

    if(end >= patternedtr::selptrn->numRows())
        end = patternedtr::selptrn->numRows()-1;


    unsigned int entry;
    for(int i = begin; i <= end; i++)
    {
        entry = patternedtr::selptrn->at(patternedtr::seltrack, i);

        if((entry & R_INSTRUMENT) != R_INSTRUMENT)
        {
            entry &= ~R_INSTRUMENT;
            entry |= inst;
            patternedtr::selptrn->setAt(patternedtr::seltrack, i, entry);
        }
    }

}







void editor::handle_fxset(std::vector<char *> &params)
{

    if(params.size() > 1)
    {

        unsigned int effect = 0;
        unsigned char begin, end;
        if(params.size() == 3)
        {
            effect = parseUnsigned(params.at(0)) & R_EFFECTSEG;
            begin = parseUnsigned(params.at(1));
            end = parseUnsigned(params.at(2));

        }
        else if(params.size() == 2)
        {
            effect = 0;
            begin = parseUnsigned(params.at(0));
            end = parseUnsigned(params.at(1));
        }
        else
        {
            inform("Effect Set(fxset): Requires 2 or 3 parameters");
            return;
        }

        if(begin >= patternedtr::selptrn->numRows())
        {
            inform("Effect Set(fxset): Beginning row too high.");
            return;
        }

        if(end >= patternedtr::selptrn->numRows())
            end = patternedtr::selptrn->numRows()-1;


        unsigned int entry;
        for(int i = begin; i <= end; i++)
        {
            entry = patternedtr::selptrn->at(patternedtr::seltrack, i);
            entry &= R_EMPTY;
            entry |= effect;
            patternedtr::selptrn->setAt(patternedtr::seltrack, i, entry);
        }
    }
    else
        inform("Effect Set(fxset): Requires at least 2 parameters");

}


void editor::handle_fxsetmark(std::vector<char *> &params)
{

    if(editor::playback_mark >= patternedtr::selptrn->numRows())
    {
        inform("Effect Set Mark(fxsetmark): No playback mark set");
        return;
    }
    unsigned int effect = 0;
    unsigned char begin, end;
    if(patternedtr::selrow > editor::playback_mark)
    {
        begin = editor::playback_mark;
        end = patternedtr::selrow;
    }
    else
    {
        begin = patternedtr::selrow;
        end = editor::playback_mark;
    }

    if(params.size() > 0)
    {
        effect = parseUnsigned(params.at(0)) & R_EFFECTSEG;
    }


    if(begin >= patternedtr::selptrn->numRows())
        return;

    if(end >= patternedtr::selptrn->numRows())
        end = patternedtr::selptrn->numRows()-1;


    unsigned int entry;
    for(int i = begin; i <= end; i++)
    {
        entry = patternedtr::selptrn->at(patternedtr::seltrack, i);
        entry &= R_EMPTY;
        entry |= effect;
        patternedtr::selptrn->setAt(patternedtr::seltrack, i, entry);
    }

}


void editor::handle_amp(std::vector<char *> &params)
{
    if(params.size() > 0)
    {
        float amp;
        unsigned char begin=0, end=patternedtr::selptrn->numRows()-1;
        amp = parseFloat(params.at(0));
        if(params.size() > 1)
        {
            begin = parseUnsigned(params.at(1));
            if(params.size() > 2)
            {
                end = parseUnsigned(params.at(2));

                if(params.size() > 3)
                {
                    inform("Amplify Track(amp) requires 1 to 3 params");
                    return;
                }
            }
        }

        if(amp == 1)
            return;

        if(end >= patternedtr::selptrn->numRows())
            end = patternedtr::selptrn->numRows() - 1;

        unsigned int entry;
        unsigned char volume;
        for(int i = begin; i <= end; i++)
        {
            entry = patternedtr::selptrn->at(patternedtr::seltrack, i);
            if((entry & R_EMPTY) != R_EMPTY) //effectseg can be set without a volume needing to be defined
            {
                volume = (entry & R_VOLUME) >> RI_VOLUME;
                volume*= amp;
                if(volume > 0x3F)
                    volume = 0x3F;
                entry &= ~R_VOLUME;
                entry |= (volume << RI_VOLUME);
                patternedtr::selptrn->setAt(patternedtr::seltrack, i, entry);
            }
        }

    }
    else
        inform("Amplify Track(amp) requires 1 to 3 params");
}

void editor::handle_resizesong(std::vector<char *> &params)
{
    Pattern *bup = patternedtr::selptrn;
    for(int i = 0; i < editor::song->numPatterns(); i++)
    {
        patternedtr::selptrn = editor::song->getPattern(i);
        handle_resize(params);
    }
    patternedtr::selptrn = bup;
}

void editor::handle_resize(std::vector<char *> &params)
{
    if(params.size() > 0)
    {
        float scale;
        scale = parseFloat(params.at(0));
        if(scale <= 0)
        {
            inform("Please.");
            return;
        }
        if(scale == 1)
            return;

        unsigned char begin, end;
        unsigned int entry;
        if(scale < 1)
        {
            begin=(patternedtr::selptrn->numRows()-1)*scale;
            end=0;
            for(int trk = 0; trk < patternedtr::selptrn->numTracks(); trk++)
            {
                for(int i = begin; i >= end; i--)
                {
                    entry = patternedtr::selptrn->at(trk, i/scale);
                    patternedtr::selptrn->setAt(trk, i, entry);
                }
                for(int i = begin+1; i < patternedtr::selptrn->numRows(); i++)
                    patternedtr::selptrn->setAt(trk, i, R_EMPTY);
            }
            patternedtr::selptrn->setRowNum(patternedtr::selptrn->numRows()*scale);
        }
        else
        {
            patternedtr::selptrn->setRowNum(patternedtr::selptrn->numRows()*scale);
            begin=(patternedtr::selptrn->numRows()-1)/scale;
            end=0;
            for(int trk = 0; trk < patternedtr::selptrn->numTracks(); trk++)
            {
                for(int i = begin; i >= end; i--)
                {
                    entry = patternedtr::selptrn->at(trk, i);
                    patternedtr::selptrn->setAt(trk, i*scale, entry);
                    for(int j = 1; j < scale; j++)
                        patternedtr::selptrn->setAt(trk, i*scale+j, R_EMPTY);
                }
            }

        }

    }
    else
        inform("Resize Pattern(scale) requires 1 param");
}

void editor::handle_ampmark(std::vector<char *> &params)
{
    if(params.size() > 0)
    {
        float amp;
        unsigned char begin, end;
        unsigned char trkbegin, trkend;


        if(editor::playback_mark > patternedtr::selrow)
        {
            begin = patternedtr::selrow;
            end = editor::playback_mark;
        }
        else
        {
            begin = editor::playback_mark;
            end = patternedtr::selrow;
        }

        amp = parseFloat(params.at(0));


        if(amp == 1)
            return;

        if(params.size() > 1)
        {
            trkbegin = parseUnsigned(params.at(1));
            trkend = trkbegin;

            if(trkbegin >= editor::song->numTracks())
            {
                inform("Amplify Track(ampmark) beginning track too high");
                return;
            }

            if(params.size() > 2)
            {
                trkend = parseUnsigned(params.at(2));

                if(params.size() > 3)
                {
                    inform("Amplify Track(ampmark) requires 1 to 3 params");
                    return;
                }
            }
        }
        else
        {
            trkbegin = patternedtr::seltrack;
            trkend = trkbegin;
        }


        if(trkend >= editor::song->numTracks())
            trkend = editor::song->numTracks() - 1;

        unsigned int entry;
        unsigned char volume;
        for(int t = trkbegin; t <= trkend; t++)
            for(int i = begin; i <= end; i++)
            {
                entry = patternedtr::selptrn->at(t, i);
                if(entry != R_EMPTY)
                {
                    volume = (entry & R_VOLUME) >> RI_VOLUME;
                    volume*= amp;
                    if(volume > 0x3F)
                        volume = 0x3F;
                    entry &= ~R_VOLUME;
                    entry |= (volume << RI_VOLUME);
                    patternedtr::selptrn->setAt(t, i, entry);
                }
            }

    }
    else
        inform("Amplify Track(ampmark) requires 1 to 3 params");
}

void editor::handle_yy(std::vector<char *> &params)
{
    if(params.size() > 0)
    {
        float amp = 1;
        unsigned char begin=0, end=patternedtr::selptrn->numRows()-1;
        if(params.size() > 1)
        {
            begin = parseUnsigned(params.at(1));
            if(params.size() > 2)
            {
                end = parseUnsigned(params.at(2));
                if(params.size() > 3)
                {
                    amp = parseFloat(params.at(3));

                    if(params.size() > 4)
                    {
                        inform("Copy Track(yy) requires 1 to 4 params");
                        return;
                    }
                }
            }
        }


        unsigned char srctrk = parseUnsigned(params.at(0));
        if(srctrk < patternedtr::selptrn->numTracks())
        {
            int len = end-begin+1;
            if((patternedtr::selrow + len) > patternedtr::selptrn->numRows())
                len = patternedtr::selptrn->numRows() - patternedtr::selrow;
            //Copy from srctrk to editor::seltrack
            unsigned int entry;
            unsigned char volume;
            for(int i = 0; i < len; i++)
            {
                entry = patternedtr::selptrn->at(srctrk, begin+i);
                if(amp != 1 && entry != R_EMPTY)
                {
                    volume = (entry & R_VOLUME) >> RI_VOLUME;
                    volume*= amp;
                    if(volume > 0x3F)
                        volume = 0x3F;
                    entry &= ~R_VOLUME;
                    entry |= (volume << RI_VOLUME);
                }
                patternedtr::selptrn->setAt(patternedtr::seltrack, patternedtr::selrow+i, entry);
            }
        }

    }
    else
        inform("Copy Track(yy) requires 1 to 4 params");
}

void editor::handle_y(std::vector<char*> &params)
{
    //get region by parameters
    //if no region, copy cell selected
    if(params.size() > 0)
    {
        char *p = params.at(0);
        if(strcmp(p, "track") == 0)
        {
            params.erase(params.begin());
            handle_yy(params);
        }
        else if(strcmp(p, "inst") == 0)
        {
            if(params.size() > 1)
            {
                unsigned short entry = parseUnsigned(params.at(1));
                if(entry > song->numInstruments())
                    inform("Copy(y): Instrument invalid, cant copy");
                else
                {
                    patternedtr::selinstrument = entry;
                    selinst = song->getInstrument(entry);
                }

            }
            else
                patternedtr::cloneInstrument(patternedtr::selinstrument);
        }
        else if(strcmp(p, "order") == 0)
            patternedtr::cloneOrder(patternedtr::selorder);
        else if(strcmp(p, "ptrn") == 0)
            patternedtr::clonePattern(editor::song->getPatternIndexByOrder(patternedtr::selorder));
        else if(strcmp(p, "optrn") == 0)
        {
            patternedtr::cloneOrder(patternedtr::selorder);
            patternedtr::clonePattern(editor::song->getPatternIndexByOrder(patternedtr::selorder));
        }
        else 
            inform("Copy(y) Param not understood");

    }


}
#include <iostream>

//Transpose
void editor::handle_trans(std::vector<char*> &params)
{
    //transpose x semitones
    //parameters :trans semitone trkstart trkend start end
    if(params.size() > 0)
    {

        unsigned char trkbegin=0, trkend=patternedtr::selptrn->numTracks()-1;
        unsigned char begin=0, end=patternedtr::selptrn->numRows()-1;
        if(params.size() > 1)
        {
            trkbegin = parseUnsigned(params.at(1));
            trkend = trkbegin;
            if(params.size() > 2)
            {
                trkend = parseUnsigned(params.at(2));
                if(params.size() > 3)
                {
                    begin = parseUnsigned(params.at(3));

                    if(params.size() > 4)
                    {
                        end = parseUnsigned(params.at(4));

                        if(params.size() > 5)
                        {
                            inform("Tanspose(trans) requires 1 to 5 params");
                            return;
                        }
                    }
                }
            }
        }

        if(trkbegin >= patternedtr::selptrn->numTracks())
        {
            inform("Transpose(trans) param 2(trkstart) >= tracks");
            return;
        }

        if(trkend >= patternedtr::selptrn->numTracks())
        {
            trkend = patternedtr::selptrn->numTracks()-1;
        }

        if(begin >=  patternedtr::selptrn->numRows())
        {
            inform("Transpose(trans) param 4(rowstart) >= rows");
            return;
        }

        if(end >=  patternedtr::selptrn->numRows())
        {
            end = patternedtr::selptrn->numRows()-1;
        }


        char semitones = parseSigned(params.at(0));
        if(semitones == 0)
            return;

        int len = end-begin+1;
        int trklen = trkend-trkbegin+1;
        
        unsigned int entry;
        unsigned int note;
        unsigned int oct;
        for(int j = 0; j < trklen; j++)
        {
            for(int i = 0; i < len; i++)
            {
                entry = patternedtr::selptrn->at(trkbegin+j, begin+i);
                if((entry & R_PITCHSEG) != R_PITCHSEG)
                {
                    note = (entry & R_PITCHSEG);
                    std::cerr << "semitones " << (int)semitones << '\n';;
                    if(semitones > 0)
                        note = patternedtr::addNotes(note, semitones*0x02000000);
                    else 
                        note = patternedtr::subNotes(note, (-semitones)*0x02000000);


                    entry &= ~R_PITCHSEG;
                    entry |= note;
                    patternedtr::selptrn->setAt(trkbegin+j, begin+i, entry);
                }
            }
        }

    }
    else
        inform("Transpose(trans) requires 1 to 5 params");

}


void editor::handle_transkeysong(std::vector<char*> &params)
{
    if(params.size() > 0)
    {

        unsigned char key = parseKeySignature(params.at(0));
        if (key > 11)//-1
        {
            inform("Transpose To Key(transkeysong): key signature doesn't exist");
            return;
        }

        Pattern *selection = patternedtr::selptrn;

        unsigned char numpats = editor::song->numPatterns();
        for(int i = 0; i < numpats; i++)
        {
            patternedtr::selptrn = editor::song->getPattern(i);
            handle_transkey(params);
        }

        patternedtr::selptrn = selection;
    }
    else
        inform("Tanspose Song To Key(transkeysong) requires 1 to 7 params");
}



void editor::handle_transsong(std::vector<char*> &params)
{
    if(params.size() > 0)
    {

        Pattern *selection = patternedtr::selptrn;

        unsigned char numpats = editor::song->numPatterns();
        for(int i = 0; i < numpats; i++)
        {
            patternedtr::selptrn = editor::song->getPattern(i);
            handle_trans(params);
        }

        patternedtr::selptrn = selection;
    }
    else
        inform("Tanspose Song(transsong) requires 1 to 6 params");
}


//Transpose to a key
void editor::handle_transkey(std::vector<char*> &params)
{


    //transpose x semitones
    //parameters :trans semitone trkstart trkend start end
    if(params.size() > 0)
    {

        unsigned char scalebuffer[12]; //in case the user specifies a scale
        unsigned char spinner = patternedtr::scalespinner;

        unsigned char *scale = patternedtr::scaleconst;
        unsigned char key = parseKeySignature(params.at(0));
        if (key > 11)
        {
            inform("Transpose To Key(transkey): key signature doesn't exist");
            return;
        }
        unsigned char trkbegin=0, trkend=patternedtr::selptrn->numTracks()-1;
        unsigned char begin=0, end=patternedtr::selptrn->numRows()-1;
        unsigned char semitones = 0;
        if(params.size() > 1)
        {
            bool scc = parseScale(params.at(1), spinner, scalebuffer);
            scale = scalebuffer;
            if(!scc)
            {
                inform("Transpose To Key(transkey): scale not recognized");
                return;
            }

            if(params.size() > 2)
            {
                semitones = parseSigned(params.at(2));
                if(params.size() > 3)
                {
                    trkbegin = parseUnsigned(params.at(3));
                    trkend = trkbegin;
                    if(params.size() > 4)
                    {
                        trkend = parseUnsigned(params.at(4));
                        if(params.size() > 5)
                        {
                            begin = parseUnsigned(params.at(5));

                            if(params.size() > 6)
                            {
                                end = parseUnsigned(params.at(6));

                                if(params.size() > 7)
                                {
                                    inform("Tanspose To Key(trans) requires 1 to 7 params");
                                    return;
                                }
                            }
                        }
                    }
                }
            }
        }

        if(trkbegin >= patternedtr::selptrn->numTracks())
        {
            inform("Transpose To Key(transkey) param 4(trkstart) >= tracks");
            return;
        }

        if(trkend >= patternedtr::selptrn->numTracks())
        {
            trkend = patternedtr::selptrn->numTracks() - 1;
        }

        if(begin >=  patternedtr::selptrn->numRows())
        {
            inform("Transpose To Key(transkey) param 6(rowstart) >= rows");
            return;
        }

        if(end >=  patternedtr::selptrn->numRows())
        {
            end = patternedtr::selptrn->numRows()-1;
        }



        int len = end-begin+1;
        int trklen = trkend-trkbegin+1;
        
        unsigned int entry;
        unsigned int note;
        unsigned int oct;
        for(int j = 0; j < trklen; j++)
        {
            for(int i = 0; i < len; i++)
            {
                entry = patternedtr::selptrn->at(trkbegin+j, begin+i);
                if((entry & R_PITCHSEG) != R_PITCHSEG)
                {
                    note = (entry & R_PITCHSEG);
                    if(semitones != 0)
                    {
                        if(semitones > 0)
                            note = patternedtr::addNotes(note, semitones*0x02000000);
                        else 
                            note = patternedtr::subNotes(note, (-semitones)*0x02000000);
                    }

                    note = patternedtr::toKey(note, scale, key);

                    entry &= ~R_PITCHSEG;
                    entry |= note;
                    patternedtr::selptrn->setAt(trkbegin+j, begin+i, entry);
                }
            }
        }

    }
    else
        inform("Transpose To Key(trans) requires 1 to 6 params");

}



void editor::handle_n(std::vector<char*> &params)
{
    //get type by parameter...
    //   no param: new Song
    //          song: new Song
    //          inst: new Instrument
    //             i: new Instrument
    //   
    if(params.size() > 0)
    {
        char *p = params.at(0);
        if(strcmp(p, "song") == 0)
        {
            patternedtr::newSong();
        }
        else if(strcmp(p, "inst") == 0)
            patternedtr::newInstrument();
        else if(strcmp(p, "order") == 0)
            patternedtr::newOrder();
        else if(strcmp(p, "ptrn") == 0)
            patternedtr::newPattern();
        else 
            inform("(n)Param not understood");

    }
    else
    {
        bool scc = confirm("New song? All unsaved will be lost");
        if(scc)
            patternedtr::newSong();
    }


}

void  editor::handle_wave(std::vector<char*> &params)
{
    if(params.size() == 1)
    {
        unsigned short entry = parseUnsigned(params.at(0));
        editor::selinst->setWaveIndex(entry);
    }
    else
        inform("Too many params for Set Wave Index");
}

void editor::handle_inst(std::vector<char*> &params)
{
    if(params.size() == 1)
    {
        unsigned char entry = parseUnsigned(params.at(0));
        if(entry >= song->numInstruments())
            entry = song->numInstruments()-1;
        patternedtr::selinstrument = entry;
        editor::selinst = song->getInstrument(entry);
        instedtr::selwavrow = editor::selinst->getWaveIndex();
        instedtr::chgSelWavRow(0);
    }
    else
        inform("Too many params for Set Selected Instrument");
}

void editor::handle_order(std::vector<char*> &params)
{
    if(params.size() == 1)
    {
        unsigned char entry = parseUnsigned(params.at(0));
        if(entry >= song->numOrders())
            entry = song->numOrders()-1;
        patternedtr::selorder = entry;
        patternedtr::selptrn = song->getPatternByOrder(entry);
    }
    else
        inform("Too many params for Set Selected Order");
}

void editor::handle_ptrn(std::vector<char*> &params)
{
    if(params.size() == 1)
    {
        unsigned char entry = parseUnsigned(params.at(0));
        if(entry >= song->numPatterns())
            entry = song->numPatterns()-1;
        song->setPatternIndexByOrder(patternedtr::selorder, entry);
        patternedtr::selptrn = song->getPattern(entry);
    }
    else
        inform("Too many params for Set Order's Pattern");
}

void editor::handle_row(std::vector<char*> &params)
{
    if(params.size() == 1)
    {
        unsigned char entry = parseUnsigned(params.at(0));
        if(entry >= patternedtr::selptrn->numRows())
            entry = patternedtr::selptrn->numRows()-1;
        patternedtr::selrow = entry;
    }
    else
        inform("Too many params for Set Row");
}

void editor::handle_track(std::vector<char*> &params)
{
    if(params.size() == 1)
    {
        unsigned char entry = parseUnsigned(params.at(0));
        if(entry >= patternedtr::selptrn->numTracks())
            entry = patternedtr::selptrn->numTracks()-1;
        patternedtr::seltrack = entry;
    }
    else
        inform("Too many params for Set Track");
}

void editor::handle_step(std::vector<char*> &params)
{
    if(params.size() == 1)
    {
        unsigned char entry = parseUnsigned(params.at(0));
        patternedtr::edit_step = entry;
    }
    else
        inform("Too many params for Set Edit Step");
}

void editor::handle_oct(std::vector<char*> &params)
{
    if(params.size() == 1)
    {
        unsigned char entry = parseUnsigned(params.at(0));
        if(entry >= 6)
            entry = 6;
        patternedtr::octave = entry;
    }
    else
        inform("Too many params for Set Octave");
}

void editor::handle_mark(std::vector<char*> &params)
{
    if(params.size() == 1)
    {
        editor::playback_mark = parseUnsigned(params.at(0));
    }
    else
    {
        if(editor::playback_mark == 0xFF)
            editor::playback_mark = patternedtr::selrow;
        else
            editor::playback_mark = 0xFF;
    }
}

void editor::handle_playlen(std::vector<char*> &params)
{
    if(params.size() == 1)
    {
        editor::playback_length = parseUnsigned(params.at(0));
    }
    else
    {
        editor::playback_length = 8;
    }
}


void editor::handle_stop(std::vector<char *> &params)
{
    editor::stopPlayback();
}
void editor::handle_render(std::vector<char *> &params)
{
    editor::renderSong();
}

void editor::handle_mute(std::vector<char*> &params)
{
    if(params.size() > 0)
    {
        unsigned int *arr = new unsigned int[params.size()];
        for(int i = 0; i < params.size(); i++)
        {
            arr[i] = parseUnsigned(params.at(i));
            if(arr[i] >= editor::song->numTracks())
            {
                inform("Mute (mute) track doesn't exist");
                return;
            }
        }
        patternedtr::setMuteTracks(arr, params.size());
    }
    else
    {

        patternedtr::unmuteTracks();
    }
    editor::refreshMutedTracks();
}
void editor::handle_unmute(std::vector<char*> &params)
{
    if(params.size() > 0)
    {
        unsigned int *arr = new unsigned int[params.size()];
        for(int i = 0; i < params.size(); i++)
        {
            arr[i] = parseUnsigned(params.at(i));
            if(arr[i] >= editor::song->numTracks())
            {
                inform("Unmute (unmute) track doesn't exist");
                return;
            }
        }
        patternedtr::setUnmuteTracks(arr, params.size());
    }
    else
    {
        patternedtr::unmuteTracks();
    }
    editor::refreshMutedTracks();
}

void editor::handle_play(std::vector<char*> &params)
{
    unsigned char orderstart, orderend, rowstart, rowend;
    if(params.size() == 0)
    {
        editor::playSong();
    }
    else
    {
        unsigned char numrowsstart, numrowsend;
        //orderstart, orderend, rowstart, rowend
        orderstart = parseUnsigned(params.at(0));
        if(orderstart >= editor::song->numOrders())
        {
            inform("Invalid starting order");
            return;
        }
        numrowsstart = editor::song->getPatternByOrder(orderstart)->numRows();

        //verify order
        orderend = orderstart;
        rowstart = 0;
        rowend = numrowsstart-1;
        if(params.size() > 1)
        {
            orderend = parseUnsigned(params.at(1));
            if(orderend >= editor::song->numOrders())
            {
                orderend = editor::song->numOrders()-1;
            }
            numrowsend = editor::song->getPatternByOrder(orderend)->numRows();
            rowend = numrowsend-1;
            if(params.size() > 2)
            {
                rowstart = parseUnsigned(params.at(2));
                if(rowstart >= numrowsstart)
                    rowstart = numrowsstart-1;

                if(params.size() > 3)
                {
                    rowend = parseUnsigned(params.at(3));
                    if(rowend >= numrowsend)
                        rowend = numrowsend-1;
                }
            }
        }

        editor::playExcerptExplicit(orderstart, orderend, rowstart, rowend);

    }

}
void editor::handle_e(std::vector<char*> &params)
{
    bool ans = editor::confirm("Restore File?");
    if(ans)
    {
        unsigned char selorder = patternedtr::selorder;
        patternedtr::openSong(lastSongPath);
        if(selorder < editor::song->numOrders())
        {
            patternedtr::selorder = selorder;
            patternedtr::selptrn = song->getPatternByOrder(selorder);
        }

    }
}

void editor::handle_dd(std::vector<char*> &params)
{
    patternedtr::clearTrack(patternedtr::seltrack); 
}

void editor::handle_ddd(std::vector<char*> &params)
{
    patternedtr::clearPattern(editor::song->getPatternIndexByOrder(patternedtr::selorder));
}

void editor::handle_key(std::vector<char*> &params)
{
    using patternedtr::scalespinner;
    using patternedtr::scaleconst;
    if(params.size() > 0)
    {
        patternedtr::key = parseKeySignature(params.at(0));
        if(params.size() > 1)
        {
            unsigned char scalebfr[12];
            bool scc = parseScale(params.at(1), scalespinner, scalebfr);
            if(!scc)
            {
                inform("Key (key): param 2(scale) not recognized");
                return;
            }
            for(int i = 0; i < 12; i++)
                scaleconst[i] = scalebfr[i];
            scalespinner = patternedtr::inferScaleType(scaleconst);
        }
    }
    else
        patternedtr::key = 0;
    patternedtr::populateNoteMap();

}


void editor::handle_scale(std::vector<char*> &params)
{
    using patternedtr::scalespinner;
    using patternedtr::scaleconst;
    if(params.size() > 0)
    {
            unsigned char scalebfr[12];
            bool scc = parseScale(params.at(0), scalespinner, scalebfr);
            if(!scc)
            {
                inform("Set Scale (scale): param 1(scale) not recognized");
                return;
            }
            for(int i = 0; i < 12; i++)
                scaleconst[i] = scalebfr[i];
            scalespinner = patternedtr::inferScaleType(scaleconst);
    }
    else
        inform("Set Scale (scale): Requires a parameter");

    patternedtr::populateNoteMap();

}

void editor::handle_amplin(std::vector<char*> &params)
{

}

void editor::handle_pid(std::vector<char*> &params)
{
    if(params.size() > 0)
    {
        editor::daemonpid = parseSigned(params.at(0));
    }
    else editor::inform("No PID provided.");

}


void editor::handle_playamp(std::vector<char *> &params)
{
    if(params.size() > 0)
    {
        float amp, out;
        amp = parseFloat(params.at(0));
        if(amp > 2.5)
        {
            bool imsure = editor::confirm("Are you sure you really want to amplify that much?");
            if(imsure)
            {
                editor::playamp = amp;
                setPlaybackAmplitude();
            }
        }
        else
        {
                editor::playamp = amp;
                setPlaybackAmplitude();
        }

    }
}

void editor::handle_fxchange(std::vector<char *> &params)
{
    //Change one effect to another in whole song

    if(params.size() > 1)
    {

        unsigned int effectsrc = 0;
        unsigned int effectdest = 0;
        
        effectsrc = parseUnsigned(params.at(0))*0x100;
        effectdest = parseUnsigned(params.at(1))*0x100;
        

        
        unsigned int entry;
        unsigned int effect;
        Pattern *selection = patternedtr::selptrn;

        unsigned char numpats = editor::song->numPatterns();
        for(int i = 0; i < numpats; i++)
        {
            patternedtr::selptrn = editor::song->getPattern(i);
            for(int trk = 0; trk < patternedtr::selptrn->numTracks(); trk++)
                for(int row = 0; row < patternedtr::selptrn->numRows(); row++)
                {
                    entry = patternedtr::selptrn->at(trk, row);
                    effect = entry & 0xF00;
                    if(effect == effectsrc)
                    {
                        entry &= ~R_EFFECT;
                        entry |= effectdest;
                        patternedtr::selptrn->setAt(trk, row, entry);
                    }
                }
        }
        patternedtr::selptrn = selection;
        
    }
    else
        inform("Effect Change(fxchange): Requires 2 parameters");

}


