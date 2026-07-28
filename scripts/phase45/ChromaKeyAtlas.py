"""Create a transparent, decontaminated atlas from a uniform chroma background."""

from __future__ import annotations

import argparse
import json
from pathlib import Path

import numpy as np
from PIL import Image


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--report", type=Path)
    parser.add_argument("--threshold", type=float, default=24.0)
    parser.add_argument("--feather", type=float, default=42.0)
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    image = Image.open(args.source).convert("RGB")
    rgb = np.asarray(image, dtype=np.float32)
    key = rgb[0, 0].copy()
    distance = np.linalg.norm(rgb - key, axis=2)
    alpha = np.clip(
        (distance - args.threshold) / max(args.feather, 1.0),
        0.0,
        1.0,
    )

    safe_alpha = np.maximum(alpha[..., None], 1.0 / 255.0)
    foreground = (rgb - (1.0 - alpha[..., None]) * key) / safe_alpha
    foreground = np.clip(foreground, 0.0, 255.0)
    foreground[alpha <= 0.0] = 0.0
    rgba = np.dstack((foreground, alpha[..., None] * 255.0)).astype(np.uint8)

    args.output.parent.mkdir(parents=True, exist_ok=True)
    Image.fromarray(rgba, mode="RGBA").save(args.output, optimize=True)
    report = {
        "source": str(args.source.resolve()),
        "output": str(args.output.resolve()),
        "sampled_key_rgb": [int(value) for value in key],
        "threshold": args.threshold,
        "feather": args.feather,
        "pixels": int(alpha.size),
        "transparent_pixels": int(np.count_nonzero(alpha == 0.0)),
        "partial_pixels": int(np.count_nonzero((alpha > 0.0) & (alpha < 1.0))),
        "opaque_pixels": int(np.count_nonzero(alpha == 1.0)),
    }
    report_path = args.report or args.output.with_suffix(".chroma.json")
    report_path.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
