#include "ftp.h"


extern char download_per[];
extern char download_per_content[64];
/*
unsigned char PADDING[]={0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
                         
void MD5Init(MD5_CTX *context)
{
     context->count[0] = 0;
     context->count[1] = 0;
     context->state[0] = 0x67452301;
     context->state[1] = 0xEFCDAB89;
     context->state[2] = 0x98BADCFE;
     context->state[3] = 0x10325476;
}
void MD5Update(MD5_CTX *context,unsigned char *input,unsigned int inputlen)
{
    unsigned int i = 0,index = 0,partlen = 0;
    index = (context->count[0] >> 3) & 0x3F;
    partlen = 64 - index;
    context->count[0] += inputlen << 3;
    if(context->count[0] < (inputlen << 3))
       context->count[1]++;
    context->count[1] += inputlen >> 29;
    
    if(inputlen >= partlen)
    {
       memcpy(&context->buffer[index],input,partlen);
       MD5Transform(context->state,context->buffer);
       for(i = partlen;i+64 <= inputlen;i+=64)
           MD5Transform(context->state,&input[i]);
       index = 0;        
    }  
    else
    {
        i = 0;
    }
    memcpy(&context->buffer[index],&input[i],inputlen-i);
}
void MD5Final(MD5_CTX *context,unsigned char digest[16])
{
    unsigned int index = 0,padlen = 0;
    unsigned char bits[8];
    index = (context->count[0] >> 3) & 0x3F;
    padlen = (index < 56)?(56-index):(120-index);
    MD5Encode(bits,context->count,8);
    MD5Update(context,PADDING,padlen);
    MD5Update(context,bits,8);
    MD5Encode(digest,context->state,16);
}
void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len)
{
    unsigned int i = 0,j = 0;
    while(j < len)
    {
         output[j] = input[i] & 0xFF;  
         output[j+1] = (input[i] >> 8) & 0xFF;
         output[j+2] = (input[i] >> 16) & 0xFF;
         output[j+3] = (input[i] >> 24) & 0xFF;
         i++;
         j+=4;
    }
}
void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len)
{
     unsigned int i = 0,j = 0;
     while(j < len)
     {
           output[i] = (input[j]) |
                       (input[j+1] << 8) |
                       (input[j+2] << 16) |
                       (input[j+3] << 24);
           i++;
           j+=4; 
     }
}
void MD5Transform(unsigned int state[4],unsigned char block[64])
{
     unsigned int a = state[0];
     unsigned int b = state[1];
     unsigned int c = state[2];
     unsigned int d = state[3];
     unsigned int x[64];
     MD5Decode(x,block,64);
     FF(a, b, c, d, x[ 0], 7, 0xd76aa478);  1 */

//建立连接函数
int ftp_connect(char *ip,int port,int *control_sockfd)
{
   	//初始化端口信息
	struct sockaddr_in serv_addr;
	//创建socket
    	*control_sockfd = socket(AF_INET,SOCK_STREAM,0);
   	if(*control_sockfd<0)
    	{
		printf("socket fail\n");
	    	return 0;
    	}
    	printf("命令端口号:%d\n",*control_sockfd);
    	//设置sockaddr_in 结构体中的相关参数
    	serv_addr.sin_family=AF_INET;
    	serv_addr.sin_port=htons(port);
    	serv_addr.sin_addr.s_addr=inet_addr(ip);
    	//调用connect函数发起连接
   	if((connect(*control_sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr)))<0)
    	{
	 	printf("connect is error123.please enter the right IP.\n");
		return 0;
    	}
    	memset(recvline,0, sizeof(sendline));
    	recvbytes=recv(*control_sockfd,recvline,sizeof(recvline),0);
    	if(recvbytes==-1)
    	{
		printf("recv is connect error\n");
	   	return 0;
    	}
    	else if(strncmp(recvline,"220",3)==0)
    	{
		printf("%s\n",recvline);
        	return 1;
    	}
    		else 
   		{
        		printf("connect is error!456please enter the right IP.\n");
			return 0;
    		}
	
}


