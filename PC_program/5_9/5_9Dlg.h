
// 5_9Dlg.h : ͷ�ļ�
//

#pragma once
#include "explorer.h"
#include "SocketManager.h"
#include "mexcel.h"
using namespace miniexcel;

#define MAX_CONNECTION		10

// CMy5_9Dlg �Ի���
class CMy5_9Dlg : public CDialogEx
{
// ����
public:
	CMy5_9Dlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MY5_9_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	CString	m_strPort;
	CSocketManager m_SocketManager[MAX_CONNECTION];
	CSocketManager* m_pCurServer;

	void PickNextAvailable();
	bool StartServer();
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_latitude,m_longitude;
	CExplorer m_Explorer;
	CMiniExcel miniexcel;
	afx_msg void OnBnClickedButtonStartTcp();
	afx_msg void OnBnClickedButtonStopTcp();
	afx_msg LRESULT OnUpdateConnection(WPARAM wParam, LPARAM lParam);	
	afx_msg LRESULT OnGetNewStr(WPARAM nLen, LPARAM strText);
	afx_msg void OnDestroy();
};
