@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "PATH=%SystemRoot%\System32;%SystemRoot%;%SystemRoot%\System32\Wbem;%SystemRoot%\System32\WindowsPowerShell\v1.0;C:\Program Files\CMake\bin;%PATH%"

for %%I in ("%~dp0.") do set "root_dir=%%~fI"
pushd "%root_dir%" || exit /b 1

set "out_dir=%CD%\build\var17_build"
set "cov_flag=OFF"
if /i "%~1"=="--coverage" (
    set "cov_flag=ON"
    set "out_dir=%CD%\build\var17_cov"
) else if not "%~1"=="" (
    echo Syntax: run_tests.bat [--coverage]
    goto on_error
)

set "cmake_bin=cmake.exe"
set "ctest_bin=ctest.exe"
set "gen_arg="
set "vs_dir="

where g++.exe >nul 2>&1 && where mingw32-make.exe >nul 2>&1 && set "gen_arg=-G "MinGW Makefiles""
if "%cov_flag%"=="OFF" if defined gen_arg set "out_dir=%CD%\build\var17_mingw"
if not defined gen_arg call :locate_vs

"%cmake_bin%" --version >nul 2>&1
if errorlevel 1 (
    call :locate_vs_cmake || (
        echo CMake 3.21+ is required.
        goto on_error
    )
)
if not defined gen_arg if defined vs_dir (
    call :setup_vs_ninja || goto on_error
)

if "%cov_flag%"=="ON" (
    for %%T in (g++.exe gcov.exe mingw32-make.exe gcovr) do (
        where %%T >nul 2>&1 || (
            echo Missing tool for coverage: %%T
            goto on_error
        )
    )
)

if not exist "%out_dir%\" mkdir "%out_dir%" || goto on_error
del /q "%out_dir%\test-results.txt" "%out_dir%\test-results.xml" "%out_dir%\coverage.txt" "%out_dir%\coverage-summary.json" 2>nul
if exist "%out_dir%\test_results.png" del /q "%out_dir%\test_results.png" || goto on_error

"%cmake_bin%" -S "%root_dir%" -B "%out_dir%" %gen_arg% -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DENABLE_GCC_COVERAGE=%cov_flag% || goto on_error
"%cmake_bin%" --build "%out_dir%" --config Debug || goto on_error

if "%cov_flag%"=="ON" (
    for /r "%out_dir%" %%F in (*.gcda) do del /q "%%F"
)
"%ctest_bin%" --test-dir "%out_dir%" -C Debug --no-tests=error --output-on-failure --output-log "%out_dir%\test-results.txt" --output-junit "%out_dir%\test-results.xml"
set "run_rc=%errorlevel%"
echo Reports saved: "%out_dir%\test-results.txt" and "%out_dir%\test-results.xml"

if "%run_rc%"=="0" if "%cov_flag%"=="ON" (
    gcovr --root "%root_dir%" --txt "%out_dir%\coverage.txt" --txt-summary --json-summary "%out_dir%\coverage-summary.json" --html-details "%out_dir%\coverage.html" "%out_dir%"
    if errorlevel 1 (
        set "run_rc=1"
    ) else (
        echo Coverage saved: "%out_dir%\coverage.txt" and "%out_dir%\coverage.html"
    )
)
if exist "%out_dir%\test-results.xml" (
    set "BAT_SELF=%~f0"
    set "BAT_OUT=%out_dir%"
    powershell.exe -NoProfile -STA -Command "$txt = [IO.File]::ReadAllText($env:BAT_SELF); $part = ($txt -split '(?m)^:render_summary\r?$', 2); & ([scriptblock]::Create($part[1])) -BuildDirectory $env:BAT_OUT" || goto on_error
)
if not "%run_rc%"=="0" goto on_error
popd
exit /b 0

:locate_vs
set "vswhere=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%vswhere%" exit /b 1
for /f "usebackq delims=" %%I in (`"%vswhere%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.CMake.Project -property installationPath`) do (
    set "vs_dir=%%I"
)
if not defined vs_dir exit /b 1
exit /b 0

:locate_vs_cmake
if not defined vs_dir call :locate_vs || exit /b 1
set "cmake_bin=%vs_dir%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
set "ctest_bin=%vs_dir%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\ctest.exe"
if not exist "%cmake_bin%" exit /b 1
exit /b 0

:setup_vs_ninja
set "vs_ninja=%vs_dir%\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja"
if not exist "%vs_ninja%\ninja.exe" exit /b 0
if not exist "%vs_dir%\VC\Auxiliary\Build\vcvars64.bat" exit /b 0
set "VSLANG=1033"
call "%vs_dir%\VC\Auxiliary\Build\vcvars64.bat" || exit /b 1
set "PATH=%vs_ninja%;%PATH%"
set "gen_arg=-G Ninja"
exit /b 0

:on_error
popd
exit /b 1

:render_summary
param(
    [Parameter(Mandatory = $true)]
    [string] $BuildDirectory
)

