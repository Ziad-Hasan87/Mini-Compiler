@echo off

echo ===============================
echo Generating parser with Bison...
echo ===============================

bison -d parser.y
if errorlevel 1 (
    echo Bison failed.
    exit /b 1
)

echo ===============================
echo Generating lexer with Flex...
echo ===============================

flex flex.l
if errorlevel 1 (
    echo Flex failed.
    exit /b 1
)

echo ===============================
echo Compiling with GCC...
echo ===============================

gcc parser.tab.c lex.yy.c -o compiler.exe
if errorlevel 1 (
    echo Compilation failed.
    exit /b 1
)

echo ===============================
echo Build successful!
echo ===============================

compiler.exe %*
