#include "http17.h" 
#define READ_DATA_SIZE	1024
#define MD5_SIZE		16
#define MD5_STR_LEN		(MD5_SIZE * 2)
#define THREADNUM 4
extern char download_per[];
extern char download_per_content[64];
char file_info_to_php[] = 
	"%s\r\n%s\r\n%d";
char file_info_php[128] = {0};
int showbyte=0;
long totalbyte=0;
float percent=0.00;
int Compute_file_md5(const char *file_path, char *value);
char * md5_cpy(char *file_path,char * md5_file);
char * md5_cpy(char *file_path,char*md5_file)//比较md5的值是否相同
{
	int ret;
	char md5buf[36]={0};
	char md5_str[MD5_STR_LEN + 1];
	FILE *md5_fp;
	char *md5_return = "";
	md5_fp = fopen(md5_file,"rb");
	if(md5_fp == NULL)
		printf("hahahaahahdaidaonole\n");
	printf("%s\n",md5_file);
	printf("ddd\n");
	bzero(&md5buf,sizeof(md5buf));
	int test = fread(md5buf,1,sizeof(md5buf),md5_fp);
	printf("tttttt%d\n",test);
	printf("%s\n",md5buf);
	fclose(md5_fp);
	ret = Compute_file_md5(file_path, md5_str);//计算文件的MD5值
	if (0 == ret)
	{
		printf("%s\n", md5_str);
		if(strncmp(md5_str,md5buf,32)==0)
		{
			md5_return = md5_str;
			//printf("md5值一样\n");
			return md5_return;	
		}
		else		
			return 0;
	}

}
 
int Compute_file_md5(const char *file_path, char *md5_str)//计算文件的md5值
{
	int i;
	int fd;
	int ret;
	unsigned char data[READ_DATA_SIZE];
	unsigned char md5_value[MD5_SIZE];
	MD5_CTX md5;
 
	fd = open(file_path, O_RDONLY);
	if (-1 == fd)
	{
		perror("open");
		return -1;
	}
 
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
 
	MD5_Final(md5_value, &md5);
 
	for(i = 0; i < MD5_SIZE; i++)
	{
		snprintf(md5_str + i*2, 2+1, "%02x", md5_value[i]);
	}
	md5_str[MD5_STR_LEN] = '\0'; // add end
 
	return 0;
}





void print_error(const char *msg)

{

	perror(msg);

	exit(1);

}

 

//获取服务端地址和别名

int get_http_info(const char *host_name,HTTP_INFO *http_info)

{

	//判断是否为IP地址

	int a,b,c,d;

	if (sscanf(host_name,"%d.%d.%d.%d",&a,&b,&c,&d) == 4){

		if (a >= 0x0 && a <= 0xff && b >= 0x0 && b <= 0xff && c >=0 && c <= 0xff && d >=0 && d <= 0xff){

			strcpy(http_info->ip_addr,host_name);

			strcpy(http_info->aliases,"no aliases");

			return 0;

		}

	}

	struct hostent *phost = gethostbyname(host_name);

	if (!phost)

		return -1;

	/*char **ptr = NULL,temp[INET_ADDRSTRLEN];
	for (ptr = phost->h_addr_list;*ptr != NULL;ptr++)
	{
		bzero(temp,sizeof(temp));
		printf("对方服务器地址:%s\n",inet_ntop(AF_INET,*ptr,temp,sizeof(temp)));
	}
	for (ptr = phost->h_aliases;*ptr != NULL;ptr++)
	{
		printf("域名别名:%s\n",*ptr);
	}*/

	inet_ntop(AF_INET,phost->h_addr_list[0],http_info->ip_addr,sizeof(http_info->ip_addr));//将网络字节序二进制值转换成点分十进制串

	if (*phost->h_aliases != NULL)

		strcpy(http_info->aliases,phost->h_aliases[0]);

	else

		strcpy(http_info->aliases,"no aliases");

	return 0;

}

 

//发送http报文请求

int send_http_header(int sfd,HOST_INFO host_info,int startp,int endp)

