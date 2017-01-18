#include "pattern.h"

#include <iostream>

//Copy pattern data from src to dest
void copyRegion(unsigned int **src, unsigned int **dest,
        const unsigned char &tracks, const unsigned char &rows)
{
    for(int i = 0; i < tracks; i++)
        for(int j = 0; j < rows; j++)
            dest[i][j] = src[i][j];
}

//min defined on unsigned chars
const unsigned char &min(const unsigned char &a, const unsigned char &b)
{
    return (a<b)?a:b;
}

//absolute value of char
char abs(char a)
{
    return (a<0)?-a:a;
}


Pattern::Pattern()
{
    tracks = 4;
    rows = 64;
    data = new unsigned int*[tracks];
    for(int i = 0; i < tracks; i++)
    {
        data[i] = new unsigned int[rows];
        for(int j = 0; j < rows; j++)
            data[i][j] = R_EMPTY;
    }
}


Pattern::Pattern(std::istream &in)
{
    data = NULL;
    input(in);
}


Pattern::Pattern(unsigned int track, unsigned int length)
{
    tracks = track;
    rows = length;

    data = new unsigned int*[tracks];
    for(int i = 0; i < tracks; i++)
    {
        data[i] = new unsigned int[rows];
        for(int j = 0; j < rows; j++)
            data[i][j] = R_EMPTY;
    }

}

Pattern::Pattern(const Pattern &other)
{
    tracks = other.tracks;
    rows = other.rows;

    data = new unsigned int*[tracks];
    for(int i = 0; i < tracks; i++)
        data[i] = new unsigned int[rows];

    copyRegion(other.data, data, tracks, rows);
}

Pattern::~Pattern()
{
    for(int i = 0; i < tracks; i++)
        delete [] data[i];
    delete [] data;
}


std::ostream &Pattern::output(std::ostream &out) const
{
    out.write((char*)&tracks, sizeof(char));
    out.write((char*)&rows,   sizeof(char));

    for(int i = 0; i < tracks; i++)
        out.write((char*)(data[i]), sizeof(int)*rows);

    return out;
}

std::istream &Pattern::input(std::istream &in)
{
    if(data)
    {
        for(int i = 0; i < tracks; i++)
            delete [] data[i];
        delete [] data;
    }

    in.read((char*)&tracks, sizeof(char));
    in.read((char*)&rows,   sizeof(char));
    data = new unsigned int *[tracks];
    for(int i = 0; i < tracks; i++)
        data[i] = new unsigned int[rows];
    //in.read((char*)data, sizeof(int)*rows*tracks); //DOESNT WORK

    for(int i = 0; i < tracks; i++)
        in.read((char*)(data[i]), sizeof(int)*rows);

    return in;
}


void Pattern::setSize(const unsigned char &newtracks, const unsigned char &newrows)
{
    int i, j;
    unsigned int**newdata = new unsigned int*[newtracks];
    for(i = 0; i < tracks; i++)
        newdata[i] = new unsigned int[newrows];

    //Find the amount of rows and tracks to copy over to the new pattern data
    const unsigned char &mintracks = min(newtracks, tracks);
    const unsigned char &minrows = min(newrows, rows);

    //Copy relevant old data
    copyRegion(data, newdata, mintracks, minrows);

    //Initialize information in extra tracks to EMPTY
    if(newtracks > tracks)
        for(i = tracks; i < newtracks; i++)
            for(j = 0; j < minrows; j++)
                newdata[i][j] = R_EMPTY;

    //Initialize information in extra rows to EMPTY
    if(newrows > rows)
        for(i = 0; i < newtracks; i++)
            for(j = minrows; j < newrows; j++)
                newdata[i][j] = R_EMPTY;

    //Delete old pattern
    for(i = 0; i < tracks; i++)
        delete [] data[i];
    delete [] data;

    //Set new information
    data = newdata;
    tracks = newtracks;
    rows   = newrows;

}

void Pattern::setRowNum(const unsigned char &newrows)
{

    //NOTE it may not be necessary to create a new array
    //when decreasing row size, just keep unused data, less CPU

    int i, j;
    unsigned int**newdata = new unsigned int*[tracks];
    for(i = 0; i < tracks; i++)
        newdata[i] = new unsigned int[newrows];

    const unsigned char &minrows = min(newrows, rows);

    //Copy relevant old data
    copyRegion(data, newdata, tracks, minrows);

    //Initialize information in extra rows to 0
    if(newrows > rows)
        for(i = 0; i < tracks; i++)
            for(j = minrows; j < newrows; j++)
                newdata[i][j] = R_EMPTY;

    //Delete old pattern
    for(i = 0; i < tracks; i++)
        delete [] data[i];
    delete [] data;

    //Set new information
    data = newdata;
    rows = newrows;
}

