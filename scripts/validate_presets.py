#!/usr/bin/env python3
"""Fail if FactoryPresets.h count does not match the factory[] table."""
import re
import sys
from pathlib import Path

root = Path(__file__).resolve().parents[1]
text = (root / "Source/FactoryPresets.h").read_text(encoding="utf-8")
declared = int(re.search(r"constexpr int count = (\d+)", text).group(1))
entries = len(re.findall(r'\{\s*"[^"]+"', text.split("static constexpr PresetData factory[]")[1].split("};")[0]))
if declared != entries:
    print(f"count={declared} but factory[] has {entries} entries", file=sys.stderr)
    sys.exit(1)
print(f"ok: {entries} presets")
