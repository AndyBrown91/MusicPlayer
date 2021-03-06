//
//  LastFmConnection.cpp
//  MusicPlayer
//
//  Created by Andy on 09/06/2013.
//
//

#include "LastFmConnection.h"
#include "LastFmButton.h"

OptionalScopedPointer<LastFmButton> lastFmButton;

LastFmConnection::LastFmConnection() : toScrobble("SCROBBLE")
{
    enabled.addListener(this);
    apiKey = "5becb8a0259e2b0ee071e33c970610b4";
    
    scrobblesFile = File::getSpecialLocation(File::userMusicDirectory).getFullPathName() + "/MusicPlayer/.scrobbles";
}

LastFmConnection::~LastFmConnection()
{
    if (toScrobble.getNumChildren() > 0)
    {
        writeValueTreeToFile(toScrobble, scrobblesFile);
    }
}

void LastFmConnection::setLastFmButton (Component* button)
{
    lastFmButton.set(dynamic_cast<LastFmButton*>(button), false);
}

bool LastFmConnection::getAuthToken()
{
    URL tokenAddress("http://ws.audioscrobbler.com/2.0/?method=auth.gettoken&api_key="+apiKey);
    
    ScopedPointer<XmlElement> token;
    token = tokenAddress.readEntireXmlStream();
    
    if (token != nullptr)
    {
        String status = token->getAttributeValue(0);
        if (status == "ok")
        {
            apiToken = token->getChildElement(0)->getAllSubText();
            
            return true;
        }
        else
        {
            //int statusInt = status.getIntValue();
            displayError(token);
            return false;
        }
    }
    
    else
        return false;
}

void LastFmConnection::connect()
{
    //Connect
    if (sessionKey == String::empty)
    {
        if (getAuthToken())
        {
            //Display auth dialog
            LastFmAuthPopup* authWindow;
            authWindow = new LastFmAuthPopup(apiKey, apiToken);
            
            DialogWindow::LaunchOptions o;
            
            o.content.setOwned (authWindow);
            o.content->setSize (authWindow->getWidth(), authWindow->getHeight());
            
            o.dialogTitle                   = "Please Authorise MusicPlayer";
            o.dialogBackgroundColour        = Colours::lightgrey;
            o.escapeKeyTriggersCloseButton  = true;
            o.useNativeTitleBar             = false;
            o.resizable                     = false;
            
            if (o.runModal() != 0 )
            {
                String apiSig = generateApiSig("auth.getSession");
                
                URL sessionAddress("http://ws.audioscrobbler.com/2.0/?method=auth.getSession&api_key="+apiKey+"&token="+apiToken+"&api_sig="+apiSig);
                
                ScopedPointer<XmlElement> session;
                session = sessionAddress.readEntireXmlStream();
                
                if (session != nullptr)
                {
                    String status = session->getAttributeValue(0);
                    
                    if (status == "ok")
                    {
                        ScopedPointer<XmlElement> info;
                        info = session->getChildElement(0);
                        
                        userName = info->getChildElement(0)->getAllSubText();
                        sessionKey = info->getChildElement(1)->getAllSubText();
                        
                        DBG(userName)
                        
                        connected = true;
                        lastFmButton->repaint();
                    }
                    else
                    {
                        DBG("Status error " << status);
                        displayError(session);
                    }
                }
            }
        }
    }
    else
    {
        //Session key exists
        connected = true;
        lastFmButton->repaint();
    }
}

void LastFmConnection::displayError(XmlElement *error)
{
    DBG("Error: " << error->getChildElement(0)->getAttributeValue(0).getIntValue() << "\n");
    DBG("Message: " << error->getChildElement(0)->getAllSubText());
    lastFmButton->displayError(error->getChildElement(0)->getAttributeValue(0).getIntValue(), error->getChildElement(0)->getAllSubText());
}

void LastFmConnection::valueChanged(Value &changed)
{
    if (changed == enabled)
    {
        if (enabled.getValue())
        {
            connect();
        }
        else
        {
            //disconnect or something....
            connected = false;
        }
        lastFmButton->repaint();
    }
}

String LastFmConnection::generateApiSig(String method)
{
    String apiSigString;

    apiSigString = "api_key"+apiKey+"method"+method+"token"+apiToken+"5502dc6ec6a34709b17139cf6a0026b8";
    
    MD5 apiSig = MD5(apiSigString.toUTF8());
    
    return apiSig.toHexString();
}

String LastFmConnection::lastFmString (ValueTree playingInfo, Identifier infoRequired)
{
    return URL::addEscapeChars(playingInfo.getProperty(infoRequired).toString(), true);
}

