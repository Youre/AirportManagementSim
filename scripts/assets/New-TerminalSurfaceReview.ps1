param(
    [string]$CandidateDirectory = "SourceAssets/TerminalGrowth/Candidates/ComfyUI",
    [string]$OutputPath = "SourceAssets/TerminalGrowth/Reviews/terminal-surface-candidates.png"
)

$ErrorActionPreference = "Stop"
Add-Type -AssemblyName System.Drawing

$root = (Resolve-Path (Join-Path $PSScriptRoot "../..")).Path
$candidatePath = [System.IO.Path]::GetFullPath((Join-Path $root $CandidateDirectory))
$outputAbsolute = [System.IO.Path]::GetFullPath((Join-Path $root $OutputPath))
$outputDirectory = Split-Path -Parent $outputAbsolute
[System.IO.Directory]::CreateDirectory($outputDirectory) | Out-Null

$files = Get-ChildItem -LiteralPath $candidatePath -Filter "*.png" | Sort-Object Name
if ($files.Count -eq 0) {
    throw "No PNG candidates found in $candidatePath"
}

$canvas = New-Object System.Drawing.Bitmap 1920, 1080
$graphics = [System.Drawing.Graphics]::FromImage($canvas)
$images = New-Object System.Collections.Generic.List[System.Drawing.Image]
try {
    $graphics.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::HighQuality
    $graphics.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
    $graphics.Clear([System.Drawing.Color]::FromArgb(8, 35, 52))

    $titleFont = New-Object System.Drawing.Font("Segoe UI", 30, [System.Drawing.FontStyle]::Bold)
    $labelFont = New-Object System.Drawing.Font("Segoe UI", 14, [System.Drawing.FontStyle]::Bold)
    $smallFont = New-Object System.Drawing.Font("Consolas", 10, [System.Drawing.FontStyle]::Regular)
    $white = New-Object System.Drawing.SolidBrush([System.Drawing.Color]::FromArgb(236, 243, 240))
    $cyan = New-Object System.Drawing.Pen([System.Drawing.Color]::FromArgb(74, 205, 226), 2)
    $panelBrush = New-Object System.Drawing.SolidBrush([System.Drawing.Color]::FromArgb(15, 61, 82))

    $graphics.DrawString("TERMINAL SURFACE CANDIDATES - 2 x 2 SEAM TEST", $titleFont, $white, 48, 28)
    $graphics.DrawString("ComfyUI candidates only. Labels use family + request-id prefix.", $smallFont, $white, 52, 76)

    $columns = 5
    $cellWidth = 360
    $cellHeight = 450
    $startX = 48
    $startY = 112
    for ($index = 0; $index -lt $files.Count; ++$index) {
        $row = [math]::Floor($index / $columns)
        $column = $index % $columns
        $x = $startX + $column * $cellWidth
        $y = $startY + $row * $cellHeight
        $graphics.FillRectangle($panelBrush, $x, $y, 330, 420)
        $graphics.DrawRectangle($cyan, $x, $y, 330, 420)

        $image = [System.Drawing.Image]::FromFile($files[$index].FullName)
        $images.Add($image)
        $tileSize = 150
        for ($tileY = 0; $tileY -lt 2; ++$tileY) {
            for ($tileX = 0; $tileX -lt 2; ++$tileX) {
                $graphics.DrawImage($image, $x + 15 + $tileX * $tileSize, $y + 15 + $tileY * $tileSize, $tileSize, $tileSize)
            }
        }

        if ($files[$index].Name -match "public-airport-terminal-floor") {
            $family = "PUBLIC FLOOR"
        } elseif ($files[$index].Name -match "staff-and-service") {
            $family = "SERVICE FLOOR"
        } else {
            $family = "ROOF"
        }
        $id = if ($files[$index].BaseName -match "_([0-9a-f]{8})-") { $Matches[1] } else { "unknown" }
        $graphics.DrawString("$family - $id", $labelFont, $white, $x + 16, $y + 326)
        $hash = (Get-FileHash -LiteralPath $files[$index].FullName -Algorithm SHA256).Hash.Substring(0, 16).ToLowerInvariant()
        $graphics.DrawString("sha256 $hash...", $smallFont, $white, $x + 16, $y + 360)
        $graphics.DrawString("Review: seam - scale - palette - focal noise", $smallFont, $white, $x + 16, $y + 382)
    }

    $canvas.Save($outputAbsolute, [System.Drawing.Imaging.ImageFormat]::Png)
}
finally {
    foreach ($image in $images) { $image.Dispose() }
    $graphics.Dispose()
    $canvas.Dispose()
}

Write-Output $outputAbsolute