{

	char http_header[BUFSIZ];

	bzero(http_header,sizeof(http_header));//整个空间块置为0

	sprintf(http_header,\

			"GET %s HTTP/1.1\r\n"\
			"Host: %s\r\n"\
			"Range:bytes=%d-%ld\r\n"\
			"Connection: Keep-Alive\r\n"\

			"Content-Type: application/octet-stream\r\n"\

			"\r\n",host_info.file_path,host_info.host_name,req_info.startp,req_info.endp);

	return write(sfd,http_header,strlen(http_header));

}


//发送请求 来获取下载文件的长度
int send_http_headersize(int sfd,HOST_INFO host_info)

{

	char http_header[BUFSIZ];

	bzero(http_header,sizeof(http_header));//整个空间块置为0

	sprintf(http_header,\

			"GET %s HTTP/1.1\r\n"\
			"Host: %s\r\n"\
			
			"Connection: Keep-Alive\r\n"\

			"Content-Type: application/octet-stream\r\n"\

			"\r\n",host_info.file_path,host_info.host_name);

	return write(sfd,http_header,strlen(http_header));

}

 

//解析http报文头

int parse_http_header(int sfd,HTTP_INFO *http_info)

{

	char buffer[BUFSIZ],temp[BUFSIZ],*ptr;

	bzero(buffer,sizeof(buffer));

	bzero(temp,sizeof(temp));

	int len,n = 0;

	while((len = read(sfd,buffer,1)) != 0){//参数fd所指的文件传送nbyte个字节到buf指针所指的内存中

		temp[n] = *buffer;

		if (*buffer == '\n'){

			ptr = strstr(temp,"HTTP/1.1");//字符串 temp 中查找第一次出现字符串 HTTP/1.1 的位置，不包含终止符 '\0'

			if (ptr != NULL){

				ptr = strchr(ptr,' ');//在参数 ptr 所指向的字符串中搜索第一次出现字符 的位置

				ptr++;

				http_info->status_code = atoi(ptr);// 把参数 ptr 所指向的字符串转换为一个整数（类型为 int 型）

			}

			ptr = strstr(temp,"Content-Length:");

			if (ptr != NULL){

				ptr = strchr(ptr,':');

				ptr++;

				http_info->file_size = strtoul(ptr,NULL,10);

			}

			ptr = strstr(temp,"Content-Type:");

			if (ptr != NULL){

				ptr = strchr(ptr,':');	

				ptr++;

				strcpy(http_info->content_type,ptr);

				http_info->content_type[strlen(ptr) - 1] = '\0';

			}

			

			if (temp[0] == '\r' && temp[1] == '\n')

				break;

			bzero(temp,sizeof(temp));

			n = -1;

		}

		n++;

	}


	return sfd;

}

 
//连接网络资源服务器
int connect_server(const char *ip_addr,unsigned short port)

{

	int sfd = socket(AF_INET,SOCK_STREAM,0);

	if (sfd == -1)

		print_error("socket");

 

	struct sockaddr_in serv_addr;

	serv_addr.sin_family = AF_INET;

 

	//默认80端口

	serv_addr.sin_port = htons(port);

 

	//根据域名获取到ip地址

	int ret = inet_pton(AF_INET,ip_addr,&serv_addr.sin_addr.s_addr);

	if (ret == -1)

		print_error("inet_pton");

	

	int flags = fcntl(sfd,F_GETFL,0);

 

	flags |= O_NONBLOCK;

	fcntl(sfd,F_SETFL,flags);

 

	//连接服务器

	ret = connect(sfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));

	if (ret != 0 && errno == EINPROGRESS){

		printf("connecting server,please wait for 10 seconds\n");

		struct pollfd pfd;

		socklen_t optlen;

		int optval,timeout = 10 *1000;

		pfd.fd = sfd;

		pfd.events = POLLOUT;

		

		if ((ret = poll(&pfd,1,timeout)) == 1){

			//建立连接成功

			optlen = sizeof(optval);

			ret = getsockopt(sfd,SOL_SOCKET,SO_ERROR,&optval,&optlen);

			errno = optval;

			ret = optval == 0 ? 0 : -1;

			if (!ret)

				printf("connection %s\n",strerror(errno));

		}

		else if (!ret){

			//连接超时

			errno = ETIMEDOUT;

			ret = -1;

		}

		else

			print_error("poll failed");

	}

	flags &= ~O_NONBLOCK;

	fcntl(sfd,F_SETFL,flags);

	return ret == -1 ? -1 : sfd;

}

 

