[CmdletBinding()]
param(
    [switch] $Json
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$repoRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..\..')).Path
$projectRoot = Join-Path $repoRoot 'AMSim'
$localAppData = [Environment]::GetFolderPath(
    [Environment+SpecialFolder]::LocalApplicationData)
$programData = [Environment]::GetFolderPath(
    [Environment+SpecialFolder]::CommonApplicationData)

function Get-TreeBytes {
    param([Parameter(Mandatory)][string] $Path)

    if (-not (Test-Path -LiteralPath $Path)) {
        return 0L
    }

    $sum = Get-ChildItem -LiteralPath $Path -File -Recurse -Force `
        -ErrorAction SilentlyContinue |
        ForEach-Object -Begin { $total = 0L } `
            -Process { $total += [int64] $_.Length } `
            -End { $total }
    if ($null -eq $sum) {
        return 0L
    }
    return [int64] $sum
}

$cacheDefinitions = @(
    @{ Scope = 'Project'; Name = 'Intermediate build'; Path = (Join-Path $projectRoot 'Intermediate\Build') }
    @{ Scope = 'Project'; Name = 'Cached asset registry'; Path = (Join-Path $projectRoot 'Intermediate\CachedAssetRegistry') }
    @{ Scope = 'Project'; Name = 'Cooked output'; Path = (Join-Path $projectRoot 'Saved\Cooked') }
    @{ Scope = 'Project'; Name = 'Staged builds'; Path = (Join-Path $projectRoot 'Saved\StagedBuilds') }
    @{ Scope = 'Project'; Name = 'Project DDC'; Path = (Join-Path $projectRoot 'DerivedDataCache') }
    @{ Scope = 'User'; Name = 'Shared DDC'; Path = (Join-Path $localAppData 'UnrealEngine\Common\DerivedDataCache') }
    @{ Scope = 'User'; Name = 'Shared Zen'; Path = (Join-Path $localAppData 'UnrealEngine\Common\Zen') }
    @{ Scope = 'User'; Name = 'UE 5.8 intermediate'; Path = (Join-Path $localAppData 'UnrealEngine\5.8\Intermediate') }
    @{ Scope = 'Machine'; Name = 'Epic Zen'; Path = (Join-Path $programData 'Epic\Zen') }
    @{ Scope = 'Machine'; Name = 'UBA'; Path = (Join-Path $programData 'Epic\UnrealBuildAccelerator') }
    @{ Scope = 'Machine'; Name = 'UBA CLI'; Path = (Join-Path $programData 'Epic\UbaCli') }
)

$caches = foreach ($definition in $cacheDefinitions) {
    $bytes = Get-TreeBytes -Path $definition.Path
    [pscustomobject]@{
        scope = $definition.Scope
        name = $definition.Name
        path = $definition.Path
        exists = Test-Path -LiteralPath $definition.Path
        bytes = $bytes
        gib = [math]::Round($bytes / 1GB, 2)
    }
}

$projectBytes = [int64](($caches | Where-Object scope -eq 'Project' |
    Measure-Object -Property bytes -Sum).Sum)
$sharedBytes = [int64](($caches | Where-Object scope -ne 'Project' |
    Measure-Object -Property bytes -Sum).Sum)
$drive = Get-PSDrive -Name C
$freeGiB = [math]::Round($drive.Free / 1GB, 2)

$status = if ($freeGiB -lt 8) {
    'Critical'
} elseif ($freeGiB -lt 15 -or (($projectBytes + $sharedBytes) / 1GB) -gt 12) {
    'Warning'
} else {
    'Healthy'
}

$report = [pscustomobject]@{
    timestamp = (Get-Date).ToString('o')
    status = $status
    freeGiB = $freeGiB
    minimumFreeGiBBeforePackaging = 15
    stopBuildsBelowFreeGiB = 8
    projectCacheGiB = [math]::Round($projectBytes / 1GB, 2)
    sharedCacheGiB = [math]::Round($sharedBytes / 1GB, 2)
    automaticCleanupAllowed = $false
    caches = @($caches)
}

if ($Json) {
    $report | ConvertTo-Json -Depth 4
    return
}

$caches |
    Sort-Object -Property @{ Expression = 'gib'; Descending = $true } |
    Format-Table scope, name, gib, exists, path -AutoSize

Write-Host ''
Write-Host ("Status: {0}" -f $report.status)
Write-Host ("C: free: {0} GiB" -f $report.freeGiB)
Write-Host ("Project caches: {0} GiB" -f $report.projectCacheGiB)
Write-Host ("Shared Unreal caches: {0} GiB" -f $report.sharedCacheGiB)
Write-Host 'This script measures only. It never deletes cache data.'
