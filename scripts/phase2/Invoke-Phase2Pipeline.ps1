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
$repoRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..'))
$phase1Pipeline = Join-Path $repoRoot 'scripts\phase1\Invoke-Phase1Pipeline.ps1'
if ([string]::IsNullOrWhiteSpace($PackageRoot)) {
    $PackageRoot = Join-Path $repoRoot 'AMSim\Saved\Phase1Packages'
}
$arguments = @{
    EngineRoot = $EngineRoot
    PackageRoot = $PackageRoot
    SkipBuild = $SkipBuild
    SkipPackage = $SkipPackage
    SkipScaleMatrix = $SkipScaleMatrix
    SkipFirewall = $SkipFirewall
}

& $phase1Pipeline @arguments
if ($LASTEXITCODE -ne 0) {
    throw "Integrated foundation pipeline exited with code $LASTEXITCODE."
}

$resultPath = Join-Path $repoRoot 'AMSim\Saved\Phase1\pipeline-result.json'
$smokePath = Join-Path $repoRoot 'AMSim\Saved\Phase1\smoke-result.json'
$pipeline = Get-Content -LiteralPath $resultPath -Raw | ConvertFrom-Json
$smoke = Get-Content -LiteralPath $smokePath -Raw | ConvertFrom-Json
$requiredProofs = @(
    'va06-weather.png',
    'va06-incident.png',
    'va07-progression.png',
    'phase2-complete.png'
)
$proofRoot = Join-Path $repoRoot 'AMSim\Saved\Phase1'
$missingProofs = @($requiredProofs | Where-Object {
    -not (Test-Path -LiteralPath (Join-Path $proofRoot $_))
})
$sourcePhase2Root = Join-Path $repoRoot 'AMSim\Content\Phase2'
$sourcePhase2Assets = @(
    Get-ChildItem -LiteralPath $sourcePhase2Root -Filter '*.uasset' -File -Recurse
)
$shippingUtoc = Get-ChildItem `
    -LiteralPath (Join-Path $PackageRoot 'Shipping') `
    -Filter 'AMSim-Windows.utoc' `
    -File `
    -Recurse |
    Select-Object -First 1
$unrealPak = Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealPak.exe'
$ioStoreInventory = if ($shippingUtoc) {
    @(& $unrealPak $shippingUtoc.FullName -List)
} else {
    @()
}
$cookedPhase2AssetNames = @(
    $ioStoreInventory |
        Select-String -SimpleMatch '/AMSim/Content/Phase2/' |
        Where-Object { $_.Line -match '/([^/]+)\.uasset"' } |
        ForEach-Object { $Matches[1] } |
        Sort-Object -Unique
)
$missingCookedPhase2Assets = @(
    Compare-Object `
        -ReferenceObject @($sourcePhase2Assets.BaseName | Sort-Object -Unique) `
        -DifferenceObject $cookedPhase2AssetNames |
        Where-Object { $_.SideIndicator -eq '<=' } |
        ForEach-Object { $_.InputObject }
)
$phase2SourceFiles = @(
    Get-ChildItem -LiteralPath (Join-Path $repoRoot 'AMSim\Source') `
        -Include '*.h', '*.cpp' `
        -File `
        -Recurse |
        Where-Object {
            $_.Name -match 'Phase2|LivingAirport' -or
            $_.Name -in @('AMSimWorldPresenter.cpp', 'AMSimWorldPresenter.h')
        }
)
$runtimeStringAssetLoads = @(
    $phase2SourceFiles |
        Select-String `
            -Pattern '\b(LoadClass|LoadObject|StaticLoadClass|StaticLoadObject)\s*(<|\()' `
            -AllMatches
)
$phase2Passed =
    $pipeline.passed -and
    $smoke.passed -and
    $smoke.schema -ge 2 -and
    $smoke.phase2OperatingDay -ge 8 -and
    $smoke.phase2CompletedFlights -ge 20 -and
    $smoke.phase2IncidentState -eq 5 -and
    $smoke.phase2ExpansionStage -eq 5 -and
    $missingProofs.Count -eq 0 -and
    $sourcePhase2Assets.Count -eq 33 -and
    $cookedPhase2AssetNames.Count -eq $sourcePhase2Assets.Count -and
    $missingCookedPhase2Assets.Count -eq 0 -and
    $runtimeStringAssetLoads.Count -eq 0
if (-not $phase2Passed) {
    throw "Phase 2 integrated pipeline evidence is incomplete."
}

$phase2Result = [ordered]@{
    schema = 1
    generatedUtc = [DateTime]::UtcNow.ToString('o')
    phase = 'Phase2'
    foundationPipeline = $resultPath
    smoke = $smokePath
    operatingDay = $smoke.phase2OperatingDay
    completedFlights = $smoke.phase2CompletedFlights
    incidentResolved = $smoke.phase2IncidentState -eq 5
    expansionOperational = $smoke.phase2ExpansionStage -eq 5
    sourcePhase2Assets = $sourcePhase2Assets.Count
    cookedPhase2Assets = $cookedPhase2AssetNames.Count
    missingCookedPhase2Assets = $missingCookedPhase2Assets
    runtimeStringAssetLoads = $runtimeStringAssetLoads.Count
    proofs = $requiredProofs
    passed = $true
}
$phase2ResultPath = Join-Path $repoRoot 'AMSim\Saved\Phase2\pipeline-result.json'
New-Item -ItemType Directory -Path (Split-Path $phase2ResultPath) -Force | Out-Null
$phase2Result | ConvertTo-Json -Depth 8 |
    Set-Content -LiteralPath $phase2ResultPath -Encoding utf8
Write-Host "Phase 2 pipeline passed. Result: $phase2ResultPath"
