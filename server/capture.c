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

/*
gcc -o udpacket udpacket.c -lpcap -lmysqlclient -L/usr/lib/x86_64-linux-gnu
*/

static void usage(char *prog) {

    fprintf(stderr, "Usage: %s <device>\n", prog);
    exit(EXIT_FAILURE);
}

int capture(void) {
    /*カウンタの設定*/
    int MAX_COUNT = 1;
		int sendcount = 0;
		/*mysqlの設定*/
    MYSQL *conn = NULL;
    MYSQL_RES *resp = NULL;
    MYSQL_ROW row;

    char sql_str[255];
    char *sql_serv = "localhost";
    char *user = "root";
    char *passwd = "passwd";
    char *db_name = "NetworkVisual";
   
    //データベースの初期化
    memset(&sql_str[0], 0x00, sizeof(sql_str));
    conn = mysql_init(NULL);
    mysql_real_connect(conn, sql_serv, user, passwd, db_name, 0, NULL, 0);
    /* ソケットの設定*/
		int sock0;
    struct sockaddr_in addr;
    struct sockaddr_in client;
    int len;
    int sock;
    char buff[200];
    char *hostip;
    hostip = "192.168.33.1";

    sock0 = socket(AF_INET, SOCK_DGRAM, 0);

    if(sock0 < 0){
      printf("socket error\n");
      return 1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = inet_addr("192.168.33.1");

    len = sizeof(client);

    /*パケットの設定*/
    pcap_t *handle;
    const unsigned char *packet;
    char *dev;
    char errbuf[PCAP_ERRBUF_SIZE];
    struct pcap_pkthdr header;
    struct bpf_program fp;
    struct ip *ip;
    struct tcphdr * tcp;
    char *p;
    char *tcp_p;
    bpf_u_int32 net;
    bpf_u_int32 mask;
    //char filter_exp[] = "not host 192.168.11.5";
		char filter_exp[] = "not port 12345 and not port 12346 and not port 12347 and not port 12348";
    int packetcount = 0;
    char *beforeip_src;
    int beforetcp_sport;
		char comma[2] = ",";
		char buffer[10];
		char packetinfo[200];
		char ipprot[5];
		char tcpflag[5];
		char sendbuf[2000];
		char packetnum[100];


    /*if ((dev = argv[1]) == NULL){
        usage(argv[0]);
    }*/
		dev = "eth1";

    if(pcap_lookupnet(dev, &net, &mask, errbuf) == -1){
        fprintf(stderr, "デバイスのネットマスクを取得できませんでした:%s\n", dev);
        net = 0;
        mask = 0;
    }

    /* 受信用のデバイスを開く */
    if((handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf)) == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        exit(EXIT_FAILURE);
    }

   if(pcap_compile(handle, &fp, filter_exp, 0, net) == -1){
        fprintf(stderr, "フィルタ[%s]の解析がきませんでした\n", filter_exp);
        exit(1);
    }


    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "フィルタ「%s」の組み込みができませんでした: %s\n", filter_exp, pcap_geterr(handle));
          exit(1);
    }


    /* ループでパケットを受信 */
    while (1) {
		memset(packetinfo, 0x00, sizeof(packetinfo));
		memset(sendbuf, 0x00, sizeof(sendbuf));
        if ((packet = pcap_next(handle, &header)) == NULL)
            continue;

        /* イーサネット、IP、TCPの合計ヘッダサイズに満たなければ無視 */
        if (header.len < sizeof(struct ether_header)+sizeof(struct ip)+sizeof(struct tcphdr))
            continue;
        //print_ipheader((char *)(packet+sizeof(struct ether_header)), (char *)(packet+sizeof(struct ether_header)+sizeof(struct ip)), (int)sock);
	p = (char *)(packet+sizeof(struct ether_header));
	tcp_p = (char *)(packet+sizeof(struct ether_header)+sizeof(struct ip));
	ip = (struct ip*)p;
	tcp = (struct tcphdr *)tcp_p;
	sprintf(ipprot, "%.4x", ntohs(ip->ip_p));
	sprintf(tcpflag, "%.4x", ntohs(tcp->th_flags));

	printf("ipprot %s", ipprot);
	printf("packet protocol 0x%.4x\n", ntohs(ip->ip_p));
	printf("tcp protocol 0x%x\n", ntohs(tcp->th_flags));
	//送信データの最適化
	strcat(packetinfo, inet_ntoa(ip->ip_src));
	strcat(packetinfo, comma);
	strcat(packetinfo, inet_ntoa(ip->ip_dst));
	strcat(packetinfo, comma);
	sprintf(buffer, "%d", ntohs(tcp->th_sport));
	strcat(packetinfo, buffer);
	strcat(packetinfo, comma);
	sprintf(buffer, "%d", ntohs(tcp->th_dport));
	strcat(packetinfo, buffer);
	strcat(packetinfo, comma);
	if(strcmp(ipprot, "0100") == 0){
		strcat(packetinfo, "ICMP");
	}else if(strcmp(ipprot, "0600") == 0){
		if(strcmp(tcpflag, "0100") == 0){
			strcat(packetinfo, "TCPFIN");
		}else if(strcmp(tcpflag, "0200") == 0){
			strcat(packetinfo, "TCPSYN");
		}else if(strcmp(tcpflag, "0400") == 0){
			strcat(packetinfo, "TCPRST");
		}else if(strcmp(tcpflag, "1000") == 0){
			strcat(packetinfo, "TCPACK");
		}else if(strcmp(tcpflag, "1200") == 0){
			strcat(packetinfo, "TCPSYNACK");
		}else{
			strcat(packetinfo, "other");
		}
	}else if(strcmp(ipprot, "1700") == 0){
		strcat(packetinfo, "UDP");
	}else{
		strcat(packetinfo, "other");
	}

	if(packetcount == 0){
	    beforeip_src = inet_ntoa(ip->ip_src);
	    beforetcp_sport = ntohs(tcp->th_sport);
	    packetcount++;
	}else if(beforeip_src == inet_ntoa(ip->ip_src) && beforetcp_sport == ntohs(tcp->th_sport)){
	    packetcount++;
	    if(packetcount == 10){
	        printf("送信\n");
	    }
	}else{
	    beforeip_src = inet_ntoa(ip->ip_src);
	    beforetcp_sport = ntohs(tcp->th_sport);
	    packetcount = 1;
	    printf("送信\n");
	}
	if(sendcount < MAX_COUNT){
		snprintf(&sql_str[0], sizeof(sql_str)-1, "insert into packetbuf (data, count) values ('%s', 1) on duplicate key update count = count + 1", packetinfo);
		if(mysql_query(conn, &sql_str[0])){
			fprintf(stderr, "error %s :%s\n", mysql_error(conn), sql_str);
			mysql_close(conn);
			exit(-1);
		}
		memset(&sql_str[0], 0x00, sizeof(sql_str));
		resp = mysql_use_result(conn);
		mysql_free_result(resp);
		sendcount++;
	}else{
		snprintf(&sql_str[0], sizeof(sql_str)-1, "select * from packetbuf");
		if(mysql_query(conn, &sql_str[0])){
			fprintf(stderr, "error %s :%s\n", mysql_error(conn), sql_str);
			mysql_close(conn);
			exit(-1);
		}
		memset(&sql_str[0], 0x00, sizeof(sql_str));
		resp = mysql_use_result(conn);
		while((row = mysql_fetch_row(resp)) != NULL){
			strcat(sendbuf, row[0]);
			//strcat(sendbuf, ":");
			sprintf(packetnum, "%s", row[1]);
			//strcat(sendbuf, packetnum);
			//strcat(sendbuf, "/");
		}
		printf("sendbuf: %s\n", sendbuf);
		sendto(sock0, sendbuf, sizeof(sendbuf), 0, (struct sockaddr *)&addr, sizeof(addr));
		mysql_free_result(resp);
	 	snprintf(&sql_str[0], sizeof(sql_str)-1, "truncate table packetbuf");
		if(mysql_query(conn, &sql_str[0])){
			fprintf(stderr, "error %s :%s\n", mysql_error(conn), sql_str);
			mysql_close(conn);
			exit(-1);
		}
		memset(&sql_str[0], 0x00, sizeof(sql_str));
		resp = mysql_use_result(conn);
		mysql_free_result(resp);
		sendcount = 0;
	} 	
	/*データベースの処理*/
	if(hostip != inet_ntoa(ip->ip_src)){
	    snprintf(&sql_str[0], sizeof(sql_str)-1, "insert into packetinfo (src_ipaddress, count) values ('%s', 1) on duplicate key update count = count + 1", inet_ntoa(ip->ip_src));
	}
	if(mysql_query(conn, &sql_str[0])){
	    fprintf(stderr, "error %s :%s\n", mysql_error(conn), sql_str);
	    mysql_close(conn);
	    exit(-1);
	}
	memset(&sql_str[0], 0x00, sizeof(sql_str));
	resp = mysql_use_result(conn);
	mysql_free_result(resp);
	}

    /* ここに到達することはない */
    pcap_close(handle);
    //close(sock);
    close(sock0);
    //mysql_free_result(resp);
    mysql_close(conn);
    return 0;
}
