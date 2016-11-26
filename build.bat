@echo off
setlocal
JNI_INCLUDE=%JAVA_HOME%/include
JNI_INCLUDE_WIN32=%JAVA_HOME%/include/win32
JNI_LIBRARY=%JAVA_HOME%/jre/lib/amd64/server


cl /DHAS_JAVA_TREEVIEW /Feklc /I %JNI_INCLUDE% /I JNI_INCLUDE_WIN32 main.c jvm.lib /link /LIBPATH:%JNI_LIBRARY_PATH%

endlocal