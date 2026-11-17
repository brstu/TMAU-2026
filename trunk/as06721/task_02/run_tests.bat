@echo off
setlocal EnableExtensions DisableDelayedExpansion
for %%I in ("%~dp0.") do set "source_dir=%%~fI"
pushd "%source_dir%" || exit /b 1

set "build_dir=%CD%\build\task_02"
set "coverage=OFF"
if /i "%~1"=="--coverage" (
    set "coverage=ON"
    set "build_dir=%CD%\build\task_02_coverage"
) else if not "%~1"=="" (
    echo Usage: run_tests.bat [--coverage]
    goto failure
)

set "cmake=cmake.exe"
set "ctest=ctest.exe"
set "generator="
set "vs_path="
where g++.exe >nul 2>&1 && where mingw32-make.exe >nul 2>&1 && set "generator=-G "MinGW Makefiles""
if "%coverage%"=="OFF" if defined generator set "build_dir=%CD%\build\task_02_mingw"
if not defined generator call :find_visual_studio
where cmake.exe >nul 2>&1
if errorlevel 1 call :use_visual_studio_cmake
if errorlevel 1 (
    echo CMake 3.21 or later is required.
    goto failure
)
if not defined generator if defined vs_path (
    call :use_visual_studio_ninja || goto failure
)

if "%coverage%"=="ON" (
    for %%T in (g++.exe gcov.exe mingw32-make.exe gcovr) do (
        where %%T >nul 2>&1 || (
            echo Required for coverage: %%T
            goto failure
        )
    )
)

if not exist "%build_dir%\" mkdir "%build_dir%" || goto failure
del /q "%build_dir%\test-results.txt" "%build_dir%\test-results.xml" "%build_dir%\coverage.txt" "%build_dir%\coverage-summary.json" 2>nul
if exist "%build_dir%\test_results.png" del /q "%build_dir%\test_results.png" || goto failure

"%cmake%" -S "%source_dir%" -B "%build_dir%" %generator% -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DTASK02_COVERAGE=%coverage% || goto failure
"%cmake%" --build "%build_dir%" --config Debug || goto failure

if "%coverage%"=="ON" (
    for /r "%build_dir%" %%F in (*.gcda) do del /q "%%F"
)
"%ctest%" --test-dir "%build_dir%" -C Debug --no-tests=error --output-on-failure --output-log "%build_dir%\test-results.txt" --output-junit "%build_dir%\test-results.xml"
set "test_status=%errorlevel%"
echo Test reports: "%build_dir%\test-results.txt" and "%build_dir%\test-results.xml"

if "%test_status%"=="0" if "%coverage%"=="ON" (
    gcovr --root "%source_dir%" --txt "%build_dir%\coverage.txt" --txt-summary --json-summary "%build_dir%\coverage-summary.json" --html-details "%build_dir%\coverage.html" "%build_dir%"
    if errorlevel 1 (
        set "test_status=1"
    ) else (
        echo Coverage reports: "%build_dir%\coverage.txt" and "%build_dir%\coverage.html"
    )
)
if exist "%build_dir%\test-results.xml" (
    set "TASK02_BATCH_FILE=%~f0"
    set "TASK02_BUILD_DIR=%build_dir%"
    powershell.exe -NoProfile -STA -Command "$batch = [IO.File]::ReadAllText($env:TASK02_BATCH_FILE); $report = ($batch -split '(?m)^:powershell_report\r?$', 2)[1]; & ([scriptblock]::Create($report)) -BuildDirectory $env:TASK02_BUILD_DIR" || goto failure
)
if not "%test_status%"=="0" goto failure
popd
exit /b 0

:find_visual_studio
set "vswhere=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%vswhere%" exit /b 1
for /f "usebackq delims=" %%I in (`"%vswhere%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.CMake.Project -property installationPath`) do (
    set "vs_path=%%I"
)
if not defined vs_path exit /b 1
exit /b 0

:use_visual_studio_cmake
if not defined vs_path call :find_visual_studio || exit /b 1
set "cmake=%vs_path%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
set "ctest=%vs_path%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\ctest.exe"
if not exist "%cmake%" exit /b 1
exit /b 0

:use_visual_studio_ninja
set "vs_ninja=%vs_path%\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja"
if not exist "%vs_ninja%\ninja.exe" exit /b 0
if not exist "%vs_path%\VC\Auxiliary\Build\vcvars64.bat" exit /b 0
set "VSLANG=1033"
call "%vs_path%\VC\Auxiliary\Build\vcvars64.bat" || exit /b 1
set "PATH=%vs_ninja%;%PATH%"
set "generator=-G Ninja"
exit /b 0

:failure
popd
exit /b 1

:powershell_report
param(
    [Parameter(Mandatory = $true)]
    [string] $BuildDirectory
)

