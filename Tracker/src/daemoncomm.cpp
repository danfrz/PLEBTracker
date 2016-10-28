#include "daemoncomm.h"

char *editor::signalpath;
char *editor::playbackpath;



void editor::playExcerptImplicit()
{
    using namespace patternedtr;
    
    if(editor::playback_length == 0)
        return;

    //std::cerr << "Creating Excerpt\n";
    if(playback_mark >= selptrn->numRows())
    {
        playback = song->makeExcerpt(playback_length, selorder, selrow);
    }
    else if(selrow < playback_mark)
    {
        playback = song->makeExcerpt(playback_length+(playback_mark-selrow), selorder, selrow);
    }
    else
    {
        playback = song->makeExcerpt(playback_length+(selrow-playback_mark), selorder, playback_mark);
    }
    playback->setName("Playback Excerpt", 16);


    std::ofstream playfile(playbackpath);
    if(playfile.is_open())
    {
        //std::cerr << "Openned Playback ofstream\n";
        playback->output(playfile);
        //std::cerr << "Outputted to playback\n";
        playfile.close();

        //std::cerr << "Openned Signal ofstream\n";
        std::ofstream signal(signalpath, std::ofstream::out | std::ofstream::app);
        if(signal.is_open())
        {
            signal << "p" << std::endl;
            signal.close();

            //std::cerr << "Done playing\n";
        }
        //else std::cerr << "Couldn't write to Signal\n";
    }

    delete playback;
    playback = NULL;
    //std::cerr << "Deleted\n";

}
void editor::playExcerptExplicit(unsigned char orderstart, unsigned char orderend, unsigned char rowstart, unsigned char rowend)
{
    using namespace patternedtr;

    playback = song->makeExcerpt(orderstart, orderend, rowstart, rowend);
    playback->setName("Playback Excerpt", 16);

    std::ofstream playfile(playbackpath);
    if(playfile.is_open())
    {
        //std::cerr << "Openned Playback ofstream\n";
        playback->output(playfile);
        //std::cerr << "Outputted to playback\n";
        playfile.close();

        //std::cerr << "Openned Signal ofstream\n";
        std::ofstream signal(signalpath, std::ofstream::out | std::ofstream::app);
        if(signal.is_open())
        {
            signal << "p" << std::endl;
            signal.close();

            //std::cerr << "Done playing\n";
        }
        //else std::cerr << "Couldn't write to Signal\n";
    }

    delete playback;
    playback = NULL;
    //std::cerr << "Deleted\n";

}


void editor::playSong()
{
    using namespace patternedtr;

    std::ofstream playfile(playbackpath);
    //std::cerr << "playbackpath " << editor::playbackpath << " signal: " << editor::signalpath << '\n';
    if(playfile.is_open())
    {
        //std::cerr << "Openned Playback ofstream\n";
        song->output(playfile);
        //std::cerr << "Outputted to playback\n";
        playfile.close();

        //std::cerr << "Openned Signal ofstream\n";
        std::ofstream signal(signalpath, std::ofstream::out | std::ofstream::app);
        if(signal.is_open())
        {
            signal << "p" << std::endl;
            signal.close();

            //std::cerr << "Done playing\n";
        }
        //else std::cerr << "Couldn't write to Signal\n";
    }
}

void editor::stopPlayback()
{
    std::ofstream signal(signalpath, std::ofstream::out | std::ofstream::app);
    if(signal.is_open())
    {
        signal << "x" << std::endl;
        signal.close();
    }
}

void editor::renderSong()
{
    std::ofstream playfile(playbackpath);
    //std::cerr << "playbackpath " << editor::playbackpath << " signal: " << editor::signalpath << '\n';
    if(playfile.is_open())
    {
        //std::cerr << "Openned Playback ofstream\n";
        song->output(playfile);
        //std::cerr << "Outputted to playback\n";
        playfile.close();


        std::ofstream signal(signalpath, std::ofstream::out | std::ofstream::app);
        if(signal.is_open())
        {
            signal << "r" << std::endl;
            signal.close();
        }
    }
}

//Refreshes the muted tracks with the plebtrkdaemon
void editor::refreshMutedTracks()
{
    bool anymuted = false;
    for(int i = 0; !anymuted && i < editor::song->numTracks(); i++)
        anymuted = editor::muted_tracks[i];

    

    std::ofstream signal(signalpath, std::ofstream::out | std::ofstream::app);
    if(signal.is_open())
    {
        if(!anymuted)
        {
            signal << "unmute ";
        }
        else
        {
            signal << "mute ";
            for(int i = 0; i < editor::song->numTracks(); i++)
            {
                if(editor::muted_tracks[i])
                    signal << i << ',';
            }
        }

        signal << "\n";
        signal.close();
    }
}

void editor::setPlaybackAmplitude()
{
    std::ofstream signal(signalpath, std::ofstream::out | std::ofstream::app);
    if(signal.is_open())
    {
        signal << "amp " << editor::playamp << '\n';
        signal.close();
    }
}


