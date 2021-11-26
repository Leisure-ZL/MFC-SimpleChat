
// SimpleChatClientDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "SimpleChatClient.h"
#include "SimpleChatClientDlg.h"
#include "afxdialogex.h"

#include "Protocol.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define BUF_SIZE    64
#pragma warning(disable : 4996)
#define WM_SERLISTENMESSAME WM_APP + 10
#define TEXT_BUF_SIZE	2048

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSimpleChatClientDlg 对话框



CSimpleChatClientDlg::CSimpleChatClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SIMPLECHATCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSimpleChatClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTBOX, m_listHis);
	DDX_Control(pDX, IDC_EDITTEXT, m_edit);
	DDX_Control(pDX, IDC_EDITIP, m_ipEdit);
	DDX_Control(pDX, IDC_EDITUSER, m_userEdit);
	DDX_Control(pDX, IDC_EDITPASS, m_passEdit);
	DDX_Control(pDX, IDC_EDITPORT, m_portEdit);
	DDX_Control(pDX, IDC_STATE, m_state);
	DDX_Control(pDX, IDC_WAVESTART, m_waveStartBtn);
	DDX_Control(pDX, IDC_WAVEPLAY, m_wavePlayBtn);
	DDX_Control(pDX, IDSENDBTN, m_sendBtn);
	DDX_Control(pDX, IDC_CONNECT, m_connectBtn);
	DDX_Control(pDX, IDEXITBTN, m_exitBtn);
}

BEGIN_MESSAGE_MAP(CSimpleChatClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SERLISTENMESSAME, AcceptListenMes)
	ON_BN_CLICKED(IDC_CONNECT, &CSimpleChatClientDlg::OnBnClickedConnect)
	ON_BN_CLICKED(IDSENDBTN, &CSimpleChatClientDlg::OnBnClickedSendbtn)
	ON_BN_CLICKED(IDEXITBTN, &CSimpleChatClientDlg::OnBnClickedExit)
	ON_BN_CLICKED(IDC_WAVESTART, &CSimpleChatClientDlg::OnBnClickedWavestart)
	ON_BN_CLICKED(IDC_WAVEPLAY, &CSimpleChatClientDlg::OnBnClickedWaveplay)
END_MESSAGE_MAP()


// CSimpleChatClientDlg 消息处理程序

BOOL CSimpleChatClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	
	InitWave();
	connectFlag = 0;
	waveFlag = 0;
	//初始化按钮不可点击
	m_exitBtn.EnableWindow(false);
	m_sendBtn.EnableWindow(false);
	m_wavePlayBtn.EnableWindow(false);
	m_waveStartBtn.EnableWindow(false);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSimpleChatClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSimpleChatClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSimpleChatClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


LRESULT CSimpleChatClientDlg::AcceptListenMes(WPARAM wParam, LPARAM lParam)
{
	DWORD MessageLowWord, MessageHignWord;
	MessageHignWord = HIWORD(lParam);
	MessageLowWord = LOWORD(lParam);
	char buf[BUFFER_SIZE];
	CString TempBufCS;
	int length;
	struct user_protocol* plogininfo;
	char bufuser[128], bufpass[128];
	int retlen;


	if (MessageLowWord & FD_READ && MessageHignWord == 0)
	{
		Protocol* p = (Protocol*)buf;
		char* data = buf;
		memset(buf, 0, sizeof(buf));

		if (waveFlag) //语音包
		{
			length = ::recv(m_toServerSocket, (char*)buffer, BUFFER_SIZE, 0);

			if (length == SOCKET_ERROR)
			{
				MessageBox("recv failed !");
			}
			waveFlag = 0;
			return 0;
		}

		length = ::recv(m_toServerSocket, buf, sizeof(buf), 0);
		if (length == SOCKET_ERROR)
		{
			MessageBox("recv failed !");
		}
		if (p->ctl == '0')	//验证返回包
		{
			if (!strcmp(p->buf ,"0")) 
			{
				MessageBox("用户名或密码错误！");
				return 0;
			}
			MessageBox("连接成功！");
			GetDlgItem(IDC_STATUS)->SetWindowText("通话中...");
			m_listHis.ResetContent();
			m_listHis.AddString("与服务器连接成功，可以开始对话了;");
			connectFlag = 1;
			m_connectBtn.EnableWindow(false);
			m_exitBtn.EnableWindow(true);
			m_sendBtn.EnableWindow(true);
			m_wavePlayBtn.EnableWindow(true);
			m_waveStartBtn.EnableWindow(true);
			return 0;
		}
		if (p->ctl == '1')	//正常传输包
		{
			if (p->type == '0') //文本消息
			{
				data = p->buf;
				TempBufCS = "对方说：";
				TempBufCS = TempBufCS + data;
				m_listHis.AddString(TempBufCS);
			}
			else if (p->type == '1')	//语音消息
			{
				waveFlag = 1;
				TempBufCS = "对方说：<<<(语音消息，点击播放按钮收听)";
				m_listHis.AddString(TempBufCS);
			}
		}
		return 0;
	}
}

