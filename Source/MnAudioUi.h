#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
// The look shared by the MNAudio plugins (mnKord, mnArpee, mnBazz): the palette, the sizes of the
// header and of the cards, and the drawing of the window frame, the cards, the brand and the
// "unsaved" dot.
//
// Keep Source/MnAudioUi.h in sync across MNAudio plugin repos when changing the shared parts.
//==============================================================================
namespace mnaudio::ui
{
struct Palette
{
    static juce::Colour bg()       { return juce::Colour(0xff090909); }
    static juce::Colour card()     { return juce::Colour(0xff101010); }
    static juce::Colour cardHi()   { return juce::Colour(0xff181818); }
    static juce::Colour border()   { return juce::Colour(0xff2a2a2a); }
    static juce::Colour text()     { return juce::Colour(0xfff0faf3); }
    static juce::Colour dimText()  { return juce::Colour(0xffb4cdbc); }
    static juce::Colour accent()   { return juce::Colour(0xffe8952a); }
    static juce::Colour accentHi() { return juce::Colour(0xfff4c56a); }
    static juce::Colour accent2()  { return juce::Colour(0xffff5d5d); }
    static juce::Colour danger()   { return juce::Colour(0xffff7a7a); }
    static juce::Colour cellOff()  { return juce::Colour(0xff101814); }
    static juce::Colour sectionHeader() { return juce::Colour(0xff16120e); }
    static juce::Colour playHead() { return juce::Colour(0x40f4c56a); }
};

namespace metrics
{
constexpr int margin = 16;
constexpr int headerHeight = 56;
constexpr int brandWidth = 170;
constexpr int brandTitleHeight = 38;
constexpr int brandVersionHeight = 16;
constexpr int cardGap = 14;
constexpr float cardRadius = 8.0f;
constexpr float titleSize = 28.0f;
constexpr float versionSize = 13.0f;

constexpr int pickerHeight = 34;
constexpr int arrowWidth = 26;
constexpr int chipWidth = 190;
constexpr int newWidth = 52;
constexpr int saveWidth = 52;
constexpr int menuWidth = 62;
constexpr int historyWidth = 30;
}

struct HeaderRects
{
    juce::Rectangle<int> brand;
    juce::Rectangle<int> version;
    juce::Rectangle<int> prev, preset, next, newButton, save, menu;
    juce::Rectangle<int> undo, redo;
    juce::Rectangle<int> right;
    juce::Rectangle<int> below;
};

inline HeaderRects layoutHeader(juce::Rectangle<int> windowBounds, int rightWidth = 0)
{
    using namespace metrics;
    HeaderRects r;

    auto area = windowBounds.reduced(margin);
    auto header = area.removeFromTop(headerHeight);
    r.right = header.removeFromRight(rightWidth);
    auto brand = header.removeFromLeft(brandWidth);
    r.brand = brand.removeFromTop(brandTitleHeight);
    r.version = brand.removeFromTop(brandVersionHeight);

    const int pickerWidth = arrowWidth + 4 + chipWidth + 4 + arrowWidth + 8 + newWidth + 6 + saveWidth + 6 + menuWidth;
    const int groupWidth = pickerWidth + 14 + historyWidth + 6 + historyWidth;
    auto group = header.withSizeKeepingCentre(groupWidth, pickerHeight);
    auto picker = group.removeFromLeft(pickerWidth);
    group.removeFromLeft(14);
    r.undo = group.removeFromLeft(historyWidth);
    group.removeFromLeft(6);
    r.redo = group.removeFromLeft(historyWidth);

    r.prev = picker.removeFromLeft(arrowWidth).reduced(6, 8);
    picker.removeFromLeft(4);
    r.preset = picker.removeFromLeft(chipWidth);
    picker.removeFromLeft(4);
    r.next = picker.removeFromLeft(arrowWidth).reduced(6, 8);
    picker.removeFromLeft(8);
    r.newButton = picker.removeFromLeft(newWidth);
    picker.removeFromLeft(6);
    r.save = picker.removeFromLeft(saveWidth);
    picker.removeFromLeft(6);
    r.menu = picker.removeFromLeft(menuWidth);

    r.below = area;
    return r;
}

// mnBazz header: category + preset row + Init/Save/Load; scope in rightWidth.
struct BazzHeaderRects
{
    juce::Rectangle<int> brand, version, scope;
    juce::Rectangle<int> category, presetLabel, prev, preset, next, init, save, load;
    juce::Rectangle<int> below;
};

inline int scaledMetric(int px, float scale)
{
    return juce::roundToInt((float) px * scale);
}

inline BazzHeaderRects layoutBazzHeader(juce::Rectangle<int> windowBounds, int scopeWidth, float scale = 1.f)
{
    using namespace metrics;
    BazzHeaderRects r;

    const int m = scaledMetric(margin, scale);
    const int hh = scaledMetric(headerHeight, scale);
    auto area = windowBounds.reduced(m);
    auto header = area.removeFromTop(hh);

    r.scope = header.removeFromRight(scaledMetric(scopeWidth, scale))
                 .reduced(scaledMetric(4, scale), scaledMetric(10, scale));

    auto brand = header.removeFromLeft(scaledMetric(brandWidth, scale));
    r.brand = brand.removeFromTop(scaledMetric(brandTitleHeight, scale));
    r.version = brand.removeFromTop(scaledMetric(brandVersionHeight, scale));

    const int ph = scaledMetric(pickerHeight, scale);
    const int aw = scaledMetric(arrowWidth, scale);
    int catW = scaledMetric(88, scale);
    int labelW = scaledMetric(50, scale);
    const int btnW = scaledMetric(newWidth, scale);
    const int chipMin = scaledMetric(72, scale);
    const int gapSm = scaledMetric(4, scale);
    const int gapMd = scaledMetric(6, scale);
    const int gapLg = scaledMetric(8, scale);
    const int avail = header.getWidth();

    auto fixedWidth = [&] (int cat, int lbl) {
        return cat + lbl + gapSm + aw + gapSm + aw + gapLg + 3 * btnW + 2 * gapMd + 4 * gapSm + gapLg
               + (lbl > 0 ? gapSm : 0);
    };

    int fixedW = fixedWidth(catW, labelW);
    if (avail < fixedW + chipMin)
    {
        labelW = 0;
        fixedW = fixedWidth(catW, labelW);
    }
    if (avail < fixedW + chipMin)
    {
        catW = scaledMetric(72, scale);
        fixedW = fixedWidth(catW, labelW);
    }

    const int chipW = juce::jmax (scaledMetric (40, scale), avail - fixedW);
    const int groupW = juce::jmin (avail, fixedW + chipW);

    auto group = header.withSizeKeepingCentre (groupW, ph);
    r.category = group.removeFromLeft (catW);
    group.removeFromLeft (gapSm);
    r.presetLabel = labelW > 0 ? group.removeFromLeft (labelW) : juce::Rectangle<int>();
    if (labelW > 0)
        group.removeFromLeft (gapSm);
    r.prev = group.removeFromLeft(aw).reduced(scaledMetric(6, scale), scaledMetric(8, scale));
    group.removeFromLeft(gapSm);
    r.preset = group.removeFromLeft(chipW);
    group.removeFromLeft(gapSm);
    r.next = group.removeFromLeft(aw).reduced(scaledMetric(6, scale), scaledMetric(8, scale));
    group.removeFromLeft(gapLg);
    r.init = group.removeFromLeft(btnW);
    group.removeFromLeft(gapMd);
    r.save = group.removeFromLeft(btnW);
    group.removeFromLeft(gapMd);
    r.load = group.removeFromLeft(btnW);

    r.below = area;
    return r;
}

inline void paintWindow(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    g.fillAll(Palette::bg());
    g.setColour(Palette::border());
    g.drawRect(bounds, 1);
}

inline void paintCard(juce::Graphics& g, juce::Rectangle<int> card)
{
    g.setColour(Palette::card());
    g.fillRoundedRectangle(card.toFloat(), metrics::cardRadius);
    g.setColour(Palette::accent().withAlpha(0.9f));
    g.drawRoundedRectangle(card.toFloat().reduced(0.5f), metrics::cardRadius, 1.25f);
}

// Section block: narancs sáv + cím, alatta a controls a háttéren (nincs kártya keret).
inline void paintSectionHeader(juce::Graphics& g, juce::Rectangle<int> section,
                               const juce::String& title, const juce::Font& titleFont, int headerHeight)
{
    auto header = section.withHeight(juce::jmin(headerHeight, section.getHeight()));

    g.setColour(Palette::sectionHeader());
    g.fillRect(header);

    g.setColour(Palette::text());
    g.setFont(titleFont);
    g.drawText(title, header.reduced(14, 0), juce::Justification::centredLeft);
}

inline void paintBrand(juce::Graphics& g, juce::Rectangle<int> titleArea, juce::Rectangle<int> versionArea,
                       const juce::String& name, const juce::String& version,
                       const juce::Font& titleFont, const juce::Font& versionFont)
{
    g.setColour(Palette::accent());
    g.setFont(titleFont);
    for (const float dx : { 0.0f, 0.6f })
        g.drawText(name, titleArea.toFloat().translated(dx, 0.0f), juce::Justification::centredLeft);

    g.setColour(Palette::dimText());
    g.setFont(versionFont);
    g.drawText(version, versionArea, juce::Justification::centredLeft);
}

inline void paintDirtyDot(juce::Graphics& g, juce::Rectangle<int> presetButtonBounds)
{
    g.setColour(Palette::accent());
    g.fillEllipse((float) presetButtonBounds.getX() + 10.0f, (float) presetButtonBounds.getCentreY() - 4.0f, 8.0f, 8.0f);
}

class HistoryButton : public juce::Button
{
public:
    explicit HistoryButton(bool redoIn) : juce::Button(redoIn ? "redo" : "undo"), redo(redoIn) {}

