/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class PluginEditor  : public gin::ProcessorEditor,
                      private gin::Parameter::Listener
{
public:
    PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    void resized() override;
    Rectangle<int> getGridArea (int x, int y, int w = 1, int h = 1) override;

private:
    CriticalSection lock;
    void updateScope();
    void parameterChanged (gin::Parameter*) override { updateScope(); }
    
    PluginProcessor& scopeProc;

    gin::TriggeredScope scope { scopeProc.fifo };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
