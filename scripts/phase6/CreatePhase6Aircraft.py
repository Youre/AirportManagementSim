"""Build the approved Riverbend Longreach 787-9 Paper2D source set."""

from __future__ import annotations

import hashlib
import json
import subprocess
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont


PROJECT_ROOT = Path(__file__).resolve().parents[2]
SOURCE_DIR = PROJECT_ROOT / "SourceAssets" / "Phase6" / "Aircraft"
MASTER_SVG = SOURCE_DIR / "Riverbend_Longreach_787-9.svg"
MASTER_PNG = SOURCE_DIR / "Riverbend_Longreach_787-9_master.png"
REVIEW_BOARD = SOURCE_DIR / "Riverbend_Longreach_787-9_heading-review.png"
MANIFEST = SOURCE_DIR / "aircraft-manifest.json"
CHROME_CANDIDATES = (
    Path(r"C:\Program Files\Google\Chrome\Application\chrome.exe"),
    Path(r"C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe"),
)
SOURCE_CHECKSUM = (
    "6AA56CD2CDD7035E3EE3526CFF5A603751E03441DA11ECE6C7E6E550B9B04449"
)
NAVY = (6, 19, 31, 255)
CYAN = (8, 169, 209, 255)
AMBER = (255, 183, 44, 255)
IVORY = (246, 242, 230, 255)


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest().upper()


def font(size: int, bold: bool = False) -> ImageFont.FreeTypeFont:
    name = "segoeuib.ttf" if bold else "segoeui.ttf"
    return ImageFont.truetype(str(Path(r"C:\Windows\Fonts") / name), size)


def rasterize_master() -> None:
    chrome = next((path for path in CHROME_CANDIDATES if path.exists()), None)
    if chrome is None:
        raise RuntimeError("Chrome or Edge is required to rasterize the SVG.")
    command = [
        str(chrome),
        "--headless",
        "--disable-gpu",
        "--hide-scrollbars",
        "--force-device-scale-factor=1",
        "--default-background-color=00000000",
        "--window-size=2048,2048",
        f"--screenshot={MASTER_PNG}",
        MASTER_SVG.as_uri(),
    ]
    subprocess.run(command, check=True, capture_output=True, text=True)
    with Image.open(MASTER_PNG) as image:
        converted = image.convert("RGBA")
        if converted.size != (2048, 2048):
            raise RuntimeError(f"Unexpected master dimensions: {converted.size}")
        if converted.getchannel("A").getbbox() is None:
            raise RuntimeError("Rasterized master has no visible alpha content.")
        converted.save(MASTER_PNG, optimize=True)


def generate_headings() -> list[dict[str, object]]:
    with Image.open(MASTER_PNG) as source_image:
        source = source_image.convert("RGBA")
        records: list[dict[str, object]] = []
        for index in range(16):
            heading = index * 22.5
            rotation = 90.0 - heading
            result = source.rotate(
                rotation,
                resample=Image.Resampling.BICUBIC,
                expand=True,
            )
            path = SOURCE_DIR / (
                f"T_RiverbendLongreach787_9_H{index:02d}.png"
            )
            result.save(path, optimize=True)
            records.append(
                {
                    "index": index,
                    "heading_degrees": heading,
                    "source_rotation_degrees": rotation,
                    "file": path.name,
                    "width": result.width,
                    "height": result.height,
                    "pivot": [0.5, 0.5],
                    "sha256": sha256(path),
                }
            )
        return records