void CSimpleChatClientDlg::OnBnClickedConnect()
{
	HMODULE hModule = ::GetModuleHandle(nullptr);
	WSADATA     wsd;		// 用于初始化Windows Socket    
	SOCKADDR_IN servAddr;	// 服务器地址     
	int         retVal;	// 调用各种Socket函数的返回值

	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		MessageBox("WSAStartup failed !");
	}
	m_toServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == m_toServerSocket)
	{
		MessageBox("socket failed !");
		WSACleanup();
	}

	CString serverIP,serverPort;
	m_ipEdit.GetWindowText(serverIP);//获取输入框服务器IP,port
	m_portEdit.GetWindowText(serverPort);

	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = inet_addr(serverIP);
	servAddr.sin_port = htons(atoi(serverPort));
	//servAddr.sin_addr.S_un.S_addr = inet_addr("192.168.0.1");
	//servAddr.sin_port = htons(9990);

	retVal = connect(m_toServerSocket, (LPSOCKADDR)&servAddr, sizeof(servAddr));
	if (SOCKET_ERROR == retVal)
	{
		MessageBox("连接失败，请检查服务器IP地址与端口号！");
		closesocket(m_toServerSocket);
		WSACleanup();
		return;
	}
	else
	{
		connectFlag = 1;
		
		WSAAsyncSelect(m_toServerSocket, m_hWnd, WM_SERLISTENMESSAME, FD_READ || FD_WRITE | FD_CLOSE);//监听消息

		//获取用户名、密码
		CString tStr;
		char tmp[100];
		m_userEdit.GetWindowText(username);
		m_passEdit.GetWindowText(pass);
		
		tStr = username + ' ' + pass;//用空格分开用户名和密码
		memcpy(tmp, tStr, 100);

		char tbuf[TEXT_BUF_SIZE] = { 0 };//封装协议
		Protocol* p = (Protocol*)tbuf;
		PkgProtocol(p, '0', '0', tmp);
		int retVal = ::send(m_toServerSocket, tbuf, sizeof(tbuf), 0);//发送验证

		if (SOCKET_ERROR == retVal)
		{
			MessageBox("send failed !");
			closesocket(m_toServerSocket);
			return ;
		}
	}
	
}



void CSimpleChatClientDlg::OnBnClickedSendbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	int         retVal;	// 调用各种Socket函数的返回值
	char buf[TEXT_BUF_SIZE];
	CString temp;
	m_edit.GetWindowText(m_editText);
	temp = "我向对方说：";
	temp = temp + m_editText;   
	m_listHis.AddString(temp);
	sprintf_s(buf, TEXT_BUF_SIZE, "%s\n", (LPCTSTR)m_editText);

	char tbuf[TEXT_BUF_SIZE] = { 0 };
	Protocol* p = (Protocol*)tbuf;
	PkgProtocol(p, '1', '0', buf);//正常传输、文本内容
	retVal = ::send(m_toServerSocket, tbuf, sizeof(tbuf), 0);
	if (SOCKET_ERROR == retVal)
	{
		MessageBox("send failed !");
		closesocket(m_toServerSocket);
		return;
	}
	m_editText = "";
	m_edit.SetWindowText("");
}


void CSimpleChatClientDlg::OnBnClickedExit()
{
	// TODO: 在此添加控件通知处理程序代码
	//向服务器发送一个挥手包
	char tmp[20];
	memcpy(tmp, username, 20);

	char tbuf[TEXT_BUF_SIZE] = { 0 };
	Protocol* p = (Protocol*)tbuf;
	PkgProtocol(p, '2', '0', tmp);	//将用户名传过去，方便服务器判别
	int retVal = ::send(m_toServerSocket, tbuf, sizeof(tbuf), 0);
	if (SOCKET_ERROR == retVal)
	{
		MessageBox("send failed !");
		closesocket(m_toServerSocket);
		return;
	}
	closesocket(m_toServerSocket);
	WSACleanup();
	MessageBox("已退出！");
	GetDlgItem(IDC_STATUS)->SetWindowText("未连接");
	m_listHis.ResetContent();	//清空
	connectFlag = 0;
	m_connectBtn.EnableWindow(true);
	m_exitBtn.EnableWindow(false);
	m_sendBtn.EnableWindow(false);
	m_wavePlayBtn.EnableWindow(false);
	m_waveStartBtn.EnableWindow(false);

}


void CSimpleChatClientDlg::OnBnClickedWavestart()
{
	// TODO: 在此添加控件通知处理程序代码
	Record(this);
	SendSound();
}

void CSimpleChatClientDlg::OnBnClickedWaveplay()
{
	// TODO: 在此添加控件通知处理程序代码
	Play((char*)buffer);
}

void CSimpleChatClientDlg::SendSound() {
	int         retVal;	// 调用各种Socket函数的返回值
	CString temp;
	temp = "我向对方说：>>>(一段语音消息)";
	m_listHis.AddString(temp);


	char tbuf[TEXT_BUF_SIZE] = { 0 };
	Protocol* p = (Protocol*)tbuf;
	PkgProtocol(p, '1', '1', "0");
	retVal = ::send(m_toServerSocket, tbuf, sizeof(tbuf), 0);//第一次传输语音握手包，
	
	if (SOCKET_ERROR == retVal)
	{
		MessageBox("send failed !");
		closesocket(m_toServerSocket);
		return;
	}
	retVal = ::send(m_toServerSocket, (char*)buffer, BUFFER_SIZE, 0);//第二次传输真正语音包

}