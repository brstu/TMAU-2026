param(
    [Parameter(Mandatory = $true)]
    [string] $FiguresDirectory,
    [switch] $Saved
)

$ErrorActionPreference = 'Stop'
$logDirectory = Join-Path (Split-Path $PSScriptRoot -Parent) 'build'
[void] [IO.Directory]::CreateDirectory($logDirectory)
$logPath = Join-Path $logDirectory 'plot-window.log'
Set-Content -LiteralPath $logPath -Value "Starting viewer: $(Get-Date -Format o); figures: $FiguresDirectory"
trap {
    Add-Content -LiteralPath $logPath -Value ($_ | Out-String)
    Write-Error $_ -ErrorAction Continue
    exit 1
}
Add-Type -AssemblyName System.Drawing
Add-Type -AssemblyName System.Windows.Forms
[System.Windows.Forms.Application]::EnableVisualStyles()

$figures = [ordered]@{
    'Model 1.2' = 'model12.png'
    'Model 2.7' = 'model27.png'
    'Model 3.5' = 'model35.png'
    'PID tuning' = 'tuning.png'
    'Mode B' = 'modeb.png'
}

$window = New-Object System.Windows.Forms.Form
$images = New-Object 'System.Collections.Generic.List[System.Drawing.Image]'
try {
    $window.Text = if ($Saved) { 'Lab 03 - Transition plots (saved)' } else { 'Lab 03 - Transition plots' }
    $window.ShowInTaskbar = $true
    $window.TopMost = $true
    $window.StartPosition = [System.Windows.Forms.FormStartPosition]::CenterScreen
    $screen = [System.Windows.Forms.Screen]::PrimaryScreen.WorkingArea
    $window.ClientSize = New-Object System.Drawing.Size(
        [Math]::Min(1160, $screen.Width - 64),
        [Math]::Min(820, $screen.Height - 96)
    )

    $tabs = New-Object System.Windows.Forms.TabControl
    $tabs.Dock = [System.Windows.Forms.DockStyle]::Fill
    $window.Controls.Add($tabs)
    foreach ($figure in $figures.GetEnumerator()) {
        $path = Join-Path $FiguresDirectory $figure.Value
        Add-Content -LiteralPath $logPath -Value ('Loading: ' + $path)
        $sourceImage = [System.Drawing.Image]::FromFile($path)
        try {
            $image = New-Object System.Drawing.Bitmap($sourceImage)
        } finally {
            $sourceImage.Dispose()
        }
        $images.Add($image)
        $tab = New-Object System.Windows.Forms.TabPage($figure.Key)
        $picture = New-Object System.Windows.Forms.PictureBox
        $picture.Dock = [System.Windows.Forms.DockStyle]::Fill
        $picture.SizeMode = [System.Windows.Forms.PictureBoxSizeMode]::Zoom
        $picture.Image = $image
        $tab.Controls.Add($picture)
        $tabs.TabPages.Add($tab)
    }
    $window.Add_Shown({
        $this.WindowState = [System.Windows.Forms.FormWindowState]::Normal
        $this.Activate()
        $this.BringToFront()
        Add-Content -LiteralPath $logPath -Value 'SHOWN: five plot tabs'
        Write-Host 'Plot window opened. Close it to finish.'
        $this.TopMost = $false
    })
    [void] $window.ShowDialog()
    Add-Content -LiteralPath $logPath -Value 'CLOSED' 
} finally {
    $window.Dispose()
    foreach ($image in $images) { $image.Dispose() }
}
