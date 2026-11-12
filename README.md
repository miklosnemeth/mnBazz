# mnBazz

Monophonic bass synthesizer for Windows. Two oscillators, sub, ladder filter, two envelopes, and an LFO. **VST3** and **Standalone**.

**Home page and download:** [miklosnemeth.github.io/mnBazz](https://miklosnemeth.github.io/mnBazz/) ([magyar](https://miklosnemeth.github.io/mnBazz/hu/)) · [Latest release](https://github.com/miklosnemeth/mnBazz/releases/latest) · [User guide](docs/USER_GUIDE.md) · [Changelog](CHANGELOG.md)

## Install

Builds are on [Releases](https://github.com/miklosnemeth/mnBazz/releases).

**Installer.** Run `mnBazz-1.1.0-win64-setup.exe` as administrator. The VST3 always goes to `C:\Program Files\Common Files\VST3\mnBazz.vst3`. Standalone is a separate checkbox, off by default. When selected, the app is installed to `C:\Program Files\mnBazz\mnBazz.exe` and a Start menu shortcut is created. Rescan plug-ins in the DAW afterwards. Uninstall also removes the VST3 folder.

**Manual copy.** The zip contains the same two files with no extra wrapper folder: the `mnBazz.vst3` bundle and `Standalone\mnBazz.exe`.

Rebuild the installer after a Release build:

```text
& "$env:LOCALAPPDATA\Programs\Inno Setup 6\ISCC.exe" dist\mnBazz.iss
```

## Sound

| Section | What it does |
| --- | --- |
| OSC 1 / OSC 2 | Saw, square, or triangle. Octave −2…+2. OSC 2 can be detuned in cents. |
| Sub | Square, one octave below OSC 1. |
| Filter | 4-pole ladder, resonance, drive, envelope depth, and key tracking. |
| Filter Env / Amp Env | Separate ADSR for the filter and the amplifier. |
| LFO | Triangle, square, saw, sample & hold, or sine, routed to cutoff and pitch. |
| Play | Glide, retrigger or legato. Mod wheel (CC 1) opens the cutoff. Pitch bend tunes the note. |

One note at a time. In legato mode an overlapping key does not restart the envelopes. In retrigger mode it does.

## Presets

47 factory presets: Deep House, House, Techno, Trance, Sub, Classic, Acid, Funk, FX. The category filter in the header narrows the list. The arrows step through it.

**SAVE** writes a `.mnbuzz` file to `Documents\mnBazz\Presets\`. **LOAD** reads from there. User presets are prefixed with `*` in the list. **INIT** loads the first factory preset.

## Build from source

Requires Windows x64, CMake 3.22+, a Visual Studio 2022 (or newer) C++ toolchain, and Git.

```text
cmake -B build -DCMAKE_BUILD_TYPE=Release -DGIT_EXECUTABLE="C:/Program Files/Git/bin/git.exe"
cmake --build build --config Release
```

`GIT_EXECUTABLE` is only needed when CMake cannot find Git. CMake fetches JUCE 9.0.5.

Output:

- `build/mnBazz_artefacts/Release/VST3/mnBazz.vst3/`
- `build/mnBazz_artefacts/Release/Standalone/mnBazz.exe`

Package: `.\dist\package.ps1` reads the version from the `project()` line in `CMakeLists.txt` and writes `dist\mnBazz-<version>-win64.zip`.

## License

The source is built on the [JUCE](https://juce.com/) framework under the GPL (FetchContent, tag 9.0.5). Distributing the binary follows the [JUCE licence](https://github.com/juce-framework/JUCE/blob/9.0.5/LICENSE.md): GPL, or a separately purchased commercial licence.

The UI fonts are Montserrat and Roboto Condensed, both under the SIL Open Font License.
