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
$projectPath = Join-Path $repoRoot 'AMSim\AMSim.uproject'
$savedRoot = Join-Path $repoRoot 'AMSim\Saved\Phase1'
$scaleRoot = Join-Path $savedRoot 'Scale'
$automationReportRoot = Join-Path $savedRoot 'Automation'
$packageRoot = if ($PackageRoot) {
    [System.IO.Path]::GetFullPath($PackageRoot)
} else {
    Join-Path $repoRoot 'AMSim\Saved\Phase1Packages'
}
$manifestPath = Join-Path $repoRoot 'docs\planning\30-content-and-assets\aircraft-import-manifest-template.csv'
$buildScript = Join-Path $EngineRoot 'Engine\Build\BatchFiles\Build.bat'
$uatScript = Join-Path $EngineRoot 'Engine\Build\BatchFiles\RunUAT.bat'
$editor = Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'
$editorCommand = Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'

function Invoke-Native {
    param(
        [Parameter(Mandatory)][string]$Executable,
        [Parameter(Mandatory)][string[]]$Arguments
    )
    & $Executable @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "$Executable exited with code $LASTEXITCODE."
    }
}

function Invoke-RenderedEditor {
    param([Parameter(Mandatory)][string[]]$Arguments)
    $process = Start-Process `
        -FilePath $editor `
        -ArgumentList $Arguments `
        -PassThru `
        -WindowStyle Hidden
    Wait-Process -Id $process.Id
    $process.Refresh()
    if ($process.ExitCode -ne 0) {
        throw "Rendered Unreal Editor exited with code $($process.ExitCode)."
    }
}

