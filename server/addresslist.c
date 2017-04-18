#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <mysql/mysql.h>

int main(){
	MYSQL *conn = NULL;
	MYSQL_RES *resp = NULL;
	MYSQL_ROW row;

	char sql_str[255];
	char *sql_serv = "localhost";
	char *user = "root";
	char *passwd = "passwd";
	char *db_name = "NetworkVisual";
	char *hostip = "10.0.3.15";

	//socket setting
	conn = mysql_init(NULL);
	mysql_real_connect(conn, sql_serv, user, passwd, db_name, 0, NULL, 0);
	int sock0;
	struct sockaddr_in addr;
	struct sockaddr_in client;
	int len;

	char before_ip[10][16]; //更新用格納変数
	memset(&before_ip[0], 0x00, sizeof(before_ip));

	sock0 = socket(AF_INET, SOCK_DGRAM, 0);

	if(sock0 < 0){
		printf("socket error\n");
		exit(-1);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(12346);
	addr.sin_addr.s_addr = inet_addr("192.168.33.1");
	sendto(sock0, "hogehoge", 9, 0, (struct sockaddr*)&addr, sizeof(addr));
	int count = 0;
	int judge;
	char dbbuf[1024] = "";
	char slice[2];
	char comnum[6];
	while(1){
		sleep(1);
		snprintf(&sql_str[0], sizeof(sql_str)-1, "select * from packetinfo order by count desc");
		if(mysql_query(conn, &sql_str[0])){
			fprintf(stderr, "error %s :%s\n", mysql_error(conn), sql_str);
			mysql_close(conn);
			exit(-1);
		}
		memset(&sql_str[0], 0x00, sizeof(sql_str));
		resp = mysql_use_result(conn);
		//更新して変更があった場合にソケット通信する
		judge = 0;
		memset(dbbuf, 0x00, sizeof(dbbuf));
		while((row = mysql_fetch_row(resp)) != NULL){
				/*if(strcmp(before_ip[count],row[0]) != 0){
					judge = 1;
				}
				if(judge == 1){
					strcat(dbbuf, row[0]);
					sprintf(slice, ",");
					strcat(dbbuf, slice);
				}
				//before_ip[count] = row[0];
				strcpy(before_ip[count], row[0]);
				//printf("%s\n", before_ip[count]);
				count++;*/
				strcat(dbbuf, row[0]);
				sprintf(slice, ",");
				strcat(dbbuf, slice);
		}
		sendto(sock0, dbbuf, sizeof(dbbuf), 0, (struct sockaddr*)&addr, sizeof(addr));
		count = 0;
		/*if(judge == 1){
			while((row = mysql_fetch_row(resp)) != NULL){
				//before_ip[count] = row[0];
				strcpy(before_ip[count], row[0]);
				strcat(dbbuf, row[0]);
				//sprintf(slice , ":");
				//strcat(dbbuf, slice);
				//sprintf(comnum, "%s", row[1]);
				//strcat(dbbuf, comnum);
				//strcat(dbbuf, row[1]);
				sprintf(slice , ",");
				strcat(dbbuf, slice);
				count++;
			}
			//write(sock, dbbuf, 200);
			
			printf("%s\n", dbbuf);
			sendto(sock0, dbbuf, sizeof(dbbuf), 0, (struct sockaddr*)&addr, sizeof(addr));
		}*/
		//judge = 0;
		//printf("%s:通信数%s\n", row[0], row[1]);
		//resp = mysql_use_result(conn);
		mysql_free_result(resp);
	}
	close(sock0);
	//close(sock);
	mysql_close(conn);
	return 0;
}

