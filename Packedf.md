#ifndef _PACKDEF_H
#define _PACKDEF_H


#define SERVER_IP   "192.168.137.133"
typedef char   PackType;

#define BEGIN_PROTOCOL_MAP static const ProtocolMap m_ProtocolMapEntries[]= \
{

#define END_PROTOCOL_MAP 	{0,0} \
};


#define PM(X,Y)  {X,Y},


//边界值
#define _DEF_SIZE		     	64
#define _DEF_STREAMSIZE			1000
#define _DEF_SQLLEN             300
#define _DEF_BUFFERNUM          10000




//
#define _register_accountexists  0
#define _register_success		 1

#define _login_accountnoexists      0
#define _login_passworderr       1
#define _login_success           2


 


//协议
#define _DEF_PROTOCOL_BASE		100

#define _DEF_PROTOCOL_REGISTER_RQ			 _DEF_PROTOCOL_BASE +1//注册请求
#define _DEF_PROTOCOL_REGISTER_RS			 _DEF_PROTOCOL_BASE +2

#define _DEF_PROTOCOL_LOGIN_RQ				 _DEF_PROTOCOL_BASE +3//登录请求
#define _DEF_PROTOCOL_LOGIN_RS				 _DEF_PROTOCOL_BASE +4


//协议包


//注册请求
struct STRU_REGISTER_RQ
{
	 PackType m_nType;
	 char m_szAccount[_DEF_SIZE];
	 char m_szPassword[_DEF_SIZE];
	 
};

//登出请求
typedef  struct STRU_REGISTER_RS
{
	 PackType m_nType;
	 char m_szAccount[_DEF_SIZE];
	 char m_szResult;
}STRU_LOGIN_RS;

//登录请求
struct STRU_LOGIN_RQ
{
	 PackType m_nType;
	 char m_szAccount[_DEF_SIZE];
	 char m_szPassword[_DEF_SIZE];
	 
};



#endif