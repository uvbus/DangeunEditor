#include "stdafx.h"

void Print(HWND hChild)
{
	PRINTDLG pd;
	int nPages, nCopies;
	int nFirst, nFinal;

	memset(&pd,0,sizeof(PRINTDLG));
	pd.lStructSize=sizeof(PRINTDLG);
	pd.Flags=PD_RETURNDC | PD_NOSELECTION;
	pd.hwndOwner=g_hFrameWnd;
	pd.nFromPage=1;
	pd.nToPage=1;
	pd.nMinPage=1;
	pd.nMaxPage=65535;
	pd.nCopies=1;
	if (PrintDlg(&pd)==FALSE || pd.hDC==NULL) {
		return;
	}
	if (pd.Flags & PD_COLLATE) {
		nPages=1;
		nCopies=pd.nCopies;
	} else {
		nPages=pd.nCopies;
		nCopies=1;
	}

	if (pd.Flags & PD_PAGENUMS) {
		nFirst=pd.nFromPage;
		nFinal=pd.nToPage;
	} else {
		nFirst=pd.nMinPage;
		nFinal=pd.nMaxPage;
	}

	PrintDoc(hChild,pd.hDC,nPages,nCopies,nFirst,nFinal);

	if (pd.hDevMode) GlobalFree(pd.hDevMode);
	if (pd.hDevNames) GlobalFree(pd.hDevNames);
	DeleteDC(pd.hDC);
}

void PrintDirect(HWND hChild)
{
	HDC pdc;

	TCHAR szPrinter[80];
	TCHAR *szDevice, *szDriver, *szOutput;
	GetProfileString("windows","device",",,,",szPrinter,80);
	szDevice = strtok(szPrinter,",");
	szDriver = strtok(NULL, ", ");
	szOutput = strtok(NULL, ", ");
	pdc=CreateDC(szDriver,szDevice,szOutput,NULL);
	if (pdc) {
		PrintDoc(hChild,pdc,1,1,1,65535);
	}
	DeleteDC(pdc);
}

