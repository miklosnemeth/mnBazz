#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "UserPresets.h"

using Layout = juce::AudioProcessorValueTreeState::ParameterLayout;
using Range  = juce::NormalisableRange<float>;

namespace
{
    void addFloat (Layout& l, const char* id, const char* name, Range r, float def, const char* unit = "")
    {
        l.add (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { id, 1 }, name, r, def,
            juce::AudioParameterFloatAttributes().withLabel (unit)));
    }
    void addChoice (Layout& l, const char* id, const char* name, juce::StringArray c, int def)
    {
        l.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { id, 1 }, name, c, def));
    }
    void addInt (Layout& l, const char* id, const char* name, int lo, int hi, int def)
    {
        l.add (std::make_unique<juce::AudioParameterInt> (juce::ParameterID { id, 1 }, name, lo, hi, def));
    }
    void addEnv (Layout& l, const juce::String& p, const juce::String& n, float a, float d, float s, float r)
    {
        auto t = [] (float lo, float hi) { return Range (lo, hi, 0.001f, 0.4f); };
        addFloat (l, (p + "A").toRawUTF8(), (n + " Attack").toRawUTF8(),  t (0.001f, 5.f), a, "s");
        addFloat (l, (p + "D").toRawUTF8(), (n + " Decay").toRawUTF8(),   t (0.001f, 5.f), d, "s");
        addFloat (l, (p + "S").toRawUTF8(), (n + " Sustain").toRawUTF8(), Range (0.f, 1.f), s);
        addFloat (l, (p + "R").toRawUTF8(), (n + " Release").toRawUTF8(), t (0.001f, 8.f), r, "s");
    }
}

Layout MnBazzProcessor::createLayout()
{
    Layout l;
    const juce::StringArray waves { "Saw", "Square", "Triangle" };

    addChoice (l, "osc1Wave", "Osc1 Wave", waves, 0);
    addInt    (l, "osc1Oct",  "Osc1 Octave", -2, 2, 0);
    addFloat  (l, "osc1Level","Osc1 Level", Range (0.f, 1.f), 1.f);
    addChoice (l, "osc2Wave", "Osc2 Wave", waves, 0);
    addInt    (l, "osc2Oct",  "Osc2 Octave", -2, 2, 0);
    addFloat  (l, "osc2Detune","Osc2 Detune", Range (-100.f, 100.f, 0.1f), 7.f, "ct");
    addFloat  (l, "osc2Level","Osc2 Level", Range (0.f, 1.f), 0.7f);
    addFloat  (l, "subLevel", "Sub Level", Range (0.f, 1.f), 0.6f);

    addFloat (l, "cutoff", "Cutoff", Range (20.f, 20000.f, 0.f, 0.3f), 400.f, "Hz");
    addFloat (l, "res",    "Resonance", Range (0.f, 1.f), 0.25f);
    addFloat (l, "drive",  "Drive", Range (0.f, 1.f), 0.3f);
    addFloat (l, "envAmt", "Env Amount", Range (-1.f, 1.f), 0.5f);
    addFloat (l, "keytrack","Key Track", Range (0.f, 1.f), 0.3f);

    addEnv (l, "f", "Filter", 0.002f, 0.25f, 0.1f, 0.15f);
    addEnv (l, "a", "Amp",    0.002f, 0.30f, 0.8f, 0.12f);

    addFloat  (l, "glide",  "Glide", Range (0.f, 2.f, 0.f, 0.4f), 0.f, "s");
    addChoice (l, "legato", "Mode", { "Retrig", "Legato" }, 1);

    addFloat  (l, "lfoRate", "LFO Rate", Range (0.05f, 30.f, 0.f, 0.4f), 4.f, "Hz");
    addChoice (l, "lfoShape","LFO Shape", { "Triangle", "Square", "Saw", "S&H", "Sine" }, 0);
    addFloat  (l, "lfoCutoff","LFO > Cutoff", Range (0.f, 1.f), 0.f);
    addFloat  (l, "lfoPitch", "LFO > Pitch", Range (0.f, 100.f), 0.f, "ct");
    addFloat  (l, "modCutoff", "Mod > Cutoff", Range (0.f, 1.f), 0.35f);

    addFloat (l, "master", "Volume", Range (0.f, 1.f), 0.7f);
    return l;
}

MnBazzProcessor::MnBazzProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "STATE", createLayout())
{
}

VoiceParams MnBazzProcessor::readParams() const
{
    VoiceParams p;
    p.osc1Wave = (int) get ("osc1Wave"); p.osc2Wave = (int) get ("osc2Wave");
    p.osc1Oct  = (int) get ("osc1Oct");  p.osc2Oct  = (int) get ("osc2Oct");
    p.osc2Detune = get ("osc2Detune");
    p.osc1Level = get ("osc1Level"); p.osc2Level = get ("osc2Level"); p.subLevel = get ("subLevel");
    p.cutoff = get ("cutoff"); p.res = get ("res"); p.drive = get ("drive");
    p.envAmt = get ("envAmt"); p.keytrack = get ("keytrack");
    p.glide = get ("glide"); p.legato = get ("legato") > 0.5f;
    p.lfoRate = get ("lfoRate"); p.lfoShape = (int) get ("lfoShape");
    p.lfoToCutoff = get ("lfoCutoff"); p.lfoToPitch = get ("lfoPitch");
    p.modToCutoff = get ("modCutoff");
    p.master = get ("master");
    p.fEnv = { get ("fA"), get ("fD"), get ("fS"), get ("fR") };
    p.aEnv = { get ("aA"), get ("aD"), get ("aS"), get ("aR") };
    return p;
}

