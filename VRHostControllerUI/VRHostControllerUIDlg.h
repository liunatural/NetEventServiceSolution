
// VRHostControllerUIDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include <thread>
#include "afxdialogex.h"
#include "VRHostController.h"

#include "Util.h"
#include "afxcmn.h"


#define WM_MYMESSAGE (WM_USER + 100)



// CVRHostControllerUIDlg dialog
class CVRHostControllerUIDlg : public CDialogEx
{
// Construction
public:
	CVRHostControllerUIDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VRHOSTCONTROLLERUI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnMyMessage(WPARAM w, LPARAM l);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnStop();
	
	void SendUserConnectMsg(Output_Log* log);
	void RefreshVRHostList();


	std::shared_ptr<std::thread> m_StartThread;
	VRHostController		*pvrHostController;
	ClientMessage			msg;

	CListBox m_vrhost_listbox;
	CListCtrl m_vrhost_listctrl;
	CListBox m_msg_listbox;

	CEdit m_edit_svr_port;
	CEdit m_edit_svr_maxlinks;
	CButton m_btn_start;
	CButton m_btn_stop;

};
