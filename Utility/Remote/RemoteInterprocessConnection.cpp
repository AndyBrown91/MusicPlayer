//
//  RemoteInterprocessConnection.cpp
//  MusicPlayer
//
//  Created by Andy on 28/12/2012.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "RemoteInterprocessConnection.h"

RemoteInterprocessConnection::RemoteInterprocessConnection () : InterprocessConnection(true)
{
    connectionNumber = remoteConnections.size();
}

RemoteInterprocessConnection::~RemoteInterprocessConnection()
{
    
}

void RemoteInterprocessConnection::connectionMade()
{
    DBG("Connection #" + String (connectionNumber) + " - connection started");
    DBG("Connected to = " << getConnectedHostName());
}

void RemoteInterprocessConnection::connectionLost()
{
    DBG("Connection #" + String (connectionNumber) + " - connection lost");
    remoteConnections.remove(connectionNumber);
}

void RemoteInterprocessConnection::messageReceived (const MemoryBlock& message)
{
    String stringMessage = message.toString();
    DBG("Connection #" + String (connectionNumber) + " - message received: " + stringMessage);
    
    if (stringMessage.startsWith("ConnectionMade"))
        {
            sendPlayingData();
        }
    
    if (stringMessage.startsWith("Play"))
        {
            ValueTree test (singletonLibraryTree.getChildWithProperty(MusicColumns::columnNames[MusicColumns::Song], stringMessage.fromFirstOccurrenceOf("Play ", false, true)));
            if (test.isValid()) {
                tableSelectedRow = test;
                tableShouldPlay = true;
            }
            
        }
    if (stringMessage.startsWith("Next")) {
        if (tableSelectedRow.isValid()) {
            int toPlay = filteredDataList.indexOf(tableSelectedRow);
            toPlay++;
            tableSelectedRow = filteredDataList.getChild(toPlay);
            tableShouldPlay = true;
        }
        
    }
    if (stringMessage.startsWith("Previous")) {
        if (tableSelectedRow.isValid()) {
            int toPlay = filteredDataList.indexOf(tableSelectedRow);
            toPlay--;
            tableSelectedRow = filteredDataList.getChild(toPlay);
            tableShouldPlay = true;
        }
        
    }
    if (stringMessage.startsWith("Position: "))
    {
        guiControl->setPosition(stringMessage.fromFirstOccurrenceOf("Position: ", false, true).getDoubleValue());
    }
    if (stringMessage.startsWith("Volume: "))
    {
        guiControl->setVolume(stringMessage.fromFirstOccurrenceOf("Volume: ", false, true).getDoubleValue());
    }
}

void RemoteInterprocessConnection::sendString (String incomingString)
{
    MemoryBlock messageData (incomingString.toUTF8(), (size_t) incomingString.getNumBytesAsUTF8());
    sendMessage(messageData);
}

void RemoteInterprocessConnection::setControls(GuiControl *gui, AudioControl *audio)
{
    guiControl.set(gui, false);
    audioControl.set(audio, false);
}

void RemoteInterprocessConnection::sendPlayingData()
{
    
    sendString("Artist: " + tablePlayingRow.getProperty(MusicColumns::columnNames[MusicColumns::Artist]).toString());
    sendString("Song: " + tablePlayingRow.getProperty(MusicColumns::columnNames[MusicColumns::Song]).toString());
    sendString("AlbumTitle: " + tablePlayingRow.getProperty(MusicColumns::columnNames[MusicColumns::Album]).toString());
    sendString("TracksTotal: " + String(filteredDataList.getNumChildren()));
    sendString("TrackNum: " + String(filteredDataList.indexOf(tablePlayingRow)));
    sendLength(audioControl->getTransportLength());
    sendString("PlayState: " + singletonPlayState.getValue().toString());            
    
    sendAlbumArt();
    
    sendString("NewTrack");
}

void RemoteInterprocessConnection::sendAlbumArt()
{
    sendString("AlbumArt");
    //Send album art memory block
}
void RemoteInterprocessConnection::sendLength(double length)
{
    for (int counter = 0; counter < remoteConnections.size(); counter++)
    {
        remoteConnections[counter]->sendString("Length: " + String(length));
    }
}
void RemoteInterprocessConnection::sendPosition (double position)
{
    for (int counter = 0; counter < remoteConnections.size(); counter++)
    {
        remoteConnections[counter]->sendString("Position: " + String(position));
    }
}
void RemoteInterprocessConnection::sendVolume (double volume)
{
    for (int counter = 0; counter < remoteConnections.size(); counter++)
    {
        remoteConnections[counter]->sendString("Volume: " + String(volume));
    }
}
void RemoteInterprocessConnection::sendPlayState()
{
    for (int counter = 0; counter < remoteConnections.size(); counter++)
    {
        remoteConnections[counter]->sendString("PlayState: " + singletonPlayState.getValue().toString());
    }
}