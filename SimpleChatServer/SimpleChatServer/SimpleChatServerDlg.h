
// SimpleChatServerDlg.h: 头文件
//
#include "ClientInfo.h"

#define LINK_MAX	64
#pragma once


// CSimpleChatServerDlg 对话框
class CSimpleChatServerDlg : public CDialogEx
{
// 构造
public:
	CSimpleChatServerDlg(CWnd* pParent = nullptr);	// 标准构造函数
	SOCKET m_listenSocket;
	SOCKET* m_curClientSocket;
	SOCKET m_toClientSocket[LINK_MAX];
	struct sockaddr addrOfClient;
	LRESULT AcceptListenMes(WPARAM wParam, LPARAM lParam);

	int waveFlag;	//语音包标志
	int linkNum;	//已连接客户端个数
	void Init();
	void SendSound();
	CList<ClientInfo, ClientInfo&> clientList;


// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SIMPLECHATSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSendbtn();
	CListBox m_listHis;
	CString m_editText;
	CEdit m_edit;
	afx_msg void OnBnClickedPlaybtn();
	afx_msg void OnLbnSelchangeListhis();
	CListBox m_clientList;
	CStatic m_linkNum;
	afx_msg void OnLbnSelchangeClientlist();
	afx_msg void OnBnClickedWavestart();
};
