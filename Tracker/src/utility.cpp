#include "tables.h"
#include "daemoncomm.h"



bool editor::validateHexChar(char a)
{
    if(a >= 'a')
        a -= 32;
    return(a <= '9' && a >= '0' || a >= 'A' && a <= 'F');
}

bool editor::validate64(char str[2])
{
    //values < 64 have to start with 0 to 3
    //invalid ascii characters are filterred
    return str[0] < '4' && str[0] >= '0' && validateHexChar(str[1]);
} 
unsigned char editor::charHex(char c)
{
    if(c<='9' && c>='0')
        return c-'0';
    if(c >= 'a')
        c -= 32;
    if(c>='A' && c <= 'F')
        return c-55;
    return -1;
}

char *editor::byteString(char *string, unsigned char byte)
{
    unsigned char d = byte % 0x10;
    string[1] = hexnums[d];
    byte /=0x10;
    d = byte % 0x10;
    string[0] = hexnums[d];
    return string;
}
char *editor::intString(char *string, unsigned int integer, const unsigned int &strlen)
{
    unsigned char d;
    for(int i = 0; i < strlen; i++)
    {
        d = integer & 0xF;
        string[strlen-i-1]= hexnums[d];
        integer >>= 4;
    }
    return string;
}

char *editor::shortString(char *string,  unsigned short shrt, const unsigned int &strlen)
{
    unsigned char d;
    for(int i = 0; i < strlen; i++)
    {
        d = shrt & 0xF;
        string[strlen-i-1]= hexnums[d];
        shrt >>= 4;
    }
    return string;
}

unsigned char editor::parseHexChar(char str[2])
{
    //values < 64 have to start with 0 to 3
    //invalid ascii characters are filterred
    unsigned char out = charHex(str[0])*0x10;
    out + charHex(str[1]);
    return out;
}


void editor::inform(const char *message)
{
    int length = strlen(message);
    int msglen = length;
    if(length < 64)
        length = 64;
    attroff(-1);
    attron(COLOR_PAIR(patternedtr::COL_META_SSU));
    mvprintw(3,4,"+--------------------------------------------------------------+",stdscr);

    mvprintw(8,4,"+--------------------------------------------------------------+",stdscr);
    attroff(-1);
    attron(COLOR_PAIR(patternedtr::COL_META_US));
    mvprintw(4,4,"                                                                ",stdscr);
    mvprintw(5,4,message,stdscr);
    if(msglen < 64)
        for(int i = msglen; i < 64; i++)
            mvprintw(5, 4+i, " ", stdscr);
    mvprintw(6,4,"                                                                ",stdscr);
    mvprintw(7,4," Press [Space] to continue                                      ",stdscr);
    editor::lastwin = editor::dialog;
    int ch;
    while((ch=getch()) != ' ' && ch != '\n');
    return;
}
bool editor::confirm(const char *message)
{
    int length = strlen(message);
    int msglen = length;
    if(length < 64)
        length = 64;
    attroff(-1);
    attron(COLOR_PAIR(patternedtr::COL_META_SSU));
    //modify these to output to the length
    mvprintw(3,4,"+--------------------------------------------------------------+",stdscr);
    mvprintw(8,4,"+--------------------------------------------------------------+",stdscr);
    attroff(-1);
    attron(COLOR_PAIR(patternedtr::COL_META_US));
    mvprintw(4,4,"                                                                ",stdscr);
    mvprintw(5,4,message,stdscr);
    if(msglen < 64)
        for(int i = msglen; i < 64; i++)
            mvprintw(5, 4+i, " ", stdscr);
    mvprintw(6,4,"                                                                ",stdscr);
    mvprintw(7,4," Are you sure? [y/n]                                            ",stdscr);
    editor::lastwin = editor::dialog;

    int ch;
    while((ch=getch()) != 'y' && ch != 'n' && ch != 27);
    if(ch == 'y')
        return true;
    return false;
}
void editor::displayAbout()
{
    attroff(-1);
    attron(COLOR_PAIR(patternedtr::COL_META_SSU));
    mvprintw(3,4, "+---------------------------------+",stdscr);
    mvprintw(15,4,"+---------------------------------+",stdscr);
    attroff(-1);
    attron(COLOR_PAIR(patternedtr::COL_META_US));
    mvprintw(4,4, "  Welcome to PLEBTracker!       v1 ",stdscr);
    mvprintw(5,4, "                                   ",stdscr);
    mvprintw(6,4, "Press TAB to switch windows        ",stdscr);
    mvprintw(7,4, "Press :   to open the command bar  ",stdscr);
    mvprintw(8,4, "                                   ",stdscr);
    mvprintw(9,4, "To view a list of commands, see    ",stdscr);
    mvprintw(10,4,"   the man page for plebtrkraw(1)  ",stdscr);
    mvprintw(11,4,"                                   ",stdscr);
    mvprintw(12,4,"                      -Dan Frazier ",stdscr);
    mvprintw(13,4,"                                   ",stdscr);
    mvprintw(14,4," Press [Space] to continue         ",stdscr);
    editor::lastwin = editor::dialog;
    int ch;
    while((ch=getch()) != ' ' && ch != '\n');
    return;
}


const char *editor::locateFile(bool save)
{
    return NULL;
    /*
    struct dirent *de=NULL;
    DIR *dir;

    int selection = 0;
    int viewport = 0;
    std::vector<dirent*> files;

    dir = opendir(".");
    if(dir == NULL)
    {
        std::cerr << "Failed to open current directory\n";
        return NULL;
    }
    while(de = readdir(dir))
        files.push_back(de);

    mvprintw(3,4, "+----------------------------------------------------------+",stdscr);
    if(save)
        mvprintw(4,4, "                         SAVE SONG                          ",stdscr);
    else
        mvprintw(4,4, "                         OPEN SONG                          ",stdscr);
    mvprintw(5,4, "                                                            ",stdscr);
    mvprintw(6,4, "                                                            ",stdscr);
    mvprintw(7,4, "                                                            ",stdscr);
    mvprintw(8,4, "                                                            ",stdscr);
    mvprintw(9,4, "                                                            ",stdscr);
    mvprintw(10,4,"                                                            ",stdscr);
    mvprintw(11,4,"                                                            ",stdscr);
    mvprintw(12,4,"                                                            ",stdscr);
    mvprintw(13,4,"                                                            ",stdscr);
    mvprintw(14,4,"                                                            ",stdscr);
    mvprintw(15,4,"+----------------------------------------------------------+",stdscr);



    closedir(dir);
*/
    editor::lastwin = editor::dialog;
}

void editor::setPID(int PID)
{
    //Prepare the paths that the daemon will use
    std::string playpath("/var/tmp/plebtrk/");
    if(PID)
        playpath += std::to_string(PID);
    std::string signalpath(playpath + "signal");
    playpath+= "playback.plb";

    editor::playbackpath = new char[playpath.length()];
    for(int i = 0; i < playpath.length(); i++)
        editor::playbackpath[i]=playpath.at(i);

    editor::signalpath = new char[signalpath.length()];
    for(int i = 0; i < signalpath.length(); i++)
        editor::signalpath[i]=signalpath.at(i);
}
