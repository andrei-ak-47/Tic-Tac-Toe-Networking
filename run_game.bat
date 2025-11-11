@echo off
title Tic-Tac-Toe Game Launcher
echo ================================
echo    TIC-TAC-TOE MULTIPLAYER
echo ================================
echo.

echo Step 1: Compiling all files...
echo.
g++ -o Server.exe Server.cpp -lws2_32
if %errorlevel% neq 0 (
    echo ERROR: Server compilation failed!
    pause
    exit /b
)

g++ -o P1.exe P1.cpp -lws2_32
if %errorlevel% neq 0 (
    echo ERROR: P1 compilation failed!
    pause
    exit /b
)

g++ -o P2.exe P2.cpp -lws2_32
if %errorlevel% neq 0 (
    echo ERROR: P2 compilation failed!
    pause
    exit /b
)

echo.
echo ✅ All files compiled successfully!
echo.

echo Step 2: Starting game...
echo.
echo 🎮 Starting Server and Players...
echo ⚠️  Make sure to run them in ORDER:
echo.

timeout 2 >nul

echo 🖥️  Starting SERVER (run this FIRST)...
start "🎮 Tic-Tac-Toe SERVER" cmd /k "cd /d %~dp0 && echo ✅ SERVER RUNNING - Wait for players... && Server.exe"

timeout 3 >nul

echo 👤 Starting PLAYER 1 (run this SECOND)...
start "👤 Player 1" cmd /k "cd /d %~dp0 && echo ✅ PLAYER 1 CONNECTING... && P1.exe"

timeout 2 >nul

echo 👤 Starting PLAYER 2 (run this THIRD)...
start "👤 Player 2" cmd /k "cd /d %~dp0 && echo ✅ PLAYER 2 CONNECTING... && P2.exe"

echo.
echo ====================================
echo 🎉 ALL WINDOWS LAUNCHED!
echo ====================================
echo.
echo INSTRUCTIONS:
echo 1. Wait for SERVER to say "Both Players Connected"
echo 2. Then PLAYERS can start making moves
echo 3. Have fun! 🎯
echo.
echo Press any key to close this launcher...
pause >nul