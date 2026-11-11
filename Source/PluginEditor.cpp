#include "PluginEditor.h"
#include "MnBazzVersion.h"
#include "UserPresets.h"

namespace
{
    juce::String U (const char* utf8) { return juce::String::fromUTF8 (utf8); }
}

//==============================================================================
MnBazzEditor::MnBazzEditor (MnBazzProcessor& p)
    : AudioProcessorEditor (&p), proc (p)
{
    using namespace theme;
    setLookAndFeel (&laf);

    scopeView     = std::make_unique<widgets::ScopeView> (proc);
    filterView    = std::make_unique<widgets::FilterCurveView> (proc, coral());
    filterEnvView = std::make_unique<widgets::EnvelopeView> (proc, "f", cyan());
    ampEnvView    = std::make_unique<widgets::EnvelopeView> (proc, "a", cyan());
    for (auto* v : { (juce::Component*) scopeView.get(), (juce::Component*) filterView.get(),
                     (juce::Component*) filterEnvView.get(), (juce::Component*) ampEnvView.get() })
        addAndMakeVisible (*v);

    cards.reserve (8);
    addCard ("OSC 1", amber(), { { "osc1Oct", "Octave" }, { "osc1Level", "Level" } }, "osc1Wave");
    addCard ("OSC 2", amber(), { { "osc2Oct", "Octave" }, { "osc2Detune", "Detune" }, { "osc2Level", "Level" } }, "osc2Wave");
    addCard ("SUB / PLAY", amber(), { { "subLevel", "Sub" }, { "glide", "Glide" } }, "legato");

    auto& out = addCard ("OUTPUT", amber(), { { "master", "Volume" } });
    out.view = scopeView.get();
    out.viewWidth = 158.f;
    out.viewOnLeft = true;

    auto& flt = addCard ("FILTER", coral(), { { "cutoff", "Cutoff" }, { "res", "Reso" }, { "drive", "Drive" },
                                              { "envAmt", "Env Amt" }, { "keytrack", "Key Track" } });
    flt.view = filterView.get();
    flt.viewWidth = 232.f;

    addCard ("LFO", violet(), { { "lfoRate", "Rate" }, { "lfoCutoff", "To Cutoff" }, { "lfoPitch", "To Pitch" },
                                { "modCutoff", "Mod Wheel" } }, "lfoShape");

    auto& fe = addCard ("FILTER ENV", cyan(), { { "fA", "Attack" }, { "fD", "Decay" }, { "fS", "Sustain" }, { "fR", "Release" } });
    fe.view = filterEnvView.get();
    fe.viewWidth = 196.f;

    auto& ae = addCard ("AMP ENV", cyan(), { { "aA", "Attack" }, { "aD", "Decay" }, { "aS", "Sustain" }, { "aR", "Release" } });
    ae.view = ampEnvView.get();
    ae.viewWidth = 196.f;

    // preset bar
    presetPrev.setTooltip (U ("Previous preset"));
    presetNext.setTooltip (U ("Next preset"));
    presetPrev.onClick = [this] { stepPreset (-1); };
    presetNext.onClick = [this] { stepPreset (+1); };
    addAndMakeVisible (presetPrev);
    addAndMakeVisible (presetNext);

    categoryBox.addItemList (FactoryPresets::categories(), 1);
    categoryBox.setSelectedId (1, juce::dontSendNotification);
    categoryBox.setTooltip ("Category");
    categoryBox.onChange = [this] { rebuildPresetList(); };
    addAndMakeVisible (categoryBox);

    presetBox.setTooltip ("Preset list");
    presetBox.onChange = [this]
    {
        if (ignorePresetChange) return;
        applyPresetListIndex (presetBox.getSelectedItemIndex());
    };
    addAndMakeVisible (presetBox);

    initBtn.setTooltip (U ("Factory init"));
    saveBtn.setTooltip (U ("Save user preset"));
    loadBtn.setTooltip (U ("Load user preset"));
    initBtn.onClick = [this]
    {
        proc.loadFactoryPreset (0);
        rebuildPresetList();
    };
    saveBtn.onClick = [this] { saveUserPreset(); };
    loadBtn.onClick = [this] { loadUserPreset(); };
    for (auto* btn : { &initBtn, &saveBtn, &loadBtn })
        addAndMakeVisible (*btn);

    applyColours();

    setOpaque (true);
    setResizable (true, true);
    aspectConstrainer.setFixedAspectRatio ((double) designW / (double) designH);
    aspectConstrainer.setMinimumSize (840, 532);
    aspectConstrainer.setMaximumSize (2400, 1520);
    setConstrainer (&aspectConstrainer);
    setSize (designW, designH);
    rebuildPresetList();
}

