#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <cmath>
#include <array>
#include <vector>

struct VoiceParams
{
    int   osc1Wave = 0, osc2Wave = 0;
    int   osc1Oct = 0, osc2Oct = 0;
    float osc2Detune = 0.f;
    float osc1Level = 1.f, osc2Level = 0.f, subLevel = 0.f;
    float osc2Width = 0.5f;
    bool  osc2Sync = false;
    float cutoff = 800.f, res = 0.f, drive = 0.f, envAmt = 0.f, keytrack = 0.f;
    float fEnvToRes = 0.f;
    float glide = 0.f;
    float lfoRate = 4.f; int lfoShape = 0;
    float lfoToCutoff = 0.f, lfoToPitch = 0.f, lfoToAmp = 0.f;
    float modToCutoff = 0.35f;
    float modToPitch = 0.f;
    float velToAmp = 0.65f;
    float velToFilt = 0.25f;
    float noiseLevel = 0.f;
    float master = 0.7f;
    bool  legato = true;
    juce::ADSR::Parameters fEnv, aEnv;
};

class BuzzVoice
{
public:
    void prepare (double sampleRate)
    {
        sr = sampleRate;
        fEnv.setSampleRate (sr);
        aEnv.setSampleRate (sr);

        sCutoff.reset (sr, 0.055);
        sRes.reset (sr, 0.035);
        sDrive.reset (sr, 0.035);
        sEnvAmt.reset (sr, 0.040);
        sKey.reset (sr, 0.030);
        sLvl1.reset (sr, 0.040);
        sLvl2.reset (sr, 0.040);
        sSub.reset (sr, 0.040);
        sDetune.reset (sr, 0.030);
        sLfoCut.reset (sr, 0.035);
        sLfoPitch.reset (sr, 0.030);
        sLfoAmp.reset (sr, 0.035);
        sMaster.reset (sr, 0.045);
        sLfoRate.reset (sr, 0.040);
        sModCut.reset (sr, 0.035);
        sModPitch.reset (sr, 0.030);
        sNoise.reset (sr, 0.025);
        sFEtoRes.reset (sr, 0.040);
        sWidth.reset (sr, 0.030);
        sVelAmp.reset (sr, 0.020);
        sVelFilt.reset (sr, 0.020);
        sBend.reset (sr, 0.015);
        sMod.reset (sr, 0.020);

        firstParams = true;
        reset();
    }

    void reset()
    {
        fEnv.reset(); aEnv.reset();
        for (auto& s : st) s = 0.f;
        y4 = 0.f; notes.clear();
        ph1 = ph2 = phSub = phLfo = 0.f;
        pitchBend = 0.f; modWheel = 0.f;
        noteVelocity = 1.f;
        sBend.setCurrentAndTargetValue (0.f);
        sMod.setCurrentAndTargetValue (0.f);
        dcState = 0.f;
        syncActiveOsc();
    }

    void setParams (const VoiceParams& p)
    {
        P = p;
        fEnv.setParameters (p.fEnv);
        aEnv.setParameters (p.aEnv);

        const float targets[] { std::log2 (p.cutoff), p.res, p.drive, p.envAmt, p.keytrack,
                                p.osc1Level, p.osc2Level, p.subLevel, p.osc2Detune,
                                p.lfoToCutoff, p.lfoToPitch, p.lfoToAmp, p.modToCutoff, p.modToPitch,
                                p.noiseLevel, p.fEnvToRes, p.osc2Width, p.velToAmp, p.velToFilt, p.master, p.lfoRate };
        auto sms = smoothers();
        for (size_t i = 0; i < sms.size(); ++i)
        {
            if (firstParams) sms[i]->setCurrentAndTargetValue (targets[i]);
            else             sms[i]->setTargetValue (targets[i]);
        }
        firstParams = false;

        if (! playing)
            syncActiveOsc();

        glideCoef = p.glide < 0.001f ? 1.f
                  : 1.f - std::exp (-1.f / (p.glide * (float) sr * 0.35f));
    }

    void noteOn (int note, float velocity = 1.f)
    {
        noteVelocity = juce::jlimit (0.f, 1.f, velocity);
        const bool wasHeld = ! notes.empty();
        std::erase (notes, note);
        notes.push_back (note);
        targetNote = (float) note;
        if (! wasHeld || ! playing)
        {
            curNote = (P.glide > 0.001f && playing) ? curNote : targetNote;
            fEnv.noteOn(); aEnv.noteOn(); playing = true;
        }
        else if (! P.legato)
        {
            fEnv.noteOn(); aEnv.noteOn();
        }
        if (P.glide < 0.001f) curNote = targetNote;
        syncActiveOsc();
    }

