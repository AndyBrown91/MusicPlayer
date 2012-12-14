#ifndef H_SETTINGS
#define H_SETTINGS

#include "../JuceLibraryCode/JuceHeader.h"
#include "Utility.h"
#include "MusicLibraryHelpers.h"


#define singletonLibraryFile Settings::getInstance()->getLibraryFile()
#define singletonLibraryTree Settings::getInstance()->getLibraryTree()
#define singletonCurrentLibId Settings::getInstance()->getCurrentLibId()
#define singletonCurrentValueTreeId Settings::getInstance()->getCurrentValueTreeId()
#define singletonPlayState Settings::getInstance()->getPlayState()
#define singletonUndoManager Settings::getInstance()->getUndoManager()

#define tableSelectedRow Settings::getInstance()->getSelectedRow()
#define tableShouldPlay Settings::getInstance()->getShouldPlay()
#define tablePlayingRow Settings::getInstance()->getPlayingRow()
#define tableUpdateRequired Settings::getInstance()->getUpdateRequired()

#define filteredDataList Settings::getInstance()->getFilteredList()


class Settings : public DeletedAtShutdown
{
public:
	Settings();
	
	~Settings();

	File& getLibraryFile();
	
    UndoManager* getUndoManager();
    
    int& getCurrentLibId();
    int& getCurrentValueTreeId();
    
    ValueTree& getLibraryTree();
    ValueTree& getSelectedRow();
    ValueTree& getPlayingRow();
    ValueTree& getFilteredList();
    
    Value& getShouldPlay();
    Value& getPlayState();
    Value& getUpdateRequired();
    
    
    void saveSingletons();
	
	juce_DeclareSingleton_SingleThreaded_Minimal (Settings)
	
private:

    File settingsXmlFile, libraryFile;
	ValueTree libraryTree, selectedRow, playingRow, filteredDataTree;
    ScopedPointer<UndoManager> undoManager;
    int currentLibId, currentValueTreeId;
    Value shouldPlay, playState, updateRequired;

};

#endif //H_SETTINGS