@echo off
chcp 1251 > nul

echo ==========================================
echo      0. НАСТРОЙКА ОКРУЖЕНИЯ
echo ==========================================

:: Автопоиск VS 2022
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat" (
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"
)

echo.
echo ==========================================
echo      1. ЗАПУСК ГЕНЕРАТОРА
echo ==========================================

"E:\University\Сем_3\КПО\BPV_2025\Debug\BPV_2025.exe" -in:in.txt -out:prog.asm -log:log.txt

if not exist prog.asm (
    echo [ERROR] prog.asm не создан!
    pause
    exit /b
)

echo.
echo ==========================================
echo      2. АССЕМБЛИРОВАНИЕ (MASM)
echo ==========================================

del prog.obj 2>nul

ml /c /coff /Zi prog.asm

if errorlevel 1 (
    echo [ERROR] Ошибка в ASM коде!
    pause
    exit /b
)

echo.
echo ==========================================
echo      3. ЛИНКОВКА (DEBUG)
echo ==========================================

:: ИСПРАВЛЕНИЕ: Используем библиотеки с суффиксом 'd' (Debug), 
:: так как твоя STL.lib собрана в Debug.
:: ucrt.lib (вместо libucrt), msvcrtd.lib, vcruntimed.lib

link /SUBSYSTEM:CONSOLE /OPT:NOREF prog.obj STL.lib msvcrtd.lib vcruntimed.lib ucrtd.lib legacy_stdio_definitions.lib user32.lib kernel32.lib

if errorlevel 1 (
    echo [ERROR] Ошибка линковки!
    echo Попробуй пересобрать STL.lib в Visual Studio в режиме RELEASE.
    pause
    exit /b
)

echo.
echo ==========================================
echo      4. ЗАПУСК ПРОГРАММЫ (ASM EXE)
echo ==========================================
echo.

prog.exe

echo.
echo ==========================================
pause