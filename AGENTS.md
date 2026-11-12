# Agent notes (mnBazz)

- **Version** lives in `CMakeLists.txt` `project(mnBazz VERSION …)`.
- **Factory presets** live in `Source/FactoryPresets.h` (`count` must match the table and category list).
- **Do not hand-edit** version or preset counts in `docs/index.html` / `docs/hu/index.html` when those values come from the project — run `python site/build-site.py`.
- **Git branch:** `master`. Pages workflow deploys `docs/` on push to `master`.
- **Release build:** `cmake -B build -DCMAKE_BUILD_TYPE=Release` then `cmake --build build --config Release`. Installer: `ISCC.exe dist\mnBazz.iss`.
