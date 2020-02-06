#include <stdio.h>
#include <string.h>
#include <openssl/md5.h>

int main(int argc, char *argv[])
{
	//printf("%d %s %s",argc,argv[0],argv[1]);
	MD5_CTX ctx;
	unsigned char outmd[16];
	char buffer[1024];
	char *filename;
	strcpy(filename,argv[1]);
	int len = 0;
	int i;
	FILE *fp;

	memset(outmd, 0, sizeof(outmd));
	memset(buffer, 0, sizeof(buffer));

//	printf("please input filename:");
//	scanf("%s",filename);
//	fp = fopen("test.txt","rb");
	fp = fopen(filename,"rb");
	if(fp == NULL)
	{
		printf("fp error\n");
	}
	MD5_Init(&ctx);

	while((len = fread(buffer, 1, 1024, fp)) > 0)
	{
		MD5_Update(&ctx, buffer, len);
		memset(buffer, 0, sizeof(buffer));
	}
	MD5_Final(outmd, &ctx);
	
	for(i = 0; i < 16 ;i++)
	{
		printf("%02X",outmd[i]);
	}
	printf("\n");
	fclose(fp);
	return 0;
}










