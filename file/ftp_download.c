#include "ftp.h"
#include "ftp_download.h"
int control_sockfd;              //控制端口的套接字描述符
int recvbytes,sendbytes;
char sendline[MAXSIZE],recvline[MAXSIZE];
pthread_mutex_t ftp_mutex;	//互斥区
long down_size=0 ;

extern char file_info_to_php[];
extern char file_info_php[128];

int ftp_download_main(int sockfd, char * ip, char *filename, int thread_num)
{	
    	int download_flag = 0;
	struct info down_info;	
    	int n=thread_num;	//传递线程数
    	int file_size=0;
	char *md5 = {0};
	struct websocket_res_head * websocket_head_main = (struct websocket_res_head *)malloc(sizeof(struct websocket_res_head *));
  	strcpy(down_info.IP,ip);
	strcpy(down_info.name, filename);
	pthread_mutex_init(&ftp_mutex,NULL);	//初始化互斥
	pthread_mutex_lock(&ftp_mutex);
	printf("111111\t%d\t11111\n",down_size);
	int  downed = progress_bar(n,down_info.name);
	printf("222222\t%d\t22222\n",downed);
	printf("333333\t%d\t33333\n",down_size);
    	pthread_mutex_unlock(&ftp_mutex);
	
    	int res = ftp_download(&down_info,n,&file_size,sockfd);
    	printf("文件返回大小%d\n",file_size);
    	if(res == 1)
    	{
        	//文件合并
        	printf("多线程下载成功，文件正在合并\n");
        	int merge_flag = ftp_file_merge(n,down_info.name);
        	if(merge_flag == 1)
        	{
        		printf("文件合并成功\n");
	    		//计算文件md5,并校验
	    		md5 = ftp_md5_cpy(down_info.name);
	    		if(md5 != NULL)
	    		{
				printf("md5验证成功\n");
        			//md5校验成功，删除临时文件
				printf("+++++++%s\n",md5);
				snprintf(file_info_php, 128, file_info_to_php, md5,down_info.name, file_size);
				send_websocket_res_head(sockfd, websocket_head_main, file_info_php);
				free(websocket_head_main);
				websocket_head_main = NULL;
				res = removedir(down_info.name,n);
				if(res == 1)
					printf("删除临时文件成功\n");
	    		}
		}
    	}
    	printf("download ok \n");
    	pthread_mutex_destroy(&ftp_mutex);
	down_size = 0;
    	download_flag = 1;
	return download_flag;
}









