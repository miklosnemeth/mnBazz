#pragma once
#include <array>
#include <atomic>
#include <juce_audio_processors/juce_audio_processors.h>
#include "BuzzVoice.h"
#include "FactoryPresets.h"
#include "UserPresets.h"

class MnBazzProcessor : public juce::AudioProcessor
{
public:
    MnBazzProcessor();

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& l) const override
    { return l.getMainOutputChannelSet() == juce::AudioChannelSet::stereo(); }
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "mnBazz"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }

    int getNumPrograms() override { return FactoryPresets::count; }
    int getCurrentProgram() override { return currentFactoryProgram; }
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int, const juce::String&) override {}

    void loadFactoryPreset (int index);
    bool loadUserPresetByName (const juce::String& name);
    bool loadUserPresetFromFile (const juce::File& file);
    bool saveUserPresetToFile (const juce::File& file);
    juce::StringArray listUserPresets() const { return UserPresets::list(); }

    bool isUserPresetActive() const { return currentUserPreset.isNotEmpty(); }
    const juce::String& getActiveUserPresetName() const { return currentUserPreset; }

    float getOutputLevel() const { return outputLevel.load (std::memory_order_relaxed); }

    static constexpr int scopeBufferSize = 2048;
    int copyScopeSamples (float* dest, int maxSamples) const noexcept;

    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    VoiceParams readParams() const;
    float get (const char* id) const { return apvts.getRawParameterValue (id)->load(); }

    BuzzVoice voice;
    std::atomic<float> outputLevel { 0.f };
    std::array<float, scopeBufferSize> scopeData {};
    std::atomic<int> scopeWrite { 0 };

    void pushScopeSample (float sample) noexcept;
    int currentFactoryProgram = 0;
    juce::String currentUserPreset;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MnBazzProcessor)
};