//打印进度条

void print_progress_bar(const char *file_name,float sum,float file_size)

{

	float percent = (sum / file_size) * 100;

	char *sign = "#";

	if ((int)percent != 0){

		sign = (char *)malloc((int)percent + 1);

		strncpy(sign,"####################################################",(int) percent);

	}

	printf("%s %7.2f%% [%-*.*s] %.2f/%.2f mb\r",file_name,percent,50,(int)percent / 2,sign,sum / 1024.0 / 1024.0,file_size / 1024.0 / 1024.0);

	if ((int)percent != 0)

		free(sign);

	fflush(stdout);

}

 
//下载
unsigned long download(int sockfd, int sfd,HOST_INFO host_info,HTTP_INFO http_info)

{

	umask(0111);

	int len;


	pthread_mutex_t work_mutex;
	int res;

	res = pthread_mutex_init(&work_mutex, NULL);
	if(res!=0){
		perror("初始化互斥量失败");
		exit(EXIT_FAILURE);
	}


	unsigned long sum = 0;

	char buffer[BUFSIZ] = { 0 };
	struct websocket_res_head * websocket_head = (struct websocket_res_head *)malloc(sizeof(struct websocket_res_head *));
	FILE *fd;
	fd = fopen( host_info.new_name,"ab");

	if (fd ==NULL)

		printf("open fail");
printf("shuchu %d \n",showbyte);
	while((len = read(sfd,buffer,sizeof(buffer))) > 0)
	{

		fwrite(buffer,1,len,fd);

		sum+=len;

//printf("len %%%%%%       %d\n",len);


		pthread_mutex_lock(&work_mutex);
// printf("totalbyte:%%%%%%%% %d\n",totalbyte);
		showbyte+=len ;
		percent =100.00* (float)showbyte/totalbyte;
		snprintf(download_per_content, 64, download_per, percent);
		send_websocket_res_head(sockfd, websocket_head, download_per_content);
		printf("percent:okok%%%%%%%% %f% \n",percent);
		pthread_mutex_unlock(&work_mutex);


	//	showbyte += sum;
		
//		print_progress_bar(host_info.new_name,(float)sum,(float)http_info.file_size);

		if (http_info.file_size == sum)
		{

			printf("\n");

			break;

		}

	}

 pthread_mutex_destroy(&work_mutex);
	free(websocket_head);
	websocket_head = NULL;
	fclose(fd);

	close(sfd);

	return sum;	

}

 

//解析url

void parse_http_url(char *url,HOST_INFO *host_info)

{

	char *protocol[] = {"http://","https://",NULL};

	int i,len = 0;

 

	//删除http/https协议头

	for (i = 0;protocol[i] != NULL;i++)

		if (!strncmp(url,protocol[i],strlen(protocol[i]))){

			len = strlen(protocol[i]);

			break;

		}

 

	

	char *ptr = strchr(url + len,':');

	if (ptr != NULL){

		//提取域名

		strncpy(host_info->host_name,url + len,strlen(url + len) - strlen(ptr));

		//提取端口

		sscanf(++ptr,"%5hu",&host_info->port);

		//判断端口是否为有效端口

		if (host_info->port > 65535){

			printf("invalid port\n");

			exit(1);

		}

		//提取文件路径

		while(*ptr != '/')

			ptr++;

		strcpy(host_info->file_path,ptr);

		//提取文件名

		ptr = strrchr(host_info->file_path,'/');

		ptr++;

		strcpy(host_info->new_name,ptr);

	}

	else{

		ptr = strchr(url + len,'/');

		//提取域名

		strncpy(host_info->host_name,url + len,strlen(url + len) - strlen(ptr));

		//设置为默认端口

		host_info->port = 80;

		//提取文件路径

		strcpy(host_info->file_path,ptr);

		//提取文件名

		ptr = strrchr(host_info->file_path,'/');

		ptr++;

		strcpy(host_info->new_name,ptr);

	}

}

 

