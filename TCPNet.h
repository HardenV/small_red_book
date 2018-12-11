#pragma once
#include"head.h"
#include"threadpool.h"
#include"TCPKernel.h"

#define MAX_EVENT 1024 //epoll_events 的最大个数

#define MAX_BUFFER 2048 //buffer 最大字节
class TCPKernel;
class TCPNet;

class BaseTask
{
public:
	virtual void doit()=0;

};
class Task:public BaseTask
{
private:
	int sockfd;
    int repollfd;
	TCPKernel* m_TCPKernel;
public:
	Task(int epollfd,int fd,TCPKernel*tcpKernel):sockfd(fd)
	{ 
		repollfd=epollfd;
		m_TCPKernel =tcpKernel;
	}
	void doit();
        void reset_oneshot(int repollfd,int fd){//重置事件
	    epoll_event event;
	    event.data.fd=fd;
	    event.events=EPOLLIN|EPOLLET|EPOLLONESHOT;
	    epoll_ctl(repollfd,EPOLL_CTL_MOD,fd,&event);
	}
        
};

class TCPNet
{
private:
	bool is_stop;
	int threadnum;
	int sockfd;
    int epollfd;
	int port;

	
public:
	TCPKernel*m_pTCPKernel;
	threadpool<BaseTask>*pool;
	char address[20];
	epoll_event events[MAX_EVENT]; 
	struct sockaddr_in bindAddr;
public:
	TCPNet(TCPKernel*tcpKernel);
	virtual ~TCPNet(){}
	bool InitNetWork();
	void UnInitNetWork();
	void DealData();
	bool SendData(int sockfd,char *buf,int nlen);
	static int setnonblocking(int fd)  //将fd设置为非阻塞
	{
		int old_option =fcntl(fd,F_GETFL);
		int new_option=old_option|O_NONBLOCK;
		fcntl(fd, F_SETFL, new_option);
		return old_option;
	}
	static void addfd(int epollfd,int sockfd,bool oneshot)
	{
		epoll_event event;
		event.data.fd=sockfd;
		event.events=EPOLLIN|EPOLLET;//epoll采用的边缘触发
		if(oneshot)
		{
			event.events|=EPOLLONESHOT;
		}
		epoll_ctl(epollfd,EPOLL_CTL_ADD,sockfd,&event);
		//TCPNet::setnonblocking(sockfd);
	}
};
