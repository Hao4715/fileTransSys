#include "mission.h"
#include "p2p_download.h"
#include "p2p_continue.h"
//#include "http17.h"

int mission_dis(int sockfd, struct php_oper * php_operation)
{
	int download_flag; //下载完成标志
	if(strcmp(php_operation->protocol, "http") == 0)
	{
		printf("执行http\n");
		download_flag = http(sockfd,php_operation->http_url,php_operation->thread_num);
	}
	else if(strcmp(php_operation->protocol, "ftp") == 0)
	{
		printf("执行ftp\n");
		download_flag = ftp_download_main(sockfd, php_operation->ftp_ip, php_operation->file_name, php_operation->thread_num);
	}
	else if(strcmp(php_operation->protocol, "p2p") == 0)
	{
		printf("执行p2p\n");
		if(strcmp(php_operation->operation, "start") == 0)
		{
			download_flag = p2p_download_process_create(sockfd, php_operation);
		}
		else
		{
			download_flag = p2p_continue_process_create(sockfd, php_operation);
		}
	}
	else
	{
		perror("protocol error\n");
	}
	return download_flag;
}
/*
pid_t create_process(ssize_t efd, int sockfd, struct php_oper * php_operation)
{
	pid_t pid;
	int res;
	int download_flag = 0;
	if((pid = fork()) == 0)
	{
		//printf("sssssssss\n");
	       	download_flag = p2p_process_create(sockfd, php_operation); 
	       	printf("%d\n",download_flag);
		if(download_flag == 1)
		{
			res = epoll_ctl(efd, EPOLL_CTL_DEL, sockfd, NULL);
			if(res == -1)
			{
				perror("epoll_ctl error in mission.c of function create_process\n");
				exit(0);
			}
			close(sockfd);
			exit(1);
		}
		
	}
	else
	{
		//printf("11111pid:%d\n",pid);
		return pid;
	}
}*/
