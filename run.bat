@echo on

:: Copy the assets into the x64
xcopy /y /e "Assets" "build\Debug\Assets\"
cd /d "%~dp0build\Debug"
