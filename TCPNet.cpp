#include"TCPNet.h"

TCPNet::TCPNet(TCPKernel* tcpKernel)
{
	m_pTCPKernel=tcpKernel;
	is_stop=false;
	port=8000;
	pool=NULL;
	threadnum=2;
}
bool TCPNet::InitNetWork()
{
	bzero(&bindAddr,sizeof(bindAddr));
	bindAddr.sin_family=AF_INET;
	bindAddr.sin_port=htons(port);
	bindAddr.sin_addr.s_addr=inet_addr(SERVER_IP);
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0)
	{
		printf("server initerror...\n");
		return false;
	}
	int ret=bind(sockfd,(struct sockaddr*)&bindAddr,sizeof(bindAddr));
	if(ret<0)
	{
		printf("server bind error...\n");
		return false;

	}
	ret =listen(sockfd,10);
	if(ret<0)
	{
		printf("server listen error..\n");
		return false;
	}
	epollfd=epoll_create(1024);
	if(epollfd<0)
	{
		printf("server create epoll error...\n");
		return false;
	}
	pool =new threadpool<BaseTask>(threadnum);//创建线程池
	return true;
}

void Task::doit()
{                    
	char buffer[MAX_BUFFER];
    memset(buffer, 0, sizeof(buffer));
       
    while(1)
    {
		//先按包头大小收包头 获取实际数据长度
		int ret = recv(sockfd,buffer, sizeof(STRU_HEADER),0);
		int pac_size;
		if(ret > 0)
		{
			STRU_HEADER* shead = (STRU_HEADER*)buffer;
			pac_size = shead->pac_size;
			printf("%d\n",pac_size);
			//再按实际长度收取数据
			ret = recv(sockfd, buffer, pac_size ,0);
			printf("ret %d\n",ret);
			//printf("%s\n",buffer);
			//某个fd关闭了连接，从Epoll中删除并关闭fd
			if(ret == 0)
			{
				struct epoll_event ev;
				ev.events = EPOLLIN;
				ev.data.fd = sockfd;
				epoll_ctl(repollfd, EPOLL_CTL_DEL, sockfd, &ev);
				shutdown(sockfd, SHUT_RDWR);
				printf("%d 接收完成\n", sockfd);
				break;
			}
			//读取出错，尝试再次读取
			else if(ret < 0)
			{
				if(errno == EAGAIN)	
				{
					printf("read error! read again\n");
                    reset_oneshot(repollfd,sockfd);
                    cout<<"reset epollfd"<<endl;
					break;
				}
			}
			//成功读取，将读取信息发送给中介者
			else
			{
				m_TCPKernel->DealData(sockfd,buffer);
			}
   		}
		else if(ret < 0)
		{
			cout<<"error"<<endl;
			break;
		}
	} 
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
				//TCPNet::addfd(epollfd, confd, true);
				int recvsize = 500*1024*1024;
				if(setsockopt(fd, SOL_SOCKET,SO_RCVBUF,(const char*)&recvsize,sizeof(int)) == -1)
					printf("setsocket error\n");
				else
					printf("setsocket success\n");
				TCPNet::addfd(epollfd, confd, true);
			}
			else if(events[i].events & EPOLLIN)  //某个fd上有数据可读
			{
				//更改sock缓冲区大小
				/*int recvsize = 500*1024*1024;
				if(setsockopt(fd, SOL_SOCKET,SO_RCVBUF,(const char*)&recvsize,sizeof(int)) == -1)
					printf("setsocket error\n");
				else
					printf("setsocket success\n");*/
                BaseTask *task = new Task(epollfd,fd,m_pTCPKernel);
				pool->append_task(task);
			}
			else
			{
				printf("something else had happened\n");
			}
		}
	}
	pool->stop();
}

bool TCPNet::SendData(int sockfd,char *szbuf,int nlen)
{
	if(!szbuf||nlen<=0)
	{
		return false;
	}
	//包大小
	if(write(sockfd,(const char*)&nlen,sizeof(int))<0)
		return false;
	//包内容
	if(write(sockfd,szbuf,nlen)<0)
		return false;

		return true;
}
void TCPNet::UnInitNetWork()
{
	close(sockfd);
	close(epollfd);
}
