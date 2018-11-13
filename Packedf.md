#ifndef _PACKDEF_H
#define _PACKDEF_H


#define SERVER_IP   "192.168.137.133"
typedef char   PackType;

#define BEGIN_PROTOCOL_MAP static const ProtocolMap m_ProtocolMapEntries[]= \
{

#define END_PROTOCOL_MAP 	{0,0} \
};


#define PM(X,Y)  {X,Y},


//�߽�ֵ
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


 


//Э��
#define _DEF_PROTOCOL_BASE		100

#define _DEF_PROTOCOL_REGISTER_RQ			 _DEF_PROTOCOL_BASE +1//ע������
#define _DEF_PROTOCOL_REGISTER_RS			 _DEF_PROTOCOL_BASE +2

#define _DEF_PROTOCOL_LOGIN_RQ				 _DEF_PROTOCOL_BASE +3//��¼����
#define _DEF_PROTOCOL_LOGIN_RS				 _DEF_PROTOCOL_BASE +4


//Э���


//ע������
struct STRU_REGISTER_RQ
{
	 PackType m_nType;
	 char m_szAccount[_DEF_SIZE];
	 char m_szPassword[_DEF_SIZE];
	 
};

//�ǳ�����
typedef  struct STRU_REGISTER_RS
{
	 PackType m_nType;
	 char m_szAccount[_DEF_SIZE];
	 char m_szResult;
}STRU_LOGIN_RS;

//��¼����
struct STRU_LOGIN_RQ
{
	 PackType m_nType;
	 char m_szAccount[_DEF_SIZE];
	 char m_szPassword[_DEF_SIZE];
	 
};



#endif