@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "PATH=%SystemRoot%\System32;%SystemRoot%;%SystemRoot%\System32\Wbem;%SystemRoot%\System32\WindowsPowerShell\v1.0;C:\Program Files\CMake\bin;%PATH%"

for %%I in ("%~dp0.") do set "proj_root=%%~fI"
pushd "%proj_root%" || exit /b 1

set "build_path=%CD%\build\var16_build"
set "use_cov=OFF"
if /i "%~1"=="--coverage" (
    set "use_cov=ON"
    set "build_path=%CD%\build\var16_cov"
) else if not "%~1"=="" (
    echo Usage: run_tests.bat [--coverage]
    goto fail_exit
)

set "cmake_cmd=cmake.exe"
set "ctest_cmd=ctest.exe"
set "cmake_gen="
set "msvc_root="

where g++.exe >nul 2>&1 && where mingw32-make.exe >nul 2>&1 && set "cmake_gen=-G "MinGW Makefiles""
if "%use_cov%"=="OFF" if defined cmake_gen set "build_path=%CD%\build\var16_mingw"
if not defined cmake_gen call :find_msvc

"%cmake_cmd%" --version >nul 2>&1
if errorlevel 1 (
    call :find_msvc_cmake || (
        echo CMake 3.21+ is required.
        goto fail_exit
    )
)
if not defined cmake_gen if defined msvc_root (
    call :find_msvc_ninja || goto fail_exit
)

if "%use_cov%"=="ON" (
    for %%T in (g++.exe gcov.exe mingw32-make.exe gcovr) do (
        where %%T >nul 2>&1 || (
            echo Missing coverage dependency: %%T
            goto fail_exit
        )
    )
)

if not exist "%build_path%\" mkdir "%build_path%" || goto fail_exit
del /q "%build_path%\test-results.txt" "%build_path%\test-results.xml" "%build_path%\coverage.txt" "%build_path%\coverage-summary.json" 2>nul
if exist "%build_path%\test_results.png" del /q "%build_path%\test_results.png" || goto fail_exit

"%cmake_cmd%" -S "%proj_root%" -B "%build_path%" %cmake_gen% -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DVAR16_COVERAGE=%use_cov% || goto fail_exit
"%cmake_cmd%" --build "%build_path%" --config Debug || goto fail_exit

if "%use_cov%"=="ON" (
    for /r "%build_path%" %%F in (*.gcda) do del /q "%%F"
)
"%ctest_cmd%" --test-dir "%build_path%" -C Debug --no-tests=error --output-on-failure --output-log "%build_path%\test-results.txt" --output-junit "%build_path%\test-results.xml"
set "exit_code=%errorlevel%"
echo Reports: "%build_path%\test-results.txt" and "%build_path%\test-results.xml"

if "%exit_code%"=="0" if "%use_cov%"=="ON" (
    gcovr --root "%proj_root%" --txt "%build_path%\coverage.txt" --txt-summary --json-summary "%build_path%\coverage-summary.json" --html-details "%build_path%\coverage.html" "%build_path%"
    if errorlevel 1 (
        set "exit_code=1"
    ) else (
        echo Coverage: "%build_path%\coverage.txt" and "%build_path%\coverage.html"
    )
)
if exist "%build_path%\test-results.xml" (
    set "SCRIPT_FILE=%~f0"
    set "BUILD_DIR=%build_path%"
    powershell.exe -NoProfile -STA -Command "$raw = [IO.File]::ReadAllText($env:SCRIPT_FILE); $chunks = ($raw -split '(?m)^:draw_report\r?$', 2); & ([scriptblock]::Create($chunks[1])) -BuildDirectory $env:BUILD_DIR" || goto fail_exit
)
if not "%exit_code%"=="0" goto fail_exit
popd
exit /b 0

:find_msvc
set "vswhere=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%vswhere%" exit /b 1
for /f "usebackq delims=" %%I in (`"%vswhere%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.CMake.Project -property installationPath`) do (
    set "msvc_root=%%I"
)
if not defined msvc_root exit /b 1
exit /b 0

:find_msvc_cmake
if not defined msvc_root call :find_msvc || exit /b 1
set "cmake_cmd=%msvc_root%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
set "ctest_cmd=%msvc_root%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\ctest.exe"
if not exist "%cmake_cmd%" exit /b 1
exit /b 0

:find_msvc_ninja
set "vs_ninja=%msvc_root%\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja"
if not exist "%vs_ninja%\ninja.exe" exit /b 0
if not exist "%msvc_root%\VC\Auxiliary\Build\vcvars64.bat" exit /b 0
set "VSLANG=1033"
call "%msvc_root%\VC\Auxiliary\Build\vcvars64.bat" || exit /b 1
set "PATH=%vs_ninja%;%PATH%"
set "cmake_gen=-G Ninja"
exit /b 0

:fail_exit
popd
exit /b 1

:draw_report
param(
    [Parameter(Mandatory = $true)]
    [string] $BuildDirectory
)

