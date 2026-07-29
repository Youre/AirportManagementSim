[CmdletBinding()]
param(
    [string]$EngineRoot = 'C:\Program Files\Epic Games\UE_5.8'
)

$ErrorActionPreference = 'Stop'
$repoRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..'))
$project = Join-Path $repoRoot 'AMSim\AMSim.uproject'
$editor = Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'
$phase0Root = Join-Path $repoRoot 'AMSim\Saved\Phase0'
$outputRoot = Join-Path $repoRoot 'AMSim\Saved\Phase7\Scale'

foreach ($required in @($project, $editor)) {
    if (-not (Test-Path -LiteralPath $required -PathType Leaf)) {
        throw "Required Phase 7 scale-matrix path is missing: $required"
    }
}
if (Get-Process UnrealEditor -ErrorAction SilentlyContinue) {
    throw 'Close Unreal Editor before capturing the Phase 7 scale matrix.'
}

$resolvedOutput = [IO.Path]::GetFullPath($outputRoot)
$savedBoundary = [IO.Path]::GetFullPath((Join-Path $repoRoot 'AMSim\Saved'))
if (-not $resolvedOutput.StartsWith(
    $savedBoundary.TrimEnd('\') + '\',
    [StringComparison]::OrdinalIgnoreCase)) {
    throw "Refusing to reset a directory outside AMSim/Saved: $resolvedOutput"
}
if (Test-Path -LiteralPath $resolvedOutput) {
    Remove-Item -LiteralPath $resolvedOutput -Recurse -Force
}
New-Item -ItemType Directory -Path $resolvedOutput -Force | Out-Null

$captures = @()
foreach ($scale in @(1.0, 1.25, 1.5, 1.75, 2.0)) {
    $percent = [int]($scale * 100)
    $proof = Join-Path $phase0Root 'proof-screen.png'
    if (Test-Path -LiteralPath $proof) {
        Remove-Item -LiteralPath $proof -Force
    }
    $process = Start-Process `
        -FilePath $editor `
        -ArgumentList @(
            $project,
            '-game',
            '-AMSimPhase0Smoke',
            '-AMSimReleaseGuide',
            '-AMSimIgnoreUserProfile',
            '-windowed',
            '-ForceRes',
            '-ResX=1920',
            '-ResY=1080',
            '-unattended',
            '-nop4',
            '-NoSound',
            "-ini:Engine:[/Script/Engine.UserInterfaceSettings]:ApplicationScale=$scale"
        ) `
        -WindowStyle Hidden `
        -PassThru `
        -Wait
    if ($process.ExitCode -ne 0) {
        throw "Phase 7 guide capture failed at $percent percent UI scale."
    }
    if (-not (Test-Path -LiteralPath $proof -PathType Leaf)) {
        throw "Phase 7 guide capture is missing at $percent percent UI scale."
    }
    $destination = Join-Path $resolvedOutput "$percent-release-guide.png"
    Copy-Item -LiteralPath $proof -Destination $destination -Force
    $captures += [ordered]@{
        percent = $percent
        applicationScale = $scale
        path = $destination
        sha256 = (Get-FileHash -LiteralPath $destination -Algorithm SHA256).Hash
        passed = $true
    }
}

$result = [ordered]@{
    schema = 1
    generatedUtc = [DateTime]::UtcNow.ToString('o')
    resolution = '1920x1080'
    actualUnrealCaptures = $true
    conceptReferences = @('VA-01', 'VA-06', 'VA-07')
    captures = $captures
    passed = $captures.Count -eq 5
}
$resultPath = Join-Path $resolvedOutput 'scale-matrix.json'
$result | ConvertTo-Json -Depth 6 |
    Set-Content -LiteralPath $resultPath -Encoding utf8
Write-Host "Phase 7 scale matrix passed. Result: $resultPath"
