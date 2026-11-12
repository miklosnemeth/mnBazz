#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

struct PresetData
{
    const char* name;
    int   osc1Wave, osc1Oct, osc2Wave, osc2Oct, legato, lfoShape;
    float osc1Level, osc2Level, subLevel, osc2Detune;
    float cutoff, res, drive, envAmt, keytrack;
    float fA, fD, fS, fR;
    float aA, aD, aS, aR;
    float glide, lfoRate, lfoCutoff, lfoPitch, master;
};

namespace FactoryPresets
{
    inline void setParam (juce::AudioProcessorValueTreeState& apvts, const char* id, float value)
    {
        if (auto* p = apvts.getParameter (id))
            p->setValueNotifyingHost (p->convertTo0to1 (value));
    }

    inline void apply (juce::AudioProcessorValueTreeState& apvts, const PresetData& p)
    {
        setParam (apvts, "osc1Wave", (float) p.osc1Wave);
        setParam (apvts, "osc1Oct",  (float) p.osc1Oct);
        setParam (apvts, "osc1Level", p.osc1Level);
        setParam (apvts, "osc2Wave", (float) p.osc2Wave);
        setParam (apvts, "osc2Oct",  (float) p.osc2Oct);
        setParam (apvts, "osc2Detune", p.osc2Detune);
        setParam (apvts, "osc2Level", p.osc2Level);
        setParam (apvts, "subLevel", p.subLevel);
        setParam (apvts, "cutoff", p.cutoff);
        setParam (apvts, "res", p.res);
        setParam (apvts, "drive", p.drive);
        setParam (apvts, "envAmt", p.envAmt);
        setParam (apvts, "keytrack", p.keytrack);
        setParam (apvts, "fA", p.fA);
        setParam (apvts, "fD", p.fD);
        setParam (apvts, "fS", p.fS);
        setParam (apvts, "fR", p.fR);
        setParam (apvts, "aA", p.aA);
        setParam (apvts, "aD", p.aD);
        setParam (apvts, "aS", p.aS);
        setParam (apvts, "aR", p.aR);
        setParam (apvts, "glide", p.glide);
        setParam (apvts, "legato", (float) p.legato);
        setParam (apvts, "lfoRate", p.lfoRate);
        setParam (apvts, "lfoShape", (float) p.lfoShape);
        setParam (apvts, "lfoCutoff", p.lfoCutoff);
        setParam (apvts, "lfoPitch", p.lfoPitch);
        setParam (apvts, "modCutoff", 0.35f);
        setParam (apvts, "modToPitch", 0.f);
        setParam (apvts, "lfoToAmp", 0.f);
        setParam (apvts, "velToAmp", 0.65f);
        setParam (apvts, "velToFilt", 0.25f);
        setParam (apvts, "noiseLevel", 0.f);
        setParam (apvts, "osc2Width", 0.5f);
        setParam (apvts, "osc2Sync", 0.f);
        setParam (apvts, "fEnvToRes", 0.f);
        setParam (apvts, "outputMode", 1.f);
        setParam (apvts, "master", p.master);
    }

    constexpr int count = 47;

    inline const char* categoryFor (int index)
    {
        static const char* cats[] =
        {
            "Classic", "Sub", "Classic", "Classic", "Funk", "Acid", "Classic", "Acid", "FX", "Sub",
            "FX", "Classic", "Funk", "Sub", "Acid", "FX", "FX", "Sub", "Classic", "Classic",
            "FX", "FX", "Sub", "Classic", "FX", "Acid",
            "Deep House", "Deep House", "Deep House", "Deep House",
            "House", "House", "House", "House",
            "Techno", "Techno", "Techno", "Techno",
            "Trance", "Trance", "Trance", "Trance",
            "House", "Techno", "Techno", "Acid", "House"
        };
        constexpr int n = (int) (sizeof (cats) / sizeof (cats[0]));
        static_assert (n == count);
        return cats[juce::jlimit (0, n - 1, index)];
    }

    inline juce::StringArray categories()
    {
        return { "All", "Deep House", "House", "Techno", "Trance", "Sub", "Classic", "Acid", "Funk", "FX" };
    }

    inline bool matchesCategory (int index, const juce::String& category)
    {
        return category == "All" || juce::String (categoryFor (index)) == category;
    }

