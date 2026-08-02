[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$RunwayCandidate,

    [Parameter(Mandatory = $true)]
    [string]$TaxiwayCandidate,

    [string]$OutputDirectory = (Join-Path $PSScriptRoot '..\..\SourceAssets\Phase1.5\Surfaces'),

    [string]$ReviewDirectory = (Join-Path $PSScriptRoot '..\..\.ai\scratch\runway-taxi-surface-review-20260802')
)

$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing

function Get-BoundedPath {
    param([string]$Path, [string]$AllowedRoot)

    $resolved = [IO.Path]::GetFullPath($Path)
    $root = [IO.Path]::GetFullPath($AllowedRoot)
    if (-not $resolved.StartsWith($root, [StringComparison]::OrdinalIgnoreCase)) {
        throw "Resolved path '$resolved' is outside '$root'."
    }
    return $resolved
}

function Convert-HexColor {
    param([string]$Hex)
    return [Drawing.ColorTranslator]::FromHtml($Hex)
}

function Draw-WithOpacity {
    param(
        [Drawing.Graphics]$Graphics,
        [Drawing.Image]$Image,
        [Drawing.Rectangle]$Destination,
        [float]$Opacity
    )

    $attributes = New-Object Drawing.Imaging.ImageAttributes
    try {
        $matrix = New-Object Drawing.Imaging.ColorMatrix
        $matrix.Matrix33 = $Opacity
        $attributes.SetColorMatrix($matrix)
        $Graphics.DrawImage(
            $Image,
            $Destination,
            0,
            0,
            $Image.Width,
            $Image.Height,
            [Drawing.GraphicsUnit]::Pixel,
            $attributes)
    } finally {
        $attributes.Dispose()
    }
}

