
// SimpleChatClientDlg.h: 头文件
//
#pragma once
// CSimpleChatClientDlg 对话框
class CSimpleChatClientDlg : public CDialogEx
{
// 构造
public:
	CSimpleChatClientDlg(CWnd* pParent = nullptr);	// 标准构造函数
	SOCKET m_listenSocket, m_toServerSocket;
	struct sockaddr addrOfServer;
	LRESULT AcceptListenMes(WPARAM wParam, LPARAM lParam);
	
	void SendSound();
	int connectFlag;
	CString username, pass;
	int waveFlag;



// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SIMPLECHATCLIENT_DIALOG };
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
	DWORD WINAPI MTClientRecvThread(LPVOID pParam);
	CListBox m_listHis;
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedSendbtn();
	afx_msg void OnBnClickedExit();
	CEdit m_edit;
	CString m_editText;

	afx_msg void OnBnClickedWavestart();
	afx_msg void OnBnClickedWaveplay();

	

	CEdit m_ipEdit;
	CEdit m_userEdit;
	CEdit m_passEdit;
	CEdit m_portEdit;
	CStatic m_state;
	CButton m_waveStartBtn;
	CButton m_wavePlayBtn;
	CButton m_sendBtn;
	CButton m_connectBtn;
	CButton m_exitBtn;
};