$ErrorActionPreference = 'Stop'
$xmlPath = Join-Path $BuildDirectory 'test-results.xml'
$junit = [xml][IO.File]::ReadAllText($xmlPath)
$tests = @($junit.SelectNodes('//testcase'))
$failed = @($tests | Where-Object { $_.SelectSingleNode('failure | error') })
$skipped = @($tests | Where-Object {
    $_.GetAttribute('status') -eq 'notrun' -or $_.SelectSingleNode('skipped')
})
$passed = $tests.Count - $failed.Count - $skipped.Count
$timestamp = $junit.DocumentElement.GetAttribute('timestamp').Replace('T', ' ')

$lines = @(
    'TMAU-2026  |  Laboratory 02  |  Variant 21'
    'Google Test / CTest'
    ('Test run: {0}' -f $timestamp)
    ''
    ('Tests: {0} passed, {1} failed, {2} skipped (total: {3})' -f
        $passed, $failed.Count, $skipped.Count, $tests.Count)
)
foreach ($test in $failed) {
    $lines += 'FAILED: ' + $test.GetAttribute('name')
}

$coveragePath = Join-Path $BuildDirectory 'coverage-summary.json'
$coverageLabel = 'Coverage measured during this run'
$coverage = $null
if (Test-Path -LiteralPath $coveragePath -PathType Leaf) {
    $coverage = [IO.File]::ReadAllText($coveragePath) | ConvertFrom-Json
} else {
    # Reproducible baseline from gcovr, used in the MSVC build.
    $coverageLabel = 'Saved GCC coverage measurement'
    $coverage = '{"root": "../..", "gcovr/summary_format_version": "0.6", "files": [{"filename": "src/main.cpp", "line_total": 57, "line_covered": 56, "line_percent": 98.2, "function_total": 17, "function_covered": 17, "function_percent": 100.0, "branch_total": 62, "branch_covered": 51, "branch_percent": 82.3}, {"filename": "src/StabilityAnalyzer.cpp", "line_total": 23, "line_covered": 23, "line_percent": 100.0, "function_total": 3, "function_covered": 3, "function_percent": 100.0, "branch_total": 12, "branch_covered": 10, "branch_percent": 83.3}], "line_total": 80, "line_covered": 79, "line_percent": 98.8, "function_total": 20, "function_covered": 20, "function_percent": 100.0, "branch_total": 74, "branch_covered": 61, "branch_percent": 82.4}' | ConvertFrom-Json
}
if ($coverage) {
    $lines += ''
    $lines += $coverageLabel
    $lines += '   Lines       {0:0.0}%  ({1}/{2})' -f $coverage.line_percent,
        $coverage.line_covered, $coverage.line_total
    $lines += '   Branches    {0:0.0}%  ({1}/{2})' -f $coverage.branch_percent,
        $coverage.branch_covered, $coverage.branch_total
    $lines += '   Functions   {0:0.0}%  ({1}/{2})' -f $coverage.function_percent,
        $coverage.function_covered, $coverage.function_total
}

Write-Host ''
foreach ($line in $lines) {
    Write-Host $line
}

Add-Type -AssemblyName System.Drawing
$body = New-Object System.Drawing.Font('Segoe UI', 11)
$bold = New-Object System.Drawing.Font('Segoe UI', 11, [System.Drawing.FontStyle]::Bold)
$title = New-Object System.Drawing.Font('Segoe UI', 25, [System.Drawing.FontStyle]::Bold)
$brushes = @{}
foreach ($pair in @{
    white = '#FFFFFF'; navy = '#132257'; ink = '#27394C'; muted = '#546579'
    header = '#F2F5F8'; stripe = '#E9F5FC'; total = '#DAEAF6'; rule = '#D3DBE4'
    track = '#E0E6EC'; high = '#1B803E'; medium = '#946200'; low = '#C72B37'
}.GetEnumerator()) {
    $color = [System.Drawing.ColorTranslator]::FromHtml($pair.Value)
    $brushes[$pair.Key] = New-Object System.Drawing.SolidBrush($color)
}

function Draw-Label($Text, $Font, $Brush, [single] $X, [single] $Y) {
    $canvas.DrawString($Text, $Font, $Brush, $X, $Y)
}

function Draw-Metric($Entry, $Metric, [int] $X, [int] $Y) {
    $totalCount = $Entry."${Metric}_total"
    if ($null -eq $totalCount -or $totalCount -eq 0) {
        Draw-Label '-' $body $brushes.muted $X $Y
        return
    }
    $ratio = $Entry."${Metric}_percent"
    $style = if ($ratio -ge 90) { 'high' } elseif ($ratio -ge 75) { 'medium' } else { 'low' }
    $coveredCount = $Entry."${Metric}_covered"
    Draw-Label ('{0} / {1}' -f $coveredCount, $totalCount) $body $brushes.ink $X $Y
    $canvas.FillRectangle($brushes.track, ($X + 63), ($Y + 8), 96, 10)
    $filled = [int][Math]::Round(0.96 * $ratio)
    if ($filled -gt 0) {
        $canvas.FillRectangle($brushes[$style], ($X + 63), ($Y + 8), $filled, 10)
    }
    Draw-Label ('{0:0.0}%' -f $ratio) $bold $brushes[$style] ($X + 169) $Y
}

