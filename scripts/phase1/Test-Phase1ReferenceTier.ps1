[CmdletBinding()]
param(
    [string]$PackageRoot = '',
    [ValidateRange(5, 14400)]
    [int]$SoakSeconds = 14400,
    [ValidateSet('Unreviewed', 'AtOrBelowApprovedTier')]
    [string]$TierAttestation = 'Unreviewed',
    [string]$ManifestPath = '',
    [string]$OutputPath = '',
    [switch]$AllowIncompleteEvidence
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
    Join-Path $repoRoot 'AMSim\Saved\Phase1\reference-tier-result.json'
}

function Get-Median {
    param(
        [Parameter(Mandatory)]
        [AllowEmptyCollection()]
        [double[]]$Values
    )

    if ($Values.Count -eq 0) {
        return 0.0
    }
    $ordered = @($Values | Sort-Object)
    $middle = [int][Math]::Floor($ordered.Count / 2)
    if ($ordered.Count % 2 -eq 1) {
        return [double]$ordered[$middle]
    }
    return ([double]$ordered[$middle - 1] + [double]$ordered[$middle]) / 2.0
}

function Start-PackagedGame {
    param(
        [Parameter(Mandatory)][System.IO.FileInfo]$Launcher,
        [Parameter(Mandatory)][string]$InnerExecutableName,
        [Parameter(Mandatory)][string[]]$Arguments
    )

    $innerPath = Join-Path $Launcher.Directory.FullName "AMSim\Binaries\Win64\$InnerExecutableName"
    $existing = Get-Process -ErrorAction SilentlyContinue |
        Where-Object { $_.Path -eq $innerPath }
    if ($existing) {
        throw "Close the existing packaged game before measuring it: $innerPath"
    }

    $launcherProcess = Start-Process `
        -FilePath $Launcher.FullName `
        -ArgumentList $Arguments `
        -PassThru
    $deadline = [DateTime]::UtcNow.AddSeconds(15)
    do {
        $innerProcess = Get-Process -ErrorAction SilentlyContinue |
            Where-Object { $_.Path -eq $innerPath } |
            Select-Object -First 1
        if ($innerProcess) {
            return $innerProcess
        }
        Start-Sleep -Milliseconds 100
    } while ([DateTime]::UtcNow -lt $deadline)

    if (-not $launcherProcess.HasExited) {
        Stop-Process -Id $launcherProcess.Id -Force -ErrorAction SilentlyContinue
    }
    throw "Packaged child process did not start: $innerPath"
}

$manifest = Get-Phase1AcceptanceManifest -Path $manifestPath
$packageIdentity = Get-Phase1PackageIdentity `
    -PackageRoot $packageRoot `
    -Manifest $manifest
if (-not $packageIdentity.passed) {
    throw 'The exact final Phase 1 Development and Shipping packages must exist before reference-tier measurement.'
}
$developmentExe = Get-Item -LiteralPath (
    $packageIdentity.packages.developmentLauncher.path)
$developmentInnerExe = Get-Item -LiteralPath (
    $packageIdentity.packages.developmentRuntime.path)

$os = Get-CimInstance Win32_OperatingSystem
$computerSystem = Get-CimInstance Win32_ComputerSystem
$processors = @(Get-CimInstance Win32_Processor)
$videoControllers = @(Get-CimInstance Win32_VideoController)
$packageDrive = Get-CimInstance Win32_LogicalDisk |
    Where-Object { $developmentExe.FullName.StartsWith($_.DeviceID, [StringComparison]::OrdinalIgnoreCase) } |
    Select-Object -First 1
$commit = ''
$sourceTreeClean = $false
try {
    $commitOutput = & git -C $repoRoot rev-parse HEAD 2>$null
    if ($LASTEXITCODE -eq 0) {
        $commit = ($commitOutput | Out-String).Trim()
        $statusOutput = @(& git -C $repoRoot status --porcelain 2>$null)
        $sourceTreeClean = $LASTEXITCODE -eq 0 -and $statusOutput.Count -eq 0
    }
} catch {
    $commit = ''
    $sourceTreeClean = $false
}

$packagedResultPath = Join-Path $developmentExe.Directory.FullName 'AMSim\Saved\Phase1\smoke-result.json'
if (Test-Path -LiteralPath $packagedResultPath) {
    Remove-Item -LiteralPath $packagedResultPath -Force
}

$memorySamples = @()
$developmentProcess = $null
$stopwatch = [Diagnostics.Stopwatch]::StartNew()
try {
    $developmentProcess = Start-PackagedGame `
        -Launcher $developmentExe `
        -InnerExecutableName 'AMSim.exe' `
        -Arguments @(
            '-windowed',
            '-ForceRes',
            '-ResX=1920',
            '-ResY=1080',
            '-AMSimPhase1Smoke',
            '-AMSimPhase1ReferenceProfile',
            "-AMSimPhase1PerformanceSeconds=$SoakSeconds"
        )
    $sampleIntervalSeconds = [Math]::Max(1.0, [Math]::Min(60.0, $SoakSeconds / 60.0))
    $nextSampleSeconds = 0.0
    $deadline = [DateTime]::UtcNow.AddSeconds($SoakSeconds + 180)
    while (-not $developmentProcess.HasExited -and [DateTime]::UtcNow -lt $deadline) {
        if ($stopwatch.Elapsed.TotalSeconds -ge $nextSampleSeconds) {
            $developmentProcess.Refresh()
            $memorySamples += [ordered]@{
                elapsedSeconds = [Math]::Round($stopwatch.Elapsed.TotalSeconds, 3)
                workingSetMiB = [Math]::Round(
                    $developmentProcess.WorkingSet64 / (1024.0 * 1024.0),
                    3)
            }
            $nextSampleSeconds += $sampleIntervalSeconds
        }
        Start-Sleep -Milliseconds 250
        $developmentProcess.Refresh()
    }
    if (-not $developmentProcess.HasExited) {
        throw "Reference-tier S01/soak exceeded $($SoakSeconds + 180) seconds."
    }
} finally {
    $stopwatch.Stop()
    if ($developmentProcess -and -not $developmentProcess.HasExited) {
        Stop-Process -Id $developmentProcess.Id -Force -ErrorAction SilentlyContinue
    }
}

