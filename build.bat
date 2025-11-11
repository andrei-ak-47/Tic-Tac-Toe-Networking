@echo off
title 🔨 Compiling Tic-Tac-Toe
echo ========================================
echo    TIC-TAC-TOE COMPILATION
echo ========================================
echo.

echo 🖥️  Compiling Server...
g++ -o Server.exe Server.cpp -lws2_32
if %errorlevel% neq 0 (
    echo ❌ SERVER COMPILATION FAILED!
    pause
    exit /b
)

echo 👤 Compiling Player 1...
g++ -o P1.exe P1.cpp -lws2_32
if %errorlevel% neq 0 (
    echo ❌ PLAYER 1 COMPILATION FAILED!
    pause
    exit /b
)

echo 👤 Compiling Player 2...
g++ -o P2.exe P2.cpp -lws2_32
if %errorlevel% neq 0 (
    echo ❌ PLAYER 2 COMPILATION FAILED!
    pause
    exit /b
)

echo.
echo ========================================
echo ✅ ALL FILES COMPILED SUCCESSFULLY!
echo ========================================
echo.
echo 📁 Files created:
echo    Server.exe - Game server
echo    P1.exe - Player 1 client  
echo    P2.exe - Player 2 client
echo.
echo 🎮 Run run_game.bat to play!
echo.
pause