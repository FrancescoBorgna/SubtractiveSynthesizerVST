/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class Subtractive_SynthesisAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Subtractive_SynthesisAudioProcessorEditor (Subtractive_SynthesisAudioProcessor&);
    ~Subtractive_SynthesisAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Subtractive_SynthesisAudioProcessor& audioProcessor;

    juce::Slider ampSlider;
    juce::Label ampLabel;

    juce::ComboBox waveBox;

    juce::Slider freqLowFilterSlider;
    juce::Label freqLowFilterLabel;
    juce::Slider qLowFilterSlider;
    juce::Label qLowFilterLabel;

    juce::Slider freqHighFilterSlider;
    juce::Label freqHighFilterLabel;
    juce::Slider qHighFilterSlider;
    juce::Label qHighFilterLabel;

    juce::Slider attackSlider;
    juce::Label attackLabel;
    juce::Slider releaseSlider;
    juce::Label releaseLabel;
    juce::Slider sustainSlider;
    juce::Label sustainLabel;
    juce::Slider decaySlider;
    juce::Label decayLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ampAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveBoxAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> qualityLowSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frequencyLowSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> qualityHighSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frequencyHighSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;

    juce::Image image;
    juce::ImageComponent imageComponent;

    juce::Image backgroundImage;
    
    int xMarginLeft = 32;
    int xADSR = 365;
    int yADSR = 70;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Subtractive_SynthesisAudioProcessorEditor)
};
