#include "get_php_oper_info.h"

struct php_oper *get_websocket_oper(char * p)
{
	int recv_len = strlen(p);	
	struct php_oper *php_operation = (struct php_oper *)malloc(sizeof(struct php_oper));
	char *pos = strstr(p, "Operation: ");
	if(pos)
	{
		sscanf(pos, "%*s %s", &php_operation->operation);
	}
	pos = strstr(p, "Protocol: ");	
	if(pos)
	{
		sscanf(pos, "%*s %s",&php_operation->protocol);
	}
	pos = strstr(p, "File_name: ");	
	if(pos)
	{
		sscanf(pos, "%*s %s",&php_operation->file_name);
	}
	pos = strstr(p, "File_size: ");
	if(pos)
	{
		sscanf(pos, "%*s %d",&php_operation->file_size);
	}
	pos = strstr(p, "Thread_num: ");
	if(pos)
	{
		sscanf(pos, "%*s %d",&php_operation->thread_num);
	}
	pos = strstr(p, "P2p_server_num: ");
	if(pos)
	{
		sscanf(pos, "%*s %d",&php_operation->p2p_server_num);
		printf("server_num:%d\n",php_operation->p2p_server_num);
	}
	pos = strstr(p, "P2p_server_ip: ");
	if(pos)
	{
		sscanf(pos, "%*s %s %s",&php_operation->p2p_server_ip[0],&php_operation->p2p_server_ip[1]);
	}
	pos = strstr(p, "P2p_server_port: ");
	if(pos)
	{
		sscanf(pos, "%*s %d %d",&php_operation->p2p_server_port[0],&php_operation->p2p_server_port[1]);
	}
	pos = strstr(p, "Http_url: ");
	if(pos)
	{
		sscanf(pos, "%*s %s",&php_operation->http_url);
	}
	pos = strstr(p, "Ftp_ip: ");
	if(pos)
	{
		sscanf(pos, "%*s %s",&php_operation->ftp_ip);
	}
	pos = strstr(p, "Md5: ");
	if(pos)
	{
		sscanf(pos, "%*s %s",&php_operation->md5);
	}
	php_operation->php_req_len = recv_len;
	return php_operation;
}

struct php_oper *get_php_oper(int sockfd)
{
	printf("qunalile\n");
	int recv_num;
	int flag = 0;
	int recv_len = 0;
	char p[1024] = {0};
	while(recv_len<1023)
	{
		recv_num = recv(sockfd, p + recv_len, 1, 0);
		if(recv_num < 1)
		{
			break;
		}
		if((p[recv_len] == '\r'&&(flag == 0||flag == 2)) || (p[recv_len]=='\n'&&(flag == 1 || flag == 3)))
		{	
			flag ++; 
		}
		else
		{
			flag = 0;
		}
		recv_len += recv_num;
		if(flag ==4)
		{
			break;
		}
	}
	struct php_oper *php_operation = (struct php_oper *)malloc(sizeof(struct php_oper));
	printf("*****++++++%s\n",p);
	//sleep(20);
	char *pos = strstr(p, "Operation: ");
	if(pos)
	{
		sscanf(pos, "%*s %s", &php_operation->operation);
	}
	pos = strstr(p, "Protocol: ");	
	if(pos)
	{
		sscanf(pos, "%*s %s",&php_operation->protocol);
	}
	pos = strstr(p, "File_name: ");	
	if(pos)
	{
		sscanf(pos, "%*s %s",&php_operation->file_name);
	}
	pos = strstr(p, "File_size: ");
	if(pos)
	{
		sscanf(pos, "%*s %d",&php_operation->file_size);
	}
	pos = strstr(p, "Thread_num: ");
	if(pos)
	{
		sscanf(pos, "%*s %d",&php_operation->thread_num);
	}
	pos = strstr(p, "P2p_server_num: ");
	if(pos)
	{
		sscanf(pos, "%*s %d",&php_operation->p2p_server_num);
	}
	pos = strstr(p, "P2p_server_ip: ");
	if(pos)
	{
		sscanf(pos, "%*s %s %s",&php_operation->p2p_server_ip[0],&php_operation->p2p_server_ip[1]);
	}
	pos = strstr(p, "P2p_server_port: ");
	if(pos)
	{
		sscanf(pos, "%*s %d %d",&php_operation->p2p_server_port[0],&php_operation->p2p_server_port[1]);
	}
	php_operation->php_req_len = recv_len;
	return php_operation;
}

int shake_hands(int sockfd)
{
	char buffer[1024] = {0};
	int res;
	char key[] = {0};
	char *pos;
	bzero(&buffer,sizeof(buffer));
	char sec_accept[32] = {0};
	unsigned char sha1_data[SHA_DIGEST_LENGTH + 1] = {0};
	char response_head[1024] = {0};
	res = recv(sockfd, buffer,sizeof(buffer),0);
	//printf("++res:%d\n%s\n",res,buffer);
	pos = strstr(buffer,"Sec-WebSocket-Key: ");
	if(pos)
	{
		sscanf(pos, "%*s %s",&key);
	}
	//printf("***GFHkey:%s\n",key);
	strcat(key,GUID);
	printf("new key:%s\n",key);
	SHA1((unsigned char*)&key, strlen(key), (unsigned char *)&sha1_data);
	base64_encode(sha1_data, strlen(sha1_data), sec_accept);
	sprintf(response_head,"HTTP/1.1 101 Switching Protocols\r\nUpgrade:websocket\r\nConnection:Upgrade\r\nSec-WebSocket-Accept:%s\r\n\r\n",sec_accept);
	printf("response head:%s\n",response_head);
	res = send(sockfd, response_head, sizeof(response_head), 0);
	return 0;
}