void LastFmConnection::getTrackInfo(ValueTree selectedTrack)
{
    Identifier artist = MusicColumns::columnNames[MusicColumns::Artist];
    Identifier album = MusicColumns::columnNames[MusicColumns::Album];
    Identifier track = MusicColumns::columnNames[MusicColumns::Song];

    
    URL getInfoAddress("http://ws.audioscrobbler.com/2.0/?method=track.getInfo&artist="+lastFmString(selectedTrack, artist)
        +"&track="+lastFmString(selectedTrack, track)
        +"&api_key="+apiKey
        +"&autocorrect=1"
        +"&username="+userName);
    
    ScopedPointer<XmlElement> response;
    response = getInfoAddress.readEntireXmlStream(true);
    
    saveXmlTest(response);
    
    if (response != nullptr)
    {
        String status = response->getAttributeValue(0);
        
        DBG("Response: \n" + response->getAllSubText());
        
        if (status == "ok")
        {
            DBG(status);
        }
        else
        {
            displayError(response);
        }
    }
}

void LastFmConnection::sendNowPlaying(ValueTree playingInfo)
{
    if (connected)
    {
        Identifier artist = MusicColumns::columnNames[MusicColumns::Artist];
        Identifier album = MusicColumns::columnNames[MusicColumns::Album];
        Identifier track = MusicColumns::columnNames[MusicColumns::Song];
        
        String apiSigString = "album"+playingInfo.getProperty(album).toString()
        +"api_key"+apiKey
        +"artist"+playingInfo.getProperty(artist).toString()
        +"method"+"track.updateNowPlaying"
        +"sk"+sessionKey
        +"track"+playingInfo.getProperty(track).toString()
        +"5502dc6ec6a34709b17139cf6a0026b8";
        
        MD5 apiSig = MD5(apiSigString.toUTF8());
        
        URL nowPlayingAddress("http://ws.audioscrobbler.com/2.0/?method=track.updateNowPlaying&artist="+lastFmString(playingInfo, artist)
                              +"&album="+lastFmString(playingInfo, album)
                              +"&track="+lastFmString(playingInfo, track)
                              +"&api_key="+apiKey
                              +"&sk="+sessionKey
                              +"&api_sig="+apiSig.toHexString());
        
        ScopedPointer<XmlElement> response;
        response = nowPlayingAddress.readEntireXmlStream(true);
        
        saveXmlTest(response);
        
        if (response != nullptr)
        {
            String status = response->getAttributeValue(0);
            
            DBG("Response: \n" + response->getAllSubText());
            
            if (status == "ok")
            {
                DBG(status);
            }
            else
            {
                displayError(response);
            }
            
        }
    }
}

void LastFmConnection::scrobbleTrack(ValueTree incomingTrack, Time startedPlaying)
{
    if (sessionKey != String::empty)
    {
        String timestamp (startedPlaying.toMilliseconds()/1000);
        
        ValueTree track = incomingTrack.createCopy();
        track.setProperty("Time", timestamp, 0);
        
        toScrobble.addChild(track, -1, 0);
        
        if (connected)
        {
            for (int i = 0; i < toScrobble.getNumChildren(); i++)
            {
                Identifier artist = MusicColumns::columnNames[MusicColumns::Artist];
                Identifier album = MusicColumns::columnNames[MusicColumns::Album];
                Identifier track = MusicColumns::columnNames[MusicColumns::Song];
                
                String timestamp (startedPlaying.toMilliseconds()/1000);
                
                String apiSigString = "album"+toScrobble.getChild(i).getProperty(album).toString()
                +"api_key"+apiKey
                +"artist"+toScrobble.getChild(i).getProperty(artist).toString()
                +"method"+"track.scrobble"
                +"sk"+sessionKey
                +"timestamp"+timestamp
                +"track"+toScrobble.getChild(i).getProperty(track).toString()
                +"5502dc6ec6a34709b17139cf6a0026b8";
                
                
                MD5 apiSig = MD5(apiSigString.toUTF8());
                
                URL nowPlayingAddress("http://ws.audioscrobbler.com/2.0/?method=track.scrobble&artist="+lastFmString(toScrobble.getChild(i), artist)
                                      +"&album="+lastFmString(toScrobble.getChild(i), album)
                                      +"&track="+lastFmString(toScrobble.getChild(i), track)
                                      +"&timestamp="+timestamp
                                      +"&api_key="+apiKey
                                      +"&sk="+sessionKey
                                      +"&api_sig="+apiSig.toHexString());
                
                ScopedPointer<XmlElement> response;
                response = nowPlayingAddress.readEntireXmlStream(true);
                
                saveXmlTest(response);
                
                if (response != nullptr)
                {
                    String status = response->getAttributeValue(0);
                    
                    DBG("Response: \n" + response->getAllSubText());
                    
                    if (status == "ok")
                    {
                        DBG(status);
                        //Successfully scrobbled, so can be removed
                        toScrobble.removeChild(i, 0);
                    }
                    else
                    {
                        displayError(response);
                    }
                    
                }
                
            }
        }
    }
}