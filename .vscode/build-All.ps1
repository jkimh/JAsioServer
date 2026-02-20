# Build JAsioServer (Debug x64). Run from JAsioServer folder.
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$vcpkg = Join-Path (Split-Path $root -Parent) "vcpkg\installed\x64-windows"
$env:BOOST_INCLUDE_DIR = Join-Path $vcpkg "include"
$env:BOOST_LIB_DIR     = Join-Path $vcpkg "lib"

$vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) { throw "vswhere not found: $vswhere" }
$msbuild = & $vswhere -latest -products * -requires Microsoft.Component.MSBuild -find "MSBuild\**\Bin\MSBuild.exe" | Select-Object -First 1
if (-not $msbuild) { throw "MSBuild not found" }

$config = "Debug"
$platform = "x64"
$projects = @("JNetLib\JNetLib.vcxproj", "JLogger\JLogger.vcxproj", "JSharedLib\JSharedLib.vcxproj", "JServerLib\JServerLib.vcxproj", "JChatServer\JChatServer.vcxproj")
foreach ($p in $projects) {
    $path = Join-Path $root $p
    & $msbuild $path /m /p:Configuration=$config /p:Platform=$platform
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}
$workspaceRoot = Split-Path $root -Parent
Write-Host "Build succeeded: $workspaceRoot\Bin\JChatServer_Debug.exe"
