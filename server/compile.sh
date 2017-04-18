#!/bin/bash
gcc -c main.c
gcc -c capture.c
gcc -c netstat.c
gcc -c ipadd.c
gcc -c firewall.c 

gcc main.o capture.o netstat.o ipadd.o firewall.o -o dev -lpcap -lmysqlclient -L/usr/lib/x86_64-linux-gnu -pthread
