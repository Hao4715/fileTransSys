/*#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
*/
#include "p2p_download.h"
//#include "get_php_oper_info.h"
/*#define res_header_len 29
#define res_content_len 1024

struct res_info
{
	int start_flag;
};
*/
char mozart_req_header[] = 
	"GET %s MOZART/1.0\r\n"
	"Req_Size: %d\r\n"
	"Req_Start_Location: %d\r\n"
	"\r\n"
	;     //p2p下载请求头

char download_per[] = 
	"Percent: %f\r\n"
	"\r\n"
	;
char mozart_req_content[512] = {0};  //p2p请求字符串

char download_per_content[64] = {0}; //文件下载完成百分比

extern char file_info_to_php[];
extern char file_info_php[128];

int p2p_download_process_create(int sockfd, struct php_oper * php_operation)//下载任务进程
{
	printf("开始了吗\n");
	struct websocket_res_head * websocket_head_file_info = (struct websocket_res_head *)malloc(sizeof(struct websocket_res_head *));
	int download_flag;//下载完成标志	
	int rate = 0;
	//pthread_mutex_t work_mutex;
	pthread_mutex_init(&work_mutex, NULL);
	int res;//等待线程结束标志
	char location_temp[32] = "";
	strcpy(location_temp, php_operation->file_name);
	strcat(location_temp, "_temp");
	int i, size_ed = 0;
	pthread_t th[php_operation->thread_num];
	void *thread_result;
	mkdir(location_temp, 0777);
	chdir(location_temp);
	for(i = 0; i < php_operation->thread_num; i++)
	{
		struct req_info *request_info = (struct req_info*)malloc(sizeof(struct req_info));
		strcpy(request_info->file_location_all, php_operation->file_name);
		char file_num[4] = {0};
		sprintf(file_num, "%d", i);
		sprintf(request_info->file_name_dis, "%s%s", php_operation->file_name, file_num);
		if(i != (php_operation->thread_num -1))
		{
			request_info->req_size = php_operation->file_size / php_operation->thread_num;
		}
		else
		{
			request_info->req_size = php_operation->file_size - (php_operation->file_size / php_operation->thread_num) * i;
		}
		request_info->req_start_location = (php_operation->file_size / php_operation->thread_num) * i;
		request_info->rate = &rate;
		request_info->sockfd = sockfd;
		request_info->file_size = php_operation->file_size;
		if(i <= php_operation->thread_num / php_operation->p2p_server_num)
		{
			strcpy(request_info->p2p_server_ip, php_operation->p2p_server_ip[0]);
			request_info->p2p_server_port = php_operation->p2p_server_port[0];
		}
		else
		{
			strcpy(request_info->p2p_server_ip, php_operation->p2p_server_ip[1]);
			request_info->p2p_server_port = php_operation->p2p_server_port[1];	
		}
		res = pthread_create(&th[i], NULL, file_download, (void *)request_info); //创建线程
	}
	printf("等待线程结束\n");
	for(i = php_operation->thread_num -1; i>= 0 ;i--)
	{
		res = pthread_join(th[i], &thread_result);
		if(res == 0)
		{
			printf("结束一个线程\n");
		}
		else
		{
			printf("线程结束失败\n");
		}
	}
	printf("线程全部结束\n");
	int merge_flag = file_merge(php_operation->thread_num, php_operation->file_name, php_operation->md5);
	if(merge_flag == 1)
	{
		printf("文件合并成功\n");
	}
	chdir("..");
	char system_call[] = {0};
	sprintf(system_call, "%s%s", "rm -rf ",location_temp);
	printf("%s\n",system_call);
	system(system_call);
	snprintf(file_info_php, 128, file_info_to_php, php_operation->md5, php_operation->file_name, php_operation->file_size);
	send_websocket_res_head(sockfd, websocket_head_file_info, file_info_php);
	free(websocket_head_file_info);
	websocket_head_file_info = NULL;
	download_flag = 1;
	return download_flag;
}



struct res_info * get_res_info(int sockfd)
{
	//printf("*******IN:%d\n",sockfd);
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
		if((p[recv_len] == '\r' && (flag == 0 || flag == 2)) || (p[recv_len] == '\n' && (flag == 1 || flag == 3)))
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
	struct res_info *response_info = (struct res_info*)malloc(sizeof(struct res_info));
	char *pos = strstr(p,"Start: ");
	if(pos)
	{
		sscanf(pos, "%*s%d", &response_info->start_flag);
	}
	return response_info;
}