if (-not (Test-Path -LiteralPath $packagedResultPath)) {
    throw 'The reference-tier packaged run did not emit smoke-result.json.'
}
$smokeResult = Get-Content -LiteralPath $packagedResultPath -Raw | ConvertFrom-Json

$windowSize = [Math]::Max(1, [int][Math]::Ceiling($memorySamples.Count / 4.0))
$firstWindow = @($memorySamples | Select-Object -First $windowSize)
$lastWindow = @($memorySamples | Select-Object -Last $windowSize)
$firstMemoryMedian = Get-Median -Values @($firstWindow | ForEach-Object { $_.workingSetMiB })
$lastMemoryMedian = Get-Median -Values @($lastWindow | ForEach-Object { $_.workingSetMiB })
$observedMemoryGrowthMiB = $lastMemoryMedian - $firstMemoryMedian
$memoryTrendPassed =
    $smokeResult.maximumMemoryMiB -lt 4096 -and
    $observedMemoryGrowthMiB -le 64.0
$metricsPassed =
    $smokeResult.passed -and
    $smokeResult.averageFps -ge 60.0 -and
    $smokeResult.p99FrameMilliseconds -le 16.6 -and
    $smokeResult.simulationMedianMilliseconds -le 4.0 -and
    $smokeResult.simulationP99Milliseconds -le 8.0 -and
    $smokeResult.maximum8xBacklogSteps -eq 0 -and
    $smokeResult.snapshotCaptureMilliseconds -le 50.0 -and
    $smokeResult.saveWriteMilliseconds -le 2000.0 -and
    $smokeResult.maximumMemoryMiB -lt 4096 -and
    $smokeResult.referenceProfileApplied
$formalSoakPassed =
    $SoakSeconds -eq 14400 -and
    $smokeResult.performanceTargetSeconds -eq 14400 -and
    $smokeResult.elapsedSeconds -ge 14400 -and
    $memorySamples.Count -ge 200 -and
    $memoryTrendPassed
