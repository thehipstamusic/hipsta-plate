#include "PluginProcessor.h"
#include "PluginEditor.h"

HipstaPlateProcessor::HipstaPlateProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

HipstaPlateProcessor::~HipstaPlateProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout HipstaPlateProcessor::createParameterLayout()
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

const juce::String HipstaPlateProcessor::getName() const { return JucePlugin_Name; }
bool HipstaPlateProcessor::acceptsMidi() const { return false; }
bool HipstaPlateProcessor::producesMidi() const { return false; }
bool HipstaPlateProcessor::isMidiEffect() const { return false; }
double HipstaPlateProcessor::getTailLengthSeconds() const { return 5.0; }
int HipstaPlateProcessor::getNumPrograms() { return 1; }
int HipstaPlateProcessor::getCurrentProgram() { return 0; }
void HipstaPlateProcessor::setCurrentProgram(int) {}
const juce::String HipstaPlateProcessor::getProgramName(int) { return {}; }
void HipstaPlateProcessor::changeProgramName(int, const juce::String&) {}

void HipstaPlateProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    reverb.prepare(sampleRate, samplesPerBlock);
}

void HipstaPlateProcessor::releaseResources() {}

bool HipstaPlateProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void HipstaPlateProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
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

    reverb.process(buffer.getWritePointer(0),
                   buffer.getWritePointer(1),
                   buffer.getNumSamples(),
                   mixPct);
}

void HipstaPlateProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    const std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void HipstaPlateProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HipstaPlateProcessor();
}
