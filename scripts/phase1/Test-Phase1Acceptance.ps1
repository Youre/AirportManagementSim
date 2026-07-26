[CmdletBinding()]
param(
    [string]$PackageRoot = '',
    [string]$ManifestPath = '',
    [string]$PipelineResultPath = '',
    [string]$NetworkDeniedResultPath = '',
    [string]$ReferenceTierResultPath = '',
    [string]$TesterAcceptanceResultPath = '',
    [string]$OutputPath = '',
    [switch]$AllowIncompleteEvidence
)

$ErrorActionPreference = 'Stop'
$repoRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..'))
. (Join-Path $PSScriptRoot 'Phase1Acceptance.Common.ps1')

function Resolve-OptionalPath {
    param(
        [string]$Provided,
        [Parameter(Mandatory)]
        [string]$Default
    )

    if ($Provided) {
        return [System.IO.Path]::GetFullPath($Provided)
    }
    return [System.IO.Path]::GetFullPath($Default)
}

function Read-EvidenceFile {
    param(
        [Parameter(Mandatory)]
        [string]$Path
    )

    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) {
        return $null
    }
    try {
        return Get-Content -LiteralPath $Path -Raw | ConvertFrom-Json
    } catch {
        throw "Invalid JSON evidence file: $Path`n$($_.Exception.Message)"
    }
}

function Test-AllTrue {
    param(
        [Parameter(Mandatory)]
        [AllowNull()]
        [AllowEmptyCollection()]
        [object[]]$Values
    )

    foreach ($value in $Values) {
        if ($value -ne $true) {
            return $false
        }
    }
    return $true
}

$packageRoot = Resolve-OptionalPath `
    -Provided $PackageRoot `
    -Default (Join-Path $repoRoot 'AMSim\Saved\Phase1Packages')
$manifestPath = Resolve-OptionalPath `
    -Provided $ManifestPath `
    -Default (Join-Path $PSScriptRoot 'Phase1AcceptanceManifest.json')
$pipelineResultPath = Resolve-OptionalPath `
    -Provided $PipelineResultPath `
    -Default (Join-Path $repoRoot 'AMSim\Saved\Phase1\pipeline-result.json')
$networkDeniedResultPath = Resolve-OptionalPath `
    -Provided $NetworkDeniedResultPath `
    -Default (Join-Path $repoRoot 'AMSim\Saved\Phase1\network-denied-result.json')
$referenceTierResultPath = Resolve-OptionalPath `
    -Provided $ReferenceTierResultPath `
    -Default (Join-Path $repoRoot 'AMSim\Saved\Phase1\reference-tier-result.json')
$testerAcceptanceResultPath = Resolve-OptionalPath `
    -Provided $TesterAcceptanceResultPath `
    -Default (Join-Path $repoRoot 'AMSim\Saved\Phase1\tester-acceptance-result.json')
$outputPath = Resolve-OptionalPath `
    -Provided $OutputPath `
    -Default (Join-Path $repoRoot 'AMSim\Saved\Phase1\acceptance-result.json')

$manifest = Get-Phase1AcceptanceManifest -Path $manifestPath
$packageIdentity = Get-Phase1PackageIdentity `
    -PackageRoot $packageRoot `
    -Manifest $manifest
$pipeline = Read-EvidenceFile -Path $pipelineResultPath
$networkDenied = Read-EvidenceFile -Path $networkDeniedResultPath
$referenceTier = Read-EvidenceFile -Path $referenceTierResultPath
$testerAcceptance = Read-EvidenceFile -Path $testerAcceptanceResultPath

$expectedScalePercents = @(100, 125, 150, 175, 200)
$scaleMatrixPassed =
    $null -ne $pipeline -and
    @($pipeline.scaleMatrix).Count -eq $expectedScalePercents.Count
