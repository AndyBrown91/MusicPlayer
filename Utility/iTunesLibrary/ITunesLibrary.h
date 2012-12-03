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

#ifndef __DROWAUDIO_ITUNESLIBRARY_H__
#define __DROWAUDIO_ITUNESLIBRARY_H__

#include "../JuceLibraryCode/JuceHeader.h"
#include "MusicLibraryHelpers.h"
#include "ITunesLibraryParser.h"
#include "Utility.h"
#include "Settings.h"


//==============================================================================
/** An ITunesLibrary manages the parsing of an iTunes library into a ValueTree.
    You can register yourselves as a listener to recieve callbacks when its 
    content has changed. You can also load a previously generated library tree 
    for data to me merged into if it is newer.
    For an example of its use see the MusicLibraryTable class.
 */
class ITunesLibrary : public Timer,
					  public DeletedAtShutdown,
                      public ValueTree::Listener
{
public:
    //==============================================================================
	juce_DeclareSingleton (ITunesLibrary, false);
	
    /** Creates an ITunesLibrary.
        This class can also be used as a singleton which may be more appropriate.
        To start the parsing use the setLibraryFile method.
     */
	ITunesLibrary();
	
    /** Destructor. */
	~ITunesLibrary();
	
	/**	Sets a new library to base the ValueTree on and starts the parse.
     
        This will merge new data into the tree if a valid tree has been set with
        the setLibraryTree method. This will preseve any child trees that may have 
        been added e.g. those holding loop or cue data.
	 */
	void setLibraryFile (File newFile);

    /** Returns the file that this library was generated from.
     */
    const File& getLibraryFile()    {   return libraryFile; }

    /** This can be used to load a previously saved iTuneLibarary.
        This has to be in the same format as that generated by this class.
        If the tree passed in is invalid, it will create a new, valid one.
     */
    void setLibraryTree (ValueTree& newTreeToUse);

    /** Returns the ValueTree that is being filled.
     */
	ValueTree getLibraryTree()      {	return libraryTree;	}
        
    /** Returns the lock being used in the parser.
        Bear in mind that if the parser has finished and been deleted this will be
        an unused lock so is not worth using it.
     */
    inline const CriticalSection& getParserLock()    {   return parserLock;   }
    
	//==============================================================================
    /** This returns the default iTunes library file.
        E.g. on Mac this will be something like:
        "/Users/username/Music/iTunes/iTunes Music Library.xml"
     */
	static File getDefaultITunesLibraryFile();
    
	//==============================================================================
    /** A class for receiving callbacks from an ITunesLibrary.
	 
		This will repeatedly call libraryUpdated so you can respond to any changes
		that may have happened until a single call to libraryFinished() where you
		may want to do some additional set-up.
	 
		@see ITunesLibrary::addListener, ITunesLibrary::removeListener
	 */
    class  Listener
    {
    public:
        //==============================================================================
        /** Destructor. */
        virtual ~Listener() {}
		
        //==============================================================================
        /** Called when the library file has changed, before it starts updating itself.
			Use this callback to initialise any data storage you may be holding.
		 */
        virtual void libraryChanged (ITunesLibrary* /*library*/) {};

        /** Called when the library has been updated.
		 */
        virtual void libraryUpdated (ITunesLibrary* library) = 0;

        /** Called when the library has finished updating.
		 */
        virtual void libraryFinished (ITunesLibrary* /*library*/) {}
    };
	
    /** Adds a listener to be called when this slider's value changes. */
    void addListener (Listener* listener);
	
    /** Removes a previously-registered listener. */
    void removeListener (Listener* listener);
	
    //==============================================================================
    /** @internal */
	void timerCallback();
	//NON DROW
	void saveLibrary(File& saveDestination);
    
    static File libraryNotFound();
    
    void valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property);
    
    void valueTreeChildAdded (ValueTree &parentTree, ValueTree &childWhichHasBeenAdded);
    
    void valueTreeChildRemoved (ValueTree &parentTree, ValueTree &childWhichHasBeenRemoved);
    
    void valueTreeChildOrderChanged (ValueTree &parentTreeWhoseChildrenHaveMoved);
    
    void valueTreeParentChanged (ValueTree &treeWhoseParentHasChanged);
    
private:
    //==============================================================================
    CriticalSection parserLock;
	ListenerList <Listener> listeners;

    File libraryFile;
	ScopedPointer<ITunesLibraryParser> parser;
	ValueTree libraryTree;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ITunesLibrary);
};

#endif  // __DROWAUDIO_ITUNESLIBRARY_H__