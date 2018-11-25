#pragma once

//#ifndef _TCPKERNEL_H

//#define _TCPKERNEL_H

#include "CMySql.h"

#include "TCPNet.h"

#include "thread_pool.h"

#include "Packdef.h"

#include "Online.h"

class TCPKernel;

class TCPNet;

class CMySql;

//class threadpool;

typedef void (TCPKernel:: *PFUN)(int ,char*);

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

	 bool Open() ;

	 void Close() ;

	 void DealData(int sockfd,char* szbuf) ;

public:

	void RegisterRq(int sockfd,char* szbuf);

	void LoginRq(int sockfd,char* szbuf);

public:

	TCPNet *m_pTcpNet;

private:

	CMySql m_sql;

	list<OlAccount*> m_lstOlAccount;

//	threadpool m_threadpool;





};



/*class RedItask

{

public:

	    RedItask(int sockfd,TCPKernel*pKernel)

		{

			m_sockfd = sockfd;

	

			m_pKernel = pKernel;

		}

		~RedItask(){}

		void RunItask();

private:

										

		int  m_sockfd;											

	    TCPKernel* m_pKernel;



};*/

//#endif
