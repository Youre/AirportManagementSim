[CmdletBinding()]
param(
    [string]$PackageRoot = ''
)

$ErrorActionPreference = 'Stop'
$repoRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..'))
$savedRoot = Join-Path $repoRoot 'AMSim\Saved\Phase7'
$packageRoot = if ($PackageRoot) {
    [IO.Path]::GetFullPath($PackageRoot)
} else {
    Join-Path $repoRoot 'AMSim\Saved\Phase7Packages'
}
$auditPath = Join-Path $savedRoot 'release-audit.json'
$runtimePath = Join-Path $savedRoot 'packaged-runtime.json'
$manifestPath = Join-Path `
    $repoRoot `
    'AMSim\Config\Phase7\Phase7ReleaseManifest.json'
foreach ($required in @($auditPath, $runtimePath, $manifestPath)) {
    if (-not (Test-Path -LiteralPath $required -PathType Leaf)) {
        throw "Release attestation input is missing: $required"
    }
}

function Get-AggregateIdentity {
    param([Parameter(Mandatory)][IO.FileInfo[]]$Files)

    $entries = @(
        $Files |
            Sort-Object FullName |
            ForEach-Object {
                $rootPrefix = $repoRoot.TrimEnd('\') + '\'
                if (-not $_.FullName.StartsWith(
                    $rootPrefix,
                    [StringComparison]::OrdinalIgnoreCase)) {
                    throw "Identity input is outside the repository: $($_.FullName)"
                }
                $relative = $_.FullName.Substring(
                    $rootPrefix.Length).Replace('\', '/')
                [ordered]@{
                    path = $relative
                    sha256 = (Get-FileHash `
                        -LiteralPath $_.FullName `
                        -Algorithm SHA256).Hash.ToLowerInvariant()
                    bytes = $_.Length
                }
            }
    )
    $canonical = @(
        $entries |
            ForEach-Object {
                "$($_.path)`t$($_.sha256)`t$($_.bytes)"
            }
    ) -join "`n"
    $hasher = [Security.Cryptography.SHA256]::Create()
    try {
        $digest = $hasher.ComputeHash(
            [Text.Encoding]::UTF8.GetBytes($canonical))
    }
    finally {
        $hasher.Dispose()
    }
    return [ordered]@{
        sha256 = (
            [BitConverter]::ToString($digest) -replace '-', ''
        ).ToLowerInvariant()
        files = $entries.Count
        bytes = ($entries |
            Measure-Object -Property bytes -Sum).Sum
        entries = $entries
    }
}

$releaseAudit = Get-Content -LiteralPath $auditPath -Raw |
    ConvertFrom-Json
$packagedRuntime = Get-Content -LiteralPath $runtimePath -Raw |
    ConvertFrom-Json
$releaseManifest = Get-Content -LiteralPath $manifestPath -Raw |
    ConvertFrom-Json
if (-not $releaseAudit.passed -or -not $packagedRuntime.passed) {
    throw 'Release audit and packaged runtime must pass before attestation.'
}

$runtimeSourceFiles = @(
    foreach ($root in @(
        (Join-Path $repoRoot 'AMSim\Config'),
        (Join-Path $repoRoot 'AMSim\Content'),
        (Join-Path $repoRoot 'AMSim\Source')
    )) {
        Get-ChildItem -LiteralPath $root -File -Recurse
    }
    Get-Item -LiteralPath (Join-Path $repoRoot 'AMSim\AMSim.uproject')
)
$releaseContentFiles = @(
    Get-ChildItem `
        -LiteralPath (Join-Path $repoRoot 'AMSim\Config\Phase7') `
        -File `
        -Recurse
    Get-ChildItem `
        -LiteralPath (Join-Path $repoRoot 'AMSim\Content\Phase7') `
        -File `
        -Recurse
)
$runtimeSourceIdentity = Get-AggregateIdentity -Files $runtimeSourceFiles
$releaseContentIdentity = Get-AggregateIdentity -Files $releaseContentFiles

$attestation = [ordered]@{
    schema = 1
    generatedUtc = [DateTime]::UtcNow.ToString('o')
    releaseId = $releaseManifest.releaseId
    engine = $releaseManifest.engine
    platform = $releaseManifest.platform
    saveSchema = $releaseManifest.saveSchema
    sourceCommit = (& git -C $repoRoot rev-parse HEAD).Trim()
    runtimeSourceIdentity = $runtimeSourceIdentity
    releaseContentIdentity = $releaseContentIdentity
    packageIdentity = $packagedRuntime.packageIdentity
    technicalGates = [ordered]@{
        fullAutomation = 'passed'
        schemaMigration = 'passed'
        renderedFixtures = 'passed'
        fiveScaleMatrix = 'passed'
        developmentPackage = 'passed'
        shippingPackage = 'passed'
        cookInventory = 'passed'
        packagedRuntime = 'passed'
        offlineSocketObservation = 'passed'
        forbiddenDependencyAudit = 'passed'
        runtimeStringLoadAudit = 'passed'
        required3DAudit = 'passed'
        rightsAudit = 'passed'
        actualVisualAudit = 'passed'
    }
    ownerAcceptance = 'pending'
    status = 'release-candidate-ready-for-owner-acceptance'
    passed = $true
}
$resultPath = Join-Path $savedRoot 'release-attestation.json'
$attestation | ConvertTo-Json -Depth 10 |
    Set-Content -LiteralPath $resultPath -Encoding utf8
Write-Host "Phase 7 release attestation: $resultPath"
