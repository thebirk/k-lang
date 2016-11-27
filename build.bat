@echo off
setlocal
set JNI_INCLUDE="%JAVA_HOME%/include"
set JNI_INCLUDE_WIN32="%JAVA_HOME%/include/win32"
set JNI_LIBRARY="%JAVA_HOME%/lib"
set PATH=%JAVA_HOME/jre/lib/amd64/server%;%PATH%

REM set LLVM_LIB="C:\Program Files\LLVM\lib"
REM set LLVM_INCLUDE="C:\Program Files\LLVM\include"
REM set LLVM_LIBS=LLVMAsmParser.lib LLVMAsmPrinter.lib LLVMBitReader.lib LLVMBitWriter.lib LLVMCodeGen.lib LLVMCore.lib LLVMCoverage.lib LLVMExecutionEngine.lib

REM Doesnt work until llvm-config qutoes paths
REM have to set these manually for now
set LLVM_LDFLAGS=-LIBPATH:"C:\Program Files\LLVM\lib"
set LLVM_CFLAGS=-"IC:\Program Files\LLVM\include" /DWIN32 /D_WINDOWS /W4 -wd4141 -wd4146 -wd4180 -wd4244 -wd4258 -wd4267 -wd4291 -wd4345 -wd4351 -wd4355 -wd4456 -wd4457 -wd4458 -wd4459 -wd4503 -wd4624 -wd4722 -wd4800 -wd4100 -wd4127 -wd4512 -wd4505 -wd4610 -wd4510 -wd4702 -wd4245 -wd4706 -wd4310 -wd4701 -wd4703 -wd4389 -wd4611 -wd4805 -wd4204 -wd4577 -wd4091 -wd4592 -wd4319 -wd4324 -w14062 -we4238 /Zc:inline /Zc:strictStrings /Oi /Zc:sizedDealloc- /D_DEBUG /MDd /Zi /Ob0 /Od /RTC1 /MP -D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_NONSTDC_NO_WARNINGS -D_SCL_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS -DUNICODE -D_UNICODE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS
REM for /f "tokens=*" %%x IN ('"llvm-config --ldflags core"') DO set LLVM_LDFLAGS=%%x
REM for /f "tokens=*" %%x IN ('"llvm-config --cflags core"') DO set LLVM_CFLAGS=%%x
for /f "tokens=*" %%x IN ('"llvm-config --libnames core engine analysis interpreter"') DO set LLVM_LIBS=%%x

cl /Zi /DHAS_JAVA_TREEVIEW /Feklc %LLVM_CFLAGS% /I %JNI_INCLUDE% /I %JNI_INCLUDE_WIN32% main.c jvm.lib %LLVM_LIBS% /link /LIBPATH:%JNI_LIBRARY% %LLVM_LDFLAGS%

endlocal