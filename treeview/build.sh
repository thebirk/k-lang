#!/bin/bash
cp TreeView.java.c TreeView.java
javac TreeView.java
javah TreeView

gcc -o treeview -I/usr/lib/jvm/java-7-openjdk-amd64/include -L/usr/lib/jvm/java-7-openjdk-amd64/jre/lib/amd64/server treeview.c -ljvm