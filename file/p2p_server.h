#ifndef _P2P_SERVER_
#define _P2P_SERVER_
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

struct p2p_req_info
{
	int req_size;
	int req_start_location;
	char file_location[];
};

int create_listenfd(void);

struct p2p_req_info * get_p2p_req_info(int sockfd);

void *handle_request(void *arg);

int p2p_server_create(void);
#endif 

