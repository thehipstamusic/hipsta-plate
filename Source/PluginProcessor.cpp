#include "PluginProcessor.h"
#include "PluginEditor.h"

IvanSoundProcessor::IvanSoundProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

IvanSoundProcessor::~IvanSoundProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout IvanSoundProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("mix", 1), "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 50.0f, "%"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("decay", 1), "Decay",
        juce::NormalisableRange<float>(0.1f, 0.999f, 0.001f, 0.5f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("size", 1), "Size",
        juce::NormalisableRange<float>(0.1f, 2.0f, 0.01f), 1.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("damping", 1), "Damping",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.3f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("predelay", 1), "Pre-Delay",
        juce::NormalisableRange<float>(0.0f, 500.0f, 0.1f, 0.4f), 0.0f, "ms"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("width", 1), "Width",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("mode", 1), "Mode",
        juce::StringArray("Clean", "Bright", "Dark", "Lush", "Dense"), 0));

    return { params.begin(), params.end() };
}

const juce::String IvanSoundProcessor::getName() const { return JucePlugin_Name; }
bool IvanSoundProcessor::acceptsMidi() const { return false; }
bool IvanSoundProcessor::producesMidi() const { return false; }
bool IvanSoundProcessor::isMidiEffect() const { return false; }
double IvanSoundProcessor::getTailLengthSeconds() const { return 5.0; }
int IvanSoundProcessor::getNumPrograms() { return 1; }
int IvanSoundProcessor::getCurrentProgram() { return 0; }
void IvanSoundProcessor::setCurrentProgram(int) {}
const juce::String IvanSoundProcessor::getProgramName(int) { return {}; }
void IvanSoundProcessor::changeProgramName(int, const juce::String&) {}

void IvanSoundProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    reverb.prepare(sampleRate, samplesPerBlock);
}

void IvanSoundProcessor::releaseResources() {}

bool IvanSoundProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void IvanSoundProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    const juce::ScopedNoDenormals noDenormals;

    const auto mixPct     = apvts.getRawParameterValue("mix")->load() / 100.0f;
    const auto decayVal   = apvts.getRawParameterValue("decay")->load();
    const auto sizeVal    = apvts.getRawParameterValue("size")->load();
    const auto dampingVal = apvts.getRawParameterValue("damping")->load();
    const auto predelayMs = apvts.getRawParameterValue("predelay")->load();
    const auto widthVal   = apvts.getRawParameterValue("width")->load();
    const auto modeVal    = static_cast<int>(apvts.getRawParameterValue("mode")->load());

    reverb.setParameters(decayVal, sizeVal, dampingVal, predelayMs, widthVal,
                         static_cast<DSP::PlateMode>(modeVal));

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Ensure stereo
    const auto* inL = buffer.getReadPointer(0);
    const auto* inR = numChannels > 1 ? buffer.getReadPointer(1) : buffer.getReadPointer(0);

    // Temp buffers for wet signal
    std::vector<float> wetL(static_cast<size_t>(numSamples));
    std::vector<float> wetR(static_cast<size_t>(numSamples));

    reverb.process(inL, inR, wetL.data(), wetR.data(), numSamples);

    // Mix dry/wet
    auto* outL = buffer.getWritePointer(0);
    for (int i = 0; i < numSamples; ++i)
        outL[i] = (inL[i] * (1.0f - mixPct)) + (wetL[static_cast<size_t>(i)] * mixPct);

    if (numChannels > 1)
    {
        auto* outR = buffer.getWritePointer(1);
        for (int i = 0; i < numSamples; ++i)
            outR[i] = (inR[i] * (1.0f - mixPct)) + (wetR[static_cast<size_t>(i)] * mixPct);
    }
}

void IvanSoundProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    const std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void IvanSoundProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new IvanSoundProcessor();
}