//建立连接函数
int ftp_dataconnect(int *data_sockfd,char *ip,int port)
{
    	//初始化端口信息
	char IP[] = "39.106.152.112";
    	struct sockaddr_in serv_addr;
    	//创建socket
    	*data_sockfd=socket(AF_INET,SOCK_STREAM,0);
    	int fd=*data_sockfd;
    	if(fd<0)
    	{
        	printf("socket fail\n");
        	return 0;
    	}
    	printf("数据端口号：%d\n",fd);
    	//设置sockaddr_in 结构体中的相关参数
    	serv_addr.sin_family=AF_INET;
    	serv_addr.sin_port=htons(port);
    	serv_addr.sin_addr.s_addr=inet_addr(IP);
    	//调用connect函数发起连接
    	if((connect(fd,(struct sockaddr*)&serv_addr,sizeof(serv_addr)))<0)
    	{
    		printf("connect is error.789please enter the right IP.\n");
        	return 0;
    	}
    	else
    	{
        	return 1;
    	}
}
//ftp用户登录主体部分
void login(int control_sockfd)
{       
    	memset(recvline,0, sizeof(sendline));
    	memset(sendline,0,sizeof(sendline));
    	strcat(sendline,"USER ");
    	strcat(sendline,"anonymous");
    	strcat(sendline,"\r\n");
    	printf("--->%s",sendline);
    	sendbytes=send(control_sockfd,sendline,strlen(sendline),0);
    	if(sendbytes==-1)
    	{
        	printf("send is wrong\n");
    	}
    	recvbytes=recv(control_sockfd,recvline,sizeof(recvline),0);
    	if(strncmp(recvline,"331",3)==0)
    	{
        	printf("%s\n",recvline);
    	}
    	else
    	{
        	printf("recv date is error.\n");
    	}
	//密码pass
   	memset(sendline,0,sizeof(sendline));
   	memset(recvline,0, sizeof(sendline));
    	strcat(sendline,"PASS ");
    	strcat(sendline,"pass");
    	strcat(sendline,"\r\n");
    	printf("--->%s",sendline);
    	sendbytes=send(control_sockfd,sendline,strlen(sendline),0);
    	if(sendbytes==-1)
    	{
        	printf("pass send is error\n");
    	}
    	recvbytes=recv(control_sockfd,recvline,sizeof(recvline),0);
    	if(strncmp(recvline,"230",3)==0)
    	{
        	printf("%s\n",recvline);
    	}
    	else 
    	{
        	printf("pass recv is error\n");
    	}
}

//quit函数,control_sockfd,通过实参传递
void ftp_quit(int control_sockfd)
{
    	memset(sendline,0,sizeof(sendline));
    	memset(recvline,0, sizeof(sendline));
    	strcat(sendline,"QUIT\r\n");
    	sendbytes=send(control_sockfd,sendline,strlen(sendline),0);
    	if(sendbytes<0)
    	{
        	printf("quit send is error!\n");
    	}
    	recvbytes=recv(control_sockfd,recvline,sizeof(recvline),0);
    	if(recvbytes<0)
	{
		printf("quit recv is error\n");
	}
	if(strncmp(recvline,"221",3)==0)
	{
		printf("%s\n",recvline);
	}
    	else
    	{
		printf("quit error\n");
    	}
}
 
//获得文件大小//返回值是文件大小
int  ftp_size(char *filename,int control_sockfd)
{
	int S;
    	memset(sendline,0,sizeof(sendline));
    	memset(recvline,0, sizeof(sendline));
    	strcat(sendline,"SIZE ");
    	strcat(sendline,filename);
    	strcat(sendline,"\r\n");
    	sendbytes=send(control_sockfd,sendline,strlen(sendline),0);
    	if(sendbytes<0)
    	{
        	printf("size send is error\n");
		return 0;
    	}
    	recvbytes=recv(control_sockfd,recvline,sizeof(recvline),0);
    	if(strncmp(recvline,"213",3)==0)
    	{
        	printf("%s",recvline);
	    	char s[100],m[5];
	    	memset(s,0,100);
	    	memset(m,0,5);
	    	sscanf(recvline,"%s %s",m,s);	//截取213 后面的字符串
	    	S = atoi(s);		//函数atoi()，char->int
	    	return S;			//返回文件大小S
 	}
    	else
    	{
        	printf("size is error\n");
    		return 0;
	}
}
//二进制传输命令
void ftp_type(int control_sockfd)
{
    	memset(recvline,0, sizeof(sendline));
    	memset(sendline,0,sizeof(sendline));
    	strcat(sendline,"TYPE ");
    	strcat(sendline,"I");
    	strcat(sendline,"\r\n");
    	sendbytes=send(control_sockfd,sendline,strlen(sendline),0);
    	if(sendbytes<0)
    	{
        	printf(" type send is error!\n");
   	}	
    	recvbytes=recv(control_sockfd,recvline,sizeof(recvline),0);
    	if(strncmp(recvline,"200",3)==0)
    	{
        	printf("%s\n",recvline);
    	}	
    	else
    	{
        	printf("type recv is not 200 error!\n");
    	}
}
 
