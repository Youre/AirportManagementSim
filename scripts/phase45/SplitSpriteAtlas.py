"""Split a reviewed transparent sprite atlas into cooker-ready source PNGs."""

from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path

from PIL import Image


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--columns", type=int, required=True)
    parser.add_argument("--rows", type=int, required=True)
    parser.add_argument("--names", required=True)
    parser.add_argument("--padding", type=int, default=12)
    parser.add_argument(
        "--rotate-clockwise",
        default="",
        help="Comma-separated sprite names to rotate 90 degrees clockwise.",
    )
    return parser.parse_args()


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def split_atlas(args: argparse.Namespace) -> None:
    names = [name.strip() for name in args.names.split(",") if name.strip()]
    expected = args.columns * args.rows
    if len(names) != expected:
        raise ValueError(f"Expected {expected} names, received {len(names)}")
    rotate_clockwise = {
        name.strip()
        for name in args.rotate_clockwise.split(",")
        if name.strip()
    }
    unknown_rotations = rotate_clockwise.difference(names)
    if unknown_rotations:
        raise ValueError(
            "Rotation names are not atlas cells: "
            + ", ".join(sorted(unknown_rotations))
        )

    atlas = Image.open(args.source).convert("RGBA")
    args.output.mkdir(parents=True, exist_ok=True)
    records: list[dict[str, object]] = []
    for index, name in enumerate(names):
        column = index % args.columns
        row = index // args.columns
        left = round(column * atlas.width / args.columns)
        right = round((column + 1) * atlas.width / args.columns)
        top = round(row * atlas.height / args.rows)
        bottom = round((row + 1) * atlas.height / args.rows)
        cell = atlas.crop((left, top, right, bottom))
        alpha_bounds = cell.getchannel("A").getbbox()
        if alpha_bounds is None:
            raise ValueError(f"Cell {index} ({name}) contains no visible pixels")
        trimmed = cell.crop(alpha_bounds)
        rotation_degrees = 0
        if name in rotate_clockwise:
            trimmed = trimmed.transpose(Image.Transpose.ROTATE_270)
            rotation_degrees = 90
        output_image = Image.new(
            "RGBA",
            (
                trimmed.width + 2 * args.padding,
                trimmed.height + 2 * args.padding,
            ),
            (0, 0, 0, 0),
        )
        output_image.alpha_composite(trimmed, (args.padding, args.padding))
        output_path = args.output / f"T_{name}.png"
        output_image.save(output_path, optimize=True)
        records.append(
            {
                "name": name,
                "cell": index,
                "source_box": [left, top, right, bottom],
                "alpha_bounds": list(alpha_bounds),
                "rotation_degrees_clockwise": rotation_degrees,
                "dimensions": list(output_image.size),
                "sha256": sha256(output_path),
            }
        )

    manifest = {
        "source": str(args.source.resolve()),
        "source_sha256": sha256(args.source),
        "grid": [args.columns, args.rows],
        "padding": args.padding,
        "sprites": records,
    }
    (args.output / "split-manifest.json").write_text(
        json.dumps(manifest, indent=2) + "\n",
        encoding="utf-8",
    )


if __name__ == "__main__":
    split_atlas(parse_args())
