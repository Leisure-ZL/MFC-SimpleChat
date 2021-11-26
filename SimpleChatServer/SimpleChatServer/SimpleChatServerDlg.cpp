
// SimpleChatServerDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "SimpleChatServer.h"
#include "SimpleChatServerDlg.h"
#include "afxdialogex.h"


#include "ClientInfo.h"
#include"protocol.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable : 4996)
#define WM_SERLISTENMESSAME WM_APP + 10


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


// CSimpleChatServerDlg 对话框



CSimpleChatServerDlg::CSimpleChatServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SIMPLECHATSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSimpleChatServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTHIS, m_listHis);
	DDX_Control(pDX, IDC_EDITSEND, m_edit);
	DDX_Control(pDX, IDC_CLIENTLIST, m_clientList);
	DDX_Control(pDX, IDC_LINKNUM, m_linkNum);
}

BEGIN_MESSAGE_MAP(CSimpleChatServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDSENDBTN, &CSimpleChatServerDlg::OnBnClickedSendbtn)
	ON_MESSAGE(WM_SERLISTENMESSAME, AcceptListenMes)
	ON_BN_CLICKED(IDC_PlayBtn, &CSimpleChatServerDlg::OnBnClickedPlaybtn)
	ON_LBN_SELCHANGE(IDC_CLIENTLIST, &CSimpleChatServerDlg::OnLbnSelchangeClientlist)
	ON_BN_CLICKED(IDC_WAVESTART, &CSimpleChatServerDlg::OnBnClickedWavestart)
END_MESSAGE_MAP()


// CSimpleChatServerDlg 消息处理程序

