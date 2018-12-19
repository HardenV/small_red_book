class TCPNet
{
public :
	TCPNet(){}
	~TCPNet()
	{
		UnInitNetWork();
	}
public:
	bool InitNetWork(int port);
	void UnInitNetWork();
	//bool SendData(char* szbuf,int nlen);
public:
	static  unsigned _stdcall ThreadCon( void * );
	static void InitCS();
private:
	SOCKET sockClient;
	HANDLE m_hThreadCon;
	static int con;
};

int TCPNet::con = 0;
CRITICAL_SECTION g_CS;

bool TCPNet::InitNetWork(int port)
{   
	//2.雇佣店长 -- socket 
	sockClient = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sockClient == INVALID_SOCKET )
	{
		UnInitNetWork();
		 return false;
	}

	//3.client选址 --- 
	sockaddr_in  addrClient;
	addrClient.sin_family = AF_INET;
	addrClient.sin_addr.S_un.S_addr = inet_addr("10.50.104.210");
	addrClient.sin_port = htons(port);
	if(SOCKET_ERROR == bind(sockClient,(const sockaddr*)&addrClient,sizeof(addrClient)))
	{
		 UnInitNetWork();
		 return false;
	}

	//5.创建线程
	m_hThreadCon = (HANDLE)_beginthreadex(NULL,0,&ThreadCon,this,0,0);
	
	return true;
}

void TCPNet::UnInitNetWork()
{
	if(m_hThreadCon)
	{
		if(WAIT_TIMEOUT == WaitForSingleObject(m_hThreadCon,100))
			TerminateThread(m_hThreadCon,-1);
		
		CloseHandle(m_hThreadCon);
		m_hThreadCon = NULL;
	}
	if(sockClient)
	{
		closesocket(sockClient);
		sockClient = NULL;
	}
	cout<<"出错啦"<<endl;
}

unsigned _stdcall TCPNet::ThreadCon( void * lpvoid)
{
	TCPNet* pthis  = (TCPNet*)lpvoid;
	//4.server选址 --- 
	sockaddr_in  addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_addr.S_un.S_addr = inet_addr("10.50.66.240");
	addrServer.sin_port = htons(8000);
	if(SOCKET_ERROR == connect(pthis->sockClient,(const sockaddr*)&addrServer,sizeof(addrServer)))
	{
		pthis->UnInitNetWork();
	}
	EnterCriticalSection(&g_CS);
	cout<<con++<<endl;
	LeaveCriticalSection(&g_CS);
	while(1)
	{
		Sleep(20);
	}
	return 0;
}

void TCPNet::InitCS()
{
	InitializeCriticalSection(&g_CS);
}

int main()
{
	TCPNet tcpNet[3000];
	int port = 10000;
	//1.选择种类 -- 
	WORD wVersionRequested;
    WSADATA wsaData;
    int err;

	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        /* Tell the user that we could not find a usable */
        /* Winsock DLL.                                  */
        // printf("WSAStartup failed with error: %d\n", err);
        return false;
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        // printf("Could not find a usable version of Winsock.dll\n");
        for(int i=0;i<1000;i++)
		{
			tcpNet[i].UnInitNetWork();
		}
        return false;
    }

	TCPNet::InitCS();

	for(int i=0;i<3000;i++)
	{
		tcpNet[i].InitNetWork(port);
		port++;
		Sleep(1);
	}
	while(1)
	{
		Sleep(20);
	}

	WSACleanup();
	return 0;
}
