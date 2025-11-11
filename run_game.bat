@echo off
title 🎮 Tic-Tac-Toe Launcher
echo ========================================
echo    TIC-TAC-TOE MULTIPLAYER LAUNCHER
echo ========================================
echo.

echo 🔍 Checking if files exist...
if not exist "Server.exe" (
    echo ❌ Server.exe not found! Run compile.bat first.
    pause
    exit /b
)
if not exist "P1.exe" (
    echo ❌ P1.exe not found! Run compile.bat first.
    pause
    exit /b
)
if not exist "P2.exe" (
    echo ❌ P2.exe not found! Run compile.bat first.
    pause
    exit /b
)

echo ✅ All game files found!
echo.
echo 🚀 Launching game...
echo.

echo 1. Starting SERVER (run this FIRST)...
timeout 1 >nul
start "🎮 Tic-Tac-Toe SERVER" cmd /k "echo 🖥️  SERVER RUNNING - Waiting for players... && echo ⏳ Please wait for 'Both Players Connected' message && echo. && Server.exe"

echo.
echo 2. Starting PLAYER 1 in 3 seconds...
timeout 3 >nul
start "👤 Player 1" cmd /k "echo 👤 PLAYER 1 CONNECTING... && P1.exe"

echo.
echo 3. Starting PLAYER 2 in 3 seconds...  
timeout 3 >nul
start "👤 Player 2" cmd /k "echo 👤 PLAYER 2 CONNECTING... && P2.exe"

echo.
echo ========================================
echo 🎉 GAME LAUNCHED!
echo ========================================
echo.
echo 📋 INSTRUCTIONS:
echo 1. Wait for SERVER to say "Both Players Connected"
echo 2. Players take turns entering moves (1-9)
echo 3. First player to get 3 in a row wins!
echo.
echo 🎯 TIP: Make sure all 3 windows are visible
echo.
echo Press any key to close this launcher...
pause >nul