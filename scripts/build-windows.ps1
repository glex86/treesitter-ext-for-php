<#
.SYNOPSIS
    Builds (and tests) the extension on Windows with php-windows-builder.

.DESCRIPTION
    Wraps Invoke-PhpBuildExtension of the BuildPhpExtension module:

      Install-Module BuildPhpExtension -Repository PSGallery -Scope CurrentUser -Force

    and takes care of the things that go wrong when the module is pointed at a working tree:

    - it builds a clean copy of the sources (the files git knows plus the new ones, without the ignored
      ones), the tree may hold the leftovers of a Linux build (Makefile, config.h, build/, *.lo) that
      confuse phpize, and the path stays short;
    - the PHP SDK starts its task script from the current directory, which does not work when the
      environment has NoDefaultCurrentDirectoryInExePath=1: that is cleared for this process only.

    Needs Visual Studio with the C++ tools and the Windows SDK (mc.exe comes from it) that belong to the
    PHP version: Visual Studio 2022 for PHP 8.4. The result is copied to the artifacts directory.

.PARAMETER PhpVersion
    PHP version to build for, 8.4 by default.

.PARAMETER Arch
    x64 (default) or x86, has to match the php.exe that loads the extension.

.PARAMETER Ts
    nts (default) or ts, has to match the php.exe that loads the extension.

.PARAMETER Work
    Working directory, keep the path short. It is deleted and created again.

.PARAMETER SkipTests
    Do not run the phpt tests after the build.

.EXAMPLE
    .\scripts\build-windows.ps1
    .\scripts\build-windows.ps1 -PhpVersion 8.4 -Arch x64 -Ts ts
#>
param(
    [string] $PhpVersion = '8.4',
    [ValidateSet('x64', 'x86')] [string] $Arch = 'x64',
    [ValidateSet('nts', 'ts')] [string] $Ts = 'nts',
    [string] $Work = 'C:\tsb',
    [switch] $SkipTests
)

$ErrorActionPreference = 'Stop'

$repository = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path

if (-not (Get-Module -ListAvailable -Name BuildPhpExtension)) {
    throw "The BuildPhpExtension module is missing. Install it: Install-Module BuildPhpExtension -Repository PSGallery -Scope CurrentUser -Force"
}

if (-not (Test-Path (Join-Path $repository '.git'))) {
    throw "$repository is not a git working tree, the list of the files to build comes from git."
}

# a clean copy of the sources
$source = Join-Path $Work 'src'

if (Test-Path $Work) {
    Remove-Item $Work -Recurse -Force
}

New-Item -ItemType Directory -Force $source | Out-Null

# the files git knows plus the new ones, the submodules (mode 160000 in the index) are entered recursively
function Get-SourceFiles([string] $root, [string] $prefix) {
    $submodules = @(git -C $root ls-files --stage | Where-Object { $_ -match '^160000 ' } |
        ForEach-Object { ($_ -split "`t", 2)[1] })

    foreach ($file in (git -C $root ls-files --cached --others --exclude-standard)) {
        if ($submodules -contains $file) {
            Get-SourceFiles (Join-Path $root $file) "$prefix$file/"
        } else {
            [pscustomobject]@{ From = Join-Path $root $file; Relative = "$prefix$file" }
        }
    }
}

$copied = 0
foreach ($entry in (Get-SourceFiles $repository '')) {
    $from = $entry.From

    if (-not (Test-Path -LiteralPath $from -PathType Leaf)) {
        continue
    }

    $to = Join-Path $source $entry.Relative
    $directory = Split-Path $to -Parent

    if (-not (Test-Path $directory)) {
        New-Item -ItemType Directory -Force $directory | Out-Null
    }

    Copy-Item -LiteralPath $from -Destination $to -Force
    $copied++
}

Write-Host "Copied $copied files to $source"

# the task script of the PHP SDK is started from the current directory
$env:NoDefaultCurrentDirectoryInExePath = $null

# vcvars and the SDK append to PATH inside cmd.exe, whose command line is limited to 8191 characters: with a
# long PATH the build stops with "The input line is too long". Drop the duplicates and the missing directories.
$env:Path = (($env:Path -split ';' | Where-Object { $_ -and (Test-Path -LiteralPath $_) } |
    Select-Object -Unique) -join ';')
# The module rebuilds PATH from the registry (user + machine) in its Add-Path, which brings the long PATH back
# and also writes the user PATH. Within the module that function is replaced with one that touches only the
# PATH of this process.
Import-Module BuildPhpExtension -Force
& (Get-Module BuildPhpExtension) {
    # script: scope, a plain "function" would only live in the scope of this script block
    Set-Item -Path 'Function:\script:Add-Path' -Value {
        param([Parameter(Mandatory = $true, Position = 0)] [string] $PathItem)
        if (-not ($env:Path.Split(';') -contains $PathItem)) {
            $env:Path = $PathItem + ';' + $env:Path
        }
    }
}
$env:BUILD_DIRECTORY = Join-Path $Work 'work'
# the module does not run the tests, this script does (see below)
$env:RUN_TESTS = 'false'

New-Item -ItemType Directory -Force $env:BUILD_DIRECTORY | Out-Null

Import-Module BuildPhpExtension

$log = Join-Path $Work 'build.log'

Push-Location $source

try {
    Invoke-PhpBuildExtension -PhpVersion $PhpVersion -Arch $Arch -Ts $Ts *>&1 | Tee-Object -FilePath $log
} finally {
    Pop-Location
}

$artifacts = Join-Path $source 'artifacts'

if (-not (Test-Path $artifacts)) {
    throw "The build failed, there are no artifacts. See $log"
}

# The tests. The module could run them (RUN_TESTS), but before that it lists C:\Windows\Temp recursively,
# which fails with "access denied" for a user who is not an administrator. The build directory holds
# everything that is needed: the PHP that the module downloaded, run-tests.php, the tests and the DLL.
if (-not $SkipTests) {
    $build = Get-ChildItem $env:BUILD_DIRECTORY -Directory | Select-Object -First 1
    $php = Join-Path $build.FullName 'php-bin\php.exe'
    $dll = Get-ChildItem $build.FullName -Recurse -Filter 'php_treesitter.dll' | Select-Object -First 1

    if (-not (Test-Path $php) -or $null -eq $dll) {
        throw "Cannot find the PHP or the built DLL in $($build.FullName) to run the tests."
    }

    $env:TEST_PHP_EXECUTABLE = $php
    $env:TEST_PHP_ARGS = "-n -d extension=$($dll.FullName)"
    $env:NO_INTERACTION = '1'
    $env:REPORT_EXIT_STATUS = '1'

    Push-Location $build.FullName
    try {
        & $php run-tests.php -q --offline --show-diff -j4 tests *>&1 | Tee-Object -FilePath (Join-Path $Work 'tests.log') | Select-Object -Last 12

        if ($LASTEXITCODE -ne 0) {
            throw "The tests failed, see $(Join-Path $Work 'tests.log')"
        }
    } finally {
        Pop-Location
    }
}

$destination = Join-Path $repository 'artifacts'

if (Test-Path $destination) {
    Remove-Item $destination -Recurse -Force
}

Copy-Item $artifacts $destination -Recurse
Write-Host "Done, the artifacts are in $destination"
Get-ChildItem $destination | Select-Object Name, Length
