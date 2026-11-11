#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "AppFonts.h"
#include "Theme.h"

class MnLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void setUiScale (float s) { uiScale = juce::jmax (0.6f, s); }
    float getUiScale() const { return uiScale; }

    MnLookAndFeel()
    {
        using namespace theme;
        setColour (juce::ResizableWindow::backgroundColourId, bgBottom());
        setColour (juce::Label::textColourId, text());
        setColour (juce::Slider::textBoxTextColourId, text());
        setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
        setColour (juce::Slider::textBoxHighlightColourId, amber().withAlpha (0.3f));
        setColour (juce::TextEditor::backgroundColourId, well());
        setColour (juce::TextEditor::textColourId, text());
        setColour (juce::TextEditor::highlightColourId, amber().withAlpha (0.3f));
        setColour (juce::TextEditor::highlightedTextColourId, text());
        setColour (juce::TextEditor::outlineColourId, amber().withAlpha (0.6f));
        setColour (juce::TextEditor::focusedOutlineColourId, amber());
        setColour (juce::CaretComponent::caretColourId, amber());
        setColour (juce::ComboBox::backgroundColourId, raised());
        setColour (juce::ComboBox::textColourId, text());
        setColour (juce::ComboBox::outlineColourId, border());
        setColour (juce::ComboBox::arrowColourId, dim());
        setColour (juce::PopupMenu::backgroundColourId, cardTop());
        setColour (juce::PopupMenu::textColourId, text());
        setColour (juce::PopupMenu::highlightedBackgroundColourId, amber().withAlpha (0.16f));
        setColour (juce::PopupMenu::highlightedTextColourId, amber());
        setColour (juce::TooltipWindow::backgroundColourId, raised());
        setColour (juce::TooltipWindow::textColourId, text());
        setColour (juce::TooltipWindow::outlineColourId, borderHi());
        setColour (juce::Slider::rotarySliderFillColourId, amber());
    }

    juce::Typeface::Ptr getTypefaceForFont (const juce::Font&) override { return AppFonts::uiTypeface(); }

    //==========================================================================
    // Fonts
    juce::Font getComboBoxFont (juce::ComboBox& box) override
    {
        return AppFonts::ui (juce::jmin (17.f * uiScale, (float) box.getHeight() * 0.5f));
    }

    juce::Font getPopupMenuFont() override { return AppFonts::ui (15.f * uiScale); }
    juce::Font getLabelFont (juce::Label& label) override { return label.getFont(); }

    juce::Font getTextButtonFont (juce::TextButton&, int buttonH) override
    {
        return AppFonts::title (juce::jmin (11.5f * uiScale, (float) buttonH * 0.34f), 0.08f);
    }

    //==========================================================================
    // Buttons
    void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                               bool highlighted, bool down) override
    {
        auto r = button.getLocalBounds().toFloat().reduced (0.5f);
        const float radius = 9.f * uiScale;
        const bool primary = button.getProperties()["style"].toString() == "primary";

        if (primary)
        {
            auto fill = backgroundColour;
            if (down) fill = fill.darker (0.12f);
            else if (highlighted) fill = fill.brighter (0.1f);
            g.setGradientFill (juce::ColourGradient::vertical (fill.brighter (0.18f), r.getY(), fill.darker (0.12f), r.getBottom()));
            g.fillRoundedRectangle (r, radius);
            g.setColour (juce::Colours::white.withAlpha (0.18f));
            g.drawRoundedRectangle (r, radius, 1.f);
        }
        else
        {
            auto fill = theme::raised();
            if (down) fill = fill.brighter (0.18f);
            else if (highlighted) fill = fill.brighter (0.09f);
            g.setColour (fill);
            g.fillRoundedRectangle (r, radius);
            g.setColour (highlighted ? theme::borderHi().brighter (0.2f) : theme::border());
            g.drawRoundedRectangle (r, radius, 1.f);
        }
    }

    //==========================================================================
    // Combo box
    void drawComboBox (juce::Graphics& g, int w, int h, bool isButtonDown,
                       int, int, int, int, juce::ComboBox& box) override
    {
        auto r = juce::Rectangle<float> (0.f, 0.f, (float) w, (float) h).reduced (0.5f);
        const float radius = 9.f * uiScale;
        auto fill = box.findColour (juce::ComboBox::backgroundColourId);
        if (isButtonDown)                fill = fill.brighter (0.15f);
        else if (box.isMouseOver (true)) fill = fill.brighter (0.07f);

        g.setColour (fill);
        g.fillRoundedRectangle (r, radius);
        g.setColour (box.hasKeyboardFocus (false) ? theme::amber().withAlpha (0.7f)
                                                  : box.findColour (juce::ComboBox::outlineColourId));
        g.drawRoundedRectangle (r, radius, 1.f);

        juce::Path arrow;
        const float cx = (float) w - 16.f * uiScale, cy = (float) h * 0.5f;
        const float a = 3.6f * uiScale;
        arrow.startNewSubPath (cx - a, cy - a * 0.5f);
        arrow.lineTo (cx, cy + a * 0.5f);
        arrow.lineTo (cx + a, cy - a * 0.5f);
        g.setColour (box.findColour (juce::ComboBox::arrowColourId));
        g.strokePath (arrow, juce::PathStrokeType (1.8f * uiScale, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    void positionComboBoxText (juce::ComboBox& box, juce::Label& label) override
    {
        const int padL = juce::roundToInt (14.f * uiScale);
        const int padR = juce::roundToInt (32.f * uiScale);
        label.setBounds (padL, 1, box.getWidth() - padL - padR, box.getHeight() - 2);
        label.setFont (getComboBoxFont (box));
    }

    //==========================================================================
    // Popup menu
    void drawPopupMenuBackground (juce::Graphics& g, int w, int h) override
    {
        g.fillAll (theme::cardTop());
        g.setColour (theme::borderHi());
        g.drawRect (0, 0, w, h, 1);
    }

    void drawPopupMenuItem (juce::Graphics& g, const juce::Rectangle<int>& area, bool isSeparator, bool isActive,
                            bool isHighlighted, bool isTicked, bool, const juce::String& text,
                            const juce::String&, const juce::Drawable*, const juce::Colour* textColour) override
    {
        if (isSeparator)
        {
            g.setColour (theme::border());
            g.fillRect (area.withSizeKeepingCentre (area.getWidth() - 16, 1));
            return;
        }

        auto row = area.reduced (3, 1);
        if (isHighlighted && isActive)
        {
            g.setColour (findColour (juce::PopupMenu::highlightedBackgroundColourId));
            g.fillRoundedRectangle (row.toFloat(), 6.f);
        }

        auto colour = textColour != nullptr ? *textColour
                    : isHighlighted         ? findColour (juce::PopupMenu::highlightedTextColourId)
                    : isTicked              ? theme::amber()
                                            : findColour (juce::PopupMenu::textColourId);
        if (! isActive)
            colour = colour.withAlpha (0.4f);

        g.setColour (colour);
        g.setFont (getPopupMenuFont());
        g.drawText (text, row.withTrimmedLeft (22), juce::Justification::centredLeft, true);

        if (isTicked)
        {
            g.setColour (theme::amber());
            g.fillEllipse (juce::Rectangle<float> (6.f, 6.f).withCentre ({ (float) row.getX() + 11.f, (float) row.getCentreY() }));
        }
    }

    void getIdealPopupMenuItemSize (const juce::String& t, bool sep, int standardH, int& w, int& h) override
    {
        h = standardH > 0 ? standardH : juce::roundToInt (26.f * uiScale);
        h = juce::jmax (h, juce::roundToInt (26.f * uiScale));
        if (sep) { w = 50; h = juce::roundToInt (8.f * uiScale); return; }
        w = juce::GlyphArrangement::getStringWidthInt (getPopupMenuFont(), t) + 22 + 24;
    }

    int getPopupMenuBorderSize() override { return 4; }

    //==========================================================================
    // Rotary knob
    void drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h, float pos,
                           float startAngle, float endAngle, juce::Slider& s) override
    {
        const auto accent = s.findColour (juce::Slider::rotarySliderFillColourId);
        const bool active = s.isEnabled() && (s.isMouseOverOrDragging() || s.hasKeyboardFocus (false));
        const bool bipolar = s.getMinimum() < 0.0 && s.getMaximum() > 0.0;

        const float d = (float) juce::jmin (w, h);
        auto area = juce::Rectangle<float> (d, d).withCentre (juce::Rectangle<int> (x, y, w, h).toFloat().getCentre());
        const auto c = area.getCentre();

        const float arcW = juce::jmax (3.f, d * 0.062f);
        const float rArc = d * 0.5f - arcW * 1.4f;
        const float angle = startAngle + pos * (endAngle - startAngle);
        const float zero = bipolar ? (startAngle + endAngle) * 0.5f : startAngle;

        const juce::PathStrokeType stroke (arcW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);

        juce::Path track;
        track.addCentredArc (c.x, c.y, rArc, rArc, 0.f, startAngle, endAngle, true);
        g.setColour (theme::raised());
        g.strokePath (track, stroke);

        if (std::abs (angle - zero) > 0.004f)
        {
            juce::Path val;
            val.addCentredArc (c.x, c.y, rArc, rArc, 0.f, juce::jmin (zero, angle), juce::jmax (zero, angle), true);
            g.setColour (accent.withAlpha (active ? 0.30f : 0.16f));
            g.strokePath (val, juce::PathStrokeType (arcW * 2.6f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
            g.setColour (accent);
            g.strokePath (val, stroke);
        }

        // centre detent tick for bipolar knobs
        if (bipolar)
        {
            const float zr0 = rArc + arcW * 0.9f, zr1 = rArc + arcW * 1.45f;
            g.setColour (theme::faint());
            g.drawLine (c.x, c.y - zr0, c.x, c.y - zr1, 1.2f);
        }

        // knob body
        const float rBody = rArc - arcW * 1.15f;
        auto body = juce::Rectangle<float> (rBody * 2.f, rBody * 2.f).withCentre (c);

        g.setColour (juce::Colours::black.withAlpha (0.5f));
        g.fillEllipse (body.translated (0.f, rBody * 0.10f).expanded (rBody * 0.04f));

        g.setGradientFill (juce::ColourGradient (juce::Colour (active ? 0xff343b4c : 0xff2d3342), c.x - rBody * 0.4f, c.y - rBody * 0.6f,
                                                 juce::Colour (0xff151921), c.x + rBody * 0.5f, c.y + rBody * 0.9f, true));
        g.fillEllipse (body);

        g.setColour (juce::Colours::white.withAlpha (active ? 0.16f : 0.09f));
        g.drawEllipse (body.reduced (0.5f), 1.f);

        // pointer
        const float p0 = rBody * 0.34f, p1 = rBody * 0.86f;
        const float sn = std::sin (angle), cs = std::cos (angle);
        g.setColour (accent);
        g.drawLine (c.x + sn * p0, c.y - cs * p0, c.x + sn * p1, c.y - cs * p1,
                    juce::jmax (2.2f, rBody * 0.11f));
    }

    juce::Label* createSliderTextBox (juce::Slider& s) override
    {
        auto* l = LookAndFeel_V4::createSliderTextBox (s);
        l->setFont (AppFonts::ui (14.f * uiScale));
        l->setColour (juce::Label::textColourId, theme::text());
        l->setColour (juce::Label::backgroundColourId, juce::Colours::transparentBlack);
        l->setColour (juce::Label::outlineColourId, juce::Colours::transparentBlack);
        l->setJustificationType (juce::Justification::centred);
        l->setMinimumHorizontalScale (0.7f);
        return l;
    }

    //==========================================================================
    // Tooltip
    juce::Rectangle<int> getTooltipBounds (const juce::String& t, juce::Point<int> pos, juce::Rectangle<int> parent) override
    {
        const int w = juce::GlyphArrangement::getStringWidthInt (AppFonts::ui (13.f * uiScale), t) + 22;
        return juce::Rectangle<int> (pos.x, pos.y + 20, w, juce::roundToInt (26.f * uiScale)).constrainedWithin (parent);
    }

    void drawTooltip (juce::Graphics& g, const juce::String& t, int w, int h) override
    {
        auto r = juce::Rectangle<float> (0.f, 0.f, (float) w, (float) h);
        g.setColour (theme::raised());
        g.fillRoundedRectangle (r, 7.f);
        g.setColour (theme::borderHi());
        g.drawRoundedRectangle (r.reduced (0.5f), 7.f, 1.f);
        g.setColour (theme::text());
        g.setFont (AppFonts::ui (13.f * uiScale));
        g.drawText (t, 0, 0, w, h, juce::Justification::centred);
    }

private:
    float uiScale = 1.f;
};
