#pragma once
#include <array>
#include <complex>
#include <vector>
#include "PluginProcessor.h"
#include "AppFonts.h"
#include "Theme.h"

//==============================================================================
// Custom widgets used by the editor: value formatting, segmented selectors, prev/next buttons
// and the three live displays (oscilloscope + meter, filter response, envelope shape).
//==============================================================================
namespace widgets
{
    //--------------------------------------------------------------------------
    // Parameter value <-> text
    inline juce::String formatParam (const juce::RangedAudioParameter& p, double v)
    {
        const auto unit = p.getLabel();

        if (unit == "Hz")
        {
            if (v >= 1000.0) return juce::String (v / 1000.0, v >= 10000.0 ? 1 : 2) + " kHz";
            if (v >= 100.0)  return juce::String (juce::roundToInt (v)) + " Hz";
            if (v >= 10.0)   return juce::String (v, 1) + " Hz";
            return juce::String (v, 2) + " Hz";
        }
        if (unit == "s")
        {
            if (v < 1.0) return juce::String (juce::roundToInt (v * 1000.0)) + " ms";
            return juce::String (v, 2) + " s";
        }
        if (unit == "ct")
        {
            const bool whole = std::abs (v - std::round (v)) < 0.05;
            return (whole ? juce::String (juce::roundToInt (v)) : juce::String (v, 1)) + " ct";
        }
        if (dynamic_cast<const juce::AudioParameterInt*> (&p) != nullptr)
        {
            const int i = juce::roundToInt (v);
            return (i > 0 ? "+" : "") + juce::String (i);
        }

        const int pct = juce::roundToInt (v * 100.0);
        return (pct > 0 && p.getNormalisableRange().start < 0.f ? "+" : "") + juce::String (pct) + "%";
    }

    inline double parseParam (const juce::RangedAudioParameter& p, const juce::String& t)
    {
        const auto unit = p.getLabel();
        const auto lower = t.trim().toLowerCase();
        double d = lower.retainCharacters ("0123456789.,-+").replaceCharacter (',', '.').getDoubleValue();

        if (unit == "Hz")      { if (lower.contains ("k"))  d *= 1000.0; }
        else if (unit == "s")  { if (lower.contains ("ms")) d /= 1000.0; }
        else if (unit == "ct" || dynamic_cast<const juce::AudioParameterInt*> (&p) != nullptr) {}
        else                   d /= 100.0;

        return d;
    }

    //--------------------------------------------------------------------------
    inline void strokeWaveIcon (juce::Graphics& g, juce::Rectangle<float> r, const juce::String& name,
                                juce::Colour colour, float thickness)
    {
        auto pt = [&] (float x, float y) { return juce::Point<float> (r.getX() + x * r.getWidth(), r.getY() + y * r.getHeight()); };
        juce::Path p;
        auto poly = [&] (std::initializer_list<std::pair<float, float>> pts)
        {
            bool first = true;
            for (auto& [x, y] : pts)
            {
                if (first) p.startNewSubPath (pt (x, y)); else p.lineTo (pt (x, y));
                first = false;
            }
        };

        if (name == "Saw")           poly ({ { 0.f, 0.9f }, { 0.5f, 0.1f }, { 0.5f, 0.9f }, { 1.f, 0.1f } });
        else if (name == "Square")   poly ({ { 0.f, 0.85f }, { 0.f, 0.15f }, { 0.5f, 0.15f }, { 0.5f, 0.85f }, { 1.f, 0.85f }, { 1.f, 0.15f } });
        else if (name == "Triangle") poly ({ { 0.f, 0.5f }, { 0.25f, 0.1f }, { 0.75f, 0.9f }, { 1.f, 0.5f } });
        else if (name == "Sine")
        {
            constexpr int steps = 24;
            for (int i = 0; i <= steps; ++i)
            {
                const float t = (float) i / (float) steps;
                const auto q = pt (t, 0.5f - 0.4f * std::sin (juce::MathConstants<float>::twoPi * t));
                if (i == 0) p.startNewSubPath (q); else p.lineTo (q);
            }
        }
        else                         poly ({ { 0.f, 0.65f }, { 0.22f, 0.65f }, { 0.22f, 0.2f }, { 0.46f, 0.2f }, { 0.46f, 0.85f },
                                             { 0.72f, 0.85f }, { 0.72f, 0.4f }, { 1.f, 0.4f } });

        g.setColour (colour);
        g.strokePath (p, juce::PathStrokeType (thickness, juce::PathStrokeType::mitered, juce::PathStrokeType::rounded));
    }

