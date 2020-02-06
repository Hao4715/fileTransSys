#ifndef FTP_H
#define FTP_H

#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<strings.h>
#include<unistd.h>
#include<netinet/in.h>
#include<netdb.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include <pthread.h>
#include<malloc.h>
#include <memory.h>
#include <openssl/md5.h>
#define SERV_PORT 21
#define MAXSIZE 1024
#define READ_DATA_SIZE	1024
#define MD5_SIZE		16
#define MD5_STR_LEN		(MD5_SIZE * 2)
#define IP_LONG		33
#include "p2p_download.h"
#include "get_php_oper_info.h"
extern pthread_mutex_t ftp_mutex;
extern long down_size;
extern int control_sockfd;		//控制端口的套接字描述符
extern int recvbytes,sendbytes;	
extern char sendline[MAXSIZE],recvline[MAXSIZE];	//发送缓冲区，接收缓冲区

struct info
{
    char IP[IP_LONG];
    char name[MAXSIZE];
};
struct ftp_md5_info
{
    char filename[MAXSIZE];
    char serv_IP[IP_LONG];
    char fdname[MAXSIZE];
    long size;
};

struct login_IP
{
    int sockfd;
    char serv_IP[IP_LONG];
    char filename[MAXSIZE];
    char fdname[MAXSIZE];
    int offset;		//偏移量
    int max;		//下载大小
};
/*typedef struct
{
    unsigned int count[2];
    unsigned int state[4];
    unsigned char buffer[64];   
}MD5_CTX;

                         
#define F(x,y,z) ((x & y) | (~x & z))
#define G(x,y,z) ((x & z) | (y & ~z))
#define H(x,y,z) (x^y^z)
#define I(x,y,z) (y ^ (x | ~z))
#define ROTATE_LEFT(x,n) ((x << n) | (x >> (32-n)))
#define FF(a,b,c,d,x,s,ac) \
          { \
          a += F(b,c,d) + x + ac; \
          a = ROTATE_LEFT(a,s); \
          a += b; \
          }
#define GG(a,b,c,d,x,s,ac) \
          { \
          a += G(b,c,d) + x + ac; \
          a = ROTATE_LEFT(a,s); \
          a += b; \
          }
#define HH(a,b,c,d,x,s,ac) \
          { \
          a += H(b,c,d) + x + ac; \
          a = ROTATE_LEFT(a,s); \
          a += b; \
          }
#define II(a,b,c,d,x,s,ac) \
          { \
          a += I(b,c,d) + x + ac; \
          a = ROTATE_LEFT(a,s); \
          a += b; \
          }                                            
void MD5Init(MD5_CTX *context);
void MD5Update(MD5_CTX *context,unsigned char *input,unsigned int inputlen);
void MD5Final(MD5_CTX *context,unsigned char digest[16]);
void MD5Transform(unsigned int state[4],unsigned char block[64]);
void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len);
void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len);
*/

int ftp_connect( char *ip,int port,int *control_sockfd);			//建立连接函数，
int ftp_dataconnect(int *data_sockfd, char *ip,int port);	//data
void login(int control_sockfd);			//登录函数	
void ftp_quit(int control_sockfd);         //
int ftp_size( char *filename,int control_sockfd);
void ftp_type(int control_sockfd);			//传输类型，默认二进制传输
int ftp_rest(int control_sockfd,int offset);		//断点续传
int get_dataport(int control_sockfd, char *dataIP, int *dataport);		//获取数据端口，ip
int ftp_md5retr(int control_sockfd, char *filename, char *fdname,int offset,int max);
int ftp_retr(int control_sockfd,int data_sockfd, char *filename, char *fdname,int offset,int max,int sockfd);
int  ftp_download(struct info *,int n,int * back_size,int sockfd);
void * mul_download(void* argv);
void * ftp_md5_download(void* argv);
int ftp_file_merge(int num,char *file_name);
char * ftp_md5_cpy(char *);
int ftp_Compute_file_md5(char *file_path,char *value);
int removedir(char * filename,int n);
int file_del(char *);
int progress_bar(int n,char *filename);
#endif