    void paintButton(juce::Graphics& g, bool highlighted, bool down) override
    {
        const auto r = getLocalBounds().toFloat().reduced(1.0f);
        g.setColour(down ? Palette::cardHi().brighter(0.2f) : highlighted ? Palette::cardHi().brighter(0.1f) : Palette::cardHi());
        g.fillRoundedRectangle(r, 5.0f);
        g.setColour(Palette::border());
        g.drawRoundedRectangle(r, 5.0f, 1.0f);

        const auto c = r.getCentre();
        const float rad = std::min(r.getWidth(), r.getHeight()) * 0.28f;
        juce::Path arc;
        arc.addCentredArc(c.x, c.y + 1.0f, rad, rad, 0.0f, -2.3f, 1.6f, true);
        juce::Path head;
        head.addTriangle(-4.5f, 0.0f, 4.5f, 0.0f, 0.0f, -6.0f);
        g.setColour(isEnabled() ? Palette::text() : Palette::dimText().withAlpha(0.35f));
        juce::AffineTransform flip = redo ? juce::AffineTransform::scale(-1.0f, 1.0f, c.x, c.y) : juce::AffineTransform();
        g.strokePath(arc, juce::PathStrokeType(2.0f), flip);
        const auto tip = arc.getPointAlongPath(0.0f).transformedBy(flip);
        head = head.createPathWithRoundedCorners(1.0f);
        g.fillPath(head, juce::AffineTransform::rotation(redo ? 0.6f : -0.6f).translated(tip.x, tip.y));
    }

private:
    bool redo;
};
} // namespace mnaudio::ui
