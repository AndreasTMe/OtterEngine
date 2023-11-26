@echo off
REM Used in Debug only
REM Compile shaders to SPIR-V

echo "Building in %1 mode for the %2 platform..."

set IN_DIR="%cd%"
set OUT_DIR="%cd%\bin\%1-%2\OtterEngine"

if not exist %OUT_DIR%\Assets\Shaders mkdir %OUT_DIR%\Assets\Shaders
if not exist %OUT_DIR%\Assets\Textures mkdir %OUT_DIR%\Assets\Textures

xcopy /s %IN_DIR%\Assets\Textures %OUT_DIR%\Assets\Textures /Y

echo "Compiling shaders..."

set IN_SHADER_DIR=%IN_DIR%\Assets\Shaders
set OUT_SHADER_DIR=%OUT_DIR%\Assets\Shaders

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