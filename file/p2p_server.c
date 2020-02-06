#include "p2p_server.h"

char mozart_res_header[] = 
	"MOZART/1.0 200 OK\r\n"
	"Start: %d\r\n"
	"\r\n"
	;
char mozart_res_content[1024] = {0};

int create_listenfd(void)
{
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	int opt = 1;

	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8666);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	int res = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if(res == -1)
	{
		perror("bind error\n");
	}

	listen(server_fd,20);
	return server_fd;
}

struct p2p_req_info * get_p2p_req_info(int sockfd)
{
	//printf("buffer in\n%s\n",buffer);
	int recv_num;
	int flag = 0;
	int recv_len = 0;
	char p[256] = {0};
	while(recv_len < 255)
	{
		recv_num = recv(sockfd, p + recv_len, 1, 0);
		if(recv_num < 1)
		{
			break;
		}
		if((p[recv_len] == '\r' && (flag == 0 || flag == 2)) || (p[recv_len] == '\n' &&(flag ==1 || flag ==3)))
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
	struct p2p_req_info *request_info =(struct p2p_req_info*)malloc(sizeof(struct p2p_req_info));
	char *pos = strstr(p,"GET");
	{
		sscanf(pos, "%*s %s",&request_info->file_location);
	}
	pos = strstr(p,"Req_Size:");
	if(pos)
	{
		sscanf(pos, "%*s %d",&request_info->req_size);
	}
	pos = strstr(p,"Req_Start_Location:");
	if(pos)
	{
		sscanf(pos, "%*s %d",&request_info->req_start_location);
	}
	//printf("请求解析结果：\nfile_location:%s\nreq_size:%d\nreq_start_location:%d\n",request_info->file_location,request_info->req_size,request_info->req_start_location);
	return request_info;
}

void *handle_request(void *arg)
{
	int client_sockfd = (intptr_t)arg;
	char buffer[1024] = "";
//	int res;   //用于判断客户端信息接收是否正确
//	char buf[4] = ""; // 用于接收信息判断客户端是否断开
	//char buffer[512] = {0};
	//int res = recv(client_sockfd, buffer, sizeof(buffer), 0);
//	printf("客户端说：\n%s\n",buffer); 

	struct p2p_req_info *request_info = get_p2p_req_info(client_sockfd);
	printf("请求解析结果：\nfile_location:%s\nreq_size:%d\nreq_start_location:%d\n",request_info->file_location,request_info->req_size,request_info->req_start_location);

	FILE *file = fopen(request_info->file_location, "rb+");
	if(file != NULL)
	{
		int start_flag = 1;
		int content_len = snprintf(mozart_res_content, 40, mozart_res_header, start_flag);
		//printf("content_len:%d\n",content_len);
		//printf("content:%s\n",mozart_res_content);
		int send_len = send(client_sockfd, mozart_res_content, content_len, 0);
		//printf("send_len:%d\n",send_len);
	}
	int read_flag = 0;
	fseek(file, request_info->req_start_location, SEEK_SET);
	while(1)
	{
		bzero(buffer,sizeof(buffer));
		if(request_info->req_size >= 1024)
		{
			read_flag = fread(buffer, sizeof(char), 1024, file);
			request_info->req_size -= 1024;
		}else
		{
			read_flag = fread(buffer, sizeof(char), request_info->req_size, file);
			request_info->req_size = 0;
		}
		//printf("file content:%s\n",buffer);	
		int send_flag = send(client_sockfd, buffer, read_flag, 0);
		if(send_flag != 1024 && request_info->req_size != 0)
		{
			printf("发送中断\n");
			break;
		}
//		printf("send_flag:%d\n",send_flag);
		/*res = recv(client_sockfd, buf, 2, 0);
		if(strcmp(buf, "ok") != 0)
		{
			pthread_exit("1");
		}
		*/
		if(request_info->req_size == 0)
		{
			printf("发送停止\n");
			break;
		}
	}
	fclose(file);
	pthread_exit("1");

}

int p2p_server_create(void)
{
//	chdir("file_rep");
	int server_sockfd,client_sockfd;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len;
	int num = 0;
	pthread_t th;

	server_sockfd = create_listenfd();
	printf("server_sockfd:%d\n",server_sockfd);
	client_addr_len = sizeof(client_addr);
	while(1)
	{
		num ++;
		printf("waiting for the %d client to connect.....\n",num);
		int client_sockfd = accept(server_sockfd,(struct sockaddr *) &client_addr, &client_addr_len);
		printf("this is the %d client.....\n",num);
		int res = pthread_create(&th, NULL, handle_request, (void *)(intptr_t)client_sockfd);
		pthread_detach(th);
	}
	close(client_sockfd);
	close(server_sockfd);
	return 0;
}


