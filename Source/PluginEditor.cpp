#include "PluginEditor.h"

static const juce::Colour kBackground (0xFF1A1A2E);
static const juce::Colour kPanel      (0xFF16213E);
static const juce::Colour kAccent     (0xFF00C896);
static const juce::Colour kAccentDim  (0xFF008866);
static const juce::Colour kTextLight  (0xFFE0E0E0);
static const juce::Colour kDivider    (0xFF333355);

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
    label.setFont(juce::FontOptions(12.0f));
    parent->addAndMakeVisible(label);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, paramId, slider);
}

HipstaPlateEditor::HipstaPlateEditor(HipstaPlateProcessor& p)
    : AudioProcessorEditor(&p), pluginProcessor(p)
{
    setupKnob(decaySlider,    decayLabel,    "DECAY",     pluginProcessor.apvts, "decay",    decayAttach,    this);
    setupKnob(sizeSlider,     sizeLabel,     "SIZE",      pluginProcessor.apvts, "size",     sizeAttach,     this);
    setupKnob(dampingSlider,  dampingLabel,  "DAMPING",   pluginProcessor.apvts, "damping",  dampingAttach,  this);
    setupKnob(predelaySlider, predelayLabel, "PRE-DELAY", pluginProcessor.apvts, "predelay", predelayAttach, this);
    setupKnob(widthSlider,    widthLabel,    "WIDTH",     pluginProcessor.apvts, "width",    widthAttach,    this);
    setupKnob(mixSlider,      mixLabel,      "MIX",       pluginProcessor.apvts, "mix",      mixAttach,      this);

    modeBox.addItemList(juce::StringArray("Clean", "Bright", "Dark", "Lush", "Dense"), 1);
    modeBox.setColour(juce::ComboBox::backgroundColourId, kPanel);
    modeBox.setColour(juce::ComboBox::textColourId, kTextLight);
    modeBox.setColour(juce::ComboBox::outlineColourId, kAccentDim);
    modeBox.setColour(juce::ComboBox::arrowColourId, kAccent);
    addAndMakeVisible(modeBox);

    modeLabel.setText("MODE", juce::dontSendNotification);
    modeLabel.setJustificationType(juce::Justification::centred);
    modeLabel.setColour(juce::Label::textColourId, kTextLight);
    modeLabel.setFont(juce::FontOptions(12.0f));
    addAndMakeVisible(modeLabel);

    modeAttach = std::make_unique<ComboBoxAttachment>(pluginProcessor.apvts, "mode", modeBox);

    setSize(480, 380);
    setResizable(false, false);
}

HipstaPlateEditor::~HipstaPlateEditor() = default;

void HipstaPlateEditor::paint(juce::Graphics& g)
{
    g.fillAll(kBackground);

    // Header background
    g.setColour(kPanel);
    g.fillRect(0, 0, getWidth(), 62);

    // Title
    g.setColour(kAccent);
    g.setFont(juce::FontOptions(22.0f));
    g.drawFittedText("HIPSTA PLATE", getLocalBounds().removeFromTop(38), juce::Justification::centred, 1);

    // Subtitle
    g.setColour(kAccentDim);
    g.setFont(juce::FontOptions(11.0f));
    g.drawFittedText("PLATE REVERB", getLocalBounds().removeFromTop(56).removeFromBottom(16), juce::Justification::centred, 1);

    // Divider
    g.setColour(kAccent.withAlpha(0.3f));
    g.fillRect(0, 62, getWidth(), 1);
}

void HipstaPlateEditor::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(66);

    // Mode selector row
    auto modeRow = area.removeFromTop(34).reduced(100, 0);
    modeLabel.setBounds(modeRow.removeFromLeft(46));
    modeBox.setBounds(modeRow.reduced(0, 2));

    area.removeFromTop(6);

    // Top row: Decay, Size, Damping
    const int knobH = 130;
    auto topRow = area.removeFromTop(knobH);
    const int knobW = topRow.getWidth() / 3;

    auto placeKnob = [&](juce::Slider& slider, juce::Label& label, juce::Rectangle<int>& row) {
        auto col = row.removeFromLeft(knobW);
        label.setBounds(col.removeFromTop(16));
        slider.setBounds(col);
    };

    placeKnob(decaySlider, decayLabel, topRow);
    placeKnob(sizeSlider, sizeLabel, topRow);
    {
        auto col = topRow;
        dampingLabel.setBounds(col.removeFromTop(16));
        dampingSlider.setBounds(col);
    }

    area.removeFromTop(4);

    // Bottom row: Pre-Delay, Width, Mix
    auto botRow = area.removeFromTop(knobH);

    placeKnob(predelaySlider, predelayLabel, botRow);
    placeKnob(widthSlider, widthLabel, botRow);
    {
        auto col = botRow;
        mixLabel.setBounds(col.removeFromTop(16));
        mixSlider.setBounds(col);
    }
}

juce::AudioProcessorEditor* HipstaPlateProcessor::createEditor()
{
    return new HipstaPlateEditor(*this);
}

bool HipstaPlateProcessor::hasEditor() const { return true; }
