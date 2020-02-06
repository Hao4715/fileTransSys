#ifndef _P2P_DOWNLOAD_H_
#define _P2P_DOWNLOAD_H_
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <dirent.h>
#include <openssl/md5.h>
#include "get_php_oper_info.h"

#define res_header_len 29
#define res_content_len 1024

struct res_info
{
	int start_flag;
};
struct req_info
{
	int req_size;
	int file_size;
	int req_start_location;
	int sockfd; //php与c连接cosket，用于返回下载进度
	int *rate;  //记录已下进度
	char file_location_all[32],file_name_dis[32];
	char p2p_server_ip[16];
	int p2p_server_port;
	char md5[64];
};

pthread_mutex_t work_mutex;

int p2p_download_process_create(int sockfd, struct php_oper * php_operation);

struct res_info * get_res_info(int sockfd);

void *file_download(void *arg);

int file_merge(int num,char file_name[] ,char origin_md5[64]);

int md5_com(char file_name[], char origin_md5[64]); 
#endif
