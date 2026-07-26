[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$CandidateDirectory,

    [Parameter(Mandatory = $true)]
    [string]$AircraftSource,

    [string]$OutputDirectory = (Join-Path $PSScriptRoot '..\..\SourceAssets\Phase1.5')
)

$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing

function Get-ResolvedBoundedPath {
    param([string]$Path, [string]$AllowedRoot)
    $resolved = [System.IO.Path]::GetFullPath($Path)
    $root = [System.IO.Path]::GetFullPath($AllowedRoot)
    if (-not $resolved.StartsWith($root, [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "Resolved path '$resolved' is outside '$root'."
    }
    return $resolved
}

function Convert-HexColor {
    param([string]$Hex)
    return [System.Drawing.ColorTranslator]::FromHtml($Hex)
}

function New-SeamlessSurface {
    param(
        [string]$InputPath,
        [string]$OutputPath,
        [string]$BaseHex,
        [float]$SourceOpacity
    )

    $source = [System.Drawing.Image]::FromFile($InputPath)
    try {
        $quarter = New-Object System.Drawing.Bitmap 256, 256
        try {
            $quarterGraphics = [System.Drawing.Graphics]::FromImage($quarter)
            try {
                $quarterGraphics.Clear((Convert-HexColor $BaseHex))
                $quarterGraphics.CompositingQuality = [System.Drawing.Drawing2D.CompositingQuality]::HighQuality
                $quarterGraphics.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
                $attributes = New-Object System.Drawing.Imaging.ImageAttributes
                try {
                    $matrix = New-Object System.Drawing.Imaging.ColorMatrix
                    $matrix.Matrix33 = $SourceOpacity
                    $attributes.SetColorMatrix($matrix)
                    $cropSize = [Math]::Min($source.Width, $source.Height) * 0.75
                    $cropX = ($source.Width - $cropSize) / 2
                    $cropY = ($source.Height - $cropSize) / 2
                    $quarterGraphics.DrawImage(
                        $source,
                        (New-Object System.Drawing.Rectangle 0, 0, 256, 256),
                        [single]$cropX,
                        [single]$cropY,
                        [single]$cropSize,
                        [single]$cropSize,
                        [System.Drawing.GraphicsUnit]::Pixel,
                        $attributes)
                } finally {
                    $attributes.Dispose()
                }
            } finally {
                $quarterGraphics.Dispose()
            }

            $surface = New-Object System.Drawing.Bitmap 512, 512
            try {
                $surfaceGraphics = [System.Drawing.Graphics]::FromImage($surface)
                try {
                    $surfaceGraphics.DrawImageUnscaled($quarter, 0, 0)
                    $horizontal = $quarter.Clone()
                    try {
                        $horizontal.RotateFlip([System.Drawing.RotateFlipType]::RotateNoneFlipX)
                        $surfaceGraphics.DrawImageUnscaled($horizontal, 256, 0)
                    } finally {
                        $horizontal.Dispose()
                    }
                    $vertical = $quarter.Clone()
                    try {
                        $vertical.RotateFlip([System.Drawing.RotateFlipType]::RotateNoneFlipY)
                        $surfaceGraphics.DrawImageUnscaled($vertical, 0, 256)
                    } finally {
                        $vertical.Dispose()
                    }
                    $both = $quarter.Clone()
                    try {
                        $both.RotateFlip([System.Drawing.RotateFlipType]::RotateNoneFlipXY)
                        $surfaceGraphics.DrawImageUnscaled($both, 256, 256)
                    } finally {
                        $both.Dispose()
                    }
                } finally {
                    $surfaceGraphics.Dispose()
                }
                $surface.Save($OutputPath, [System.Drawing.Imaging.ImageFormat]::Png)
            } finally {
                $surface.Dispose()
            }
        } finally {
            $quarter.Dispose()
        }
    } finally {
        $source.Dispose()
    }
}

function New-PropSources {
    param([string]$Directory)

    $white = New-Object System.Drawing.Bitmap 64, 64
    try {
        $graphics = [System.Drawing.Graphics]::FromImage($white)
        try { $graphics.Clear([System.Drawing.Color]::White) } finally { $graphics.Dispose() }
        $white.Save((Join-Path $Directory 'T_WhiteSquare.png'), [System.Drawing.Imaging.ImageFormat]::Png)
    } finally { $white.Dispose() }

    $hut = New-Object System.Drawing.Bitmap 256, 256
    try {
        $graphics = [System.Drawing.Graphics]::FromImage($hut)
        try {
            $graphics.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::AntiAlias
            $graphics.Clear([System.Drawing.Color]::Transparent)
            $outline = New-Object System.Drawing.Pen (Convert-HexColor '#09141D'), 12
            $roof = New-Object System.Drawing.SolidBrush (Convert-HexColor '#B9AA8E')
            $door = New-Object System.Drawing.SolidBrush (Convert-HexColor '#122433')
            try {
                $graphics.FillRectangle($roof, 34, 50, 188, 156)
                $graphics.DrawRectangle($outline, 34, 50, 188, 156)
                $graphics.FillRectangle($door, 100, 146, 56, 60)
                $graphics.DrawLine($outline, 34, 96, 222, 96)
            } finally {
                $outline.Dispose()
                $roof.Dispose()
                $door.Dispose()
            }
        } finally { $graphics.Dispose() }
        $hut.Save((Join-Path $Directory 'T_OperationsHut.png'), [System.Drawing.Imaging.ImageFormat]::Png)
    } finally { $hut.Dispose() }

    $windsock = New-Object System.Drawing.Bitmap 160, 160
    try {
        $graphics = [System.Drawing.Graphics]::FromImage($windsock)
        try {
            $graphics.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::AntiAlias
            $graphics.Clear([System.Drawing.Color]::Transparent)
            $pole = New-Object System.Drawing.Pen (Convert-HexColor '#09141D'), 8
            $red = New-Object System.Drawing.SolidBrush (Convert-HexColor '#F06A66')
            $whiteBrush = New-Object System.Drawing.SolidBrush (Convert-HexColor '#EAF5F8')
            try {
                $graphics.DrawLine($pole, 40, 28, 40, 140)
                $graphics.FillPolygon($red, @(
                    (New-Object System.Drawing.Point 42, 32),
                    (New-Object System.Drawing.Point 138, 58),
                    (New-Object System.Drawing.Point 42, 82)))
                $graphics.FillPolygon($whiteBrush, @(
                    (New-Object System.Drawing.Point 68, 39),
                    (New-Object System.Drawing.Point 92, 46),
                    (New-Object System.Drawing.Point 68, 72)))
            } finally {
                $pole.Dispose()
                $red.Dispose()
                $whiteBrush.Dispose()
            }
        } finally { $graphics.Dispose() }
        $windsock.Save((Join-Path $Directory 'T_Windsock.png'), [System.Drawing.Imaging.ImageFormat]::Png)
    } finally { $windsock.Dispose() }

    $selection = New-Object System.Drawing.Bitmap 256, 256
    try {
        $graphics = [System.Drawing.Graphics]::FromImage($selection)
        try {
            $graphics.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::AntiAlias
            $graphics.Clear([System.Drawing.Color]::Transparent)
            $pen = New-Object System.Drawing.Pen (Convert-HexColor '#51D4E8'), 16
            try { $graphics.DrawEllipse($pen, 18, 18, 220, 220) } finally { $pen.Dispose() }
        } finally { $graphics.Dispose() }
        $selection.Save((Join-Path $Directory 'T_SelectionRing.png'), [System.Drawing.Imaging.ImageFormat]::Png)
    } finally { $selection.Dispose() }
}

function New-AircraftHeadings {
    param([string]$SourcePath, [string]$Directory)

    $source = [System.Drawing.Image]::FromFile($SourcePath)
    try {
        for ($index = 0; $index -lt 16; $index++) {
            $canvas = New-Object System.Drawing.Bitmap 512, 512
            try {
                $graphics = [System.Drawing.Graphics]::FromImage($canvas)
                try {
                    $graphics.Clear([System.Drawing.Color]::Transparent)
                    $graphics.CompositingQuality = [System.Drawing.Drawing2D.CompositingQuality]::HighQuality
                    $graphics.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
                    $graphics.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::HighQuality
                    $graphics.TranslateTransform(256, 256)
                    $graphics.RotateTransform([single]($index * 22.5))
                    $graphics.DrawImage($source, -133, -176, 266, 352)
                    $graphics.ResetTransform()
                } finally {
                    $graphics.Dispose()
                }
                $name = 'T_Cessna152_Heading_{0:D2}.png' -f $index
                $canvas.Save((Join-Path $Directory $name), [System.Drawing.Imaging.ImageFormat]::Png)
            } finally {
                $canvas.Dispose()
            }
        }
    } finally {
        $source.Dispose()
    }
}

$projectRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..'))
$outputRoot = Get-ResolvedBoundedPath $OutputDirectory $projectRoot
$surfaceDirectory = Join-Path $outputRoot 'Surfaces'
$propDirectory = Join-Path $outputRoot 'Props'
$aircraftDirectory = Join-Path $outputRoot 'Aircraft'
New-Item -ItemType Directory -Force -Path $surfaceDirectory, $propDirectory, $aircraftDirectory | Out-Null

$candidates = Get-ChildItem -LiteralPath $CandidateDirectory -Filter '*.png' |
    Where-Object { $_.Name -ne 'surface-contact-sheet.png' }
$grass = $candidates | Where-Object Name -Like '*c8bdf495*' | Select-Object -First 1
$runway = $candidates | Where-Object Name -Like '*448f9cfe*' | Select-Object -First 1
$taxi = $candidates | Where-Object Name -Like '*85ac9585*' | Select-Object -First 1
$stand = $candidates | Where-Object Name -Like '*54b9ea33*' | Select-Object -First 1
if (-not $grass -or -not $runway -or -not $taxi -or -not $stand) {
    throw 'Expected all four reviewed ComfyUI candidates.'
}
if (-not (Test-Path -LiteralPath $AircraftSource -PathType Leaf)) {
    throw "Aircraft source '$AircraftSource' does not exist."
}

New-SeamlessSurface $grass.FullName (Join-Path $surfaceDirectory 'T_TemperateGrass.png') '#5F8F57' 0.18
New-SeamlessSurface $runway.FullName (Join-Path $surfaceDirectory 'T_GrassRunway.png') '#789A68' 0.24
New-SeamlessSurface $taxi.FullName (Join-Path $surfaceDirectory 'T_TaxiWear.png') '#B09A68' 0.20
New-SeamlessSurface $stand.FullName (Join-Path $surfaceDirectory 'T_WarmStand.png') '#B9AA8E' 0.12
New-PropSources $propDirectory

$approvedAircraftSource = Join-Path $aircraftDirectory 'Cessna_152-JWsK-d3E.png'
Copy-Item -LiteralPath $AircraftSource -Destination $approvedAircraftSource -Force
New-AircraftHeadings $approvedAircraftSource $aircraftDirectory

Get-ChildItem -LiteralPath $outputRoot -Recurse -File | Sort-Object FullName | ForEach-Object {
    [PSCustomObject]@{
        Path = $_.FullName.Substring($projectRoot.Length + 1)
        Bytes = $_.Length
        SHA256 = (Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName).Hash
    }
}