//获取下载的文件大小

unsigned long get_file_size(const char *file_name)

{

	struct stat buf;

	if (stat(file_name,&buf) == -1)

		return 0;

	else

		return buf.st_size;

}

 

//get请求

void *http_get(void *arg)

{

	struct httpdown_info *hd_info= (struct httpdown_info *) arg;
	//hd_info = (struct httpdown_info *) arg;

//	printf("****************************hd_info: %ld\n%ld\n",hd_info->startp,hd_info->endp);
	//printf("****************************hd_info: %s\n%s\n",hd_info->url,hd_info->new_name);
	//printf("****************************hd_info: %s\n%s\n%d\n%d\n",hd_info->url,hd_info->new_name,hd_info->startp,hd_info->endp);
	bzero(&host_info,sizeof(host_info));

	bzero(&http_info,sizeof(http_info));

 

	//解析url

	parse_http_url( hd_info->url,&host_info);
	if (hd_info->new_name != NULL)

		strcpy(host_info.new_name,hd_info->new_name);
//printf("\n get host_info.new_name::%s\n",host_info.new_name);
 	//sprintf(hd_info->new_name,"%s",host_info.new_name);
	//printf("get hd_info->new_name::%s\n\n",hd_info->new_name);
	//根据域名获取IP地址

	if (get_http_info(host_info.host_name,&http_info) == -1)

		print_error("gethostbyname");

	

	//连接服务器
//	printf("http_info.ip_addr is : %s\n ",http_info.ip_addr);
//	printf("host_info.port is : %d\n ",host_info.port);	

	int sfd = connect_server(http_info.ip_addr,host_info.port);

	//printf("sfd!!!: %d\n",sfd);
	if (sfd == -1)

		print_error("connect failed");

	printf("waiting for http response\n");

	//发送http报文
	
	//long long downLoaded=get_file_size(hd_info->new_name);
	
	req_info.startp=hd_info->startp;//起始位置赋值
	req_info.endp=hd_info->endp;//终止位置不变
printf("断点！\n");
printf("req_info.startp: %d\n",req_info.startp);
printf("req_info.endp: %d\n",req_info.endp);
	int	ret = send_http_header(sfd,host_info,req_info.startp,req_info.endp);	

	if (ret == -1)

		print_error("write");

 

	//解析http报头

	sfd = parse_http_header(sfd,&http_info);

	

	printf("http response:\n\tstatus code: %d\n",http_info.status_code);

	printf("\thost: %s:%hu\n",http_info.ip_addr,host_info.port);

	printf("\taliases: %s\n",http_info.aliases);

	printf("\tcontent-type:%s\n",http_info.content_type);

	if (http_info.file_size > 1024*1024)	

		printf("\tcontent-length: %.2f mb\n",(float)http_info.file_size / 1024.0 / 1024.0);

	else

		printf("\tcontent-length: %lu bytes\n",http_info.file_size);

 

	//if (http_info.status_code != 200)

		//printf("warning:not found file %s\n",host_info.new_name);

	

//	printf("are you sure to download this file?\n");

//	printf("please enter any key to continue\n");
//
//	getchar();

	

	//下载文件

//	printf("%s\n",host_info.new_name);

	unsigned long download_size = download(hd_info->sockfd,sfd,host_info,http_info);

	unsigned long file_size = get_file_size(host_info.new_name);

//	printf("dowload_size %d\n",download_size);

//	printf("file_size %lu\n",file_size);

//	if (download_size != file_size){

//		printf("download %s failure\n",host_info.new_name);

//		remove(host_info.new_name);

//	}

	//return 0;

}

 
 
 //获取网络文件大小
 long getsize(void *arg)
 {
	 struct httpdown_info *hd_info= (struct httpdown_info *) arg;
	//hd_info = (struct httpdown_info *) arg;

//	printf("****************************hd_info: %ld\n%ld\n",hd_info->startp,hd_info->endp);
//	printf("****************************hd_info: %s\n%s\n",hd_info->url,hd_info->new_name);
	//printf("****************************hd_info: %s\n%s\n%d\n%d\n",hd_info->url,hd_info->new_name,hd_info->startp,hd_info->endp);
	bzero(&host_info,sizeof(host_info));

	bzero(&http_info,sizeof(http_info));

 

	//解析url

	parse_http_url( hd_info->url,&host_info);

	

	if (hd_info->new_name != NULL)

		strcpy(host_info.new_name,hd_info->new_name);

 

	//根据域名获取IP地址

	if (get_http_info(host_info.host_name,&http_info) == -1)

		print_error("gethostbyname");

	

	//连接服务器
//	printf("http_info.ip_addr is : %s\n ",http_info.ip_addr);
//	printf("host_info.port is : %d\n ",host_info.port);	

	int sfd = connect_server(http_info.ip_addr,host_info.port);

//	printf("sfd!!!: %d\n",sfd);
	if (sfd == -1)

		print_error("connect failed");

	printf("waiting for http response\n");

	//发送http报文
	

	int	ret = send_http_headersize(sfd,host_info);	

	if (ret == -1)

		print_error("write");

 

	//解析http报头

	sfd = parse_http_header(sfd,&http_info);

	

//	printf("http response:\n\tstatus code: %d\n",http_info.status_code);

//	printf("\thost: %s:%hu\n",http_info.ip_addr,host_info.port);

//	printf("\taliases: %s\n",http_info.aliases);

//	printf("\tcontent-type:%s\n",http_info.content_type);
	 
//	if (http_info.file_size > 1024*1024)	

//		printf("\tcontent-length: %.2f mb\n",(float)http_info.file_size / 1024.0 / 1024.0);

//	else

//		printf("\tcontent-length: %lu bytes\n",http_info.file_size);
	long fsize = http_info.file_size;
	if(fsize==0)
		return -1;
	return fsize;
	close(sfd); 
 }
 
 

