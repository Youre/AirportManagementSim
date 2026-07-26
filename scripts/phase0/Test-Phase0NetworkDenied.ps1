[CmdletBinding()]
param(
    [string]$PackageRoot = ''
)

$ErrorActionPreference = 'Stop'
$repoRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..'))
$packageRoot = if ($PackageRoot) {
    [System.IO.Path]::GetFullPath($PackageRoot)
} else {
    Join-Path $repoRoot 'AMSim\Saved\Phase0Packages'
}
$resultDirectory = Join-Path $repoRoot 'AMSim\Saved\Phase0'
$resultPath = Join-Path $resultDirectory 'network-denied-result.json'

$identity = [Security.Principal.WindowsIdentity]::GetCurrent()
$principal = [Security.Principal.WindowsPrincipal]::new($identity)
if (-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    throw 'The network-denied smoke test requires an elevated PowerShell session.'
}

$developmentExe = Get-ChildItem -LiteralPath (Join-Path $packageRoot 'Development') -Filter 'AMSim.exe' -File -Recurse |
    Select-Object -First 1
$shippingExe = Get-ChildItem -LiteralPath (Join-Path $packageRoot 'Shipping') -Filter 'AMSim.exe' -File -Recurse |
    Select-Object -First 1
if (-not $developmentExe -or -not $shippingExe) {
    throw 'Development and Shipping packages must exist before the network-denied smoke test.'
}

function Start-PackagedGame {
    param(
        [Parameter(Mandatory)][System.IO.FileInfo]$Launcher,
        [Parameter(Mandatory)][string]$InnerExecutableName,
        [Parameter(Mandatory)][string[]]$Arguments
    )
    $launcherProcess = Start-Process -FilePath $Launcher.FullName -ArgumentList $Arguments -PassThru
    $innerPath = Join-Path $Launcher.Directory.FullName "AMSim\Binaries\Win64\$InnerExecutableName"
    $deadline = [DateTime]::UtcNow.AddSeconds(10)
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
        $rules += New-NetFirewallRule `
            -DisplayName "AMSim Phase0 $($target.Name) Outbound $ruleSuffix" `
            -Direction Outbound `
            -Program $target.Path `
            -Action Block `
            -Profile Any
        $rules += New-NetFirewallRule `
            -DisplayName "AMSim Phase0 $($target.Name) Inbound $ruleSuffix" `
            -Direction Inbound `
            -Program $target.Path `
            -Action Block `
            -Profile Any
    }

    $packagedSmokeResult = Join-Path $developmentExe.Directory.FullName 'AMSim\Saved\Phase0\smoke-result.json'
    if (Test-Path -LiteralPath $packagedSmokeResult) {
        Remove-Item -LiteralPath $packagedSmokeResult -Force
    }
    $developmentProcess = Start-PackagedGame `
        -Launcher $developmentExe `
        -InnerExecutableName 'AMSim.exe' `
        -Arguments @('-windowed', '-ForceRes', '-ResX=1920', '-ResY=1080', '-nosound', '-AMSimPhase0Smoke')
    $deadline = [DateTime]::UtcNow.AddSeconds(45)
    while (-not $developmentProcess.HasExited -and [DateTime]::UtcNow -lt $deadline) {
        $developmentTcp += Get-NetTCPConnection -OwningProcess $developmentProcess.Id -ErrorAction SilentlyContinue |
            Select-Object State, LocalAddress, LocalPort, RemoteAddress, RemotePort
        Start-Sleep -Milliseconds 100
        $developmentProcess.Refresh()
    }
    if (-not $developmentProcess.HasExited) {
        Stop-Process -Id $developmentProcess.Id -Force
        throw 'Network-denied Development smoke exceeded 45 seconds.'
    }
    if (-not (Test-Path -LiteralPath $packagedSmokeResult)) {
        throw 'Network-denied Development smoke did not emit its result.'
    }
    $smokeResult = Get-Content -LiteralPath $packagedSmokeResult -Raw | ConvertFrom-Json
    if (-not $smokeResult.passed) {
        throw 'Network-denied Development save/load/performance smoke failed.'
    }

    $shippingProcess = Start-PackagedGame `
        -Launcher $shippingExe `
        -InnerExecutableName 'AMSim-Win64-Shipping.exe' `
        -Arguments @('-windowed', '-ForceRes', '-ResX=1280', '-ResY=720', '-nosound')
    Start-Sleep -Seconds 5
    $shippingProcess.Refresh()
    if ($shippingProcess.HasExited) {
        throw 'Network-denied Shipping package exited unexpectedly during its five-second check.'
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
        throw 'A packaged runtime opened an unexpected TCP socket while its network access was denied.'
    }

    [ordered]@{
        schema = 1
        generatedUtc = [DateTime]::UtcNow.ToString('o')
        firewallDirections = @('Inbound', 'Outbound')
        firewallProfiles = 'Any'
        development = [ordered]@{
            exitCode = 0
            saveLoadContinuity = $smokeResult.saveLoadContinuity
            averageFps = $smokeResult.averageFps
            p99FrameMilliseconds = $smokeResult.p99FrameMilliseconds
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
    if ($shippingProcess -and -not $shippingProcess.HasExited) {
        Stop-Process -Id $shippingProcess.Id -Force -ErrorAction SilentlyContinue
    }
    foreach ($rule in $rules) {
        Remove-NetFirewallRule -Name $rule.Name -ErrorAction SilentlyContinue
    }
}

Write-Host "Network-denied Phase 0 smoke passed. Result: $resultPath"