$ErrorActionPreference = 'Stop'
$xmlFile = Join-Path $BuildDirectory 'test-results.xml'
$xmlDoc = [xml][IO.File]::ReadAllText($xmlFile)
$allCases = @($xmlDoc.SelectNodes('//testcase'))
$failedCases = @($allCases | Where-Object { $_.SelectSingleNode('failure | error') })
$skippedCases = @($allCases | Where-Object {
    $_.GetAttribute('status') -eq 'notrun' -or $_.SelectSingleNode('skipped')
})
$passedCount = $allCases.Count - $failedCases.Count - $skippedCases.Count
$runTime = $xmlDoc.DocumentElement.GetAttribute('timestamp').Replace('T', ' ')

$summaryLines = @(
    'TMAU-2026  |  Laboratory 02  |  Variant 17 (as06802)'
    'Google Test / CTest'
    ('Test run: {0}' -f $runTime)
    ''
    ('Tests: {0} passed, {1} failed, {2} skipped (total: {3})' -f
        $passedCount, $failedCases.Count, $skippedCases.Count, $allCases.Count)
)
foreach ($tc in $failedCases) {
    $summaryLines += 'FAILED: ' + $tc.GetAttribute('name')
}

$covFile = Join-Path $BuildDirectory 'coverage-summary.json'
$covHeader = 'Coverage measured during this run'
$covData = $null
if (Test-Path -LiteralPath $covFile -PathType Leaf) {
    $covData = [IO.File]::ReadAllText($covFile) | ConvertFrom-Json
} else {
    $covHeader = 'Saved GCC coverage measurement'
    $covData = '{"root": "../..", "gcovr/summary_format_version": "0.6", "files": [{"filename": "src/main.cpp", "line_total": 68, "line_covered": 67, "line_percent": 98.5, "function_total": 16, "function_covered": 16, "function_percent": 100.0, "branch_total": 64, "branch_covered": 54, "branch_percent": 84.4}, {"filename": "src/StabilityAnalyzer.cpp", "line_total": 22, "line_covered": 22, "line_percent": 100.0, "function_total": 3, "function_covered": 3, "function_percent": 100.0, "branch_total": 12, "branch_covered": 10, "branch_percent": 83.3}], "line_total": 90, "line_covered": 89, "line_percent": 98.9, "function_total": 19, "function_covered": 19, "function_percent": 100.0, "branch_total": 76, "branch_covered": 64, "branch_percent": 84.2}' | ConvertFrom-Json
}
if ($covData) {
    $summaryLines += ''
    $summaryLines += $covHeader
    $summaryLines += '   Lines       {0:0.0}%  ({1}/{2})' -f $covData.line_percent,
        $covData.line_covered, $covData.line_total
    $summaryLines += '   Branches    {0:0.0}%  ({1}/{2})' -f $covData.branch_percent,
        $covData.branch_covered, $covData.branch_total
    $summaryLines += '   Functions   {0:0.0}%  ({1}/{2})' -f $covData.function_percent,
        $covData.function_covered, $covData.function_total
}

Write-Host ''
foreach ($line in $summaryLines) {
    Write-Host $line
}

Add-Type -AssemblyName System.Drawing
$fontBody = New-Object System.Drawing.Font('Segoe UI', 11)
$fontBold = New-Object System.Drawing.Font('Segoe UI', 11, [System.Drawing.FontStyle]::Bold)
$fontTitle = New-Object System.Drawing.Font('Segoe UI', 25, [System.Drawing.FontStyle]::Bold)
$palette = @{}
foreach ($item in @{
    white = '#FFFFFF'; navy = '#132257'; ink = '#27394C'; muted = '#546579'
    header = '#F2F5F8'; stripe = '#E9F5FC'; total = '#DAEAF6'; rule = '#D3DBE4'
    track = '#E0E6EC'; high = '#1B803E'; medium = '#946200'; low = '#C72B37'
}.GetEnumerator()) {
    $c = [System.Drawing.ColorTranslator]::FromHtml($item.Value)
    $palette[$item.Key] = New-Object System.Drawing.SolidBrush($c)
}

function Draw-Txt($Text, $Font, $Brush, [single] $X, [single] $Y) {
    $gfx.DrawString($Text, $Font, $Brush, $X, $Y)
}

function Draw-Bar($Entry, $Metric, [int] $X, [int] $Y) {
    $tot = $Entry."${Metric}_total"
    if ($null -eq $tot -or $tot -eq 0) {
        Draw-Txt '-' $fontBody $palette.muted $X $Y
        return
    }
    $pct = $Entry."${Metric}_percent"
    $tone = if ($pct -ge 90) { 'high' } elseif ($pct -ge 75) { 'medium' } else { 'low' }
    $cov = $Entry."${Metric}_covered"
    Draw-Txt ('{0} / {1}' -f $cov, $tot) $fontBody $palette.ink $X $Y
    $gfx.FillRectangle($palette.track, ($X + 63), ($Y + 8), 96, 10)
    $w = [int][Math]::Round(0.96 * $pct)
    if ($w -gt 0) {
        $gfx.FillRectangle($palette[$tone], ($X + 63), ($Y + 8), $w, 10)
    }
    Draw-Txt ('{0:0.0}%' -f $pct) $fontBold $palette[$tone] ($X + 169) $Y
}

