
// Labs-Gate-HelperDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "Labs-Gate-Helper.h"
#include "Labs-Gate-HelperDlg.h"
#include "afxdialogex.h"
#include <afxinet.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CLabsGateHelperDlg 对话框



CLabsGateHelperDlg::CLabsGateHelperDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LABSGATEHELPER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLabsGateHelperDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLabsGateHelperDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CLabsGateHelperDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CLabsGateHelperDlg 消息处理程序

CString token = _T("");
CString host = _T("");
CString port = _T("");

BOOL CLabsGateHelperDlg::OnInitDialog()
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

	VECTOR_DATA v;

	GetLocoalIPandMac(v);
	for (unsigned int x = 0; x < v.size(); x++)
	{
		SetDlgItemText(IP, v[x].ip);
		SetDlgItemText(MAC, v[x].mac);
	}

	std::vector<CString> names = SplitCString(GetModuleName(), _T("-"));
	switch (names.size())
	{
	case 4:
		port = names[3];
	case 3:
		host = names[2];
	case 2:
		token = names[1];
	default:
		SetDlgItemText(LAB, names[0]);
	}

	GetDlgItem(CODE)->SetFocus();

	return FALSE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CLabsGateHelperDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CLabsGateHelperDlg::OnPaint()
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
HCURSOR CLabsGateHelperDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CLabsGateHelperDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL GetLocoalIPandMac(std::vector<MacIp> &macip)
{
	CString myIP, myMAC;
	bool bNetReady =false;
	ULONG outBufLen = 0;
	DWORD dwRetVal = 0;
 
	PIP_ADAPTER_INFO pAadpterInfo;
	PIP_ADAPTER_INFO pAadpterInfoTmp = NULL;
 
	MacIp tmpmacip;
	
	pAadpterInfo = (IP_ADAPTER_INFO*)GlobalAlloc(GMEM_ZEROINIT,sizeof(IP_ADAPTER_INFO));
 
	if(GetAdaptersInfo(pAadpterInfo, &outBufLen) != ERROR_SUCCESS)
	{
		GlobalFree(pAadpterInfo);
		pAadpterInfo = (IP_ADAPTER_INFO*)GlobalAlloc(GMEM_ZEROINIT, outBufLen);
	}
 
	if ((dwRetVal = GetAdaptersInfo(pAadpterInfo, &outBufLen)) == NO_ERROR)
	{
		pAadpterInfoTmp = pAadpterInfo;
		myIP = "";
 
		while(pAadpterInfoTmp)
		{
			if (pAadpterInfoTmp->Type == MIB_IF_TYPE_ETHERNET)
			{
				if (pAadpterInfoTmp->CurrentIpAddress)
				{
					myIP = pAadpterInfoTmp->CurrentIpAddress->IpAddress.String;
				}
				else
				{
					myIP = pAadpterInfoTmp->IpAddressList.IpAddress.String;
				}
			}
 
			if (!myIP.IsEmpty() && (myIP.CompareNoCase(_T("0.0.0.0")) != 0))
			{
				bNetReady = true;
				myMAC.Format(_T("%02X:%02X:%02X:%02X:%02X:%02X"),
					pAadpterInfoTmp->Address[0],
					pAadpterInfoTmp->Address[1],
					pAadpterInfoTmp->Address[2],
					pAadpterInfoTmp->Address[3],
					pAadpterInfoTmp->Address[4],
					pAadpterInfoTmp->Address[5]);
				tmpmacip.ip = myIP;
				tmpmacip.mac = myMAC;
				macip.push_back(tmpmacip);
 
				break;
			}
			pAadpterInfoTmp = pAadpterInfoTmp->Next;
		}
	}
	GlobalFree(pAadpterInfo);
 
	return bNetReady;
}

CString GetModuleName()
{
	HMODULE module = GetModuleHandle(0);
	TCHAR pFileName[MAX_PATH];
	GetModuleFileName(module, pFileName, MAX_PATH);

	CString csFullPath(pFileName);
	int nPos = csFullPath.ReverseFind(_T('\\'));
	CString filename = csFullPath.Right(csFullPath.GetLength() - nPos - 1);
	return filename.Left(filename.ReverseFind('.'));
}

std::vector<CString> SplitCString(CString strSource, CString ch)
{
	std::vector<CString> vecString;
	int iPos = 0;
	CString strTmp;
	strTmp = strSource.Tokenize(ch, iPos);
	while (strTmp.Trim() != _T(""))
	{
		vecString.push_back(strTmp);
		strTmp = strSource.Tokenize(ch, iPos);
	}
	return vecString;
}

void CLabsGateHelperDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//通过 http POST 协议来发送命令给服务器
	CInternetSession session;
	session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 1000 * 20);
	session.SetOption(INTERNET_OPTION_CONNECT_BACKOFF, 1000);
	session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 1);

	INTERNET_PORT cport;

	if (port == "")
	{
		cport = (INTERNET_PORT)8080;
	}
	else
	{
		cport = (INTERNET_PORT)_ttoi(port);
	}

	CHttpConnection* pConnection = session.GetHttpConnection(host, cport);
	CHttpFile* pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST,
		TEXT("/api/v1/machine/add"),
		NULL,
		1,
		NULL,
		TEXT("HTTP/1.1"),
		INTERNET_FLAG_RELOAD);

	//需要提交的数据
	CString code;
	CString lab;
	CString ip;
	CString mac;
	CString des;
	
	GetDlgItemText(CODE, code);
	GetDlgItemText(LAB, lab);
	GetDlgItemText(IP, ip);
	GetDlgItemText(MAC, mac);
	GetDlgItemText(DES, des);

	//下面这段编码，则是可以让服务器正常处理

	pFile->AddRequestHeaders(_T("Content-Type: application/x-www-form-urlencoded;"));

	char strFormData[1000] = "code=";
	strcat(strFormData, ToChar(code));
	strcat(strFormData, "&lab=");
	strcat(strFormData, ToChar(lab));
	strcat(strFormData, "&ip=");
	strcat(strFormData, ToChar(ip));
	strcat(strFormData, "&mac=");
	strcat(strFormData, ToChar(mac));
	strcat(strFormData, "&des=");
	strcat(strFormData, ToChar(des));
	strcat(strFormData, "&token=");
	strcat(strFormData, ToChar(token));

	pFile->SendRequest(NULL,0,
		(LPVOID)strFormData,
		strlen(strFormData));

	DWORD dwRet;
	pFile->QueryInfoStatusCode(dwRet);

	CString strFile = L"";

	if (dwRet != HTTP_STATUS_OK)
	{
		CString errText;
		errText.Format(L"网络请求出错，错误码：%d", dwRet);
		AfxMessageBox(errText);
	}
	else
	{
		int len = pFile->GetLength();
		char buf[2000];
		int numread;
		while ((numread = pFile->Read(buf, sizeof(buf) - 1)) > 0)
		{
			buf[numread] = '\0';
			strFile += buf;
		}
	}

	session.Close();
	pFile->Close();
	delete pFile;

	MessageBox(strFile);

	CDialogEx::OnOK();
}

char * ToChar(CString origw) {
	const size_t newsizew = (origw.GetLength() + 1) * 2;
	char *nstringw = new char[newsizew];
	size_t convertedCharsw = 0;
	wcstombs_s(&convertedCharsw, nstringw, newsizew, origw, _TRUNCATE);
	return nstringw;
}
