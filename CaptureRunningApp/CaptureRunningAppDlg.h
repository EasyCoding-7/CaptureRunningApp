
// CaptureRunningAppDlg.h: 헤더 파일
//

#pragma once


// CCaptureRunningAppDlg 대화 상자
class CCaptureRunningAppDlg : public CDialogEx
{
// 생성입니다.
public:
	CCaptureRunningAppDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAPTURERUNNINGAPP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCapture();
	CListCtrl m_captureList;
	afx_msg void OnLvnItemchangedCaptureList(NMHDR *pNMHDR, LRESULT *pResult);
	CStatic m_PicControl;
	afx_msg void OnBnClickedWinverBtn();
	CButton m_checkSavePNG;
	afx_msg void OnBnClickedCapturePro();
};