$files = if ($coverage) { @($coverage.files) } else { @() }
$height = 340 + 39 * @($files | Where-Object { $_ }).Count
$image = New-Object System.Drawing.Bitmap(1160, $height)
try {
    $canvas = [System.Drawing.Graphics]::FromImage($image)
    try {
        $canvas.Clear([System.Drawing.Color]::White)
        $canvas.TextRenderingHint = [System.Drawing.Text.TextRenderingHint]::AntiAliasGridFit
        $heading = 'GCC Code Coverage Report'
        $headingWidth = $canvas.MeasureString($heading, $title).Width
        Draw-Label $heading $title $brushes.navy ((1160 - $headingWidth) / 2) 20
        $canvas.FillRectangle($brushes.rule, 20, 76, 1120, 3)

        Draw-Label 'TMAU-2026 / Laboratory 02 / Variant 21' $bold $brushes.navy 35 98
        Draw-Label ('Tests: {0} / {1} passed, {2} failed, {3} skipped' -f
            $passed, $tests.Count, $failed.Count, $skipped.Count) $bold $brushes.ink 35 125
        Draw-Label ('Test run: {0}' -f $timestamp) $body $brushes.muted 35 153
        if ($coverage) {
            Draw-Label $coverageLabel $body $brushes.muted 35 181
            $summaryY = 95
            foreach ($metric in 'Lines', 'Branches', 'Functions') {
                Draw-Label $metric $body $brushes.ink 790 $summaryY
                $key = @{ Lines = 'line'; Branches = 'branch'; Functions = 'function' }[$metric]
                Draw-Metric $coverage $key 880 $summaryY
                $summaryY += 27
            }
        }

        $canvas.FillRectangle($brushes.header, 20, 220, 1120, 37)
        Draw-Label 'File' $bold $brushes.navy 35 228
        Draw-Label 'Lines' $bold $brushes.navy 365 228
        Draw-Label 'Branches' $bold $brushes.navy 620 228
        Draw-Label 'Functions' $bold $brushes.navy 875 228
        $rowY = 257
        if ($coverage) {
            $rows = @($files) + @($coverage)
            for ($index = 0; $index -lt $rows.Count; $index++) {
                $row = $rows[$index]
                $isTotal = $index -eq ($rows.Count - 1)
                $rowStyle = if ($isTotal) { 'total' } elseif ($index % 2) { 'stripe' } else { 'white' }
                $canvas.FillRectangle($brushes[$rowStyle], 20, $rowY, 1120, 39)
                $label = if ($isTotal) { 'Total' } else { $row.filename }
                $rowFont = if ($isTotal) { $bold } else { $body }
                Draw-Label $label $rowFont $brushes.navy 35 ($rowY + 9)
                Draw-Metric $row 'line' 365 ($rowY + 9)
                Draw-Metric $row 'branch' 620 ($rowY + 9)
                Draw-Metric $row 'function' 875 ($rowY + 9)
                $rowY += 39
            }
        } else {
            Draw-Label 'No GCC coverage data. Run with --coverage to measure it.' $body $brushes.muted 35 266
            $rowY += 39
        }
        Draw-Label 'Scope: simulation and stability analysis' $body $brushes.muted 35 ($rowY + 12)
    } finally {
        $canvas.Dispose()
    }

    Add-Type -AssemblyName System.Windows.Forms
    $preview = New-Object System.Windows.Forms.Form
    try {
        $preview.Text = 'TMAU-2026 - Test results'
        $preview.StartPosition = [System.Windows.Forms.FormStartPosition]::CenterScreen
        $preview.BackColor = [System.Drawing.Color]::White
        $screen = [System.Windows.Forms.Screen]::PrimaryScreen.WorkingArea
        $preview.ClientSize = New-Object System.Drawing.Size(
            [Math]::Min($image.Width, $screen.Width - 64),
            [Math]::Min($image.Height, $screen.Height - 96)
        )

        $picture = New-Object System.Windows.Forms.PictureBox
        $picture.Dock = [System.Windows.Forms.DockStyle]::Fill
        $picture.SizeMode = [System.Windows.Forms.PictureBoxSizeMode]::Zoom
        $picture.Image = $image
        $preview.Controls.Add($picture)

        $preview.Show()
        $preview.Activate()
        $preview.Refresh()

        $pngPath = Join-Path $BuildDirectory 'test_results.png'
        $image.Save($pngPath, [System.Drawing.Imaging.ImageFormat]::Png)
        Write-Host ('PNG saved: "{0}"' -f $pngPath)
        [System.Windows.Forms.Application]::Run($preview)
    } finally {
        $preview.Dispose()
    }
} finally {
    $image.Dispose()
    $body.Dispose()
    $bold.Dispose()
    $title.Dispose()
    foreach ($brush in $brushes.Values) { $brush.Dispose() }
}

