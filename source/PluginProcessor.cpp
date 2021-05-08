/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Subtractive_SynthesisAudioProcessor::Subtractive_SynthesisAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameters()),
                        lowPassFilter(juce::dsp::IIR::Coefficients<float>::makeLowPass(44100, 20000.0f, 0.1)),
                        highPassFilter(juce::dsp::IIR::Coefficients<float>::makeHighPass(44100, 20000.0f, 0.1)),
                        adsr()
#endif
{
}

Subtractive_SynthesisAudioProcessor::~Subtractive_SynthesisAudioProcessor()
{
}

//==============================================================================
const juce::String Subtractive_SynthesisAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Subtractive_SynthesisAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Subtractive_SynthesisAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Subtractive_SynthesisAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Subtractive_SynthesisAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Subtractive_SynthesisAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Subtractive_SynthesisAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Subtractive_SynthesisAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Subtractive_SynthesisAudioProcessor::getProgramName (int index)
{
    return {};
}

void Subtractive_SynthesisAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Subtractive_SynthesisAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    freq = 0.0;
    amp = 0;
    phase = 1.0;
    freqLowFilter = 20000.0;
    qLowFilter = 0.0;
    freqHighFilter = 50.0;
    qHighFilter = 0.0;

    adsr.setSampleRate(sampleRate);
    parameters.attack = 0.1f;
    parameters.decay = 0.1f;
    parameters.sustain = 1.f;
    parameters.release = 0.1f;
    adsr.setParameters(parameters);

    //filter parameters
    lastSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();

    lowPassFilter.prepare(spec);
    lowPassFilter.reset();

    highPassFilter.prepare(spec);
    highPassFilter.reset();
}

void Subtractive_SynthesisAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Subtractive_SynthesisAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Subtractive_SynthesisAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
    parameters.attack = *apvts.getRawParameterValue("A");
    parameters.decay = *apvts.getRawParameterValue("D");
    parameters.sustain = *apvts.getRawParameterValue("S");
    parameters.release = *apvts.getRawParameterValue("R");
    adsr.setParameters(parameters);
    
    //We have to retrieve the incoming midi message
    juce::MidiMessage m;
    int time;
    for (juce::MidiBuffer::Iterator i(midiMessages); i.getNextEvent(m, time);) {
        if (m.isNoteOn()) {
            amp = *apvts.getRawParameterValue("AMP");
            freq = m.getMidiNoteInHertz(m.getNoteNumber());
            adsr.noteOn();
        }
        else if (m.isNoteOff())
        {
            adsr.noteOff();
        }
    }

    //oscillator implementation
    float* channelDataL = buffer.getWritePointer(0);
    float* channelDataR = buffer.getWritePointer(1);
    int numSamples = buffer.getNumSamples();

    for (int i = 0; i < numSamples; ++i) {
        //we write into the output channels
        channelDataL[i] = amp * waveFunction(phase);
        channelDataR[i] = channelDataL[i];
        
        //we have to increment our phase value 2*pi*f*t with t=1/sample_rate
        phase += (float)(M_PI * 2. * (((double)freq/ (double)SAMPLE_RATE)));
    
        //when the phase becomes grater than 2pi we restart from 0
        if (phase > M_PI * 2.)
            phase -= M_PI * 2.;
    }
    
    adsr.applyEnvelopeToBuffer(buffer, 0, buffer.getNumSamples());

    // 12. Actually apply the filtering using Audio Block and ProcessContextReplacing
    // we use AudioBlock as a container for the input buffer
    // AudioBlock (AudioBuffer< OtherSampleType > &buffer)
    juce::dsp::AudioBlock <float> block(buffer);

    // we get the parameters from the value tree state
    // since getRawParameters return a pointer we need to use the * to access the value
    float freqLow = *apvts.getRawParameterValue("FREQL");
    float qualityLow = *apvts.getRawParameterValue("QL");

    // we update the filter state
    // static Ptr makeLowPass (double sampleRate, NumericType frequency, NumericType Q)
    *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, freqLow, qualityLow);
    lowPassFilter.process(juce::dsp::ProcessContextReplacing<float>(block));

    float freqHigh = *apvts.getRawParameterValue("FREQH");
    float qualityHigh = *apvts.getRawParameterValue("QH");

    *highPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(lastSampleRate, freqHigh, qualityHigh);
    highPassFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
}

//==============================================================================
bool Subtractive_SynthesisAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Subtractive_SynthesisAudioProcessor::createEditor()
{
    return new Subtractive_SynthesisAudioProcessorEditor (*this);
}

//==============================================================================
void Subtractive_SynthesisAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Subtractive_SynthesisAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Subtractive_SynthesisAudioProcessor();
}

float Subtractive_SynthesisAudioProcessor::waveFunction(float phase) {
    float waveForm;
    int waveType = *apvts.getRawParameterValue("WAVE");
    switch (waveType) {
        case 0: 
            return waveForm = (float)sin((double)phase);
        case 1: 
            return waveForm = (float)sign(sin((double)phase));
        case 2: 
            return waveForm = (float)triangle((double)phase);
        case 3:
            return waveForm = (float)sawtooth((double)phase);
    }
}

int Subtractive_SynthesisAudioProcessor::sign(double value) { return (value >= 0.0) ? 1 : -1; }

float Subtractive_SynthesisAudioProcessor::triangle(double phase)
{
    float res = 0.0;
    float value = phase / (M_PI * 2.);

    if (value < 0.25) {
        res = value*4;
    }
    else if (value < 0.75) {
        res = 2.0 - (value*4);
    }
    else {
        res = (value*4) - 4.0;
    }
    return res;
}

float Subtractive_SynthesisAudioProcessor::sawtooth(double phase)
{
    return ((phase / (M_PI * 2.)) * 2 - 1.0);
}

 //2. Let's define the function createParameters()
juce::AudioProcessorValueTreeState::ParameterLayout Subtractive_SynthesisAudioProcessor::createParameters()
{
  // parameters correspond to a vector of unique pointers to objects of classes RangedAudioParameters
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

  // we push in the vector two objects using make unique template (which actually allocate the memory for the objects) on the class AudioParameterFloat
  // AudioParameterFloat (String parameterID, String parameterName, float minValue, float maxValue, float defaultValue)
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FREQL", "Lowpass CutOff Frequency",
        juce::NormalisableRange<float>(
            20.0f,
            20000.0f,
            20.0f,
            0.5,
            false
        ), 20000.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("QL", "Lowpass Q Factor", 0.1f, 1.0f, 0.1f));

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FREQH", "Highpass CutOff Frequency",
        juce::NormalisableRange<float>(
            20.0f,
            20000.0f,
            20.0f,
            0.5,
            false
        ), 20.0f));

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("QH", "Highpass Q Factor", 0.1f, 1.0f, 0.1f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("A", "Attack", 0.1f, 10.0f, 0.1f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("D", "Decay", 0.1f, 10.0f, 0.1f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("S", "Sustain", 0.1f, 1.0f, 1.f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("R", "Release", 0.1f, 10.0f, 0.1f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("AMP", "Amplitude", 0.0f, 1.0f, 0.5f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("WAVE", "Wave Type", 0.0f, 3.0f, 0.f));

    // we return
    return { parameters.begin(), parameters.end() };
}

