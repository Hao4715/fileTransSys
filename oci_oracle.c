#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <oci.h>


struct OCIHP
{
	OCIEnv* phOCIEnv ;
	OCIError* phOCIErr;
	OCISvcCtx* phOCISvctx;
	OCIServer* phOCIServe;	
	OCIStmt* phOCIstmt; 
	OCISession * phSession; 
	char* db_name;
	char* user_name;
	char* pwd;
};

/*void oci_init(struct OCIHP *hp)
{
	printf("%s\n",hp->user_name);
	hp->user_name =  "hello";
}
*/

int oci_connect(struct OCIHP *hp)
{
	printf("dbname:%s\n",hp->db_name);
	printf("username:%s\n",hp->user_name);
	printf("pwd:%s\n",hp->pwd);
	char sErrorMsg[1024];
	sb4 sb4ErrorCode;
	ub4 ub4RecordNo = 1;
	int nRet = 0;
	nRet = OCIInitialize((ub4)OCI_DEFAULT,(dvoid*)0,(dvoid*(*)(dvoid*,size_t))0,(dvoid*(*)(dvoid*, dvoid*, size_t))0,(void(*)(dvoid*, dvoid*))0);
	if(nRet)
	{
		printf("init error\n");
		return -1;
	}

	nRet = OCIEnvInit(&(hp->phOCIEnv), (ub4)OCI_DEFAULT, (size_t)0, (dvoid**)0);
	if(nRet)
	{
		printf("envinit error\n");
		return -1;
	}

	nRet = OCIHandleAlloc((dvoid *)(hp->phOCIEnv), (dvoid**)&(hp->phOCIErr), (ub4)(OCI_HTYPE_ERROR), (size_t)0, (dvoid**)0);
	if(nRet)
	{
		printf("handle 1 error\n");
		return -1;
	}	
	
	nRet = OCIHandleAlloc((dvoid *)(hp->phOCIEnv), (dvoid**)&(hp->phSession), (ub4)(OCI_HTYPE_SESSION), (size_t)0, (dvoid**)0);
	if(nRet)
	{
		printf("handle 2 error\n");
		return -1;
	}	
	
	nRet = OCIHandleAlloc((dvoid *)(hp->phOCIEnv), (dvoid**)&(hp->phOCIServe), (ub4)(OCI_HTYPE_SERVER), (size_t)0, (dvoid**)0);
	if(nRet)
	{
		printf("handle 3 error\n");
		return -1;
	}	

	nRet= OCIServerAttach(hp->phOCIServe,hp->phOCIErr,(text *)hp->db_name, 
	strlen(hp->db_name),(ub4) OCI_DEFAULT);
	if(nRet)
	{
		printf("OCIServerAttach error\n");
		return -1;
	} 

	nRet = OCIHandleAlloc((dvoid *)(hp->phOCIEnv),(dvoid **)&(hp->phOCISvctx),(ub4)OCI_HTYPE_SVCCTX,(size_t)0,(dvoid **)0);
	if(nRet)
	{
		printf("OCIHandleAlloc() errorn");
		return -1;
	}

	nRet = OCIAttrSet((dvoid *)(hp->phOCISvctx), (ub4)OCI_HTYPE_SVCCTX, (dvoid *)hp->phOCIServe, (ub4)0, OCI_ATTR_SERVER, hp->phOCIErr);
	if(nRet)
	{
		printf("ociattrset error\n");
		return 0;
	} 

	nRet =	OCIHandleAlloc( (dvoid *)(hp->phOCIEnv),(dvoid **)&(hp->phOCIstmt),(ub4)OCI_HTYPE_STMT,(size_t)0,(dvoid **)0);
	if(nRet)
	{
		printf("OCIHandleAlloc() error\n");
		return -1;
	}

	nRet= OCIAttrSet((dvoid *)hp->phSession, (ub4)OCI_HTYPE_SESSION, (dvoid *)hp->user_name,(ub4)strlen(hp->user_name), OCI_ATTR_USERNAME, hp->phOCIErr);
	if(nRet)
	{
		printf("error\n");
		return -1;
	} 

	nRet = OCIAttrSet((dvoid *)hp->phSession, (ub4)OCI_HTYPE_SESSION, (dvoid *)hp->pwd,(ub4)strlen(hp->pwd), OCI_ATTR_PASSWORD, hp->phOCIErr);
	if(nRet)
	{
		printf("error\n");
		return -1;
	} 

	nRet = OCISessionBegin (hp->phOCISvctx, hp->phOCIErr, hp->phSession, OCI_CRED_RDBMS,OCI_DEFAULT);
	if(nRet)
	{
		printf("zhi xing OCISessionBegin hanhsu\n");
		if (OCIErrorGet(hp->phOCIErr, ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg,sizeof(sErrorMsg), OCI_HTYPE_ERROR) == OCI_SUCCESS) 
		{ 	 
	 		printf("%s\n",sErrorMsg);
        	}
		return -1;
	} 

	nRet = 	OCIAttrSet(hp->phOCISvctx, OCI_HTYPE_SVCCTX, hp->phSession, sizeof(OCISession*), OCI_ATTR_SESSION, hp->phOCIErr);
    if(nRet)
	{
		printf("OCIAttrSet() error:%d\n",nRet);
		if (OCIErrorGet(hp->phOCIErr, ub4RecordNo++, NULL, &sb4ErrorCode, (OraText*) sErrorMsg,sizeof(sErrorMsg), OCI_HTYPE_ERROR) == OCI_SUCCESS) 
	 	{
	 		printf("%s\n",sErrorMsg);
        	}
			return -1;
	} 
	printf("%s\n","CONNECT OK");
	return nRet;

}

