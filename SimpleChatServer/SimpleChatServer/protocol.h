#pragma once

#include"MySound.h"
#define TEXT_BUF_SIZE	2048
//�û�Э��
struct Protocol
{
	char ctl;		//�����ֶ�	1���ֽ�		���Ƹð��ǹر��ź�/�û���Ϣ/��֤
	char type;		//�����ֶ�	1���ֽ�		�ð����ı�/����...
	char buf[BUFFER_SIZE];	//��������	
};


/*
ctl:	������Ϣ
0		�������ӣ������û���������
1		��������
2		�Ͽ����ӣ�һ�λ���

type:	�û���������
0		�ı�����
1		��������

buf		�û�����

*/


void PkgProtocol(Protocol* p, char ctl, char type, char* data)
{
	p->ctl = ctl;
	p->type = type;
	memcpy(p->buf, data, strlen(data));
}


