# mnBazz

Monofonikus basszusszintetizátor Windowsra. Két oszcillátor, sub, létra-szűrő, két burkológörbe és LFO. **VST3** és **Standalone**.

A projektoldal: [miklosnemeth.github.io/mnBazz](https://miklosnemeth.github.io/mnBazz/)

## Telepítés

A kész build a [Releases](https://github.com/miklosnemeth/mnBazz/releases) oldalon van, `mnBazz-1.0.0-win64.zip` néven.

**VST3.** Másold a `mnBazz.vst3` mappát a `C:\Program Files\Common Files\VST3\` könyvtárba, vagy a DAW saját VST3 útvonalára, majd szkenneld újra a plugineket. Instrument / Synth kategóriában jelenik meg.

**Standalone.** Indítsd a `Standalone\mnBazz.exe` fájlt. A hangkimenet a Windows, illetve az ASIO eszköz beállításától függ.

## Hang

| Szakasz | Mit csinál |
| --- | --- |
| OSC 1 / OSC 2 | Fűrész, négyszög vagy háromszög. Oktáv −2…+2. A második oszcillátor centben elhangolható. |
| Sub | Négyszög, egy oktávval az első oszcillátor alatt. |
| Filter | Négypólusú létraszűrő, rezonancia, drive, burkológörbe-mélység és billentyűkövetés. |
| Filter Env / Amp Env | Külön ADSR a szűrőre és a hangerőre. |
| LFO | Háromszög, négyszög, fűrész, sample & hold vagy szinusz. A cutoffra és a hangmagasságra megy. |
| Play | Glide, retrigger vagy legato. A mod wheel (CC 1) a cutoffot nyitja, a pitch bend hangolja. |

Egy hang szól egyszerre. Átfedő billentyűknél legato módban a burkológörbe nem indul újra, retrigger módban igen.

## Presetek

42 gyári preset: Deep House, House, Techno, Trance, Sub, Classic, Acid, Funk, FX. A fejléc kategória-szűrője szűkíti a listát, a nyilak léptetnek.

A **SAVE** a `Dokumentumok\mnBazz\Presets\` mappába ír egy `.mnbuzz` fájlt. A **LOAD** onnan olvas. A felhasználói presetek a listában `*` előtaggal jelennek meg. Az **INIT** az első gyári presetet tölti be.

## Forrásból

Szükséges: Windows x64, CMake 3.22+, Visual Studio 2022 (vagy újabb) C++ fordító, Git.

```text
cmake -B build -DCMAKE_BUILD_TYPE=Release -DGIT_EXECUTABLE="C:/Program Files/Git/bin/git.exe"
cmake --build build --config Release
```

A `GIT_EXECUTABLE` csak akkor kell, ha a CMake nem találja a Git-et. A JUCE 8.0.8-at a CMake tölti le.

Kimenet:

- `build/mnBazz_artefacts/Release/VST3/mnBazz.vst3/`
- `build/mnBazz_artefacts/Release/Standalone/mnBazz.exe`

Csomag: `.\dist\package.ps1` — a verziót a `CMakeLists.txt` `project()` sorából olvassa, és `dist\mnBazz-<verzió>-win64.zip` fájlt készít.

## Licenc

A forrás a [JUCE](https://juce.com/) keretrendszerre épül, GPL feltételekkel (FetchContent, tag 8.0.8). A bináris terjesztése a [JUCE licenc](https://github.com/juce-framework/JUCE/blob/8.0.8/LICENSE.md) szerint GPL, vagy külön megvásárolt commercial licenc mellett lehetséges.

A felület betűi a Montserrat és a Roboto Condensed, mindkettő SIL Open Font License.
