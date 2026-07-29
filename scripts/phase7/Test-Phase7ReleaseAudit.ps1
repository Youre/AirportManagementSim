[CmdletBinding()]
param(
    [string]$EngineRoot = 'C:\Program Files\Epic Games\UE_5.8',
    [string]$PackageRoot = ''
)

$ErrorActionPreference = 'Stop'
$repoRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..'))
$savedRoot = Join-Path $repoRoot 'AMSim\Saved\Phase7'
$contentRoot = Join-Path $repoRoot 'AMSim\Content'
$phase7Content = Join-Path $contentRoot 'Phase7\Definitions'
$sourceRoot = Join-Path $repoRoot 'AMSim\Source'
$configRoot = Join-Path $repoRoot 'AMSim\Config'
$releaseManifestPath = Join-Path $configRoot 'Phase7\Phase7ReleaseManifest.json'
$rightsPath = Join-Path $configRoot 'Phase7\Phase7RightsInventory.json'
$radioPath = Join-Path $configRoot 'Phase7\Phase7RadioPhraseCatalog.json'

foreach ($required in @(
    $phase7Content,
    $releaseManifestPath,
    $rightsPath,
    $radioPath
)) {
    if (-not (Test-Path -LiteralPath $required)) {
        throw "Required Phase 7 audit path is missing: $required"
    }
}
New-Item -ItemType Directory -Path $savedRoot -Force | Out-Null

$release = Get-Content -LiteralPath $releaseManifestPath -Raw |
    ConvertFrom-Json
$rights = Get-Content -LiteralPath $rightsPath -Raw |
    ConvertFrom-Json
$radio = Get-Content -LiteralPath $radioPath -Raw |
    ConvertFrom-Json
$phase7Assets = @(
    Get-ChildItem -LiteralPath $phase7Content -Filter '*.uasset' -File
)