void Pattern::setTrackNum(const unsigned char &newtracks)
{
    int i, j;
    unsigned int**newdata = new unsigned int*[newtracks];
    for(i = 0; i < newtracks; i++)
        newdata[i] = new unsigned int[rows];

    const unsigned char &mintracks = min(newtracks, tracks);

    std::cerr << int(mintracks) << '\n';

    //Copy relevant old data
    copyRegion(data, newdata, mintracks, rows);

    //Initialize information in extra tracks to 0
    if(newtracks > tracks)
        for(i = tracks; i < newtracks; i++)
            for(j = 0; j < rows; j++)
                newdata[i][j] = R_EMPTY;

    //Delete old pattern
    for(i = 0; i < tracks; i++)
        delete [] data[i];
    delete [] data;

    //Set new information
    data = newdata;
    tracks = newtracks;

}

void Pattern::setAt(const unsigned char &track, const unsigned char &row, const unsigned int &cell)
{
    data[track][row] = cell;
}

void Pattern::setTrack(const unsigned char &which, unsigned int *track)
{
    for(int i = 0; i < rows; i++)
        data[which][rows] = track[i];
}

void Pattern::setRow(const unsigned char &which, unsigned int *row)
{
    for(int i = 0; i < tracks; i++)
        data[i][which] = row[i];
}


void Pattern::chop(unsigned char start, unsigned char end)
{
    for(int trk = 0; trk < tracks; trk++)
        for(int row = start, i = 0; row <= end; row++, i++)
            data[trk][i] = data[trk][row];

    rows = end-start+1;
}


void Pattern::clearAt(const unsigned char &track, const unsigned char &row)
{
    data[track][row] = R_EMPTY;
}

void Pattern::clearTrack(const unsigned char &which)
{
    for(int i = 0; i < rows; i++)
        data[which][i] = R_EMPTY;
}

void Pattern::clearRow(const unsigned char &which)
{
    for(int i = 0; i < tracks; i++)
        data[i][which] = R_EMPTY;
}

void Pattern::clear()
{
    for(int i = 0; i < tracks; i++)
        for(int j = 0; j < rows; j++)
            data[i][j] = R_EMPTY;

}

void Pattern::insertRow(const unsigned char &track, const unsigned char &row, const unsigned int &entry)
{
    for(int i = rows-1; i > row; i--)
        data[track][i] = data[track][i-1];
    data[track][row] = entry;
}

void Pattern::insertRow(const unsigned char &row, const unsigned int *entry)
{
    for(int i = 0; i < tracks; i++)
    {
        for(int j = rows-1; j > row; j++)
            data[i][j] = data[i][j-1];
        data[i][row] = entry[i];
    }

}

void Pattern::copyTrack(const unsigned char &srctrk, const unsigned char &desttrk)
{
    for(int j = 0; j < rows; j++)
        data[desttrk][j] = data[srctrk][j];
}


void Pattern::removeTracks(const unsigned char &srctrk, const unsigned char &endtrk)
{
    int dif = endtrk - srctrk + 1;
    for(int i = srctrk; i < tracks-dif; i++)
    {
        for(int j = 0; j < rows; j++)
        {
            data[i][j] = data[i+dif][j];
        }
    }

}


void Pattern::insertRow(const unsigned char &row, const unsigned int &entry)
{

    for(int i = 0; i < tracks; i++)
    {
        for(int j = rows-1; j > row; j--)
            data[i][j] = data[i][j-1];
        data[i][row] = entry;
    }
}

void Pattern::deleteRow(const unsigned char &track, const unsigned char &row, unsigned char len)
{

    int tmp = row;
    tmp += len;
    if(tmp > rows)
        len = rows-row;

    for(int j = row+len, k = row; j < rows; j++, k++)
        data[track][k] = data[track][j];
    for(int j = rows-len; j < rows; j++)
        data[track][j] = R_EMPTY;
}

void Pattern::deleteRow(const unsigned char &row, unsigned char len)
{
    unsigned int tmp = row;
    tmp += len;
    if(tmp > rows)
        len = rows-row;

    for(int i = 0; i < tracks; i++)
    {
        for(int j = row+len, k = row; j < rows; j++, k++)
            data[i][k] = data[i][j];
        for(int j = rows-len; j < rows; j++)
            data[i][j] = R_EMPTY;
    }

}

void Pattern::purgeInstrument(const unsigned char &inst)
{
    //Loop through all entries in the pattern and set the instruments
    //to FF

}

unsigned int Pattern::at(const unsigned char &track, const unsigned char &row)
{
    return data[track][row];
}

unsigned int *Pattern::trackAt(const unsigned char &track)
{
    return data[track];
}


