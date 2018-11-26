#include "TCPNet.h"

TCPNet::TCPNet(TCPKernel* tcpKernel)
{
//	printf("TCPNet init\n");
	m_pTCPKernel = tcpKernel;	
    is_stop=false;
	port=8000;
	pool=NULL;
	threadnum=20;
}

bool TCPNet::InitNetWork()   //EpollServer的初始化
{
	bzero(&bindAddr, sizeof(bindAddr));
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_port = htons(port);
	bindAddr.sin_addr.s_addr = inet_addr("10.50.66.240");  //创建Socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		printf("EpollServer socket init error\n");
		return false;
	}
	
	//绑定
	int ret = bind(sockfd, (struct sockaddr *)&bindAddr, sizeof(bindAddr));
	if(ret < 0)
	{
		printf("EpollServer bind init error\n");
		return false;
	}
	
	//监听
	ret = listen(sockfd, 10);
	if(ret < 0)
	{
		printf("EpollServer listen init error\n");
		return false;
	}
	
    //创建Epoll
	epollfd = epoll_create(1024);
	if(epollfd < 0)
	{
		printf("EpollServer epoll_create init error\n");
		return false;
	}
	pool = new threadpool<BaseTask>(threadnum);  //创建线程池

	//将sockfd加入epoll
	return true;
}

void Task::doit()
{
		//do something of the order
		//printf("%s\n", order);
//		snprintf(order, MAX_BUFFER - 1, "somedata\n");
	        //write(sockfd, order, strlen(order));
        m_TCPKernel->DealData(sockfd,order);
}

void TCPNet::DealData()
{
	pool->start();
	//将sockfd加入epoll
	addfd(epollfd, sockfd, false);

	//epoll开始监听IO请求
	while(!is_stop)
	{
		int ret = epoll_wait(epollfd, events, MAX_EVENT, -1);
		if(ret < 0)  //出错处理
		{
			printf("epoll_wait error\n");
			break;
		}
		for(int i = 0; i < ret; ++i)
		{
			int fd = events[i].data.fd;
			if(fd == sockfd)  //新的连接到来
			{
				struct sockaddr_in clientAddr;
				socklen_t len = sizeof(clientAddr);
				//accept连接请求
				int confd = accept(sockfd, (struct sockaddr *)
					&clientAddr, &len);
 
				//将新的confd加入epoll中
				TCPNet::addfd(epollfd, confd, false);
			}
			else if(events[i].events & EPOLLIN)  //某个fd上有数据可读
			{
				char buffer[MAX_BUFFER];
		readagain:	memset(buffer, 0, sizeof(buffer));
				int ret = recv(fd, buffer, MAX_BUFFER - 1,0);
				//某个fd关闭了连接，从Epoll中删除并关闭fd
				if(ret == 0)
				{
					struct epoll_event ev;
					ev.events = EPOLLIN;
					ev.data.fd = fd;
					epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
					shutdown(fd, SHUT_RDWR);
					printf("%d logout\n", fd);
					continue;
				}
				//读取出错，尝试再次读取
				else if(ret < 0)
				{
					if(errno == EAGAIN)	
					{
						printf("read error! read again\n");
						goto readagain;
					    	break;
					}
				}
				//成功读取，将读取信息发送给中介者
				else
				{ //printf("aaa\n");
					Task *task = new Task(buffer,fd,m_pTCPKernel);
					pool->append_task(task);
			//		m_pTCPKernel->DealData(fd,buffer);
				}
			}
			else
			{
				printf("something else had happened\n");
			}
		}
	}
	pool->stop();
}

bool TCPNet::SendData(int sockfd, char* szbuf, int nlen)
{
	if(!szbuf || nlen <= 0)
		return false;
	
	//发送包的大小
	if(write(sockfd,(const char*)&nlen,sizeof(int)) < 0)
		return false;
	
	//发送包的内容
	if(write(sockfd, szbuf, nlen) < 0)
		return false;

	return true;
}

void TCPNet::UnInitNetWork()
{
	close(sockfd);
	close(epollfd);
}
