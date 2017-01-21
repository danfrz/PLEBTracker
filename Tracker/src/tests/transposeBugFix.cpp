#include "../../include/tables.h"
#include "../../include/daemoncomm.h"
#include "../../include/cmdbar.h"
#include <string>
#include <iostream>

void testParsedSigned()
{
    using std::string;
    char * cstr = new char[32];
    std::cin.getline(cstr, 32, '\n');
    std::cout << "string= " << cstr << '\n';

    int signedval = editor::parseSigned(cstr);
    std::cout << signedval << '\n';
}

void testNoteArithmetic()
{
    using namespace patternedtr;
    using std::cout;
    //should output all notes
    const unsigned int onenote = 0x02000000;
    unsigned int basenote = 0;
    char noteStr[4];
    noteStr[3]=0;
    
    cout << "ADDITION\n";
    for(int i = 0; i < 12*6; i++)
    {
        editor::noteString(noteStr,basenote);
        cout << noteStr << "\n";
        basenote = patternedtr::addNotes(basenote, onenote);
    }
    editor::noteString(noteStr,basenote);
    cout << noteStr << "\n";



    cout << "\nSUBTRACTION\n";
    for(int i = 0; i < 12*6; i++)
    {
        editor::noteString(noteStr,basenote);
        cout << noteStr << "\n";
        basenote = patternedtr::subNotes(basenote, onenote);
    }
    editor::noteString(noteStr,basenote);
    cout << noteStr << "\n";

}

void getKeySig(unsigned int notes_in_key[12], const unsigned char *scale, const unsigned char &key)
{
    using patternedtr::addNotes;

    unsigned int last;

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

}

void testKey()
{
    using std::cout;
    unsigned int notes_in_key[12];

    unsigned char scale[12];
    unsigned char spinner = 0;
    //Need a char (not const) for parseScale
    char a[24];
    const char *scalename = "major";
    for(int i = 0; i < strlen(scalename); i++)
        a[i] = scalename[i];
    a[strlen(scalename)] = 0;
    editor::parseScale(a, spinner, scale);

    char noteStr[4];
    char keyStr[4];
    noteStr[3] = 0;
    keyStr[3] = 0;

    for(int i = 0; i < 12; i++)
    {
        patternedtr::getKeyChar(keyStr,i);
        getKeySig(notes_in_key, scale, i);
        for(int j = 0; j < 7; j++)
        {
            editor::noteString(noteStr,notes_in_key[j]);
            cout << keyStr << ' ' << noteStr << '\n';
        }
    }
}


void testToKey()
{
    using namespace patternedtr;
    using std::cout;

    editor::initializeScaleMap();

    unsigned char scale[12];
    unsigned char spinner = 0;
    //Need a char (not const) for parseScale
    char a[24];
    const char *scalename = "maj";
    for(int i = 0; i < strlen(scalename); i++)
        a[i] = scalename[i];
    a[strlen(scalename)] = 0;
    bool scc = editor::parseScale(a, spinner, scale);

    if(!scc)
    {
        cout << "Failed to parse scale using " << a << '\n';
        return;
    }
    for(int i = 0; i < 12; i++)
        cout << (char)(scale[i] + '0');
    cout << '\n';

    unsigned int basenote;
    unsigned int out;
    char noteStr[4];
    noteStr[3]=0;
    char baseStr[4];
    baseStr[3]=0;
    char keyStr[4];
    keyStr[3]=0;
    cout << "KEY|BAE|toKEY\n";

    for(int j = 0; j < 12; j++)
    {
        patternedtr::getKeyChar(keyStr,j);
        basenote = 0;
        for(int i = 0; i < 12; i++)
        {
            editor::noteString(baseStr,basenote);
            out = toKey(basenote, scale, j);
            editor::noteString(noteStr,out);
            cout << keyStr << " :" << baseStr << ':' << noteStr << '\n';
        
            basenote += 0x02000000;
        }
    }




}


int main()
{
    testToKey();
    
    return 0;
}
