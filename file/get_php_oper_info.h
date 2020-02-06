#ifndef _GET_PHP_OPER_INFO_
#define _GET_PHP_OPER_INFO_
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>

#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

struct php_oper
{
	int php_req_len;
	int file_size;
	int thread_num;
	int p2p_server_num;
	char p2p_server_ip[2][16];
	int p2p_server_port[2];
	char file_name[512];
	char operation[16];
	char protocol[8];
	char ftp_ip[16];
	char http_url[1024];
	char md5[64];
};

struct websocket_res_head
{
	char fin;
	char opcode;
	char mask;
	unsigned long long payload_length;
	char masking_key[4];
};

#define GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11" 

struct php_oper * get_php_oper(int sockfd);

struct php_oper * get_websocket_oper(char * p);

int shake_hands(int sockfd);

int base64_encode(char *in_str, int in_len, char *out_str);

char * analy_data(int sockfd);

int send_websocket_res_head(int sockfd, struct websocket_res_head * websocket_head, char *message);
#endif

