#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

namespace UserPresets
{
    inline juce::File directory()
    {
        return juce::File::getSpecialLocation (juce::File::userDocumentsDirectory)
            .getChildFile ("mnBazz")
            .getChildFile ("Presets");
    }

    inline void ensureDirectory()
    {
        directory().createDirectory();
    }

    inline juce::StringArray list()
    {
        ensureDirectory();
        juce::Array<juce::File> files;
        directory().findChildFiles (files, juce::File::findFiles, false, "*.mnbuzz");
        files.sort();
        juce::StringArray names;
        for (auto& f : files)
            names.add (f.getFileNameWithoutExtension());
        return names;
    }

    inline bool save (juce::AudioProcessorValueTreeState& apvts, const juce::File& file)
    {
        ensureDirectory();
        if (auto xml = apvts.copyState().createXml())
            return xml->writeTo (file);
        return false;
    }

    inline bool load (juce::AudioProcessorValueTreeState& apvts, const juce::File& file)
    {
        if (auto xml = juce::XmlDocument::parse (file))
            if (xml->hasTagName (apvts.state.getType()))
            {
                apvts.replaceState (juce::ValueTree::fromXml (*xml));
                return true;
            }
        return false;
    }
}
