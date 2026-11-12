#pragma once
#include "PluginProcessor.h"
#include "LookAndFeel.h"
#include "Widgets.h"

class MnBazzEditor : public juce::AudioProcessorEditor,
                     private juce::Timer
{
public:
    explicit MnBazzEditor (MnBazzProcessor&);
    ~MnBazzEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;
    bool keyPressed (const juce::KeyPress&) override;

private:
    struct PresetEntry
    {
        bool user = false;
        int factoryIndex = 0;
        juce::String userName;
    };

    struct Knob
    {
        std::unique_ptr<juce::Slider> slider;
        std::unique_ptr<juce::Label> name;
        std::unique_ptr<juce::SliderParameterAttachment> attachment;
    };

    // One panel of the synth: title row (+ optional selector), a row of knobs and an optional display.
    struct Card
    {
        juce::String title;
        juce::Colour accent;
        std::vector<std::unique_ptr<Knob>> knobs;
        std::unique_ptr<widgets::SegmentedChoice> selector;
        juce::Component* view = nullptr;
        float viewWidth = 0.f;      // design units
        bool viewOnLeft = false;

        juce::Rectangle<float> bounds;     // design units, set in resized()
        juce::Rectangle<float> titleArea;  // design units
    };

    Card& addCard (const juce::String& title, juce::Colour accent,
                   std::initializer_list<std::pair<const char*, const char*>> knobs,
                   const char* selectorParam = nullptr);
    void layoutCard (Card& card, float s);
    void applyColours();

    void rebuildPresetList();
    void applyPresetListIndex (int listIndex);
    void stepPreset (int delta);
    void saveUserPreset();
    void loadUserPreset();
    void showPresetMenu();
    void markPresetClean();
    void updatePresetDirty();
    void timerCallback() override;

    MnLookAndFeel laf;
    MnBazzProcessor& proc;

    juce::ComboBox categoryBox, presetBox;
    widgets::ChevronButton presetPrev { false }, presetNext { true };
    juce::TextButton initBtn { "INIT" }, saveBtn { "SAVE" }, loadBtn { "LOAD" }, presetMenuBtn { "..." };
    juce::ComponentBoundsConstrainer aspectConstrainer;
    std::unique_ptr<juce::FileChooser> fileChooser;
    juce::ScopedMessageBox scopedMessageBox;
    juce::TooltipWindow tooltips { this, 600 };

    std::unique_ptr<widgets::ScopeView> scopeView;
    std::unique_ptr<widgets::FilterCurveView> filterView;
    std::unique_ptr<widgets::EnvelopeView> filterEnvView, ampEnvView;
    std::vector<Card> cards;

    std::vector<PresetEntry> presetEntries;
    bool ignorePresetChange = false;
    int currentListIndex = 0;
    juce::ValueTree cleanPresetState;
    bool presetDirty = false;
    juce::Rectangle<int> presetBarBounds;

    juce::Rectangle<float> markArea, brandArea, taglineArea;
    float scale = 1.f;

    static constexpr int designW = 1200;
    static constexpr int designH = 760;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MnBazzEditor)
};
