[CmdletBinding()]
param(
    [string]$EngineRoot = 'C:\Program Files\Epic Games\UE_5.8',
    [string]$PackageRoot = '',
    [switch]$SkipBuild,
    [switch]$SkipFirewall
)

$ErrorActionPreference = 'Stop'
$repoRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..'))
$projectPath = Join-Path $repoRoot 'AMSim\AMSim.uproject'
$savedRoot = Join-Path $repoRoot 'AMSim\Saved\Phase0'
$packageRoot = if ($PackageRoot) {
    [System.IO.Path]::GetFullPath($PackageRoot)
} else {
    Join-Path $repoRoot 'AMSim\Saved\Phase0Packages'
}
$manifestPath = Join-Path $repoRoot 'docs\planning\30-content-and-assets\aircraft-import-manifest-template.csv'
$buildScript = Join-Path $EngineRoot 'Engine\Build\BatchFiles\Build.bat'
$uatScript = Join-Path $EngineRoot 'Engine\Build\BatchFiles\RunUAT.bat'
$editorCommand = Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'

function Invoke-Native {
    param(
        [Parameter(Mandatory)]
        [string]$Executable,
        [Parameter(Mandatory)]
        [string[]]$Arguments
    )
    & $Executable @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "$Executable exited with code $LASTEXITCODE."
    }
}

