#include "tables.h"
#include "daemoncomm.h"
#include "cmdbar.h"
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

void getKeySig(unsigned int notes_in_key[7], const unsigned char &key)
{
    using patternedtr::addNotes;

    if(key < 12)
    {
        unsigned int base = key*0x02000000;
        notes_in_key[0]= addNotes(base, 0x00000000) & R_NOTE;
        notes_in_key[1]= addNotes(base, 0x04000000) & R_NOTE;
        notes_in_key[2]= addNotes(base, 0x08000000) & R_NOTE;
        notes_in_key[3]= addNotes(base, 0x0A000000) & R_NOTE;
        notes_in_key[4]= addNotes(base, 0x0E000000) & R_NOTE;
        notes_in_key[5]= addNotes(base, 0x12000000) & R_NOTE;
        notes_in_key[6]= addNotes(base, 0x16000000) & R_NOTE;
    }
    else if(key < 24)
    {
        unsigned int base = (key-12)*0x02000000;
        notes_in_key[0]= addNotes(base, 0x00000000) & R_NOTE;
        notes_in_key[1]= addNotes(base, 0x04000000) & R_NOTE;
        notes_in_key[2]= addNotes(base, 0x06000000) & R_NOTE;
        notes_in_key[3]= addNotes(base, 0x0A000000) & R_NOTE;
        notes_in_key[4]= addNotes(base, 0x0E000000) & R_NOTE;
        notes_in_key[5]= addNotes(base, 0x10000000) & R_NOTE;
        notes_in_key[6]= addNotes(base, 0x14000000) & R_NOTE;
    }
}

void testKey()
{
    using std::cout;
    unsigned int notes_in_key[7];
    char noteStr[4];
    char keyStr[4];
    noteStr[3] = 0;
    keyStr[3] = 0;

    for(int i = 0; i < 24; i++)
    {
        patternedtr::getKeyChar(keyStr,i);
        getKeySig(notes_in_key, i);
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

    unsigned int basenote;
    unsigned int out;
    char noteStr[4];
    noteStr[3]=0;
    char baseStr[4];
    baseStr[3]=0;
    char keyStr[4];
    keyStr[3]=0;
    cout << "KEY|BASE|toKEY\n";

    for(int j = 0; j < 24; j++)
    {
        patternedtr::getKeyChar(keyStr,j);
        basenote = 0;
        for(int i = 0; i < 12; i++)
        {
            editor::noteString(baseStr,basenote);
            out = toKey(basenote, j);
            editor::noteString(noteStr,out);
            cout << keyStr << ':' << baseStr << ':' << noteStr << '\n';
        
            basenote += 0x02000000;
        }
    }




}


int main()
{
    testToKey();
    
    return 0;
}
