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
	 m_pTcpNet = NULL;
	}
}

BEGIN_PROTOCOL_MAP
	PM(_DEF_PROTOCOL_REGISTER_RQ,&TCPKernel::RegisterRq)
	PM(_DEF_PROTOCOL_LOGIN_RQ,&TCPKernel::LoginRq)
END_PROTOCOL_MAP

void TCPKernel::LoginRq(int sockfd,char* szbuf)
{
	printf("login\n");
	//1 2  3
	//在数据库中查找
	//检验密码
	STRU_LOGIN_RQ *pslr = (STRU_LOGIN_RQ*)szbuf;
	char szsql[_DEF_SQLLEN] = {0};
	list<string> lststr;
	STRU_LOGIN_RS slr;
	slr.m_nType = _DEF_PROTOCOL_LOGIN_RS;
	memcpy(slr.m_szAccount,pslr->m_szAccount,_DEF_SIZE);
	slr.m_szResult = _login_accountnoexists;
		
	sprintf(szsql,"SELECT password FROM client where account = '%s'" ,pslr->m_szAccount);
	m_sql.SelectMySql(szsql,1,lststr);
	if(lststr.size() >0)
	{
		slr.m_szResult = _login_passworderr;

		string strpassword = lststr.front();
		lststr.pop_front();

		if(0 == strcmp(strpassword.c_str(),pslr->m_szPassword))
		{	
			slr.m_szResult = _login_success;
			printf("登录成功\n");
		}
		else
			printf("密码错误\n");
	}
	else
		printf("账户不存在\n");
	
	m_pTcpNet->SendData(sockfd,(char*)&slr,sizeof(slr));
}
	/*else if(pslr->m_role == role_audience)
	{
		sprintf_s(szsql,"SELECT a_password FROM audience where  a_name = '%s'" ,pslr->m_szName);
		m_sql.SelectMySql(szsql,1,lststr);
		if(lststr.size() >0)
		{
			slr.m_szResult = _login_passworderr;

			string strpassword = lststr.front();
			lststr.pop_front();

			if(0 == strcmp(strpassword.c_str(),pslr->m_szPassword))
			{
				slr.m_szResult = _login_success;
				//记录观众信息(sock，事件，偏移量)
				Audience_Info *pInfo = new Audience_Info;
				pInfo->m_hEvent = WSACreateEvent();
				pInfo->m_noffset = 0;
				pInfo->m_sock = sock;
				m_lstAudience.push_back(pInfo);
				m_mapSocketToAudienceInfo[sock] = pInfo;
			}



		}
	}*/

//注册请求
void TCPKernel::RegisterRq(int sockfd,char* szbuf)
{
	printf("register\n");
	char result[1024];
	STRU_REGISTER_RQ *psrr = (STRU_REGISTER_RQ *)szbuf;
	char szsql[_DEF_SQLLEN] = {0};
	STRU_REGISTER_RS srr;
	srr.m_nType = _DEF_PROTOCOL_REGISTER_RS;
	memcpy(srr.m_szAccount,psrr->m_szAccount,_DEF_SIZE);
	list<string> lststr;
	
	
		//检查数据库中是否存在此人
		sprintf(szsql,"SELECT account FROM client where account =  '%s'" ,psrr->m_szAccount);
		m_sql.SelectMySql(szsql,1,lststr);

		if(lststr.size() ==0)
		{
			srr.m_szResult = _register_success;
			 //将新用户插入到数据库中
			sprintf(szsql,"insert into client (account,password) values('%s','%s')",psrr->m_szAccount,psrr->m_szPassword);
			m_sql.UpdateMySql(szsql);
			sprintf(result,"成功注册");
		}
		else 
		{
			string strId = lststr.front();
			lststr.pop_front();
			srr.m_szResult = _register_accountexists;
			sprintf(result,"注册失败");
		}
		
	//回复
	m_pTcpNet->SendData(sockfd,(char*)&srr,sizeof(srr));
	printf("%s\n",result);
}


bool TCPKernel::Open()
{
	if(!m_sql.ConnectMySql("localhost","root","1511780760wyw","red_book"))
		return false;

	//初始化socket和epoll
	if(!m_pTcpNet->InitNetWork())
		return false;

	//epoll监听
	m_pTcpNet->DealData();
	
//	if(!m_threadpool.CreateThreadPool(1,1000))
//		return false;

	return true;


}

void TCPKernel::Close()
{
	m_pTcpNet->UnInitNetWork();
//	m_threadpool.DestroyThreadPool();

	//释放链表
/*	auto ite = m_lstAuthor.begin();
	while(ite != m_lstAuthor.end())
	{
		CloseHandle((*ite)->m_hSemaphore);
		closesocket((*ite)->m_sock);
		delete *ite;
		*ite = NULL;
		ite++;
	}

	m_lstAuthor.clear();
	auto iteAudience = m_lstAudience.begin();
	while(iteAudience != m_lstAudience.end())
	{
		CloseHandle((*iteAudience)->m_hEvent);
		closesocket((*iteAudience)->m_sock);
		delete *iteAudience;
		*iteAudience = NULL;
		iteAudience++;
	}

	m_lstAudience.clear();
*/
}

void TCPKernel::DealData(int sockfd,char* szbuf)
{
	//printf("%d data:%s",sockfd,szbuf);
	//处理数据---协议映射表
	printf("处理来自%d的数据\n",sockfd);
	PackType *pType = (PackType *)szbuf;
	printf("%d\n",*pType);
	int i = 0;
	while(1)
	{
		printf("%d\n",i);
		if(m_ProtocolMapEntries[i].m_nType ==  *pType)
		{
			(this->*m_ProtocolMapEntries[i].m_pfun)(sockfd,szbuf);
			break;
		}
		else if(m_ProtocolMapEntries[i].m_nType ==0 || m_ProtocolMapEntries[i].m_pfun ==0)
			break;

		i++;
	}
}
