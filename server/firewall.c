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

int fwrule(void){
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
	addr.sin_port = htons(12349);
	addr.sin_addr.s_addr = inet_addr("192.168.33.1");
	len = sizeof(client);
	char command[50] = "iptables -L";
	FILE *fp;
	char buf[1024];
	char sendmsg[1024];
	while(1){
		sleep(1);
		fp = popen(command, "r");
		while(fgets(buf, sizeof(buf), fp) != NULL){
			//printf("%s\n", buf);
			strcat(sendmsg, buf);
		}
		printf("%s\n", sendmsg);
		sendto(sock0, sendmsg, sizeof(sendmsg), 0, (struct sockaddr *)&addr, sizeof(addr));
		memset(sendmsg, 0x00, sizeof(sendmsg));
		pclose(fp);
	}
	close(sock0);
	return 0;
}
