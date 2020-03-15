@echo off
IF NOT EXIST build mkdir build
pushd build
cl /EHsc /Zi /I..\src\include  ..\src\emblec.cpp ..\src\frontend\ast_routines.cpp ..\src\frontend\lexer.cpp ..\src\frontend\parser.cpp ..\src\frontend\typecheck.cpp ..\src\codegen\codegen.cpp ..\src\ir\ir.cpp ..\src\backend\x86asm.cpp
popd
copy build\emblec.exe emblec.exe
IF "%1"=="run" emblec.exe -d -r test.e