#pragma once
#include"head.h"
#include "thread_pool.h"
#include "TCPKernel.h"
#define MAX_EVENT 1024   //epoll_events的最大个数
#define MAX_BUFFER 2048  //Buffer的最大字节
class TCPKernel;
class TCPNet;
class BaseTask
{
public:
	virtual void doit() = 0;
};
 
class Task:public BaseTask 
{
private:
	int sockfd;
	char order[MAX_BUFFER];
	TCPKernel* m_TCPKernel;
public:
	Task(char *str, int fd,TCPKernel *tcpKernel) : sockfd(fd)
	{
		m_TCPKernel = tcpKernel;
		memset(order, '\0', MAX_BUFFER);
		strcpy(order, str);
	}
/*	void doit()  //任务的执行函数
	{
		//do something of the order
		//printf("%s\n", order);
		snprintf(order, MAX_BUFFER - 1, "somedata\n");
	        //write(sockfd, order, strlen(order));
                 m_TCPNet->m_pTCPKernel->DealData(sockfd,order);
                
                
                  
	}*/
	void doit();
};


class TCPNet
{
private:
	bool is_stop;   //是否停止epoll_wait的标志
	int threadnum;   //线程数目
	int sockfd;     //监听的fd
	int port;      //端口
	int epollfd;    //Epoll的fd
public:
    TCPKernel* m_pTCPKernel;
	threadpool<BaseTask> *pool;   //线程池的指针
	char address[20];
	epoll_event events[MAX_EVENT];  //epoll的events数组
	struct sockaddr_in bindAddr;   //绑定的sockaddr
 
public:
	//构造函数
	TCPNet(TCPKernel*tcpKernel);
//	TCPNet();

	
	//析构
	virtual ~TCPNet(){}	

 	//创建Socket 绑定 监听 创建Epoll
	bool InitNetWork();

	void UnInitNetWork();
 	
	void DealData();

	bool SendData(int sockfd, char* szbuf, int nlen);	
 
	static int setnonblocking(int fd)  //将fd设置称非阻塞
	{
		int old_option = fcntl(fd, F_GETFL);
		int new_option = old_option | O_NONBLOCK;
		fcntl(fd, F_SETFL, new_option);
		return old_option;
	}
 
	static void addfd(int epollfd, int sockfd, bool oneshot)  //向Epoll中添加fd
	{//oneshot表示是否设置称同一时刻，只能有一个线程访问fd，数据的读取都在主线程中，所以调用都设置成false
		epoll_event event;
		event.data.fd = sockfd;
		event.events = EPOLLIN | EPOLLET;
		if(oneshot)
		{
			event.events |= EPOLLONESHOT;
		}
		epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &event); //添加fd
		TCPNet::setnonblocking(sockfd);
	}
};
