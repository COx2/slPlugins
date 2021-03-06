/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#if __clang__
 #pragma clang diagnostic push
 #pragma clang diagnostic ignored "-Wimplicit-float-conversion"
 #pragma clang diagnostic ignored "-Wdefaulted-function-deleted"
 #pragma clang diagnostic ignored "-Wfloat-conversion"
 #pragma clang diagnostic ignored "-Wshift-sign-overflow"
 #pragma clang diagnostic ignored "-Wshadow-field-in-constructor"
 #pragma clang diagnostic ignored "-Wshadow"
 #pragma clang diagnostic ignored "-Wc++98-compat-extra-semi"
 #pragma clang diagnostic ignored "-Wsign-conversion"
 #pragma clang diagnostic ignored "-Wextra-semi"
 #pragma clang diagnostic ignored "-Wshorten-64-to-32"
 #pragma clang diagnostic ignored "-Wsign-compare"
 #pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#endif

#include <q/support/literals.hpp>
#include <q/support/notes.hpp>
#include <q/pitch/pitch_detector.hpp>

#if __clang__
 #pragma clang diagnostic pop
#endif

using namespace gin;
//==============================================================================
/**
*/
class PitchTrackAudioProcessor : public GinProcessor
{
public:
    //==============================================================================
    PitchTrackAudioProcessor();
    ~PitchTrackAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    
    LevelTracker& getOutputLevel() { return outputLevel; }
    float getPitch();

private:
    LevelTracker outputLevel {48.0};

    std::unique_ptr<cycfi::q::pitch_detector> detector;
        
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchTrackAudioProcessor)
};