BOOL PrintDoc(HWND hChild, HDC pdc, int nPages, int nCopies, int nFirst, int nFinal)
{
	SInfo *pSi;
	DOCINFO doc;
	TCHAR DocName[MAX_PATH+20];
	int Result;
	int xpage, ypage;
	int dpiX, dpiY;
	int nRepeatCopy,nRepeatPage,nPage;
	BOOL bEnd;
	LOGFONT tLogFont;
	HFONT prtfont, oldfont;
	HFONT bkFont, bkOldfont;
	int BkHeight;
	UINT OldAlign;
	COLORREF OldColor;
	TCHAR *HFText;
	int HorF,HFLiney,HFTexty;
	SIZE sz;
	HPEN hPen, OldPen;
	int PenWidth;

	pSi=(SInfo *)GetWindowLongPtr(hChild,0);

	bPrinting=TRUE;
	SetAbortProc(pdc, (ABORTPROC)AbortProc);
	hDlgCancel=CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_PRINTING), 
		g_hFrameWnd, (DLGPROC)DgPrintingProc);
	SetDlgItemText(hDlgCancel,IDC_PRINTINGDOC,pSi->NowFile);
	EnableWindow(g_hFrameWnd, FALSE);

	doc.cbSize=sizeof(DOCINFO);
	wsprintf(DocName,"당근 문서 : %s",pSi->NowFile);
	doc.lpszDocName=DocName;
	doc.lpszOutput=NULL;
	doc.lpszDatatype=NULL;
	doc.fwType=0;
	if (StartDoc(pdc, &doc) <= 0) {
		return FALSE;
	}

	xpage = GetDeviceCaps(pdc, HORZRES);
	ypage = GetDeviceCaps(pdc, VERTRES);

	dpiX = GetDeviceCaps(pdc, LOGPIXELSX);
	dpiY = GetDeviceCaps(pdc, LOGPIXELSY);

	prtfont=NULL;
	HDC hdc;
	int CharRatio;
	// 시스템 글꼴이 아닐 때만 글꼴을 설정한다.
	if (Option.prtFont.lfHeight != 0) {
		// 화면 글꼴을 사용할 경우
		if (Option.prtFont.lfHeight==-1) {
			pSi->Ae.GetFont(&tLogFont);
			// 화면 글꼴이 시스템 글꼴이면 아무 것도 할 필요없다.
			if (tLogFont.lfHeight != 0) {
				// 화면 글꼴의 픽셀 단위는 포인트로 변환한다.
				if (tLogFont.lfWidth == 0) {
					CharRatio=100;
				} else {
					CharRatio=tLogFont.lfWidth*200/tLogFont.lfHeight;
				}
				hdc=GetDC(NULL);
				tLogFont.lfHeight=tLogFont.lfHeight*72/GetDeviceCaps(hdc,LOGPIXELSY);
				ReleaseDC(NULL,hdc);
			}
		} else {
			tLogFont=Option.prtFont;
		}
		if (tLogFont.lfHeight) {
			tLogFont.lfHeight=tLogFont.lfHeight*dpiY/72;
			if (CharRatio != 100) {
				tLogFont.lfWidth=tLogFont.lfHeight*CharRatio/200;
			} else {
				tLogFont.lfWidth=0;
			}
			prtfont=CreateFontIndirect(&tLogFont);
			oldfont=(HFONT)SelectObject(pdc,prtfont);
		}
	}

	if (lstrlen(Option.prtBkText)) {
		BkHeight=min(xpage,ypage)/lstrlen(Option.prtBkText);
		bkFont=CreateFont(BkHeight,0,600,0,FW_BOLD,0,0,0,HANGEUL_CHARSET,
			3,2,1,VARIABLE_PITCH | FF_ROMAN,"궁서");
	}

	Ae_PrintInfo pi;
	pi.pdc=pdc;
	pi.dpiY=dpiY;
	if (Option.prtbParse==TRUE) {
		pi.ParseID=-1;
	} else {
		pi.ParseID=0;
	}
	pi.nWrap=Option.prtnWrap;
	pi.bLineNum=Option.prtbLineNum;
	SetRect(&pi.prt,Mili(Option.prtMargin.left,dpiY),Mili(Option.prtMargin.top,dpiY),
		xpage-Mili(Option.prtMargin.right,dpiY),ypage-Mili(Option.prtMargin.bottom,dpiY));
	pSi->Ae.BeginPrint(&pi);

	for (nRepeatCopy=1;nRepeatCopy<=nCopies;nRepeatCopy++) {
	for (nPage=nFirst;nPage<=nFinal;nPage++) {
	for (nRepeatPage=1;nRepeatPage<=nPages;nRepeatPage++) {
		Result=StartPage(pdc);
		if (Result <= 0) goto failpage;

		if (Option.prtbSysHeader && prtfont!=NULL) {
			SelectObject(pdc,oldfont);
		}
		for (HorF=0;HorF<=1;HorF++) {
			if (HorF==0) {
				PenWidth=Mili((double)Option.prtnHeader/10,dpiY);
				HFLiney=pi.prt.top-PenWidth;
				GetTextExtentPoint32(pdc,"한",2,&sz);
				HFTexty=HFLiney-Mili(1,dpiY)-sz.cy;
				HFText=Option.prtHeader;
			} else {
				PenWidth=Mili((double)Option.prtnFooter/10,dpiY);
				HFLiney=pi.prt.bottom+PenWidth;
				HFTexty=HFLiney+Mili(1,dpiY);
				HFText=Option.prtFooter;
			}

			if (PenWidth) {
				hPen=CreatePen(PS_SOLID,PenWidth,RGB(0,0,0));
				OldPen=(HPEN)SelectObject(pdc,hPen);
				MoveToEx(pdc,pi.prt.left,HFLiney,NULL);
				LineTo(pdc,pi.prt.right,HFLiney);
				SelectObject(pdc,OldPen);
				DeleteObject(hPen);
			}
			PrintFormatText(HFText,&pi,pSi->NowFile,nPage,HFTexty);
		}
		if (Option.prtbSysHeader && prtfont!=NULL) {
			SelectObject(pdc,prtfont);
		}

		if (lstrlen(Option.prtBkText)) {
			bkOldfont=(HFONT)SelectObject(pdc,bkFont);
			OldColor=SetTextColor(pdc,RGB(200,200,200));
			OldAlign=SetTextAlign(pdc,TA_CENTER);
			TextOut(pdc,xpage/2,ypage/2-BkHeight/2,Option.prtBkText,lstrlen(Option.prtBkText));
			SetTextColor(pdc,OldColor);
			SetTextAlign(pdc,OldAlign);
			SelectObject(pdc,bkOldfont);
		}

		SendMessage(hDlgCancel,WM_USER+1,nPage,pi.TotalPage);
		bEnd=pSi->Ae.PrintPage(&pi,nPage);

		Result=EndPage(pdc);
		if (Result <= 0) goto failpage;
	}
	if (bEnd) break;
	}
	}