    //==========================================================================
    // Pill-shaped selector bound to a choice parameter. Shows waveform icons for wave names,
    // upper-case text for everything else.
    class SegmentedChoice : public juce::Component, public juce::TooltipClient
    {
    public:
        SegmentedChoice (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramId, juce::Colour accentColour)
            : accent (accentColour)
        {
            auto* choice = dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter (paramId));
            jassert (choice != nullptr);
            if (choice == nullptr)
                return;

            labels = choice->choices;
            paramName = choice->getName (40);
            iconStyle = labels.contains ("Saw") || labels.contains ("Square") || labels.contains ("Triangle");

            attachment = std::make_unique<juce::ParameterAttachment> (*choice, [this] (float v)
            {
                current = juce::roundToInt (v);
                repaint();
            });
            attachment->sendInitialUpdate();
        }

        int getNumSegments() const noexcept { return labels.size(); }
        bool usesIcons() const noexcept { return iconStyle; }

        juce::String getTooltip() override
        {
            return juce::isPositiveAndBelow (hover, labels.size()) ? paramName + ": " + labels[hover] : paramName;
        }

        void paint (juce::Graphics& g) override
        {
            const int n = labels.size();
            if (n == 0) return;

            auto b = getLocalBounds().toFloat();
            const float radius = b.getHeight() * 0.32f;
            theme::paintInset (g, b, radius);

            const float segW = b.getWidth() / (float) n;
            for (int i = 0; i < n; ++i)
            {
                auto seg = juce::Rectangle<float> (b.getX() + segW * (float) i, b.getY(), segW, b.getHeight()).reduced (2.5f);
                const bool on = i == current;
                const bool over = i == hover;

                if (on)
                {
                    g.setColour (accent.withAlpha (0.20f));
                    g.fillRoundedRectangle (seg, radius - 2.f);
                    g.setColour (accent.withAlpha (0.55f));
                    g.drawRoundedRectangle (seg.reduced (0.5f), radius - 2.f, 1.f);
                }
                else if (over)
                {
                    g.setColour (juce::Colours::white.withAlpha (0.06f));
                    g.fillRoundedRectangle (seg, radius - 2.f);
                }

                const auto col = on ? accent : (over ? theme::text() : theme::dim());
                if (iconStyle)
                {
                    auto icon = juce::Rectangle<float> (seg.getWidth() * 0.46f, seg.getHeight() * 0.42f).withCentre (seg.getCentre());
                    strokeWaveIcon (g, icon, labels[i], col, juce::jmax (1.5f, b.getHeight() * 0.06f));
                }
                else
                {
                    g.setColour (col);
                    g.setFont (AppFonts::title (b.getHeight() * 0.30f, 0.06f));
                    g.drawText (labels[i].toUpperCase(), seg.toNearestInt(), juce::Justification::centred, false);
                }
            }
        }

        void mouseMove (const juce::MouseEvent& e) override { setHover (indexAt (e.position)); }
        void mouseExit (const juce::MouseEvent&) override   { setHover (-1); }

        void mouseDown (const juce::MouseEvent& e) override
        {
            const int i = indexAt (e.position);
            if (attachment != nullptr && i >= 0)
                attachment->setValueAsCompleteGesture ((float) i);
        }

    private:
        int indexAt (juce::Point<float> p) const
        {
            const int n = labels.size();
            if (n == 0 || getWidth() <= 0) return -1;
            return juce::jlimit (0, n - 1, (int) (p.x / ((float) getWidth() / (float) n)));
        }

