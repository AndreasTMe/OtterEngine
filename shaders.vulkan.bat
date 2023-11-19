REM Used in Debug only
REM Compile shaders to SPIR-V

if not exist "%cd%\bin\Debug-Windows\OtterEngine\Assets\Shaders" mkdir "%cd%\bin\Debug-Windows\OtterEngine\Assets\Shaders"

echo "Compiling shaders..."

"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=vert Assets\Shaders\default.glsl.vert -o bin\Debug-Windows\OtterEngine\Assets\Shaders\default.glsl.vert.spv
if %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=frag Assets\Shaders\default.glsl.frag -o bin\Debug-Windows\OtterEngine\Assets\Shaders\default.glsl.frag.spv
if %ERRORLEVEL% NEQ 0 (echo Error: %ERRORLEVEL% && exit)

echo "Done."