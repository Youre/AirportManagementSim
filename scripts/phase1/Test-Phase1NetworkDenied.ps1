[CmdletBinding()]
param(
    [string]$PackageRoot = '',
    [string]$ManifestPath = ''
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
$manifest = Get-Phase1AcceptanceManifest -Path $manifestPath
$resultDirectory = Join-Path $repoRoot 'AMSim\Saved\Phase1'
$resultPath = Join-Path $resultDirectory 'network-denied-result.json'

$identity = [Security.Principal.WindowsIdentity]::GetCurrent()
$principal = [Security.Principal.WindowsPrincipal]::new($identity)
if (-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    throw 'The Phase 1 network-denied smoke requires an elevated PowerShell session.'
}

$developmentExe = Get-ChildItem `
    -LiteralPath (Join-Path $packageRoot 'Development') `
    -Filter 'AMSim.exe' `
    -File `
    -Recurse |
    Where-Object { $_.FullName -notmatch '\\AMSim\\Binaries\\Win64\\' } |
    Select-Object -First 1
$shippingExe = Get-ChildItem `
    -LiteralPath (Join-Path $packageRoot 'Shipping') `
    -Filter 'AMSim.exe' `
    -File `
    -Recurse |
    Where-Object { $_.FullName -notmatch '\\AMSim\\Binaries\\Win64\\' } |
    Select-Object -First 1
if (-not $developmentExe -or -not $shippingExe) {
    throw 'Development and Shipping Phase 1 packages must exist first.'
}
$packageIdentity = Get-Phase1PackageIdentity `
    -PackageRoot $packageRoot `
    -Manifest $manifest
if (-not $packageIdentity.passed) {
    throw 'The Phase 1 packages do not match the acceptance manifest.'
}

function Start-PackagedGame {
    param(
        [Parameter(Mandatory)][System.IO.FileInfo]$Launcher,
        [Parameter(Mandatory)][string]$InnerExecutableName,
        [Parameter(Mandatory)][string[]]$Arguments
    )
    $launcherProcess = Start-Process `
        -FilePath $Launcher.FullName `
        -ArgumentList $Arguments `
        -PassThru `
        -WindowStyle Hidden
    $innerPath = Join-Path $Launcher.Directory.FullName "AMSim\Binaries\Win64\$InnerExecutableName"
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

New-Item -ItemType Directory -Path $resultDirectory -Force | Out-Null
$rules = @()
$developmentTcp = @()
$shippingTcp = @()
$developmentProcess = $null
$shippingProcess = $null
try {
    $ruleSuffix = [Guid]::NewGuid().ToString('N')
    foreach ($target in @(
        [pscustomobject]@{ Name = 'DevelopmentLauncher'; Path = $developmentExe.FullName },
        [pscustomobject]@{
            Name = 'DevelopmentGame'
            Path = (Join-Path $developmentExe.Directory.FullName 'AMSim\Binaries\Win64\AMSim.exe')
        },
        [pscustomobject]@{ Name = 'ShippingLauncher'; Path = $shippingExe.FullName },
        [pscustomobject]@{
            Name = 'ShippingGame'
            Path = (Join-Path $shippingExe.Directory.FullName 'AMSim\Binaries\Win64\AMSim-Win64-Shipping.exe')
        }
    )) {
        foreach ($direction in @('Inbound', 'Outbound')) {
            $rules += New-NetFirewallRule `
                -DisplayName "AMSim Phase1 $($target.Name) $direction $ruleSuffix" `
                -Direction $direction `
                -Program $target.Path `
                -Action Block `
                -Profile Any
        }
    }

    $packagedResult = Join-Path $developmentExe.Directory.FullName 'AMSim\Saved\Phase1\smoke-result.json'
    if (Test-Path -LiteralPath $packagedResult) {
        Remove-Item -LiteralPath $packagedResult -Force
    }
    $developmentProcess = Start-PackagedGame `
        -Launcher $developmentExe `
        -InnerExecutableName 'AMSim.exe' `
        -Arguments @(
            '-windowed',
            '-ForceRes',
            '-ResX=1920',
            '-ResY=1080',
            '-AMSimPhase1Smoke',
            '-AMSimPhase1ReferenceProfile'
        )
    $deadline = [DateTime]::UtcNow.AddSeconds(90)
    while (-not $developmentProcess.HasExited -and [DateTime]::UtcNow -lt $deadline) {
        $developmentTcp += Get-NetTCPConnection -OwningProcess $developmentProcess.Id -ErrorAction SilentlyContinue |
            Select-Object State, LocalAddress, LocalPort, RemoteAddress, RemotePort
        Start-Sleep -Milliseconds 100
        $developmentProcess.Refresh()
    }
    if (-not $developmentProcess.HasExited) {
        Stop-Process -Id $developmentProcess.Id -Force
        $developmentProcess = $null
        throw 'Network-denied Development S01 exceeded 90 seconds.'
    }
    $developmentProcess = $null
    if (-not (Test-Path -LiteralPath $packagedResult)) {
        throw 'Network-denied Development S01 did not emit a result.'
    }
    $smokeResult = Get-Content -LiteralPath $packagedResult -Raw | ConvertFrom-Json
    if (-not $smokeResult.passed) {
        throw 'Network-denied Development S01 failed.'
    }

    $shippingProcess = Start-PackagedGame `
        -Launcher $shippingExe `
        -InnerExecutableName 'AMSim-Win64-Shipping.exe' `
        -Arguments @('-windowed', '-ForceRes', '-ResX=1280', '-ResY=720')
    Start-Sleep -Seconds 5
    $shippingProcess.Refresh()
    if ($shippingProcess.HasExited) {
        throw 'Network-denied Shipping package exited during its clean-launch check.'
    }
    $shippingTcp = @(
        Get-NetTCPConnection -OwningProcess $shippingProcess.Id -ErrorAction SilentlyContinue |
            Select-Object State, LocalAddress, LocalPort, RemoteAddress, RemotePort
    )
    Stop-Process -Id $shippingProcess.Id -Force
    $shippingProcess = $null

    $unexpectedDevelopmentTcp = @($developmentTcp | Where-Object {
        -not ($_.State -eq 'Listen' -and $_.LocalPort -eq 1985 -and $_.RemotePort -eq 0)
    })
    if ($unexpectedDevelopmentTcp.Count -ne 0 -or $shippingTcp.Count -ne 0) {
        throw 'A Phase 1 packaged runtime opened an unexpected TCP socket while network access was denied.'
    }

    [ordered]@{
        schema = 1
        generatedUtc = [DateTime]::UtcNow.ToString('o')
        packageIdentity = $packageIdentity
        firewallDirections = @('Inbound', 'Outbound')
        firewallProfiles = 'Any'
        development = [ordered]@{
            journeyPassed = $smokeResult.journeyPassed
            saveLoadContinuity = $smokeResult.saveLoadContinuity
            p99FrameMilliseconds = $smokeResult.p99FrameMilliseconds
            maximum8xBacklogSteps = $smokeResult.maximum8xBacklogSteps
            tcpSocketObservations = $developmentTcp.Count
            expectedDeveloperTraceListenerOnly = $unexpectedDevelopmentTcp.Count -eq 0
        }
        shipping = [ordered]@{
            cleanLaunchSeconds = 5
            tcpSocketObservations = $shippingTcp.Count
        }
        passed = $true
    } | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $resultPath -Encoding utf8
}
finally {
    foreach ($process in @($developmentProcess, $shippingProcess)) {
        if ($process -and -not $process.HasExited) {
            Stop-Process -Id $process.Id -Force -ErrorAction SilentlyContinue
        }
    }
    foreach ($rule in $rules) {
        Remove-NetFirewallRule -Name $rule.Name -ErrorAction SilentlyContinue
    }
}

Write-Host "Network-denied Phase 1 smoke passed. Result: $resultPath"
