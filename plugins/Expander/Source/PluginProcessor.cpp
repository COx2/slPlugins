/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <random>

using namespace gin;

//==============================================================================
ExpanderAudioProcessor::ExpanderAudioProcessor()
{
    fifo.setSize (3, 44100);
    
    attack    = addExtParam ("attack",    "Attack",    "", "ms",   { 1.0f,   200.0f, 0.0f, 0.4f},    1.0f, 0.1f);
    release   = addExtParam ("release",   "Release",   "", "ms",   { 1.0f,  2000.0f, 0.0f, 0.4f},    5.0f, 0.1f);
    ratio     = addExtParam ("ratio",     "Ratio",     "", "",     { 1.0f,    30.0f, 0.0f, 0.4f},    5.0f, 0.1f);
    threshold = addExtParam ("threshold", "Threshold", "", "",     { -60.0f,   0.0f, 0.0f, 1.0f},  -30.0f, 0.1f);
    knee      = addExtParam ("knee",      "Knee",      "", "",     { 0.0f,    60.0f, 0.0f, 1.0f},    5.0f, 0.1f);
    input     = addExtParam ("input",     "Input",     "", "",     { -30.0f,  30.0f, 0.0f, 1.0f},    0.0f, 0.1f);
    output    = addExtParam ("output",    "Output",    "", "",     { -30.0f,  30.0f, 0.0f, 1.0f},    0.0f, 0.1f);
    
    attack->conversionFunction  = [] (float in) { return in / 1000.0; };
    release->conversionFunction = [] (float in) { return in / 1000.0; };
    input->conversionFunction   = [] (float in) { return Decibels::decibelsToGain (in); };
    output->conversionFunction  = [] (float in) { return Decibels::decibelsToGain (in); };
}

ExpanderAudioProcessor::~ExpanderAudioProcessor()
{
}

//==============================================================================
void ExpanderAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    gin::Processor::prepareToPlay (sampleRate, samplesPerBlock);

	expander.setMode (Dynamics::expander);
    expander.setSampleRate (sampleRate);
    expander.reset();
    expander.setNumChannels (getTotalNumInputChannels());
}

void ExpanderAudioProcessor::reset()
{
    gin::Processor::reset();
    
    expander.reset();
}

void ExpanderAudioProcessor::numChannelsChanged()
{
    expander.setNumChannels (getTotalNumInputChannels());
}

void ExpanderAudioProcessor::releaseResources()
{
}

void ExpanderAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer&)
{
    int numSamples = buffer.getNumSamples();

    ScratchBuffer fifoData (3, numSamples);
    if (getTotalNumInputChannels() == 2)
    {
        FloatVectorOperations::copy (fifoData.getWritePointer (0), buffer.getReadPointer (0), numSamples);
        FloatVectorOperations::add (fifoData.getWritePointer (0), buffer.getReadPointer (1), numSamples);
        FloatVectorOperations::multiply (fifoData.getWritePointer (0), 0.5, numSamples);
    }
    else
    {
        FloatVectorOperations::copy (fifoData.getWritePointer (0), buffer.getReadPointer (0), numSamples);
    }
    
    ScratchBuffer envData (1, numSamples);
        
    if (isSmoothing())
    {
        int pos = 0;
        while (pos < numSamples)
        {
            auto workBuffer = sliceBuffer (buffer, pos, 1);
            auto envWorkBuffer = sliceBuffer (envData, pos, 1);
            
            expander.setInputGain (input->getProcValue (1));
            expander.setOutputGain (output->getProcValue (1));
            expander.setParams (attack->getProcValue (1),
                                  release->getProcValue (1),
                                  threshold->getProcValue (1),
                                  ratio->getProcValue (1),
                                  knee->getProcValue (1));
            
            expander.process (workBuffer, &envWorkBuffer);
            
            pos++;
        }

    }
    else
    {
        expander.setInputGain (input->getProcValue (numSamples));
        expander.setOutputGain (output->getProcValue (numSamples));
        expander.setParams (attack->getProcValue (numSamples),
                              release->getProcValue (numSamples),
                              threshold->getProcValue (numSamples),
                              ratio->getProcValue (numSamples),
                              knee->getProcValue (numSamples));

        expander.process (buffer, &envData);
    }
    
    if (getTotalNumInputChannels() == 2)
    {
        FloatVectorOperations::copy (fifoData.getWritePointer (1), buffer.getReadPointer (0), numSamples);
        FloatVectorOperations::add (fifoData.getWritePointer (1), buffer.getReadPointer (1), numSamples);
        FloatVectorOperations::multiply (fifoData.getWritePointer (1), 0.5, numSamples);
    }
    else
    {
        FloatVectorOperations::copy (fifoData.getWritePointer (1), buffer.getReadPointer (0), numSamples);
    }
    
    FloatVectorOperations::copy (fifoData.getWritePointer (2), envData.getReadPointer (0), numSamples);
    
    if (fifo.getFreeSpace() >= numSamples)
        fifo.write (fifoData);
}

//==============================================================================
bool ExpanderAudioProcessor::hasEditor() const
{
    return true;
}

AudioProcessorEditor* ExpanderAudioProcessor::createEditor()
{
    return new ExpanderAudioProcessorEditor (*this);
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ExpanderAudioProcessor();
}