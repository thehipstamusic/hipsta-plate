#pragma once

#include "PluginProcessor.h"

class IvanSoundEditor : public juce::AudioProcessorEditor
{
public:
    explicit IvanSoundEditor(IvanSoundProcessor&);
    ~IvanSoundEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    IvanSoundProcessor& pluginProcessor;

    juce::Slider mixSlider, decaySlider, sizeSlider;
    juce::Slider dampingSlider, predelaySlider, widthSlider;
    juce::Label mixLabel, decayLabel, sizeLabel;
    juce::Label dampingLabel, predelayLabel, widthLabel;

    juce::ComboBox modeBox;
    juce::Label modeLabel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<SliderAttachment> mixAttach, decayAttach, sizeAttach;
    std::unique_ptr<SliderAttachment> dampingAttach, predelayAttach, widthAttach;
    std::unique_ptr<ComboBoxAttachment> modeAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IvanSoundEditor)
};
