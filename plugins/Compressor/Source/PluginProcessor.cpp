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
CompressorAudioProcessor::CompressorAudioProcessor()
{
    fifo.setSize (2, 44100);
    
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

CompressorAudioProcessor::~CompressorAudioProcessor()
{
}

//==============================================================================
void CompressorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    GinProcessor::prepareToPlay (sampleRate, samplesPerBlock);

    compressor.setSampleRate (sampleRate);
    compressor.reset();
}

void CompressorAudioProcessor::reset()
{
    GinProcessor::reset();
    
    compressor.reset();
}

void CompressorAudioProcessor::releaseResources()
{
}

void CompressorAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer&)
{
    int numSamples = buffer.getNumSamples();

    ScratchBuffer fifoData (buffer);
    FloatVectorOperations::add (fifoData.getWritePointer (0), fifoData.getReadPointer (0), numSamples);
    FloatVectorOperations::multiply (fifoData.getWritePointer (0), 0.5, numSamples);
        
    if (isSmoothing())
    {
        int pos = 0;
        while (pos < numSamples)
        {
            auto workBuffer = sliceBuffer (buffer, pos, 1);
            
            compressor.setInputGain (input->getProcValue (1));
            compressor.setOutputGain (output->getProcValue (1));
            compressor.setParams (attack->getProcValue (1),
                                  release->getProcValue (1),
                                  threshold->getProcValue (1),
                                  ratio->getProcValue (1),
                                  knee->getProcValue (1));
            
            compressor.process (workBuffer);
            
            pos++;
        }

    }
    else
    {
        compressor.setInputGain (input->getProcValue (numSamples));
        compressor.setOutputGain (output->getProcValue (numSamples));
        compressor.setParams (attack->getProcValue (numSamples),
                              release->getProcValue (numSamples),
                              threshold->getProcValue (numSamples),
                              ratio->getProcValue (numSamples),
                              knee->getProcValue (numSamples));

        compressor.process (buffer);
    }
    
    FloatVectorOperations::copy (fifoData.getWritePointer (1), buffer.getReadPointer (0), numSamples);
    FloatVectorOperations::add (fifoData.getWritePointer (1), buffer.getReadPointer (1), numSamples);
    FloatVectorOperations::multiply (fifoData.getWritePointer (1), 0.5, numSamples);
    
    if (fifo.getFreeSpace() >= numSamples)
        fifo.write (fifoData);
}

//==============================================================================
bool CompressorAudioProcessor::hasEditor() const
{
    return true;
}

AudioProcessorEditor* CompressorAudioProcessor::createEditor()
{
    return new CompressorAudioProcessorEditor (*this);
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CompressorAudioProcessor();
}