int oci_select(struct OCIHP *hp)
{
	int nRet = 0;
	ub4 ub4RecordNo = 1;
 	OCIDefine * bhp[10];  
 	char name[33];
 	char password[30];
	char sErrorMsg[1024];
	sb4 sb4ErrorCode;
	
	b2 sb2aIndid[30];
	char sql[]="select * from TEST";
	nRet=OCIStmtPrepare(hp->phOCIstmt, hp->phOCIErr, (text*)sql, (ub4) strlen(sql), (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT);
	if(nRet)
		return -1;
	ub2 datalen = 0;
	
	if (OCIDefineByPos(hp->phOCIstmt, &bhp[0], hp->phOCIErr, 1, (dvoid *)&name, (ub4)sizeof(name),SQLT_STR/*LBI long binary type */, &sb2aIndid[0], &datalen, NULL, OCI_DEFAULT) !=0)
	{
        	OCIHandleFree(hp->phOCIstmt, OCI_HTYPE_STMT);
		return -1;
    	}
	if (OCIDefineByPos(hp->phOCIstmt, &bhp[1], hp->phOCIErr, 2, (dvoid *)&password, (ub4)sizeof(password),SQLT_STR/*LBI long binary type */, &sb2aIndid[1], &datalen, NULL, OCI_DEFAULT) !=0)
    	{
		OCIHandleFree(hp->phOCIstmt, OCI_HTYPE_STMT);
		return -1;

    	}
	
	ub2 stmt_type;
	OCIAttrGet ((dvoid *)hp->phOCIstmt, (ub4)OCI_HTYPE_STMT, (dvoid *)&stmt_type, (ub4 *)0, (ub4)OCI_ATTR_STMT_TYPE, hp->phOCIErr);
   	printf("%s\n","start sql exec\n");
    	nRet = OCIStmtExecute( hp->phOCISvctx, hp->phOCIstmt, hp->phOCIErr, (ub4)(stmt_type==OCI_STMT_SELECT?1:0), (ub4)0, (OCISnapshot *) NULL,(OCISnapshot *) NULL, (ub4) OCI_DEFAULT);
    	if(nRet)
		return -1;
	
	int rows_fetched;
	do
	{
		printf("name=%s,psw =%s\n", (sb2aIndid[0] == -1 ? "NULL" : name), password);
	}
	while(OCIStmtFetch2(hp->phOCIstmt, hp->phOCIErr, 1, OCI_FETCH_NEXT, OCI_FETCH_NEXT, OCI_DEFAULT) != OCI_NO_DATA);

	OCIAttrGet((CONST void *)hp->phOCIstmt, OCI_HTYPE_STMT, (void *)&rows_fetched, (ub4 *)sizeof(rows_fetched),OCI_ATTR_ROW_COUNT, hp->phOCIErr);
	printf("all lines: %d\n",rows_fetched);
	return 0;









}



int main()
{
	int res;
	struct OCIHP* hp = (struct OCIHP*)malloc(sizeof(struct OCIHP));
	hp->phOCIEnv	= NULL;
	hp->phOCIErr	= NULL;
	hp->phOCISvctx  = NULL;
	hp->phOCIServe = NULL;
	hp->phOCIstmt = NULL;
	hp->phSession = NULL;
	hp->db_name = "orcl";
	hp->user_name = "unomoist";
	hp->pwd = "unoMOIST";
//	oci_init(hp);
//	printf("%s\n",hp->user_name);
	res = oci_connect(hp);
	oci_select(hp);
	return 0;
	
}
	




	