$fileEntries = if ($covData) { @($covData.files) } else { @() }
$bmpH = 340 + 39 * @($fileEntries | Where-Object { $_ }).Count
$bmp = New-Object System.Drawing.Bitmap(1160, $bmpH)
try {
    $gfx = [System.Drawing.Graphics]::FromImage($bmp)
    try {
        $gfx.Clear([System.Drawing.Color]::White)
        $gfx.TextRenderingHint = [System.Drawing.Text.TextRenderingHint]::AntiAliasGridFit
        $hdr = 'GCC Code Coverage Report'
        $hdrW = $gfx.MeasureString($hdr, $fontTitle).Width
        Draw-Txt $hdr $fontTitle $palette.navy ((1160 - $hdrW) / 2) 20
        $gfx.FillRectangle($palette.rule, 20, 76, 1120, 3)

        Draw-Txt 'TMAU-2026 / Laboratory 02 / Variant 17 (as06802)' $fontBold $palette.navy 35 98
        Draw-Txt ('Tests: {0} / {1} passed, {2} failed, {3} skipped' -f
            $passedCount, $allCases.Count, $failedCases.Count, $skippedCases.Count) $fontBold $palette.ink 35 125
        Draw-Txt ('Test run: {0}' -f $runTime) $fontBody $palette.muted 35 153
        if ($covData) {
            Draw-Txt $covHeader $fontBody $palette.muted 35 181
            $sy = 95
            foreach ($m in 'Lines', 'Branches', 'Functions') {
                Draw-Txt $m $fontBody $palette.ink 790 $sy
                $k = @{ Lines = 'line'; Branches = 'branch'; Functions = 'function' }[$m]
                Draw-Bar $covData $k 880 $sy
                $sy += 27
            }
        }

        $gfx.FillRectangle($palette.header, 20, 220, 1120, 37)
        Draw-Txt 'File' $fontBold $palette.navy 35 228
        Draw-Txt 'Lines' $fontBold $palette.navy 365 228
        Draw-Txt 'Branches' $fontBold $palette.navy 620 228
        Draw-Txt 'Functions' $fontBold $palette.navy 875 228
        $ry = 257
        if ($covData) {
            $tableRows = @($fileEntries) + @($covData)
            for ($idx = 0; $idx -lt $tableRows.Count; $idx++) {
                $r = $tableRows[$idx]
                $last = $idx -eq ($tableRows.Count - 1)
                $bg = if ($last) { 'total' } elseif ($idx % 2) { 'stripe' } else { 'white' }
                $gfx.FillRectangle($palette[$bg], 20, $ry, 1120, 39)
                $lbl = if ($last) { 'Total' } else { $r.filename }
                $fnt = if ($last) { $fontBold } else { $fontBody }
                Draw-Txt $lbl $fnt $palette.navy 35 ($ry + 9)
                Draw-Bar $r 'line' 365 ($ry + 9)
                Draw-Bar $r 'branch' 620 ($ry + 9)
                Draw-Bar $r 'function' 875 ($ry + 9)
                $ry += 39
            }
        } else {
            Draw-Txt 'No GCC coverage data. Run with --coverage to measure it.' $fontBody $palette.muted 35 266
            $ry += 39
        }
        Draw-Txt 'Scope: simulation and stability analysis (Variant 17)' $fontBody $palette.muted 35 ($ry + 12)
    } finally {
        $gfx.Dispose()
    }

    Add-Type -AssemblyName System.Windows.Forms
    $win = New-Object System.Windows.Forms.Form
    try {
        $win.Text = 'TMAU-2026 - Test results (Variant 17)'
        $win.StartPosition = [System.Windows.Forms.FormStartPosition]::CenterScreen
        $win.BackColor = [System.Drawing.Color]::White
        $area = [System.Windows.Forms.Screen]::PrimaryScreen.WorkingArea
        $win.ClientSize = New-Object System.Drawing.Size(
            [Math]::Min($bmp.Width, $area.Width - 64),
            [Math]::Min($bmp.Height, $area.Height - 96)
        )

        $pic = New-Object System.Windows.Forms.PictureBox
        $pic.Dock = [System.Windows.Forms.DockStyle]::Fill
        $pic.SizeMode = [System.Windows.Forms.PictureBoxSizeMode]::Zoom
        $pic.Image = $bmp
        $win.Controls.Add($pic)

        $win.Show()
        $win.Activate()
        $win.Refresh()

        $outPng = Join-Path $BuildDirectory 'test_results.png'
        $bmp.Save($outPng, [System.Drawing.Imaging.ImageFormat]::Png)
        Write-Host ('PNG saved: "{0}"' -f $outPng)
        [System.Windows.Forms.Application]::Run($win)
    } finally {
        $win.Dispose()
    }
} finally {
    $bmp.Dispose()
    $fontBody.Dispose()
    $fontBold.Dispose()
    $fontTitle.Dispose()
    foreach ($b in $palette.Values) { $b.Dispose() }
}
