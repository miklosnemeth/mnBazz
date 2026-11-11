#pragma once
#include <juce_graphics/juce_graphics.h>
#include "BinaryData.h"

namespace AppFonts
{
    inline juce::Typeface::Ptr headingTypeface()
    {
        static const auto tf = juce::Typeface::createSystemTypefaceFor (
            BinaryData::MontserratBold_ttf, (size_t) BinaryData::MontserratBold_ttfSize);
        return tf;
    }

    inline juce::Typeface::Ptr uiTypeface()
    {
        static const auto tf = juce::Typeface::createSystemTypefaceFor (
            BinaryData::RobotoCondensed_ttf, (size_t) BinaryData::RobotoCondensed_ttfSize);
        return tf;
    }

    inline juce::Typeface::Ptr uiBoldTypeface()
    {
        static const auto tf = juce::Typeface::createSystemTypefaceFor (
            BinaryData::RobotoCondensedBold_ttf, (size_t) BinaryData::RobotoCondensedBold_ttfSize);
        return tf;
    }

    // Montserrat Bold: brand, section titles, small caps labels (pass a kerning factor for tracking).
    inline juce::Font title (float height, float tracking = 0.f)
    {
        return juce::Font (juce::FontOptions (headingTypeface()).withHeight (height).withKerningFactor (tracking));
    }

    // Roboto Condensed Bold: values, buttons, combo boxes.
    inline juce::Font ui (float height)
    {
        return juce::Font (juce::FontOptions (uiBoldTypeface()).withHeight (height));
    }

    // Roboto Condensed Regular: secondary text.
    inline juce::Font small (float height)
    {
        return juce::Font (juce::FontOptions (uiTypeface()).withHeight (height));
    }
}
