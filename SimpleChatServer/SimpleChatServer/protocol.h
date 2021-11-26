#pragma once

#include"MySound.h"
#define TEXT_BUF_SIZE	2048
//用户协议
struct Protocol
{
	char ctl;		//控制字段	1个字节		控制该包是关闭信号/用户信息/验证
	char type;		//类型字段	1个字节		该包是文本/语音...
	char buf[BUFFER_SIZE];	//传输内容	
};


/*
ctl:	控制信息
0		建立连接，发送用户名、密码
1		正常传输
2		断开连接，一次挥手

type:	用户数据类型
0		文本内容
1		语音内容

buf		用户数据

*/


void PkgProtocol(Protocol* p, char ctl, char type, char* data)
{
	p->ctl = ctl;
	p->type = type;
	memcpy(p->buf, data, strlen(data));
}