$tierAttestationPassed = $TierAttestation -eq 'AtOrBelowApprovedTier'
$passed =
    $packageIdentity.passed -and
    $metricsPassed -and
    $formalSoakPassed -and
    $tierAttestationPassed -and
    $sourceTreeClean

$result = [ordered]@{
    schema = 1
    generatedUtc = [DateTime]::UtcNow.ToString('o')
    scenario = 'S01.StarterGrassAirfield.ReferenceTier'
    sourceCommit = $commit
    sourceTreeClean = $sourceTreeClean
    acceptanceManifest = $manifestPath
    packageSourceCommit = $manifest.packageSourceCommit
    packageIdentity = $packageIdentity
    package = [ordered]@{
        root = $packageRoot
        launcher = $developmentExe.FullName
        launcherSha256 = (Get-FileHash -LiteralPath $developmentExe.FullName -Algorithm SHA256).Hash
        runtime = $developmentInnerExe.FullName
        runtimeSha256 = (Get-FileHash -LiteralPath $developmentInnerExe.FullName -Algorithm SHA256).Hash
    }
    hardware = [ordered]@{
        tierAttestation = $TierAttestation
        operatingSystem = [ordered]@{
            caption = $os.Caption
            version = $os.Version
            buildNumber = $os.BuildNumber
        }
        processors = @($processors | ForEach-Object {
            [ordered]@{
                name = $_.Name.Trim()
                physicalCores = $_.NumberOfCores
                logicalProcessors = $_.NumberOfLogicalProcessors
                maximumClockMHz = $_.MaxClockSpeed
            }
        })
        systemMemoryMiB = [Math]::Round(
            $computerSystem.TotalPhysicalMemory / (1024.0 * 1024.0),
            0)
        videoControllers = @($videoControllers | ForEach-Object {
            [ordered]@{
                name = $_.Name
                driverVersion = $_.DriverVersion
                adapterMemoryMiB = if ($_.AdapterRAM) {
                    [Math]::Round([uint64]$_.AdapterRAM / (1024.0 * 1024.0), 0)
                } else {
                    $null
                }
                currentResolution = if ($_.CurrentHorizontalResolution -and $_.CurrentVerticalResolution) {
                    "$($_.CurrentHorizontalResolution)x$($_.CurrentVerticalResolution)"
                } else {
                    ''
                }
            }
        })
        packageDrive = if ($packageDrive) {
            [ordered]@{
                deviceId = $packageDrive.DeviceID
                sizeGiB = [Math]::Round($packageDrive.Size / 1GB, 1)
                freeGiB = [Math]::Round($packageDrive.FreeSpace / 1GB, 1)
            }
        } else {
            $null
        }
    }
    referenceProfile = $smokeResult.scalability
    soak = [ordered]@{
        requestedSeconds = $SoakSeconds
        measuredSeconds = $smokeResult.elapsedSeconds
        sampleIntervalSeconds = $sampleIntervalSeconds
        sampleCount = $memorySamples.Count
        firstQuarterMedianMiB = [Math]::Round($firstMemoryMedian, 3)
        lastQuarterMedianMiB = [Math]::Round($lastMemoryMedian, 3)
        medianGrowthMiB = [Math]::Round($observedMemoryGrowthMiB, 3)
        memoryTrendPassed = $memoryTrendPassed
        samples = $memorySamples
    }
    metrics = $smokeResult
    checks = [ordered]@{
        packageIdentityPassed = [bool]$packageIdentity.passed
        tierAttestationPassed = $tierAttestationPassed
        sourceTreeClean = $sourceTreeClean
        metricsPassed = $metricsPassed
        formalFourHourSoakPassed = $formalSoakPassed
    }
    passed = $passed
}

$writtenPath = Write-Phase1Json -Value $result -Path $outputPath

if (-not $passed -and -not $AllowIncompleteEvidence) {
    throw "Reference-tier Phase 1 acceptance did not pass. Review $writtenPath"
}

Write-Host "Reference-tier Phase 1 result: $writtenPath"
Write-Host "Passed: $passed"
