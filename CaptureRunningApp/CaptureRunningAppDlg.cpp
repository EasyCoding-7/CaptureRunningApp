
// CaptureRunningAppDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "CaptureRunningApp.h"
#include "CaptureRunningAppDlg.h"
#include "afxdialogex.h"

#include "afxwin.h"
#include <vector>
#include <string>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static vector<wstring> gs_blackList;

size_t wchar_to_utf8(const wchar_t *in, size_t insize, char *out,
	size_t outsize, int flags)
{
	int i_insize = (int)insize;
	int ret;

	if (i_insize == 0)
		i_insize = (int)wcslen(in);

	ret = WideCharToMultiByte(CP_UTF8, 0, in, i_insize, out, (int)outsize,
		NULL, NULL);

	//UNUSED_PARAMETER(flags);
	return (ret > 0) ? (size_t)ret : 0;
}

size_t os_wcs_to_utf8(const wchar_t *str, size_t len, char *dst,
	size_t dst_size)
{
	size_t in_len;
	size_t out_len;

	if (!str)
		return 0;

	in_len = (len != 0) ? len : wcslen(str);
	out_len = dst ? (dst_size - 1) : wchar_to_utf8(str, in_len, NULL, 0, 0);

	if (dst) {
		if (!dst_size)
			return 0;

		if (out_len)
			out_len =
			wchar_to_utf8(str, in_len, dst, out_len + 1, 0);

		dst[out_len] = 0;
	}

	return out_len;
}

static bool WindowValid(HWND window)
{
	LONG_PTR styles, ex_styles;
	RECT rect;
	DWORD id;

	// check UWP program
	TCHAR buffer_class[256];	
	::GetClassName(window, buffer_class, 255);
	TCHAR buffer_title[256];
	::GetWindowTextW(window, buffer_title, 255);

	char c_szText[256];
	wcstombs(c_szText, buffer_class, wcslen(buffer_class) + 1);

	if (strcmp(c_szText, "Windows.UI.Core.CoreWindow") == 0) {
		gs_blackList.push_back((wstring)buffer_title);
		return false;
	}
		
	// check black list
	for (auto& s : gs_blackList) {
		if (s == (wstring)buffer_title)
			return false;
	}

	if (!IsWindowVisible(window))
		return false;
	GetWindowThreadProcessId(window, &id);
	if (id == GetCurrentProcessId())
		return false;

	GetClientRect(window, &rect);
	styles = GetWindowLongPtr(window, GWL_STYLE);
	ex_styles = GetWindowLongPtr(window, GWL_EXSTYLE);

	if (ex_styles & WS_EX_TOOLWINDOW)
		return false;
	if (styles & WS_CHILD)
		return false;

	if ((rect.right == rect.left) || (rect.bottom == rect.top))
		return false;

	return true;
}

static bool GetWindowTitle(HWND window, string &title)
{
	size_t len = (size_t)GetWindowTextLengthW(window);
	wstring wtitle;

	wtitle.resize(len);
	if (!GetWindowTextW(window, &wtitle[0], (int)len + 1))
		return false;

	len = os_wcs_to_utf8(wtitle.c_str(), 0, nullptr, 0);
	title.resize(len);
	os_wcs_to_utf8(wtitle.c_str(), 0, &title[0], len + 1);
	return true;
}

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CCaptureRunningAppDlg 대화 상자



CCaptureRunningAppDlg::CCaptureRunningAppDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CAPTURERUNNINGAPP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCaptureRunningAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CAPTURE_LIST, m_captureList);
	DDX_Control(pDX, IDC_PICTUREC, m_PicControl);
}

BEGIN_MESSAGE_MAP(CCaptureRunningAppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_CAPTURE, &CCaptureRunningAppDlg::OnBnClickedCapture)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CAPTURE_LIST, &CCaptureRunningAppDlg::OnLvnItemchangedCaptureList)
END_MESSAGE_MAP()


// CCaptureRunningAppDlg 메시지 처리기

BOOL CCaptureRunningAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	m_captureList.InsertColumn(0, _T("Running Apps Title"), LVCFMT_LEFT, 600);
	m_captureList.InsertColumn(1, _T("Class"), LVCFMT_LEFT, 400);
	m_captureList.InsertColumn(2, _T("HWND"), LVCFMT_LEFT, 200);


	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CCaptureRunningAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CCaptureRunningAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CCaptureRunningAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCaptureRunningAppDlg::OnBnClickedCapture()
{
	 CWnd * pWnd = GetForegroundWindow();
	 HWND window = pWnd->m_hWnd;
	 int i = 0;

	 while (window) {
		 string title;
		 if (WindowValid(window) && GetWindowTitle(window, title)) {
			 TCHAR buffer_title[256];
			 TCHAR buffer_class[256];
			 ::GetWindowTextW(window, buffer_title, 255);
			 ::GetClassName(window, buffer_class, 255);

			 m_captureList.InsertItem(i, buffer_title);
			 m_captureList.SetItem(i, 1, LVIF_TEXT, buffer_class, 0, 0, 0, NULL);

			 CString str;
			 str.Format(_T("%x"), window);
			 m_captureList.SetItem(i++, 2, LVIF_TEXT, str, 0, 0, 0, NULL);
		 }
		 window = ::GetNextWindow(window, GW_HWNDNEXT);
	 }
}


void CCaptureRunningAppDlg::OnLvnItemchangedCaptureList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	// Erase image
	CDC* p = m_PicControl.GetWindowDC();
	OnEraseBkgnd(p);

	CRect rt;
	GetClientRect(&rt);  // 클라이언트 영역의 크기 계산
	p->FillSolidRect(&rt, RGB(255, 255, 255));

	// get List Ctrl index
	POSITION pos;
	pos = m_captureList.GetFirstSelectedItemPosition();
	int idx = m_captureList.GetNextSelectedItem(pos);

	// get HWND
	CString cs_window = m_captureList.GetItemText(idx, 2);
	CT2CA pszConvertedAnsiString(cs_window);
	std::string s_window(pszConvertedAnsiString);

	int n_window = (int)strtol(s_window.c_str(), NULL, 16);
	HWND window = (HWND)n_window;

	// draw capture
	HDC hdc_target;
	HDC hdc = m_PicControl.GetWindowDC()->m_hDC;

	hdc_target = ::GetDC(window);

	RECT rect;
	::GetWindowRect(window, &rect);
	int captured_width = rect.right - rect.left;
	int captured_height = rect.bottom - rect.top;

	CRect crect;
	RECT rect2;
	m_PicControl.GetClientRect(crect);
	::GetWindowRect(m_PicControl, &rect2);
	ScreenToClient(&rect2);

	CDC* dc = GetDC();
	// BitBlt(hdc, 0, 0, captured_width, captured_height, hdc_target, 0, 0, SRCCOPY);
	dc->StretchBlt(rect2.left, rect2.top, crect.Width(), crect.Height(), CDC::FromHandle(hdc_target), 0, 0, captured_width, captured_height, SRCCOPY);

	::ReleaseDC(NULL, hdc_target);
}