function Reset-Directory {
    param([Parameter(Mandatory)][string]$Path)
    $resolvedTarget = [System.IO.Path]::GetFullPath($Path)
    $allowedBoundaries = @(
        [System.IO.Path]::GetFullPath((Join-Path $repoRoot 'AMSim\Saved')),
        [System.IO.Path]::GetFullPath($packageRoot)
    )
    $isAllowed = @($allowedBoundaries | Where-Object {
        $resolvedTarget.Equals($_, [System.StringComparison]::OrdinalIgnoreCase) -or
        $resolvedTarget.StartsWith($_.TrimEnd('\') + '\', [System.StringComparison]::OrdinalIgnoreCase)
    }).Count -gt 0
    if (-not $isAllowed) {
        throw "Refusing to reset a directory outside the Phase 0 generated roots: $resolvedTarget"
    }
    if (Test-Path -LiteralPath $resolvedTarget) {
        Remove-Item -LiteralPath $resolvedTarget -Recurse -Force
    }
    New-Item -ItemType Directory -Path $resolvedTarget -Force | Out-Null
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

foreach ($requiredPath in @($projectPath, $manifestPath, $buildScript, $uatScript, $editorCommand)) {
    if (-not (Test-Path -LiteralPath $requiredPath)) {
        throw "Required Phase 0 path is missing: $requiredPath"
    }
}
if (-not $SkipFirewall) {
    $identity = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = [Security.Principal.WindowsPrincipal]::new($identity)
    if (-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
        throw 'Run this pipeline from an elevated PowerShell session, or use -SkipFirewall for a non-network package check.'
    }
}
if (Get-Process UnrealEditor -ErrorAction SilentlyContinue) {
    throw 'Close Unreal Editor before running the Phase 0 native build pipeline.'
}

Reset-Directory -Path $savedRoot
Reset-Directory -Path $packageRoot

if (-not $SkipBuild) {
    Invoke-Native -Executable $buildScript -Arguments @(
        'AMSimEditor', 'Win64', 'Development', $projectPath, '-WaitMutex', '-NoHotReload'
    )
}

Invoke-Native -Executable $editorCommand -Arguments @(
    $projectPath,
    '-run=AMSimProjectAudit',
    '-unattended',
    '-nop4',
    '-NullRHI',
    "-Manifest=$manifestPath"
)
Invoke-Native -Executable $editorCommand -Arguments @(
    $projectPath,
    '-unattended',
    '-nop4',
    '-NullRHI',
    '-ExecCmds=Automation RunTests AMSim.Phase0',
    '-TestExit=Automation Test Queue Empty'
)

foreach ($configuration in @('Development', 'Shipping')) {
    $archiveDirectory = Join-Path $packageRoot $configuration
    Invoke-Native -Executable $uatScript -Arguments @(
        'BuildCookRun',
        "-project=$projectPath",
        '-noP4',
        '-platform=Win64',
        "-clientconfig=$configuration",
        '-build',
        '-cook',
        '-stage',
        '-pak',
        '-package',
        '-archive',
        "-archivedirectory=$archiveDirectory",
        '-unattended',
        '-utf8output'
    )
}

$developmentExe = Get-ChildItem -LiteralPath (Join-Path $packageRoot 'Development') -Filter 'AMSim.exe' -File -Recurse |
    Select-Object -First 1
$shippingExe = Get-ChildItem -LiteralPath (Join-Path $packageRoot 'Shipping') -Filter 'AMSim.exe' -File -Recurse |
    Select-Object -First 1
if (-not $developmentExe -or -not $shippingExe) {
    throw 'One or both packaged AMSim executables are missing.'
}

$firewallRules = @()
$networkRows = @()
try {
    if (-not $SkipFirewall) {
        $ruleSuffix = [Guid]::NewGuid().ToString('N')
        foreach ($program in @(
            $developmentExe.FullName,
            (Join-Path $developmentExe.Directory.FullName 'AMSim\Binaries\Win64\AMSim.exe')
        )) {
            $firewallRules += New-NetFirewallRule `
                -DisplayName "AMSim Phase0 Outbound $ruleSuffix $([IO.Path]::GetFileName($program))" `
                -Direction Outbound `
                -Program $program `
                -Action Block `
                -Profile Any
            $firewallRules += New-NetFirewallRule `
                -DisplayName "AMSim Phase0 Inbound $ruleSuffix $([IO.Path]::GetFileName($program))" `
                -Direction Inbound `
                -Program $program `
                -Action Block `
                -Profile Any
        }
    }

    $smokeProcess = Start-PackagedGame `
        -Launcher $developmentExe `
        -InnerExecutableName 'AMSim.exe' `
        -Arguments @('-windowed', '-ForceRes', '-ResX=1920', '-ResY=1080', '-nosound', '-AMSimPhase0Smoke')
    $deadline = [DateTime]::UtcNow.AddSeconds(45)
    while (-not $smokeProcess.HasExited -and [DateTime]::UtcNow -lt $deadline) {
        $networkRows += Get-NetTCPConnection -OwningProcess $smokeProcess.Id -ErrorAction SilentlyContinue |
            Select-Object State, LocalAddress, LocalPort, RemoteAddress, RemotePort
        Start-Sleep -Milliseconds 100
        $smokeProcess.Refresh()
    }
    if (-not $smokeProcess.HasExited) {
        Stop-Process -Id $smokeProcess.Id -Force
        throw 'Development package smoke test exceeded 45 seconds.'
    }
}
finally {
    foreach ($rule in $firewallRules) {
        Remove-NetFirewallRule -Name $rule.Name -ErrorAction SilentlyContinue
    }
}

$smokeResultPath = Join-Path $developmentExe.Directory.FullName 'AMSim\Saved\Phase0\smoke-result.json'
if (-not (Test-Path -LiteralPath $smokeResultPath)) {
    throw "Packaged smoke result is missing: $smokeResultPath"
}
$smokeResult = Get-Content -LiteralPath $smokeResultPath -Raw | ConvertFrom-Json
if (-not $smokeResult.passed) {
    throw "Packaged performance/save smoke failed: $(Get-Content -LiteralPath $smokeResultPath -Raw)"
}
$proofPath = Join-Path $developmentExe.Directory.FullName 'AMSim\Saved\Phase0\proof-screen.png'
if (-not (Test-Path -LiteralPath $proofPath)) {
    throw "Packaged visual proof is missing: $proofPath"
}
Add-Type -AssemblyName System.Drawing
$proofBitmap = [System.Drawing.Bitmap]::new($proofPath)
try {
    if ($proofBitmap.Width -ne 1920 -or $proofBitmap.Height -ne 1080) {
        throw "Packaged visual proof has the wrong dimensions: $($proofBitmap.Width)x$($proofBitmap.Height)"
    }
    $sampleCount = 0
    $nonBlackCount = 0
    for ($y = 0; $y -lt $proofBitmap.Height; $y += 32) {
        for ($x = 0; $x -lt $proofBitmap.Width; $x += 32) {
            $pixel = $proofBitmap.GetPixel($x, $y)
            $sampleCount++
            if ($pixel.R -gt 2 -or $pixel.G -gt 2 -or $pixel.B -gt 2) {
                $nonBlackCount++
            }
        }
    }
    $proofNonBlackRatio = $nonBlackCount / $sampleCount
    if ($proofNonBlackRatio -lt 0.5) {
        throw "Packaged visual proof is blank or nearly blank (non-black ratio $proofNonBlackRatio)."
    }
}
finally {
    $proofBitmap.Dispose()
}
$unexpectedDevelopmentTcp = @($networkRows | Where-Object {
    -not ($_.State -eq 'Listen' -and $_.LocalPort -eq 1985 -and $_.RemotePort -eq 0)
})
if ($unexpectedDevelopmentTcp.Count -ne 0) {
    throw "Development runtime opened $($unexpectedDevelopmentTcp.Count) unexpected TCP socket observations."
}

$shippingProcess = Start-PackagedGame `
    -Launcher $shippingExe `
    -InnerExecutableName 'AMSim-Win64-Shipping.exe' `
    -Arguments @('-windowed', '-ResX=1280', '-ResY=720', '-nosound')
Start-Sleep -Seconds 5
$shippingProcess.Refresh()
if ($shippingProcess.HasExited) {
    throw 'Shipping package exited unexpectedly during its five-second clean-launch check.'
}
$shippingTcp = @(Get-NetTCPConnection -OwningProcess $shippingProcess.Id -ErrorAction SilentlyContinue)
Stop-Process -Id $shippingProcess.Id -Force
if ($shippingTcp.Count -ne 0) {
    throw "Shipping runtime opened $($shippingTcp.Count) TCP sockets."
}

$forbiddenNames = @(
    'AMSimEditor',
    'AMSimTests',
    'EditorToolset',
    'ModelContextProtocol',
    'PythonScriptPlugin',
    'RemoteControl',
    'ToolsetRegistry',
    'UMGToolSet'
)
$packageFiles = Get-ChildItem -LiteralPath (Join-Path $packageRoot 'Shipping') -File -Recurse
$forbiddenFiles = @($packageFiles | Where-Object {
    $name = $_.Name
    $forbiddenNames | Where-Object { $name -like "*$_*" }
})
$textInventory = @($packageFiles | Where-Object {
    $_.Extension -in @('.ini', '.json', '.modules', '.target', '.txt', '.uplugin', '.uproject')
})
$forbiddenText = @()
foreach ($file in $textInventory) {
    $forbiddenText += Select-String `
        -LiteralPath $file.FullName `
        -Pattern $forbiddenNames `
        -SimpleMatch `
        -ErrorAction SilentlyContinue
}
if ($forbiddenFiles.Count -ne 0 -or $forbiddenText.Count -ne 0) {
    throw 'Shipping package contains an editor, MCP, Python, RemoteControl, or test dependency.'
}
$shippingReceiptPath = Join-Path $repoRoot 'AMSim\Binaries\Win64\AMSim-Win64-Shipping.target'
$forbiddenReceipt = @(
    Select-String -LiteralPath $shippingReceiptPath -Pattern $forbiddenNames -SimpleMatch -ErrorAction SilentlyContinue
)
if ($forbiddenReceipt.Count -ne 0) {
    throw 'Shipping build receipt contains an editor, MCP, Python, RemoteControl, or test dependency.'
}

$noticeInventory = @($packageFiles | Where-Object {
    $_.Name -match '(?i)(third.?party|notice|license|copyright)'
} | ForEach-Object {
    $_.FullName.Substring((Join-Path $packageRoot 'Shipping').Length).TrimStart('\')
})

$result = [ordered]@{
    schema = 1
    generatedUtc = [DateTime]::UtcNow.ToString('o')
    engine = '5.8.0'
    platform = 'Win64'
    automationLog = (Join-Path $repoRoot 'AMSim\Saved\Logs\AMSim.log')
    developmentPackage = $developmentExe.FullName
    shippingPackage = $shippingExe.FullName
    performance = $smokeResult
    proofScreen = [ordered]@{
        path = $proofPath
        nonBlackSampleRatio = $proofNonBlackRatio
    }
    firewallApplied = -not $SkipFirewall
    networkDeniedVerified = -not $SkipFirewall
    developmentTcpSocketObservations = $networkRows.Count
    developmentExpectedTraceListenerOnly = $unexpectedDevelopmentTcp.Count -eq 0
    shippingTcpSocketObservations = $shippingTcp.Count
    forbiddenShippingFiles = $forbiddenFiles.Count
    forbiddenShippingTextMatches = $forbiddenText.Count
    forbiddenShippingReceiptMatches = $forbiddenReceipt.Count
    packagedNoticeInventory = $noticeInventory
    passed = $true
}
$resultPath = Join-Path $savedRoot 'pipeline-result.json'
$result | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $resultPath -Encoding utf8
Write-Host "Phase 0 pipeline passed. Result: $resultPath"