if ($scaleMatrixPassed) {
    foreach ($percent in $expectedScalePercents) {
        $scale = @($pipeline.scaleMatrix | Where-Object {
            $_.percent -eq $percent
        })
        if ($scale.Count -ne 1 -or $scale[0].passed -ne $true) {
            $scaleMatrixPassed = $false
            break
        }
    }
}
$pipelinePassed = $null -ne $pipeline -and (Test-AllTrue -Values @(
    $pipeline.schema -eq 1
    $pipeline.engine -eq $manifest.engine
    $pipeline.platform -eq $manifest.platform
    $pipeline.auditPassed
    $pipeline.fullAutomationPassed
    $pipeline.automation.succeeded -eq 24
    $pipeline.automation.succeededWithWarnings -eq 1
    $pipeline.automation.failed -eq 0
    $pipeline.automation.notRun -eq 0
    $pipeline.automation.inProcess -eq 0
    $scaleMatrixPassed
    $pipeline.packagedSmoke.passed
    $pipeline.packagedSmoke.journeyPassed
    $pipeline.packagedSmoke.saveLoadContinuity
    $pipeline.packagedSmoke.stateAssertions
    $pipeline.packagedSmoke.screenshotsCaptured
    $pipeline.packagedSmoke.p99FrameMilliseconds -le 16.6
    $pipeline.packagedSmoke.simulationMedianMilliseconds -le 4.0
    $pipeline.packagedSmoke.simulationP99Milliseconds -le 8.0
    $pipeline.packagedSmoke.maximum8xBacklogSteps -eq 0
    $pipeline.packagedSmoke.maximumMemoryMiB -lt 4096
    $pipeline.forbiddenShippingFiles -eq 0
    $pipeline.forbiddenShippingTextMatches -eq 0
    $pipeline.forbiddenShippingReceiptMatches -eq 0
    $pipeline.required3DAssetCandidates -eq 0
    $pipeline.passed
))
$networkDeniedPassed = $null -ne $networkDenied -and (Test-AllTrue -Values @(
    $networkDenied.schema -eq 1
    @($networkDenied.firewallDirections).Count -eq 2
    $networkDenied.firewallDirections -contains 'Inbound'
    $networkDenied.firewallDirections -contains 'Outbound'
    $networkDenied.firewallProfiles -eq 'Any'
    $networkDenied.development.journeyPassed
    $networkDenied.development.saveLoadContinuity
    $networkDenied.development.p99FrameMilliseconds -le 16.6
    $networkDenied.development.maximum8xBacklogSteps -eq 0
    $networkDenied.development.expectedDeveloperTraceListenerOnly
    $networkDenied.shipping.cleanLaunchSeconds -ge 5
    $networkDenied.shipping.tcpSocketObservations -eq 0
    $networkDenied.passed
))

$referencePackagePassed = $null -ne $referenceTier -and (Test-AllTrue -Values @(
    $referenceTier.package.launcherSha256 -ceq
        $manifest.packages.developmentLauncher.sha256
    $referenceTier.package.runtimeSha256 -ceq
        $manifest.packages.developmentRuntime.sha256
))
$referenceTierPassed = $null -ne $referenceTier -and (Test-AllTrue -Values @(
    $referenceTier.schema -eq 1
    $referenceTier.sourceCommit -match '^[0-9a-fA-F]{40}$'
    $referenceTier.sourceTreeClean
    $referenceTier.packageSourceCommit -ceq $manifest.packageSourceCommit
    $referenceTier.packageIdentity.passed
    $referenceTier.hardware.tierAttestation -eq 'AtOrBelowApprovedTier'
    $referenceTier.soak.requestedSeconds -eq 14400
    $referenceTier.soak.measuredSeconds -ge 14400
    $referenceTier.soak.sampleCount -ge 200
    $referenceTier.soak.memoryTrendPassed
    $referenceTier.soak.medianGrowthMiB -le 64.0
    $referenceTier.metrics.passed
    $referenceTier.metrics.referenceProfileApplied
    $referenceTier.metrics.performanceTargetSeconds -eq 14400
    $referenceTier.metrics.elapsedSeconds -ge 14400
    $referenceTier.metrics.averageFps -ge 60.0
    $referenceTier.metrics.p99FrameMilliseconds -le 16.6
    $referenceTier.metrics.simulationMedianMilliseconds -le 4.0
    $referenceTier.metrics.simulationP99Milliseconds -le 8.0
    $referenceTier.metrics.maximum8xBacklogSteps -eq 0
    $referenceTier.metrics.snapshotCaptureMilliseconds -le 50.0
    $referenceTier.metrics.saveWriteMilliseconds -le 2000.0
    $referenceTier.metrics.maximumMemoryMiB -lt 4096
    $referenceTier.checks.tierAttestationPassed
    $referenceTier.checks.sourceTreeClean
    $referenceTier.checks.packageIdentityPassed
    $referenceTier.checks.metricsPassed
    $referenceTier.checks.formalFourHourSoakPassed
    $referencePackagePassed
    $referenceTier.passed
))

$requiredTesterChecks = @(
    'formalEvidence',
    'sessionTimingPassed',
    'consentPassed',
    'packageIdentityPassed',
    'displayPrerequisitesPassed',
    'privacyPreconditionsPassed',
    'testerEligibilityPassed',
    'cleanStartPassed',
    'protocolIsolationPassed',
    'audioPreconditionPassed',
    'journeyPassed',
    'zeroGameplayHintsPassed',
    'saveLoadPassed',
    'comprehensionPassed',
    'normalAudioCaptionPassed',
    'forcedFallbackPassed',
    'noBlockingObservationsPassed',
    'observationRecordConsistent',
    'facilitatorAttestationPassed'
)
$testerChecksPassed = $null -ne $testerAcceptance
if ($testerChecksPassed) {
    foreach ($name in $requiredTesterChecks) {
        if ($testerAcceptance.checks.$name -ne $true) {
            $testerChecksPassed = $false
            break
        }
    }
}
$testerBandPassed =
    $null -ne $testerAcceptance -and
    $testerAcceptance.session.testerBand -in @('5-7', '8-10', 'adult proxy')
