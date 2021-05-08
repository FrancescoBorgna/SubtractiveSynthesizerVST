/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Subtractive_SynthesisAudioProcessorEditor::Subtractive_SynthesisAudioProcessorEditor (Subtractive_SynthesisAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (850, 500);
    
    // BACKGROUND //capire come inserire il nome dell'immagine
    backgroundImage = juce::ImageCache::getFromMemory (BinaryData::Background_png, BinaryData::Background_pngSize);//** << HERE

    //AMP SLIDER
    ampSlider.setColour(ampSlider.textBoxOutlineColourId, juce::Colour::fromRGBA(0, 0, 0, 0));
    ampSlider.setRange(0.0, 1.0);
    ampSlider.setSliderStyle(juce::Slider::Rotary);
    ampSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    ampSlider.setValue(0.5);
    addAndMakeVisible(ampSlider);

    //COMB BOX
    waveBox.addItem("Sine", 1);
    waveBox.addItem("Rectangle", 2);
    waveBox.addItem("Triangle", 3);
    waveBox.addItem("Sawtooth", 4);
    waveBox.setSelectedId(1);
    addAndMakeVisible(waveBox);

    //low pass filter
    qLowFilterSlider.setColour(ampSlider.textBoxOutlineColourId, juce::Colour::fromRGBA(0, 0, 0, 0));
    qLowFilterSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    qLowFilterSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 20);
    qLowFilterSlider.setRange(0.0, 1.0, 0.1);
    addAndMakeVisible(qLowFilterSlider);

    
    freqLowFilterSlider.setColour(ampSlider.textBoxOutlineColourId, juce::Colour::fromRGBA(0, 0, 0, 0));
    freqLowFilterSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    freqLowFilterSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 20);
    freqLowFilterSlider.setSkewFactor(0.5);
    freqLowFilterSlider.setRange(20.0, 20000.0);
    freqLowFilterSlider.setValue(20000.0);
    addAndMakeVisible(freqLowFilterSlider);

    

    //high pass filter
    qHighFilterSlider.setColour(ampSlider.textBoxOutlineColourId, juce::Colour::fromRGBA(0, 0, 0, 0));
    qHighFilterSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    qHighFilterSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 20);
    qHighFilterSlider.setRange(0.0, 1.0, 0.1);
    addAndMakeVisible(qHighFilterSlider);


    freqHighFilterSlider.setColour(ampSlider.textBoxOutlineColourId, juce::Colour::fromRGBA(0, 0, 0, 0));
    freqHighFilterSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    freqHighFilterSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 20);
    freqHighFilterSlider.setSkewFactor(0.5);
    freqHighFilterSlider.setRange(20.0, 20000.0);
    freqHighFilterSlider.setValue(20.0);
    addAndMakeVisible(freqHighFilterSlider);


    //ADSR
    attackSlider.setColour(ampSlider.textBoxOutlineColourId, juce::Colour::fromRGBA(0, 0, 0, 0));
    attackSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    attackSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 20);
    attackSlider.setRange(0.1, 10.0, 0.2);
    attackSlider.setValue(0.1);
    addAndMakeVisible(attackSlider);
  
    decaySlider.setColour(ampSlider.textBoxOutlineColourId, juce::Colour::fromRGBA(0, 0, 0, 0));
    decaySlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    decaySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 20);
    decaySlider.setRange(0.1, 10.0, 0.2);
    decaySlider.setValue(0.1);
    addAndMakeVisible(decaySlider);
 
    sustainSlider.setColour(ampSlider.textBoxOutlineColourId, juce::Colour::fromRGBA(0, 0, 0, 0));
    sustainSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    sustainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 20);
    sustainSlider.setRange(0.1, 1.0, 0.1);
    sustainSlider.setValue(1);
    addAndMakeVisible(sustainSlider);

    releaseSlider.setColour(ampSlider.textBoxOutlineColourId, juce::Colour::fromRGBA(0, 0, 0, 0));
    releaseSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    releaseSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 100, 20);
    releaseSlider.setRange(0.1, 10.0, 0.2);
    releaseSlider.setValue(0.1);
    addAndMakeVisible(releaseSlider);


    //memory allocation
    ampAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "AMP", ampSlider);
    waveBoxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "WAVE", waveBox);
    frequencyLowSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FREQL", freqLowFilterSlider);
    qualityLowSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "QL", qLowFilterSlider);
    frequencyHighSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FREQH", freqHighFilterSlider);
    qualityHighSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "QH", qHighFilterSlider);
    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "A", attackSlider);
    decayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "D", decaySlider);
    sustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "S", sustainSlider);
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "R", releaseSlider);
}

Subtractive_SynthesisAudioProcessorEditor::~Subtractive_SynthesisAudioProcessorEditor()
{
}

//==============================================================================
void Subtractive_SynthesisAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::darkorange);

    g.setColour (juce::Colours::white);
    g.setFont (20.0f);
    g.drawSingleLineText("LowPass Filter", 150, 179, juce::Justification::centred);
    g.drawSingleLineText("HighPass Filter", 150, 329, juce::Justification::centred);
    g.drawImageAt (backgroundImage, 0, 0);
}

void Subtractive_SynthesisAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    ampSlider.setBounds(xMarginLeft, 50, 100, 100);
    waveBox.setBounds(140, 50, 100, 20);

    //low pass filter bounds
    freqLowFilterSlider.setBounds(xMarginLeft, 200, 100, 100);
    qLowFilterSlider.setBounds(150, 200, 100, 100);

    //high pass filter bounds
    freqHighFilterSlider.setBounds(xMarginLeft, 370, 100, 100);
    qHighFilterSlider.setBounds(150, 370, 100, 100);

    //ADSR
    attackSlider.setBounds(xADSR, yADSR, 100, 100);
    decaySlider.setBounds(xADSR+110, yADSR, 100, 100);
    sustainSlider.setBounds(xADSR+220, yADSR, 100, 100);
    releaseSlider.setBounds(xADSR+330, yADSR, 100, 100);
}