    void noteOff (int note)
    {
        std::erase (notes, note);
        if (notes.empty())
        {
            fEnv.noteOff(); aEnv.noteOff();
        }
        else
        {
            targetNote = (float) notes.back();
            if (P.glide < 0.001f) curNote = targetNote;
            if (! P.legato) { fEnv.noteOn(); aEnv.noteOn(); }
        }
    }

    void allNotesOff() { notes.clear(); fEnv.noteOff(); aEnv.noteOff(); }
    void setPitchBend (float semis) { pitchBend = semis; sBend.setTargetValue (semis); }
    void setModWheel (float v)      { modWheel = v; sMod.setTargetValue (v); }

    float process()
    {
        if (! aEnv.isActive())
        {
            playing = false;
            for (auto* sm : smoothers()) sm->setCurrentAndTargetValue (sm->getTargetValue());
            syncActiveOsc();
            return 0.f;
        }

        const float cutoffLog2 = sCutoff.getNextValue(), res = sRes.getNextValue(), drive = sDrive.getNextValue(),
                    envAmt = sEnvAmt.getNextValue(), keytrack = sKey.getNextValue(),
                    lvl1 = sLvl1.getNextValue(), lvl2 = sLvl2.getNextValue(), lvlSub = sSub.getNextValue(),
                    detune = sDetune.getNextValue(), lfoCut = sLfoCut.getNextValue(),
                    lfoPitch = sLfoPitch.getNextValue(), lfoAmp = sLfoAmp.getNextValue(),
                    modToCut = sModCut.getNextValue(), modToPitch = sModPitch.getNextValue(),
                    noiseLvl = sNoise.getNextValue(), fEnvToRes = sFEtoRes.getNextValue(),
                    osc2Width = sWidth.getNextValue(),
                    velToAmp = sVelAmp.getNextValue(), velToFilt = sVelFilt.getNextValue(),
                    master = sMaster.getNextValue();
        const float lfoHz = sLfoRate.getNextValue();
        const float bend = sBend.getNextValue();
        const float modAmt = juce::jlimit (0.f, 1.f, sMod.getNextValue());

        curNote += (targetNote - curNote) * glideCoef;

        phLfo += lfoHz / (float) sr;
        if (phLfo >= 1.f) { phLfo -= 1.f; shVal = rnd.nextFloat() * 2.f - 1.f; }
        float lfo = 0.f;
        switch (activeLfoShape)
        {
            case 0: lfo = 2.f * std::abs (2.f * phLfo - 1.f) - 1.f; break;
            case 1: lfo = phLfo < 0.5f ? 1.f : -1.f; break;
            case 2: lfo = 2.f * phLfo - 1.f; break;
            case 4: lfo = std::sin (juce::MathConstants<float>::twoPi * phLfo); break;
            default: lfo = shVal; break;
        }

        const float velAmp = (1.f - velToAmp) + velToAmp * noteVelocity;
        const float velFiltOct = (noteVelocity - 0.5f) * velToFilt * 1.5f;

        const float semis = curNote + bend + modAmt * modToPitch + lfo * lfoPitch * 0.01f;
        const float f0 = 440.f * std::pow (2.f, (semis - 69.f) / 12.f);
        const float f1 = f0 * std::pow (2.f, (float) activeOsc1Oct);
        const float f2 = f0 * std::pow (2.f, (float) activeOsc2Oct + detune / 1200.f);

        const float prevPh1 = ph1;
        float o = lvl1 * osc (ph1, f1, activeOsc1Wave, 0.5f);
        if (activeOsc2Sync && ph1 < prevPh1)
            ph2 = 0.f;
        o += lvl2 * osc (ph2, f2, activeOsc2Wave, osc2Width)
           + lvlSub * osc (phSub, f1 * 0.5f, 1, 0.5f);

        if (noiseLvl > 1.0e-5f)
            o += (rnd.nextFloat() * 2.f - 1.f) * noiseLvl * 0.22f;

        o *= 0.45f;

        const float fe = fEnv.getNextSample();
        const float oct = envAmt * 5.f * fe
                        + lfo * lfoCut * 2.f
                        + modAmt * modToCut * 2.5f
                        + keytrack * (curNote - 60.f) / 12.f
                        + velFiltOct;
        float fc = std::exp2 (cutoffLog2 + oct);
        fc = juce::jlimit (15.f, 0.45f * (float) sr, fc);
        const float g = std::tan (juce::MathConstants<float>::pi * fc / (2.f * (float) sr));
        const float G = g / (1.f + g);
        const float resEff = juce::jlimit (0.f, 1.f, res + fe * fEnvToRes);
        const float k = resEff * 3.95f;
        const float drv = 1.f + drive * 7.f;

        const float ladderIn = drive > 0.45f ? std::tanh (o * (1.f + drive * 0.5f)) : o;
        float y = 0.f;
        for (int i = 0; i < (drive > 0.45f ? 2 : 1); ++i)
            y += ladder (ladderIn, G, k, drv);
        y *= (drive > 0.45f ? 0.25f : 0.5f) * (1.f + 0.6f * k);

        float a = aEnv.getNextSample() * velAmp;
        a *= 1.f + lfo * lfoAmp * 0.85f;
        a = juce::jlimit (0.f, 1.2f, a);

        const float out = std::tanh (y * a * master * 1.5f);

        dcState += (out - dcState) * 0.0025f;
        return out - dcState;
    }

private:
    void syncActiveOsc()
    {
        activeOsc1Wave = P.osc1Wave;
        activeOsc2Wave = P.osc2Wave;
        activeOsc1Oct  = P.osc1Oct;
        activeOsc2Oct  = P.osc2Oct;
        activeLfoShape = P.lfoShape;
        activeOsc2Sync = P.osc2Sync;
    }

