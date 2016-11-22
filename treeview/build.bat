@echo off
copy TreeView.java.c TreeView.java
javac TreeView.java
REM javah TreeView

REM gcc -o treeview -D -I"C:/Program Files/Java/jdk1.8.0_111/include" -I"C:/Program Files/Java/jdk1.8.0_111/include/win32" -L"C:/Program Files/Java/jdk1.8.0_111/lib" treeview.c -ljvm

cl /Fetreeview /I "C:/Program Files/Java/jdk1.8.0_111/include" /I "C:/Program Files/Java/jdk1.8.0_111/include/win32" treeview.c jvm.lib /link /LIBPATH:"C:/Program Files/Java/jdk1.8.0_111/lib"