MnBazzEditor::~MnBazzEditor()
{
    setLookAndFeel (nullptr);
}

//==============================================================================
MnBazzEditor::Card& MnBazzEditor::addCard (const juce::String& title, juce::Colour accent,
                                           std::initializer_list<std::pair<const char*, const char*>> knobs,
                                           const char* selectorParam)
{
    cards.emplace_back();
    auto& card = cards.back();
    card.title = title;
    card.accent = accent;

    if (selectorParam != nullptr)
    {
        card.selector = std::make_unique<widgets::SegmentedChoice> (proc.apvts, selectorParam, accent);
        addAndMakeVisible (*card.selector);
    }

    for (auto& [id, label] : knobs)
    {
        auto* param = proc.apvts.getParameter (id);
        jassert (param != nullptr);

        auto k = std::make_unique<Knob>();
        k->slider = std::make_unique<juce::Slider> (juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow);
        auto& sl = *k->slider;
        sl.setColour (juce::Slider::rotarySliderFillColourId, accent);
        sl.setPopupDisplayEnabled (false, false, nullptr);
        sl.setSliderSnapsToMousePosition (false);
        sl.setTooltip (param->getName (40));
        addAndMakeVisible (sl);

        // the attachment installs its own text callbacks, so ours go on afterwards
        k->attachment = std::make_unique<juce::SliderParameterAttachment> (*param, sl);
        sl.textFromValueFunction = [param] (double v) { return widgets::formatParam (*param, v); };
        sl.valueFromTextFunction = [param] (const juce::String& t) { return widgets::parseParam (*param, t); };
        sl.updateText();

        k->name = std::make_unique<juce::Label> ("", juce::String (label).toUpperCase());
        k->name->setJustificationType (juce::Justification::centred);
        k->name->setInterceptsMouseClicks (false, false);
        k->name->setColour (juce::Label::textColourId, theme::dim());
        addAndMakeVisible (*k->name);

        card.knobs.push_back (std::move (k));
    }
    return card;
}

void MnBazzEditor::applyColours()
{
    using namespace theme;

    for (auto* btn : { &initBtn, &loadBtn })
    {
        btn->setColour (juce::TextButton::textColourOffId, text());
        btn->setColour (juce::TextButton::textColourOnId, text());
    }
    saveBtn.getProperties().set ("style", "primary");
    saveBtn.setColour (juce::TextButton::buttonColourId, amber());
    saveBtn.setColour (juce::TextButton::textColourOffId, bgBottom());
    saveBtn.setColour (juce::TextButton::textColourOnId, bgBottom());

    categoryBox.setColour (juce::ComboBox::textColourId, dim());
    presetBox.setColour (juce::ComboBox::textColourId, text());
}

//==============================================================================
void MnBazzEditor::rebuildPresetList()
{
    const auto cat = categoryBox.getText();
    presetEntries.clear();
    ignorePresetChange = true;
    presetBox.clear();

    for (int i = 0; i < FactoryPresets::count; ++i)
        if (FactoryPresets::matchesCategory (i, cat))
        {
            PresetEntry e;
            e.factoryIndex = i;
            presetEntries.push_back (e);
            presetBox.addItem (FactoryPresets::get (i).name, (int) presetEntries.size());
        }

    for (auto& name : proc.listUserPresets())
    {
        PresetEntry e;
        e.user = true;
        e.userName = name;
        presetEntries.push_back (e);
        presetBox.addItem ("* " + name, (int) presetEntries.size());
    }

    currentListIndex = 0;
    if (proc.isUserPresetActive())
    {
        for (int i = 0; i < (int) presetEntries.size(); ++i)
            if (presetEntries[(size_t) i].user && presetEntries[(size_t) i].userName == proc.getActiveUserPresetName())
                currentListIndex = i;
    }
    else
    {
        for (int i = 0; i < (int) presetEntries.size(); ++i)
            if (! presetEntries[(size_t) i].user && presetEntries[(size_t) i].factoryIndex == proc.getCurrentProgram())
            {
                currentListIndex = i;
                break;
            }
    }

    if (! presetEntries.empty())
        presetBox.setSelectedId (currentListIndex + 1, juce::dontSendNotification);
    ignorePresetChange = false;
}

void MnBazzEditor::applyPresetListIndex (int listIndex)
{
    if (! juce::isPositiveAndBelow (listIndex, (int) presetEntries.size())) return;
    currentListIndex = listIndex;
    const auto& e = presetEntries[(size_t) listIndex];
    if (e.user)
        proc.loadUserPresetByName (e.userName);
    else
        proc.loadFactoryPreset (e.factoryIndex);
}

