@echo off
REM Generate lexer
flex flex.l
if errorlevel 1 (
    echo Flex failed.
    exit /b 1
)

REM Compile
gcc lex.yy.c -o analyzer.exe
if errorlevel 1 (
    echo Compilation failed.
    exit /b 1
)

REM Run analyzer (pass any command-line args through)
analyzer.exe %*
