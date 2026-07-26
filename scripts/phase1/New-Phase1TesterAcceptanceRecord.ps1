[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [ValidatePattern('^[A-Za-z0-9][A-Za-z0-9_-]{0,47}$')]
    [string]$SessionId,

    [Parameter(Mandatory)]
    [ValidateSet('5-7', '8-10', 'adult proxy')]
    [string]$TesterBand,

    [Parameter(Mandatory)]
    [DateTimeOffset]$SessionStarted,

    [Parameter(Mandatory)]
    [DateTimeOffset]$SessionEnded,

    [ValidateRange(0, 100)]
    [int]$GameplayHints = 0,

    [ValidateRange(1, 16384)]
    [int]$DisplayWidth = 1920,

    [ValidateRange(1, 16384)]
    [int]$DisplayHeight = 1080,

    [ValidateRange(50, 400)]
    [int]$UiScalePercent = 100,

    [ValidatePattern('^[A-Z][A-Z0-9-]{0,31}$')]
    [string[]]$BlockingObservationCodes = @(),

    [ValidatePattern('^[A-Z][A-Z0-9-]{0,31}$')]
    [string[]]$FollowUpReferences = @(),

    [string]$PackageRoot = '',
    [string]$ManifestPath = '',
    [string]$OutputPath = '',

    [switch]$FirstTimeTesterConfirmed,
    [switch]$ParentGuardianConsentConfirmed,
    [switch]$EmptySaveSlotConfirmed,
    [switch]$AudioOutputPrechecked,
    [switch]$ProtocolHiddenFromTester,
    [switch]$NoPersonalMediaCaptured,
    [switch]$JourneyCompleted,
    [switch]$SaveLoadRecognized,
    [switch]$CompatibilityUnderstood,
    [switch]$RouteStandUnderstood,
    [switch]$ServicesUnderstood,
    [switch]$RewardUnderstood,
    [switch]$AudibleLocalSpeechConfirmed,
    [switch]$EquivalentCaptionConfirmed,
    [switch]$ForcedFallbackCueConfirmed,
    [switch]$FallbackCaptionUnderstood,
    [switch]$NoBlockingObservations,
    [switch]$FacilitatorAttested,
    [switch]$RecordFormalEvidence,
    [switch]$AllowFailedEvidence
)

$ErrorActionPreference = 'Stop'
$repoRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..'))
. (Join-Path $PSScriptRoot 'Phase1Acceptance.Common.ps1')

$packageRoot = if ($PackageRoot) {
    [System.IO.Path]::GetFullPath($PackageRoot)
} else {
    Join-Path $repoRoot 'AMSim\Saved\Phase1Packages'
}
$manifestPath = if ($ManifestPath) {
    [System.IO.Path]::GetFullPath($ManifestPath)
} else {
    Join-Path $PSScriptRoot 'Phase1AcceptanceManifest.json'
}
$outputPath = if ($OutputPath) {
    [System.IO.Path]::GetFullPath($OutputPath)
} else {
    Join-Path $repoRoot 'AMSim\Saved\Phase1\tester-acceptance-result.json'
}

if ($SessionEnded -le $SessionStarted) {
    throw 'SessionEnded must be later than SessionStarted.'
}
$elapsedMinutes = [Math]::Round(
    ($SessionEnded.ToUniversalTime() - $SessionStarted.ToUniversalTime()).TotalMinutes,
    2)
if ($elapsedMinutes -gt 360) {
    throw 'A Phase 1 tester session cannot exceed 360 minutes.'
}

$manifest = Get-Phase1AcceptanceManifest -Path $manifestPath
$packageIdentity = Get-Phase1PackageIdentity `
    -PackageRoot $packageRoot `
    -Manifest $manifest

$isChildBand = $TesterBand -ne 'adult proxy'
$consentPassed = -not $isChildBand -or $ParentGuardianConsentConfirmed.IsPresent
$displayPassed =
    $DisplayWidth -eq 1920 -and
    $DisplayHeight -eq 1080 -and
    $UiScalePercent -eq 100
$observationRecordConsistent =
    (
        $NoBlockingObservations.IsPresent -and
        $BlockingObservationCodes.Count -eq 0 -and
        $FollowUpReferences.Count -eq 0
    ) -or (
        -not $NoBlockingObservations.IsPresent -and
        $BlockingObservationCodes.Count -gt 0 -and
        $FollowUpReferences.Count -gt 0
    )