//断点函数的支持
int ftp_rest(int control_sockfd,int offset)
{
    	memset(sendline,0,sizeof(sendline));
    	memset(recvline,0, sizeof(sendline));
    	sprintf(sendline,"%s %d","REST",offset);
    	strcat(sendline,"\r\n");
	printf("%s\n",sendline);
    	sendbytes=send(control_sockfd,sendline,strlen(sendline),0);
    	if(sendbytes<0)
    	{
        	printf("stru send is error!\n");
		return 0;
    	}
    	recvbytes=recv(control_sockfd,recvline,sizeof(recvline),0);
    	if(recvbytes<0)
    	{
        	printf("stru recv is error!\n");
		return 0;
    	}
    	if(strncmp(recvline,"350",3)==0)
    	{
        	printf("%s\n",recvline);
		return 1;
    	}
	else
	{
	 	printf("recv is not 350!\n");
		return 0;
	}
}
 
//设置ftp服务器为被动模式，并解析数据端口
int get_dataport(int control_sockfd,char *dataIP, int *dataport)
{
    	char *find;
    	int a,b,c,d;
    	int pa,pb;
	char f[64]={0};
	find = f;
    	memset(sendline,0,sizeof(sendline));
    	memset(recvline,0, sizeof(sendline));
    	//pasv mode
    	strcat(sendline,"PASV");
    	strcat(sendline,"\r\n");
    	sendbytes=send(control_sockfd,sendline,strlen(sendline),0);
    	if(sendbytes<0)
    	{
        	printf("pasv send is error!\n");
		return 0;
    	}
    	memset(recvline,0, sizeof(sendline));
    	recvbytes=recv(control_sockfd,recvline,sizeof(recvline),0);
    	if(recvbytes<0)
    	{
        	printf("pasv recv is error!\n");
		return 0;
    	}
    	if(strncmp(recvline,"227",3)==0)
    	{
		printf("%s",recvline);
	    	find = strrchr(recvline ,'(');
	    	sscanf(find, "(%d,%d,%d,%d,%d,%d)", &a, &b, &c, &d, &pa, &pb);
	    	sprintf(dataIP, "%d.%d.%d.%d", a, b, c, d);
	    	*dataport = (pa * 256) + pb;
		return 1;
    	}
    	else
    	{
		return 0;
    	}
}
//下载的实现函数
int ftp_md5retr(int control_sockfd,char *filename,char *fdname,int offset,int max)
{
    	struct sockaddr_in serv_addr;
    	FILE *fd;
 	int  f_offset;
	char dataIP[33];
    	int data_sockfd;
        int dataport;
        int ret;
       	char buffer[MAXSIZE];
       	int i;
	fd=fopen(fdname,"ab");		//以读方式打开下载文件
	if(fd!=NULL)
    	{
        	//printf("有文件，继续下载\n");
        	fseek(fd, 0, SEEK_END);        
        	f_offset = ftell(fd);		//文件偏移量+多线程偏移量
        	if (f_offset == max)
        	{
            		printf("偏移量：%d\tmd5文件，已下载\n", f_offset);
            		fclose(fd);
        		close(data_sockfd);
        		ftp_quit(control_sockfd);
            		return 1;
        	}
        	ret = get_dataport(control_sockfd, dataIP, &dataport);
        	if (ret != 1)
		{
			printf("get_dataport error\n");
            		exit(1);
        	}
        	if(ret = ftp_dataconnect(&data_sockfd, dataIP, dataport) != 1)
            	{
			printf("dataconnect fail\n");
			exit(1);
		}
        	//send the command retr;
        	memset(sendline, 0, sizeof(sendline));
        	memset(recvline, 0, sizeof(sendline));
        	strcat(sendline, "RETR ");
        	strcat(sendline, filename);
        	strcat(sendline, "\r\n");
        	printf("%s", sendline);
        	sendbytes = send(control_sockfd, sendline, strlen(sendline), 0);
        	if (sendbytes < 0)
		{
        		printf("retr send is error!\n");
            		fclose(fd);
        		close(data_sockfd);
        		ftp_quit(control_sockfd);
        	}
        	recvbytes = recv(control_sockfd, recvline, sizeof(recvline), 0);
        	if (recvbytes < 0)
		{
        		printf("retr recv is error!\n");
            		fclose(fd);
        		close(data_sockfd);
        		ftp_quit(control_sockfd);
        	}
        	if (strncmp(recvline, "150", 3) == 0)
		{
           		printf("%s", recvline);
        	}
		else
		{
            		printf("recv error is not 150\n");
            		fclose(fd);
        		close(data_sockfd);
        	}
        	for (i = f_offset; i < max; i + recvbytes)
		{
            		memset(buffer, 0, sizeof(buffer));
            		recvbytes = recv(data_sockfd, buffer, sizeof(buffer), 0);
            		if (recvbytes < 0)
			{
                		printf("data recv error\n");
                		close(data_sockfd);
            		} 
			else if (recvbytes == 0)
					break;
            			else
				{
			                fwrite(buffer, recvbytes, 1, fd);
				}
		
        	}
        	fclose(fd);
        	close(data_sockfd);
		ftp_quit(control_sockfd);
		        
    	}
    	/*memset(recvline,0, sizeof(sendline));
    	recvbytes=recv(control_sockfd,recvline,sizeof(recvline),0);
    	if(recvbytes<0)
    	{
 		printf("recv is error\n");
    	}
    	if(strncmp(recvline,"226",3)==0)
    	{
	 	printf("%s\n",recvline);
    	}
    	else
	   	printf("226 error\n");*/
}

