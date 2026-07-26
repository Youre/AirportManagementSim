$ErrorActionPreference = 'Stop'

function Get-Phase1AcceptanceManifest {
    param(
        [Parameter(Mandatory)]
        [string]$Path
    )

    $fullPath = [System.IO.Path]::GetFullPath($Path)
    if (-not (Test-Path -LiteralPath $fullPath -PathType Leaf)) {
        throw "Phase 1 acceptance manifest not found: $fullPath"
    }

    $manifest = Get-Content -LiteralPath $fullPath -Raw | ConvertFrom-Json
    if (
        $manifest.schema -ne 1 -or
        $manifest.phase -ne 1 -or
        $manifest.engine -ne '5.8.0' -or
        $manifest.platform -ne 'Win64' -or
        $manifest.packageSourceCommit -notmatch '^[0-9a-fA-F]{40}$'
    ) {
        throw "Invalid Phase 1 acceptance manifest header: $fullPath"
    }

    $requiredPackages = @(
        'developmentLauncher',
        'developmentRuntime',
        'shippingLauncher',
        'shippingRuntime'
    )
    foreach ($name in $requiredPackages) {
        $entry = $manifest.packages.$name
        if (
            -not $entry -or
            [string]::IsNullOrWhiteSpace($entry.relativePath) -or
            $entry.sha256 -notmatch '^[0-9a-fA-F]{64}$'
        ) {
            throw "Invalid package entry '$name' in $fullPath"
        }
    }

    return $manifest
}

function Resolve-Phase1PackagePath {
    param(
        [Parameter(Mandatory)]
        [string]$PackageRoot,
        [Parameter(Mandatory)]
        [string]$RelativePath
    )

    $root = [System.IO.Path]::GetFullPath($PackageRoot)
    $candidate = [System.IO.Path]::GetFullPath(
        (Join-Path $root ($RelativePath -replace '/', [System.IO.Path]::DirectorySeparatorChar)))
    $rootPrefix = $root.TrimEnd(
        [System.IO.Path]::DirectorySeparatorChar,
        [System.IO.Path]::AltDirectorySeparatorChar
    ) + [System.IO.Path]::DirectorySeparatorChar
    if (-not $candidate.StartsWith($rootPrefix, [StringComparison]::OrdinalIgnoreCase)) {
        throw "Package manifest path escapes the package root: $RelativePath"
    }
    return $candidate
}

function Get-Phase1PackageIdentity {
    param(
        [Parameter(Mandatory)]
        [string]$PackageRoot,
        [Parameter(Mandatory)]
        [object]$Manifest
    )

    $root = [System.IO.Path]::GetFullPath($PackageRoot)
    $packages = [ordered]@{}
    $passed = $true
    foreach ($property in $Manifest.packages.PSObject.Properties) {
        $expected = $property.Value
        $path = Resolve-Phase1PackagePath `
            -PackageRoot $root `
            -RelativePath $expected.relativePath
        $exists = Test-Path -LiteralPath $path -PathType Leaf
        $actualHash = if ($exists) {
            (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToUpperInvariant()
        } else {
            ''
        }
        $expectedHash = $expected.sha256.ToUpperInvariant()
        $matchesExpected = $exists -and $actualHash -ceq $expectedHash
        if (-not $matchesExpected) {
            $passed = $false
        }
        $packages[$property.Name] = [ordered]@{
            path = $path
            exists = $exists
            expectedSha256 = $expectedHash
            actualSha256 = $actualHash
            matchesExpected = $matchesExpected
        }
    }

    return [ordered]@{
        packageRoot = $root
        packageSourceCommit = $Manifest.packageSourceCommit
        packages = $packages
        passed = $passed
    }
}

function Write-Phase1Json {
    param(
        [Parameter(Mandatory)]
        [object]$Value,
        [Parameter(Mandatory)]
        [string]$Path
    )

    $fullPath = [System.IO.Path]::GetFullPath($Path)
    $directory = Split-Path -Parent $fullPath
    New-Item -ItemType Directory -Path $directory -Force | Out-Null
    $Value | ConvertTo-Json -Depth 12 | Set-Content -LiteralPath $fullPath -Encoding utf8
    return $fullPath
}
