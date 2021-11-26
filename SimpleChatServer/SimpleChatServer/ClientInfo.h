
#pragma once
class ClientInfo
{
public:
	SOCKET* clientSocket;
	CString userName;
	CString pass;

	ClientInfo();
	ClientInfo(CString tuserName,CString tpass);
};