    inline const PresetData& get (int index)
    {
        static constexpr PresetData factory[] =
        {
            { "Init Bass",     0,-1, 0,-1, 1,0, 0.9f,0.5f,0.65f, 7.f,  380.f,0.28f,0.25f,0.55f,0.35f, 0.002f,0.22f,0.12f,0.18f, 0.002f,0.28f,0.78f,0.14f, 0.f, 4.f,0.f,0.f,0.72f },
            { "Deep Sub",      0,-2, 0,-2, 1,0, 1.f,0.f,0.95f, 0.f,  120.f,0.08f,0.15f,0.2f, 0.15f, 0.004f,0.35f,0.05f,0.25f, 0.002f,0.4f,0.85f,0.2f,  0.f, 2.f,0.f,0.f,0.75f },
            { "Punchy Saw",    0, 0, 0, 0, 0,0, 1.f,0.35f,0.5f, 5.f,  650.f,0.42f,0.45f,0.7f,0.4f,  0.001f,0.18f,0.08f,0.12f, 0.001f,0.2f,0.55f,0.08f, 0.f, 0.f,0.f,0.f,0.7f },
            { "Moog Classic",  0, 0, 0, 0, 1,0, 0.85f,0.7f,0.55f, 12.f, 520.f,0.55f,0.35f,0.65f,0.45f, 0.002f,0.25f,0.15f,0.2f,  0.003f,0.35f,0.8f,0.18f, 0.02f,3.f,0.f,0.f,0.68f },
            { "Funk Slap",     0, 0, 1, 0, 0,0, 0.7f,0.8f,0.35f,-3.f,  1100.f,0.35f,0.55f,0.85f,0.55f, 0.001f,0.08f,0.f,0.1f,    0.001f,0.12f,0.1f,0.06f,  0.f, 8.f,0.15f,0.f,0.65f },
            { "Reese Bass",    0, 0, 0, 0, 1,0, 0.75f,0.75f,0.4f, 18.f,  280.f,0.62f,0.4f,0.35f,0.5f,  0.008f,0.4f,0.22f,0.35f, 0.004f,0.45f,0.88f,0.25f, 0.05f,0.35f,0.2f,3.f,0.7f },
            { "Rubber Duck",   2,-1, 2,-1, 1,0, 0.6f,0.5f,0.7f, 6.f,  240.f,0.48f,0.22f,0.9f,0.3f,  0.005f,0.3f,0.18f,0.28f, 0.002f,0.35f,0.75f,0.22f, 0.f, 6.f,0.45f,0.f,0.68f },
            { "Acid 303",      0, 0, 0, 0, 0,1, 0.95f,0.15f,0.2f, 4.f,  900.f,0.72f,0.65f,0.95f,0.6f,  0.001f,0.05f,0.f,0.08f,    0.001f,0.15f,0.7f,0.1f,   0.f,12.f,0.55f,8.f,0.62f },
            { "Dub Wobble",    0,-1, 0,-2, 1,0, 0.8f,0.6f,0.85f, 0.f,  180.f,0.5f,0.3f,0.75f,0.25f, 0.01f,0.5f,0.28f,0.4f,  0.004f,0.5f,0.9f,0.4f,  0.f, 1.2f,0.85f,0.f,0.72f },
            { "808 Trap",      0,-2, 0,-2, 1,0, 0.5f,0.f,1.f, 0.f,   90.f,0.05f,0.55f,0.15f,0.1f,  0.001f,0.6f,0.f,0.45f,   0.001f,0.55f,0.f,0.35f,  0.f, 0.f,0.f,0.f,0.78f },
            { "Neuro Growl",   0, 0, 1,-1, 0,2, 0.65f,0.85f,0.55f, 22.f,  420.f,0.68f,0.75f,0.55f,0.65f, 0.002f,0.2f,0.35f,0.15f, 0.002f,0.18f,0.6f,0.12f, 0.f, 9.f,0.25f,12.f,0.66f },
            { "Pluck Bass",    2, 0, 0, 1, 0,0, 0.55f,0.45f,0.35f, 8.f,  1400.f,0.38f,0.4f,0.9f,0.7f,  0.001f,0.06f,0.f,0.18f,    0.001f,0.08f,0.f,0.22f,  0.f, 0.f,0.f,0.f,0.64f },
            { "House Bass",    0, 0, 1, 0, 1,0, 0.7f,0.65f,0.6f, 3.f,  480.f,0.33f,0.38f,0.6f,0.35f,  0.002f,0.2f,0.18f,0.15f, 0.002f,0.22f,0.72f,0.12f, 0.01f,5.f,0.1f,0.f,0.7f },
            { "Trap Sub 808",  0,-2, 0,-2, 1,0, 0.4f,0.f,1.f, 0.f,   70.f,0.02f,0.7f,0.1f,0.05f,  0.001f,0.7f,0.f,0.5f,    0.001f,0.65f,0.f,0.42f, 0.f, 0.f,0.f,0.f,0.8f },
            { "DnB Reese",     0, 0, 0, 0, 1,0, 0.8f,0.8f,0.45f, 25.f,  320.f,0.58f,0.5f,0.4f,0.55f, 0.006f,0.35f,0.25f,0.3f,  0.003f,0.4f,0.85f,0.28f, 0.03f,0.5f,0.35f,5.f,0.69f },
            { "Wah Filter",    0,-1, 0, 0, 1,0, 0.75f,0.35f,0.5f, 0.f,  350.f,0.45f,0.3f,0.95f,0.5f,  0.003f,0.25f,0.2f,0.2f,  0.002f,0.3f,0.7f,0.18f, 0.f, 4.f,0.9f,0.f,0.67f },
            { "Metallic",      1, 0, 1, 1, 0,2, 0.6f,0.75f,0.3f, 35.f,  1800.f,0.72f,0.8f,0.5f,0.75f, 0.001f,0.1f,0.f,0.12f,    0.001f,0.12f,0.45f,0.1f, 0.f,11.f,0.05f,18.f,0.63f },
            { "Soft Sub",      2,-2, 2,-2, 1,0, 0.55f,0.3f,0.9f, 3.f,  140.f,0.12f,0.1f,0.25f,0.2f,  0.005f,0.45f,0.1f,0.35f, 0.004f,0.5f,0.88f,0.35f, 0.f, 2.f,0.f,0.f,0.74f },
            { "Legato Glide",  0, 0, 0, 0, 1,0, 0.9f,0.55f,0.55f, 9.f,  450.f,0.4f,0.3f,0.6f,0.4f,  0.004f,0.3f,0.2f,0.25f,  0.008f,0.35f,0.82f,0.35f, 0.18f,3.f,0.f,0.f,0.68f },
            { "Wide Detune",   0, 0, 0, 0, 1,0, 0.7f,0.75f,0.5f, 28.f,  390.f,0.38f,0.28f,0.45f,0.42f, 0.003f,0.28f,0.18f,0.22f, 0.004f,0.32f,0.8f,0.2f,  0.02f,2.f,0.f,0.f,0.7f },
            { "Filter Swoop",  0,-1, 0,-1, 0,0, 0.8f,0.5f,0.65f, 5.f,  220.f,0.52f,0.35f,0.85f,0.55f, 0.08f,0.55f,0.18f,0.45f,  0.004f,0.4f,0.75f,0.3f,  0.f, 1.5f,0.6f,0.f,0.71f },
            { "Sidechain Pump",0, 0, 0,-1, 1,0, 0.85f,0.4f,0.75f, 0.f,  300.f,0.4f,0.25f,0.55f,0.3f,  0.004f,0.4f,0.15f,0.25f,  0.003f,0.38f,0.82f,0.22f, 0.f, 2.5f,0.95f,0.f,0.73f },
            { "Dark Room",     0,-1, 1,-2, 1,0, 0.65f,0.55f,0.8f, -5.f,  160.f,0.35f,0.45f,0.35f,0.25f, 0.006f,0.45f,0.22f,0.5f,  0.005f,0.5f,0.88f,0.55f, 0.f, 0.8f,0.3f,0.f,0.72f },
            { "Bright Pop",    0, 1, 0, 0, 0,0, 0.75f,0.5f,0.35f, 6.f,  1200.f,0.32f,0.42f,0.75f,0.65f, 0.001f,0.15f,0.05f,0.14f, 0.001f,0.18f,0.65f,0.12f, 0.f, 5.f,0.08f,0.f,0.66f },
            { "LFO Vibrato",   0, 0, 0, 0, 1,0, 0.8f,0.45f,0.5f, 0.f,  600.f,0.3f,0.22f,0.4f,0.4f,  0.002f,0.25f,0.12f,0.18f,  0.003f,0.3f,0.78f,0.2f,  0.f, 5.5f,0.f,35.f,0.68f },
            { "Square Crusher",1,-1, 1, 0, 0,1, 0.9f,0.7f,0.6f, 10.f,  500.f,0.5f,0.85f,0.55f,0.5f,  0.001f,0.12f,0.1f,0.1f,   0.002f,0.25f,0.7f,0.15f,  0.f,15.f,0.35f,0.f,0.64f },

            // Deep house: round sub, soft filter, notes that sit under a kick.
            { "Velvet Floor",  2,-1, 2,-2, 1,4, 0.35f,0.12f,0.95f, 3.f,   160.f,0.14f,0.08f,0.22f,0.18f, 0.006f,0.42f,0.18f,0.35f, 0.004f,0.38f,0.86f,0.28f, 0.06f,0.25f,0.06f,0.f,0.76f },
            { "Deep Roller",   0,-1, 0,-1, 1,4, 0.55f,0.4f, 0.8f,  8.f,   240.f,0.22f,0.16f,0.32f,0.28f, 0.003f,0.3f, 0.22f,0.24f, 0.002f,0.26f,0.78f,0.18f, 0.03f,0.4f, 0.1f, 0.f,0.72f },
            { "Offbeat Deep",  0,-1, 1,-1, 0,0, 0.6f, 0.35f,0.75f, 5.f,   190.f,0.26f,0.2f, 0.62f,0.22f, 0.001f,0.16f,0.05f,0.14f, 0.001f,0.18f,0.22f,0.12f, 0.f,  0.f,  0.f,  0.f,0.7f },
            { "Slide Deep",    0, 0, 2,-1, 1,4, 0.5f, 0.28f,0.7f,  6.f,   280.f,0.3f, 0.18f,0.4f, 0.32f, 0.008f,0.36f,0.25f,0.3f,  0.01f, 0.4f, 0.84f,0.32f, 0.16f,0.3f, 0.08f,0.f,0.7f },

            // House: shorter, more mid, a little drive so it cuts a drum bus.
            { "Jack Saw",      0, 0, 0, 0, 0,0, 0.85f,0.55f,0.45f, 9.f,   720.f,0.3f, 0.34f,0.58f,0.4f,  0.001f,0.12f,0.06f,0.1f,  0.001f,0.16f,0.4f, 0.08f, 0.f,  0.f,  0.f,  0.f,0.68f },
            { "Organ Stab",    1, 0, 1, 1, 0,0, 0.7f, 0.4f, 0.25f,-7.f,   980.f,0.22f,0.28f,0.72f,0.55f, 0.001f,0.07f,0.f,  0.12f, 0.001f,0.09f,0.f,  0.14f, 0.f,  0.f,  0.f,  0.f,0.64f },
            { "UKG Knock",     0,-1, 0,-2, 0,0, 0.4f, 0.15f,1.f,   0.f,   140.f,0.18f,0.42f,0.7f, 0.12f, 0.001f,0.08f,0.f,  0.1f,  0.001f,0.11f,0.05f,0.08f, 0.f,  0.f,  0.f,  0.f,0.74f },
            { "Funky Roll",    0, 0, 1, 0, 1,0, 0.65f,0.55f,0.5f, -4.f,   640.f,0.28f,0.3f, 0.55f,0.38f, 0.002f,0.2f, 0.16f,0.16f, 0.002f,0.22f,0.7f, 0.14f, 0.02f,4.5f, 0.08f,0.f,0.69f },

            // Techno: darker, more resonance and drive, built for repetition.
            { "Warehouse",     0,-1, 1, 0, 0,0, 0.75f,0.45f,0.55f, 4.f,   360.f,0.5f, 0.52f,0.48f,0.35f, 0.001f,0.14f,0.08f,0.12f, 0.001f,0.18f,0.55f,0.1f,  0.f,  0.f,  0.f,  0.f,0.66f },
            { "Hypnotic",      0,-1, 0,-1, 1,4, 0.6f, 0.35f,0.7f,  6.f,   250.f,0.46f,0.28f,0.3f, 0.3f,  0.01f, 0.5f, 0.35f,0.4f,  0.004f,0.45f,0.9f, 0.35f, 0.f,  0.28f,0.55f,0.f,0.7f },
            { "Peak Square",   1, 0, 1,-1, 0,1, 0.8f, 0.35f,0.4f,  2.f,   780.f,0.38f,0.72f,0.78f,0.45f, 0.001f,0.07f,0.f,  0.08f, 0.001f,0.12f,0.25f,0.08f, 0.f,  8.f,  0.12f,0.f,0.62f },
            { "Low Rumble",    0,-2, 0,-1, 1,0, 0.45f,0.3f, 0.95f, 0.f,   130.f,0.32f,0.38f,0.25f,0.15f, 0.008f,0.55f,0.3f, 0.5f,  0.006f,0.6f, 0.92f,0.45f, 0.f,  0.6f, 0.2f, 0.f,0.73f },

            // Trance: rolling lines, filter movement, a gated one for offbeat patterns.
            { "Rolling 138",   0, 0, 0, 0, 1,4, 0.7f, 0.65f,0.5f,  12.f,  460.f,0.34f,0.22f,0.52f,0.42f, 0.004f,0.38f,0.28f,0.32f, 0.003f,0.34f,0.72f,0.22f, 0.05f,0.22f,0.16f,0.f,0.68f },
            { "Uplift Line",   0, 0, 0, 1, 1,0, 0.65f,0.4f, 0.4f,  10.f,  1100.f,0.28f,0.26f,0.62f,0.58f, 0.002f,0.22f,0.12f,0.18f, 0.002f,0.28f,0.62f,0.16f, 0.04f,0.15f,0.1f, 0.f,0.66f },
            { "Psy Bite",      0, 0, 0,-1, 0,2, 0.9f, 0.25f,0.35f, 3.f,   260.f,0.7f, 0.4f, 0.95f,0.5f,  0.001f,0.08f,0.f,  0.1f,  0.001f,0.12f,0.f,  0.12f, 0.f,  0.f,  0.f,  0.f,0.64f },
            { "Trance Gate",   0,-1, 0, 0, 1,1, 0.75f,0.6f, 0.45f, 8.f,   560.f,0.36f,0.24f,0.35f,0.4f,  0.002f,0.3f, 0.4f, 0.25f, 0.002f,0.25f,0.88f,0.2f,  0.f,  10.f, 0.48f,0.f,0.67f },

            { "Vel Pluck",     0, 0, 1, 0, 0,0, 0.55f,0.5f,0.3f, 5.f,  1100.f,0.3f,0.32f,0.85f,0.6f,  0.001f,0.06f,0.f,0.16f, 0.001f,0.08f,0.25f,0.18f, 0.f, 0.f,0.f,0.f,0.66f },
            { "Click Sub",     0,-2, 0,-2, 0,0, 0.35f,0.f,1.f, 0.f,  95.f, 0.08f,0.55f,0.2f, 0.1f,  0.001f,0.45f,0.f,0.2f,  0.001f,0.05f,0.95f,0.06f, 0.f, 0.f,0.f,0.f,0.78f },
            { "Reese Wide",    0, 0, 0, 0, 1,0, 0.72f,0.78f,0.42f, 32.f, 310.f,0.58f,0.48f,0.42f,0.52f, 0.006f,0.38f,0.28f,0.32f, 0.003f,0.42f,0.86f,0.28f, 0.04f,0.45f,0.3f,6.f,0.69f },
            { "Sync Acid",     0, 0, 0, 0, 0,1, 0.9f,0.2f,0.15f, 2.f,  880.f,0.74f,0.7f,0.92f,0.58f, 0.001f,0.04f,0.f,0.07f, 0.001f,0.12f,0.72f,0.09f, 0.f,14.f,0.5f,5.f,0.62f },
            { "FM Knock",      1,-1, 1, 0, 0,0, 0.65f,0.55f,0.7f, 14.f, 420.f,0.36f,0.58f,0.75f,0.45f, 0.001f,0.1f,0.f,0.12f, 0.001f,0.14f,0.55f,0.1f,  0.f, 6.f,0.12f,0.f,0.7f },
        };
        static_assert (sizeof (factory) / sizeof (factory[0]) == count);
        const int n = (int) (sizeof (factory) / sizeof (factory[0]));
        return factory[juce::jlimit (0, n - 1, index)];
    }

    inline void apply (juce::AudioProcessorValueTreeState& apvts, int index)
    {
        apply (apvts, get (index));
        const auto name = juce::String (get (index).name);
        if (name == "Sync Acid")
        {
            setParam (apvts, "osc2Sync", 1.f);
            setParam (apvts, "osc2Width", 0.22f);
        }
        else if (name == "Click Sub")
        {
            setParam (apvts, "noiseLevel", 0.38f);
        }
        else if (name == "Vel Pluck")
        {
            setParam (apvts, "velToAmp", 0.95f);
            setParam (apvts, "velToFilt", 0.45f);
        }
    }
}
