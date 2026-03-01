#include "PluginEditor.h"

static const juce::Colour kBackground   (0xFF1A1A2E);
static const juce::Colour kAccent       (0xFF00C896);
static const juce::Colour kAccentDim    (0xFF008866);
static const juce::Colour kTextLight    (0xFFE0E0E0);
static const juce::Colour kDivider      (0xFF333355);

static void setupKnob(juce::Slider& slider, juce::Label& label, const juce::String& text,
                       juce::AudioProcessorValueTreeState& apvts, const juce::String& paramId,
                       std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment,
                       juce::Component* parent)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    slider.setColour(juce::Slider::rotarySliderFillColourId, kAccent);
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, kDivider);
    slider.setColour(juce::Slider::thumbColourId, kAccent);
    slider.setColour(juce::Slider::textBoxTextColourId, kTextLight);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    parent->addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, kTextLight);
    label.setFont(juce::FontOptions(13.0f));
    parent->addAndMakeVisible(label);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, paramId, slider);
}

IvanSoundEditor::IvanSoundEditor(IvanSoundProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setupKnob(mixSlider, mixLabel, "MIX", processor.apvts, "mix", mixAttach, this);
    setupKnob(decaySlider, decayLabel, "DECAY", processor.apvts, "decay", decayAttach, this);
    setupKnob(sizeSlider, sizeLabel, "SIZE", processor.apvts, "size", sizeAttach, this);
    setupKnob(dampingSlider, dampingLabel, "DAMPING", processor.apvts, "damping", dampingAttach, this);
    setupKnob(predelaySlider, predelayLabel, "PRE-DELAY", processor.apvts, "predelay", predelayAttach, this);
    setupKnob(widthSlider, widthLabel, "WIDTH", processor.apvts, "width", widthAttach, this);

    // Mode dropdown
    modeBox.addItemList(juce::StringArray("Clean", "Bright", "Dark", "Lush", "Dense"), 1);
    modeBox.setColour(juce::ComboBox::backgroundColourId, kDivider);
    modeBox.setColour(juce::ComboBox::textColourId, kTextLight);
    modeBox.setColour(juce::ComboBox::outlineColourId, kAccentDim);
    modeBox.setColour(juce::ComboBox::arrowColourId, kAccent);
    addAndMakeVisible(modeBox);

    modeLabel.setText("MODE", juce::dontSendNotification);
    modeLabel.setJustificationType(juce::Justification::centred);
    modeLabel.setColour(juce::Label::textColourId, kTextLight);
    modeLabel.setFont(juce::FontOptions(13.0f));
    addAndMakeVisible(modeLabel);

    modeAttach = std::make_unique<ComboBoxAttachment>(processor.apvts, "mode", modeBox);

    setSize(520, 400);
}

IvanSoundEditor::~IvanSoundEditor() {}

void IvanSoundEditor::paint(juce::Graphics& g)
{
    g.fillAll(kBackground);

    // Title
    g.setColour(kAccent);
    g.setFont(juce::FontOptions(26.0f));
    g.drawFittedText("IVAN SOUND", getLocalBounds().removeFromTop(50), juce::Justification::centred, 1);

    // Subtitle
    g.setColour(kAccentDim);
    g.setFont(juce::FontOptions(12.0f));
    g.drawFittedText("PLATE REVERB", getLocalBounds().removeFromTop(68).removeFromBottom(16), juce::Justification::centred, 1);

    // Divider line
    g.setColour(kDivider);
    g.drawLine(30.0f, 72.0f, static_cast<float>(getWidth() - 30), 72.0f, 1.0f);
}

void IvanSoundEditor::resized()
{
    auto area = getLocalBounds().reduced(20);
    area.removeFromTop(75); // title + subtitle

    // Mode selector row
    auto modeRow = area.removeFromTop(36);
    auto modeArea = modeRow.withSizeKeepingCentre(200, 32);
    modeLabel.setBounds(modeArea.removeFromLeft(50));
    modeBox.setBounds(modeArea);

    area.removeFromTop(10);

    // Top row of knobs: Decay, Size, Damping
    auto topRow = area.removeFromTop(140);
    int knobW = topRow.getWidth() / 3;
    {
        auto col = topRow.removeFromLeft(knobW);
        decayLabel.setBounds(col.removeFromTop(18));
        decaySlider.setBounds(col);
    }
    {
        auto col = topRow.removeFromLeft(knobW);
        sizeLabel.setBounds(col.removeFromTop(18));
        sizeSlider.setBounds(col);
    }
    {
        auto col = topRow;
        dampingLabel.setBounds(col.removeFromTop(18));
        dampingSlider.setBounds(col);
    }

    area.removeFromTop(5);

    // Bottom row: Pre-Delay, Width, Mix
    auto botRow = area;
    knobW = botRow.getWidth() / 3;
    {
        auto col = botRow.removeFromLeft(knobW);
        predelayLabel.setBounds(col.removeFromTop(18));
        predelaySlider.setBounds(col);
    }
    {
        auto col = botRow.removeFromLeft(knobW);
        widthLabel.setBounds(col.removeFromTop(18));
        widthSlider.setBounds(col);
    }
    {
        auto col = botRow;
        mixLabel.setBounds(col.removeFromTop(18));
        mixSlider.setBounds(col);
    }
}

juce::AudioProcessorEditor* IvanSoundProcessor::createEditor()
{
    return new IvanSoundEditor(*this);
}

bool IvanSoundProcessor::hasEditor() const { return true; }
