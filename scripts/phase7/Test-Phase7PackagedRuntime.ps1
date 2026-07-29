[CmdletBinding()]
param(
    [string]$PackageRoot = ''
)

$ErrorActionPreference = 'Stop'
$repoRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..'))
$packageRoot = if ($PackageRoot) {
    [IO.Path]::GetFullPath($PackageRoot)
} else {
    Join-Path $repoRoot 'AMSim\Saved\Phase7Packages'
}
$resultDirectory = Join-Path $repoRoot 'AMSim\Saved\Phase7'
$resultPath = Join-Path $resultDirectory 'packaged-runtime.json'

function Find-PackagedLauncher {
    param([Parameter(Mandatory)][string]$Configuration)

    $configurationRoot = Join-Path $packageRoot $Configuration
    $launcher = Get-ChildItem `
        -LiteralPath $configurationRoot `
        -Filter 'AMSim.exe' `
        -File `
        -Recurse |
        Where-Object { $_.FullName -notmatch '\\AMSim\\Binaries\\Win64\\' } |
        Select-Object -First 1
    if (-not $launcher) {
        throw "Packaged $Configuration launcher not found below $configurationRoot."
    }
    return $launcher
}

function Start-PackagedGame {
    param(
        [Parameter(Mandatory)][IO.FileInfo]$Launcher,
        [Parameter(Mandatory)][string]$InnerExecutableName,
        [Parameter(Mandatory)][string[]]$Arguments
    )

    $launcherProcess = Start-Process `
        -FilePath $Launcher.FullName `
        -ArgumentList $Arguments `
        -PassThru `
        -WindowStyle Hidden
    $innerPath = Join-Path `
        $Launcher.Directory.FullName `
        "AMSim\Binaries\Win64\$InnerExecutableName"
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

function Get-SocketObservation {
    param([Parameter(Mandatory)][int]$ProcessId)

    return @(
        Get-NetTCPConnection `
            -OwningProcess $ProcessId `
            -ErrorAction SilentlyContinue |
            Select-Object State, LocalAddress, LocalPort, RemoteAddress, RemotePort
    )
}

$developmentLauncher = Find-PackagedLauncher -Configuration 'Development'
$shippingLauncher = Find-PackagedLauncher -Configuration 'Shipping'
$developmentInner = Join-Path `
    $developmentLauncher.Directory.FullName `
    'AMSim\Binaries\Win64\AMSim.exe'
$shippingInner = Join-Path `
    $shippingLauncher.Directory.FullName `
    'AMSim\Binaries\Win64\AMSim-Win64-Shipping.exe'
foreach ($required in @($developmentInner, $shippingInner)) {
    if (-not (Test-Path -LiteralPath $required -PathType Leaf)) {
        throw "Packaged runtime executable is missing: $required"
    }
}

New-Item -ItemType Directory -Path $resultDirectory -Force | Out-Null
$developmentProcess = $null
$shippingProcess = $null
$developmentTcp = @()
$shippingTcp = @()
try {
    $packagedResult = Join-Path `
        $developmentLauncher.Directory.FullName `
        'AMSim\Saved\Phase1\smoke-result.json'
    if (Test-Path -LiteralPath $packagedResult) {
        Remove-Item -LiteralPath $packagedResult -Force
    }

    $developmentProcess = Start-PackagedGame `
        -Launcher $developmentLauncher `
        -InnerExecutableName 'AMSim.exe' `
        -Arguments @(
            '-windowed',
            '-ForceRes',
            '-ResX=1920',
            '-ResY=1080',
            '-AMSimPhase1Smoke',
            '-AMSimPhase1ReferenceProfile',
            '-AMSimIgnoreUserProfile'
        )
    $deadline = [DateTime]::UtcNow.AddSeconds(90)
    while (
        -not $developmentProcess.HasExited -and
        [DateTime]::UtcNow -lt $deadline
    ) {
        $developmentTcp += Get-SocketObservation `
            -ProcessId $developmentProcess.Id
        Start-Sleep -Milliseconds 100
        $developmentProcess.Refresh()
    }
    if (-not $developmentProcess.HasExited) {
        throw 'Phase 7 packaged Development smoke exceeded 90 seconds.'
    }
    $developmentProcess = $null
    if (-not (Test-Path -LiteralPath $packagedResult -PathType Leaf)) {
        throw 'Phase 7 packaged Development smoke emitted no result.'
    }
    $smokeResult = Get-Content -LiteralPath $packagedResult -Raw |
        ConvertFrom-Json
    if (-not $smokeResult.passed) {
        throw 'Phase 7 packaged Development smoke failed.'
    }

    $shippingProcess = Start-PackagedGame `
        -Launcher $shippingLauncher `
        -InnerExecutableName 'AMSim-Win64-Shipping.exe' `
        -Arguments @(
            '-windowed',
            '-ForceRes',
            '-ResX=1280',
            '-ResY=720',
            '-AMSimIgnoreUserProfile'
        )
    $shippingDeadline = [DateTime]::UtcNow.AddSeconds(8)
    while ([DateTime]::UtcNow -lt $shippingDeadline) {
        $shippingProcess.Refresh()
        if ($shippingProcess.HasExited) {
            throw 'Shipping package exited during the eight-second launch check.'
        }
        $shippingTcp += Get-SocketObservation -ProcessId $shippingProcess.Id
        Start-Sleep -Milliseconds 100
    }
    Stop-Process -Id $shippingProcess.Id -Force
    $shippingProcess = $null

    $unexpectedDevelopmentTcp = @(
        $developmentTcp |
            Where-Object {
                -not (
                    $_.State -eq 'Listen' -and
                    $_.LocalPort -eq 1985 -and
                    $_.RemotePort -eq 0
                )
            }
    )
    if (
        $unexpectedDevelopmentTcp.Count -ne 0 -or
        $shippingTcp.Count -ne 0
    ) {
        throw 'A packaged runtime opened an unexpected TCP socket.'
    }

    $identity = [ordered]@{}
    foreach ($entry in @(
        [pscustomobject]@{
            Name = 'developmentLauncher'
            Path = $developmentLauncher.FullName
        },
        [pscustomobject]@{
            Name = 'developmentExecutable'
            Path = $developmentInner
        },
        [pscustomobject]@{
            Name = 'shippingLauncher'
            Path = $shippingLauncher.FullName
        },
        [pscustomobject]@{
            Name = 'shippingExecutable'
            Path = $shippingInner
        }
    )) {
        $identity[$entry.Name] = [ordered]@{
            path = $entry.Path
            sha256 = (Get-FileHash `
                -LiteralPath $entry.Path `
                -Algorithm SHA256).Hash.ToLowerInvariant()
        }
    }

    [ordered]@{
        schema = 1
        generatedUtc = [DateTime]::UtcNow.ToString('o')
        sourceCommit = (& git -C $repoRoot rev-parse HEAD).Trim()
        packageIdentity = $identity
        development = [ordered]@{
            journeyPassed = $smokeResult.journeyPassed
            saveLoadContinuity = $smokeResult.saveLoadContinuity
            tcpSocketObservations = $developmentTcp.Count
            expectedDeveloperTraceListenerOnly = (
                $unexpectedDevelopmentTcp.Count -eq 0
            )
        }
        shipping = [ordered]@{
            cleanLaunchSeconds = 8
            tcpSocketObservations = $shippingTcp.Count
        }
        passed = $true
    } | ConvertTo-Json -Depth 8 |
        Set-Content -LiteralPath $resultPath -Encoding utf8
}
finally {
    foreach ($process in @($developmentProcess, $shippingProcess)) {
        if ($process -and -not $process.HasExited) {
            Stop-Process -Id $process.Id -Force -ErrorAction SilentlyContinue
        }
    }
}

Write-Host "Phase 7 packaged runtime passed. Result: $resultPath"
