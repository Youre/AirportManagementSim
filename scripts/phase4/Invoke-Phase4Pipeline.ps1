[CmdletBinding()]
param(
    [string]$EngineRoot = 'C:\Program Files\Epic Games\UE_5.8',
    [string]$PackageRoot = '',
    [switch]$SkipBuild,
    [switch]$SkipPackage,
    [switch]$SkipScaleMatrix,
    [switch]$SkipFirewall
)

$ErrorActionPreference = 'Stop'
$repoRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..'))
$foundation = Join-Path $repoRoot 'scripts\phase1\Invoke-Phase1Pipeline.ps1'
if ([string]::IsNullOrWhiteSpace($PackageRoot)) {
    $PackageRoot = Join-Path $repoRoot 'AMSim\Saved\Phase1Packages'
}

& $foundation `
    -EngineRoot $EngineRoot `
    -PackageRoot $PackageRoot `
    -SkipBuild:$SkipBuild `
    -SkipPackage:$SkipPackage `
    -SkipScaleMatrix `
    -SkipFirewall:$SkipFirewall
if ($LASTEXITCODE -ne 0) {
    throw "Integrated foundation pipeline exited with code $LASTEXITCODE."
}

$project = Join-Path $repoRoot 'AMSim\AMSim.uproject'
$editor = Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'
$savedRoot = Join-Path $repoRoot 'AMSim\Saved\Phase4'
$smokePath = Join-Path $savedRoot 'smoke-result.json'
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
        '-nop4'
    ) `
    -WindowStyle Hidden `
    -PassThru `
    -Wait
if ($process.ExitCode -ne 0) {
    throw "Phase 4 rendered smoke exited with code $($process.ExitCode)."
}
$smoke = Get-Content -LiteralPath $smokePath -Raw | ConvertFrom-Json
if (-not $smoke.passed -or $smoke.schema -ne 4) {
    throw 'Phase 4 rendered smoke failed.'
}

if (-not $SkipScaleMatrix) {
    & (Join-Path $PSScriptRoot 'Invoke-Phase4ScaleMatrix.ps1') `
        -EngineRoot $EngineRoot
}

$proofs = @(
    'va04-regional-timetable.png',
    'va06-regional-incident.png',
    'phase4-complete.png'
)
$missingProofs = @($proofs | Where-Object {
    -not (Test-Path -LiteralPath (Join-Path $savedRoot $_))
})
$scaleSummaryPath = Join-Path $savedRoot 'Scale\scale-matrix.json'
$scalePassed = $SkipScaleMatrix
if (-not $SkipScaleMatrix) {
    $scale = Get-Content -LiteralPath $scaleSummaryPath -Raw |
        ConvertFrom-Json
    $scalePassed = $scale.passed
}

$sourceAssets = @(
    Get-ChildItem `
        -LiteralPath (Join-Path $repoRoot 'AMSim\Content\Phase4') `
        -Filter '*.uasset' `
        -File `
        -Recurse
)
$cookedAssetCount = 0
$missingCookedAssets = @()
if (-not $SkipPackage) {
    $shippingUtoc = Get-ChildItem `
        -LiteralPath (Join-Path $PackageRoot 'Shipping') `
        -Filter 'AMSim-Windows.utoc' `
        -File `
        -Recurse |
        Select-Object -First 1
    $unrealPak = Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealPak.exe'
    $inventory = @(& $unrealPak $shippingUtoc.FullName -List)
    $cookedNames = @(
        $inventory |
            Select-String -SimpleMatch '/AMSim/Content/Phase4/' |
            Where-Object { $_.Line -match '/([^/]+)\.uasset"' } |
            ForEach-Object { $Matches[1] } |
            Sort-Object -Unique
    )
    $cookedAssetCount = $cookedNames.Count
    $missingCookedAssets = @(
        Compare-Object `
            -ReferenceObject @($sourceAssets.BaseName | Sort-Object -Unique) `
            -DifferenceObject $cookedNames |
            Where-Object { $_.SideIndicator -eq '<=' } |
            ForEach-Object { $_.InputObject }
    )
}

$phase4Sources = @(
    Get-ChildItem -LiteralPath (Join-Path $repoRoot 'AMSim\Source') `
        -Include '*.h', '*.cpp' `
        -File `
        -Recurse |
        Where-Object {
            $_.Name -match 'Phase4|RegionalOperations|RegionalScheduled' -or
            $_.Name -in @('AMSimWorldPresenter.cpp', 'AMSimWorldPresenter.h')
        }
)
$runtimeStringLoads = @(
    $phase4Sources |
        Select-String `
            -Pattern '\b(LoadClass|LoadObject|StaticLoadClass|StaticLoadObject)\s*(<|\()' `
            -AllMatches
)
$required3D = @(
    Get-ChildItem `
        -LiteralPath (Join-Path $repoRoot 'AMSim\Content\Phase4') `
        -File `
        -Recurse |
        Where-Object {
            $_.Name -match '(?i)(staticmesh|skeletalmesh|nanite|materialinstance)'
        }
)
$passed =
    $smoke.passed -and
    $missingProofs.Count -eq 0 -and
    $scalePassed -and
    $sourceAssets.Count -eq 34 -and
    ($SkipPackage -or
        ($cookedAssetCount -eq $sourceAssets.Count -and
         $missingCookedAssets.Count -eq 0)) -and
    $runtimeStringLoads.Count -eq 0 -and
    $required3D.Count -eq 0
if (-not $passed) {
    throw 'Phase 4 integrated pipeline evidence is incomplete.'
}

$result = [ordered]@{
    schema = 1
    generatedUtc = [DateTime]::UtcNow.ToString('o')
    phase = 'Phase4'
    smoke = $smokePath
    completedFlights = $smoke.phase4CompletedFlights
    completedTransferBags = $smoke.phase4CompletedTransferBags
    borderProcessedPassengers = $smoke.phase4BorderProcessedPassengers
    missedConnections = $smoke.phase4MissedConnections
    rebookedPassengers = $smoke.phase4RebookedPassengers
    incidentLifecycle = $smoke.phase4IncidentLifecycle
    renewals = $smoke.phase4Renewals
    sourceAssets = $sourceAssets.Count
    cookedAssets = $cookedAssetCount
    missingCookedAssets = $missingCookedAssets
    runtimeStringAssetLoads = $runtimeStringLoads.Count
    required3DMatches = $required3D.Count
    scaleMatrix = if ($SkipScaleMatrix) { $null } else { $scaleSummaryPath }
    proofs = $proofs
    passed = $true
}
$resultPath = Join-Path $savedRoot 'pipeline-result.json'
$result | ConvertTo-Json -Depth 8 |
    Set-Content -LiteralPath $resultPath -Encoding utf8
Write-Host "Phase 4 pipeline passed. Result: $resultPath"