void MnBazzEditor::stepPreset (int delta)
{
    const int n = (int) presetEntries.size();
    if (n == 0) return;
    applyPresetListIndex ((currentListIndex + delta + n) % n);
    ignorePresetChange = true;
    presetBox.setSelectedId (currentListIndex + 1, juce::dontSendNotification);
    ignorePresetChange = false;
}

void MnBazzEditor::saveUserPreset()
{
    UserPresets::ensureDirectory();
    fileChooser = std::make_unique<juce::FileChooser> (U ("Save preset"), UserPresets::directory(), "*.mnbuzz");
    const auto chooserFlags = juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting;
    fileChooser->launchAsync (chooserFlags, [this] (const juce::FileChooser& ch)
    {
        auto file = ch.getResult();
        if (file != juce::File())
        {
            if (! file.hasFileExtension (".mnbuzz"))
                file = file.withFileExtension (".mnbuzz");
            proc.saveUserPresetToFile (file);
            rebuildPresetList();
        }
    });
}

void MnBazzEditor::loadUserPreset()
{
    UserPresets::ensureDirectory();
    fileChooser = std::make_unique<juce::FileChooser> (U ("Load preset"), UserPresets::directory(), "*.mnbuzz");
    fileChooser->launchAsync (juce::FileBrowserComponent::openMode, [this] (const juce::FileChooser& ch)
    {
        auto file = ch.getResult();
        if (file != juce::File() && proc.loadUserPresetFromFile (file))
            rebuildPresetList();
    });
}

//==============================================================================
void MnBazzEditor::paint (juce::Graphics& g)
{
    using namespace theme;
    const float s = scale;
    const auto b = getLocalBounds().toFloat();

    // backdrop
    g.setGradientFill (juce::ColourGradient::vertical (bgTop(), 0.f, bgBottom(), b.getBottom()));
    g.fillAll();
    g.setGradientFill (juce::ColourGradient (amber().withAlpha (0.075f), 170.f * s, 0.f,
                                             amber().withAlpha (0.f), 170.f * s + 620.f * s, 0.f, true));
    g.fillRect (b);
    g.setGradientFill (juce::ColourGradient (coral().withAlpha (0.045f), b.getRight(), b.getBottom(),
                                             coral().withAlpha (0.f), b.getRight() - 640.f * s, b.getBottom(), true));
    g.fillRect (b);

    // brand
    {
        g.setGradientFill (juce::ColourGradient (amber(), markArea.getX(), markArea.getY(),
                                                 coral(), markArea.getRight(), markArea.getBottom(), false));
        g.fillRoundedRectangle (markArea, 11.f * s);
        g.setColour (juce::Colours::white.withAlpha (0.22f));
        g.drawRoundedRectangle (markArea.reduced (0.5f), 11.f * s, 1.f);
        widgets::strokeWaveIcon (g, markArea.reduced (9.f * s, 11.f * s), "Saw", bgBottom(), 2.6f * s);

        const auto font = AppFonts::title (26.f * s, -0.01f);
        const float mnW = juce::GlyphArrangement::getStringWidth (font, "mn");
        g.setFont (font);
        g.setColour (text());
        g.drawText ("mn", brandArea, juce::Justification::centredLeft, false);
        g.setColour (amber());
        g.drawText ("Bazz", brandArea.withTrimmedLeft (mnW + 0.5f * s), juce::Justification::centredLeft, false);

        g.setColour (dim());
        g.setFont (AppFonts::title (8.5f * s, 0.14f));
        g.drawText (U ("MONO BASS SYNTH  ·  v") + MNBAZZ_VERSION_STRING, taglineArea, juce::Justification::centredLeft, false);
    }

    // cards
    for (auto& c : cards)
    {
        paintCard (g, c.bounds * s, cardRadius * s, c.accent);

        const auto t = c.titleArea * s;
        paintGlowDot (g, { t.getX() + 4.f * s, t.getCentreY() }, 2.6f * s, c.accent);
        g.setColour (text());
        g.setFont (AppFonts::title (11.5f * s, 0.14f));
        g.drawText (c.title, t.withTrimmedLeft (16.f * s), juce::Justification::centredLeft, false);
    }
}

