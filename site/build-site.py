#!/usr/bin/env python3
"""Refresh version and preset count in docs/*.html (run from repo root).

    python site/build-site.py
"""
import os
import re

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))


def read(path):
    with open(os.path.join(ROOT, path), encoding="utf-8") as f:
        return f.read()


def write(path, text):
    with open(os.path.join(ROOT, path), "w", encoding="utf-8", newline="\n") as f:
        f.write(text)


def project_version():
    m = re.search(r"project\(mnBazz VERSION ([0-9.]+)\)", read("CMakeLists.txt"))
    return m.group(1) if m else "?"


def factory_preset_count():
    m = re.search(r"constexpr int count = (\d+)", read("Source/FactoryPresets.h"))
    return m.group(1) if m else "?"


def patch_file(rel):
    text = read(rel)
    ver = project_version()
    n = factory_preset_count()
    text = re.sub(r"mnBazz \d+\.\d+(?:\.\d+)?", f"mnBazz {ver}", text)
    text = re.sub(r"\b\d+ factory presets\b", f"{n} factory presets", text)
    text = re.sub(r"\b\d+ gyári preset\b", f"{n} gyári preset", text)
    text = re.sub(
        r"releases/latest/download/mnBazz-\d+\.\d+\.\d+-win64-setup\.exe",
        f"releases/latest/download/mnBazz-{ver}-win64-setup.exe",
        text,
    )
    text = re.sub(
        r"Telepítő \d+\.\d+(?:\.\d+)?",
        f"Telepítő {ver}",
        text,
    )
    text = re.sub(
        r">Installer \d+\.\d+(?:\.\d+)?<",
        f">Installer {ver}<",
        text,
    )
    text = re.sub(
        r"· \d+\.\d+(?:\.\d+)? · Windows",
        f"· {ver} · Windows",
        text,
    )
    write(rel, text)
    print(f"updated {rel}")


def main():
    patch_file("docs/index.html")
    patch_file("docs/hu/index.html")


if __name__ == "__main__":
    main()