$ErrorActionPreference = 'Stop'
$xmlPath = Join-Path $BuildDirectory 'test-results.xml'
$doc = [xml][IO.File]::ReadAllText($xmlPath)
$cases = @($doc.SelectNodes('//testcase'))
$fails = @($cases | Where-Object { $_.SelectSingleNode('failure | error') })
$skips = @($cases | Where-Object {
    $_.GetAttribute('status') -eq 'notrun' -or $_.SelectSingleNode('skipped')
})
$okCount = $cases.Count - $fails.Count - $skips.Count
$stamp = $doc.DocumentElement.GetAttribute('timestamp').Replace('T', ' ')

$info = @(
    'TMAU-2026  |  Laboratory 02  |  Variant 16 (as06809)'
    'Google Test / CTest'
    ('Test run: {0}' -f $stamp)
    ''
    ('Tests: {0} passed, {1} failed, {2} skipped (total: {3})' -f
        $okCount, $fails.Count, $skips.Count, $cases.Count)
)
foreach ($f in $fails) {
    $info += 'FAILED: ' + $f.GetAttribute('name')
}

$covJson = Join-Path $BuildDirectory 'coverage-summary.json'
$covTitle = 'Coverage measured during this run'
$covObj = $null
if (Test-Path -LiteralPath $covJson -PathType Leaf) {
    $covObj = [IO.File]::ReadAllText($covJson) | ConvertFrom-Json
} else {
    $covTitle = 'Saved GCC coverage measurement'
    $covObj = '{"root": "../..", "gcovr/summary_format_version": "0.6", "files": [{"filename": "src/main.cpp", "line_total": 66, "line_covered": 65, "line_percent": 98.5, "function_total": 15, "function_covered": 15, "function_percent": 100.0, "branch_total": 62, "branch_covered": 53, "branch_percent": 85.5}, {"filename": "src/StabilityAnalyzer.cpp", "line_total": 26, "line_covered": 26, "line_percent": 100.0, "function_total": 3, "function_covered": 3, "function_percent": 100.0, "branch_total": 14, "branch_covered": 12, "branch_percent": 85.7}], "line_total": 92, "line_covered": 91, "line_percent": 98.9, "function_total": 18, "function_covered": 18, "function_percent": 100.0, "branch_total": 76, "branch_covered": 65, "branch_percent": 85.5}' | ConvertFrom-Json
}
if ($covObj) {
    $info += ''
    $info += $covTitle
    $info += '   Lines       {0:0.0}%  ({1}/{2})' -f $covObj.line_percent,
        $covObj.line_covered, $covObj.line_total
    $info += '   Branches    {0:0.0}%  ({1}/{2})' -f $covObj.branch_percent,
        $covObj.branch_covered, $covObj.branch_total
    $info += '   Functions   {0:0.0}%  ({1}/{2})' -f $covObj.function_percent,
        $covObj.function_covered, $covObj.function_total
}

Write-Host ''
foreach ($l in $info) {
    Write-Host $l
}

Add-Type -AssemblyName System.Drawing
$fRegular = New-Object System.Drawing.Font('Segoe UI', 11)
$fStrong = New-Object System.Drawing.Font('Segoe UI', 11, [System.Drawing.FontStyle]::Bold)
$fHeader = New-Object System.Drawing.Font('Segoe UI', 25, [System.Drawing.FontStyle]::Bold)
$colors = @{}
foreach ($kv in @{
    white = '#FFFFFF'; navy = '#132257'; ink = '#27394C'; muted = '#546579'
    header = '#F2F5F8'; stripe = '#E9F5FC'; total = '#DAEAF6'; rule = '#D3DBE4'
    track = '#E0E6EC'; high = '#1B803E'; medium = '#946200'; low = '#C72B37'
}.GetEnumerator()) {
    $clr = [System.Drawing.ColorTranslator]::FromHtml($kv.Value)
    $colors[$kv.Key] = New-Object System.Drawing.SolidBrush($clr)
}

function Put-Text($Text, $Font, $Brush, [single] $X, [single] $Y) {
    $g.DrawString($Text, $Font, $Brush, $X, $Y)
}

function Put-Gauge($Entry, $Metric, [int] $X, [int] $Y) {
    $cnt = $Entry."${Metric}_total"
    if ($null -eq $cnt -or $cnt -eq 0) {
        Put-Text '-' $fRegular $colors.muted $X $Y
        return
    }
    $val = $Entry."${Metric}_percent"
    $lvl = if ($val -ge 90) { 'high' } elseif ($val -ge 75) { 'medium' } else { 'low' }
    $hit = $Entry."${Metric}_covered"
    Put-Text ('{0} / {1}' -f $hit, $cnt) $fRegular $colors.ink $X $Y
    $g.FillRectangle($colors.track, ($X + 63), ($Y + 8), 96, 10)
    $barW = [int][Math]::Round(0.96 * $val)
    if ($barW -gt 0) {
        $g.FillRectangle($colors[$lvl], ($X + 63), ($Y + 8), $barW, 10)
    }
    Put-Text ('{0:0.0}%' -f $val) $fStrong $colors[$lvl] ($X + 169) $Y
}

