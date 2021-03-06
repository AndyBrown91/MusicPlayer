/*
 *  InfoBar.cpp
 *  MusicPlayer
 *
 *  Created by Andy on 26/10/2012.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "InfoBar.h"

InfoBar::InfoBar ()
{
    filteredDataList.addListener(this);
    
    addAndMakeVisible(&indicator);
    
    addAndMakeVisible(&lastFm);
    currentLastFm.setLastFmButton(&lastFm);
    
    addAndMakeVisible(&infoLabel);
    infoLabel.setJustificationType(Justification::centredRight);
    infoLabel.addMouseListener(this, false);
    
}

InfoBar::~InfoBar()
{
}

//==============================================================================
void InfoBar::paint (Graphics& g)
{
    g.setGradientFill (ColourGradient (Colour (0xffe7e9f9),
                                       0.0f, 0.0f,
                                       Colour (0xffeaeaea),
                                       0.0f, 24.0f,
                                       false));
    g.fillRect (0, 0, getWidth(), getHeight());

    g.setColour (Colour (0xff686868));
    g.drawRect (0, 0, getWidth(), getHeight(), 1);

    g.setColour (Colours::black);
    g.drawText (numTracks,
                (getWidth()/2)-250, 0, 150, getHeight(),
                Justification::centredRight, true);
    g.drawText (time,
                (getWidth()/2)-50, 0, 100, getHeight(),
                Justification::centred, true);
    g.drawText (size,
                (getWidth()/2)+100, 0, 100, getHeight(),
                Justification::centredLeft, true);
    
    g.drawText("Remote: ", 5, 0, 47, getHeight(), Justification::centredLeft, false);

}

void InfoBar::resized()
{
    indicator.setBounds(57, getHeight()/4, 10, getHeight()-5);
    lastFm.setBounds(85, 2, 115, getHeight()-3);
    infoLabel.setBounds (getWidth() - 400, 0, 400, getHeight());
}

void InfoBar::updateBar()
{
    int64 sizeInt = 0;
    int64 timeInt = 0;
    
    if (tableSelectedTracks.size() > 1)
    {
        for (int i = 0 ; i < tableSelectedTracks.size(); i++) {
            ValueTree currentTrack = filteredDataList.getChildWithProperty(MusicColumns::columnNames[MusicColumns::ID], tableSelectedTracks[i]);
            
            sizeInt += int(currentTrack.getProperty(MusicColumns::columnNames[MusicColumns::Size]));
            timeInt += int(currentTrack.getProperty(MusicColumns::columnNames[MusicColumns::Length]));
        }   
        
        numTracks = String(tableSelectedTracks.size())+ " of " + String(filteredDataList.getNumChildren())+ " Songs";
    }
    else
    {
        for (int i = 0 ; i < filteredDataList.getNumChildren(); i++) {
            ValueTree currentTrack = filteredDataList.getChild(i);
        
            sizeInt += int(currentTrack.getProperty(MusicColumns::columnNames[MusicColumns::Size]));
            timeInt += int(currentTrack.getProperty(MusicColumns::columnNames[MusicColumns::Length]));
        }
        
        numTracks = String(filteredDataList.getNumChildren())+ " Songs";
    }
    
    size = File::descriptionOfSizeInBytes(sizeInt);
    
    time = RelativeTime::milliseconds(timeInt).getDescription();
    
    repaint();
}

void InfoBar::valueTreeChildOrderChanged (ValueTree &parentTreeWhoseChildrenHaveMoved)
{
    //Everytime the table changes it re-orders/re-creates the filtered data list, including sorting it so this is the most called function
    
    updateBar();
}

void InfoBar::displayFileStatus (File& file, int result)
{
    currentFile = file;
    
    if (result == 1)
    {
        String notFoundString = currentFile.getFileName();
        notFoundString << " Could not be found";
        infoLabel.setText(notFoundString, dontSendNotification);
    }
    else if (result == 2)
    {
        ValueTree fileTree = singletonLibraryTree.getChildWithProperty(MusicColumns::columnNames[MusicColumns::Location], currentFile.getFullPathName());
     
        String notReadString = fileTree.getProperty("Song").toString();
        notReadString << " Could not be read";
        
        infoLabel.setText(notReadString, dontSendNotification);
    }
    else
        infoLabel.setText(String::empty, dontSendNotification);
}


void InfoBar::mouseDoubleClick(const MouseEvent &e)
{
    if (e.getMouseDownScreenX() >= infoLabel.getScreenPosition().getX())
    {
        //Bottom right hand corner clicked
        if (infoLabel.getText().contains("Could not be found"))
        {
            ScopedPointer<AlertWindow> fileFailed;
            String longString = currentFile.getFileName();
            longString << " could not be found, the listing for it could be incorrect\n Would you like to try finding this file?";
            
            fileFailed = new AlertWindow(infoLabel.getText(), longString, AlertWindow::WarningIcon);
            fileFailed->addButton("Find", 1);
            fileFailed->addButton("Cancel", 0);
            
            
            if (fileFailed->runModalLoop() != 0)
            {
                ValueTree currentTrack = singletonLibraryTree.getChildWithProperty(MusicColumns::columnNames[MusicColumns::Location], currentFile.getFullPathName());
                
                if (currentTrack.isValid())
                {
                    String findString = "Please find ";
                    findString << currentFile.getFileName();
                    
                    String wildcard = "*";
                    wildcard << currentFile.getFileExtension();
                    
                    FileChooser fc (findString,currentFile.getFullPathName(),wildcard,true);
                    
                    if (fc.browseForFileToOpen())
                    {
                        singletonLibraryTree.removeChild(currentTrack, 0/*add undoManager possibly*/);
                        singletonLibraryTree.addChild(TagReader::addToLibrary(fc.getResult()),-1,0);
                    }
                    
                    //tableUpdateRequired = true;
                }
            }
            
        }
    }
}