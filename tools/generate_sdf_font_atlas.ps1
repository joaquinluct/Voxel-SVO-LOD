Add-Type -AssemblyName System.Drawing

# Parameters
$chars = 32..126
$cell = 32
$cols = 16
$rows = [math]::Ceiling(($chars.Count) / $cols)
$w = $cols * $cell
$h = $rows * $cell
$fontName = 'Consolas'
$fontSize = 20
$spread = 8.0 # pixels for SDF normalization

# Create atlas bitmap
$atlas = New-Object System.Drawing.Bitmap($w, $h, [System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
$gAtlas = [System.Drawing.Graphics]::FromImage($atlas)
$black = [System.Drawing.Color]::FromArgb(0,0,0)
$white = [System.Drawing.Color]::FromArgb(255,255,255)
$gAtlas.Clear($black)
$gAtlas.TextRenderingHint = 'AntiAliasGridFit'

# Prepare per-cell rendering and metrics
$metrics = @{}
$index = 0
for ($code = 32; $code -le 126; $code++) {
    $c = [char]$code
    $col = $index % $cols
    $row = [math]::Floor($index / $cols)
    $cellX = $col * $cell
    $cellY = $row * $cell

    # render glyph into temporary bitmap with padding
    $bmp = New-Object System.Drawing.Bitmap($cell, $cell, [System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.Clear($black)
    $g.TextRenderingHint = 'AntiAliasGridFit'

    $font = New-Object System.Drawing.Font($fontName, $fontSize, [System.Drawing.FontStyle]::Regular, [System.Drawing.GraphicsUnit]::Pixel)
    $sf = New-Object System.Drawing.StringFormat
    $sf.Alignment = 'Center'
    $sf.LineAlignment = 'Center'

    # Draw string centered
    $rect = New-Object System.Drawing.RectangleF(0,0,$cell,$cell)
    $brush = [System.Drawing.Brushes]::White
    $g.DrawString($c, $font, $brush, $rect, $sf)
    $g.Dispose()

    # Build binary mask (foreground where pixel luminance > threshold)
    $mask = New-Object 'int[,]' $cell, $cell
    for ($yy=0; $yy -lt $cell; $yy++) {
        for ($xx=0; $xx -lt $cell; $xx++) {
            $colp = $bmp.GetPixel($xx,$yy)
            # luminance
            $lum = (0.299*$colp.R + 0.587*$colp.G + 0.114*$colp.B)
            if ($lum -gt 10) { $mask[$xx,$yy] = 1 } else { $mask[$xx,$yy] = 0 }
        }
    }

    # Compute SDF per pixel within cell
    $sdf = New-Object 'double[,]' $cell, $cell
    for ($yy=0; $yy -lt $cell; $yy++) {
        for ($xx=0; $xx -lt $cell; $xx++) {
            $inside = $mask[$xx,$yy] -eq 1
            $best = [double]::MaxValue
            for ($y2=0; $y2 -lt $cell; $y2++) {
                for ($x2=0; $x2 -lt $cell; $x2++) {
                    if (($mask[$x2,$y2] -eq 1) -ne $inside) {
                        $dx = $x2 - $xx
                        $dy = $y2 - $yy
                        $d = [math]::Sqrt($dx*$dx + $dy*$dy)
                        if ($d -lt $best) { $best = $d }
                    }
                }
            }
            if ($best -eq [double]::MaxValue) { $best = 0 }
            if ($inside) { $sdf[$xx,$yy] = $best } else { $sdf[$xx,$yy] = -$best }
        }
    }

    # Write into atlas normalized to 0..255 with center 0.5
    for ($y=0; $y -lt $cell; $y++) {
        for ($x=0; $x -lt $cell; $x++) {
            $d = $sdf[$x,$y]
            $v = 0.5 + ($d / $spread) * 0.5
            if ($v -lt 0) { $v = 0 }
            if ($v -gt 1) { $v = 1 }
            $byte = [byte]([math]::Round($v * 255))
            $clr = [System.Drawing.Color]::FromArgb($byte,$byte,$byte)
            $atlas.SetPixel($cellX + $x, $cellY + $y, $clr)
        }
    }

    # measure advance as measured string width
    $measBmp = New-Object System.Drawing.Bitmap(1,1)
    $measG = [System.Drawing.Graphics]::FromImage($measBmp)
    $sz = $measG.MeasureString($c, $font)
    $measG.Dispose(); $measBmp.Dispose()
    $advance = [math]::Round($sz.Width)

    $metrics[$code] = @{ x = $cellX; y = $cellY; w = $cell; h = $cell; advance = $advance }

    $bmp.Dispose()
    $index++
}

# Save atlas
$dir = Join-Path -Path $PSScriptRoot -ChildPath "..\Assets\Textures\UI"
if (!(Test-Path $dir)) { New-Item -ItemType Directory -Path $dir | Out-Null }
$atlasPath = Join-Path $dir "atlas_ui_sdf.png"
$atlas.Save($atlasPath, [System.Drawing.Imaging.ImageFormat]::Png)
$atlas.Dispose()
$gAtlas.Dispose()
Write-Host "Generated atlas at $atlasPath"

# Emit atlas_ui.json
$json = @{ textureSize = $w; cellSize = $cell; columns = $cols; rows = $rows; glyphs = @{} }
foreach ($k in $metrics.Keys) { $json.glyphs.Add([string]$k, $metrics[$k]) }
$out = ConvertTo-Json $json -Depth 10
$outPath = Join-Path $dir "atlas_ui.json"
Set-Content -Path $outPath -Value $out -Encoding UTF8
Write-Host "Wrote metrics to $outPath"
