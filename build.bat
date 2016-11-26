@echo off
setlocal
set JNI_INCLUDE="%JAVA_HOME%/include"
set JNI_INCLUDE_WIN32="%JAVA_HOME%/include/win32"
set JNI_LIBRARY="%JAVA_HOME%/lib"
set PATH=%JAVA_HOME/jre/lib/amd64/server%;%PATH%

cl /DHAS_JAVA_TREEVIEW /Feklc /I %JNI_INCLUDE% /I %JNI_INCLUDE_WIN32% main.c jvm.lib /link /LIBPATH:%JNI_LIBRARY%

endlocal