int base64_encode(char *in_str, int in_len, char *out_str)
{
	/*
	 BIO *b64 = NULL;
	 BIO *bmem = NULL;
	 BUF_MEM *b_ptr = NULL;
	// size_t size = 0;
	//if (in_str == NULL || out_str == NULL)
	//	return -1;
	
	b64 = BIO_new(BIO_f_base64());
	bmem = BIO_new(BIO_s_mem());
	b64 = BIO_push(b64, bmem);

	BIO_write(b64, in_str, in_len);
	BIO_flush(b64);

	BIO_get_mem_ptr(b64, &b_ptr);
	memcpy(out_str, b_ptr->data, b_ptr->length);
	out_str[b_ptr->length] = 0;
	//size = bptr->length;

	BIO_free_all(b64);
	return 1;*/
	 BIO *b64, *bio;
	 BUF_MEM *bptr = NULL;
	 size_t size = 0;
	if (in_str == NULL || out_str == NULL)
		return -1;
	
	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64, bio);

	BIO_write(bio, in_str, in_len);
	BIO_flush(bio);

	BIO_get_mem_ptr(bio, &bptr);
	memcpy(out_str, bptr->data, bptr->length);
	out_str[bptr->length - 1] = '\0';
	size = bptr->length;

	BIO_free_all(bio);
	
	return size;
}

char * analy_data(int sockfd)
{
	int len = 0;
	char buffer[1024] = {0};
	len = recv(sockfd, buffer, sizeof(buffer), 0);
	printf("lllll%d\n",len);
	if(len == 8 || len == 0)
		return "";
	printf("sssss%s\n",buffer);
//	sleep(200);
	char * data;
	char fin, maskFlag,masks[4];
	char * payloadData;
	char temp[8];
	unsigned long n, payloadLen=0;
	unsigned short usLen=0;
	int i=0; 
	
	if (len < 2) 
	{
		return NULL;
	}
				  
	fin = (buffer[0] & 0x80) == 0x80;
	if (!fin)
	{
		return NULL;
	}
					       
	maskFlag = (buffer[1] & 0x80) == 0x80;
	if (!maskFlag)
	{
		return NULL;
	}
							  
	payloadLen = buffer[1] & 0x7F;
	if (payloadLen == 126)
	{      
		memcpy(masks,buffer+4, 4);      
		payloadLen =(buffer[2]&0xFF) << 8 | (buffer[3]&0xFF);  
		payloadData=(char *)malloc(payloadLen);
		memset(payloadData,0,payloadLen);
		memcpy(payloadData,buffer+8,payloadLen);
	}
	else if (payloadLen == 127)
	{
		memcpy(masks,buffer+10,4);  
		for ( i = 0; i < 8; i++)
		{
			temp[i] = buffer[9 - i];
		}
		memcpy(&n,temp,8);  	
		payloadData=(char *)malloc(n); 
		memset(payloadData,0,n); 
		memcpy(payloadData,buffer+14,n);     
		payloadLen=n;    
	}
	else
	{   
		memcpy(masks,buffer+2,4);    
		payloadData=(char *)malloc(payloadLen);
		memset(payloadData,0,payloadLen);
		memcpy(payloadData,buffer+6,payloadLen); 
	}
	
	for (i = 0; i < payloadLen; i++)
	{
		payloadData[i] = (char)(payloadData[i] ^ masks[i % 4]);
	}
	printf("data(%d):%s\n",payloadLen,payloadData);
	//sleep(200);
	return payloadData;
}


int send_websocket_res_head(int sockfd, struct websocket_res_head * websocket_head, char *message)
{
	int send_flag = 0;
//	printf("准备返回值\n");
	char * data =NULL;
	unsigned long n;
	unsigned long len = 0;
	//char *message = "kua yue qian gou duan de hi!!";
	n = strlen(message);
	if(n < 126)
	{
		data = (char *)malloc(n + 2);
		memset(data, 0, n + 2);
		data[0] = 0x81;
		data[1] = n;
		memcpy(data + 2, message, n);
		len = n + 2;
	}
	else if(n < 0xFFFF)
	{
		data = (char *)malloc(n + 4);
		memset(data, 0, n + 4);
		data[0] = 0x81;
		data[1] = 126;
		data[2] = ( n & 0xFF);
		data[3] = (n & 0xFF >> 8);
		memcpy(data + 4, message, n);
		len = n + 4;
		
	}
	else
	{
		data[0] = 0x81;
		data[1] = 127;
		data[2] = (n & 0x000000ff);
		data[3] = ((n & 0x0000ff00) >> 8);
		data[4] = ((n & 0x00ff0000) >> 16);
		data[5] = ((n & 0xff000000) >> 24);
		data[6] = 0;
		data[7] = 0;
		data[8] = 0;
		data[9] = 0;
		memcpy(data + 4, message, n);
		len = n + 10;
	}

	//printf("len:%ld\ndata:%s\n",len,data);
	send(sockfd, data, len, 0);
	return send_flag;
}



























