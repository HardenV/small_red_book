#include "TCPKernel.h"

TCPKernel::TCPKernel()
{
	m_pTcpNet = new TCPNet(this);
}
TCPKernel::~TCPKernel()
{
	if(m_pTcpNet)
	{
		delete m_pTcpNet;
		m_pTcpNet =NULL;
	}
}

BEGIN_PROTOCOL_MAP
	PM(_DEF_PROTOCOL_REGISTER_RQ,&TCPKernel::RegisterRq)
	PM(_DEF_PROTOCOL_LOGIN_RQ,&TCPKernel::LoginRq)
	PM(_DEF_PROTOCOL_PUBLISH_RQ,&TCPKernel::PublishRq)
END_PROTOCOL_MAP

void TCPKernel::PublishRq(int sockfd,char*szbuf)
{

   STRU_PUBLISH_RQ *pstr=(STRU_PUBLISH_RQ*)szbuf;

	//memcpy(pspr->m_szSTREAM,,_DEF_STREAMSIZE);
	int fd;
    while(1)
	{
		fd = open("cheer.jpg",O_RDWR|O_CREAT|O_APPEND,0777);
		if(fd > 0)
			break;
		else
			printf("打不开\n");
	}
    printf("%d\n",pstr->length);
	printf("%d\n",fd);
	write(fd,pstr->m_szSTREAM,pstr->length); 
	close(fd);
}

void TCPKernel::LoginRq(int sockfd,char*szbuf)
{
	STRU_LOGIN_RQ*pslr=(STRU_LOGIN_RQ*)szbuf;
	char szsql[_DEF_SQLLEN]={0};
	list<string>lststr;
	STRU_LOGIN_RS slr;
	slr.m_Type =_DEF_PROTOCOL_LOGIN_RS;
	memcpy(slr.m_szAccount,pslr->m_szAccount,_DEF_SIZE);
	slr.m_szResult=_login_accountnoexists;
	sprintf(szsql,"SELECT password FROM client where account='%s'",pslr->m_szAccount);
	m_sql.SelectMySql(szsql,1,lststr);
	if(lststr.size()>0)
	{
		slr.m_szResult=_login_passworderr;
		string strpassword =lststr.front();
		lststr.pop_front();
		if(0==strcmp(strpassword.c_str(),pslr->m_szPassword))
		{
			slr.m_szResult=_login_success;
            printf("登录成功\n");
		}
		else
			printf("密码错误\n");
	

	}
	else
		printf("账户不存在\n");
		m_pTcpNet->SendData(sockfd,(char*)&slr,sizeof(slr));

}
void TCPKernel::RegisterRq(int sockfd,char*szbuf)
{
	//printf("666\n");
	char result[1024];
	STRU_REGISTER_RQ *psrr=(STRU_REGISTER_RQ*)szbuf;
	char szsql[_DEF_SQLLEN]={0};
	STRU_REGISTER_RS srr;
	srr.m_Type=_DEF_PROTOCOL_REGISTER_RS;
	memcpy(srr.m_szAccount,psrr->m_szAccount,_DEF_SIZE);
	list<string>lststr;
	//检查数据库中是否有此人
	sprintf(szsql,"SELECT account FROM client  where account='%s'",psrr->m_szAccount);
	m_sql.SelectMySql(szsql,1,lststr);
	if(lststr.size()==0)
	{
		srr.m_szResult = _register_success;
		//将新用户插入到数据库中
		sprintf(szsql,"insert into client (account,password) value('%s','%s')",psrr->m_szAccount,psrr->m_szPassword);
		m_sql.UpdateMySql(szsql);
		sprintf(result,"注册成功");
	
	}
	else
	{
		string strId =lststr.front();
		lststr.pop_front();
		srr.m_szResult=_register_accountexists;
		sprintf(result,"注册失败");

	}
	m_pTcpNet->SendData(sockfd,(char*)&srr,sizeof(srr));
	printf("%s\n",result);

}
bool TCPKernel::Open()
{
	if(!m_sql.ConnectMySql("localhost","root","1511780760wyw","red_book"))
		return false;
		//初始化socket和epoll
		if(!m_pTcpNet->InitNetWork())
		{
			return false;
		}
		m_pTcpNet->DealData();
		return true;
}
void TCPKernel::Close()
{
	m_pTcpNet->UnInitNetWork();

}
void TCPKernel::DealData(int sockfd,char*szbuf)
{
	//printf("666\n");
	PackType *pType=(PackType*)szbuf;
	printf("%c\n",*pType);
	int i =0;
	while(1)
	{
		if(m_ProtocolMapEntries[i].m_nType==*pType)
		{
			//printf("%c\n",m_ProtocolMapEntries[i].m_nType);
			(this->*m_ProtocolMapEntries[i].m_pfun)(sockfd,szbuf);
			break;

		}
		else if(m_ProtocolMapEntries[i].m_nType==0 || m_ProtocolMapEntries[i].m_pfun ==0)
			break;
			i++;
	}

}
