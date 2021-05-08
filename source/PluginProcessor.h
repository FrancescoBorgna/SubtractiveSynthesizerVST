/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

//we add some constants
#define SAMPLE_RATE 44100
//if pi is not defined
#ifndef M_PI
//we define it
#define M_PI 3.14159265
#endif

#include <JuceHeader.h>

//==============================================================================
/**
*/
class Subtractive_SynthesisAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    Subtractive_SynthesisAudioProcessor();
    ~Subtractive_SynthesisAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

private:
    //============================================================================= 
    float phase;
    float amp;
    float freq;

    float waveFunction(float phase);
    int sign(double value);
    float triangle(double phase);
    float sawtooth(double phase);

    float freqLowFilter;
    float qLowFilter;

    float freqHighFilter;
    float qHighFilter;

    juce::ADSR::Parameters parameters;
    juce::ADSR adsr;

    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients <float>> lowPassFilter;
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients <float>> highPassFilter;
    float lastSampleRate;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Subtractive_SynthesisAudioProcessor)
};
