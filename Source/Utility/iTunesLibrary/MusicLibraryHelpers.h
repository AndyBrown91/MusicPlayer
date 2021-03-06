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

#ifndef __DROWAUDIO_MUSICLIBRARYHELPERS_H__
#define __DROWAUDIO_MUSICLIBRARYHELPERS_H__

#include "../JuceLibraryCode/JuceHeader.h"
#include "Utility.h"


//namespace LoopAndCueHelpers
//{
//    /** Returns the time from a give cue point index in a cue point tree.
//        The index starts at 0 and will return 0.0 if the index is out of range.
//     */
//    inline double getTimeFromCueTree (ValueTree& cueTree, int index)
//    {
//        if (index < cueTree.getNumProperties())
//        {
//            const String property(cueTree.getProperty(cueTree.getPropertyName(index)).toString());
//            return property.upToFirstOccurrenceOf(",", false, false).getDoubleValue();
//        }
//        
//        return 0.0;
//    }
//    
//    /** Returns the time from a give cue point index in a cue point tree.
//        The index starts at 0 and will return white if the index is out of range.
//     */
//    inline uint32 getColourFromCueTree (ValueTree& cueTree, int index)
//    {
//        if (index < cueTree.getNumProperties())
//        {
//            const String property(cueTree.getProperty(cueTree.getPropertyName(index)).toString());
//            return (uint32)property.fromLastOccurrenceOf(",", false, false).getLargeIntValue();
//        }
//        
//        return 0xffffffff;
//    }
//    
//    /** Returns the start time, end time and Colour of a give loop point in a loop tree.
//        The index starts at 0 and will return 0.0's if the index is out of range.
//     */
//    inline void getTimeAndColourFromLoopTree (ValueTree& loopTree, int index, double &startTime, double &endTime, uint32& colour)
//    {
//        if (index < loopTree.getNumProperties())
//        {
//            const String property(loopTree.getProperty(loopTree.getPropertyName(index)).toString());
//            startTime = property.upToFirstOccurrenceOf(",", false, false).getDoubleValue();
//            endTime = property.fromFirstOccurrenceOf(",", false, false).upToLastOccurrenceOf(",", false, false).getDoubleValue();
//            colour = (uint32)property.fromLastOccurrenceOf(",", false, false).getLargeIntValue();
//            return;
//        }
//        
//        startTime = endTime = 0.0;
//        colour = 0xffffffff;
//        
//        return;
//    }
//}

/**	Details the colums of the table.
 */
namespace MusicColumns {
    
    static const Identifier libraryIdentifier ("MUSICLIBRARY");
    static const Identifier libraryItemIdentifier ("ITEM");
    static const Identifier libraryCuePointIdentifier ("CUE");
    static const Identifier libraryLoopIdentifier ("LOOP");
    
    static const Identifier playlistsIdentifier ("PLAYLISTS");
    static const Identifier playlistName ("Name");
    static const Identifier playlistID ("PlaylistID");
    
	enum columns {
		Dummy,
		LibID,
		ID,
		Artist,
		Song,
		Album,
		Rating,
		BPM,
		Genre,
		SubGenre,
		Label,
		Key,
		Length,
		Kind,
		Added,
        Modified,
		Location,
		Score,
        TrackNum,
        Size,
        BitRate,
        SampleRate,
        PlayCount,
        
		numColumns
	};
	
	static const Identifier columnNames[] = { 
		"Dummy",
		"LibID",
		"ID",						
		"Artist",
		"Song",
		"Album",
		"Rating",
		"BPM",
		"Genre",
		"Sub_Genre",
		"Label",
		"Key",
		"Length",
		"Kind",
		"Added",
        "Modified",
		"Location",
		"Score",
        //NON Drow
        "TrackNum",
        "Size",
        "BitRate",
        "SampleRate",
        "PlayCount"
	};
	
	static const UNUSED_NOWARN char *iTunesNames[] = {
		"",
		"",
		"Track ID",
		"Artist",
		"Name",
		"Album",
		"Rating",
		"BPM",
		"Genre",
		"Grouping",
		"Comments",
		"Series",
		"Total Time",
		"Kind",
		"Date Added",
        "Date Modified",
		"Location",
		"Score",
        "Track Number",
        "Size",
        "Bit Rate",
        "Sample Rate",
        "Play Count"
	};
	
	static const int columnWidths[] = {
		1,
		30,
		50,
		150,
		200,
		150,
		60,
		60,
		80,
		100,
		100,
		30,
		80,
		60,
		125,
		125,
		300,
		60,
        50,
        80,
        50,
        50,
        50
	};
}

namespace TimeHelpers {
    /** Converts a number representing a time in seconds to a string in the format Hours:Minutes:Seconds
     @param numSeconds The time to be converted from seconds to a string
     @return The time as a string in the format H:M:S
     */
	static const String secondsToTimeLength (double numSeconds)
	{
		double decimalTime = numSeconds / 60000.0;
		
		int hrs = 0;
		int mins = (int) decimalTime;
		int secs = roundToInt ((decimalTime - mins) * 60.0);
		
		String timeString;
		
		if (mins > 59)
		{
			hrs = mins / 60;
			mins -= hrs * 60;
			
			timeString << String (hrs) << ":"
			<< String (mins).paddedLeft ('0', 2) << ":";
		}
		else
			timeString << String (mins) << ":";
		
		timeString << String (secs).paddedLeft ('0', 2);
		
		return timeString;
	}
}

#endif  // __DROWAUDIO_MUSICLIBRARYHELPERS_H__