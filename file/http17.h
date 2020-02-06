#ifndef HTTPDL
#define HTTPDL


#include <sys/ioctl.h>
#include <poll.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <ctype.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <strings.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <openssl/md5.h>
#include "get_php_oper_info.h"
#include "p2p_download.h"

#define READ_DATA_SIZE	1024
#define MD5_SIZE		16
#define MD5_STR_LEN		(MD5_SIZE * 2)
#define THREADNUM 4

int Compute_file_md5(const char *file_path, char *value);
char * md5_cpy(char *file_path,char * md5_file);

//int *download_size;

struct MERGE{
	char name[1024];

};

/*struct httpdown_info{
	 char url[1024];
	 char new_name[512];
	 char tempname[512];
	 long startp,endp;
 };
 */
struct httpdown_info
{
	int sockfd;
	long startp,endp;
	 char url[1024];
	 char new_name[512];
	 char tempname[512];
 };

typedef struct HTTP_INFO{

	 long file_size;

	char content_type[256];

	char file_name[256];
	char ip_addr[INET_ADDRSTRLEN];

	char aliases[256];

	int status_code;

}HTTP_INFO;

 

typedef struct HOST_INFO{

	char host_name[256];

	char file_path[1024];

	unsigned short port;

	char new_name[256];
	
	

}HOST_INFO;

typedef struct REQ_INFO{
	
 char tempname[256];
 long startp,endp;

}REQ_INFO;

REQ_INFO req_info;

HOST_INFO host_info;

HTTP_INFO http_info;

void print_error(const char *msg);


int get_http_info(const char *host_name,HTTP_INFO *http_info);

 

//发送http报文请求

int send_http_header(int sfd,HOST_INFO host_info,int startp,int endp);


int send_http_headersize(int sfd,HOST_INFO host_info);
 

//解析http报文头

int parse_http_header(int sfd,HTTP_INFO *http_info);
 

int connect_server(const char *ip_addr,unsigned short port);


//打印进度条

void print_progress_bar(const char *file_name,float sum,float file_size);
 

unsigned long download(int sockfd,int sfd,HOST_INFO host_info,HTTP_INFO http_info); 

//解析url

void parse_http_url(char *url,HOST_INFO *host_info);


//获取下载的文件大小

unsigned long get_file_size(const char *file_name); 

//get请求

void *http_get(void *arg);

 
 
 //获取网络文件大小
 long getsize(void *arg); 


//多线程下载实现函数

void process_create(struct httpdown_info *htdn_info,int thread_num);


int http_file_merge(int num,char file_name[]);


int http(int sockfd, char *url,int thread_num);

#endif
