
// Labs-Gate-HelperDlg.h: 头文件
//

#pragma once
#include <vector>
#include <Iphlpapi.h>  
#pragma  comment(lib,"Iphlpapi.lib")

typedef struct MacIp
{
	CString Description;
	CString ip;
	CString mac;
}MacIp;
typedef std::vector<MacIp> VECTOR_DATA;

// CLabsGateHelperDlg 对话框
class CLabsGateHelperDlg : public CDialogEx
{
// 构造
public:
	CLabsGateHelperDlg(CWnd* pParent = nullptr);	// 标准构造函数
	BOOL PreTranslateMessage(MSG * pMsg);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LABSGATEHELPER_DIALOG };
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
	afx_msg void OnBnClickedOk();
};

BOOL GetLocoalIPandMac(std::vector<MacIp>& macip);

CString GetModuleName();

std::vector<CString> SplitCString(CString strSource, CString ch);

char * ToChar(CString origw);