//如果按下了ctrl+c

/*void sig_handler(int num)

{

	if (num == SIGINT){

		printf("\ndownload %s pause\n",host_info.new_name);

		if (!access(host_info.new_name,F_OK))

			remove(host_info.new_name);

		putc('\n',stdout);

		exit(0);

	}

}

*/


//多线程下载实现函数

void process_create(struct httpdown_info *htdn_info,int thread_num)//下载任务进程
{
	pthread_mutex_t work_mutex;	
	pthread_mutex_init(&work_mutex, NULL);
// struct httpdown_info *hd_info =(struct httpdown_info *)malloc(sizeof(struct httpdown_info));


	//以上为得到文件大小
	long size =  getsize(htdn_info);


//printf("process create htdn_info!!!! %s\n%s\n%d\n%d\n",htdn_info->url,htdn_info->new_name,htdn_info->startp,htdn_info->endp);
	printf("size############ : %ld\n",size);
	totalbyte = size;	
	void *thread_result;
	int num = thread_num;//线程数
	int res,i,m,count=1;
	int dd ;//断点续传后开的线程数
	//char z_temp[32] = "";
	pthread_t thread[num];
	int eachpart = size/num;//每片文件大小
	printf("eachpart : %d\n",eachpart);
	char st[256],sk[256];
	struct MERGE merge;
//	char ch[256];
	int mark;//标记是否进行该线程
	sprintf(merge.name,"%s",htdn_info->new_name);
	sprintf(st,"%s",htdn_info->new_name);
	sprintf(sk,"%s",htdn_info->new_name);
	//printf("original st is %s\n",st);
	long downLoaded;
	//文件片下载到名字不同的文件中
	for(i= 0 ; i<num ;i++){
	
		sprintf(sk,"%s%s",sk,"_temp");
		showbyte += get_file_size(sk);
	
	}
	for(i = 0;i<num-1 ;i++)//前两片
	{
		 //htdn_info = (struct httpdown_info*)malloc(sizeof(struct httpdown_info));
		
		
		sprintf(st,"%s%s",st,"_temp");//临时文件的名字_temp

		//printf("st@@@@@tempname  : %s\n ",st);
		
		//以下为断点续传
		downLoaded = get_file_size(st);
		printf("downLoaded::!!!%d\n",downLoaded);
		if(downLoaded==0)
		{
			printf("线程%d非断点续传\n",i+1);
			mark=1;
		}else if(downLoaded==eachpart)
		{
			printf("线程%d已下载完成\n",i+1);
			mark=0;
			downLoaded=0;
		}else if(0<downLoaded<eachpart)
		{
			printf("线程%d断点续传\n",i+1);
			mark=1;
			
			
		}
		
		if(mark==1)
		{
		
		sprintf(htdn_info->new_name,"%s",st);
		//printf("#####createthread tempname  : %s\n ",htdn_info->new_name);
		
		
		htdn_info->startp = i*eachpart+downLoaded;
		htdn_info->endp = (i+1)*eachpart-1;
		printf("htdn_info->startp %d\n",htdn_info->startp);
		printf("htdn_info->endp : %d\n",htdn_info->endp);
		printf("=======创建线程%d===========\n",i+1);
		res = pthread_create(&thread[i], NULL, http_get,(void *) htdn_info); //创建线程
		count++;
		//printf("res (createthread): %d\n",res);
			if(res!=0)
			{
				perror("线程创建失败");
				exit(1);
			}
		}else if(mark==0)
		{
			thread[i]=-1;
		}
		sleep(1);//!!!!!!!
		
		//printf("线程创建成功==============================\n");
		
	}
	for(i = num-1;i<num ;i++)//最后一片
	{
		/*printf("=======创建线程3===========\n");
		htdn_info->startp = i*eachpart;
		htdn_info->endp = size-1;
		
		sprintf(st,"%s%s",st,"_temp");
		sprintf(htdn_info->new_name,"%s",st);		
		res = pthread_create(&thread[i], NULL, http_get,(void *) htdn_info); //创建线程
		count++;*/
		
		sprintf(st,"%s%s",st,"_temp");//临时文件的名字_temp
		//以下为断点续传
		downLoaded = get_file_size(st);
		printf("downLoaded::!!!%d\n",downLoaded);
		if(downLoaded==0)
		{
			printf("线程%d非断点续传\n",i+1);
			mark=1;
		}else if(downLoaded==size-i*eachpart)
		{
			printf("线程%d已下载完成\n",i+1);
			mark=0;
			downLoaded=0;
		}else if(0<downLoaded<size-i*eachpart)
		{
			printf("线程%d断点续传\n",i+1);
			mark=1;
			
		}
		
		if(mark==1)
		{
		
		sprintf(htdn_info->new_name,"%s",st);
		//printf("tempname  : %s\n ",htdn_info->new_name);
		
		
		htdn_info->startp = i*eachpart+downLoaded;
		htdn_info->endp = size-1;
		printf("htdn_info->startp %d\n",htdn_info->startp);
		printf("htdn_info->endp : %d\n",htdn_info->endp);
		printf("=======创建线程%d===========\n",i+1);
		res = pthread_create(&thread[i], NULL, http_get,(void *) htdn_info); //创建线程
		count++;
		
		
		
		if(res!=0)
		{
			perror("线程创建失败");
			exit(1);
		}
		sleep(1);//!!!!!!!
		
		//printf("线程创建成功==============================\n");
		
		}else if(mark==0)
		{
			thread[i]=-1;
		}
	
	}
	
	printf("count ::%d\n\n",count);
	
	for(i = num-1;i>=0 ;i--)
	{
		if(thread[i]!=-1)
		{
		res = pthread_join(thread[i],NULL);
		
		//printf("res(join) : %d\n",res);
			if(res==0)
			{
			//	printf("结束一个线程=======\n");
				
			}else{
				perror("线程结束失败");
			}
			
		}
			//printf("线程%d完成\n",i+1);
		//all += pres[i];
		
		
		
	}
	printf("全部下载结束！\n");
//	exit(EXIT_SUCCESS);
	
//printf("num %d\n",num);
//printf("merge name  %s\n",merge.name);
	int merge_flag = http_file_merge(num,merge.name);
//printf("kkkkkkkkkkkkkkkk %d\n");
	if(merge_flag == 1)
	{
		printf("文件合并成功\n");
	}
	



}