    static float blep (float t, float dt)
    {
        if (t < dt)        { t /= dt;           return t + t - t * t - 1.f; }
        if (t > 1.f - dt)  { t = (t - 1.f) / dt; return t * t + t + t + 1.f; }
        return 0.f;
    }

    float osc (float& ph, float freq, int wave, float pulseWidth)
    {
        const float dt = juce::jlimit (1e-5f, 0.45f, freq / (float) sr);
        float v = 0.f;
        const float pw = juce::jlimit (0.08f, 0.92f, pulseWidth);
        switch (wave)
        {
            case 0: v = 2.f * ph - 1.f - blep (ph, dt); break;
            case 1:
            {
                float p2 = ph + pw; if (p2 >= 1.f) p2 -= 1.f;
                v = (ph < pw ? 1.f : -1.f) + blep (ph, dt) - blep (p2, dt);
                break;
            }
            default: v = 2.f * std::abs (2.f * ph - 1.f) - 1.f; break;
        }
        ph += dt;
        if (ph >= 1.f) ph -= 1.f;
        return v;
    }

    float ladder (float x, float G, float k, float drv)
    {
        const float u = std::tanh (x * drv - k * y4);
        float in = u;
        for (int i = 0; i < 4; ++i)
        {
            const float v = G * (in - st[i]);
            const float y = v + st[i];
            st[i] = y + v;
            in = y;
            if (i == 1) in = std::tanh (in);
        }
        y4 = in;
        return in;
    }

    using Smoothed = juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>;
    std::array<Smoothed*, 21> smoothers()
    {
        return { &sCutoff, &sRes, &sDrive, &sEnvAmt, &sKey, &sLvl1, &sLvl2, &sSub, &sDetune,
                 &sLfoCut, &sLfoPitch, &sLfoAmp, &sModCut, &sModPitch, &sNoise, &sFEtoRes, &sWidth,
                 &sVelAmp, &sVelFilt, &sMaster, &sLfoRate };
    }

    Smoothed sCutoff, sRes, sDrive, sEnvAmt, sKey, sLvl1, sLvl2, sSub, sDetune;
    Smoothed sLfoCut, sLfoPitch, sLfoAmp, sModCut, sModPitch, sNoise, sFEtoRes, sWidth;
    Smoothed sVelAmp, sVelFilt, sMaster, sLfoRate;
    Smoothed sBend, sMod;
    bool firstParams = true;
    double sr = 44100.0;
    VoiceParams P;
    int activeOsc1Wave = 0, activeOsc2Wave = 0, activeOsc1Oct = 0, activeOsc2Oct = 0, activeLfoShape = 0;
    bool activeOsc2Sync = false;
    juce::ADSR fEnv, aEnv;
    juce::Random rnd;
    std::vector<int> notes;
    float ph1 = 0.f, ph2 = 0.f, phSub = 0.f, phLfo = 0.f, shVal = 0.f;
    float curNote = 60.f, targetNote = 60.f, glideCoef = 1.f;
    float pitchBend = 0.f, modWheel = 0.f, noteVelocity = 1.f;
    float dcState = 0.f;
    float st[4] {}, y4 = 0.f;
    bool playing = false;
};
