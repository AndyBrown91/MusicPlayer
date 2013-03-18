/*
 *  InfoBar.h
 *  MusicPlayer
 *
 *  Created by Andy on 26/10/2012.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef INFOBAR
#define INFOBAR

#include "../JuceLibraryCode/JuceHeader.h"
#include "Settings.h"

class InfoBar  : public Component,
                 public ValueTree::Listener
{
public:
    InfoBar ();
    ~InfoBar();

    void paint (Graphics& g);
    void resized();
    
    void updateBar(); 
    
    void valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property){}
    void valueTreeChildAdded (ValueTree &parentTree, ValueTree &childWhichHasBeenAdded){}
    void valueTreeChildRemoved (ValueTree &parentTree, ValueTree &childWhichHasBeenRemoved){}
    void valueTreeChildOrderChanged (ValueTree &parentTreeWhoseChildrenHaveMoved);
    void valueTreeParentChanged (ValueTree &treeWhoseParentHasChanged){}
    void valueTreeRedirected (ValueTree &treeWhichHasBeenChanged){}
    
private:
    String size, time, numTracks;
};
#endif