$criteria = [ordered]@{
    firstTimeTesterConfirmed = $FirstTimeTesterConfirmed.IsPresent
    parentGuardianConsentConfirmed = if ($isChildBand) {
        $ParentGuardianConsentConfirmed.IsPresent
    } else {
        $null
    }
    emptySaveSlotConfirmed = $EmptySaveSlotConfirmed.IsPresent
    audioOutputPrechecked = $AudioOutputPrechecked.IsPresent
    protocolHiddenFromTester = $ProtocolHiddenFromTester.IsPresent
    noPersonalMediaCaptured = $NoPersonalMediaCaptured.IsPresent
    journeyCompleted = $JourneyCompleted.IsPresent
    gameplayHints = $GameplayHints
    saveLoadRecognized = $SaveLoadRecognized.IsPresent
    compatibilityUnderstood = $CompatibilityUnderstood.IsPresent
    routeStandUnderstood = $RouteStandUnderstood.IsPresent
    servicesUnderstood = $ServicesUnderstood.IsPresent
    rewardUnderstood = $RewardUnderstood.IsPresent
    audibleLocalSpeechConfirmed = $AudibleLocalSpeechConfirmed.IsPresent
    equivalentCaptionConfirmed = $EquivalentCaptionConfirmed.IsPresent
    forcedFallbackCueConfirmed = $ForcedFallbackCueConfirmed.IsPresent
    fallbackCaptionUnderstood = $FallbackCaptionUnderstood.IsPresent
    noBlockingObservations = $NoBlockingObservations.IsPresent
    facilitatorAttested = $FacilitatorAttested.IsPresent
}

$checks = [ordered]@{
    formalEvidence = $RecordFormalEvidence.IsPresent
    sessionTimingPassed = $elapsedMinutes -gt 0
    consentPassed = $consentPassed
    packageIdentityPassed = [bool]$packageIdentity.passed
    displayPrerequisitesPassed = $displayPassed
    privacyPreconditionsPassed = $NoPersonalMediaCaptured.IsPresent
    testerEligibilityPassed = $FirstTimeTesterConfirmed.IsPresent
    cleanStartPassed = $EmptySaveSlotConfirmed.IsPresent
    protocolIsolationPassed = $ProtocolHiddenFromTester.IsPresent
    audioPreconditionPassed = $AudioOutputPrechecked.IsPresent
    journeyPassed = $JourneyCompleted.IsPresent
    zeroGameplayHintsPassed = $GameplayHints -eq 0
    saveLoadPassed = $SaveLoadRecognized.IsPresent
    comprehensionPassed =
        $CompatibilityUnderstood.IsPresent -and
        $RouteStandUnderstood.IsPresent -and
        $ServicesUnderstood.IsPresent -and
        $RewardUnderstood.IsPresent
    normalAudioCaptionPassed =
        $AudibleLocalSpeechConfirmed.IsPresent -and
        $EquivalentCaptionConfirmed.IsPresent
    forcedFallbackPassed =
        $ForcedFallbackCueConfirmed.IsPresent -and
        $FallbackCaptionUnderstood.IsPresent
    noBlockingObservationsPassed = $NoBlockingObservations.IsPresent
    observationRecordConsistent = $observationRecordConsistent
    facilitatorAttestationPassed = $FacilitatorAttested.IsPresent
}

$passed = $true
foreach ($check in $checks.Values) {
    if ($check -ne $true) {
        $passed = $false
        break
    }
}

$result = [ordered]@{
    schema = 1
    generatedUtc = [DateTime]::UtcNow.ToString('o')
    evidenceKind = if ($RecordFormalEvidence.IsPresent) {
        'unassisted-human-session'
    } else {
        'non-certifying-record'
    }
    formalEvidence = $RecordFormalEvidence.IsPresent
    session = [ordered]@{
        id = $SessionId
        started = $SessionStarted.ToString('o')
        ended = $SessionEnded.ToString('o')
        elapsedMinutes = $elapsedMinutes
        recorderTimeZone = [TimeZoneInfo]::Local.Id
        testerBand = $TesterBand
        display = [ordered]@{
            width = $DisplayWidth
            height = $DisplayHeight
            uiScalePercent = $UiScalePercent
        }
    }
    packageIdentity = $packageIdentity
    criteria = $criteria
    observations = [ordered]@{
        blockingCodes = @($BlockingObservationCodes)
        followUpReferences = @($FollowUpReferences)
    }
    checks = $checks
    passed = $passed
}

$writtenPath = Write-Phase1Json -Value $result -Path $outputPath
if (-not $passed -and -not $AllowFailedEvidence.IsPresent) {
    throw "Phase 1 tester acceptance did not pass. Review $writtenPath"
}

Write-Host "Phase 1 tester acceptance result: $writtenPath"
Write-Host "Passed: $passed"
