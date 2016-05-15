@echo off

rmdir /s /q "Distribution PC"

md "Distribution PC"

cd "Distribution PC"

md BIN
md LIB
md INCLUDE

copy "..\Compiler\cd\Compiler.exe" BIN
copy "..\IDLMake\Release\IDLMake.exe" BIN
copy "..\Class View\Debug\Class View.exe" BIN

copy "..\Common\Library\*.lib" LIB
copy "..\Platform Specific\PCWindows\Library\*.lib" LIB
copy "..\Virtual Machine\Library\Virtual Machine.lib" LIB\VM.lib

copy "..\Common\Include\*.h" INCLUDE
copy "..\Platform Specific\Include\*.h" INCLUDE
copy "..\Virtual Machine\Include\iVirtualMachine.h" INCLUDE

copy ..\lib.h INCLUDE

cd ..
