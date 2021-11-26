#include "pch.h"
#include "ClientInfo.h"

ClientInfo::ClientInfo( CString tuserName, CString tpass)
{
	userName = tuserName;
	pass = tpass;
}

ClientInfo::ClientInfo()//默认构造函数
{
}

