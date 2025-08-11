@echo off
REM Copia todos los archivos y carpetas de x64\Debug al directorio raíz de la solución, sobrescribiendo sin preguntar
xcopy /E /Y /Q x64\Debug\* .\
