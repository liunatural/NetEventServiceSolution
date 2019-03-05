
// VRHostControllerUIDlg.cpp : implementation file
//
#include "stdafx.h"
#include "VRHostControllerUI.h"
#include "VRHostControllerUIDlg.h"
#include "RemoteClientManager.h"
#include <string>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CVRHostControllerUIDlg dialog



CVRHostControllerUIDlg::CVRHostControllerUIDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_VRHOSTCONTROLLERUI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVRHostControllerUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MSG, m_msg_listbox);
	DDX_Control(pDX, IDC_LIST_VRHOST, m_vrhost_listbox);
	DDX_Control(pDX, IDC_LC_VRHOST, m_vrhost_listctrl);
	DDX_Control(pDX, IDC_EDIT_SVR_PORT, m_edit_svr_port);
	DDX_Control(pDX, IDC_EDIT_SVR_MAX_LINKS, m_edit_svr_maxlinks);
	DDX_Control(pDX, IDC_BTN_START, m_btn_start);
	DDX_Control(pDX, IDC_BTN_STOP, m_btn_stop);
}

void CVRHostControllerUIDlg::SendUserConnectMsg(Output_Log* log)
{
	::PostMessage(GetSafeHwnd(), WM_MYMESSAGE, 0, (LPARAM)log);
}


BEGIN_MESSAGE_MAP(CVRHostControllerUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_START, &CVRHostControllerUIDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_STOP, &CVRHostControllerUIDlg::OnBnClickedBtnStop)
	ON_MESSAGE(WM_MYMESSAGE, &CVRHostControllerUIDlg::OnMyMessage)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CVRHostControllerUIDlg message handlers

BOOL CVRHostControllerUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon


	
	//*****初始化VR主机控制器对象*****//
	pvrHostController = NULL;

	//*****初始化列表控件*****//
	DWORD dwStyle = m_vrhost_listctrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	m_vrhost_listctrl.SetExtendedStyle(dwStyle);
	
	m_vrhost_listctrl.InsertColumn(0, _T(" "), LVCFMT_LEFT, 25);
	m_vrhost_listctrl.InsertColumn(1, _T("VR主机IP地址"), LVCFMT_LEFT, 120);
	m_vrhost_listctrl.InsertColumn(2, _T("座位号"), LVCFMT_LEFT, 80);
	m_vrhost_listctrl.InsertColumn(3, _T("用户ID"), LVCFMT_LEFT, 120);

	GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);

	//*****开启日志系统文件*****//
	InitLogger("Log/HostController");
	SetTimer(1, 2000, NULL);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVRHostControllerUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVRHostControllerUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVRHostControllerUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CVRHostControllerUIDlg::OnBnClickedBtnStart()
{
	int ret = 0;

	pvrHostController =  new VRHostController(this);

	//*****读取配置文件*******//
	ret = pvrHostController->ReadConfigFile();
	if (ret == FAIL)
	{
		return;
	}

	ret = pvrHostController->CreateUserSeatMap();
	if (ret == FAIL)
	{
		return;
	}

	//*****启动VR主机控制器*****//
	ret = pvrHostController->Start();
	if (ret == FAIL)
	{
		m_msg_listbox.AddString(L"VR主机控制器启动失败！");
		return;
	}
	m_msg_listbox.AddString(L"VR主机控制器启动成功！");

	CString strPort(std::to_string(pvrHostController->GetPort()).c_str());
	CString strMaxLinks(std::to_string(pvrHostController->GetMaxLinks()).c_str());
	m_edit_svr_port.SetWindowText(strPort);
	m_edit_svr_maxlinks.SetWindowText(strMaxLinks);

	GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);

	//*****创建用户管理器*****//
	ret = pvrHostController->CreateVRClientManager();
	if (ret == FAIL)
	{
		return;
	}

	m_StartThread.reset(new std::thread([this]
	{
		//*****进行消息处理*****//
		pvrHostController->Run();
	}));
}


void CVRHostControllerUIDlg::OnBnClickedBtnStop()
{
	m_vrhost_listbox.ResetContent();
	m_vrhost_listctrl.DeleteAllItems();
	
	if (pvrHostController != NULL)
	{
		pvrHostController->SetStopFlag(true);
		m_StartThread->join();

		delete pvrHostController;
		pvrHostController = NULL;

		m_msg_listbox.AddString(L"VR主机控制器停止！");

		GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);

	}


}


LRESULT CVRHostControllerUIDlg::OnMyMessage(WPARAM wParam, LPARAM lParam) 
{
	Output_Log*  cmsg = (Output_Log*)lParam;

	int i = cmsg->m_logType;
	CString str(cmsg->m_logStr.c_str());

	m_msg_listbox.AddString(str);

	return LRESULT(0);


	//::PostMessage(GetSafeHwnd(), WM_MYMESSAGE, 0, 0); //GetSafeHwnd()表示得到当前窗口句柄，所以这条语句是发送给当前窗口  
	//GetParent()->PostMessage(WM_MYMESSAGE, 0, 0);//发送给父窗口   
	//this->PostMessage(WM_MYMESSAGE, 0, 0);//发送给当前窗口  
	//HWND h = ::FindWindow(NULL, _T("窗口名字"));   ::PostMessage(h, WM_MYMESSAGE, 0, 0);//发送给任意窗口SendMessage也是同样的用法。

}



void CVRHostControllerUIDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 1:
		RefreshVRHostList();
		break;
	case 2:
		break;
	case 3:
		break;
	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CVRHostControllerUIDlg::RefreshVRHostList()
{
	if (pvrHostController)
	{

		m_vrhost_listbox.ResetContent();
		m_vrhost_listctrl.DeleteAllItems();

		RemoteClientManager* pClientCollection = pvrHostController->GetClientCollection();
	
		//RemoteClientManager对象为多线程使用的对象，使用时需要加锁
		boost::mutex::scoped_lock lock(pClientCollection->GetMutex());
	
		RemoteClientManager::iterator it;
		int line = 0;

		for (it = pClientCollection->begin(); it != pClientCollection->end(); it++)
		{
			RemoteClient* rc = *it;
			if(rc->GetClientType() != VRClientAgent)
				continue;

			CString cip(rc->GetIP());
		
			std::string strSN = std::to_string(rc->GetSeatNumber());


			m_vrhost_listbox.AddString(cip);


			CString csn(strSN.c_str());
			CString cuid(rc->GetUserInfo()->UserID);


			m_vrhost_listctrl.InsertItem(line, _T(""));
			m_vrhost_listctrl.SetItemText(line, 1, cip);
			m_vrhost_listctrl.SetItemText(line, 2, csn);
			m_vrhost_listctrl.SetItemText(line, 3, cuid);
		}
	}
}