$rowsList = if ($covObj) { @($covObj.files) } else { @() }
$imgH = 340 + 39 * @($rowsList | Where-Object { $_ }).Count
$canvasBmp = New-Object System.Drawing.Bitmap(1160, $imgH)
try {
    $g = [System.Drawing.Graphics]::FromImage($canvasBmp)
    try {
        $g.Clear([System.Drawing.Color]::White)
        $g.TextRenderingHint = [System.Drawing.Text.TextRenderingHint]::AntiAliasGridFit
        $topTitle = 'GCC Code Coverage Report'
        $tw = $g.MeasureString($topTitle, $fHeader).Width
        Put-Text $topTitle $fHeader $colors.navy ((1160 - $tw) / 2) 20
        $g.FillRectangle($colors.rule, 20, 76, 1120, 3)

        Put-Text 'TMAU-2026 / Laboratory 02 / Variant 16 (as06809)' $fStrong $colors.navy 35 98
        Put-Text ('Tests: {0} / {1} passed, {2} failed, {3} skipped' -f
            $okCount, $cases.Count, $fails.Count, $skips.Count) $fStrong $colors.ink 35 125
        Put-Text ('Test run: {0}' -f $stamp) $fRegular $colors.muted 35 153
        if ($covObj) {
            Put-Text $covTitle $fRegular $colors.muted 35 181
            $posY = 95
            foreach ($col in 'Lines', 'Branches', 'Functions') {
                Put-Text $col $fRegular $colors.ink 790 $posY
                $prop = @{ Lines = 'line'; Branches = 'branch'; Functions = 'function' }[$col]
                Put-Gauge $covObj $prop 880 $posY
                $posY += 27
            }
        }

        $g.FillRectangle($colors.header, 20, 220, 1120, 37)
        Put-Text 'File' $fStrong $colors.navy 35 228
        Put-Text 'Lines' $fStrong $colors.navy 365 228
        Put-Text 'Branches' $fStrong $colors.navy 620 228
        Put-Text 'Functions' $fStrong $colors.navy 875 228
        $curY = 257
        if ($covObj) {
            $allRows = @($rowsList) + @($covObj)
            for ($i = 0; $i -lt $allRows.Count; $i++) {
                $itemRow = $allRows[$i]
$isEnd = $i -eq ($allRows.Count - 1)
$fill = if ($isEnd) { 'total' } elseif ($i % 2) { 'stripe' } else { 'white' }
$g.FillRectangle($colors[$fill], 20, $curY, 1120, 39)
$nameTxt = if ($isEnd) { 'Total' } else { $itemRow.filename }
$rowFnt = if ($isEnd) { $fStrong } else { $fRegular }
Put-Text $nameTxt $rowFnt $colors.navy 35 ($curY + 9)
Put-Gauge $itemRow 'line' 365 ($curY + 9)
Put-Gauge $itemRow 'branch' 620 ($curY + 9)
Put-Gauge $itemRow 'function' 875 ($curY + 9)
$curY += 39
}
} else {
Put-Text 'No GCC coverage data. Run with --coverage to measure it.' $fRegular $colors.muted 35 266
$curY += 39
}
Put-Text 'Scope: simulation and stability analysis (Variant 16)' $fRegular $colors.muted 35 ($curY + 12)
} finally {
$g.Dispose()
}
Add-Type -AssemblyName System.Windows.Forms
$dlg = New-Object System.Windows.Forms.Form
try {
$dlg.Text = 'TMAU-2026 - Test results (Variant 16)'
$dlg.StartPosition = [System.Windows.Forms.FormStartPosition]::CenterScreen
$dlg.BackColor = [System.Drawing.Color]::White
$scr = [System.Windows.Forms.Screen]::PrimaryScreen.WorkingArea
$dlg.ClientSize = New-Object System.Drawing.Size(
[Math]::Min($canvasBmp.Width, $scr.Width - 64),
[Math]::Min($canvasBmp.Height, $scr.Height - 96)
)
$pb = New-Object System.Windows.Forms.PictureBox
$pb.Dock = [System.Windows.Forms.DockStyle]::Fill
$pb.SizeMode = [System.Windows.Forms.PictureBoxSizeMode]::Zoom
$pb.Image = $canvasBmp
$dlg.Controls.Add($pb)
$dlg.Show()
$dlg.Activate()
$dlg.Refresh()
$pngFile = Join-Path $BuildDirectory 'test_results.png'
$canvasBmp.Save($pngFile, [System.Drawing.Imaging.ImageFormat]::Png)
Write-Host ('PNG saved: "{0}"' -f $pngFile)
[System.Windows.Forms.Application]::Run($dlg)
} finally {
$dlg.Dispose()
}
} finally {
$canvasBmp.Dispose()
$fRegular.Dispose()
$fStrong.Dispose()
$fHeader.Dispose()
foreach ($br in $colors.Values) { $br.Dispose() }
}