//下载的实现函数
int ftp_retr(int control_sockfd,int data_sockfd,char *filename,char *fdname,int offset,int max,int sockfd)
{
   	// struct sockaddr_in serv_addr;
   	FILE *fd;
    	int i;
	int bb;
    	int ret;
	int file_size;	//文件大小 
    	int  f_offset;
    	int reset;
    	int get_size;  
	struct websocket_res_head * websocket_head = (struct websocket_res_head * )malloc(sizeof(struct websocket_res_head));
    	file_size = ftp_size(filename,control_sockfd);
    	printf("文件大小用以计算进度百分比%d\n",file_size);
	if(file_size>1024000)
		bb = 1024;
	else 
		bb = 65535;
	char buffer[bb];
    	fd=fopen(fdname,"ab");		//以读方式打开下载文件
    	if(fd!=NULL)
    	{
        	fseek(fd, 0, SEEK_END);        //
        	f_offset = ftell(fd);        //文件偏移量+多线程偏移量
		//down_size = down_size +f_offset;			
        	if (f_offset == (max - offset))
		{
            		printf("偏移量：%d\n", f_offset);        //
            		printf("该部分已下载，不再请求，关闭连接\n");
            		fclose(fd);
	    		ftp_quit(control_sockfd);        
	    		return 1;
        	}
        	f_offset = f_offset+offset;			//文件大小+偏移量
        	printf("偏移量：%d\n",f_offset);			
		//rest		//断点续传
        	ftp_rest(control_sockfd,f_offset);
        	//send the command retr;
		memset(sendline,0,sizeof(sendline));
		memset(recvline,0, sizeof(sendline));
		strcat(sendline,"RETR ");
		strcat(sendline,filename);
		strcat(sendline,"\r\n");
		printf("%s",sendline);
		sendbytes=send(control_sockfd,sendline,strlen(sendline),0);
		if(sendbytes<0)
 		{
			printf("retr send is error!\n");
        	    	fclose(fd);
	    		ftp_quit(control_sockfd);        
		}	
		recvbytes=recv(control_sockfd,recvline,sizeof(recvline),0);
		if(recvbytes<0)
		{
		        printf("retr recv is error!\n");
        	    	fclose(fd);
		    	ftp_quit(control_sockfd);        
		}
		if(strncmp(recvline,"150",3)==0)
		{
		        printf("%s",recvline);
		}
		else
		{
			printf("recv is not 150\n");
        	    	fclose(fd);
	    		ftp_quit(control_sockfd);        
		}	
		for( i = f_offset;;)
		{
    			memset(buffer,0,sizeof(buffer));
   	 		recvbytes=recv(data_sockfd,buffer,sizeof(buffer),0);
	     		if(recvbytes<0)
    			{
             		    	printf("data recv error\n");
            			fclose(fd);
            			close(data_sockfd);
	    			ftp_quit(control_sockfd);        
				return 0;
        		}
        		if(recvbytes==0)
        		{	
		    		break;
			}
        		i=i+recvbytes ;
        		if( i < max )
        		{
				get_size = fwrite(buffer,recvbytes,1,fd) *recvbytes;
        	    		pthread_mutex_lock(&ftp_mutex);		//进入互斥区
				down_size = down_size +get_size;			
				float percent = ((float)down_size/file_size) * 100;	//打印百分比
				snprintf(download_per_content, 64, download_per, percent);
				send_websocket_res_head(sockfd, websocket_head, download_per_content);
				//printf("99999输出进度%f%%999999",percent*100);
				pthread_mutex_unlock(&ftp_mutex);	//离开互斥区
        		}	
        		else
        		{
        		    	i=i-recvbytes;
        		    	reset = max-i;
        		    	get_size = fwrite(buffer,reset,1,fd) * reset;
        		    	pthread_mutex_lock(&ftp_mutex);		//进入互斥区
				down_size = down_size +get_size;			
				float percent = ((float)down_size/file_size) * 100;	//打印百分比
				snprintf(download_per_content, 64, download_per, percent);
				send_websocket_res_head(sockfd, websocket_head, download_per_content);
				//printf("777777输出进度%f%%777777",percent*100);
				pthread_mutex_unlock(&ftp_mutex);	//离开互斥区
        		    	fclose(fd);
			        printf("传输完成，关闭数据端口\n");
				break;
        		}
		}
	}
	close(data_sockfd);
        ftp_quit(control_sockfd);
}
//下载实现
int  ftp_download(struct info *q,int  n,int * back_size,int sockfd)
{
    	char number[8];
    	char fname[MAXSIZE];
    	int i;
    	pthread_t p;
    	struct ftp_md5_info * M = (struct ftp_md5_info *)calloc(1,sizeof(struct ftp_md5_info));
    	strncpy(M->serv_IP,q->IP,sizeof(M->serv_IP));
    	strcpy(M->filename,q->name);
    	strcpy(M->fdname,q->name);
    	M->size = 0;
    	pthread_create(&p,NULL,ftp_md5_download,M);
    	pthread_join(p,NULL);
    	*back_size = M->size;//返回下载文件大小
        
	char location[MAXSIZE]={0};
        sprintf(location,"%s%s",q->name,"_temp");
        int j = mkdir(location,0777);
	if(j<0)
        	printf("mkdir is error!\n");
        chdir(location);
        int space = M->size / n;
        int num_thread = n;
        pthread_t thread[num_thread];//线程id
        struct login_IP *L = (struct login_IP *) calloc(num_thread, sizeof(struct login_IP));
        for (i = 0; i < num_thread; i++)
	{
            	strncpy(L[i].serv_IP, q->IP, sizeof(L[i].serv_IP));
             	strcpy(L[i].filename, q->name);
		L[i].sockfd = sockfd;
             	memset(number, 0, sizeof(number));
             	memset(fname, 0, sizeof(fname));
             	sprintf(number, "%d", i);
             	sprintf(fname, "%s%s", q->name, number);
             	strcpy(L[i].fdname, fname);
	     	if(i != num_thread-1)
	     	{
             		L[i].offset = i * space;
             		L[i].max = (i + 1) * space;
             		int res = pthread_create(&(thread[i]), NULL, mul_download, &(L[i]));
 			if (res != 0)
	     		{
                		printf("integral thread %d creation failed\n", i);
             		}	
	     		sleep(1);    
             	}
	     	else
	     	{
                 	L[i].offset = i * space;
        	 	L[i].max = M->size;
                 	int res = pthread_create(&(thread[i]), NULL, mul_download, &(L[i]));
                 	if (res != 0) 
                 		printf("integral thread %d creation failed\n", i);
             	}	
	}
        for (i = 0; i < num_thread; i++)
        {
        	pthread_join(thread[i], NULL);
        }
	chdir("..");
       	printf("下载完成\n");
   	return 1;
}
//多线程登陆下载
void * mul_download(void *argv)
{
    	char dataIP[33];
    	int data_sockfd= 0;
    	int dataport=0;
    	int ret;
    	struct login_IP * login_info =(struct login_IP *)argv;
	printf("%s\t%s\t%s\t%d\t%d\n",login_info->serv_IP,login_info->filename,login_info->fdname,login_info->offset,login_info->max);
    	int backsockfd=0;
    	int con = ftp_connect(login_info->serv_IP,SERV_PORT,&backsockfd);
    	if(con==1)
    	{
        	login(backsockfd);          //登录
        	ftp_type(backsockfd);       //二进制传输
		ret = get_dataport(backsockfd,dataIP,&dataport);
		if(ret != 1 )
		{
	    		exit(1);
		}
		if(ret = ftp_dataconnect(&data_sockfd,dataIP,dataport)!=1)
			printf("dataconnect\n");
        	ftp_retr(backsockfd,data_sockfd,login_info->filename,login_info->fdname,login_info->offset,login_info->max,login_info->sockfd);                 //RETR下载文件
        	//ftp_quit(backsockfd);
    	}
    	return 0;
}


