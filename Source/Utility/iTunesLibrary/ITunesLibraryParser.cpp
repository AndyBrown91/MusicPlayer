/*
  ==============================================================================
  
  This file is part of the dRowAudio JUCE module
  Copyright 2004-12 by dRowAudio.
  
  ------------------------------------------------------------------------------
 
  dRowAudio can be redistributed and/or modified under the terms of the GNU General
  Public License (Version 2), as published by the Free Software Foundation.
  A copy of the license is included in the module distribution, or can be found
  online at www.gnu.org/licenses.
  
  dRowAudio is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
  
  ==============================================================================
*/

#include "ITunesLibraryParser.h"
#include "MusicLibraryHelpers.h"

ITunesLibraryParser::ITunesLibraryParser (const File& iTunesLibraryFileToUse, 
                                          const ValueTree& elementToFill,
                                          const ValueTree& playslistsToFill,
                                          const CriticalSection& lockToUse)
: Thread ("iTunesLibraryParser"),
lock (lockToUse),
iTunesLibraryFile (iTunesLibraryFileToUse),
treeToFill (elementToFill), playlistsTree(playslistsToFill), 
numAdded (0),
finished (false)
{
	startThread (1);
}

ITunesLibraryParser::~ITunesLibraryParser()
{
	stopThread (5000);
}

