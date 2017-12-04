#ifndef TRACK_H_
#define TRACK_H_
#include "generator.h"
namespace itrp
{
    struct Track
    {
        paramtable *ptbl;
        float lastfrq;
        float frq;
        float nextfrq;
        float phase;

        unsigned short segments;
        unsigned char fx;
        unsigned char fxparam;
        Instrument *inst;

        unsigned short wavei;
        unsigned short pulsei;
        unsigned short filteri;
        unsigned char lastwave;
        unsigned char waveduracc;
        unsigned char pulseduracc;
        unsigned char filterduracc;
        unsigned char filters_active;

        sample_res *temp_bfr;

        //VOLUME
        unsigned char ptrnvol;
        unsigned char ptrnlastvol;
        unsigned char voljump;

        sample_res_unsigned lastvol;
        unsigned char voli;
        unsigned char volduracc;

    };
}
#endif
