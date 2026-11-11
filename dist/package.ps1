#Requires -Version 5.1
$ErrorActionPreference = "Stop"
$Root = Split-Path $PSScriptRoot -Parent

$Version = "1.0.0"
$cmakeLists = Join-Path $Root "CMakeLists.txt"
if (Test-Path $cmakeLists) {
    $m = Select-String -Path $cmakeLists -Pattern 'project\(mnBazz VERSION (\d+\.\d+\.\d+)\)' | Select-Object -First 1
    if ($m) { $Version = $m.Matches.Groups[1].Value }
}
$Build = Join-Path $Root "build\mnBazz_artefacts\Release"
$Vst3Src = Join-Path $Build "VST3\mnBazz.vst3"
$ExeSrc = Join-Path $Build "Standalone\mnBazz.exe"
$Staging = Join-Path $PSScriptRoot "staging"
$ZipName = "mnBazz-$Version-win64.zip"
$ZipPath = Join-Path $PSScriptRoot $ZipName

if (-not (Test-Path $Vst3Src)) { throw "Hiányzik: $Vst3Src — előbb Release build." }
if (-not (Test-Path $ExeSrc)) { throw "Hiányzik: $ExeSrc — előbb Release build." }

if (Test-Path $Staging) { Remove-Item $Staging -Recurse -Force }
New-Item -ItemType Directory -Path (Join-Path $Staging "Standalone") | Out-Null
Copy-Item $Vst3Src (Join-Path $Staging "mnBazz.vst3") -Recurse
Copy-Item $ExeSrc (Join-Path $Staging "Standalone\mnBazz.exe")
Copy-Item (Join-Path $PSScriptRoot "INSTALL.txt") (Join-Path $Staging "INSTALL.txt")

if (Test-Path $ZipPath) { Remove-Item $ZipPath -Force }
Compress-Archive -Path (Join-Path $Staging "*") -DestinationPath $ZipPath -CompressionLevel Optimal

Write-Host "Kész: $ZipPath"
Remove-Item $Staging -Recurse -Force
