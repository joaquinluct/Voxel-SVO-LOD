Add-Type -AssemblyName System.Drawing
$w = 256
$h = 256
$cell = 16
$radius = 6.0
$bmp = New-Object System.Drawing.Bitmap($w, $h, [System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
for ($y = 0; $y -lt $h; $y++) {
    for ($x = 0; $x -lt $w; $x++) {
        $cx = ([math]::Floor($x / $cell) * $cell) + ($cell / 2.0)
        $cy = ([math]::Floor($y / $cell) * $cell) + ($cell / 2.0)
        $dx = $x - $cx
        $dy = $y - $cy
        $dist = [math]::Sqrt($dx * $dx + $dy * $dy)
        # create a smooth radial falloff centered at radius
        $val = 0.5 + 0.5 * ($radius - $dist) / $radius
        if ($val -gt 1.0) { $val = 1.0 }
        if ($val -lt 0.0) { $val = 0.0 }
        $g = [byte]([math]::Round($val * 255.0))
        $color = [System.Drawing.Color]::FromArgb($g, $g, $g)
        $bmp.SetPixel($x, $y, $color)
    }
}
$dir = Join-Path -Path $PSScriptRoot -ChildPath "..\Assets\Textures\UI"
if (!(Test-Path $dir)) { New-Item -ItemType Directory -Path $dir | Out-Null }
$target = Join-Path -Path $dir -ChildPath "atlas_ui_sdf.png"
$bmp.Save($target, [System.Drawing.Imaging.ImageFormat]::Png)
$bmp.Dispose()
Write-Host "Generated $target"
