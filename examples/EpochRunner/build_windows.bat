@echo off
setlocal
if "%VCPKG_ROOT%"=="" (
  echo VCPKG_ROOT is not set.
  exit /b 1
)
where glslc >nul 2>nul
if errorlevel 1 if "%VULKAN_SDK%"=="" (
  echo Install the Vulkan SDK or add glslc to PATH.
  exit /b 1
)
cmake --preset windows-release || exit /b 1
cmake --build --preset windows-release || exit /b 1
ctest --test-dir build/windows-release --output-on-failure || exit /b 1
echo Built: build\windows-release\EpochRunner.exe
