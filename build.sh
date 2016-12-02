#!/bin/bash
# K language compiler - Build script
JNI_INCLUDE=$JAVA_HOME/include
JNI_INCLUDE_LINUX=$JAVA_HOME/include/linux
JNI_LIBRARY=$JAVA_HOME/jre/lib/amd64/server

LLVM_COMPS='core engine analysis interpreter native'
LLVM_LDFLAGS=`llvm-config --ldflags`
LLVM_CFLAGS=`llvm-config --cflags`
LLVM_LIBS=`llvm-config --system-libs --libs $LLVM_COMPS`
LLVM_LIBS_NOT_FOUND='-lffi'

gcc -DHAS_JAVA_TREEVIEW -I$JNI_INCLUDE -I$JNI_INCLUDE_LINUX  $LLVM_CFLAGS -o klc.o -g -c src/main.c
g++ -L$JNI_LIBRARY $LLVM_LDFLAGS -o klc klc.o -ljvm $LLVM_LIBS $LLVM_LIBS_NOT_FOUND

# gcc -DHAS_JAVA_TREEVIEW -I$JNI_INCLUDE -I$JNI_INCLUDE_LINUX $LLVM_CFLAGS $LLVM_LDFLAGS -L$JNI_LIBRARY -g src/main.c -ljvm $LLVM_LIBS -o klc

echo Build complete!