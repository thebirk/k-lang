#!/bin/bash
# K language compiler - Build script
gcc -DHAS_JAVA_TREEVIEW -o klc -g main.c -ljvm -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -L/usr/lib/jvm/java-8-openjdk-amd64/jre/lib/amd64/server
echo Build complete!