void MnBazzProcessor::prepareToPlay (double sampleRate, int)
{
    voice.prepare (sampleRate);
    scopeWrite.store (0, std::memory_order_relaxed);
    scopeData.fill (0.f);
}

void MnBazzProcessor::pushScopeSample (float sample) noexcept
{
    const int i = scopeWrite.load (std::memory_order_relaxed);
    scopeData[(size_t) (i % scopeBufferSize)] = sample;
    scopeWrite.store (i + 1, std::memory_order_relaxed);
}

int MnBazzProcessor::copyScopeSamples (float* dest, int maxSamples) const noexcept
{
    const int n = juce::jmin (maxSamples, scopeBufferSize);
    const int w = scopeWrite.load (std::memory_order_acquire);
    for (int i = 0; i < n; ++i)
    {
        const int idx = (w - n + i + scopeBufferSize * 8) % scopeBufferSize;
        dest[i] = scopeData[(size_t) idx];
    }
    return n;
}

void MnBazzProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    voice.setParams (readParams());

    auto* left = buffer.getWritePointer (0);
    auto* right = buffer.getNumChannels() > 1 ? buffer.getWritePointer (1) : nullptr;
    int pos = 0;

    float blockPeak = 0.f;

    auto render = [&] (int end)
    {
        for (; pos < end; ++pos)
        {
            const float s = voice.process();
            blockPeak = juce::jmax (blockPeak, std::abs (s));
            pushScopeSample (s);
            left[pos] = s;
            if (right != nullptr) right[pos] = s;
        }
    };

    for (const auto meta : midi)
    {
        render (meta.samplePosition);
        const auto m = meta.getMessage();
        if (m.isNoteOn())                  voice.noteOn (m.getNoteNumber());
        else if (m.isNoteOff())            voice.noteOff (m.getNoteNumber());
        else if (m.isAllNotesOff() || m.isAllSoundOff()) voice.allNotesOff();
        else if (m.isPitchWheel())         voice.setPitchBend ((m.getPitchWheelValue() - 8192) / 8192.f * 2.f);
        else if (m.isControllerOfType (1)) voice.setModWheel (m.getControllerValue() / 127.f);
    }
    render (buffer.getNumSamples());

    const float prev = outputLevel.load (std::memory_order_relaxed);
    outputLevel.store (juce::jmax (blockPeak, prev * 0.92f), std::memory_order_relaxed);
}

juce::AudioProcessorEditor* MnBazzProcessor::createEditor() { return new MnBazzEditor (*this); }

void MnBazzProcessor::loadFactoryPreset (int index)
{
    currentFactoryProgram = juce::jlimit (0, FactoryPresets::count - 1, index);
    currentUserPreset.clear();
    FactoryPresets::apply (apvts, currentFactoryProgram);
}

void MnBazzProcessor::setCurrentProgram (int index) { loadFactoryPreset (index); }

bool MnBazzProcessor::loadUserPresetByName (const juce::String& name)
{
    if (name.isEmpty()) return false;
    return loadUserPresetFromFile (UserPresets::directory().getChildFile (name + ".mnbuzz"));
}

bool MnBazzProcessor::loadUserPresetFromFile (const juce::File& file)
{
    if (! UserPresets::load (apvts, file)) return false;
    currentUserPreset = file.getFileNameWithoutExtension();
    return true;
}

bool MnBazzProcessor::saveUserPresetToFile (const juce::File& file)
{
    if (! UserPresets::save (apvts, file)) return false;
    currentUserPreset = file.getFileNameWithoutExtension();
    return true;
}

const juce::String MnBazzProcessor::getProgramName (int index)
{
    return FactoryPresets::get (index).name;
}

void MnBazzProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    if (auto xml = apvts.copyState().createXml())
    {
        xml->setAttribute ("preset", currentFactoryProgram);
        if (currentUserPreset.isNotEmpty())
            xml->setAttribute ("userPreset", currentUserPreset);
        copyXmlToBinary (*xml, dest);
    }
}

void MnBazzProcessor::setStateInformation (const void* data, int size)
{
    if (auto xml = getXmlFromBinary (data, size))
    {
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
        if (xml->hasAttribute ("preset"))
            currentFactoryProgram = xml->getIntAttribute ("preset", currentFactoryProgram);
        currentUserPreset = xml->getStringAttribute ("userPreset", {});
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new MnBazzProcessor(); }
