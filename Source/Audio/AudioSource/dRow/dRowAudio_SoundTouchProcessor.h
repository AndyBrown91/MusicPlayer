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

#ifndef __DROWAUDIO_SOUNDTOUCHPROCESSOR_H__
#define __DROWAUDIO_SOUNDTOUCHPROCESSOR_H__

#include "../JuceLibraryCode/JuceHeader.h"

#include "soundtouch/SoundTouch.h"

using namespace soundtouch;

//==============================================================================
/** Wraps a SoundTouch object to enable pitch and tempo adjustments to an audio buffer;
 
 To use this is very simple, just create one, initialise it with the desired number
 of channels and sample rate then feed it with some samples and read them back out.
 This is not thread safe so make sure that the read and write methods are not called
 simultaneously by different threads.
 */
class SoundTouchProcessor
{
public:
    //==============================================================================
    /** A struct use to hold all the different playback settings.
     */
    struct PlaybackSettings
    {
        PlaybackSettings()
        : rate (1.0f),
        tempo (1.0f),
        pitch (1.0f)
        {}
        
        float rate, tempo, pitch;
    };
    
    //==============================================================================
    /** Create a default SoundTouchProcessor.
     Make sure that you call initialise before any processing take place.
     This will apply no shifting/stretching by default, use setPlaybackSetting() to
     apply these effects.
     */
    SoundTouchProcessor();
    
    /** Destructor.
     */
    ~SoundTouchProcessor();
    
    /** Puts the processor into a ready state.
     This must be set before any processing occurs as the results are undefiend if not.
     It is the callers responsibility to make sure the numChannels parameter matches
     those supplied to the read/write methods.
     */
    void initialise (int numChannels, double sampleRate);
    
    /** Writes samples into the pipline ready to be processed.
     Remember to keep a 1:1 ratio of input and output samples more or less samples may
     be required as input compared to output (think of a time stretch). You can find
     this ratio using getNumSamplesRequiredRatio().
     */
    void writeSamples (float** sourceChannelData, int numChannels, int numSamples, int startSampleOffset = 0);
    
    /** Reads out processed samples.
     This will read out as many samples as the processor has ready. Any additional
     space in the buffer will be slienced. As the processor takes a certain ammount of
     samples to calculate an output there is a latency of around 100ms involved in the process.
     */
    void readSamples (float** destinationChannelData, int numChannels, int numSamples, int startSampleOffset = 0);
    
    /** Clears the pipeline of all samples, ready for new processing.
     */
    void clear()                                                {   soundTouch.clear();             }
    
    /** Flushes the last samples from the processing pipeline to the output.
     Clears also the internal processing buffers.
     
     Note: This function is meant for extracting the last samples of a sound
     stream. This function may introduce additional blank samples in the end
     of the sound stream, and thus it's not recommended to call this function
     in the middle of a sound stream.
     */
    void flush()                                                {   soundTouch.flush();             }
    
    /** Returns the number of samples ready.
     */
    int getNumReady()                                           {   return soundTouch.numSamples(); }
    
    /** Returns the number of samples in the pipeline but currently unprocessed.
     */
    int getNumUnprocessedSamples()                              {   return soundTouch.numUnprocessedSamples();  }
    
    /** Sets all of the settings at once.
     */
    void setPlaybackSettings (PlaybackSettings newSettings);
    
    /** Returns all of the settings.
     */
    PlaybackSettings getPlaybackSettings()                      {   return settings;                            }
    
    /** Returns the effective playback ratio i.e. the number of output samples produced per input sample.
     */
    double getEffectivePlaybackRatio()                          {   return (double) soundTouch.getEffectiveRate() * soundTouch.getEffectiveTempo(); }
    
private:
    //==============================================================================
    SoundTouch soundTouch;
    
    CriticalSection lock;
    HeapBlock<float> interleavedInputBuffer, interleavedOutputBuffer;
    int interleavedInputBufferSize, interleavedOutputBufferSize;
    PlaybackSettings settings;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoundTouchProcessor);
};

#endif // __DROWAUDIO_SOUNDTOUCHPROCESSOR_H__