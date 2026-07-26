[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [string]$ReferencePath,

    [Parameter(Mandatory)]
    [string]$CapturePath,

    [Parameter(Mandatory)]
    [string]$OutputPath,

    [string]$ReviewPath,

    [string]$ReferenceId = 'Unspecified',

    [string]$StateDescription = 'Describe the equivalent gameplay state'
)

$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing

$reference = (Resolve-Path -LiteralPath $ReferencePath).Path
$capture = (Resolve-Path -LiteralPath $CapturePath).Path
$output = [IO.Path]::GetFullPath($OutputPath)
if ([string]::IsNullOrWhiteSpace($ReviewPath)) {
    $ReviewPath = [IO.Path]::ChangeExtension($output, '.md')
}
$review = [IO.Path]::GetFullPath($ReviewPath)

foreach ($path in @($output, $review)) {
    $directory = Split-Path -Parent $path
    if ($directory -and !(Test-Path -LiteralPath $directory)) {
        New-Item -ItemType Directory -Path $directory -Force | Out-Null
    }
}

function Draw-LetterboxedImage {
    param(
        [System.Drawing.Graphics]$Graphics,
        [System.Drawing.Image]$Image,
        [System.Drawing.RectangleF]$Bounds
    )

    $scale = [Math]::Min($Bounds.Width / $Image.Width, $Bounds.Height / $Image.Height)
    $width = [single]($Image.Width * $scale)
    $height = [single]($Image.Height * $scale)
    $x = [single]($Bounds.X + (($Bounds.Width - $width) / 2.0))
    $y = [single]($Bounds.Y + (($Bounds.Height - $height) / 2.0))
    $Graphics.FillRectangle(
        [System.Drawing.Brushes]::Black,
        [System.Drawing.RectangleF]::new($x, $y, $width, $height))
    $Graphics.DrawImage(
        $Image,
        [System.Drawing.RectangleF]::new($x, $y, $width, $height))
}

$canvas = $null
$graphics = $null
$referenceImage = $null
$captureImage = $null
$titleFont = $null
$labelFont = $null
$textBrush = $null
$mutedBrush = $null

try {
    $canvas = [System.Drawing.Bitmap]::new(1920, 640)
    $graphics = [System.Drawing.Graphics]::FromImage($canvas)
    $graphics.Clear([System.Drawing.Color]::FromArgb(9, 20, 29))
    $graphics.CompositingQuality = [System.Drawing.Drawing2D.CompositingQuality]::HighQuality
    $graphics.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
    $graphics.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::HighQuality

    $referenceImage = [System.Drawing.Image]::FromFile($reference)
    $captureImage = [System.Drawing.Image]::FromFile($capture)
    $titleFont = [System.Drawing.Font]::new('Segoe UI', 20, [System.Drawing.FontStyle]::Bold)
    $labelFont = [System.Drawing.Font]::new('Segoe UI', 14, [System.Drawing.FontStyle]::Regular)
    $textBrush = [System.Drawing.SolidBrush]::new([System.Drawing.Color]::FromArgb(234, 245, 248))
    $mutedBrush = [System.Drawing.SolidBrush]::new([System.Drawing.Color]::FromArgb(175, 197, 206))

    $graphics.DrawString(
        "$ReferenceId visual comparison",
        $titleFont,
        $textBrush,
        [System.Drawing.PointF]::new(24, 16))
    $graphics.DrawString(
        $StateDescription,
        $labelFont,
        $mutedBrush,
        [System.Drawing.PointF]::new(24, 50))
    $graphics.DrawString('CONCEPT REFERENCE', $labelFont, $textBrush, [System.Drawing.PointF]::new(24, 86))
    $graphics.DrawString('IMPLEMENTATION CAPTURE', $labelFont, $textBrush, [System.Drawing.PointF]::new(984, 86))

    Draw-LetterboxedImage -Graphics $graphics -Image $referenceImage -Bounds ([System.Drawing.RectangleF]::new(24, 120, 912, 496))
    Draw-LetterboxedImage -Graphics $graphics -Image $captureImage -Bounds ([System.Drawing.RectangleF]::new(984, 120, 912, 496))

    $canvas.Save($output, [System.Drawing.Imaging.ImageFormat]::Png)
}
finally {
    foreach ($item in @(
        $mutedBrush,
        $textBrush,
        $labelFont,
        $titleFont,
        $captureImage,
        $referenceImage,
        $graphics,
        $canvas
    )) {
        if ($null -ne $item) {
            $item.Dispose()
        }
    }
}

$reviewBody = @"
# $ReferenceId Visual Comparison

- Reference: $reference
- Capture: $capture
- Comparison board: $output
- Equivalent state: $StateDescription

## Review checklist

- [ ] World versus UI dominance matches the reference intent.
- [ ] Primary, secondary, and contextual actions have a clear hierarchy.
- [ ] Palette, surface treatment, outlines, and depth cues form one visual language.
- [ ] Typography size, weight, casing, and spacing are coherent.
- [ ] Panel density and whitespace support quick scanning.
- [ ] Operational state remains legible without relying on color alone.
- [ ] Mouse cursor remains visible and interactive in the gameplay viewport.

## Highest-impact mismatches

1. 
2. 
3. 

## Intentional differences

- 

## Next bounded iteration

- Target:
- Expected visual change:
- Acceptance observation:
"@

[IO.File]::WriteAllText($review, $reviewBody, [Text.UTF8Encoding]::new($false))

Write-Host "Comparison board: $output"
Write-Host "Review worksheet: $review"