void ITunesLibraryParser::run()
{
    const MessageManagerLock mmLock;
    // parse the iTunes xml first
    iTunesDatabase = XmlDocument::parse (iTunesLibraryFile);
    if (! iTunesDatabase->hasTagName ("plist")
        || iTunesDatabase->getStringAttribute ("version") != "1.0")
    {
        jassertfalse; // not a vlid iTunesLibrary file!
        finished = true;
        return;
    }
	
    // find start of tracks library
    iTunesLibraryTracks = iTunesDatabase->getFirstChildElement()->getChildByName ("dict");
    currentElement = iTunesLibraryTracks->getFirstChildElement();
    
    // find any existing elements
    Array<int> existingIds;
    Array<ValueTree> existingItems;
    
    if (! threadShouldExit())
    {
        const ScopedLock sl (lock);
        
        if (treeToFill.hasType (MusicColumns::libraryIdentifier))
        {
            for (int i = 0; i < treeToFill.getNumChildren(); ++i)
            {
                ValueTree currentItem (treeToFill.getChild (i));
                int idOfChild = int (currentItem.getProperty (MusicColumns::columnNames[MusicColumns::ID]));
                
                existingIds.add (idOfChild);
                existingItems.add (currentItem);
            }
        }
    }
    
	while (! threadShouldExit())
	{
        //NON DROW
        while (!finished)
        {
            int currentItemId = -1;
            ValueTree newElement;
            
            bool alreadyExists = false;
            bool needToModify = false;
            bool isAudioFile = false;
            
            while (currentElement != nullptr)
            {
                if (currentElement->getTagName() == "key")
                {
                    currentItemId = currentElement->getAllSubText().getIntValue();  // e.g. <key>13452</key>
                    alreadyExists = existingIds.contains (currentItemId);
                    
                    if (alreadyExists)
                    {
                        // first get the relevant tree item
                        lock.enter();
                        const int index = existingIds.indexOf (currentItemId);
                        ValueTree existingElement (existingItems.getUnchecked (index));
                        lock.exit();
                        
                        // and then check the modification dates
                        XmlElement* trackDetails = currentElement->getNextElement(); // move on to the <dict>
                        
                        forEachXmlChildElement (*trackDetails, e)
                        {
                            if (e->getAllSubText() == MusicColumns::iTunesNames[MusicColumns::Modified])
                            {
                                const int64 newModifiedTime = parseITunesDateString (e->getNextElement()->getAllSubText()).toMilliseconds();
                                const int64 currentModifiedTime = int64 (existingElement.getProperty (MusicColumns::columnNames[MusicColumns::Modified]));
                                
                                if (newModifiedTime > currentModifiedTime)
                                {
                                    const ScopedLock sl (lock);
                                    treeToFill.removeChild (existingElement, nullptr);
                                    needToModify = true;
                                }
                                
                                break;
                            }
                        }
                    }
                    
                    newElement = ValueTree (MusicColumns::libraryItemIdentifier);
                    newElement.setProperty (MusicColumns::columnNames[MusicColumns::ID], currentItemId, nullptr);
                }
                
                currentElement = currentElement->getNextElement(); // move on to the <dict>
                
                if (! alreadyExists || needToModify)
                    break;
            }
            
            // check to see if we have reached the end
            if (currentElement == nullptr)
            {
                finished = true;
                //NON DROW
                //signalThreadShouldExit();
                break;
            }            
            
            if (currentElement->getTagName() == "dict")
            {
                // cycle through items of each track
                forEachXmlChildElement (*currentElement, e2)
                {	
                    const String elementKey (e2->getAllSubText());
                    //const String elementValue (e2->getNextElement()->getAllSubText());
                    
                    if (elementKey == "Kind")
                    {
                        if (e2->getNextElement()->getAllSubText().contains ("audio file"))
                        {
                            isAudioFile = true;
                            newElement.setProperty (MusicColumns::columnNames[MusicColumns::LibID], numAdded, nullptr);
                            numAdded++;
                        }
                    }
                    else if (elementKey == "Track Type")
                    {
                        // this is a file in iCloud, not a local, readable one
                        if (e2->getNextElement()->getAllSubText().contains ("Remote"))
                        {
                            isAudioFile = false;
                            break;
                        }
                    }
                    
                    // and check the entry against each column
                    for(int i = 2; i < MusicColumns::numColumns; i++)
                    {					
                        if (elementKey == MusicColumns::iTunesNames[i])
                        {
                            const String elementValue = e2->getNextElement()->getAllSubText();
                            
                            if (i == MusicColumns::Length
                                || i == MusicColumns::BPM
                                || i == MusicColumns::LibID)
                            {
                                newElement.setProperty (MusicColumns::columnNames[i], elementValue.getIntValue(), nullptr);
                            }
                            else if (i == MusicColumns::Added
                                     || i == MusicColumns::Modified)
                            {            
                                const int64 timeInMilliseconds (parseITunesDateString (elementValue).toMilliseconds());
                                newElement.setProperty (MusicColumns::columnNames[i], timeInMilliseconds, nullptr);
                            }
                            else
                            {
                                String textEntry (elementValue);
                                
                                if (i == MusicColumns::Location)
                                    textEntry = stripFileProtocolForLocal (elementValue);
                                
                                newElement.setProperty (MusicColumns::columnNames[i], textEntry, nullptr);
                            }
                        }
                    }
                }
                
                if (isAudioFile == true)
                {
                    const ScopedLock sl (lock);
                    treeToFill.addChild (newElement, -1, nullptr);
                }
                
                currentElement = currentElement->getNextElement(); // move to next track
            }
		}
        //NON DROW
        if (finished)
        {
             File playlistFile(File::getSpecialLocation (File::userMusicDirectory).getChildFile ("MusicPlayer/MusicPlayerPlaylists.xml"));
            
            Array<int> existingIds;
            
            for (int i = 0; i < playlistsTree.getNumChildren(); i++)
            {
                existingIds.add(playlistsTree.getChild(i).getProperty(MusicColumns::playlistID));
            }
            
            //Move onto playlists
            XmlElement* allPlaylists;
            allPlaylists = (XmlHelpers::findXmlElementContainingSubText(iTunesDatabase->getFirstChildElement(), "Playlist"))->getNextElement();
            
            XmlElement* e; 
            forEachXmlChildElement(*allPlaylists, e)
            {
                //DBG("Tag = " << e->getTagName());
                XmlElement* playlist;
                ValueTree singlePlaylist("ITEM");
                
                
                
                forEachXmlChildElement(*e, playlist)
                {
                    String elementKey = playlist->getAllSubText();
                    
                    if (elementKey == "Name")
                    {
                        String name = playlist->getNextElement()->getAllSubText();
                        ValueTree existingCheck = playlistsTree.getChildWithProperty("Name", name);
                        
                        if (name == "Library" || existingCheck.isValid())
                            break;
                        else
                            singlePlaylist.setProperty("Name", playlist->getNextElement()->getAllSubText(), 0);
                    }    
                    if (elementKey == "Playlist ID")
                    {
                        //If id already exists then it doesnt add it
                        int currentId = playlist->getNextElement()->getAllSubText().getIntValue();
                        if (!existingIds.contains(currentId))
                        {
                            //DBG("ID = " << playlist->getNextElement()->getAllSubText());
                            singlePlaylist.setProperty("PlaylistID", playlist->getNextElement()->getAllSubText(), 0);
                        }
                        else
                            break;
                    }
                    
                    if (elementKey == "Distinguished Kind" || elementKey == "Smart Info")
                    {
                        //DBG("Default itunes playlist found");
                        //Makes sure it doesn't parse default itunes libraries
                        //Remove break to parse them again
                        break;
                    }
                    
                    if (elementKey == "Playlist Items")
                    {
                        XmlElement* playlistItems;
                        int trackIDCounter = 1;
                        forEachXmlChildElement(*playlist->getNextElement(), playlistItems)
                        {
                            String playlistKey = playlistItems->getChildElementAllSubText("key", "0");
                            
                            //DBG("Playlist key = " << playlistKey);
                            if (playlistKey == "Track ID") {
                                singlePlaylist.setProperty("Size", trackIDCounter, 0);
                                String trackId = ("TrackID"+String(trackIDCounter));
                                singlePlaylist.setProperty(trackId, playlistItems->getChildElementAllSubText("integer", "0"), 0);
                                //DBG("Track Number = " << playlistItems->getChildElementAllSubText("integer", "0"));
                                trackIDCounter++;
                            }
                        }
                        singlePlaylist.setProperty("Modified", Time::getCurrentTime().toMilliseconds(), 0);
                        playlistsTree.addChild(singlePlaylist, -1, 0);
                    }
                    
                    //playlistsTree.addChild(singlePlaylist, -1, 0);
                }
            }
            
            playlistsFinished = true;
            
            if (finished && playlistsFinished)
            {
                signalThreadShouldExit();
                break;
            }
        }
        
	}
}