$runtimeRoots = @(
    (Join-Path $sourceRoot 'AMSimSimulation')
    (Join-Path $sourceRoot 'AMSimGameplay')
    (Join-Path $sourceRoot 'AMSimUI')
)
$runtimeFiles = @(
    $runtimeRoots |
        ForEach-Object {
            Get-ChildItem -LiteralPath $_ `
                -Include '*.h', '*.cpp', '*.cs' `
                -File `
                -Recurse
        }
)
$runtimeStringLoads = @(
    $runtimeFiles |
        Select-String `
            -Pattern '\b(LoadClass|LoadObject|StaticLoadClass|StaticLoadObject)\s*(<|\()' `
            -AllMatches
)
$forbiddenDependencyNames = @(
    'AMSimEditor',
    'AMSimTests',
    'EditorToolset',
    'ModelContextProtocol',
    'PythonScriptPlugin',
    'RemoteControl',
    'ToolsetRegistry',
    'UMGToolSet'
)
$forbiddenRuntimeDependencies = @(
    $runtimeFiles |
        Select-String `
            -Pattern $forbiddenDependencyNames `
            -SimpleMatch
)
$required3DAssets = @(
    Get-ChildItem -LiteralPath $contentRoot -File -Recurse |
        Where-Object {
            $_.Name -match '(?i)(staticmesh|skeletalmesh|nanite|materialinstance)'
        }
)
$unreviewedMusic = @(
    Get-ChildItem -LiteralPath $contentRoot -File -Recurse |
        Where-Object {
            $_.Extension -in @('.mp3', '.flac', '.ogg', '.wav') -and
            $_.FullName -match '(?i)music'
        }
)
$secretCandidates = @(
    Get-ChildItem -LiteralPath $configRoot `
        -Include '*.ini', '*.json' `
        -File `
        -Recurse |
        Select-String `
            -Pattern '(?i)(api[_-]?key|client[_-]?secret|password\s*=|bearer\s+[a-z0-9._-]{12,})'
)
$endpointCandidates = @(
    $runtimeFiles |
        Select-String -Pattern 'https?://'
)
$lineLimitViolations = @(
    Get-ChildItem -LiteralPath $sourceRoot `
        -Include '*.h', '*.cpp' `
        -File `
        -Recurse |
        ForEach-Object {
            $lineCount = [IO.File]::ReadAllLines($_.FullName).Length
            if ($lineCount -gt 2000) {
                [pscustomobject]@{
                    path = $_.FullName
                    lines = $lineCount
                }
            }
        }
)

$packageAudit = [ordered]@{
    requested = -not [string]::IsNullOrWhiteSpace($PackageRoot)
    developmentLauncher = ''
    shippingLauncher = ''
    forbiddenFiles = @()
    forbiddenTextMatches = @()
    cookReferenceManifest = ''
    cookedPhase7Assets = @()
    missingCookedPhase7Assets = @()
    passed = $true
}
if ($packageAudit.requested) {
    $resolvedPackageRoot = [IO.Path]::GetFullPath($PackageRoot)
    $developmentLauncher = Get-ChildItem `
        -LiteralPath (Join-Path $resolvedPackageRoot 'Development') `
        -Filter 'AMSim.exe' `
        -File `
        -Recurse |
        Where-Object { $_.FullName -notmatch '\\AMSim\\Binaries\\Win64\\' } |
        Select-Object -First 1
    $shippingLauncher = Get-ChildItem `
        -LiteralPath (Join-Path $resolvedPackageRoot 'Shipping') `
        -Filter 'AMSim.exe' `
        -File `
        -Recurse |
        Where-Object { $_.FullName -notmatch '\\AMSim\\Binaries\\Win64\\' } |
        Select-Object -First 1
    if (-not $developmentLauncher -or -not $shippingLauncher) {
        throw 'Development and Shipping launchers are required for the package audit.'
    }
    $packageAudit.developmentLauncher = $developmentLauncher.FullName
    $packageAudit.shippingLauncher = $shippingLauncher.FullName
    $shippingFiles = @(
        Get-ChildItem `
            -LiteralPath (Join-Path $resolvedPackageRoot 'Shipping') `
            -File `
            -Recurse
    )
    $packageAudit.forbiddenFiles = @(
        $shippingFiles |
            Where-Object {
                $name = $_.Name
                @(
                    $forbiddenDependencyNames |
                        Where-Object { $name -like "*$_*" }
                ).Count -gt 0
            } |
            ForEach-Object { $_.FullName }
    )
    $packageAudit.forbiddenTextMatches = @(
        $shippingFiles |
            Where-Object {
                $_.Extension -in @(
                    '.ini', '.json', '.modules', '.target',
                    '.txt', '.uplugin', '.uproject'
                )
            } |
            Select-String `
                -Pattern $forbiddenDependencyNames `
                -SimpleMatch `
                -ErrorAction SilentlyContinue |
            ForEach-Object { "$($_.Path):$($_.LineNumber)" }
    )

    $shippingUtoc = $shippingFiles |
        Where-Object { $_.Name -eq 'AMSim-Windows.utoc' } |
        Select-Object -First 1
    if (-not $shippingUtoc) {
        throw 'Shipping IoStore catalog AMSim-Windows.utoc is missing.'
    }
    $cookReferenceManifest = Join-Path `
        $repoRoot `
        'AMSim\Saved\Cooked\Windows\AMSim\Metadata\ReferencedSet.txt'
    if (-not (Test-Path -LiteralPath $cookReferenceManifest -PathType Leaf)) {
        throw 'The Shipping cook reference manifest is missing.'
    }
    $packageAudit.cookReferenceManifest = $cookReferenceManifest
    $inventory = @(Get-Content -LiteralPath $cookReferenceManifest)
    $packageAudit.cookedPhase7Assets = @(
        $inventory |
            Where-Object {
                $_ -like '/game/phase7/definitions/*'
            } |
            ForEach-Object {
                ($_ -split '/')[-1]
            } |
            Sort-Object -Unique
    )
    $packageAudit.missingCookedPhase7Assets = @(
        Compare-Object `
            -ReferenceObject @($phase7Assets.BaseName | Sort-Object) `
            -DifferenceObject @($packageAudit.cookedPhase7Assets | Sort-Object) |
            Where-Object { $_.SideIndicator -eq '<=' } |
            ForEach-Object { $_.InputObject }
    )
    $packageAudit.passed =
        $packageAudit.forbiddenFiles.Count -eq 0 -and
        $packageAudit.forbiddenTextMatches.Count -eq 0 -and
        $packageAudit.missingCookedPhase7Assets.Count -eq 0
}

$passed =
    $release.engine -eq '5.8.0' -and
    $release.platform -eq 'Win64' -and
    $release.saveSchema -eq 8 -and
    $release.mapIds.Count -eq 1 -and
    $release.specializationPaths.Count -eq 6 -and
    $release.runtimeBoundaries.offlineRequired -and
    $rights.excluded.unreviewedSourceAssetsAudioMusic -and
    $radio.captionsFirst -and
    $radio.offlineOnly -and
    $radio.families.Count -eq 15 -and
    $phase7Assets.Count -eq 15 -and
    $runtimeStringLoads.Count -eq 0 -and
    $forbiddenRuntimeDependencies.Count -eq 0 -and
    $required3DAssets.Count -eq 0 -and
    $unreviewedMusic.Count -eq 0 -and
    $secretCandidates.Count -eq 0 -and
    $endpointCandidates.Count -eq 0 -and
    $lineLimitViolations.Count -eq 0 -and
    $packageAudit.passed

$result = [ordered]@{
    schema = 1
    generatedUtc = [DateTime]::UtcNow.ToString('o')
    engine = $release.engine
    platform = $release.platform
    saveSchema = $release.saveSchema
    mapIds = $release.mapIds
    specializationPaths = $release.specializationPaths
    phase7Assets = $phase7Assets.Count
    radioFamilies = $radio.families.Count
    runtimeStringAssetLoads = $runtimeStringLoads.Count
    forbiddenRuntimeDependencies = $forbiddenRuntimeDependencies.Count
    required3DAssetCandidates = $required3DAssets.Count
    unreviewedMusicIncluded = $unreviewedMusic.Count
    secretCandidates = $secretCandidates.Count
    endpointCandidates = $endpointCandidates.Count
    lineLimitViolations = $lineLimitViolations
    package = $packageAudit
    ownerAcceptance = 'pending'
    passed = $passed
}
$resultPath = Join-Path $savedRoot 'release-audit.json'
$result | ConvertTo-Json -Depth 8 |
    Set-Content -LiteralPath $resultPath -Encoding utf8
if (-not $passed) {
    throw "Phase 7 release audit failed. Result: $resultPath"
}
Write-Host "Phase 7 release audit passed. Result: $resultPath"
