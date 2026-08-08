param(
    [string]$RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
)

$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing

$reviewRoot = Join-Path $RepositoryRoot 'docs\planning\50-production\terminal-growth-visual-validation'
$pairs = @(
    @{
        Name = 'comparison-01-starter-ga-terminal-editor.png'
        Target = Join-Path $reviewRoot 'target-01-starter-ga-terminal-editor.png'
        Runtime = Join-Path $RepositoryRoot 'AMSim\Saved\TerminalGrowth\ScaleMatrix\100\starter-ga-terminal-build.png'
        RuntimeCopy = Join-Path $reviewRoot 'runtime-01-starter-ga-terminal-editor.png'
        LeftLabel = 'APPROVED TARGET | STARTER GA TERMINAL'
        RightLabel = 'RUNTIME | 1920x1080 / 100%'
    },
    @{
        Name = 'comparison-02-grown-regional-terminal-operations.png'
        Target = Join-Path $reviewRoot 'target-02-grown-regional-terminal-operations.png'
        Runtime = Join-Path $RepositoryRoot 'AMSim\Saved\TerminalGrowth\ScaleMatrix\100\grown-regional-terminal-operations.png'
        RuntimeCopy = Join-Path $reviewRoot 'runtime-02-grown-regional-terminal-operations.png'
        LeftLabel = 'APPROVED TARGET | GROWN REGIONAL TERMINAL'
        RightLabel = 'RUNTIME | PASSENGER OPERATIONS'
    }
)

foreach ($pair in $pairs) {
    Copy-Item -LiteralPath $pair.Runtime -Destination $pair.RuntimeCopy -Force
    $target = [System.Drawing.Image]::FromFile($pair.Target)
    $runtime = [System.Drawing.Image]::FromFile($pair.Runtime)
    try {
        $headerHeight = 88
        $board = New-Object System.Drawing.Bitmap 3840, (1080 + $headerHeight)
        $graphics = [System.Drawing.Graphics]::FromImage($board)
        try {
            $graphics.Clear([System.Drawing.Color]::FromArgb(6, 36, 51))
            $graphics.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
            $graphics.DrawImage($target, 0, $headerHeight, 1920, 1080)
            $graphics.DrawImage($runtime, 1920, $headerHeight, 1920, 1080)
            $font = New-Object System.Drawing.Font 'Segoe UI Semibold', 24
            $cyan = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(128, 224, 239))
            $divider = New-Object System.Drawing.Pen ([System.Drawing.Color]::FromArgb(128, 224, 239)), 3
            try {
                $graphics.DrawString($pair.LeftLabel, $font, $cyan, 32, 24)
                $graphics.DrawString($pair.RightLabel, $font, $cyan, 1952, 24)
                $graphics.DrawLine($divider, 1920, 0, 1920, 1168)
            }
            finally {
                $font.Dispose()
                $cyan.Dispose()
                $divider.Dispose()
            }
        }
        finally {
            $graphics.Dispose()
        }
        $board.Save((Join-Path $reviewRoot $pair.Name), [System.Drawing.Imaging.ImageFormat]::Png)
        $board.Dispose()
    }
    finally {
        $target.Dispose()
        $runtime.Dispose()
    }
}

$pairs | ForEach-Object { Join-Path $reviewRoot $_.Name }
