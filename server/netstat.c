#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

int netstat(void){
	int sock0;
	struct sockaddr_in addr;
	struct sockaddr_in client;
	int len;
	int sock;

	sock0 = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock0 < 0){
		fprintf(stderr, "socket error\n");
		exit(-1);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(12347);
	addr.sin_addr.s_addr = inet_addr("192.168.33.1");
	len = sizeof(client);
	char *proto, *recv, *send, *inadd, *outadd, *type, *pid, *lport;
	char command[50] = "netstat -tlnp";
	FILE *fp;
	char buf[1024];
	int i;
	char portbuf[1024] = "";
	char portbuf2[1024] = "";
	while(1){
		sleep(1);
		i = 0;
		fp = popen(command, "r");
		while(fgets(buf, sizeof(buf), fp) != NULL){
			//printf("%s\n", buf);
			if(i > 1){
				proto = strtok(buf, " ");
				recv = strtok(NULL, " ");
				send = strtok(NULL, " ");
				inadd = strtok(NULL, " ");
	
				inadd = strtok(inadd, ":");
				lport = strtok(NULL, ":");
			 	if(lport == NULL){
					strcat(portbuf,inadd);
				}else{
					strcat(portbuf, lport);
				}
				strcat(portbuf,",");
			}
			i++;
		}
		if(strcmp(portbuf,portbuf2)){
			sendto(sock0, portbuf, sizeof(portbuf), 0, (struct sockaddr *)&addr, sizeof(addr));
			strcpy(portbuf2, portbuf);
			printf("%s\n", portbuf2);
		}
		memset(portbuf, 0x00, sizeof(portbuf));
		pclose(fp);
	}
	close(sock0);
	return 0;
}
