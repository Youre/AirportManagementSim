[CmdletBinding()]
param(
    [string]$EngineRoot = 'C:\Program Files\Epic Games\UE_5.8',
    [string]$PackageRoot = '',
    [switch]$SkipBuild,
    [switch]$SkipAutomation,
    [switch]$SkipRenderedFixtures,
    [switch]$SkipScaleMatrix,
    [switch]$SkipPackage
)

$ErrorActionPreference = 'Stop'
$repoRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..'))
$project = Join-Path $repoRoot 'AMSim\AMSim.uproject'
$savedRoot = Join-Path $repoRoot 'AMSim\Saved\Phase7'
$packageRoot = if ($PackageRoot) {
    [IO.Path]::GetFullPath($PackageRoot)
} else {
    Join-Path $repoRoot 'AMSim\Saved\Phase7Packages'
}
$build = Join-Path $EngineRoot 'Engine\Build\BatchFiles\Build.bat'
$uat = Join-Path $EngineRoot 'Engine\Build\BatchFiles\RunUAT.bat'
$editor = Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'
$editorCommand = Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
$zen = Join-Path $EngineRoot 'Engine\Binaries\Win64\zen.exe'

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

function Reset-SavedDirectory {
    param([Parameter(Mandatory)][string]$Path)
    $target = [IO.Path]::GetFullPath($Path)
    $boundary = [IO.Path]::GetFullPath((Join-Path $repoRoot 'AMSim\Saved'))
    if (-not $target.StartsWith(
        $boundary.TrimEnd('\') + '\',
        [StringComparison]::OrdinalIgnoreCase)) {
        throw "Refusing to reset a directory outside AMSim/Saved: $target"
    }
    if (Test-Path -LiteralPath $target) {
        Remove-Item -LiteralPath $target -Recurse -Force
    }
    New-Item -ItemType Directory -Path $target -Force | Out-Null
}

foreach ($required in @($project, $build, $uat, $editor, $editorCommand)) {
    if (-not (Test-Path -LiteralPath $required -PathType Leaf)) {
        throw "Required Phase 7 pipeline path is missing: $required"
    }
}
if (Get-Process UnrealEditor -ErrorAction SilentlyContinue) {
    throw 'Close Unreal Editor before running the Phase 7 pipeline.'
}
New-Item -ItemType Directory -Path $savedRoot -Force | Out-Null

if (-not $SkipBuild) {
    Invoke-Native -Executable $build -Arguments @(
        'AMSimEditor',
        'Win64',
        'Development',
        $project,
        '-WaitMutex',
        '-NoHotReloadFromIDE'
    )
}

$automationSummary = $null
if (-not $SkipAutomation) {
    Invoke-Native -Executable $editorCommand -Arguments @(
        $project,
        '-run=AMSimProjectAudit',
        '-unattended',
        '-nop4',
        '-NullRHI',
        "-Manifest=$(Join-Path $repoRoot 'docs\planning\30-content-and-assets\aircraft-import-manifest-template.csv')"
    )
    $automationRoot = Join-Path $savedRoot 'Automation'
    Reset-SavedDirectory -Path $automationRoot
    Invoke-Native -Executable $editorCommand -Arguments @(
        $project,
        '-unattended',
        '-nop4',
        '-NullRHI',
        '-ExecCmds=Automation RunTests AMSim',
        '-TestExit=Automation Test Queue Empty',
        "-ReportExportPath=$automationRoot"
    )
    $automationPath = Join-Path $automationRoot 'index.json'
    $automationSummary = Get-Content -LiteralPath $automationPath -Raw |
        ConvertFrom-Json
    if (
        $automationSummary.tests.Count -eq 0 -or
        $automationSummary.failed -ne 0 -or
        $automationSummary.notRun -ne 0 -or
        $automationSummary.inProcess -ne 0
    ) {
        throw "Full Phase 7 automation failed: $automationPath"
    }
}

if (-not $SkipRenderedFixtures) {
    foreach ($fixture in @(
        [pscustomobject]@{
            Name = 'Phase1'
            Arguments = @(
                '-AMSimPhase1Smoke',
                '-AMSimPhase1ReferenceProfile'
            )
            Result = Join-Path $repoRoot 'AMSim\Saved\Phase1\smoke-result.json'
        },
        [pscustomobject]@{
            Name = 'Phase4'
            Arguments = @(
                '-AMSimPhase4Smoke',
                '-AMSimPhase1ReferenceProfile'
            )
            Result = Join-Path $repoRoot 'AMSim\Saved\Phase4\smoke-result.json'
        }
    )) {
        $renderedArguments = @(
            $project
            '-game'
        ) + @($fixture.Arguments) + @(
            '-AMSimIgnoreUserProfile'
            '-windowed'
            '-ForceRes'
            '-ResX=1920'
            '-ResY=1080'
            '-unattended'
            '-nop4'
            '-NoSound'
        )
        $process = Start-Process `
            -FilePath $editor `
            -ArgumentList $renderedArguments `
            -WindowStyle Hidden `
            -PassThru `
            -Wait
        if ($process.ExitCode -ne 0) {
            throw "$($fixture.Name) rendered fixture exited with code $($process.ExitCode)."
        }
        $fixtureResult = Get-Content -LiteralPath $fixture.Result -Raw |
            ConvertFrom-Json
        if (-not $fixtureResult.passed) {
            throw "$($fixture.Name) rendered fixture failed."
        }
    }
}

if (-not $SkipScaleMatrix) {
    & (Join-Path $PSScriptRoot 'Invoke-Phase7ScaleMatrix.ps1') `
        -EngineRoot $EngineRoot
}

if (-not $SkipPackage) {
    Reset-SavedDirectory -Path $packageRoot
    foreach ($configuration in @('Development', 'Shipping')) {
        Invoke-Native -Executable $zen -Arguments @('up')
        Invoke-Native -Executable $uat -Arguments @(
            'BuildCookRun',
            "-project=$project",
            '-noP4',
            '-platform=Win64',
            "-clientconfig=$configuration",
            '-clean',
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
    & (Join-Path $PSScriptRoot 'Test-Phase7PackagedRuntime.ps1') `
        -PackageRoot $packageRoot
}

& (Join-Path $PSScriptRoot 'Test-Phase7ReleaseAudit.ps1') `
    -EngineRoot $EngineRoot `
    -PackageRoot $(if ($SkipPackage) { '' } else { $packageRoot })
if (-not $SkipPackage) {
    & (Join-Path $PSScriptRoot 'New-Phase7ReleaseAttestation.ps1') `
        -PackageRoot $packageRoot
}

$result = [ordered]@{
    schema = 1
    generatedUtc = [DateTime]::UtcNow.ToString('o')
    phase = 'Phase7'
    engine = '5.8.0'
    sourceCommit = (& git -C $repoRoot rev-parse HEAD).Trim()
    automation = if ($SkipAutomation) {
        'skipped'
    } else {
        [ordered]@{
            succeeded = $automationSummary.succeeded
            succeededWithWarnings = $automationSummary.succeededWithWarnings
            failed = $automationSummary.failed
            notRun = $automationSummary.notRun
        }
    }
    renderedFixtures = if ($SkipRenderedFixtures) { 'skipped' } else { 'passed' }
    scaleMatrix = if ($SkipScaleMatrix) { 'skipped' } else { 'passed' }
    package = if ($SkipPackage) { 'skipped' } else { $packageRoot }
    packagedRuntime = if ($SkipPackage) {
        'skipped'
    } else {
        Join-Path $savedRoot 'packaged-runtime.json'
    }
    releaseAttestation = if ($SkipPackage) {
        'skipped'
    } else {
        Join-Path $savedRoot 'release-attestation.json'
    }
    releaseAudit = Join-Path $savedRoot 'release-audit.json'
    ownerAcceptance = 'pending'
    status = 'release-candidate-ready-for-owner-acceptance'
    passed = $true
}
$resultPath = Join-Path $savedRoot 'pipeline-result.json'
$result | ConvertTo-Json -Depth 7 |
    Set-Content -LiteralPath $resultPath -Encoding utf8
Write-Host "Phase 7 pipeline passed. Result: $resultPath"
