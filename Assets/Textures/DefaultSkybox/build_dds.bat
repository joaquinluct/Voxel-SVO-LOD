@echo off
setlocal

set TEXASSEMBLE_PATH=C:\Users\joaqu\source\repos oficiales\DirectXTex-main\Texassemble\Bin\Desktop_2022_Win10\x64\Release\texassemble.exe
set INPUT_DIR=C:\Users\joaqu\source\repos\DirectX-Voxelado\Assets\Textures\DefaultSkybox\
set OUTPUT_DIR=C:\Users\joaqu\source\repos\DirectX-Voxelado\Assets\Skybox\
set OUTPUT_NAME=defaultSkybox.dds

"%TEXASSEMBLE_PATH%" cube ^
    -f R8G8B8A8_UNORM ^
    -y ^
    -o "%OUTPUT_DIR%%OUTPUT_NAME%" ^
    "%INPUT_DIR%clouds1_east.bmp" ^
    "%INPUT_DIR%clouds1_west.bmp" ^
    "%INPUT_DIR%clouds1_up.bmp" ^
    "%INPUT_DIR%clouds1_down.bmp" ^
    "%INPUT_DIR%clouds1_north.bmp" ^
    "%INPUT_DIR%clouds1_south.bmp"

endlocal
pause