//文件合并函数
int http_file_merge(int num,char file_name[])
{
//printf("1111111111111111111111111111111111111\n");
	//char location_temp[32] = "";
	//strcpy(location_temp, file_name);
	//strcat(location_temp, "_temp");
	//printf("222222222222   %s\n",location_temp);
	int i = 0;
	FILE *target,*current;
	int length = 0; //current file length
	char *s; //current file content
	char file_temp[1024] = "0";
//	chdir("..");
	//strcpy(file_temp,file_name);
	//memcpy(file_temp,file_name,strlen(file_name));
	
	
//	printf("file_name1::::%s\n",file_name);
	target = fopen(file_name, "wb");
//	printf("file_name2::::%s\n",file_name);
	sprintf(file_temp,"%s",file_name);
//	printf("file_temp::::%s\n",file_temp);
	
//	mkdir(location_temp,0777);
//	chdir(location_temp);


	for(i = 0; i < num ; i++)
	{
//		printf("file_temp::::%s\n",file_temp);
//printf("3333333  file_temp::   %s\n",file_temp);

		sprintf(file_temp,"%s%s",file_temp,"_temp");
//		printf("3333333  file_temp::   %s\n",file_temp);

		current = fopen(file_temp, "rb");
		fseek(current, 0, SEEK_END);
		length = ftell(current);
		if(!length)
		{
			return 0;
		}
		fseek(current, 0, SEEK_SET);
		s = (char *)malloc(length);
		fread(s,1,length,current);
		int res = fwrite(s,1,length,target);
		fflush(target);
		fclose(current);
		remove(file_temp);
	}

	fclose(target);

	return 1;
	
}



