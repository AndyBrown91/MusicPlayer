//
//  TrackMulti.h
//  MusicPlayer
//
//  Created by Andy on 03/12/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef TRACKMULTI 
#define TRACKMULTI

#include "../JuceLibraryCode/JuceHeader.h"
#include "Settings.h"
#include "MusicLibraryHelpers.h"
#include "TagReader.h"
#include "AlbumArt.h"

/** Track dialog displayed if multiple tracks have been selected when the Display Info option is selected */
class TrackMulti  :	public Component
{
public:
    /** Constructor - Passes an array of the unique ID numbers of the selected tracks*/
    TrackMulti (Array<int> incomingIds);
    /** Destructor */
    ~TrackMulti();
	
    /** @internal */
    void resized();
    /** @internal */
	void paint(Graphics& g);

    /** Displays the information that matches, leaves non matching fields blank */
    void setInfo();
    /** Saves all information to the metadata tags for all files selected */
    void saveEdits();
    
private:
    Label artistLabel;
    TextEditor artist;
    Label albumLabel;
    TextEditor album;
    Label genreLabel;
    TextEditor genre;
    Label ratingLabel;
    Slider rating;
    Label labelLabel;
    TextEditor label;
    
    Label artLabel;
    AlbumArt art;
    
    Array<int> libraryIds;
    
    bool saveRequired, settingInfo;

    Image newArt;
};


#endif