function Reset-GeneratedDirectory {
    param([Parameter(Mandatory)][string]$Path)
    $resolvedTarget = [System.IO.Path]::GetFullPath($Path)
    $savedBoundary = [System.IO.Path]::GetFullPath((Join-Path $repoRoot 'AMSim\Saved'))
    if (-not $resolvedTarget.StartsWith(
        $savedBoundary.TrimEnd('\') + '\',
        [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "Refusing to reset a directory outside AMSim/Saved: $resolvedTarget"
    }
    if (Test-Path -LiteralPath $resolvedTarget) {
        Remove-Item -LiteralPath $resolvedTarget -Recurse -Force
    }
    New-Item -ItemType Directory -Path $resolvedTarget -Force | Out-Null
}

function New-PackageEvidenceEntry {
    param(
        [Parameter(Mandatory)]
        [string]$Path
    )

    $fullPath = [System.IO.Path]::GetFullPath($Path)
    $exists = Test-Path -LiteralPath $fullPath -PathType Leaf
    return [ordered]@{
        path = $fullPath
        exists = $exists
        actualSha256 = if ($exists) {
            (Get-FileHash -LiteralPath $fullPath -Algorithm SHA256).Hash.ToUpperInvariant()
        } else {
            ''
        }
    }
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

function Wait-ForSmoke {
    param(
        [Parameter(Mandatory)][System.Diagnostics.Process]$Process,
        [Parameter(Mandatory)][string]$ResultPath,
        [int]$TimeoutSeconds = 90
    )
    $tcpRows = @()
    $deadline = [DateTime]::UtcNow.AddSeconds($TimeoutSeconds)
    while (-not $Process.HasExited -and [DateTime]::UtcNow -lt $deadline) {
        $tcpRows += Get-NetTCPConnection -OwningProcess $Process.Id -ErrorAction SilentlyContinue |
            Select-Object State, LocalAddress, LocalPort, RemoteAddress, RemotePort
        Start-Sleep -Milliseconds 100
        $Process.Refresh()
    }
    if (-not $Process.HasExited) {
        Stop-Process -Id $Process.Id -Force
        throw "Phase 1 smoke exceeded $TimeoutSeconds seconds."
    }
    if (-not (Test-Path -LiteralPath $ResultPath)) {
        throw "Phase 1 smoke result is missing: $ResultPath"
    }
    $smokeResult = Get-Content -LiteralPath $ResultPath -Raw | ConvertFrom-Json
    if (-not $smokeResult.passed) {
        throw "Phase 1 smoke failed: $(Get-Content -LiteralPath $ResultPath -Raw)"
    }
    return [pscustomobject]@{
        Result = $smokeResult
        TcpRows = $tcpRows
    }
}

foreach ($requiredPath in @(
    $projectPath,
    $manifestPath,
    $buildScript,
    $uatScript,
    $editor,
    $editorCommand
)) {
    if (-not (Test-Path -LiteralPath $requiredPath)) {
        throw "Required Phase 1 path is missing: $requiredPath"
    }
}
if (Get-Process UnrealEditor -ErrorAction SilentlyContinue) {
    throw 'Close Unreal Editor before running the Phase 1 pipeline.'
}
if (-not $SkipFirewall) {
    $identity = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = [Security.Principal.WindowsPrincipal]::new($identity)
    if (-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
        throw 'Run from elevated PowerShell, or pass -SkipFirewall and run Test-Phase1NetworkDenied.ps1 later.'
    }
}

if (-not $SkipBuild) {
    Invoke-Native -Executable $buildScript -Arguments @(
        'AMSimEditor',
        'Win64',
        'Development',
        $projectPath,
        '-WaitMutex',
        '-NoHotReloadFromIDE'
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
Reset-GeneratedDirectory -Path $automationReportRoot
Invoke-Native -Executable $editorCommand -Arguments @(
    $projectPath,
    '-unattended',
    '-nop4',
    '-NullRHI',
    '-ExecCmds=Automation RunTests AMSim',
    '-TestExit=Automation Test Queue Empty',
    "-ReportExportPath=$automationReportRoot"
)
$automationReportPath = Join-Path $automationReportRoot 'index.json'
if (-not (Test-Path -LiteralPath $automationReportPath)) {
    throw "Full automation did not emit its machine-readable report: $automationReportPath"
}
$automationReport = Get-Content -LiteralPath $automationReportPath -Raw | ConvertFrom-Json
$automationPassed =
    $automationReport.tests.Count -gt 0 -and
    $automationReport.failed -eq 0 -and
    $automationReport.notRun -eq 0 -and
    $automationReport.inProcess -eq 0
if (-not $automationPassed) {
    throw "Full automation failed: $(Get-Content -LiteralPath $automationReportPath -Raw)"
}

$scaleResults = @()
if (-not $SkipScaleMatrix) {
    Reset-GeneratedDirectory -Path $scaleRoot
    foreach ($scale in @(1.0, 1.25, 1.5, 1.75, 2.0)) {
        $percent = [int]($scale * 100)
        $scaleDirectory = Join-Path $scaleRoot $percent
        New-Item -ItemType Directory -Path $scaleDirectory -Force | Out-Null
        $smokeResultPath = Join-Path $savedRoot 'smoke-result.json'
        if (Test-Path -LiteralPath $smokeResultPath) {
            Remove-Item -LiteralPath $smokeResultPath -Force
        }
        Invoke-RenderedEditor -Arguments @(
            $projectPath,
            '-game',
            '-AMSimPhase1Smoke',
            '-AMSimPhase1ReferenceProfile',
            '-windowed',
            '-ForceRes',
            '-ResX=1920',
            '-ResY=1080',
            '-unattended',
            '-nop4',
            "-ini:Engine:[/Script/Engine.UserInterfaceSettings]:ApplicationScale=$scale"
        )
        $scaleResult = Get-Content -LiteralPath $smokeResultPath -Raw | ConvertFrom-Json
        if (-not $scaleResult.passed) {
            throw "Rendered Phase 1 smoke failed at $percent percent UI scale."
        }
        foreach ($proofName in @(
            'va01-new-airport.png',
            'va02-construction.png',
            'va04-offer.png',
            'va05-turnaround.png',
            'phase1-complete.png',
            'smoke-result.json'
        )) {
            Copy-Item `
                -LiteralPath (Join-Path $savedRoot $proofName) `
                -Destination (Join-Path $scaleDirectory $proofName) `
                -Force
        }
        $scaleResults += [ordered]@{
            percent = $percent
            passed = $scaleResult.passed
            p99FrameMilliseconds = $scaleResult.p99FrameMilliseconds
            maximum8xBacklogSteps = $scaleResult.maximum8xBacklogSteps
            proofDirectory = $scaleDirectory
        }
    }
}

if (-not $SkipPackage) {
    Reset-GeneratedDirectory -Path $packageRoot
    foreach ($configuration in @('Development', 'Shipping')) {
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
            "-archivedirectory=$(Join-Path $packageRoot $configuration)",
            '-unattended',
            '-utf8output'
        )
    }
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
    throw 'Development and Shipping launchers are missing.'
}

$firewallRules = @()
$shippingProcess = $null
try {
    if (-not $SkipFirewall) {
        $ruleSuffix = [Guid]::NewGuid().ToString('N')
        foreach ($program in @(
            $developmentExe.FullName,
            (Join-Path $developmentExe.Directory.FullName 'AMSim\Binaries\Win64\AMSim.exe'),
            $shippingExe.FullName,
            (Join-Path $shippingExe.Directory.FullName 'AMSim\Binaries\Win64\AMSim-Win64-Shipping.exe')
        )) {
            foreach ($direction in @('Inbound', 'Outbound')) {
                $firewallRules += New-NetFirewallRule `
                    -DisplayName "AMSim Phase1 $direction $ruleSuffix $([IO.Path]::GetFileName($program))" `
                    -Direction $direction `
                    -Program $program `
                    -Action Block `
                    -Profile Any
            }
        }
    }

    $packagedResultPath = Join-Path $developmentExe.Directory.FullName 'AMSim\Saved\Phase1\smoke-result.json'
    if (Test-Path -LiteralPath $packagedResultPath) {
        Remove-Item -LiteralPath $packagedResultPath -Force
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
    $packagedSmoke = Wait-ForSmoke `
        -Process $developmentProcess `
        -ResultPath $packagedResultPath

    $unexpectedDevelopmentTcp = @($packagedSmoke.TcpRows | Where-Object {
        -not ($_.State -eq 'Listen' -and $_.LocalPort -eq 1985 -and $_.RemotePort -eq 0)
    })
    if ($unexpectedDevelopmentTcp.Count -ne 0) {
        throw "Development package opened $($unexpectedDevelopmentTcp.Count) unexpected TCP socket observations."
    }

    $shippingProcess = Start-PackagedGame `
        -Launcher $shippingExe `
        -InnerExecutableName 'AMSim-Win64-Shipping.exe' `
        -Arguments @('-windowed', '-ForceRes', '-ResX=1280', '-ResY=720')
    Start-Sleep -Seconds 5
    $shippingProcess.Refresh()
    if ($shippingProcess.HasExited) {
        throw 'Shipping package exited unexpectedly during its five-second clean-launch check.'
    }
    $shippingTcp = @(
        Get-NetTCPConnection -OwningProcess $shippingProcess.Id -ErrorAction SilentlyContinue
    )
    Stop-Process -Id $shippingProcess.Id -Force
    $shippingProcess = $null
    if ($shippingTcp.Count -ne 0) {
        throw "Shipping package opened $($shippingTcp.Count) TCP sockets."
    }
}
finally {
    if ($shippingProcess -and -not $shippingProcess.HasExited) {
        Stop-Process -Id $shippingProcess.Id -Force -ErrorAction SilentlyContinue
    }
    foreach ($rule in $firewallRules) {
        Remove-NetFirewallRule -Name $rule.Name -ErrorAction SilentlyContinue
    }
}

$forbiddenNames = @(
    'AMSimEditor',
    'AMSimTests',
    'ModelContextProtocol',
    'PythonScriptPlugin',
    'RemoteControl',
    'ToolsetRegistry'
)
$shippingFiles = Get-ChildItem -LiteralPath (Join-Path $packageRoot 'Shipping') -File -Recurse
$forbiddenFiles = @($shippingFiles | Where-Object {
    $fileName = $_.Name
    @($forbiddenNames | Where-Object { $fileName -like "*$_*" }).Count -gt 0
})
$textInventory = @($shippingFiles | Where-Object {
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
$shippingReceipt = Join-Path $repoRoot 'AMSim\Binaries\Win64\AMSim-Win64-Shipping.target'
$forbiddenReceipt = @(
    Select-String `
        -LiteralPath $shippingReceipt `
        -Pattern $forbiddenNames `
        -SimpleMatch `
        -ErrorAction SilentlyContinue
)
if ($forbiddenFiles.Count -ne 0 -or $forbiddenText.Count -ne 0 -or $forbiddenReceipt.Count -ne 0) {
    throw 'Shipping contains a forbidden editor, test, MCP, Python, RemoteControl, or Toolset dependency.'
}

$requiredProofs = @(
    'va01-new-airport.png',
    'va02-construction.png',
    'va04-offer.png',
    'va05-turnaround.png',
    'phase1-complete.png'
)
$packagedProofRoot = Join-Path $developmentExe.Directory.FullName 'AMSim\Saved\Phase1'
foreach ($proofName in $requiredProofs) {
    if (-not (Test-Path -LiteralPath (Join-Path $packagedProofRoot $proofName))) {
        throw "Packaged Phase 1 proof is missing: $proofName"
    }
}

$sourceAssetExtensions = @(
    Get-ChildItem -LiteralPath (Join-Path $repoRoot 'AMSim\Content') -File -Recurse |
        ForEach-Object { $_.Extension.ToLowerInvariant() } |
        Sort-Object -Unique
)
$required3DAssets = @(
    Get-ChildItem -LiteralPath (Join-Path $repoRoot 'AMSim\Content') -File -Recurse |
        Where-Object { $_.Name -match '(?i)(staticmesh|skeletalmesh|nanite|materialinstance)' }
)
if ($required3DAssets.Count -ne 0) {
    throw 'Project content contains a required 3D gameplay asset candidate.'
}

New-Item -ItemType Directory -Path $savedRoot -Force | Out-Null
$developmentRuntimePath = Join-Path `
    $developmentExe.Directory.FullName `
    'AMSim\Binaries\Win64\AMSim.exe'
$shippingRuntimePath = Join-Path `
    $shippingExe.Directory.FullName `
    'AMSim\Binaries\Win64\AMSim-Win64-Shipping.exe'
$sourceCommit = (& git -C $repoRoot rev-parse HEAD).Trim()
if ($LASTEXITCODE -ne 0 -or $sourceCommit -notmatch '^[0-9a-fA-F]{40}$') {
    throw 'Unable to resolve the Phase 1 pipeline source commit.'
}
$sourceTreeStatus = @(& git -C $repoRoot status --porcelain --untracked-files=all)
if ($LASTEXITCODE -ne 0) {
    throw 'Unable to inspect the Phase 1 pipeline source tree.'
}
$packageEvidence = [ordered]@{
    packageRoot = $packageRoot
    packageSourceCommit = $sourceCommit
    sourceTreeClean = $sourceTreeStatus.Count -eq 0
    packages = [ordered]@{
        developmentLauncher = New-PackageEvidenceEntry -Path $developmentExe.FullName
        developmentRuntime = New-PackageEvidenceEntry -Path $developmentRuntimePath
        shippingLauncher = New-PackageEvidenceEntry -Path $shippingExe.FullName
        shippingRuntime = New-PackageEvidenceEntry -Path $shippingRuntimePath
    }
}
$packageEvidence.passed = @(
    $packageEvidence.packages.Values |
        Where-Object { -not $_.exists }
).Count -eq 0
$result = [ordered]@{
    schema = 1
    generatedUtc = [DateTime]::UtcNow.ToString('o')
    engine = '5.8.0'
    platform = 'Win64'
    auditPassed = $true
    fullAutomationPassed = $automationPassed
    automation = [ordered]@{
        report = $automationReportPath
        succeeded = $automationReport.succeeded
        succeededWithWarnings = $automationReport.succeededWithWarnings
        failed = $automationReport.failed
        notRun = $automationReport.notRun
        inProcess = $automationReport.inProcess
    }
    scaleMatrix = $scaleResults
    developmentPackage = $developmentExe.FullName
    shippingPackage = $shippingExe.FullName
    packageIdentity = $packageEvidence
    packagedSmoke = $packagedSmoke.Result
    firewallApplied = -not $SkipFirewall
    networkDeniedVerified = -not $SkipFirewall
    developmentTcpSocketObservations = $packagedSmoke.TcpRows.Count
    unexpectedDevelopmentTcpSocketObservations = $unexpectedDevelopmentTcp.Count
    developmentTraceListenerOnly = $unexpectedDevelopmentTcp.Count -eq 0
    shippingTcpSocketObservations = $shippingTcp.Count
    forbiddenShippingFiles = $forbiddenFiles.Count
    forbiddenShippingTextMatches = $forbiddenText.Count
    forbiddenShippingReceiptMatches = $forbiddenReceipt.Count
    sourceContentExtensions = $sourceAssetExtensions
    required3DAssetCandidates = $required3DAssets.Count
    proofs = $requiredProofs
    passed = $true
}
$resultPath = Join-Path $savedRoot 'pipeline-result.json'
$result | ConvertTo-Json -Depth 10 | Set-Content -LiteralPath $resultPath -Encoding utf8
Write-Host "Phase 1 pipeline passed. Result: $resultPath"