//该线程获得下载文件的大小，并下载文件对应的md5文件
void * ftp_md5_download(void* argv)
{
    	struct ftp_md5_info * down = (struct ftp_md5_info *)argv;
    	int backsockfd=0;
    	int con = ftp_connect(down->serv_IP,SERV_PORT,&backsockfd);
    	if(con==1)
    	{
        	login(backsockfd);          //登录
        	ftp_type(backsockfd);       //二进制传输*/
        	int size;                       //记录文件大小
        	size = ftp_size(down->filename,backsockfd);               //获得文件大小,
        	printf("下载文件的大小为%d B\n",size);
        	down->size = size;			//回传文件的size
	    	int md5_size;                   //记录md5文件大小,
        	strcat(down->filename,".md5");
        	strcat(down->fdname,".md5");
        	md5_size = ftp_size(down->filename,backsockfd);               //获得md5文件大小,
        	printf("MD5文件大小为%d B\n",md5_size);
        	ftp_md5retr(backsockfd,down->filename,down->fdname,0,md5_size);                 //RETR下载md5文件
        	ftp_quit(backsockfd);
    	}	
    	return 0;
}

int ftp_file_merge(int num,char *file_name)
{
	char location[MAXSIZE]={0};
    	sprintf(location,"%s%s",file_name,"_temp");
    	int i;
    	char numchar[8];
    	char fname[MAXSIZE];
    	FILE *target, *current;
    	int length;		//当前文件的长度
    	char *buf;
    	target = fopen(file_name,"wb");
    	chdir(location);
    	for(i=0;i<num;i++)
    	{
		memset(numchar,0,sizeof(numchar));
		memset(fname,0,sizeof(fname));
		sprintf(numchar, "%d", i);
        	sprintf(fname, "%s%s", file_name, numchar);
		current = fopen(fname,"rb");
 		fseek(current,0,SEEK_END);
		length = ftell(current);
		printf("length :%d\n",length);
		rewind(current);
		buf = (char *)malloc(length);
		fread(buf,1,length,current);
		fwrite(buf,1,length,target);
		fclose(current);
 	}	
    	fclose(target);
    	chdir("..");		//到达上一级目录
    	return 1;
}
//md5校验
char * ftp_md5_cpy(char *file_path)
{
	//chdir("..");
	int ret;
	char *md5buf;
        char test[]=".md5";
	char md5_file[MAXSIZE];
	memset(md5_file,0,sizeof(md5_file));
	sprintf(md5_file,"%s%s",file_path,test);
	FILE *md5fd;
	int len;
	md5fd = fopen(md5_file,"rb");
	fseek(md5fd,0,SEEK_END);
	len = ftell(md5fd);
	rewind(md5fd);
	md5buf = (char *)malloc(len);
	fread(md5buf,1,len,md5fd);
	fclose(md5fd);
	char md5_str[MD5_STR_LEN + 1];
	ret = ftp_Compute_file_md5(file_path, md5_str);//计算文件的MD5值
	if (0 == ret)
	{
		printf("%s\n", md5_str);
		if(strncmp(md5_str,md5buf,32)==0)
		{
			return md5_str;
		}
	}
	return NULL;

}
 
