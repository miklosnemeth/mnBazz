# Changelog

All notable changes to mnBazz. The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and version numbers follow [Semantic Versioning](https://semver.org/).

## [Unreleased]

## [1.1.0] - 2026-11-12

### Added
- Documented **CLAP** as a future target (stock JUCE 9.0.5 has no CLAP CMake target yet).
- MIDI **velocity** shaping (Vel > Amp, Vel > Filt), **noise** mix, **LFO → amp**, **mod wheel → pitch**.
- **Filter env → resonance**, **OSC 2 pulse width**, **hard sync** (preset/automation).
- **Mono / stereo** output selector.
- Five factory presets: Vel Pluck, Click Sub, Reese Wide, Sync Acid, FM Knock (**47** total).
- Preset **dirty indicator**, **arrow-key** browsing, **Ctrl+S** save, **...** menu (rename/delete user presets).
- `docs/USER_GUIDE.md`, `AGENTS.md`, `site/build-site.py`, `scripts/validate_presets.py`.
- GitHub Actions **Build** workflow (Windows Release + zip artifact).

### Changed
- Light **2× ladder** processing when drive is high.
- Removed unused `Source/MnAudioUi.h`.

## [1.0.1] - 2026-11-12

### Changed
- Built with JUCE 9.0.5 (was 8.0.8).
- The project site is English by default. Hungarian is at `/hu/`.
- The default git branch is `master`.

## [1.0.0] - 2026-11-11

### Added
- Monophonic bass synthesizer for Windows: two oscillators, sub, 4-pole ladder filter, filter and amp envelopes, and an LFO.
- VST3 instrument and an optional Standalone app.
- 42 factory presets (Deep House, House, Techno, Trance, Sub, Classic, Acid, Funk, FX) and user `.mnbuzz` presets.
- Windows installer. The VST3 is always installed; Standalone is optional and off by default.
