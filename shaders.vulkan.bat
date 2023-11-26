@echo off
REM Used in Debug only
REM Compile shaders to SPIR-V

if not exist "%cd%\bin\Debug-Windows\OtterEngine\Assets\Shaders" mkdir "%cd%\bin\Debug-Windows\OtterEngine\Assets\Shaders"
if not exist "%cd%\bin\Debug-Windows\OtterEngine\Assets\Textures" mkdir "%cd%\bin\Debug-Windows\OtterEngine\Assets\Textures"

xcopy /s "%cd%\Assets\Textures" "%cd%\bin\Debug-Windows\OtterEngine\Assets\Textures" /Y

echo "Compiling shaders..."

set IN_SHADER_DIR="%cd%\Assets\Shaders"
set OUT_SHADER_DIR="%cd%\bin\Debug-Windows\OtterEngine\Assets\Shaders"

for %%f in (%IN_SHADER_DIR%\*.vert %IN_SHADER_DIR%\*.frag) do (
    if "%%~xf"==".vert" (
        echo "Compiling vertex shader: %%f"
        "%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=vert "%%f" -o %OUT_SHADER_DIR%\%%~nxf.spv
    ) else if "%%~xf"==".frag" (
        echo "Compiling fragment shader: %%f"
        "%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=frag "%%f" -o %OUT_SHADER_DIR%\%%~nxf.spv
    )

    if %ERRORLEVEL% NEQ 0 (
        echo Error compiling "%%f": %ERRORLEVEL%
        exit /b %ERRORLEVEL%
    )
)

echo "Done."