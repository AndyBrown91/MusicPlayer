//
//  TrackArt.h
//  MusicPlayer
//
//  Created by Andy on 09/12/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef TRACKART
#define TRACKART

#include "../JuceLibraryCode/JuceHeader.h"
#include "TagReader.h"
#include "AlbumArt.h"
#include "Settings.h"

class TrackArt  :	public Component,
                    public ButtonListener,
public Slider::Listener
{
public:
    TrackArt ();
    ~TrackArt();
	
    void paint (Graphics& g);
    void resized();
    
    void setTrack (ValueTree incomingTrack);

    void buttonClicked (Button* buttonThatWasClicked);
    void sliderValueChanged (Slider* sliderThatWasMoved);
    
    void saveArt();
    
private:
    Viewport view;
    AlbumArt albumArt;
    Label sizeLabel;
    Slider size;
    TextButton file;
    TextButton url;

    File selectedFile;
    
    ValueTree selectedTrack;
    
    ImageWithType cover;
};



#endif