BOOL CSimpleChatServerDlg::OnInitDialog()
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
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		MessageBox("Failed to load Winsock");
		return FALSE;
	}
	if ((m_listenSocket = ::socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		MessageBox("Failed to Create SOCKET", "error message");
		return FALSE;
	}
	::WSAAsyncSelect(m_listenSocket, m_hWnd,
		WM_SERLISTENMESSAME, FD_ACCEPT | FD_CLOSE);
	struct sockaddr_in addrServ;
	int retcode;
	memset(&addrServ, 0, sizeof(addrServ));
	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(9990);
	addrServ.sin_addr.s_addr = inet_addr("192.168.0.1");//前面获取的是网络字节顺序
	if (::bind(m_listenSocket, (LPSOCKADDR)&addrServ, sizeof(addrServ)) == SOCKET_ERROR)
	{
		::closesocket(m_listenSocket);
		m_listenSocket = INVALID_SOCKET;
		MessageBox("Failed to Bind SOCKET", "error message");
		return FALSE;
	}
	if (::listen(m_listenSocket, 5) == SOCKET_ERROR)
	{
		MessageBox("Failed to Listen SOCKET", "error message");
		return FALSE;
	}

	waveFlag = 0;
	InitWave();
	Init();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSimpleChatServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSimpleChatServerDlg::OnPaint()
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
HCURSOR CSimpleChatServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


LRESULT CSimpleChatServerDlg::AcceptListenMes(WPARAM wParam, LPARAM lParam)
{
	DWORD MessageLowWord, MessageHignWord;
	MessageHignWord = HIWORD(lParam);
	MessageLowWord = LOWORD(lParam);
	char buf[TEXT_BUF_SIZE];
	CString TempBufCS;
	int length;
	
	if (wParam == m_listenSocket)
	{
		if (MessageLowWord & FD_ACCEPT && MessageHignWord == 0)
		{
			int addrofclientlen = sizeof(addrOfClient);
			linkNum++;
			m_toClientSocket[linkNum] = ::accept(m_listenSocket, &addrOfClient, &addrofclientlen);
			if (m_toClientSocket[linkNum] == INVALID_SOCKET)
			{
				GetDlgItem(IDC_STATUS)->SetWindowText("接收客户的连接请求失败...");
				return 0;
			}
			::WSAAsyncSelect(m_toClientSocket[linkNum], m_hWnd, WM_SERLISTENMESSAME, FD_READ || FD_WRITE | FD_CLOSE);
			GetDlgItem(IDC_STATUS)->SetWindowText("请选择聊天对象");
			
		}
		
	}
	//else if(wParam == m_toClientSocket)
	else
	{
		for (int i = 0; i <= linkNum; i++)//遍历查询是哪个socket发的消息
		{
			if (wParam == m_toClientSocket[i])
			{
				if (MessageLowWord & FD_READ && MessageHignWord == 0)
				{
					Protocol* p = (Protocol*)buf;
					char* data = buf;
					memset(buf, 0, sizeof(buf));
					if (waveFlag) //语音包
					{
						length = ::recv(m_toClientSocket[i], (char*)buffer, BUFFER_SIZE, 0);

						if (length == SOCKET_ERROR)
						{
							MessageBox("recv failed !");
						}
						waveFlag = 0;
						return 0;
					}

					length = ::recv(m_toClientSocket[i], buf, sizeof(buf), 0);

					if (length == SOCKET_ERROR)
					{
						MessageBox("recv failed !");
					}
					if (p->ctl == '0')	//验证包
					{
						CString username;
						CString pass;
						username = strtok(p->buf, " ");
						pass = strtok(NULL, " ");//分割出用户名和密码
						username.Replace(" ", "");//去除空格
						pass.Replace(" ", "");

						//TODO:查库验证
						//这里使用一个list存储用户
						char tbuf[TEXT_BUF_SIZE] = { 0 };//封装协议
						Protocol* p = (Protocol*)tbuf;
						POSITION prePos;
						POSITION pos = clientList.GetHeadPosition();
						while (pos != NULL)
						{
							prePos = pos;
							ClientInfo ptr = clientList.GetNext(pos);
							if (ptr.userName == username && ptr.pass == pass)
							{
								PkgProtocol(p, '0', '0', "1");//通过验证
								m_listHis.ResetContent();
								m_listHis.AddString("与客户端连接成功，可以开始对话了;");
								int retVal = ::send(m_toClientSocket[i], tbuf, sizeof(tbuf), 0);//1:验证通过
								if (SOCKET_ERROR == retVal)
								{
									MessageBox("send failed !");
									closesocket(m_toClientSocket[i]);
									return 0;
								}
								clientList.GetAt(prePos).clientSocket = &m_toClientSocket[i];//为用户bind一个socket
								m_clientList.AddString(username);//更新UI
								char tmp[2];
								itoa(linkNum, tmp, 10);
								m_linkNum.SetWindowText(tmp);
								return 0;
							}
						}
						//迭代完没有成功匹配即用户名/密码不正确
						PkgProtocol(p, '0', '0', "0");
						int retVal = ::send(m_toClientSocket[i], tbuf, sizeof(tbuf), 0);//0：验证不通过
						if (SOCKET_ERROR == retVal)
						{
							MessageBox("send failed !");
							closesocket(m_toClientSocket[i]);
							return 0;
						}
						closesocket(m_toClientSocket[i]);
						linkNum--;

						return 0;
					}
					if (p->ctl == '1')	//正常传输包
					{
						CString username;
						POSITION pos = clientList.GetHeadPosition();
						while (pos != NULL)	//这里查库会快一点
						{
							ClientInfo ptr = clientList.GetNext(pos);
							if (ptr.clientSocket!=NULL && *ptr.clientSocket == m_toClientSocket[i])
							{
								username = ptr.userName;
							}
						}
						if (p->type == '0') //文本消息
						{
							data = p->buf;
							TempBufCS = username + "说：";
							TempBufCS = TempBufCS + data;
							m_listHis.AddString(TempBufCS);
						}
						else if (p->type == '1')	//语音消息
						{
							waveFlag = 1;
							TempBufCS = username + "说：<<<(语音消息，点击播放按钮收听)";
							m_listHis.AddString(TempBufCS);
						}
					}
					if (p->ctl == '2')	//断开连接
					{
						closesocket(m_toClientSocket[i]);
						linkNum--;
						for (int j = i; j <= linkNum; j++)//将socket数组中退出的socket补齐
						{
							m_toClientSocket[j] = m_toClientSocket[j + 1];
						}
						char tmp[20];//更新UI
						itoa(linkNum, tmp, 10);
						m_linkNum.SetWindowText(tmp);
						for (int i = 0; i < m_clientList.GetCount(); i++)
						{
							m_clientList.GetText(i, tmp);
							if (!strcmp(tmp, p->buf))
							{
								m_clientList.DeleteString(i);
								break;
							}
						}
						MessageBox("对方已下线！");
						if (!linkNum) 
						{
							GetDlgItem(IDC_STATUS)->SetWindowText("等待连接中...");
						}
						
						return 0;
					}

				}
				else if (MessageLowWord & FD_WRITE && MessageHignWord == 0)
				{
					;
				}
				else if (MessageLowWord & FD_CLOSE && MessageHignWord == 0)
				{

				}

			}
		}
	}
	return 0;
}


void CSimpleChatServerDlg::OnBnClickedSendbtn()
{
	int         retVal;	// 调用各种Socket函数的返回值
	int nIndex;
	char buf[TEXT_BUF_SIZE];
	CString temp,username;
	m_edit.GetWindowText(m_editText);
	sprintf_s(buf, TEXT_BUF_SIZE, "%s\n", (LPCTSTR)m_editText);
	char tbuf[TEXT_BUF_SIZE] = { 0 };
	Protocol* p = (Protocol*)tbuf;
	PkgProtocol(p, '1', '0', buf);//正常传输、文本内容
	if (m_curClientSocket == NULL)
	{
		MessageBox("请选择发送对象！");
		return;
	}
	retVal = ::send(*m_curClientSocket, tbuf, sizeof(tbuf), 0);
	if (SOCKET_ERROR == retVal)
	{
		MessageBox("send failed !");
		closesocket(*m_curClientSocket);
		return;
	}
	nIndex = m_clientList.GetCurSel();
	m_clientList.GetText(nIndex, username);
	temp = "我向" + username+"说：";
	temp = temp + m_editText;
	m_listHis.AddString(temp);
	m_edit.SetWindowText("");
	m_editText = "";

}


void CSimpleChatServerDlg::OnBnClickedPlaybtn()
{
	// TODO: 在此添加控件通知处理程序代码
	Play((char*)buffer);
}


void CSimpleChatServerDlg::Init()
{
	linkNum = 0;

	//TODO:自己添加用户
	ClientInfo c1( "root", "123456");
	ClientInfo c2( "zl", "123456");
	ClientInfo c3("张三", "123456");
	ClientInfo c4("李四", "123456");
	clientList.AddTail(c1);
	clientList.AddTail(c2);
	clientList.AddTail(c3);
	clientList.AddTail(c4);

}

void CSimpleChatServerDlg::OnLbnSelchangeClientlist()
{
	CString str, TempBufCS;
	int nIndex = 0;

	nIndex = m_clientList.GetCurSel();
	m_clientList.GetText(nIndex, str);

	POSITION pos = clientList.GetHeadPosition();
	while (pos != NULL)	//这里查库会快一点
	{
		ClientInfo ptr = clientList.GetNext(pos);
		if (!strcmp(ptr.userName, str))
		{
			m_curClientSocket = ptr.clientSocket;//获取当前选中用户的socket
		}
	}

	TempBufCS = "聊天对象：";
	TempBufCS = TempBufCS + str;
	GetDlgItem(IDC_STATUS)->SetWindowText(TempBufCS);

}


void CSimpleChatServerDlg::OnBnClickedWavestart()
{
	// TODO: 在此添加控件通知处理程序代码
	Record(this);
	SendSound();

}

void CSimpleChatServerDlg::SendSound() {
	int         retVal;	// 调用各种Socket函数的返回值
	CString temp;
	temp = "我向对方说：>>>(一段语音消息)";
	m_listHis.AddString(temp);


	char tbuf[TEXT_BUF_SIZE] = { 0 };
	Protocol* p = (Protocol*)tbuf;
	PkgProtocol(p, '1', '1', "0");
	retVal = ::send(*m_curClientSocket, tbuf, sizeof(tbuf), 0);//第一次传输语音握手包，

	if (SOCKET_ERROR == retVal)
	{
		MessageBox("send failed !");
		closesocket(*m_curClientSocket);
		return;
	}
	retVal = ::send(*m_curClientSocket, (char*)buffer, BUFFER_SIZE, 0);//第二次传输真正语音包

}