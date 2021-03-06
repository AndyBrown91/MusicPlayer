//
//  TrackEdit.cpp
//  MusicPlayer
//
//  Created by Andy on 03/12/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "TrackEdit.h"

TrackEdit::TrackEdit()
{    
    saveRequired = false;
    addAndMakeVisible (&songLabel);
    songLabel.setText("Song: ", dontSendNotification);
    
    addAndMakeVisible (&song);
    song.setScrollbarsShown (false);
    song.setPopupMenuEnabled (true);
    
    addAndMakeVisible (&artistLabel);
    artistLabel.setText("Artist: ", dontSendNotification);
    
    addAndMakeVisible (&artist);
    artist.setScrollbarsShown (false);
    artist.setPopupMenuEnabled (true);
    
    addAndMakeVisible (&albumLabel);
    albumLabel.setText("Album: ", dontSendNotification);
    
    addAndMakeVisible (&album);
    album.setScrollbarsShown (false);
    album.setPopupMenuEnabled (true);
    
    addAndMakeVisible (&genreLabel);
    genreLabel.setText("Genre: ", dontSendNotification);
    
    addAndMakeVisible (&genre);
    genre.setScrollbarsShown (false);
    genre.setPopupMenuEnabled (true);
    
    addAndMakeVisible (&ratingLabel);
    ratingLabel.setText("Rating: ", dontSendNotification);
    
    addAndMakeVisible (&rating);
    rating.setRange (0, 5, 1);
    rating.setSliderStyle (Slider::IncDecButtons);
    rating.setTextBoxStyle (Slider::TextBoxLeft, false, 30, 20);
    
    addAndMakeVisible (&labelLabel);
    labelLabel.setText("Label: ", dontSendNotification);
    
    addAndMakeVisible (&label);
    label.setMultiLine (true);
    label.setReturnKeyStartsNewLine (true);
    label.setReadOnly (false);
    label.setScrollbarsShown (true);
    label.setCaretVisible (true);
    label.setPopupMenuEnabled (true);
    
    addAndMakeVisible (&trackLabel);
    trackLabel.setText("Track Number: ", dontSendNotification);
    
    addAndMakeVisible (&trackNum);
    trackNum.setInputRestrictions(4, "0123456789");
    trackNum.setScrollbarsShown (false);
    trackNum.setPopupMenuEnabled (false);    
    
    setSize (530, 510);
    
}

TrackEdit::~TrackEdit()
{
    
}

//==============================================================================
void TrackEdit::paint (Graphics& g)
{

}

void TrackEdit::resized()
{
    songLabel.setBounds (0, 0, 48, 24);
    song.setBounds (16, 24, 296, 24);
    artistLabel.setBounds (0, 48, 64, 24);
    artist.setBounds (16, 72, 296, 24);
    albumLabel.setBounds (0, 96, 64, 24);
    album.setBounds (16, 120, 296, 24);
    genreLabel.setBounds (0, 144, 64, 24);
    genre.setBounds (16, 168, 296, 24);
    ratingLabel.setBounds (88, 200, 64, 24);
    rating.setBounds (128, 224, 96, 24);
    labelLabel.setBounds (0, 248, 48, 24);
    label.setBounds (16, 272, 440, 120);
    trackLabel.setBounds (360, 56, 112, 24);
    trackNum.setBounds (384, 80, 56, 24);
}


void TrackEdit::setTrack(ValueTree incomingTrack)
{
    File selectedFile (incomingTrack.getProperty(MusicColumns::columnNames[MusicColumns::Location]));
    songTree = incomingTrack;
    
    artist.setText (songTree.getProperty(MusicColumns::columnNames[MusicColumns::Artist]).toString(), false);
    
    album.setText (songTree.getProperty(MusicColumns::columnNames[MusicColumns::Album]).toString(), false);
    
    song.setText (songTree.getProperty(MusicColumns::columnNames[MusicColumns::Song]).toString(), false);
    
    genre.setText (songTree.getProperty(MusicColumns::columnNames[MusicColumns::Genre]).toString(), false);
    
    label.setText (songTree.getProperty(MusicColumns::columnNames[MusicColumns::Label]).toString(), false);
    
    rating.setValue(songTree.getProperty(MusicColumns::columnNames[MusicColumns::Rating]));
    
    trackNum.setText(songTree.getProperty(MusicColumns::columnNames[MusicColumns::TrackNum]));
    
    saveRequired = false;
    
    song.addListener(this);
    artist.addListener(this);
    album.addListener(this);
    genre.addListener(this);
    rating.addListener(this);
    label.addListener(this);
    trackNum.addListener(this);
}

void TrackEdit::saveEdits ()
{
    if (saveRequired)
    {
        File selectedFile (songTree.getProperty(MusicColumns::columnNames[MusicColumns::Location]));
        
        songTree.setProperty(MusicColumns::columnNames[MusicColumns::Song], song.getText(), 0);
        TagReader::writeTag(MusicColumns::Song, songTree);
        
        songTree.setProperty(MusicColumns::columnNames[MusicColumns::Artist], artist.getText(), 0);
        TagReader::writeTag(MusicColumns::Artist, songTree);
        
        songTree.setProperty(MusicColumns::columnNames[MusicColumns::Album], album.getText(), 0);
        TagReader::writeTag(MusicColumns::Album, songTree);
        
        songTree.setProperty(MusicColumns::columnNames[MusicColumns::Genre], genre.getText(), 0);
        TagReader::writeTag(MusicColumns::Genre, songTree);
        
        songTree.setProperty(MusicColumns::columnNames[MusicColumns::Label], label.getText(), 0);
        TagReader::writeTag(MusicColumns::Label, songTree);
        if (rating.getValue() > 0)
        {
            songTree.setProperty(MusicColumns::columnNames[MusicColumns::Rating], rating.getValue(), 0);
            TagReader::writeTag(MusicColumns::Rating, songTree);
        }
        else
        {
            songTree.setProperty(MusicColumns::columnNames[MusicColumns::Rating], var::null, 0);
            TagReader::writeTag(MusicColumns::Rating, songTree);
        }
        
        songTree.setProperty(MusicColumns::columnNames[MusicColumns::Modified], Time::getCurrentTime().toMilliseconds(), 0);
        selectedFile.setLastModificationTime(Time::getCurrentTime());
        
        songTree.setProperty(MusicColumns::columnNames[MusicColumns::TrackNum], trackNum.getText().getIntValue(), 0);
        TagReader::writeTag(MusicColumns::TrackNum, songTree);
        
        saveRequired = false;
    }
    song.removeListener(this);
    artist.removeListener(this);
    album.removeListener(this);
    genre.removeListener(this);
    rating.removeListener(this);
    label.removeListener(this);
    trackNum.removeListener(this);
}

//Text editor callbacks
void TrackEdit::textEditorTextChanged (TextEditor &textEditor)
{
    saveRequired = true;
}

void TrackEdit::sliderValueChanged (Slider* sliderThatWasMoved)
{
	saveRequired = true;
}
