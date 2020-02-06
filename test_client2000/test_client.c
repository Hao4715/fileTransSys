#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
/*
char opr[] = 
	"Operation: %s\r\n"
	"Protocol: %s\r\n"
	"File_name: %s\r\n"
	"File_size: %d\r\n"
	"Thread_num: %d\r\n"
	"P2p_server_num: %d\r\n"
	"P2p_server_ip: %s %s\r\n"
	"P2p_server_port: %d %d\r\n"
	"\r\n";
char opr_string[] = "";
*/
//int sockfds[2000] = {0};

int threadnum = 0;

void *clients(void *arg)
{
	int client_sockfd;
	struct sockaddr_in server_addr;
	socklen_t serv_addr_len;
	char buf[BUFSIZ];
	int re;

	client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	

	serv_addr_len = sizeof(server_addr);
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8000);
	inet_pton(AF_INET, "182.92.65.28", &server_addr.sin_addr.s_addr);

	connect(client_sockfd, (struct sockaddr *)&server_addr, serv_addr_len);
	
	//sockfds[threadnum] = client_sockfd;
	
	sleep(400);
	close(client_sockfd);
	pthread_exit("1");
}

int main(void)
{
	int res;
	pthread_t thread_id[2000];
	void *result;
	for(threadnum = 0; threadnum < 2000; threadnum ++)
	{
		res = pthread_create(&thread_id[threadnum],NULL,clients,(void*)"thread create");
	}
	for(threadnum = 1999; threadnum >= 0; threadnum--)
	{
		pthread_join(thread_id[threadnum], &result);
	}
	/*int client_sockfd;
	struct sockaddr_in server_addr;
	socklen_t serv_addr_len;
	char buf[BUFSIZ];
	int re;

	client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	

	serv_addr_len = sizeof(server_addr);
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8000);
	inet_pton(AF_INET, "182.92.65.28", &server_addr.sin_addr.s_addr);

	connect(client_sockfd, (struct sockaddr *)&server_addr, serv_addr_len);
	//close(client_sockfd);
	int i,len;
	float percent;
	while(1)
	{
		printf("please select your option:\n");
		printf("press \"1\" to start download\n");
		printf("press \"2\" to stop download\n");
		printf("press \"3\" to continue download\n");
		printf("press \"4\" to delete download\n");
		scanf("%d",&i);
		if(i == 1)
		{
			printf("fff\n");
			len = snprintf(opr_string, 512, opr, "start", "p2p", "big.txt", 6012, 3, 2, "175.24.101.218", "111.230.96.235", 8666, 8666);
			printf("len:%d\n%s\n",len,opr_string);
		}
		if(i == 2)
		{
			len = snprintf(opr_string, 512, opr, "start", "p2p", "music.mp3", 14043252, 3, 2,"175.24.101.218", "111.230.96.235", 8666, 8666);
			printf("len:%d\n%s\n",len,opr_string);
		}
		if(i == 3)
		{
			len = snprintf(opr_string, 512, opr, "start", "p2p", "S01E01.mkv", 301199257, 3, 2,"175.24.101.218", "111.230.96.235", 8666, 8666);
			printf("len:%d\n%s\n",len,opr_string);
		}
		if(i == 4)
		{
			len = snprintf(opr_string, 512, opr, "stop", "p2p", "music.mp3", 14043252, 3, 2,"175.24.101.218", "111.230.96.235", 8666, 8666);
			printf("len:%d\n%s\n",len,opr_string);
		}
		if(i == 5)
		{
			len = snprintf(opr_string, 512, opr, "continue", "p2p", "music.mp3", 14043252, 3, 2,"175.24.101.218", "111.230.96.235", 8666, 8666);
			printf("len:%d\n%s\n",len,opr_string);
		}
		send(client_sockfd, opr_string, len, 0);
		printf("%s\n",opr_string);
		re = recv(client_sockfd, buf, 14, 0);
		//write(STDOUT_FILENO, buf, re);*/
	/*	printf("服务器说%s\n",buf);
		while(1)
		{
			int recv_num;
			int flag = 0;
			int recv_len = 0;
			char p[64] = {0};
			while(recv_len < 64)
			{
				recv_num = recv(client_sockfd, p + recv_len, 1, 0);
				if(recv_num < 1)
				{
					break;
				}
				if((p[recv_len] == '\r' && (flag == 0||flag == 2)) || (p[recv_len] == '\n' && (flag == 1 || flag == 3)))
				{
					flag ++;
				}
				else
				{
					flag = 0;
				}
				recv_len += recv_num;
				if(flag == 4)
				{
					break;
				}
			}
			char *pos = strstr(p, "Percent: ");
			if(pos)
			{
				sscanf(pos, "%*s %f",&percent);
			}
			printf("%f\n",percent);
			if(percent == 100)
			{
				break;
			}
		}*/
	//}
	//close(client_sockfd);
	return 0;

}