        void setHover (int i)
        {
            if (hover == i) return;
            hover = i;
            repaint();
        }

        juce::Colour accent;
        juce::StringArray labels;
        juce::String paramName;
        bool iconStyle = false;
        int current = 0, hover = -1;
        std::unique_ptr<juce::ParameterAttachment> attachment;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SegmentedChoice)
    };

    //==========================================================================
    class ChevronButton : public juce::Button
    {
    public:
        explicit ChevronButton (bool pointsRight) : juce::Button (pointsRight ? "next" : "prev"), right (pointsRight) {}

        void paintButton (juce::Graphics& g, bool highlighted, bool down) override
        {
            auto r = getLocalBounds().toFloat().reduced (0.5f);
            const float radius = juce::jmin (9.f, r.getHeight() * 0.28f);

            auto fill = theme::raised();
            if (down) fill = fill.brighter (0.18f);
            else if (highlighted) fill = fill.brighter (0.09f);
            g.setColour (fill);
            g.fillRoundedRectangle (r, radius);
            g.setColour (highlighted ? theme::borderHi().brighter (0.2f) : theme::border());
            g.drawRoundedRectangle (r, radius, 1.f);

            const auto c = r.getCentre();
            const float a = r.getHeight() * 0.15f, dx = right ? 1.f : -1.f;
            juce::Path p;
            p.startNewSubPath (c.x - dx * a * 0.6f, c.y - a);
            p.lineTo (c.x + dx * a * 0.6f, c.y);
            p.lineTo (c.x - dx * a * 0.6f, c.y + a);
            g.setColour (highlighted ? theme::text() : theme::dim());
            g.strokePath (p, juce::PathStrokeType (1.9f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

    private:
        bool right;
    };

    //==========================================================================
    // Oscilloscope (trigger-stabilised, auto-gain) + output level meter.
    class ScopeView : public juce::Component, private juce::Timer
    {
    public:
        explicit ScopeView (MnBazzProcessor& p) : proc (p)
        {
            raw.fill (0.f);
            disp.fill (0.f);
            setInterceptsMouseClicks (false, false);
            startTimerHz (30);
        }

        void paint (juce::Graphics& g) override
        {
            auto b = getLocalBounds().toFloat();
            auto meterR = b.removeFromRight (juce::jmax (6.f, b.getWidth() * 0.05f));
            b.removeFromRight (juce::jmax (4.f, b.getWidth() * 0.04f));

            theme::paintInset (g, b, 10.f);
            auto inner = b.reduced (8.f, 10.f);
            const float cy = inner.getCentreY();

            g.setColour (juce::Colours::white.withAlpha (0.045f));
            for (int i = 1; i < 4; ++i)
                g.fillRect (inner.getX() + inner.getWidth() * (float) i / 4.f - 0.5f, inner.getY(), 1.f, inner.getHeight());
            g.setColour (juce::Colours::white.withAlpha (0.08f));
            g.fillRect (inner.getX(), cy - 0.5f, inner.getWidth(), 1.f);

            juce::Path wave;
            const int n = (int) disp.size();
            const float amp = inner.getHeight() * 0.5f;
            for (int i = 0; i < n; ++i)
            {
                const float x = inner.getX() + inner.getWidth() * (float) i / (float) (n - 1);
                const float y = cy - juce::jlimit (-1.f, 1.f, disp[(size_t) i] * gain) * amp;
                if (i == 0) wave.startNewSubPath (x, y); else wave.lineTo (x, y);
            }

            const auto col = active ? theme::amber() : theme::faint();
            if (active)
            {
                g.setColour (col.withAlpha (0.18f));
                g.strokePath (wave, juce::PathStrokeType (5.f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
            }
            g.setColour (col);
            g.strokePath (wave, juce::PathStrokeType (1.7f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

            // level meter
            const float radius = meterR.getWidth() * 0.5f;
            g.setColour (theme::well());
            g.fillRoundedRectangle (meterR, radius);
            g.setColour (juce::Colours::black.withAlpha (0.45f));
            g.drawRoundedRectangle (meterR.reduced (0.5f), radius, 1.f);

            const float db = 20.f * std::log10 (juce::jmax (meter, 1.0e-4f));
            const float norm = juce::jlimit (0.f, 1.f, (db + 48.f) / 48.f);
            if (norm > 0.f)
            {
                auto fill = meterR.reduced (1.5f);
                fill = fill.withTop (fill.getBottom() - fill.getHeight() * norm);
                g.setGradientFill (juce::ColourGradient::vertical (theme::coral(), meterR.getY(), theme::amber(), meterR.getY() + meterR.getHeight() * 0.45f));
                g.fillRoundedRectangle (fill, radius - 1.f);
            }
        }

    private:
        void timerCallback() override
        {
            const int n = proc.copyScopeSamples (raw.data(), (int) raw.size());
            meter = juce::jmax (proc.getOutputLevel(), meter * 0.85f);

            // rising zero-crossings: first one in the first half, the second gives the period
            int c1 = -1, c2 = -1;
            for (int i = 1; i < n - 32; ++i)
            {
                if (raw[(size_t) i - 1] <= 0.f && raw[(size_t) i] > 0.f)
                {
                    if (c1 < 0)
                    {
                        if (i >= n / 2) break;
                        c1 = i;
                    }
                    else if (i - c1 > 6)
                    {
                        c2 = i;
                        break;
                    }
                }
            }

            int start = juce::jmax (0, c1);
            int count = juce::jmin (1024, n - start);
            if (c1 >= 0 && c2 > c1)
            {
                const int period = c2 - c1;
                count = (c1 + 2 * period < n) ? 2 * period : period;
            }

            float peak = 0.f;
            const int pts = (int) disp.size();
            for (int i = 0; i < pts; ++i)
            {
                const float pos = (float) start + (float) count * (float) i / (float) (pts - 1);
                const int i0 = juce::jlimit (0, n - 1, (int) pos);
                const int i1 = juce::jmin (n - 1, i0 + 1);
                const float fr = pos - (float) i0;
                disp[(size_t) i] = raw[(size_t) i0] * (1.f - fr) + raw[(size_t) i1] * fr;
                peak = juce::jmax (peak, std::abs (disp[(size_t) i]));
            }

            const bool nowActive = peak > 0.003f;
            const float target = nowActive ? 0.85f / juce::jmax (peak, 0.12f) : 1.f;
            gain += (target - gain) * 0.25f;

            if (nowActive || active || meter > 1.0e-3f)
                repaint();
            active = nowActive;
        }

        MnBazzProcessor& proc;
        std::array<float, MnBazzProcessor::scopeBufferSize> raw;
        std::array<float, 192> disp;
        float gain = 1.f, meter = 0.f;
        bool active = false;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScopeView)
    };

    //==========================================================================
    // Frequency response of the 4-pole ladder with the current cutoff / resonance; the faint
    // second curve shows where the envelope pushes the cutoff to.
    class FilterCurveView : public juce::Component, private juce::Timer
    {
    public:
        explicit FilterCurveView (MnBazzProcessor& p, juce::Colour accentColour) : proc (p), accent (accentColour)
        {
            setInterceptsMouseClicks (false, false);
            startTimerHz (24);
        }

        void paint (juce::Graphics& g) override
        {
            auto b = getLocalBounds().toFloat();
            theme::paintInset (g, b, 10.f);

            auto plot = b.reduced (10.f, 10.f);
            plot.removeFromBottom (12.f);

            const auto v = readValues();
            const float cutoff = v[0], k = v[1] * 3.95f, envAmt = v[2];

            auto xOf = [&] (float f) { return plot.getX() + plot.getWidth() * std::log (f / 20.f) / std::log (1000.f); };
            // not clamped: the curve simply leaves the plot at the bottom (clipped below)
            auto yOf = [&] (float db) { return plot.getBottom() - plot.getHeight() * (db - dbMin) / (dbMax - dbMin); };

            // grid + labels
            g.setFont (AppFonts::small (juce::jmax (8.f, b.getHeight() * 0.075f)));
            for (auto [f, txt] : { std::pair<float, const char*> { 100.f, "100" }, { 1000.f, "1k" }, { 10000.f, "10k" } })
            {
                const float x = xOf (f);
                g.setColour (juce::Colours::white.withAlpha (0.05f));
                g.fillRect (x - 0.5f, plot.getY(), 1.f, plot.getHeight());
                g.setColour (theme::faint());
                g.drawText (txt, juce::Rectangle<float> (x - 20.f, plot.getBottom() + 1.f, 40.f, 12.f), juce::Justification::centred);
            }
            g.setColour (juce::Colours::white.withAlpha (0.08f));
            g.fillRect (plot.getX(), yOf (0.f) - 0.5f, plot.getWidth(), 1.f);

            auto makeCurve = [&] (float fc)
            {
                juce::Path p;
                const int steps = juce::jmax (8, (int) (plot.getWidth() / 2.f));
                for (int i = 0; i <= steps; ++i)
                {
                    const float t = (float) i / (float) steps;
                    const float f = 20.f * std::pow (1000.f, t);
                    const float x = plot.getX() + plot.getWidth() * t;
                    const float y = yOf (responseDb (f, fc, k));
                    if (i == 0) p.startNewSubPath (x, y); else p.lineTo (x, y);
                }
                return p;
            };

            juce::Graphics::ScopedSaveState clipState (g);
            g.reduceClipRegion (plot.expanded (2.f, 3.f).toNearestInt());

            if (std::abs (envAmt) > 0.03f)
            {
                const float fcPeak = juce::jlimit (20.f, 20000.f, cutoff * std::pow (2.f, envAmt * 5.f));
                g.setColour (accent.withAlpha (0.30f));
                g.strokePath (makeCurve (fcPeak), juce::PathStrokeType (1.2f));
            }

            auto curve = makeCurve (cutoff);
            juce::Path fill (curve);
            fill.lineTo (plot.getRight(), plot.getBottom());
            fill.lineTo (plot.getX(), plot.getBottom());
            fill.closeSubPath();
            g.setGradientFill (juce::ColourGradient::vertical (accent.withAlpha (0.30f), plot.getY(), accent.withAlpha (0.02f), plot.getBottom()));
            g.fillPath (fill);

            g.setColour (accent.withAlpha (0.2f));
            g.strokePath (curve, juce::PathStrokeType (4.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
            g.setColour (accent);
            g.strokePath (curve, juce::PathStrokeType (1.8f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

            theme::paintGlowDot (g, { xOf (cutoff), yOf (responseDb (cutoff, cutoff, k)) }, 2.6f, theme::text());
        }

    private:
        static constexpr float dbMin = -42.f, dbMax = 26.f;

        static float responseDb (float f, float fc, float k)
        {
            const std::complex<float> G = 1.f / std::complex<float> (1.f, f / fc);
            const auto G2 = G * G;
            const auto G4 = G2 * G2;
            const auto H = G4 / (1.f + k * G4);
            const float mag = std::abs (H) * (1.f + 0.6f * k);
            return 20.f * std::log10 (juce::jmax (mag, 1.0e-5f));
        }

        std::array<float, 3> readValues() const
        {
            auto get = [this] (const char* id) { return proc.apvts.getRawParameterValue (id)->load(); };
            return { get ("cutoff"), get ("res"), get ("envAmt") };
        }

        void timerCallback() override
        {
            const auto v = readValues();
            if (v != last)
            {
                last = v;
                repaint();
            }
        }

        MnBazzProcessor& proc;
        juce::Colour accent;
        std::array<float, 3> last {};

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterCurveView)
    };

    //==========================================================================
    // ADSR shape for the envelope cards (time segments are compressed so every stage stays visible).
    class EnvelopeView : public juce::Component, private juce::Timer
    {
    public:
        EnvelopeView (MnBazzProcessor& p, const juce::String& prefix, juce::Colour accentColour)
            : proc (p), ids { prefix + "A", prefix + "D", prefix + "S", prefix + "R" }, accent (accentColour)
        {
            setInterceptsMouseClicks (false, false);
            startTimerHz (24);
        }

        void paint (juce::Graphics& g) override
        {
            auto b = getLocalBounds().toFloat();
            theme::paintInset (g, b, 10.f);

            auto plot = b.reduced (12.f, 12.f);
            plot.removeFromBottom (12.f);

            const auto v = readValues();
            auto seg = [] (float t, float maxT) { return 0.16f + 0.84f * std::pow (juce::jlimit (0.f, 1.f, t / maxT), 0.38f); };
            const float wa = seg (v[0], 5.f), wd = seg (v[1], 5.f), wh = 0.34f, wr = seg (v[3], 8.f);
            const float sx = plot.getWidth() / (wa + wd + wh + wr);
            const float base = plot.getBottom(), top = plot.getY();
            auto yOf = [&] (float level) { return base - level * (base - top); };

            // grid
            g.setColour (juce::Colours::white.withAlpha (0.045f));
            for (float lv : { 0.5f, 1.f })
                g.fillRect (plot.getX(), yOf (lv) - 0.5f, plot.getWidth(), 1.f);

            const juce::Point<float> p0 { plot.getX(), base },
                                     p1 { p0.x + wa * sx, top },
                                     p2 { p1.x + wd * sx, yOf (v[2]) },
                                     p3 { p2.x + wh * sx, yOf (v[2]) },
                                     p4 { p3.x + wr * sx, base };

            juce::Path line;
            line.startNewSubPath (p0);
            for (auto p : { p1, p2, p3, p4 }) line.lineTo (p);

            juce::Path fill (line);
            fill.closeSubPath();
            g.setGradientFill (juce::ColourGradient::vertical (accent.withAlpha (0.30f), top, accent.withAlpha (0.02f), base));
            g.fillPath (fill);

            // stage separators + letters
            g.setFont (AppFonts::title (juce::jmax (8.f, b.getHeight() * 0.075f), 0.05f));
            const float xs[] { p0.x, p1.x, p2.x, p3.x, p4.x };
            const char* letters[] { "A", "D", "S", "R" };
            for (int i = 0; i < 4; ++i)
            {
                if (i > 0)
                {
                    g.setColour (juce::Colours::white.withAlpha (0.05f));
                    g.fillRect (xs[i] - 0.5f, plot.getY(), 1.f, plot.getHeight());
                }
                g.setColour (theme::faint());
                g.drawText (letters[i], juce::Rectangle<float> (xs[i], base + 1.f, xs[i + 1] - xs[i], 12.f), juce::Justification::centred);
            }

            g.setColour (accent.withAlpha (0.2f));
            g.strokePath (line, juce::PathStrokeType (4.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
            g.setColour (accent);
            g.strokePath (line, juce::PathStrokeType (1.8f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

            for (auto p : { p1, p2, p3 })
            {
                g.setColour (theme::cardBottom());
                g.fillEllipse (juce::Rectangle<float> (7.f, 7.f).withCentre (p));
                g.setColour (accent);
                g.drawEllipse (juce::Rectangle<float> (7.f, 7.f).withCentre (p), 1.6f);
            }
        }

    private:
        std::array<float, 4> readValues() const
        {
            std::array<float, 4> v {};
            for (size_t i = 0; i < v.size(); ++i)
                v[i] = proc.apvts.getRawParameterValue (ids[i])->load();
            return v;
        }

        void timerCallback() override
        {
            const auto v = readValues();
            if (v != last)
            {
                last = v;
                repaint();
            }
        }

        MnBazzProcessor& proc;
        std::array<juce::String, 4> ids;
        juce::Colour accent;
        std::array<float, 4> last {};

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvelopeView)
    };
}
