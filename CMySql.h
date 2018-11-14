#pragma once

#include"head.h"



class CMySql

{

public:
	 
	CMySql(void);
	 
	~CMySql(void);

public:
	
	bool  ConnectMySql(char *host,char *user,char *pass,char *db);
	
	void  DisConnect();
	
	bool  SelectMySql(char* szSql,int nColumn,list<string>&lstStr);
	
	//更新：删除、插入、修改
    
	bool  UpdateMySql(char* szSql);

private:
    
	MYSQL *sock;   
    
	MYSQL_RES *results;   
	
	MYSQL_ROW record; 
								   

 
};
