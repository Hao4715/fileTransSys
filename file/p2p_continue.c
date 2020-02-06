#include "p2p_continue.h"

extern char mozart_req_header[]; 
	/*"GET %s MOZART/1.0\r\n"
	"Req_Size: %d\r\n"
	"Req_Start_Location: %d\r\n"
	"\r\n"
	;*/
extern char download_per[];
	/*"Percent: %f\r\n"
	"\r\n"
	;*/
extern char mozart_req_content[512];
//char download_per_content[64] = {0};
extern char download_per_content[64];

int p2p_continue_process_create(int sockfd, struct php_oper * php_operation)
{
	printf("开始断点续传\n");
	int download_flag = 0, rate = 0;
	pthread_mutex_init(&work_mutex,NULL);
	int res; //判断函数调用是否成功
	int i, size_ed = 0, temp_file_len;
	char location_temp[32] = "";
	void *thread_result;
	FILE *file;
	strcpy(location_temp, php_operation->file_name);
	strcat(location_temp, "_temp");
	pthread_t th[php_operation->thread_num];
	int num = 0;//实际线程数
	chdir(location_temp);
	for(i = 0; i < php_operation->thread_num; i++)
	{
		struct req_info *request_info = (struct req_info*)malloc(sizeof(struct req_info));
		strcpy(request_info->file_location_all, php_operation->file_name);
		char file_num[4] = {0};
		sprintf(file_num, "%d", i);
		sprintf(request_info->file_name_dis, "%s%s", php_operation->file_name, file_num);
		file = fopen(request_info->file_name_dis, "rb");
		fseek(file, 0, SEEK_END);
		temp_file_len = ftell(file);
		fclose(file);
		if(i != (php_operation->thread_num - 1))
		{
			request_info->req_size = php_operation->file_size / php_operation->thread_num;
		}
		else
		{
			request_info->req_size = php_operation->file_size - (php_operation->file_size / php_operation->thread_num)*i;
		}
		if(temp_file_len == request_info->req_size)
		{
			rate += temp_file_len;
			continue;
		}
		else
		{
			request_info->req_size -= temp_file_len;
			request_info->req_start_location = (php_operation->file_size / php_operation->thread_num) * i + temp_file_len;
			rate += temp_file_len;
			request_info->rate = &rate;
			request_info->sockfd = sockfd;
			request_info->file_size = php_operation->file_size;
		}
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
		res = pthread_create(&th[num], NULL, file_continue, (void *)request_info);
		num ++;
	}
	printf("等待继续下载线程结束\n");
	for(i = num - 1; i >= 0; i--)
	{
		res = pthread_join(th[i], &thread_result);
		if(res == 0)
		{
			printf("结束一个线程\n");
		}
		else
		{
			printf("结束线程失败\n");
		}
	}
	printf("全部继续下载线程结束\n");
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
	download_flag = 1;
	return download_flag;	
}


void *file_continue(void *arg)
{
	struct req_info *request_info = (struct req_info*)arg;
	struct websocket_res_head * websocket_head = (struct websocket_res_head *)malloc(sizeof(struct websocket_res_head *));
	int client_sockfd;
	struct sockaddr_in server_addr;
	float download_percent;
	socklen_t serv_addr_len;
	char buf_content[res_content_len], buf_header[res_header_len];
	int res;

	client_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	serv_addr_len = sizeof(server_addr);
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(request_info->p2p_server_port);
	inet_pton(AF_INET, request_info->p2p_server_ip, &server_addr.sin_addr.s_addr);

	connect(client_sockfd, (struct sockaddr*)&server_addr, serv_addr_len);

	int content_len = snprintf(mozart_req_content, 512, mozart_req_header, request_info->file_location_all, request_info->req_size, request_info->req_start_location, 0);

	send(client_sockfd, mozart_req_content, content_len, 0);
	struct res_info *response_info = get_res_info(client_sockfd);
	if(response_info->start_flag == 1)
	{
		FILE *file = fopen(request_info->file_name_dis, "ab");
		int write_flag = 0, write_all = 0;
		while(write_all < request_info->req_size)
		{
			bzero(buf_content, sizeof(buf_content));
			res = recv(client_sockfd, buf_content, sizeof(buf_content), 0);
			write_flag = fwrite(buf_content, 1, res, file);
			write_all += write_flag;
			pthread_mutex_lock(&work_mutex);
			*request_info->rate += write_flag;
			download_percent = (((float)*request_info->rate / request_info->file_size) * 100);
			//printf("jindu:%f\n",download_percent);
			content_len = snprintf(download_per_content, 64, download_per, download_percent);
			res = send_websocket_res_head(request_info->sockfd, websocket_head, download_per_content);
			pthread_mutex_unlock(&work_mutex);
		}
		fclose(file);
	}
	close(client_sockfd);
	pthread_exit("线程结束\n");
}