failpage:
	Result=EndDoc(pdc);
	pSi->Ae.EndPrint(&pi);
	if (prtfont) {
		SelectObject(pdc,oldfont);
		DeleteObject(prtfont);
	}
	if (lstrlen(Option.prtBkText)) {
		DeleteObject(bkFont);
	}
	if (bPrinting==TRUE) {
		EnableWindow(g_hFrameWnd, TRUE);
		DestroyWindow(hDlgCancel);
		hDlgCancel=NULL;
	}
	return TRUE;
}

LRESULT CALLBACK DgPrintingProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR Mes[128];

	switch (message) {
	case WM_INITDIALOG:
		MoveToParentCenter(hDlg);
		return TRUE;
	case WM_USER+1:
		wsprintf(Mes,"전체 %d쪽 중 %d쪽을 인쇄하고 있습니다.(%d%%)",
			lParam,wParam,wParam*100/lParam);
		SetDlgItemText(hDlg,IDC_PRINTINGMES,Mes);
		return TRUE;
	case WM_COMMAND:
		bPrinting=FALSE;
		EnableWindow(g_hFrameWnd, TRUE);
		DestroyWindow(hDlgCancel);
		hDlgCancel=NULL;
		return TRUE;
	default:
		return FALSE;
	}
}

BOOL CALLBACK AbortProc(HDC hPrtdc, int iError)
{
	MSG msg;
	while (bPrinting && PeekMessage(&msg, NULL,0,0,PM_REMOVE)) {
		if (!IsDialogMessage(hDlgCancel, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return bPrinting;
}

void TextReplace(TCHAR *Text,TCHAR *what,TCHAR *to)
{
	TCHAR *p;
	TCHAR Text2[1024];

	p=strstr(Text,what);
	while (p) {
		lstrcpyn(Text2,Text,p-Text+1);
		lstrcat(Text2,to);
		lstrcat(Text2,p+lstrlen(what));
		lstrcpy(Text,Text2);
		p=strstr(Text,what);
	}
}

void PrintFormatText(TCHAR *szFormat,Ae_PrintInfo *pi,TCHAR *Path,int nPage,int y)
{
	TCHAR Text[1024];
	TCHAR szFile[MAX_PATH];
	TCHAR szDate[32], szTime[32],szPage[12];
	SYSTEMTIME st;
	TCHAR *p1, *p2;
	int len;
	UINT OldAlign;

	if (lstrlen(szFormat)==0) {
		return;
	}

	OldAlign=GetTextAlign(pi->pdc);
	GetLocalTime(&st);
	wsprintf(szDate,"%d년 %d월 %d일",st.wYear, st.wMonth, st.wDay);
	wsprintf(szTime,"%d시 %d분",st.wHour, st.wMinute);
	_splitpath(Path,NULL,NULL,szFile,szPage);
	lstrcat(szFile,szPage);

	lstrcpy(Text,szFormat);
	TextReplace(Text,"[파일]",szFile);
	TextReplace(Text,"[경로]",Path);
	TextReplace(Text,"[날짜]",szDate);
	TextReplace(Text,"[시간]",szTime);
	wsprintf(szPage,"%d",nPage);
	TextReplace(Text,"[쪽]",szPage);
	wsprintf(szPage,"%d",pi->TotalPage);
	TextReplace(Text,"[전체쪽]",szPage);

	p1=strstr(Text,"[가운데]");
	p2=strstr(Text,"[오른쪽]");

	if (p1) {
		len=p1-Text;
	} else if (p2) {
		len=p2-Text;
	} else {
		len=lstrlen(Text);
	}
	TextOut(pi->pdc,pi->prt.left,y,Text,len);

	if (p1) {
		p1=p1+lstrlen("[가운데]");
		if (p2) {
			len=p2-p1;
		} else {
			len=lstrlen(p1);
		}
		SetTextAlign(pi->pdc,TA_CENTER);
		TextOut(pi->pdc,pi->prt.left+(pi->prt.right-pi->prt.left)/2,y,p1,len);
	}

	if (p2) {
		p2=p2+lstrlen("[오른쪽]");
		SetTextAlign(pi->pdc,TA_RIGHT);
		TextOut(pi->pdc,pi->prt.right,y,p2,lstrlen(p2));
	}
	SetTextAlign(pi->pdc,OldAlign);
}
