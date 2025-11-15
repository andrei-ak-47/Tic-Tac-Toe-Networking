@echo off
title 🎮 Tic-Tac-Toe LAN Party
echo ========================================
echo      TIC-TAC-TOE LAUNCHER
echo ========================================
echo.

echo 🔄 Starting HTTPServer (Results Page)...
start "HTTPServer" HTTPServer.exe

echo ⏳ Waiting for HTTPServer to start...
timeout /t 2 /nobreak >nul

echo 🎯 Starting Game Server...
start "Game Server" Server.exe

echo ⏳ Waiting for Game Server to start...
timeout /t 2 /nobreak >nul

echo 👤 Starting Player 1...
start "Player 1" P1.exe

echo 👤 Starting Player 2... 
start "Player 2" P2.exe

echo.
echo ========================================
echo 🚀 ALL COMPONENTS STARTED!
echo ========================================
echo.
echo 📊 View results at: http://localhost:8081
echo.
echo 💡 Close all windows to stop the game
echo.
pause