[CmdletBinding()]
param(
    [string]$EngineRoot = 'C:\Program Files\Epic Games\UE_5.8',
    [string]$OutputRoot = '',
    [ValidateSet(100, 125, 150, 175, 200)]
    [int[]]$ScalePercents = @(100, 125, 150, 175, 200),
    [switch]$SummarizeOnly
)

$ErrorActionPreference = 'Stop'
$repoRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..'))
$project = Join-Path $repoRoot 'AMSim\AMSim.uproject'
$editor = Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'
$savedRoot = Join-Path $repoRoot 'AMSim\Saved\Phase4'
$outputRoot = if ($OutputRoot) {
    [IO.Path]::GetFullPath($OutputRoot)
} else {
    Join-Path $savedRoot 'Scale'
}
$savedBoundary = [IO.Path]::GetFullPath((Join-Path $repoRoot 'AMSim\Saved'))
if (-not $outputRoot.StartsWith(
    $savedBoundary.TrimEnd('\') + '\',
    [StringComparison]::OrdinalIgnoreCase)) {
    throw "Scale evidence must remain under AMSim/Saved: $outputRoot"
}

$proofNames = @(
    'va04-regional-timetable.png',
    'va06-regional-incident.png',
    'phase4-complete.png',
    'smoke-result.json'
)
if (-not $SummarizeOnly) {
    foreach ($percent in $ScalePercents) {
        $scale = $percent / 100.0
        $destination = Join-Path $outputRoot $percent
        New-Item -ItemType Directory -Path $destination -Force | Out-Null
        $startedAt = [DateTime]::UtcNow
        $process = Start-Process `
            -FilePath $editor `
            -ArgumentList @(
                $project,
                '-game',
                '-AMSimPhase4Smoke',
                '-AMSimPhase1ReferenceProfile',
                '-windowed',
                '-ForceRes',
                '-ResX=1920',
                '-ResY=1080',
                '-unattended',
                '-nop4',
                "-ini:Engine:[/Script/Engine.UserInterfaceSettings]:ApplicationScale=$scale"
            ) `
            -WindowStyle Hidden `
            -PassThru `
            -Wait
        if ($process.ExitCode -ne 0) {
            throw "Rendered Phase 4 smoke exited with code $($process.ExitCode) at $percent%."
        }

        $smokePath = Join-Path $savedRoot 'smoke-result.json'
        if (-not (Test-Path -LiteralPath $smokePath)) {
            throw "Rendered Phase 4 smoke did not create $smokePath."
        }
        $smokeFile = Get-Item -LiteralPath $smokePath
        if ($smokeFile.LastWriteTimeUtc -lt $startedAt.AddSeconds(-1)) {
            throw "Rendered Phase 4 smoke result was not refreshed at $percent%."
        }
        $smoke = Get-Content -LiteralPath $smokePath -Raw | ConvertFrom-Json
        if (-not $smoke.passed -or $smoke.schema -ne 4) {
            throw "Rendered Phase 4 smoke failed at $percent%."
        }
        foreach ($proofName in $proofNames) {
            Copy-Item `
                -LiteralPath (Join-Path $savedRoot $proofName) `
                -Destination (Join-Path $destination $proofName) `
                -Force
        }
    }
}

$results = @()
foreach ($availablePercent in @(100, 125, 150, 175, 200)) {
    $directory = Join-Path $outputRoot $availablePercent
    $smokePath = Join-Path $directory 'smoke-result.json'
    if (Test-Path -LiteralPath $smokePath) {
        $smoke = Get-Content -LiteralPath $smokePath -Raw | ConvertFrom-Json
        $missingProofs = @($proofNames | Where-Object {
            -not (Test-Path -LiteralPath (Join-Path $directory $_))
        })
        $results += [ordered]@{
            scalePercent = $availablePercent
            passed = $smoke.passed -and $missingProofs.Count -eq 0
            compactMode = $availablePercent -ge 175
            p99FrameMilliseconds = $smoke.p99FrameMilliseconds
            missingProofs = $missingProofs
            proofDirectory = $directory
        }
    }
}
$summary = [ordered]@{
    generatedAtUtc = [DateTime]::UtcNow.ToString('o')
    resolution = '1920x1080'
    results = $results
    passed = $results.Count -eq 5 -and
        @($results | Where-Object { -not $_.passed }).Count -eq 0
}
$summaryPath = Join-Path $outputRoot 'scale-matrix.json'
$summary | ConvertTo-Json -Depth 6 |
    Set-Content -LiteralPath $summaryPath -Encoding utf8
Write-Output $summaryPath
