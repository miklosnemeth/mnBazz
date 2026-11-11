#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
// mnBazz visual theme: deep blue-black surfaces, one accent colour per signal-flow stage.
//==============================================================================
namespace theme
{
    // surfaces
    inline juce::Colour bgTop()     { return juce::Colour (0xff12151d); }
    inline juce::Colour bgBottom()  { return juce::Colour (0xff07080b); }
    inline juce::Colour cardTop()   { return juce::Colour (0xff171b25); }
    inline juce::Colour cardBottom(){ return juce::Colour (0xff12151d); }
    inline juce::Colour well()      { return juce::Colour (0xff0a0c11); }
    inline juce::Colour raised()    { return juce::Colour (0xff1e2330); }
    inline juce::Colour border()    { return juce::Colour (0xff262c3a); }
    inline juce::Colour borderHi()  { return juce::Colour (0xff3a4254); }

    // text
    inline juce::Colour text()      { return juce::Colour (0xffeff2f8); }
    inline juce::Colour dim()       { return juce::Colour (0xff98a2b6); }
    inline juce::Colour faint()     { return juce::Colour (0xff5b6579); }

    // accents (one per stage)
    inline juce::Colour amber()     { return juce::Colour (0xffffb340); }   // oscillators / output
    inline juce::Colour coral()     { return juce::Colour (0xffff6f5e); }   // filter
    inline juce::Colour cyan()      { return juce::Colour (0xff3ad0e8); }   // envelopes
    inline juce::Colour violet()    { return juce::Colour (0xffa393ff); }   // modulation

    constexpr float cardRadius = 14.f;

    //--------------------------------------------------------------------------
    inline void paintCard (juce::Graphics& g, juce::Rectangle<float> r, float radius, juce::Colour accent)
    {
        g.setGradientFill (juce::ColourGradient::vertical (cardTop(), r.getY(), cardBottom(), r.getBottom()));
        g.fillRoundedRectangle (r, radius);

        // faint accent bloom in the top-left corner
        {
            juce::Path clip;
            clip.addRoundedRectangle (r, radius);
            juce::Graphics::ScopedSaveState s (g);
            g.reduceClipRegion (clip);
            g.setGradientFill (juce::ColourGradient (accent.withAlpha (0.10f), r.getX() + 24.f, r.getY(),
                                                     accent.withAlpha (0.f), r.getX() + 24.f + r.getWidth() * 0.55f, r.getY() + r.getHeight() * 0.9f,
                                                     true));
            g.fillRect (r);
        }

        g.setColour (border());
        g.drawRoundedRectangle (r.reduced (0.5f), radius, 1.f);

        // top edge highlight
        g.setGradientFill ([&]
        {
            juce::ColourGradient grad (juce::Colours::white.withAlpha (0.f), r.getX() + radius, r.getY(),
                                       juce::Colours::white.withAlpha (0.f), r.getRight() - radius, r.getY(), false);
            grad.addColour (0.5, juce::Colours::white.withAlpha (0.10f));
            return grad;
        }());
        g.fillRect (juce::Rectangle<float> (r.getX() + radius, r.getY() + 0.5f, r.getWidth() - 2.f * radius, 1.f));
    }

    inline void paintInset (juce::Graphics& g, juce::Rectangle<float> r, float radius)
    {
        g.setColour (well());
        g.fillRoundedRectangle (r, radius);
        g.setColour (juce::Colours::black.withAlpha (0.45f));
        g.drawRoundedRectangle (r.reduced (0.5f), radius, 1.f);
        g.setColour (juce::Colours::white.withAlpha (0.04f));
        g.drawRoundedRectangle (r.reduced (1.5f), juce::jmax (0.f, radius - 1.f), 1.f);
    }

    inline void paintGlowDot (juce::Graphics& g, juce::Point<float> c, float radius, juce::Colour colour)
    {
        g.setGradientFill (juce::ColourGradient (colour.withAlpha (0.45f), c.x, c.y, colour.withAlpha (0.f), c.x + radius * 2.6f, c.y, true));
        g.fillEllipse (juce::Rectangle<float> (radius * 5.2f, radius * 5.2f).withCentre (c));
        g.setColour (colour);
        g.fillEllipse (juce::Rectangle<float> (radius * 2.f, radius * 2.f).withCentre (c));
    }
}
