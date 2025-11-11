@echo off
echo Compiling Tic-Tac-Toe...
echo.
echo Compiling Server...
g++ -o Server.exe Server.cpp -lws2_32
if %errorlevel% neq 0 (
    echo ❌ Server compilation failed!
    pause
    exit /b
)

echo Compiling Client...
g++ -o Client.exe Client.cpp -lws2_32
if %errorlevel% neq 0 (
    echo ❌ Client compilation failed!
    pause
    exit /b
)

echo.
echo ✅ Compilation successful!
echo Files ready: Server.exe, Client.exe
echo.
pause