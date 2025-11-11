@echo off
echo Compiling Tic-Tac-Toe Server and Clients...
echo.

echo Compiling Server...
g++ -o Server.exe Server.cpp -lws2_32
if %errorlevel% neq 0 (
    echo ERROR: Server compilation failed!
    pause
    exit /b
)

echo Compiling Player 1...
g++ -o P1.exe P1.cpp -lws2_32
if %errorlevel% neq 0 (
    echo ERROR: P1 compilation failed!
    pause
    exit /b
)

echo Compiling Player 2...
g++ -o P2.exe P2.cpp -lws2_32
if %errorlevel% neq 0 (
    echo ERROR: P2 compilation failed!
    pause
    exit /b
)

echo.
echo ========================================
echo ALL FILES COMPILED SUCCESSFULLY! 🎉
echo ========================================
echo.
echo To play the game, run these in order:
echo 1. First:    Server.exe
echo 2. Then:     P1.exe
echo 3. Then:     P2.exe
echo.
echo Press any key to close...
pause >nul