#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcap.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <net/ethernet.h>
#include <arpa/inet.h>

#include <mysql/mysql.h>

#include <pthread.h>

/*
gcc main.o capture.o -o main -lpcap -lmysqlclient -L/usr/lib/x86_64-linux-gnu -pthread
*/

int ipadd();
int netstat();
int capture();
int fwrule();

void *ipth(){
	ipadd();
	return 0;
}

void *neth(){
	netstat();
	return 0;
}

void *capth(void *interface){
	capture();
	return 0;
}

void *fwruleth(){
	fwrule();
	return 0;
}

int main(int argc, char *argv[]){
  pthread_t pthread;
	//pthread_create(&pthread, NULL, &ipth, NULL);
	pthread_create(&pthread, NULL, &neth, NULL);
	pthread_create(&pthread, NULL, &fwruleth, NULL);
	pthread_create(&pthread, NULL, &capth, (void*)argv[1]);
	pthread_join(pthread, NULL);

	return 0;
}
