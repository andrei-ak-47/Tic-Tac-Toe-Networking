@echo off
title Tic-Tac-Toe LAN Multiplayer Launcher
echo ========================================
echo    TIC-TAC-TOE LAN MULTIPLAYER LAUNCHER
echo ========================================
echo.

echo Step 1: Finding your network IP...
for /f "tokens=2 delims=:" %%i in ('ipconfig ^| findstr "IPv4"') do (
    set SERVER_IP=%%i
    goto :IPFound
)
:IPFound
set SERVER_IP=%SERVER_IP: =%
echo Server IP Address: %SERVER_IP%
echo.

echo Step 2: Compiling files...
echo Compiling Server...
g++ -o Server.exe Server.cpp -lws2_32
if %errorlevel% neq 0 (
    echo ERROR: Server compilation failed!
    pause
    exit /b
)

echo Compiling Client...
g++ -o Client.exe Client.cpp -lws2_32
if %errorlevel% neq 0 (
    echo ERROR: Client compilation failed!
    pause
    exit /b
)

echo.
echo Step 3: Starting game...
echo.
echo 🖥️  Starting SERVER (run this on the host computer)...
start "🎮 Tic-Tac-Toe SERVER" cmd /k "cd /d %~dp0 && echo ✅ SERVER RUNNING - IP: %SERVER_IP% && echo Waiting for players... && Server.exe"

timeout 3 >nul

echo.
echo 👤 Starting CLIENT 1 (use this or share IP with other players)...
start "👤 Player 1" cmd /k "cd /d %~dp0 && echo ✅ Connect using IP: %SERVER_IP% && echo Running client... && Client.exe %SERVER_IP%"

timeout 2 >nul

echo 👤 Starting CLIENT 2 (second player on this computer)...
start "👤 Player 2" cmd /k "cd /d %~dp0 && echo ✅ Connected to: %SERVER_IP% && echo Running client... && Client.exe %SERVER_IP%"

echo.
echo ========================================
echo 🎮 GAME LAUNCHED!
echo ========================================
echo.
echo TO PLAY ACROSS COMPUTERS:
echo 1. Share this IP with other players: %SERVER_IP%
echo 2. On other computers, run: Client.exe %SERVER_IP%
echo 3. Or use the pre-compiled Client.exe with the IP
echo.
echo Press any key to close this launcher...
pause >nul