//http下载的主函数
int http(int sockfd, char  *url, int thread_num)
{
	printf("\n\n@@@@@@~~~~~~~~~传递url%s\n\n",url);
	int download_flag = 0;
	struct httpdown_info *hd_info =(struct httpdown_info *)malloc(sizeof(struct httpdown_info));
	hd_info->sockfd = sockfd;	
	char md5_file[READ_DATA_SIZE]= {0};
	struct websocket_res_head * websocket_head_main = (struct websocket_res_head *)malloc(sizeof(struct websocket_res_head *));
	sprintf(hd_info->url,"%s",url);

	bzero(&host_info,sizeof(host_info));
	
	parse_http_url(hd_info->url,&host_info);

	sprintf(hd_info->new_name,"%s",host_info.new_name);
//	printf("\n\n main hd_info->new_name::%s\n\n",hd_info->new_name);
	sprintf(md5_file,"%s%s",hd_info->new_name,".md5");	
	char filenamea[1024]="0";
	sprintf(filenamea,"%s",hd_info->new_name);
	
	process_create(hd_info, thread_num);

	//char filenamea[1024]="0";
	//sprintf(filenamea,"%s",hd_info->new_name);
		
	//char md5_file[READ_DATA_SIZE]= {0};
	//sprintf(md5_file,"%s%s",filenamea,".md5");	
	char *res = "";
	res = md5_cpy(filenamea,md5_file);
	//if(res == 1)
	//	printf("md5 is same\n");	
	snprintf(file_info_php, 128, file_info_to_php, res, filenamea, totalbyte);
	printf("file_info:%s\n",file_info_php);
	send_websocket_res_head(sockfd, websocket_head_main, file_info_php);	
	free(websocket_head_main);
	websocket_head_main = NULL;
	free(hd_info);
	hd_info = NULL;
	download_flag = 1;
	return download_flag;

}