void *file_download(void *arg)
{
	struct req_info *request_info = (struct req_info*)arg;
	struct websocket_res_head * websocket_head = (struct websocket_res_head *)malloc(sizeof(struct webscoket_res_head *));
	printf("请求信息:info_IN:%s\n%d\n%d\n\n\n",request_info->file_location_all,request_info->req_size,request_info->req_start_location);
	printf("success??????:%p\n",request_info->rate);
	int client_sockfd;
	struct sockaddr_in server_addr;
	float download_percent;
	socklen_t serv_addr_len;
	char buf_content[res_content_len], buf_header[res_header_len];
	int res;
	//int res;

	client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	

	serv_addr_len = sizeof(server_addr);
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(request_info->p2p_server_port);
	inet_pton(AF_INET, request_info->p2p_server_ip, &server_addr.sin_addr.s_addr);

	connect(client_sockfd, (struct sockaddr *)&server_addr, serv_addr_len);
	
	int content_len = snprintf(mozart_req_content, 512, mozart_req_header, request_info->file_location_all, request_info->req_size, request_info->req_start_location, 0);
	
	printf("send content:\n%s\n",mozart_req_content);
	send(client_sockfd, mozart_req_content, content_len, 0);
	struct res_info *response_info = get_res_info(client_sockfd);
	if(response_info->start_flag == 1)
	{
		FILE *file = fopen(request_info->file_name_dis, "wb+");
		int write_flag = 0, write_all = 0;
		while(write_all < request_info->req_size)
		{
			bzero(buf_content, sizeof(buf_content));
			res = recv(client_sockfd, buf_content, sizeof(buf_content), 0);
			write_flag = fwrite(buf_content, 1, res, file);	
			write_all += write_flag;
			pthread_mutex_lock(&work_mutex);
			*request_info->rate += write_flag;
			download_percent = (((float)*request_info->rate / request_info->file_size)) * 100;
			//printf("jindu:%f\n",download_percent);
			content_len = snprintf(download_per_content, 64, download_per, download_percent);
			res = send_websocket_res_head(request_info->sockfd, websocket_head, download_per_content);
			//send(request_info->sockfd, download_per_content, content_len, 0);
			pthread_mutex_unlock(&work_mutex);
		//	printf("jindu:%d\n",*request_info->rate);
		}
		fclose(file);
	}
	close(client_sockfd);
	free(request_info);
	request_info =NULL;
	free(websocket_head);
	websocket_head = NULL;
	pthread_exit("线程结束");
}

int file_merge(int num,char file_name[], char origin_md5[64])
{
	int file_merge_flag = 0;
	char location_temp[32] = "";
	strcpy(location_temp, file_name);
	strcat(location_temp, "_temp");
	int i = 0;
	int res = 0;//md5 compare 
	FILE *target,*current;
	int length = 0; //current file length
	char *s; //current file content
	char file_temp[] = "";
	chdir("..");
	target = fopen(file_name, "wb");
	chdir(location_temp);
	for(i = 0; i < num ; i++)
	{
		int t = i;
		strcpy(file_temp,file_name);
		char file_num[8] = {0};
		sprintf(file_num, "%d", t);
		sprintf(file_temp, "%s%s",file_temp,file_num);
		current = fopen(file_temp, "rb");
		fseek(current, 0, SEEK_END);
		length = ftell(current);
		if(!length)
		{
			return 0;
		}
		fseek(current, 0, SEEK_SET);
		s = (char *)malloc(length);
		fread(s,1,length,current);
		printf("sssssssssss:%d\n%d\n",strlen(s),sizeof(s));
		int res = fwrite(s,1,length,target);
		fflush(target);
		fclose(current);
		
	}
	/*chdir("..");
	char system_call[] = {0};
	sprintf(system_call, "%s%s", "rm -rf ",location_temp);
	printf("%s\n",system_call);
	system(system_call);
	
	*/
	fclose(target);
	res = md5_com(file_name, origin_md5);
	if(res == 1)
		file_merge_flag = 1;
	return file_merge_flag;;
	
}


int md5_com(char file_name[], char origin_md5[64])
{
	chdir("..");
	MD5_CTX ctx;
	unsigned char outmd[32];
	char file_md5[33];
	char buffer[1024];
	int len = 0;
	int i;
	int md5_equal = 0;
	FILE *fp;

	memset(outmd, 0, sizeof(outmd));
	memset(buffer, 0 ,sizeof(buffer));

	fp = fopen(file_name, "rb");
	if(fp == NULL)
	{
		printf("file open error in md5_com\n");
	}
	MD5_Init(&ctx);
	while((len = fread(buffer, 1, 1024, fp)) > 0)
	{
		MD5_Update(&ctx, buffer, len);
		memset(buffer, 0 ,sizeof(buffer));
	}
	MD5_Final(outmd, &ctx);
	printf("md5 original:%s\n",origin_md5);
	for(i = 0; i < 32; i ++)
	{
		snprintf(file_md5 + i*2, 2+1, "%02x", outmd[i]);
	}
	file_md5[32] = '\0';
	printf("file_md5:%s\n",file_md5);
	if(strcmp(origin_md5, file_md5) == 0)
		md5_equal = 1;
	printf("md5_equal:%d\n",md5_equal);
	return md5_equal;
}

















