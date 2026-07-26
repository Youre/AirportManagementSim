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
$foundationPipeline = Join-Path $repoRoot 'scripts\phase1\Invoke-Phase1Pipeline.ps1'
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

& $foundationPipeline @arguments
if ($LASTEXITCODE -ne 0) {
    throw "Integrated Phase 3 foundation pipeline exited with code $LASTEXITCODE."
}

$foundationResultPath =
    Join-Path $repoRoot 'AMSim\Saved\Phase1\pipeline-result.json'
$smokePath = Join-Path $repoRoot 'AMSim\Saved\Phase1\smoke-result.json'
$foundation = Get-Content -LiteralPath $foundationResultPath -Raw |
    ConvertFrom-Json
$smoke = Get-Content -LiteralPath $smokePath -Raw | ConvertFrom-Json
$proofRoot = Join-Path $repoRoot 'AMSim\Saved\Phase1'
$requiredProofs = @(
    'va03-terminal-passenger-flow.png',
    'phase3-complete.png'
)
$missingProofs = @($requiredProofs | Where-Object {
    -not (Test-Path -LiteralPath (Join-Path $proofRoot $_))
})
$missingScaleProofs = @()
if (-not $SkipScaleMatrix) {
    foreach ($percent in @(100, 125, 150, 175, 200)) {
        foreach ($proof in $requiredProofs) {
            $candidate = Join-Path $proofRoot "Scale\$percent\$proof"
            if (-not (Test-Path -LiteralPath $candidate)) {
                $missingScaleProofs += $candidate
            }
        }
    }
}

$sourcePhase3Root = Join-Path $repoRoot 'AMSim\Content\Phase3'
$sourcePhase3Assets = @(
    Get-ChildItem -LiteralPath $sourcePhase3Root `
        -Filter '*.uasset' `
        -File `
        -Recurse
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
$cookedPhase3AssetNames = @(
    $ioStoreInventory |
        Select-String -SimpleMatch '/AMSim/Content/Phase3/' |
        Where-Object { $_.Line -match '/([^/]+)\.uasset"' } |
        ForEach-Object { $Matches[1] } |
        Sort-Object -Unique
)
$missingCookedPhase3Assets = @(
    Compare-Object `
        -ReferenceObject @($sourcePhase3Assets.BaseName | Sort-Object -Unique) `
        -DifferenceObject $cookedPhase3AssetNames |
        Where-Object { $_.SideIndicator -eq '<=' } |
        ForEach-Object { $_.InputObject }
)
$phase3SourceFiles = @(
    Get-ChildItem -LiteralPath (Join-Path $repoRoot 'AMSim\Source') `
        -Include '*.h', '*.cpp' `
        -File `
        -Recurse |
        Where-Object {
            $_.Name -match 'Phase3|PassengerTerminal|TerminalView' -or
            $_.Name -in @('AMSimWorldPresenter.cpp', 'AMSimWorldPresenter.h')
        }
)
$runtimeStringAssetLoads = @(
    $phase3SourceFiles |
        Select-String `
            -Pattern '\b(LoadClass|LoadObject|StaticLoadClass|StaticLoadObject)\s*(<|\()' `
            -AllMatches
)
$required3DMatches = @(
    Get-ChildItem -LiteralPath $sourcePhase3Root -File -Recurse |
        Where-Object {
            $_.Name -match '(?i)(staticmesh|skeletalmesh|nanite|materialinstance)'
        }
)
$phase3Passed =
    $foundation.passed -and
    $smoke.passed -and
    $smoke.schema -eq 3 -and
    $smoke.phase3FlightState -eq 6 -and
    $smoke.phase3Passengers -eq 52 -and
    $smoke.phase3CompletedPassengers -eq 52 -and
    $smoke.phase3Bags -eq 34 -and
    $smoke.phase3CompletedBags -eq 34 -and
    $smoke.phase3ReconciliationPasses -eq 1 -and
    $missingProofs.Count -eq 0 -and
    $missingScaleProofs.Count -eq 0 -and
    $sourcePhase3Assets.Count -eq 34 -and
    $cookedPhase3AssetNames.Count -eq $sourcePhase3Assets.Count -and
    $missingCookedPhase3Assets.Count -eq 0 -and
    $runtimeStringAssetLoads.Count -eq 0 -and
    $required3DMatches.Count -eq 0
if (-not $phase3Passed) {
    throw 'Phase 3 integrated pipeline evidence is incomplete.'
}

$result = [ordered]@{
    schema = 1
    generatedUtc = [DateTime]::UtcNow.ToString('o')
    phase = 'Phase3'
    foundationPipeline = $foundationResultPath
    smoke = $smokePath
    passengers = $smoke.phase3Passengers
    completedPassengers = $smoke.phase3CompletedPassengers
    bags = $smoke.phase3Bags
    completedBags = $smoke.phase3CompletedBags
    reconciliationPasses = $smoke.phase3ReconciliationPasses
    sourcePhase3Assets = $sourcePhase3Assets.Count
    cookedPhase3Assets = $cookedPhase3AssetNames.Count
    missingCookedPhase3Assets = $missingCookedPhase3Assets
    runtimeStringAssetLoads = $runtimeStringAssetLoads.Count
    required3DMatches = $required3DMatches.Count
    scaleProofs = 10
    proofs = $requiredProofs
    passed = $true
}
$resultPath = Join-Path $repoRoot 'AMSim\Saved\Phase3\pipeline-result.json'
New-Item -ItemType Directory -Path (Split-Path $resultPath) -Force |
    Out-Null
$result | ConvertTo-Json -Depth 8 |
    Set-Content -LiteralPath $resultPath -Encoding utf8
Write-Host "Phase 3 pipeline passed. Result: $resultPath"
