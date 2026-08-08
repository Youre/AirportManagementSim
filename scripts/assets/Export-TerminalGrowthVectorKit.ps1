param(
    [string]$SourceSvg = "SourceAssets/TerminalGrowth/Vector/terminal-modular-kit-contact-sheet.svg",
    [string]$OutputRoot = "SourceAssets/TerminalGrowth/Runtime"
)

$ErrorActionPreference = "Stop"
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "../..")).Path
$sourceAbsolute = [System.IO.Path]::GetFullPath((Join-Path $repoRoot $SourceSvg))
$outputAbsolute = [System.IO.Path]::GetFullPath((Join-Path $repoRoot $OutputRoot))
$vectorDirectory = Join-Path $outputAbsolute "Vector"
$textureDirectory = Join-Path $outputAbsolute "Textures"
[System.IO.Directory]::CreateDirectory($vectorDirectory) | Out-Null
[System.IO.Directory]::CreateDirectory($textureDirectory) | Out-Null

function Get-RepositoryRelativePath([string]$Path) {
    $absolutePath = [System.IO.Path]::GetFullPath($Path)
    if (-not $absolutePath.StartsWith($repoRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "Path is outside the repository: $absolutePath"
    }
    return $absolutePath.Substring($repoRoot.Length).TrimStart("\") -replace "\\", "/"
}

$edgeCandidates = @(
    "C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe",
    "C:\Program Files\Microsoft\Edge\Application\msedge.exe"
)
$edge = $edgeCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1
if (-not $edge) { throw "Microsoft Edge is required to rasterize the reviewed SVG assets." }

$sourceText = [System.IO.File]::ReadAllText($sourceAbsolute)
$defsMatch = [regex]::Match($sourceText, "(?s)<defs>(.*?)</defs>")
if (-not $defsMatch.Success) { throw "The source SVG does not contain a defs section." }
$defs = $defsMatch.Groups[1].Value

$assets = @(
    @{ Name="PublicFloor"; Category="Surface"; Pattern="public-floor" },
    @{ Name="ServiceFloor"; Category="Surface"; Pattern="service-floor" },
    @{ Name="RoofSurface"; Category="Surface"; Pattern="roof-surface" },
    @{ Name="ExteriorWall"; Category="Structure"; Symbol="wall-exterior" },
    @{ Name="InteriorWall"; Category="Structure"; Symbol="wall-interior" },
    @{ Name="GlassWall"; Category="Structure"; Symbol="wall-glass" },
    @{ Name="EntranceDoor"; Category="Door"; Symbol="door-entrance" },
    @{ Name="StandardDoor"; Category="Door"; Symbol="door-standard" },
    @{ Name="ServiceDoor"; Category="Door"; Symbol="door-service" },
    @{ Name="AirsideGateDoor"; Category="Door"; Symbol="door-gate" },
    @{ Name="RoofEdge"; Category="Roof"; Symbol="roof-edge" },
    @{ Name="RoofCorner"; Category="Roof"; Symbol="roof-corner" },
    @{ Name="EntranceCanopy"; Category="Roof"; Symbol="roof-canopy" },
    @{ Name="RoofHVAC"; Category="Roof"; Symbol="roof-hvac" },
    @{ Name="ConstructionFoundation"; Category="Construction"; Symbol="construction-foundation" },
    @{ Name="ConstructionDelivery"; Category="Construction"; Symbol="construction-delivery" },
    @{ Name="ConstructionFraming"; Category="Construction"; Symbol="construction-framing" },
    @{ Name="ConstructionActive"; Category="Construction"; Symbol="construction-active" },
    @{ Name="ConstructionInspection"; Category="Construction"; Symbol="construction-inspection" },
    @{ Name="ConstructionClosure"; Category="Construction"; Symbol="construction-closure" },
    @{ Name="StaffDesk"; Category="Furniture"; Body='<rect x="38" y="78" width="180" height="92" rx="18" fill="#8b694a" stroke="#263743" stroke-width="8"/><rect x="62" y="98" width="64" height="42" rx="6" fill="#163f55"/><rect x="70" y="106" width="48" height="26" rx="3" fill="#78d8e8"/><circle cx="168" cy="124" r="26" fill="#2b5670" stroke="#263743" stroke-width="7"/><rect x="148" y="166" width="40" height="22" rx="7" fill="#2b5670"/>' },
    @{ Name="Storage"; Category="Furniture"; Body='<rect x="34" y="54" width="188" height="148" rx="14" fill="#596a70" stroke="#263743" stroke-width="8"/><path d="M42 102h172M42 152h172M96 62v132M160 62v132" stroke="#d5d0bc" stroke-width="7"/><rect x="52" y="70" width="32" height="22" fill="#b68743"/><rect x="109" y="112" width="38" height="30" fill="#c79a53"/><rect x="170" y="162" width="31" height="26" fill="#a87435"/>' },
    @{ Name="Vending"; Category="Furniture"; Body='<rect x="54" y="36" width="148" height="184" rx="20" fill="#1f6d86" stroke="#263743" stroke-width="9"/><rect x="74" y="58" width="108" height="100" rx="8" fill="#d7eef1"/><g fill="#e3a93d"><circle cx="96" cy="86" r="10"/><circle cx="128" cy="86" r="10"/><circle cx="160" cy="86" r="10"/></g><g fill="#6fb55f"><circle cx="96" cy="124" r="10"/><circle cx="128" cy="124" r="10"/><circle cx="160" cy="124" r="10"/></g><rect x="82" y="178" width="92" height="18" rx="5" fill="#0a3042"/>' },
    @{ Name="Water"; Category="Furniture"; Body='<circle cx="128" cy="130" r="62" fill="#d9f4f6" stroke="#28586b" stroke-width="9"/><circle cx="128" cy="130" r="34" fill="#6fd4e5"/><path d="M108 63h40l10 30h-60z" fill="#dce7e5" stroke="#28586b" stroke-width="7"/><circle cx="128" cy="126" r="10" fill="#ffffff"/>' },
    @{ Name="WasteRecycling"; Category="Furniture"; Body='<rect x="36" y="64" width="82" height="132" rx="18" fill="#405059" stroke="#263743" stroke-width="8"/><rect x="138" y="64" width="82" height="132" rx="18" fill="#287b78" stroke="#263743" stroke-width="8"/><path d="M57 91h40M159 91h40" stroke="#dce7e5" stroke-width="10"/><path d="M168 125l16-13 14 16-10 4 7 15-12 8-8-17-11 5z" fill="#dce7e5"/>' },
    @{ Name="Noticeboard"; Category="Furniture"; Body='<rect x="28" y="58" width="200" height="140" rx="14" fill="#855e3c" stroke="#263743" stroke-width="9"/><rect x="48" y="78" width="160" height="100" rx="5" fill="#d7c79d"/><rect x="66" y="94" width="55" height="30" fill="#78d8e8"/><rect x="135" y="94" width="54" height="15" fill="#ffffff"/><rect x="135" y="119" width="54" height="42" fill="#f0c65e"/>' },
    @{ Name="Signage"; Category="Furniture"; Body='<rect x="48" y="60" width="160" height="94" rx="16" fill="#0f4f68" stroke="#263743" stroke-width="9"/><path d="M82 108h82m0 0-25-23m25 23-25 23" fill="none" stroke="#85e0ed" stroke-width="13" stroke-linecap="round" stroke-linejoin="round"/><rect x="112" y="154" width="32" height="48" fill="#263743"/><rect x="76" y="198" width="104" height="14" rx="7" fill="#263743"/>' },
    @{ Name="Plant"; Category="Furniture"; Body='<ellipse cx="128" cy="174" rx="58" ry="38" fill="#9a6336" stroke="#263743" stroke-width="8"/><g fill="#4c9a55" stroke="#2e6841" stroke-width="5"><circle cx="128" cy="96" r="40"/><circle cx="88" cy="112" r="34"/><circle cx="169" cy="116" r="35"/><circle cx="111" cy="139" r="32"/><circle cx="151" cy="145" r="30"/></g><circle cx="128" cy="128" r="19" fill="#78b85d"/>' },
    @{ Name="PlacementValid"; Category="Overlay"; Symbol="overlay-valid" },
    @{ Name="PlacementBlocked"; Category="Overlay"; Symbol="overlay-blocked" },
    @{ Name="PlacementUnaffordable"; Category="Overlay"; Symbol="overlay-unaffordable" },
    @{ Name="PlacementRouteLoss"; Category="Overlay"; Symbol="overlay-route" },
    @{ Name="PlacementSnap"; Category="Overlay"; Symbol="overlay-snap" },
    @{ Name="PlacementConnection"; Category="Overlay"; Symbol="overlay-connection" },
    @{ Name="IconStructure"; Category="Icon"; Symbol="icon-structure-symbol"; Square=$true },
    @{ Name="IconDoors"; Category="Icon"; Symbol="icon-doors-symbol"; Square=$true },
    @{ Name="IconFurniture"; Category="Icon"; Symbol="icon-furniture-symbol"; Square=$true },
    @{ Name="IconAmenities"; Category="Icon"; Symbol="icon-amenities-symbol"; Square=$true },
    @{ Name="IconOperations"; Category="Icon"; Symbol="icon-operations-symbol"; Square=$true },
    @{ Name="IconZones"; Category="Icon"; Symbol="icon-zones-symbol"; Square=$true },
    @{ Name="IconDemolish"; Category="Icon"; Symbol="icon-demolish-symbol"; Square=$true },
    @{ Name="IconRotate"; Category="Icon"; Symbol="icon-rotate-symbol"; Square=$true },
    @{ Name="IconCopy"; Category="Icon"; Symbol="icon-copy-symbol"; Square=$true },
    @{ Name="IconUndo"; Category="Icon"; Symbol="icon-undo-symbol"; Square=$true }
)

$profileDirectory = Join-Path $env:TEMP "AMSim-TerminalGrowth-Edge"
$manifestAssets = New-Object System.Collections.Generic.List[object]
foreach ($asset in $assets) {
$body = if ($asset.Body) {
		$asset.Body
    } elseif ($asset.Pattern) {
        "<rect width=`"256`" height=`"256`" fill=`"url(#$($asset.Pattern))`"/>"
	} elseif ($asset.Square) {
		"<rect width=`"256`" height=`"256`" rx=`"34`" fill=`"#0c3448`"/><use href=`"#$($asset.Symbol)`" x=`"36`" y=`"36`" width=`"184`" height=`"184`"/>"
    } else {
        "<use href=`"#$($asset.Symbol)`" x=`"16`" y=`"48`" width=`"224`" height=`"140`"/>"
    }
    $standalone = "<svg xmlns=`"http://www.w3.org/2000/svg`" width=`"256`" height=`"256`" viewBox=`"0 0 256 256`"><defs>$defs</defs>$body</svg>"
    $svgPath = Join-Path $vectorDirectory ("T_{0}.svg" -f $asset.Name)
    $pngPath = Join-Path $textureDirectory ("T_{0}.png" -f $asset.Name)
    [System.IO.File]::WriteAllText($svgPath, $standalone, [System.Text.UTF8Encoding]::new($false))
    $uri = "file:///" + ($svgPath -replace "\\", "/")
    $edgeArgs = @(
        "--headless",
        "--disable-gpu",
        "--user-data-dir=$profileDirectory",
        "--hide-scrollbars",
        "--window-size=256,256",
        "--screenshot=$pngPath",
        $uri
    )
    if (-not $asset.Pattern -and -not $asset.Square) {
        $edgeArgs = @("--default-background-color=00000000") + $edgeArgs
    }
    $process = Start-Process -FilePath $edge -ArgumentList $edgeArgs -WindowStyle Hidden -Wait -PassThru
    if ($process.ExitCode -ne 0 -or -not (Test-Path $pngPath)) {
        throw "Failed to rasterize $($asset.Name)."
    }
    $manifestAssets.Add([ordered]@{
        name = $asset.Name
        category = $asset.Category
        source = (Get-RepositoryRelativePath $svgPath)
        texture = (Get-RepositoryRelativePath $pngPath)
        dimensions = @(256, 256)
        pixelsPerMeter = 32
        rotation = "QuarterTurn"
        sha256 = (Get-FileHash -LiteralPath $pngPath -Algorithm SHA256).Hash.ToLowerInvariant()
    })
}

$manifest = [ordered]@{
    schemaVersion = 1
    generatedAt = (Get-Date).ToString("yyyy-MM-ddTHH:mm:ssK")
    source = (Get-RepositoryRelativePath $sourceAbsolute)
    sourceSha256 = (Get-FileHash -LiteralPath $sourceAbsolute -Algorithm SHA256).Hash.ToLowerInvariant()
    generator = "scripts/assets/Export-TerminalGrowthVectorKit.ps1"
    assets = $manifestAssets
}
$manifestPath = Join-Path $outputAbsolute "terminal-growth-runtime-manifest.json"
[System.IO.File]::WriteAllText(
    $manifestPath,
    ($manifest | ConvertTo-Json -Depth 8),
    [System.Text.UTF8Encoding]::new($false))

Write-Output "Exported $($assets.Count) terminal assets to $outputAbsolute"
Write-Output $manifestPath