int ftp_Compute_file_md5(char *file_path, char *md5_str)
{
	int i;
	int fd;
	int ret;
	unsigned char data[READ_DATA_SIZE];
	unsigned char md5_value[MD5_SIZE];
	MD5_CTX md5;
	 
	fd = open(file_path, O_RDONLY);
	// init md5
	MD5_Init(&md5);
 
	while (1)
	{
		ret = read(fd, data, READ_DATA_SIZE);
	if (-1 == ret)
		{
			perror("read");
			return -1;
		}
 
		MD5_Update(&md5, data, ret);
 
		if (0 == ret || ret < READ_DATA_SIZE)
		{
			break;
		}
	}
 
	close(fd);
 
	MD5_Final(md5_value,&md5);
 
	for(i = 0; i < MD5_SIZE; i++)
	{
		snprintf(md5_str + i*2, 2+1, "%02x", md5_value[i]);
	}
	md5_str[MD5_STR_LEN] = '\0'; // add end
 
	return 0;
}

//删除临时文件
int removedir(char * filename,int n)
{
	int i,res;
	char number[8];
	char fname[MAXSIZE];	
	char del_dir[MAXSIZE]={0};
	sprintf(del_dir,"%s%s",filename,"_temp");
	
	chdir(del_dir);
	for(i=0;i<n;i++)
	{
		memset(number,0,sizeof(number));
		memset(fname,0,sizeof(fname));
		sprintf(number,"%d",i);
		sprintf(fname,"%s%s",filename,number);
		remove(fname);
	}
	chdir("..");
	res = remove(del_dir);
	if(res == 0)
		return 1;
	return 0;

}
//删除md5文件
/*int file_del(char * del_name)
{
	char del_md5[MAXSIZE]={0};
	sprintf(del_md5,"%s%s",del_name,".md5");
 	int ret = remove(del_md5);
	if(ret==0)
		return 1;
	return 0;
}*/
//下载中断，进度条继续//返回以下文件大小
int progress_bar(int n, char * filename)
{
        int i;
        char numchar[8];
        char fname[MAXSIZE];
        FILE *current;
        int length;             //当前文件的长度
	char location[MAXSIZE]={0};
        sprintf(location,"%s%s",filename,"_temp");
	int exist = access(location,0);
	if(exist !=0)
	{
		printf("文件夹不存在，新建下载\n");
		down_size=0;	
		
	}
	else
	{
        	chdir(location);
        	for(i=0;i<n;i++)
        	{
               		memset(numchar,0,sizeof(numchar));
                	memset(fname,0,sizeof(fname));
                	sprintf(numchar, "%d", i);
                	sprintf(fname, "%s%s", filename, numchar);
                	current = fopen(fname,"rb");
			if(!current)
				continue;
                	fseek(current,0,SEEK_END);
                	length = ftell(current);
                	printf("length :%d\t%d\n",length,i);
                	down_size = down_size + length;
			printf("5555%d\n",down_size);
			rewind(current);
			fclose(current);
		}
		chdir("..");
	}
	return down_size;

}
