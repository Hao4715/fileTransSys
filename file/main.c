/*#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
*/
#include "get_php_oper_info.h"
#include "mission.h"
#include "p2p_server.h"

#define MAXLINE 8192
#define LISTEN_SERV_PORT 8555
#define OPEN_MAX 5000
/*
struct php_oper   //存储前端php发送的操作信息
{	
	int file_size;
	int thread_num;
	int server_id[2];
	char *file_name;
	char operation[];//1开启下载，2暂停下载。3删除下载。
};
*/
struct event_ptr //事件epoll定义结构体
{
	int fd;
	int process_pid;
};
/*
void wait_child(int para)
{
	
}
*/
int main(void)
{

	chdir("file_download");
	int server_process_pid; //开启下载服务器进程标志
	int download_process_pid;//开启下载请求进程标志
	int download_flag; //下载完成标志
	pid_t mission_pid;
	if((server_process_pid = fork()) == 0)
	{
		p2p_server_create();
		exit(1);
	}
	int mission_num = 0;
	int i, listenfd, connfd, sockfd;
	int n, num = 0;
	ssize_t nready, efd, res;
	char str[INET_ADDRSTRLEN];
	socklen_t clientlen;

	struct sockaddr_in servaddr, clieaddr;
	struct epoll_event tep, ep[OPEN_MAX];


	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	
	int opt = 1;

	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));


	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(LISTEN_SERV_PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	
	listen(listenfd, 128);
	
	efd = epoll_create(OPEN_MAX); //创建epoll模型，efd指向红黑树根节点
	if(efd == -1)
	{
		perror("epoll_create error\n");
		exit(1);
	}

	struct event_ptr *e_ptr = (struct event_ptr *)malloc(sizeof(struct event_ptr));
	struct event_ptr *e_ptr_tem = (struct event_ptr *)malloc(sizeof(struct event_ptr));
	
	tep.events = EPOLLIN;// tep.data.fd = listenfd;//指向listenfd的监听事件为读
	e_ptr->fd = listenfd;
	e_ptr->process_pid = 0;
	tep.data.ptr = (void *)e_ptr;
	res = epoll_ctl(efd, EPOLL_CTL_ADD, listenfd, &tep); //将listenfd及对应的结构体添加到树上，efd可找到该树
	if(res == -1)
	{
		perror("epoll_ctl error\n");
		exit(1);
	}
	//chdir("file_download");
	for( ; ; )
	{
		//epoll为server阻塞监听事件，ep为struct epoll_event类型数组，OPEN_MAX为数组容量，-1表示永久阻塞
		
		while(waitpid(-1, NULL, WNOHANG) > 0);	
		nready = epoll_wait(efd, ep, OPEN_MAX, -1);
		if(nready == -1)
		{
			perror("epoll_wait error 11\n");
			exit(1);
		}
		for(i = 0; i< nready; i ++)
		{
			if(!(ep[i].events & EPOLLIN)) //如果不是读事件，继续循环
				continue;
			e_ptr_tem = (struct event_ptr *)ep[i].data.ptr;
			if(e_ptr_tem->fd == listenfd)//判断满足事件fd是不是listenfd
			{
				clientlen = sizeof(clieaddr);
				connfd = accept(listenfd, (struct sockaddr *)&clieaddr, &clientlen); //接受链接
				printf("received from %s at port %d\n",inet_ntop(AF_INET, &clieaddr.sin_addr, str, sizeof(str)), ntohs(clieaddr.sin_port));
				res = shake_hands(connfd);
				printf("cfd %d----client %d\n",connfd, ++num);
				
				
				e_ptr = (struct event_ptr *)malloc(sizeof(struct event_ptr));
				e_ptr->fd = connfd;
				tep.events = EPOLLIN | EPOLLET; //tep.data.fd = connfd;
				tep.data.ptr = (void *)e_ptr;
				res = epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &tep);
				if(res == -1)
				{
					perror("epoll_ctl error\n");
					exit(1);
				}
			}
			else   //不是listenfd
			{
				printf("++++++zhixing\n\n");
				//sleep(2);
				e_ptr_tem = (struct event_ptr *)ep[i].data.ptr;
				sockfd = e_ptr_tem->fd; //sockfd用于进程中与服务器进行信息交换
				printf("sockfd has mess:%d\n",sockfd);
				char *res_str = "";
				res_str = analy_data(sockfd);
				printf("res_str%d\n",strlen(res_str));
				//sleep(200);
				//if(strlen(res_str) == 0)
				//	continue;
				printf("res_str in main:%s\n",res_str);
				//char res_str[] = {0};
				
				//strcpy(res_str,analy_data(sockfd));
				
				struct php_oper * php_operation = (struct php_oper *)malloc(sizeof(struct php_oper));
				if(strlen(res_str) != 0)
				{
					php_operation = get_websocket_oper(res_str);
					//struct php_oper * php_operation = get_websocket_oper(res_str);
				}
				else
				{
					//struct php_oper * php_operation = (struct php_oper *)malloc(sizeof(struct php_oper));
					php_operation->php_req_len = 0;
				}
				//struct php_oper * php_operation = get_php_oper(sockfd);
				if(php_operation->php_req_len > 0)//实际读到数据
				{
					//pid_t mission_pid;
					//char buffer[] = "i got you!!!!";
					//write(sockfd,buffer,sizeof(buffer));
					//printf("socket pid:%d\n",e_ptr_tem->process_pid);	
					printf("php操作信息:operation:%s\nprotocol:%s\nfile_name:%s\nfile_size:%d\nthread_num:%d\np2p_server_num:%d\nphp_req_len:%d\np2p_server_ip[0]:%s\np2p_server_ip[1]:%s\np2p_server_port[0]:%d\np2p_server_port[1]:%d\nhttp_url:%s\nftp_ip:%s\n",php_operation->operation,php_operation->protocol,php_operation->file_name,php_operation->file_size,php_operation->thread_num,php_operation->p2p_server_num,php_operation->php_req_len,php_operation->p2p_server_ip[0],php_operation->p2p_server_ip[1],php_operation->p2p_server_port[0],php_operation->p2p_server_port[1],php_operation->http_url,php_operation->ftp_ip);
					if(strcmp(php_operation->operation,"start") == 0)
					{
						printf("开始下载\n");
					//	struct websocket_res_head * websocket_head = (struct websocket_res_head *)malloc(sizeof(struct websocket_res_head));
					//	int testttt = send_websocket_res_head(e_ptr_tem->fd, websocket_head);
					//	sleep(200);
						if((download_process_pid = fork()) == 0)
						{
							download_flag = mission_dis(e_ptr_tem->fd, php_operation);
							printf("download_flag:%d\n",download_flag);
							if(download_flag == 1)
							{
								res = epoll_ctl(efd, EPOLL_CTL_DEL, e_ptr_tem->fd,NULL);
								if(res == -1)
								{
									perror("epoll_ctl error in main.c\n");
									exit(1);
								}
								close(e_ptr_tem->fd);
								free(php_operation);
								php_operation = NULL;
								printf("**********download finished**********\n");
								exit(1);
							}
						}
						else 
						{
							e_ptr->process_pid = download_process_pid;	
						}
					}
					else if(strcmp(php_operation->operation, "continue") == 0)
					{
						printf("继续下载\n");
						if((download_process_pid = fork()) == 0)
						{
							download_flag = mission_dis(e_ptr_tem->fd, php_operation);
							printf("download_flag:%d\n",download_flag);
							if(download_flag == 1)
							{
								printf("**********continue download finished\n");
								exit(1);
							}
						}
						else
						{
							e_ptr->process_pid = download_process_pid;
						}
					}
					else if(strcmp(php_operation->operation,"stop") == 0)
					{
						printf("暂停下载\n");
						printf("jin cheng hao:%d\n",e_ptr_tem->process_pid);
						/*res = epoll_ctl(efd, EPOLL_CTL_DEL, sockfd, NULL); //将文件描述符从红黑树摘除
						if(res == -1)
						{
							perror("epoll_ctl error\n");
							exit(1);
						}
						close(sockfd);		*/
						char system_call[] = {0};
						sprintf(system_call, "%s%d", "kill ", e_ptr_tem->process_pid);
						printf("%s\n",system_call);
						system(system_call);
						waitpid(e_ptr_tem->process_pid, NULL, WNOHANG);
					}
					else if(strcmp(php_operation->operation,"delete") == 0)
					{
						printf("删除下载\n");
					}
					else
					{
						perror("operation error\n");
					}
				}
				else if(php_operation->php_req_len == 0)//读到0，说明客户端关闭连接
				{
					res = epoll_ctl(efd, EPOLL_CTL_DEL, sockfd, NULL); //将文件描述符从红黑树摘除
					if(res == -1)
					{
						perror("epoll_ctl error\n");
						exit(1);
					}
					close(sockfd);
					printf("client [%d] closed connrction\n",sockfd);
					
				}
				else   //出错
				{
					res = epoll_ctl(efd, EPOLL_CTL_DEL, sockfd, NULL); //将文件描述符从红黑树摘除
					close(sockfd);
					perror("read < 0 error\n");
					exit(1);
				}
				//freeeeeeeeeephp_operation
				free(php_operation);
				php_operation = NULL;
			}
		}
	}
	
	free(e_ptr);
	free(e_ptr_tem);
	e_ptr = NULL;
	e_ptr_tem = NULL;
	return 0;
}











