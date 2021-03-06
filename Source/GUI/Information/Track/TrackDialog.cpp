//
//  TrackDialog.cpp
//  MusicPlayer
//
//  Created by Andy on 06/12/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "TrackDialog.h"

TrackDialog::TrackDialog(int incomingTrack)
{    
    selectedTrack = incomingTrack;
    
    addAndMakeVisible(&ok);
    ok.setButtonText("Ok");
    ok.addListener(this);
    
    addAndMakeVisible(&next);
    next.setButtonText("Next");
    next.addListener(this);
    
    addAndMakeVisible(&previous);
    previous.setButtonText("Previous");
    previous.addListener(this);
    
    addAndMakeVisible(&trackTabbed);
    trackTabbed.setTrack(filteredDataList.getChild(selectedTrack));
    setSize(550, 570);
}

TrackDialog::~TrackDialog()
{
    
}

void TrackDialog::paint (Graphics& g)
{

}

void TrackDialog::resized()
{
    trackTabbed.setBounds (10, 2, 530, 510);
    previous.setBounds (10, 520, 100, 24);
    next.setBounds (434, 520, 100, 24);
    ok.setBounds (218, 520, 100, 24);
}

void TrackDialog::setTrack()
{
    trackTabbed.saveEdits();
    
    ValueTree currentlyEditing = filteredDataList.getChild(selectedTrack);
    File test = currentlyEditing.getProperty(MusicColumns::columnNames[MusicColumns::Location]).toString();
    
    if (test.exists())
    {
        trackTabbed.setTrack(filteredDataList.getChild(selectedTrack));
        DialogWindow* dw = findParentComponentOfClass<DialogWindow>();
        dw->setName(filteredDataList.getChild(selectedTrack).getProperty(MusicColumns::columnNames[MusicColumns::Song]));
    }
}

void TrackDialog::buttonClicked(Button *buttonClicked)
{   
    if (buttonClicked == &next)
    {
        if (selectedTrack >= 0 && selectedTrack < filteredDataList.getNumChildren()-1)
        {
            selectedTrack++;
            setTrack();
        }
    }
    
    if (buttonClicked == &previous)
    {
        if (selectedTrack > 0 && selectedTrack <= filteredDataList.getNumChildren()-1)
        {
            selectedTrack--;
            setTrack();
        }
    }
    
    if (buttonClicked == &ok)
    {
        trackTabbed.saveEdits();
        DialogWindow* dw = findParentComponentOfClass<DialogWindow>();
        dw->closeButtonPressed();
    }
}
