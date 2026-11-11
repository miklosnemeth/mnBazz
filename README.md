# mnBazz

Monofonikus basszusszintetizátor Windowsra. Két oszcillátor, sub, létra-szűrő, két burkológörbe és LFO. **VST3** és **Standalone**.

A projektoldal: [miklosnemeth.github.io/mnBazz](https://miklosnemeth.github.io/mnBazz/)

## Telepítés

A kész build a [Releases](https://github.com/miklosnemeth/mnBazz/releases) oldalon van.

**Telepítő.** A `mnBazz-1.0.0-win64-setup.exe` rendszergazdaként fut. A VST3 mindig a `C:\Program Files\Common Files\VST3\mnBazz.vst3` mappába kerül. A Standalone külön pipa: ha be van jelölve, a program a `C:\Program Files\mnBazz\mnBazz.exe` helyre kerül, és megjelenik a Start menüben. Alapból ki van kapcsolva. Telepítés után a DAW-ban szkenneld újra a plugineket. Az eltávolító a VST3 mappát is törli.

**Kézi másolás.** A zip ugyanazt a két fájlt tartalmazza mappa nélkül: a `mnBazz.vst3` bundle-t és a `Standalone\mnBazz.exe` programot.

A telepítő újraépítése Release build után:

```text
& "$env:LOCALAPPDATA\Programs\Inno Setup 6\ISCC.exe" dist\mnBazz.iss
```

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