//==============================================================================
void MnBazzEditor::layoutCard (Card& c, float s)
{
    auto sc = [s] (juce::Rectangle<float> r) { return (r * s).toNearestInt(); };

    const float pad = 16.f;
    auto r = c.bounds;
    auto head = r.removeFromTop (44.f);
    c.titleArea = head.withTrimmedLeft (pad);

    if (c.selector != nullptr)
    {
        const float w = (c.selector->usesIcons() ? 34.f : 56.f) * (float) c.selector->getNumSegments();
        c.selector->setBounds (sc ({ head.getRight() - pad - w, head.getY() + 10.f, w, 28.f }));
    }

    auto content = r.reduced (pad, 0.f);
    content.removeFromBottom (14.f);

    if (c.view != nullptr)
    {
        juce::Rectangle<float> vr;
        if (c.viewOnLeft) { vr = content.removeFromLeft  (c.viewWidth); content.removeFromLeft  (14.f); }
        else              { vr = content.removeFromRight (c.viewWidth); content.removeFromRight (14.f); }
        c.view->setBounds (sc (vr));
    }

    const int n = (int) c.knobs.size();
    if (n == 0)
        return;

    const float knobD = 70.f, textH = 18.f, labelH = 14.f;
    const float sliderH = knobD + 2.f + textH;
    const float blockH = sliderH + 3.f + labelH;
    const float y0 = content.getY() + (content.getHeight() - blockH) * 0.5f;
    const float cellW = content.getWidth() / (float) n;

    for (int i = 0; i < n; ++i)
    {
        auto& k = *c.knobs[(size_t) i];
        const float x0 = content.getX() + cellW * (float) i;

        k.slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false,
                                   juce::roundToInt (juce::jmin (cellW - 6.f, 84.f) * s), juce::roundToInt (textH * s));
        k.slider->setBounds (sc ({ x0 + 2.f, y0, cellW - 4.f, sliderH }));

        k.name->setFont (AppFonts::title (9.5f * s, 0.10f));
        k.name->setBounds (sc ({ x0, y0 + sliderH + 3.f, cellW, labelH }));
    }
}

void MnBazzEditor::resized()
{
    scale = juce::jmin ((float) getWidth() / (float) designW, (float) getHeight() / (float) designH);
    const float s = scale;
    laf.setUiScale (s);
    auto sc = [s] (juce::Rectangle<float> r) { return (r * s).toNearestInt(); };

    const float m = 22.f, gap = 14.f;
    auto area = juce::Rectangle<float> (0.f, 0.f, (float) designW, (float) designH).reduced (m);
    auto header = area.removeFromTop (56.f);
    area.removeFromTop (16.f);

    // brand
    const float midY = header.getCentreY();
    markArea    = juce::Rectangle<float> (m, midY - 20.f, 40.f, 40.f) * s;
    brandArea   = juce::Rectangle<float> (m + 52.f, midY - 23.f, 220.f, 28.f) * s;
    taglineArea = juce::Rectangle<float> (m + 53.f, midY + 6.f, 280.f, 14.f) * s;

    // right-aligned buttons, then the preset browser in the space between
    const float ch = 38.f, cy = midY - ch * 0.5f, bw = 62.f;
    float x = (float) designW - m;
    for (auto* btn : { &loadBtn, &saveBtn, &initBtn })
    {
        btn->setBounds (sc ({ x - bw, cy, bw, ch }));
        x -= bw + 8.f;
    }

    const float presetRight = x - 12.f;
    float px = 300.f;
    categoryBox.setBounds (sc ({ px, cy, 112.f, ch }));
    px += 112.f + 10.f;
    presetPrev.setBounds (sc ({ px, cy, 38.f, ch }));
    px += 38.f + 6.f;
    presetNext.setBounds (sc ({ presetRight - 38.f, cy, 38.f, ch }));
    presetBox.setBounds (sc ({ px, cy, presetRight - 38.f - 6.f - px, ch }));

    // card grid: three rows
    auto row1 = area.removeFromTop (184.f);
    area.removeFromTop (gap);
    auto row2 = area.removeFromTop (216.f);
    area.removeFromTop (gap);
    auto row3 = area;

    auto split = [&] (juce::Rectangle<float> row, std::initializer_list<float> weights, size_t first)
    {
        float total = 0.f;
        for (float w : weights) total += w;
        const float avail = row.getWidth() - gap * (float) (weights.size() - 1);
        for (float w : weights)
        {
            cards[first++].bounds = row.removeFromLeft (avail * w / total);
            row.removeFromLeft (gap);
        }
    };
    split (row1, { 1.9f, 3.0f, 2.2f, 2.6f }, 0);
    split (row2, { 0.62f, 0.38f }, 4);
    split (row3, { 1.f, 1.f }, 6);

    for (auto& c : cards)
        layoutCard (c, s);

    repaint();
}
