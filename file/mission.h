#ifndef _MISSION_H_
#define _MISSION_H_
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include "get_php_oper_info.h"

//#include "p2p_download.h"
//#include "p2p_continue.h"
#include "http17.h"
//#include "ftp_download.h"
//struct php_oper;
/*{	
	int php_req_len;
	int file_size;
	int thread_num;
	int server_id[2];
	char file_name[512];
	char operation[10];
	char protocol[5];
};*/

pid_t mission_dis(int sockfd, struct php_oper * php_operation);
#endif