def make_review_board(records: list[dict[str, object]]) -> None:
    board = Image.new("RGBA", (1920, 1080), NAVY)
    draw = ImageDraw.Draw(board)
    draw.rounded_rectangle(
        (24, 24, 1896, 1056),
        radius=26,
        outline=CYAN,
        width=3,
        fill=(7, 28, 43, 255),
    )
    draw.text(
        (64, 48),
        "RIVERBEND LONGREACH 787-9",
        font=font(42, bold=True),
        fill=IVORY,
    )
    draw.text(
        (64, 104),
        "RIVER & SUN · 16 HEADING REVIEW · 32 PX/M",
        font=font(22, bold=True),
        fill=CYAN,
    )
    draw.text(
        (1430, 58),
        "APPROVED DIRECTION",
        font=font(20, bold=True),
        fill=AMBER,
    )

    cell_width = 440
    cell_height = 200
    start_x = 64
    start_y = 166
    for record in records:
        index = int(record["index"])
        row, column = divmod(index, 4)
        left = start_x + column * 456
        top = start_y + row * 214
        draw.rounded_rectangle(
            (left, top, left + cell_width, top + cell_height),
            radius=16,
            fill=(9, 37, 54, 255),
            outline=(23, 82, 104, 255),
            width=2,
        )
        path = SOURCE_DIR / str(record["file"])
        with Image.open(path) as heading_image:
            aircraft = heading_image.convert("RGBA")
            aircraft.thumbnail((330, 160), Image.Resampling.LANCZOS)
            x = left + (cell_width - aircraft.width) // 2
            y = top + 28 + (150 - aircraft.height) // 2
            board.alpha_composite(aircraft, (x, y))
        draw.text(
            (left + 14, top + 10),
            f"H{index:02d} · {float(record['heading_degrees']):05.1f}°",
            font=font(17, bold=True),
            fill=IVORY,
        )

    draw.line((64, 1030, 1856, 1030), fill=(23, 82, 104, 255), width=2)
    draw.text(
        (64, 1034),
        "TRUE DIMENSIONS 62.8 M × 60.1 M  ·  CENTER PIVOT  ·  "
        "STRICT TOP-DOWN 2D  ·  NO GRADIENTS",
        font=font(17, bold=True),
        fill=(167, 194, 200, 255),
    )
    board.convert("RGB").save(REVIEW_BOARD, quality=95, optimize=True)


def write_manifest(records: list[dict[str, object]]) -> None:
    approved_source = SOURCE_DIR / "Boeing_787-9_source-approved.png"
    if sha256(approved_source) != SOURCE_CHECKSUM:
        raise RuntimeError("Approved external source checksum changed.")
    payload = {
        "schema_version": 1,
        "asset_id": "Aircraft.Widebody.RiverbendLongreach787_9",
        "display_name": "Riverbend Longreach 787-9",
        "approval": {
            "owner": "Dave",
            "date": "2026-07-28",
            "treatment": "River & Sun",
        },
        "external_source": {
            "file": approved_source.name,
            "sha256": SOURCE_CHECKSUM,
            "width": 719,
            "height": 776,
            "cooked": False,
        },
        "production_master": {
            "file": MASTER_SVG.name,
            "sha256": sha256(MASTER_SVG),
            "width": 2048,
            "height": 2048,
            "orientation": "nose-right/east",
            "runtime_dimension": "2D",
            "required_3d_assets": 0,
        },
        "physical_dimensions_meters": {
            "length": 62.8,
            "span": 60.1,
        },
        "pixels_per_meter": 32,
        "livery": {
            "operator": "Riverbend Longreach",
            "fictional": True,
            "palette": {
                "navy": "#0B1E3A",
                "cyan": "#08A9D1",
                "amber": "#FFB72C",
                "ivory": "#F6F2E6",
            },
        },
        "master_raster": {
            "file": MASTER_PNG.name,
            "sha256": sha256(MASTER_PNG),
            "width": 2048,
            "height": 2048,
        },
        "heading_convention": {
            "count": 16,
            "step_degrees": 22.5,
            "h000": "nose-up/north",
            "clockwise": True,
        },
        "headings": records,
        "review_board": {
            "file": REVIEW_BOARD.name,
            "sha256": sha256(REVIEW_BOARD),
            "width": 1920,
            "height": 1080,
        },
    }
    MANIFEST.write_text(
        json.dumps(payload, indent=2) + "\n",
        encoding="utf-8",
        newline="\n",
    )


def main() -> None:
    rasterize_master()
    headings = generate_headings()
    make_review_board(headings)
    write_manifest(headings)
    print(f"Created {len(headings)} headings and {REVIEW_BOARD.name}")


if __name__ == "__main__":
    main()
