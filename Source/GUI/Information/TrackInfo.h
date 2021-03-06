/*
 *  TrackInfo.h
 *  sdaMidiMeter
 *
 *  Created by Andy on 06/01/2012.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef TRACKINFO
#define TRACKINFO

#include "../JuceLibraryCode/JuceHeader.h"
#include "MusicLibraryHelpers.h"
#include "Settings.h"
/** Displays the song title, album name and artist name of the currently playing song */
class TrackInfo  : public Component
{
public:
    /** Constructor */
    TrackInfo ();
    /** Destructor */
    ~TrackInfo();
    /** @internal */
    void resized();
    /** Loads the song title, album name and artist name of the currently playing song */
	void loadTrackInfo(ValueTree tags);

private:
    Label artist;
    Label title;
    Label album;

};


#endif   //H_TRACKINFO
