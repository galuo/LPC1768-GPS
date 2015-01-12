
// 5_9Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "5_9.h"
#include "5_9Dlg.h"
#include "afxdialogex.h"
#include "WebPage.h"  //封装了类CWebPage，用来和HTML，JSP交互

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMy5_9Dlg 对话框



CMy5_9Dlg::CMy5_9Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMy5_9Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy5_9Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER, m_Explorer);
}

BEGIN_MESSAGE_MAP(CMy5_9Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START_TCP, &CMy5_9Dlg::OnBnClickedButtonStartTcp)
	ON_BN_CLICKED(IDC_BUTTON_STOP_TCP, &CMy5_9Dlg::OnBnClickedButtonStopTcp)
	ON_MESSAGE(WM_UPDATE_CONNECTION, OnUpdateConnection)
	ON_MESSAGE(WM_GETNEWSTR, OnGetNewStr)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CMy5_9Dlg 消息处理程序

BOOL CMy5_9Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CString s;
	TCHAR szFilePath[MAX_PATH + 1]={0};
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	(_tcsrchr(szFilePath, _T('\\')))[1] = 0;//删除文件名，只获得路径字串
	CString str_url = szFilePath; //例如str_url==e:\program\Debug\ 
		
	s.Format(_T("%sGoogle_map.html"),str_url);
	m_Explorer.Navigate(s,NULL,NULL,NULL, NULL);

	//建立Excel表格，并存储
	s.Format(_T("%sa.xls"),str_url);
	FILE *f = fopen (s, "ab,ccs=UNICODE");
	miniexcel(3,0) = "Item1:";
	miniexcel(1,0) = "Item2:";
	miniexcel(2,0) = "Sum =";
	miniexcel(2,0).setBorder(BORDER_LEFT | BORDER_TOP | BORDER_BOTTOM);
	miniexcel(2,0).setAlignament(ALIGN_CENTER);
	
	miniexcel(0,1) = 10;
	miniexcel(1,1) = 20;
	miniexcel(2,1) = (double)miniexcel(0,1) + (double)miniexcel(1,1);
	miniexcel(2,1).setBorder(BORDER_RIGHT | BORDER_TOP | BORDER_BOTTOM);
	
	miniexcel.Write(f);

	GetDlgItem(IDC_EDIT_TCP_SERVERPORT)->SetWindowText("12345");
	GetDlgItem(IDC_BUTTON_STOP_TCP)->EnableWindow( FALSE );
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMy5_9Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMy5_9Dlg::OnPaint()
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
HCURSOR CMy5_9Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CMy5_9Dlg::OnGetNewStr(WPARAM nLen, LPARAM strText)
{	
	CString* rmsg = (CString*)strText;
	int nPos;
	int A,E,N;	
	CWebPage web;
	A= rmsg[0].Find("A");
	E= rmsg[0].Find("E");
	N= rmsg[0].Find("N");
	if(A>=0 && E>=0 && N>=0)
	{
		m_longitude=rmsg[0].Mid((A+1),9);		//解析得到经度
		m_latitude=rmsg[0].Mid((E+1),8);     //解析得到纬度
		nPos=m_longitude.Find("0");//确定GPS是否有信号
		if(!(nPos==0))
		{
			nPos=m_latitude.Find("0");
			if(!(nPos==0))
			{
				web.SetDocument(m_Explorer.get_Document());
				web.CallJScript("TOLatLng", m_latitude, m_longitude);
				GetDlgItem(IDC_EDIT_longitude)->SetWindowText(m_longitude);
				GetDlgItem(IDC_EDIT_LATITUDE)->SetWindowText(m_latitude);
			}
		}
	}
	return 1L;
}
///////////////////////////////////////////////////////////////////////////////
// OnUpdateConnection
// This message is sent by server manager to indicate connection status
LRESULT CMy5_9Dlg::OnUpdateConnection(WPARAM wParam, LPARAM lParam)
{
	UINT uEvent = (UINT) wParam;
	CSocketManager* pManager = reinterpret_cast<CSocketManager*>( lParam );

	if ( pManager != NULL)
	{
		// Server socket is now connected, we need to pick a new one
		if (uEvent == EVT_CONSUCCESS)
		{
			PickNextAvailable();
			StartServer();
		}
		else if (uEvent == EVT_CONFAILURE || uEvent == EVT_CONDROP)
		{
			pManager->StopComm();
			if (m_pCurServer == NULL)
			{
				PickNextAvailable();
				StartServer();
			}
		}
	}

	return 1L;
}
void CMy5_9Dlg::OnBnClickedButtonStartTcp()
{
	// TODO: 在此添加控件通知处理程序代码
	for(int i=0; i<MAX_CONNECTION; i++)
	{
		m_SocketManager[i].SetMessageWindow( (CEdit*)GetDlgItem(IDC_EDIT_RECEIVE));
		m_SocketManager[i].SetServerState( true );	// run as server
	}

	PickNextAvailable();
	UpdateData();

	StartServer();
}


void CMy5_9Dlg::OnBnClickedButtonStopTcp()
{
	// TODO: 在此添加控件通知处理程序代码
	for(int i=0; i<MAX_CONNECTION; i++)
	m_SocketManager[i].StopComm();

	if (!m_pCurServer->IsOpen())
	{
		GetDlgItem(IDC_BUTTON_START_TCP)->EnableWindow( TRUE );
		GetDlgItem(IDC_BUTTON_STOP_TCP)->EnableWindow( FALSE );
	}
}

///////////////////////////////////////////////////////////////////////////////
// StartServer : Start the server
bool CMy5_9Dlg::StartServer()
{
	bool bSuccess = false;
	if (m_pCurServer != NULL)
	{
		GetDlgItem(IDC_EDIT_TCP_SERVERPORT)->GetWindowText(m_strPort);
		// no smart addressing - we use connection oriented
		m_pCurServer->SetSmartAddressing( false );
		bSuccess = m_pCurServer->CreateSocket( m_strPort, AF_INET, SOCK_STREAM, 0); // TCP

		if (bSuccess && m_pCurServer->WatchComm())
		{
			GetDlgItem(IDC_BUTTON_START_TCP)->EnableWindow( FALSE );
			GetDlgItem(IDC_BUTTON_STOP_TCP)->EnableWindow( TRUE );
			CString strServer, strAddr;
			m_pCurServer->GetLocalName( strServer.GetBuffer(256), 256);
			strServer.ReleaseBuffer();
			m_pCurServer->GetLocalAddress( strAddr.GetBuffer(256), 256);
			strAddr.ReleaseBuffer();
			CString strMsg = _T("Server: ") + strServer;
					strMsg += _T(", @Address: ") + strAddr;
					strMsg += _T(" is running on port ") + m_strPort + CString("\r\n");
			m_pCurServer->AppendMessage( strMsg );
		}
	}

	return bSuccess;
}

///////////////////////////////////////////////////////////////////////////////
// PickNextAvailable : this is useful only for TCP socket
void CMy5_9Dlg::PickNextAvailable()
{
	m_pCurServer = NULL;
	for(int i=0; i<MAX_CONNECTION; i++)
	{
		if (!m_SocketManager[i].IsOpen())
		{
			m_pCurServer = &m_SocketManager[i];
			break;
		}
	}
}

void CMy5_9Dlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	for(int i=0; i<MAX_CONNECTION; i++)
	m_SocketManager[i].StopComm();
}
