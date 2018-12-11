#pragma once
#include"Mysql.h"
#include"TCPNet.h"
#include"threadpool.h"
#include"Packdef.h"
class TCPKernel;
class TCPNet;
class CMySql;
typedef void (TCPKernel::*PFUN)(int ,char*);
struct ProtocolMap
{
	PackType m_nType;
	PFUN     m_pfun;
};

class TCPKernel
{
public:
	TCPKernel();
	virtual ~TCPKernel();
public:
	bool Open();
	void Close();
	void DealData(int sockfd,char* szbuf) ;
public:
	void RegisterRq(int sockfd,char* szbuf);
	void LoginRq(int sockfd,char* szbuf);
	void PublishRq(int sockfd,char* szbuf);
private:
	TCPNet* m_pTcpNet;
	CMySql m_sql;
};
