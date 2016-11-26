#!/bin/bash
# K language compiler - Build script
JNI_INCLUDE=$JAVA_HOME/include
JNI_INCLUDE_LINUX=$JAVA_HOME/include/linux
JNI_LIBRARY=$JAVA_HOME/jre/lib/amd64/server

gcc -Wreturn-type -DHAS_JAVA_TREEVIEW -o klc -g main.c -ljvm -I$JNI_INCLUDE -I$JNI_INCLUDE_LINUX -L$JNI_LIBRARY
echo Build complete!