$testerConsentPassed =
    $testerBandPassed -and (
        $testerAcceptance.session.testerBand -eq 'adult proxy' -or
        $testerAcceptance.criteria.parentGuardianConsentConfirmed -eq $true
    )
$testerRawCriteriaPassed = $null -ne $testerAcceptance -and (Test-AllTrue -Values @(
    $testerAcceptance.session.id -match '^[A-Za-z0-9][A-Za-z0-9_-]{0,47}$'
    $testerAcceptance.session.elapsedMinutes -gt 0
    $testerAcceptance.session.elapsedMinutes -le 360
    $testerBandPassed
    $testerConsentPassed
    $testerAcceptance.session.display.width -eq 1920
    $testerAcceptance.session.display.height -eq 1080
    $testerAcceptance.session.display.uiScalePercent -eq 100
    $testerAcceptance.criteria.firstTimeTesterConfirmed
    $testerAcceptance.criteria.emptySaveSlotConfirmed
    $testerAcceptance.criteria.audioOutputPrechecked
    $testerAcceptance.criteria.protocolHiddenFromTester
    $testerAcceptance.criteria.noPersonalMediaCaptured
    $testerAcceptance.criteria.journeyCompleted
    $testerAcceptance.criteria.gameplayHints -eq 0
    $testerAcceptance.criteria.saveLoadRecognized
    $testerAcceptance.criteria.compatibilityUnderstood
    $testerAcceptance.criteria.routeStandUnderstood
    $testerAcceptance.criteria.servicesUnderstood
    $testerAcceptance.criteria.rewardUnderstood
    $testerAcceptance.criteria.audibleLocalSpeechConfirmed
    $testerAcceptance.criteria.equivalentCaptionConfirmed
    $testerAcceptance.criteria.forcedFallbackCueConfirmed
    $testerAcceptance.criteria.fallbackCaptionUnderstood
    $testerAcceptance.criteria.noBlockingObservations
    $testerAcceptance.criteria.facilitatorAttested
    @($testerAcceptance.observations.blockingCodes).Count -eq 0
    @($testerAcceptance.observations.followUpReferences).Count -eq 0
))
$testerPackagePassed = $null -ne $testerAcceptance -and (Test-AllTrue -Values @(
    $testerAcceptance.packageIdentity.packageSourceCommit -ceq
        $manifest.packageSourceCommit
    $testerAcceptance.packageIdentity.packages.developmentLauncher.actualSha256 -ceq
        $manifest.packages.developmentLauncher.sha256
    $testerAcceptance.packageIdentity.packages.developmentRuntime.actualSha256 -ceq
        $manifest.packages.developmentRuntime.sha256
    $testerAcceptance.packageIdentity.packages.shippingLauncher.actualSha256 -ceq
        $manifest.packages.shippingLauncher.sha256
    $testerAcceptance.packageIdentity.packages.shippingRuntime.actualSha256 -ceq
        $manifest.packages.shippingRuntime.sha256
    $testerAcceptance.packageIdentity.passed
))
$testerAcceptancePassed = $null -ne $testerAcceptance -and (Test-AllTrue -Values @(
    $testerAcceptance.schema -eq 1
    $testerAcceptance.evidenceKind -eq 'unassisted-human-session'
    $testerAcceptance.formalEvidence
    $testerChecksPassed
    $testerRawCriteriaPassed
    $testerPackagePassed
    $testerAcceptance.passed
))

$checks = [ordered]@{
    packageIdentityPassed = [bool]$packageIdentity.passed
    pipelinePassed = $pipelinePassed
    networkDeniedPassed = $networkDeniedPassed
    referenceTierPassed = $referenceTierPassed
    testerAcceptancePassed = $testerAcceptancePassed
}
$passed = Test-AllTrue -Values @($checks.Values)
$missingOrFailed = @(
    $checks.GetEnumerator() |
        Where-Object { $_.Value -ne $true } |
        ForEach-Object { $_.Key }
)
$result = [ordered]@{
    schema = 1
    generatedUtc = [DateTime]::UtcNow.ToString('o')
    phase = 1
    packageSourceCommit = $manifest.packageSourceCommit
    evidence = [ordered]@{
        manifest = $manifestPath
        pipeline = $pipelineResultPath
        networkDenied = $networkDeniedResultPath
        referenceTier = $referenceTierResultPath
        testerAcceptance = $testerAcceptanceResultPath
    }
    packageIdentity = $packageIdentity
    checks = $checks
    missingOrFailed = $missingOrFailed
    passed = $passed
}

$writtenPath = Write-Phase1Json -Value $result -Path $outputPath
if (-not $passed -and -not $AllowIncompleteEvidence.IsPresent) {
    throw "Phase 1 acceptance remains open. Review $writtenPath"
}

Write-Host "Phase 1 acceptance result: $writtenPath"
Write-Host "Passed: $passed"
if (-not $passed) {
    Write-Host "Missing or failed: $($missingOrFailed -join ', ')"
}