function New-SeamlessBase {
    param(
        [string]$InputPath,
        [string]$BaseHex,
        [float]$SourceOpacity
    )

    $source = [Drawing.Image]::FromFile($InputPath)
    try {
        $cropSize = [Math]::Floor([Math]::Min($source.Width, $source.Height) * 0.72)
        $cropX = [Math]::Floor(($source.Width - $cropSize) / 2)
        $cropY = [Math]::Floor(($source.Height - $cropSize) / 2)
        $quarter = New-Object Drawing.Bitmap 256, 256
        try {
            $quarterGraphics = [Drawing.Graphics]::FromImage($quarter)
            try {
                $quarterGraphics.Clear((Convert-HexColor $BaseHex))
                $quarterGraphics.CompositingQuality = [Drawing.Drawing2D.CompositingQuality]::HighQuality
                $quarterGraphics.InterpolationMode = [Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
                $attributes = New-Object Drawing.Imaging.ImageAttributes
                try {
                    $matrix = New-Object Drawing.Imaging.ColorMatrix
                    $matrix.Matrix33 = $SourceOpacity
                    $attributes.SetColorMatrix($matrix)
                    $quarterGraphics.DrawImage(
                        $source,
                        (New-Object Drawing.Rectangle 0, 0, 256, 256),
                        $cropX,
                        $cropY,
                        $cropSize,
                        $cropSize,
                        [Drawing.GraphicsUnit]::Pixel,
                        $attributes)
                } finally {
                    $attributes.Dispose()
                }
            } finally {
                $quarterGraphics.Dispose()
            }

            $surface = New-Object Drawing.Bitmap 512, 512
            $surfaceGraphics = [Drawing.Graphics]::FromImage($surface)
            try {
                $surfaceGraphics.DrawImageUnscaled($quarter, 0, 0)
                $horizontal = $quarter.Clone()
                $vertical = $quarter.Clone()
                $both = $quarter.Clone()
                try {
                    $horizontal.RotateFlip([Drawing.RotateFlipType]::RotateNoneFlipX)
                    $vertical.RotateFlip([Drawing.RotateFlipType]::RotateNoneFlipY)
                    $both.RotateFlip([Drawing.RotateFlipType]::RotateNoneFlipXY)
                    $surfaceGraphics.DrawImageUnscaled($horizontal, 256, 0)
                    $surfaceGraphics.DrawImageUnscaled($vertical, 0, 256)
                    $surfaceGraphics.DrawImageUnscaled($both, 256, 256)
                } finally {
                    $horizontal.Dispose()
                    $vertical.Dispose()
                    $both.Dispose()
                }
            } finally {
                $surfaceGraphics.Dispose()
            }
            return $surface
        } finally {
            $quarter.Dispose()
        }
    } finally {
        $source.Dispose()
    }
}

function Add-RunwayMarkings {
    param([Drawing.Bitmap]$Surface)

    $graphics = [Drawing.Graphics]::FromImage($Surface)
    try {
        $graphics.SmoothingMode = [Drawing.Drawing2D.SmoothingMode]::AntiAlias
        $marking = Convert-HexColor '#E7E5D7'
        $edgePen = New-Object Drawing.Pen $marking, 8
        $centerPen = New-Object Drawing.Pen $marking, 7
        $thresholdBrush = New-Object Drawing.SolidBrush $marking
        try {
            $graphics.DrawLine($edgePen, 0, 32, 512, 32)
            $graphics.DrawLine($edgePen, 0, 480, 512, 480)
            for ($x = 92; $x -le 392; $x += 50) {
                $graphics.DrawLine($centerPen, $x, 256, $x + 28, 256)
            }
            foreach ($y in @(78, 112, 146, 180, 332, 366, 400, 434)) {
                $graphics.FillRectangle($thresholdBrush, 28, $y, 34, 9)
                $graphics.FillRectangle($thresholdBrush, 450, $y, 34, 9)
            }
        } finally {
            $edgePen.Dispose()
            $centerPen.Dispose()
            $thresholdBrush.Dispose()
        }
    } finally {
        $graphics.Dispose()
    }
}

function Add-TaxiwayMarkings {
    param([Drawing.Bitmap]$Surface)

    $graphics = [Drawing.Graphics]::FromImage($Surface)
    try {
        $graphics.SmoothingMode = [Drawing.Drawing2D.SmoothingMode]::AntiAlias
        $underlay = New-Object Drawing.Pen (Convert-HexColor '#3A3422'), 12
        $center = New-Object Drawing.Pen (Convert-HexColor '#F2BE3E'), 7
        $edge = New-Object Drawing.Pen (Convert-HexColor '#D8A72E'), 4
        try {
            $graphics.DrawLine($underlay, 0, 256, 512, 256)
            $graphics.DrawLine($center, 0, 256, 512, 256)
            $graphics.DrawLine($edge, 0, 35, 512, 35)
            $graphics.DrawLine($edge, 0, 477, 512, 477)
        } finally {
            $underlay.Dispose()
            $center.Dispose()
            $edge.Dispose()
        }
    } finally {
        $graphics.Dispose()
    }
}

function New-TileReview {
    param(
        [Drawing.Bitmap]$Surface,
        [string]$OutputPath
    )

    $review = New-Object Drawing.Bitmap 1024, 1024
    try {
        $graphics = [Drawing.Graphics]::FromImage($review)
        try {
            $graphics.DrawImageUnscaled($Surface, 0, 0)
            $graphics.DrawImageUnscaled($Surface, 512, 0)
            $graphics.DrawImageUnscaled($Surface, 0, 512)
            $graphics.DrawImageUnscaled($Surface, 512, 512)
        } finally {
            $graphics.Dispose()
        }
        $review.Save($OutputPath, [Drawing.Imaging.ImageFormat]::Png)
    } finally {
        $review.Dispose()
    }
}

$projectRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..'))
$outputRoot = Get-BoundedPath $OutputDirectory $projectRoot
$reviewRoot = Get-BoundedPath $ReviewDirectory $projectRoot
$runwayInput = Get-BoundedPath $RunwayCandidate $projectRoot
$taxiwayInput = Get-BoundedPath $TaxiwayCandidate $projectRoot
foreach ($input in @($runwayInput, $taxiwayInput)) {
    if (-not (Test-Path -LiteralPath $input -PathType Leaf)) {
        throw "Surface candidate is missing: $input"
    }
}

$generatedRoot = Join-Path $outputRoot 'Generated'
New-Item -ItemType Directory -Force -Path $outputRoot, $generatedRoot, $reviewRoot | Out-Null
$runwayMaster = Join-Path $generatedRoot 'runway-aggregate-comfyui-master.png'
$taxiwayMaster = Join-Path $generatedRoot 'taxiway-aggregate-comfyui-master.png'
Copy-Item -LiteralPath $runwayInput -Destination $runwayMaster -Force
Copy-Item -LiteralPath $taxiwayInput -Destination $taxiwayMaster -Force

$runway = New-SeamlessBase $runwayMaster '#343B3E' 0.72
try {
    Add-RunwayMarkings $runway
    $runwayPath = Join-Path $outputRoot 'T_RunwayMarked.png'
    $runway.Save($runwayPath, [Drawing.Imaging.ImageFormat]::Png)
    New-TileReview $runway (Join-Path $reviewRoot 'runway-marked-2x2.png')
} finally {
    $runway.Dispose()
}

$taxiway = New-SeamlessBase $taxiwayMaster '#414244' 0.64
try {
    Add-TaxiwayMarkings $taxiway
    $taxiwayPath = Join-Path $outputRoot 'T_TaxiwayMarked.png'
    $taxiway.Save($taxiwayPath, [Drawing.Imaging.ImageFormat]::Png)
    New-TileReview $taxiway (Join-Path $reviewRoot 'taxiway-marked-2x2.png')
} finally {
    $taxiway.Dispose()
}

Get-ChildItem -LiteralPath $generatedRoot, $outputRoot, $reviewRoot -File |
    Sort-Object FullName -Unique |
    ForEach-Object {
        [PSCustomObject]@{
            Path = $_.FullName.Substring($projectRoot.Length + 1)
            Dimensions = if ($_.Extension -eq '.png') {
                $image = [Drawing.Image]::FromFile($_.FullName)
                try { "$($image.Width)x$($image.Height)" } finally { $image.Dispose() }
            } else { '' }
            Bytes = $_.Length
            SHA256 = (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash
        }
    }
