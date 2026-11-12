# mnBazz user guide

mnBazz is a monophonic bass synthesizer for Windows (VST3 and Standalone). One note at a time, aimed at house, techno, trance, and classic bass sounds.

## Quick start

1. Install from the [latest release](https://github.com/miklosnemeth/mnBazz/releases/latest) (`mnBazz-*-win64-setup.exe`, run as administrator).
2. Rescan plug-ins in the DAW.
3. Add mnBazz on an **instrument** track. Send MIDI; audio comes from the plug-in output.
4. Pick a factory preset from the header, then tweak OSC / FILTER / LFO cards.

## Presets

- **47 factory presets** grouped by category (filter in the header).
- **User presets** live in `Documents\mnBazz\Presets\` as `.mnbuzz` files.
- **SAVE** writes the current settings. **LOAD** opens a file. User entries show with a `*` in the list.
- **INIT** loads the first factory preset.
- Orange dot on the preset bar = settings changed since the last load or save.
- **`...` menu**: save duplicate, rename, or delete the active user preset.
- **Arrow keys** step presets. **Ctrl+S** opens save.

## Performance controls

| Control | MIDI |
| --- | --- |
| Pitch bend | ±2 semitones (default host range) |
| Mod wheel (CC 1) | Opens filter cutoff (Mod > Cutoff) and optional pitch (Mod > Pitch) |
| Velocity | Vel > Amp and Vel > Filt (after 1.1) |

## Sound blocks

- **OSC 1 / 2 / SUB** — Blep saw, square (with pulse width on OSC 2), triangle. Sub is a square one octave below OSC 1.
- **OSC 2 Sync** — Hard sync (factory “Sync Acid” preset, or automation).
- **FILTER** — 4-pole ladder, drive in the ladder path, filter ADSR, key track, optional filter env → resonance.
- **LFO** — Routes to cutoff, pitch, and amplitude.
- **Play** — Glide, legato vs retrigger, noise mix for offbeat clicks.

## Output

- **Output** selector: **Mono** (left channel only) or **Stereo** (duplicate mono on L+R).

## Build and support

See [README](../README.md) and [CHANGELOG](../CHANGELOG.md). Home: [miklosnemeth.github.io/mnBazz](https://miklosnemeth.github.io/mnBazz/).
