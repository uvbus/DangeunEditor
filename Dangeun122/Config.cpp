#include "stdafx.h"

arColor arPreColor[]={
	{"기본색",RGB(0,0,0)}, 
	{"사용자 선택색",RGB(0,0,0)}, 
	{"사용자 선택...",RGB(0,0,0)}, 
	{"검정",RGB(0,0,0)},
	{"흰색",RGB(255,255,255)},
	{"빨강",RGB(255,0,0)},
	{"초록",RGB(0,255,0)},
	{"파랑",RGB(0,0,255)},
	{"노랑",RGB(255,255,0)},
	{"분홍",RGB(255,0,255)},
	{"하늘색",RGB(0,255,255)},
	{"고동색",RGB(128,0,0)},
	{"진초록",RGB(0,128,0)},
	{"남색",RGB(0,0,128)},
	{"보라색",RGB(180,84,233)},
	{"회색1(32)",RGB(32,32,32)},
	{"회색2(64)",RGB(64,64,64)},
	{"회색3(96)",RGB(96,96,96)},
	{"회색4(128)",RGB(128,128,128)},
	{"회색5(160)",RGB(160,160,160)},
	{"회색6(192)",RGB(192,192,192)},
	{"회색7(220)",RGB(220,220,220)},
	{"연노랑",RGB(255,249,157)},
	{"개나리색",RGB(255,209,87)},
	{"황토색",RGB(207,182,80)},
	{"연파랑",RGB(141,207,244)},
	{"감색",RGB(177,202,147)},
	{"흐린분홍",RGB(182,99,105)},
	{"연두색",RGB(169,212,109)}
};

COLORREF arSysColor[AECOLORS]={RGB(192,192,192),RGB(160,160,160),RGB(0,0,0),
	RGB(255,255,0),RGB(128,128,128),RGB(255,255,0),
	0,0,0,0,0,RGB(220,220,220),RGB(0,0,0)};

void Config()
{
	PROPSHEETPAGE psp[11];
	PROPSHEETHEADER psh;

	NewOption=Option;
	NewOption.bModified=TRUE;

	psp[0].dwSize=sizeof(PROPSHEETPAGE);
	psp[0].dwFlags=PSP_DEFAULT;
	psp[0].hInstance=g_hInst;
	psp[0].pszTemplate=MAKEINTRESOURCE(IDD_GENERAL);
	psp[0].pfnDlgProc=(DLGPROC)GeneralDlgProc;
	psp[0].lParam=0;

	psp[1].dwSize=sizeof(PROPSHEETPAGE);
	psp[1].dwFlags=PSP_DEFAULT;
	psp[1].hInstance=g_hInst;
	psp[1].pszTemplate=MAKEINTRESOURCE(IDD_VIEW);
	psp[1].pfnDlgProc=(DLGPROC)ViewDlgProc;
	psp[1].lParam=0;

	psp[2].dwSize=sizeof(PROPSHEETPAGE);
	psp[2].dwFlags=PSP_DEFAULT;
	psp[2].hInstance=g_hInst;
	psp[2].pszTemplate=MAKEINTRESOURCE(IDD_SHAPE);
	psp[2].pfnDlgProc=(DLGPROC)ShapeDlgProc;
	psp[2].lParam=0;

	psp[3].dwSize=sizeof(PROPSHEETPAGE);
	psp[3].dwFlags=PSP_DEFAULT;
	psp[3].hInstance=g_hInst;
	psp[3].pszTemplate=MAKEINTRESOURCE(IDD_TOOLBAR);
	psp[3].pfnDlgProc=(DLGPROC)ToolBarDlgProc;
	psp[3].lParam=0;

	psp[4].dwSize=sizeof(PROPSHEETPAGE);
	psp[4].dwFlags=PSP_DEFAULT;
	psp[4].hInstance=g_hInst;
	psp[4].pszTemplate=MAKEINTRESOURCE(IDD_COLOR);
	psp[4].pfnDlgProc=(DLGPROC)ColorDlgProc;
	psp[4].lParam=0;

	psp[5].dwSize=sizeof(PROPSHEETPAGE);
	psp[5].dwFlags=PSP_DEFAULT;
	psp[5].hInstance=g_hInst;
	psp[5].pszTemplate=MAKEINTRESOURCE(IDD_EDIT);
	psp[5].pfnDlgProc=(DLGPROC)EditDlgProc;
	psp[5].lParam=0;

	psp[6].dwSize=sizeof(PROPSHEETPAGE);
	psp[6].dwFlags=PSP_DEFAULT;
	psp[6].hInstance=g_hInst;
	psp[6].pszTemplate=MAKEINTRESOURCE(IDD_SYNTAX);
	psp[6].pfnDlgProc=(DLGPROC)SyntaxDlgProc;
	psp[6].lParam=0;

	psp[7].dwSize=sizeof(PROPSHEETPAGE);
	psp[7].dwFlags=PSP_DEFAULT;
	psp[7].hInstance=g_hInst;
	psp[7].pszTemplate=MAKEINTRESOURCE(IDD_PRINT);
	psp[7].pfnDlgProc=(DLGPROC)PrintDlgProc;
	psp[7].lParam=0;

	psp[8].dwSize=sizeof(PROPSHEETPAGE);
	psp[8].dwFlags=PSP_DEFAULT;
	psp[8].hInstance=g_hInst;
	psp[8].pszTemplate=MAKEINTRESOURCE(IDD_FILE);
	psp[8].pfnDlgProc=(DLGPROC)FileDlgProc;
	psp[8].lParam=0;

	psp[9].dwSize=sizeof(PROPSHEETPAGE);
	psp[9].dwFlags=PSP_DEFAULT;
	psp[9].hInstance=g_hInst;
	psp[9].pszTemplate=MAKEINTRESOURCE(IDD_ASSOCIATE);
	psp[9].pfnDlgProc=(DLGPROC)AssociateDlgProc;
	psp[9].lParam=0;

	psp[10].dwSize=sizeof(PROPSHEETPAGE);
	psp[10].dwFlags=PSP_DEFAULT;
	psp[10].hInstance=g_hInst;
	psp[10].pszTemplate=MAKEINTRESOURCE(IDD_OPERATION);
	psp[10].pfnDlgProc=(DLGPROC)OperationDlgProc;
	psp[10].lParam=0;

	psh.dwSize=sizeof(PROPSHEETHEADER);
	psh.dwFlags=PSH_PROPSHEETPAGE | PSH_USECALLBACK;
	psh.hwndParent=g_hFrameWnd;
	psh.pszCaption="설정";
	psh.nPages=sizeof(psp)/sizeof(psp[0]);
	psh.nStartPage=g_StartPage;
	psh.ppsp=(LPCPROPSHEETPAGE)&psp;
	psh.pfnCallback=ConfigProc;

	if (arFont==NULL) {
		DgEnumFonts();
	}

	InitSysColor();
	PropertySheet(&psh);

	return;
}

#include <pshpack1.h>
typedef struct DLGTEMPLATEEX
{
    WORD dlgVer;
    WORD signature;
    DWORD helpID;
    DWORD exStyle;
    DWORD style;
    WORD cDlgItems;
    short x;
    short y;
    short cx;
    short cy;
} DLGTEMPLATEEX, *LPDLGTEMPLATEEX;
#include <poppack.h>

int CALLBACK ConfigProc(HWND hwndDlg, UINT uMsg, LPARAM lParam)
{
	switch (uMsg) {
	case PSCB_PRECREATE:
		if (((DLGTEMPLATEEX *)lParam)->signature==0xffff) {
			((DLGTEMPLATEEX *)lParam)->style &= ~DS_CONTEXTHELP;
		} else {
			((DLGTEMPLATE *)lParam)->style &= ~DS_CONTEXTHELP;
		}
		return TRUE;
	}
	return TRUE;
}

void DgEnumFonts()
{
	HDC hdc;

	hdc=GetDC(NULL);
	if (arFont==NULL) {
		arFont=(LOGFONT *)malloc(sizeof(LOGFONT)*500);
	}
	FontNum=0;
	EnumFontFamilies(hdc,NULL,(FONTENUMPROC)EnumFamCallBack,(LPARAM)NULL);
	ReleaseDC(NULL,hdc);
}

int CALLBACK EnumFamCallBack(ENUMLOGFONT FAR *lpelf, NEWTEXTMETRIC FAR *lpntm, 
							 int FontType, LPARAM lParam)
{
	if (FontNum < 500) {
		if (lpelf->elfLogFont.lfFaceName[0]!='@'/* && FontType==TRUETYPE_FONTTYPE*/) {
			arFont[FontNum] = lpelf->elfLogFont;
			FontNum++;
		}
		return TRUE;
	}
	else {
		return FALSE;
	}
}

BOOL bEditByCode=FALSE;
BOOL CALLBACK GeneralDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HWND hCon;

	switch(iMessage) {
	case WM_INITDIALOG:
		hCon=GetDlgItem(hDlg,IDC_MAXMRU);
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"5개");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"10개");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"15개");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"20개");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"25개");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"30개");

		SendDlgItemMessage(hDlg,IDC_DEFEXT,EM_LIMITTEXT,11,0);
		return TRUE;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code) {
		case PSN_SETACTIVE:
label_reload:
			g_StartPage=0;
			switch (NewOption.StartAction) {
			case 0:
				CheckRadioButton(hDlg,IDC_START1,IDC_START4,IDC_START1);
				break;
			case 1:
				CheckRadioButton(hDlg,IDC_START1,IDC_START4,IDC_START2);
				break;
			case 2:
				CheckRadioButton(hDlg,IDC_START1,IDC_START4,IDC_START3);
				break;
			case 3:
				CheckRadioButton(hDlg,IDC_START1,IDC_START4,IDC_START4);
				break;
			}

			CheckDlgButton(hDlg,IDC_EXPLORERPOPUP,NewOption.bExplorerPopup);
			CheckDlgButton(hDlg,IDC_MAXFIRSTCHILD,NewOption.bMaxFirstChild);
			CheckDlgButton(hDlg,IDC_ALLOWMULTI,NewOption.bAllowMulti);
			CheckDlgButton(hDlg,IDC_WATCHCHANGE,NewOption.bWatchChange);
			CheckDlgButton(hDlg,IDC_RELOADNOASK,NewOption.bReloadNoAsk);
			CheckDlgButton(hDlg,IDC_RELOADPROJECT,NewOption.bReloadProject);
			CheckDlgButton(hDlg,IDC_RELOADCLIP,NewOption.bReloadClip);

			SendDlgItemMessage(hDlg,IDC_MAXMRU,CB_SETCURSEL,NewOption.MaxMru,0);
			bEditByCode=TRUE;
			SetDlgItemText(hDlg,IDC_DEFEXT,NewOption.DefExt);
			bEditByCode=FALSE;
			AdjustDlgUI(0,hDlg);
			return TRUE;
		case PSN_APPLY:
			ApplyNow();
			return TRUE;
		case PSN_KILLACTIVE:
			if (IsDlgButtonChecked(hDlg, IDC_START1))
				NewOption.StartAction=0;
			if (IsDlgButtonChecked(hDlg, IDC_START2))
				NewOption.StartAction=1;
			if (IsDlgButtonChecked(hDlg, IDC_START3))
				NewOption.StartAction=2;
			if (IsDlgButtonChecked(hDlg, IDC_START4))
				NewOption.StartAction=3;

			NewOption.bExplorerPopup=IsDlgButtonChecked(hDlg, IDC_EXPLORERPOPUP);
			NewOption.bMaxFirstChild=IsDlgButtonChecked(hDlg, IDC_MAXFIRSTCHILD);
			NewOption.bAllowMulti=IsDlgButtonChecked(hDlg, IDC_ALLOWMULTI);
			NewOption.bWatchChange=IsDlgButtonChecked(hDlg, IDC_WATCHCHANGE);
			NewOption.bReloadNoAsk=IsDlgButtonChecked(hDlg, IDC_RELOADNOASK);
			NewOption.bReloadProject=IsDlgButtonChecked(hDlg, IDC_RELOADPROJECT);
			NewOption.bReloadClip=IsDlgButtonChecked(hDlg,IDC_RELOADCLIP);

			NewOption.MaxMru=SendDlgItemMessage(hDlg,IDC_MAXMRU,CB_GETCURSEL,0,0);
			GetDlgItemText(hDlg,IDC_DEFEXT,NewOption.DefExt,12);
			return TRUE;
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_WATCHCHANGE:
			AdjustDlgUI(0,hDlg);
		case IDC_START1:
		case IDC_START2:
		case IDC_START3:
		case IDC_START4:
		case IDC_EXPLORERPOPUP:
		case IDC_MAXFIRSTCHILD:
		case IDC_ALLOWMULTI:
		case IDC_RELOADNOASK:
		case IDC_RELOADPROJECT:
		case IDC_RELOADCLIP:
			PropSheet_Changed(GetParent(hDlg),hDlg);
			return TRUE;
		case IDC_MAXMRU:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				PropSheet_Changed(GetParent(hDlg),hDlg);
				break;
			}
			return TRUE;
		case IDC_DEFEXT:
			switch (HIWORD(wParam)) {
			case EN_CHANGE:
				if (bEditByCode==FALSE) {
					PropSheet_Changed(GetParent(hDlg),hDlg);
				}
				break;
			}
			return TRUE;
		case IDC_BTNDEFAULT:
			if (MessageBox(hDlg,"모든 설정 상태를 처음 설치 상태로 초기화하시겠습니까?",
				"알림",MB_YESNO)==IDYES) {
				NewOption.Init();
				NewOption.GetStyleColor();
				PropSheet_Changed(GetParent(hDlg),hDlg);
				goto label_reload;
			}
			return TRUE;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK ViewDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HWND hCon;
	int i,idx;
	int tint;
	TCHAR szFace[32];
	LPMEASUREITEMSTRUCT lpmis;
	LPDRAWITEMSTRUCT lpdis;
	RECT rt;
	HBRUSH hBrush,OldBrush;
	int c1,c2,c3;

	switch(iMessage) {
	case WM_INITDIALOG:
		hCon=GetDlgItem(hDlg,IDC_LINEHEIGHT);
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"100");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"120");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"150");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"200");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"250");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"300");

		hCon=GetDlgItem(hDlg,IDC_FONTSIZE);
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"8");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"9");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"10");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"11");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"12");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"15");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"20");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"30");

		hCon=GetDlgItem(hDlg,IDC_CARETWIDTH);
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"1");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"2");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"3");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"4");

		hCon=GetDlgItem(hDlg,IDC_SEL);
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"숨김");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"흐리게 표시");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"항상 표시");

		hCon=GetDlgItem(hDlg,IDC_CUR);
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"강조하지 않음");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"박스로 표시");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"밑줄로 표시");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"음영으로 표시");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"음영과 박스로 표시");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"마진에 표시");

		hCon=GetDlgItem(hDlg,IDC_WRAP);
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"한글 글자, 영문 글자");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"한글 글자, 영문 단어");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"한글 단어, 영문 단어");

		hCon=GetDlgItem(hDlg,IDC_CBBPL);
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"4");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"8");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"16");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"24");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"32");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"48");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"64");

		hCon=GetDlgItem(hDlg,IDC_CBHEXSEP);
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"2");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"4");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"8");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"16");

		SendDlgItemMessage(hDlg,IDC_FONTFACE,CB_ADDSTRING,0,(LPARAM)"==시스템==");
		for (i=0;i<FontNum;i++) {
			SendDlgItemMessage(hDlg,IDC_FONTFACE,CB_ADDSTRING,0,(LPARAM)arFont[i].lfFaceName);
		}

		SendDlgItemMessage(hDlg,IDC_SPINCHAREXTRA,UDM_SETRANGE,0,MAKELPARAM(300,50));
		SendDlgItemMessage(hDlg,IDC_SPINWORDEXTRA,UDM_SETRANGE,0,MAKELPARAM(400,20));
		SendDlgItemMessage(hDlg,IDC_SPINCHARRATIO,UDM_SETRANGE,0,MAKELPARAM(400,10));
		return TRUE;
	case WM_MEASUREITEM:
		lpmis=(LPMEASUREITEMSTRUCT)lParam;
		if (wParam==IDC_SEL) {
			lpmis->itemHeight=14;
		} else {
			lpmis->itemHeight=17;
		}
		return TRUE;
	case WM_DRAWITEM:
		lpdis=(LPDRAWITEMSTRUCT)lParam;
		rt=lpdis->rcItem;
		if (lpdis->itemState & ODS_SELECTED) {
			hBrush=CreateSolidBrush(RGB(0,0,255));
			OldBrush=(HBRUSH)SelectObject(lpdis->hDC,hBrush);
			Ellipse(lpdis->hDC,rt.left+1,rt.top+2,rt.left+12,rt.top+13);
			DeleteObject(SelectObject(lpdis->hDC,OldBrush));
		} else {
			FillRect(lpdis->hDC,&rt,GetSysColorBrush(COLOR_WINDOW));
		}
		if (wParam==IDC_SEL) {
			DrawBitmapById(lpdis->hDC,rt.left+15,rt.top+1,IDB_SEL1+lpdis->itemID);
		} else {
			DrawBitmapById(lpdis->hDC,rt.left+15,rt.top+1,IDB_CURLINE1+lpdis->itemID);
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_FONTFACE:
			AdjustDlgUI(1,hDlg);
		case IDC_FONTSIZE:
		case IDC_LINEHEIGHT:
		case IDC_CARETWIDTH:
		case IDC_SEL:
		case IDC_CUR:
		case IDC_WRAP:
		case IDC_CBBPL:
		case IDC_CBHEXSEP:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
			case CBN_EDITCHANGE:
				PropSheet_Changed(GetParent(hDlg),hDlg);
				break;
			}
			return TRUE;
		case IDC_HIDECURLINE:
		case IDC_NOFIRSTSPACE:
		case IDC_CHKCAPITAL:
		case IDC_CHKBOLD:
		case IDC_CHKITALIC:
			PropSheet_Changed(GetParent(hDlg),hDlg);
			return TRUE;
		case IDC_RIGHTWRAP:
		case IDC_COLMARK:
		case IDC_EDCHAREXTRA:
		case IDC_EDWORDEXTRA:
		case IDC_EDCHARRATIO:
			if (HIWORD(wParam) == EN_CHANGE) {
				if (bEditByCode==FALSE) {
					PropSheet_Changed(GetParent(hDlg),hDlg);
				}
			}
			return TRUE;
		}
		break;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code) {
		case PSN_SETACTIVE:
			g_StartPage=1;
			bEditByCode=TRUE;
			SetDlgItemInt(hDlg,IDC_LINEHEIGHT,NewOption.LineRatio,FALSE);

			CheckDlgButton(hDlg,IDC_HIDECURLINE,NewOption.TestStyle(AES_HIDECURLINE));
			CheckDlgButton(hDlg,IDC_NOFIRSTSPACE,NewOption.TestStyle(AES_NOFIRSTSPACE));
			CheckDlgButton(hDlg,IDC_CHKCAPITAL,NewOption.bCapital);

			if (NewOption.logfont.lfHeight == 0) {
				idx=0;
				SetDlgItemInt(hDlg,IDC_FONTSIZE,10,FALSE);
				SetDlgItemInt(hDlg,IDC_EDCHARRATIO,100,FALSE);
				EnableWindow(GetDlgItem(hDlg,IDC_FONTSIZE),FALSE);
			} else {
				idx=SendDlgItemMessage(hDlg,IDC_FONTFACE,CB_FINDSTRING,-1,
					(LPARAM)NewOption.logfont.lfFaceName);
				SetDlgItemInt(hDlg,IDC_FONTSIZE,NewOption.logfont.lfHeight,FALSE);
				SetDlgItemInt(hDlg,IDC_EDCHARRATIO,NewOption.CharRatio,FALSE);
				CheckDlgButton(hDlg,IDC_CHKBOLD,NewOption.logfont.lfWeight==FW_BOLD);
				CheckDlgButton(hDlg,IDC_CHKITALIC,NewOption.logfont.lfItalic);
			}
			SendDlgItemMessage(hDlg,IDC_FONTFACE,CB_SETCURSEL,idx,0);

			SendDlgItemMessage(hDlg,IDC_SEL,CB_SETCURSEL,NewOption.HideSelType,0);
			SendDlgItemMessage(hDlg,IDC_CUR,CB_SETCURSEL,NewOption.nShowCurLine,0);
			SendDlgItemMessage(hDlg,IDC_WRAP,CB_SETCURSEL,NewOption.nWrap-1,0);
			SendDlgItemMessage(hDlg,IDC_CARETWIDTH,CB_SETCURSEL,NewOption.CaretWidth-1,0);
			SendDlgItemMessage(hDlg,IDC_CBBPL,CB_SETCURSEL,NewOption.bpl,0);
			SendDlgItemMessage(hDlg,IDC_CBHEXSEP,CB_SETCURSEL,NewOption.nHexSep,0);

			SetDlgItemInt(hDlg,IDC_RIGHTWRAP,NewOption.RightWrap,FALSE);
			SetDlgItemInt(hDlg,IDC_COLMARK,NewOption.ColMark,FALSE);
			SetDlgItemInt(hDlg,IDC_EDCHAREXTRA,NewOption.CharExtra,FALSE);
			SetDlgItemInt(hDlg,IDC_EDWORDEXTRA,NewOption.WordExtra,FALSE);
			bEditByCode=FALSE;
			AdjustDlgUI(1,hDlg);
			return TRUE;
		case PSN_APPLY:
			ApplyNow();
			return TRUE;
		case PSN_KILLACTIVE:
			tint=GetDlgItemInt(hDlg,IDC_LINEHEIGHT,NULL,FALSE);
			if (tint < 100 || tint > 1000) {
				MessageBox(hDlg,"줄간은 100~1000사이의 값을 지정해야 합니다","알림",MB_OK);
				SetWindowLongPtr(hDlg,DWLP_MSGRESULT,TRUE);
				return TRUE;
			}

			tint=GetDlgItemInt(hDlg,IDC_FONTSIZE,NULL,FALSE);
			if (tint < 6 || tint > 72) {
				MessageBox(hDlg,"글꼴의 크기는 6~72사이의 값을 지정해야 합니다","알림",MB_OK);
				SetWindowLongPtr(hDlg,DWLP_MSGRESULT,TRUE);
				return TRUE;
			}

			NewOption.LineRatio=GetDlgItemInt(hDlg,IDC_LINEHEIGHT,NULL,FALSE);
			NewOption.ChangeStyle(AES_HIDECURLINE,IsDlgButtonChecked(hDlg,IDC_HIDECURLINE));
			NewOption.ChangeStyle(AES_NOFIRSTSPACE,IsDlgButtonChecked(hDlg,IDC_NOFIRSTSPACE));
			NewOption.bCapital=IsDlgButtonChecked(hDlg,IDC_CHKCAPITAL);

			c1=GetDlgItemInt(hDlg,IDC_EDCHAREXTRA,NULL,FALSE);
			c2=GetDlgItemInt(hDlg,IDC_EDWORDEXTRA,NULL,FALSE);
			c3=GetDlgItemInt(hDlg,IDC_EDCHARRATIO,NULL,FALSE);
			if (c1 < 50 || c1 > 300) {
				MessageBox(hDlg,"자간은 50~300사이여야 합니다.","알림",MB_OK);
				SetWindowLong(hDlg,DWL_MSGRESULT,TRUE);
				return TRUE;
			}
			if (c2 < 20 || c2 > 400) {
				MessageBox(hDlg,"단어간격은 20~400사이여야 합니다.","알림",MB_OK);
				SetWindowLong(hDlg,DWL_MSGRESULT,TRUE);
				return TRUE;
			}
			if (c3 < 10 || c3 > 400) {
				MessageBox(hDlg,"장평은 10~400사이여야 합니다.","알림",MB_OK);
				SetWindowLong(hDlg,DWL_MSGRESULT,TRUE);
				return TRUE;
			}

			idx=SendDlgItemMessage(hDlg,IDC_FONTFACE,CB_GETCURSEL,0,0);
			if (idx==0) {
				NewOption.logfont.lfHeight=0;
			} else {
				SendDlgItemMessage(hDlg,IDC_FONTFACE,CB_GETLBTEXT,idx,(LPARAM)szFace);
				for (i=0;i<FontNum;i++) {
					if (lstrcmp(szFace,arFont[i].lfFaceName) == 0)
						break;
				}
				NewOption.logfont=arFont[i];
				NewOption.logfont.lfHeight=GetDlgItemInt(hDlg,IDC_FONTSIZE,NULL,FALSE);
				NewOption.logfont.lfWeight=IsDlgButtonChecked(hDlg,IDC_CHKBOLD) ?
					 FW_BOLD:FW_NORMAL;
				NewOption.logfont.lfItalic=IsDlgButtonChecked(hDlg,IDC_CHKITALIC);
			}

			NewOption.CharExtra=c1;
			NewOption.WordExtra=c2;
			NewOption.CharRatio=c3;

			NewOption.HideSelType=SendDlgItemMessage(hDlg,IDC_SEL,CB_GETCURSEL,0,0);
			NewOption.nShowCurLine=SendDlgItemMessage(hDlg,IDC_CUR,CB_GETCURSEL,0,0);
			NewOption.nWrap=SendDlgItemMessage(hDlg,IDC_WRAP,CB_GETCURSEL,0,0)+1;
			NewOption.CaretWidth=SendDlgItemMessage(hDlg,IDC_CARETWIDTH,CB_GETCURSEL,0,0)+1;
			NewOption.bpl=SendDlgItemMessage(hDlg,IDC_CBBPL,CB_GETCURSEL,0,0);
			NewOption.nHexSep=SendDlgItemMessage(hDlg,IDC_CBHEXSEP,CB_GETCURSEL,0,0);
			NewOption.RightWrap=GetDlgItemInt(hDlg,IDC_RIGHTWRAP,NULL,FALSE);
			NewOption.ColMark=GetDlgItemInt(hDlg,IDC_COLMARK,NULL,FALSE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK ShapeDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HWND hCon;
	int t1,t2,t3,t4;
	OPENFILENAME OFN;
	TCHAR lpstrFile[MAX_PATH]="";
	TCHAR Sample[128];

	switch(iMessage) {
	case WM_INITDIALOG:
		hCon=GetDlgItem(hDlg,IDC_CBBACKBITPOS);
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"왼쪽 위");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"오른쪽 위");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"왼쪽 아래");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"오른쪽 아래");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"중앙");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"반복");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"반복 스크롤");

		hCon=GetDlgItem(hDlg,IDC_CBSELBLOCKTYPE);
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"문자 높이");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"줄간 높이");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"외곽선");

		hCon=GetDlgItem(hDlg,IDC_CBVALIGN);
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"위쪽");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"가운데");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"아래쪽");

		SendDlgItemMessage(hDlg,IDC_FRTLEFTSPIN,UDM_SETRANGE,0,MAKELPARAM(32,0));
		SendDlgItemMessage(hDlg,IDC_FRTTOPSPIN,UDM_SETRANGE,0,MAKELPARAM(32,0));
		SendDlgItemMessage(hDlg,IDC_FRTRIGHTSPIN,UDM_SETRANGE,0,MAKELPARAM(32,0));
		SendDlgItemMessage(hDlg,IDC_FRTBOTTOMSPIN,UDM_SETRANGE,0,MAKELPARAM(32,0));

		hCon=GetDlgItem(hDlg,IDC_CBSAMPLEBITMAP);
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"국화");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"콩");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"아가");
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_MARGIN:
			AdjustDlgUI(2,hDlg);
		case IDC_LINENUM:
		case IDC_SHOWENTER:
		case IDC_SHOWTAB:
		case IDC_SHOWSPACE:
		case IDC_SHOWRULER:
			PropSheet_Changed(GetParent(hDlg),hDlg);
			return TRUE;
		case IDC_CBBACKBITPOS:
		case IDC_CBSELBLOCKTYPE:
		case IDC_CBVALIGN:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
			case CBN_EDITCHANGE:
				PropSheet_Changed(GetParent(hDlg),hDlg);
				return TRUE;
			}
			break;
		case IDC_FRTLEFT:
		case IDC_FRTTOP:
		case IDC_FRTRIGHT:
		case IDC_FRTBOTTOM:
			switch (HIWORD(wParam)) {
			case EN_CHANGE:
				if (bEditByCode==FALSE) {
					PropSheet_Changed(GetParent(hDlg),hDlg);
					return TRUE;
				}
				break;
			}
			break;
		case IDC_BTNBRBACKBIT:
			memset(&OFN, 0, sizeof(OPENFILENAME));
			OFN.lStructSize = sizeof(OPENFILENAME);
			OFN.hwndOwner=hDlg;
			OFN.lpstrFilter="그림 파일(*.bmp)\0*.bmp\0";
			OFN.lpstrFile=lpstrFile;
			OFN.nMaxFile=MAX_PATH;
			if (GetOpenFileName(&OFN)) {
				MakeRelativePath(lpstrFile);
				SetDlgItemText(hDlg,IDC_EDBACKBIT,lpstrFile);
				PropSheet_Changed(GetParent(hDlg),hDlg);
			}
			return TRUE;
		case IDC_CBSAMPLEBITMAP:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				t1=SendDlgItemMessage(hDlg,IDC_CBSAMPLEBITMAP,CB_GETCURSEL,0,0);
				SendDlgItemMessage(hDlg,IDC_CBSAMPLEBITMAP,CB_GETLBTEXT,t1,(LPARAM)Sample);
				wsprintf(lpstrFile,"샘플:%s",Sample);
				SetDlgItemText(hDlg,IDC_EDBACKBIT,lpstrFile);
				PropSheet_Changed(GetParent(hDlg),hDlg);
				break;
			}
			return TRUE;
		case IDC_BTNREMOVEBACKBIT:
			SetDlgItemText(hDlg,IDC_EDBACKBIT,"");
			PropSheet_Changed(GetParent(hDlg),hDlg);
			return TRUE;
		}
		break;
	case WM_NOTIFY:
		LPNMHDR hdr;
		hdr=(LPNMHDR)lParam;
		switch (hdr->code) {
		case PSN_SETACTIVE:
			g_StartPage=2;
			bEditByCode=TRUE;
			CheckDlgButton(hDlg,IDC_MARGIN,NewOption.TestStyle(AES_SHOWMARGIN));
			CheckDlgButton(hDlg,IDC_LINENUM,NewOption.TestStyle(AES_SHOWLINENUM));
			CheckDlgButton(hDlg,IDC_SHOWENTER,NewOption.TestStyle(AES_SHOWENTER));
			CheckDlgButton(hDlg,IDC_SHOWTAB,NewOption.TestStyle(AES_SHOWTAB));
			CheckDlgButton(hDlg,IDC_SHOWSPACE,NewOption.TestStyle(AES_SHOWSPACE));
			CheckDlgButton(hDlg,IDC_SHOWRULER,NewOption.TestStyle(AES_SHOWRULER));

			SetDlgItemInt(hDlg,IDC_FRTLEFT,NewOption.InnerRect.left,FALSE);
			SetDlgItemInt(hDlg,IDC_FRTTOP,NewOption.InnerRect.top,FALSE);
			SetDlgItemInt(hDlg,IDC_FRTRIGHT,NewOption.InnerRect.right,FALSE);
			SetDlgItemInt(hDlg,IDC_FRTBOTTOM,NewOption.InnerRect.bottom,FALSE);

			SetDlgItemText(hDlg,IDC_EDBACKBIT,NewOption.BackBit);
			SendDlgItemMessage(hDlg,IDC_CBBACKBITPOS,CB_SETCURSEL,NewOption.BackBitPos,0);
			SendDlgItemMessage(hDlg,IDC_CBSELBLOCKTYPE,CB_SETCURSEL,NewOption.SelBlockType,0);
			SendDlgItemMessage(hDlg,IDC_CBVALIGN,CB_SETCURSEL,NewOption.VAlign,0);
			bEditByCode=FALSE;
			AdjustDlgUI(2,hDlg);
			return TRUE;
		case PSN_APPLY:
			ApplyNow();
			return TRUE;
		case PSN_KILLACTIVE:
			t1=GetDlgItemInt(hDlg,IDC_FRTLEFT,NULL,FALSE);
			t2=GetDlgItemInt(hDlg,IDC_FRTTOP,NULL,FALSE);
			t3=GetDlgItemInt(hDlg,IDC_FRTRIGHT,NULL,FALSE);
			t4=GetDlgItemInt(hDlg,IDC_FRTBOTTOM,NULL,FALSE);
			if (t1<0 || t1>32 || t2<0 || t2>32 || t3<0 || t3>32 || t4<0 || t4>32) {
				MessageBox(hDlg,"안쪽 여백은 0~32사이여야 합니다.","알림",MB_OK);
				SetWindowLong(hDlg,DWL_MSGRESULT,TRUE);
				return TRUE;
			}

			NewOption.InnerRect.left=t1;
			NewOption.InnerRect.top=t2;
			NewOption.InnerRect.right=t3;
			NewOption.InnerRect.bottom=t4;

			NewOption.ChangeStyle(AES_SHOWMARGIN,IsDlgButtonChecked(hDlg,IDC_MARGIN));
			NewOption.ChangeStyle(AES_SHOWLINENUM,IsDlgButtonChecked(hDlg,IDC_LINENUM));
			NewOption.ChangeStyle(AES_SHOWENTER,IsDlgButtonChecked(hDlg,IDC_SHOWENTER));
			NewOption.ChangeStyle(AES_SHOWTAB,IsDlgButtonChecked(hDlg,IDC_SHOWTAB));
			NewOption.ChangeStyle(AES_SHOWSPACE,IsDlgButtonChecked(hDlg,IDC_SHOWSPACE));
			NewOption.ChangeStyle(AES_SHOWRULER,IsDlgButtonChecked(hDlg,IDC_SHOWRULER));

			GetDlgItemText(hDlg,IDC_EDBACKBIT,NewOption.BackBit,MAX_PATH);
			NewOption.BackBitPos=SendDlgItemMessage(hDlg,IDC_CBBACKBITPOS,CB_GETCURSEL,0,0);
			NewOption.SelBlockType=SendDlgItemMessage(hDlg,IDC_CBSELBLOCKTYPE,CB_GETCURSEL,0,0);
			NewOption.VAlign=SendDlgItemMessage(hDlg,IDC_CBVALIGN,CB_GETCURSEL,0,0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK ToolBarDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	static HWND hListSt;
	static int oldidx;
	LVCOLUMN COL;
	LVITEM LI;
	int i,idx;
	TCHAR szTemp[32];
	static TCHAR *arStName[]={"현재 위치","문서 크기","편집 여부","파일 포맷","삽입 모드",
	"대소문자 입력 모드","한글 입력 모드","문자 코드","선택영역 정보","클립보드의 내용","읽기 전용 여부",
	"현재 시간","현재 날짜"};
	TCHAR tName[64],tShow,tCol[4][12];

	switch(iMessage) {
	case WM_INITDIALOG:
		hListSt=GetDlgItem(hDlg,IDC_LISTSTATUS);
		COL.mask=LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		COL.fmt=LVCFMT_LEFT;
		COL.cx=150;
		COL.pszText="정보";
		COL.iSubItem=0;
		ListView_InsertColumn(hListSt,0,&COL);

		COL.cx=50;
		COL.pszText="폭";
		COL.iSubItem=1;
		ListView_InsertColumn(hListSt,1,&COL);

		COL.cx=60;
		COL.pszText="정렬";
		COL.iSubItem=1;
		ListView_InsertColumn(hListSt,2,&COL);

		COL.cx=50;
		COL.pszText="모양";
		COL.iSubItem=3;
		ListView_InsertColumn(hListSt,3,&COL);
		ListView_SetExtendedListViewStyle(hListSt, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
		SendDlgItemMessage(hDlg,IDC_SLIDERSTWIDTH,TBM_SETRANGE,FALSE,MAKELPARAM(40,200));
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_TOOLBARBIG:
		case IDC_TOOLBARTEXT:
			PropSheet_Changed(GetParent(hDlg),hDlg);
			return TRUE;
		case IDC_BTNSTUP:
		case IDC_BTNSTDOWN:
			idx=ListView_GetNextItem(hListSt,-1,LVNI_ALL | LVNI_SELECTED);
			if (idx==-1 || (LOWORD(wParam)==IDC_BTNSTDOWN && idx >= STNUM-1) ||
				(LOWORD(wParam)==IDC_BTNSTUP && idx == 0)) {
				return TRUE;
			}
			LI.mask=LVIF_TEXT;
			LI.iItem=idx;
			LI.iSubItem=0;
			LI.pszText=tName;
			LI.cchTextMax=64;
			ListView_GetItem(hListSt,&LI);
			tShow=ListView_GetCheckState(hListSt,idx);
			for (i=1;i<=3;i++) {
				ListView_GetItemText(hListSt,idx,i,tCol[i],16);
			}

			ListView_DeleteItem(hListSt,idx);
			if (LOWORD(wParam)==IDC_BTNSTUP) {
				idx--;
			} else {
				idx++;
			}
			LI.iItem=idx;
			ListView_InsertItem(hListSt,&LI);
			ListView_SetItemState(hListSt,idx,INDEXTOSTATEIMAGEMASK(
				(tShow)?2:1),LVIS_STATEIMAGEMASK);
			for (i=1;i<=3;i++) {
				ListView_SetItemText(hListSt,idx,i,tCol[i]);
			}
			ListView_SetItemState(hListSt,idx,
				LVIS_SELECTED | LVIS_FOCUSED,LVIS_SELECTED | LVIS_FOCUSED);
			PropSheet_Changed(GetParent(hDlg),hDlg);
			return TRUE;
		case IDC_RASTLEFT:
			idx=ListView_GetNextItem(hListSt,-1,LVNI_ALL | LVNI_SELECTED);
			ListView_SetItemText(hListSt,idx,2,"왼쪽");
			PropSheet_Changed(GetParent(hDlg),hDlg);
			return TRUE;
		case IDC_RASTCENTER:
			idx=ListView_GetNextItem(hListSt,-1,LVNI_ALL | LVNI_SELECTED);
			ListView_SetItemText(hListSt,idx,2,"중앙");
			PropSheet_Changed(GetParent(hDlg),hDlg);
			return TRUE;
		case IDC_RASTRIGHT:
			idx=ListView_GetNextItem(hListSt,-1,LVNI_ALL | LVNI_SELECTED);
			ListView_SetItemText(hListSt,idx,2,"오른쪽");
			PropSheet_Changed(GetParent(hDlg),hDlg);
			return TRUE;
		case IDC_CHKPOPOUT:
			idx=ListView_GetNextItem(hListSt,-1,LVNI_ALL | LVNI_SELECTED);
			ListView_SetItemText(hListSt,idx,3,IsDlgButtonChecked(hDlg,IDC_CHKPOPOUT) 
				? "양각":"음각");
			PropSheet_Changed(GetParent(hDlg),hDlg);
			return TRUE;
		}
		break;
	case WM_HSCROLL:
		if ((HWND)lParam == GetDlgItem(hDlg,IDC_SLIDERSTWIDTH)) {
			idx=ListView_GetNextItem(hListSt,-1,LVNI_ALL | LVNI_SELECTED);
			wsprintf(szTemp,"%d",SendMessage(GetDlgItem(hDlg,IDC_SLIDERSTWIDTH),TBM_GETPOS,0,0));
			ListView_SetItemText(hListSt,idx,1,szTemp);
			PropSheet_Changed(GetParent(hDlg),hDlg);
		}
		return TRUE;
	case WM_NOTIFY:
		LPNMHDR hdr;
		LPNMLISTVIEW nlv;
		hdr=(LPNMHDR)lParam;
		nlv=(LPNMLISTVIEW)lParam;

		if (hdr->hwndFrom == hListSt) {
			switch (hdr->code) {
			case LVN_ITEMCHANGED:
				if (nlv->uChanged == LVIF_STATE && nlv->uNewState == (LVIS_SELECTED | LVIS_FOCUSED)) {
					oldidx=nlv->iItem;
					ListView_GetItemText(hListSt,nlv->iItem,1,szTemp,32);
					SendDlgItemMessage(hDlg,IDC_SLIDERSTWIDTH,TBM_SETPOS,TRUE,atoi(szTemp));
					ListView_GetItemText(hListSt,nlv->iItem,2,szTemp,32);
					if (lstrcmp(szTemp,"왼쪽")==0) {
						CheckRadioButton(hDlg,IDC_RASTLEFT,IDC_RASTRIGHT,IDC_RASTLEFT);
					} else if (lstrcmp(szTemp,"중앙")==0) {
						CheckRadioButton(hDlg,IDC_RASTLEFT,IDC_RASTRIGHT,IDC_RASTCENTER);
					} else {
						CheckRadioButton(hDlg,IDC_RASTLEFT,IDC_RASTRIGHT,IDC_RASTRIGHT);
					}
					ListView_GetItemText(hListSt,nlv->iItem,3,szTemp,32);
					CheckDlgButton(hDlg,IDC_CHKPOPOUT,lstrcmp(szTemp,"양각")==0);
				}
				// 체크 박스 클릭시. 코드 변경시는 막아야 함
				if (nlv->uChanged == LVIF_STATE && (nlv->uNewState & LVIS_STATEIMAGEMASK) != 
					(nlv->uOldState & LVIS_STATEIMAGEMASK)) {
					if (!bEditByCode) {
						PropSheet_Changed(GetParent(hDlg),hDlg);
					}
				}
				break;
			}
			return TRUE;
		}

		switch (hdr->code) {
		case PSN_SETACTIVE:
			g_StartPage=3;
			CheckDlgButton(hDlg,IDC_TOOLBARBIG,NewOption.bToolBarBig);
			CheckDlgButton(hDlg,IDC_TOOLBARTEXT,NewOption.bToolBarText);
			ListView_DeleteAllItems(hListSt);
			bEditByCode=TRUE;
			for (i=0;i<STNUM;i++) {
				idx=NewOption.GetStNumByOrder(i);
				LI.mask=LVIF_TEXT;
				LI.iItem=i;
				LI.iSubItem=0;
				LI.pszText=arStName[idx];
				ListView_InsertItem(hListSt,&LI);

				ListView_SetItemState(hListSt,i,INDEXTOSTATEIMAGEMASK(
					(NewOption.StMask[idx].bShow)?2:1),LVIS_STATEIMAGEMASK);

				wsprintf(szTemp,"%d",NewOption.StMask[idx].Width);
				ListView_SetItemText(hListSt,i,1,szTemp);

				switch (NewOption.StMask[idx].Align) {
				case 0:
					lstrcpy(szTemp,"왼쪽");
					break;
				case 1:
					lstrcpy(szTemp,"중앙");
					break;
				case 2:
					lstrcpy(szTemp,"오른쪽");
					break;
				}
				ListView_SetItemText(hListSt,i,2,szTemp);

				if (NewOption.StMask[idx].bPop) {
					ListView_SetItemText(hListSt,i,3,"양각");
				} else {
					ListView_SetItemText(hListSt,i,3,"음각");
				}
				// 마지막 선택했던 항목을 다시 선택한다. 그렇지 않으면 적용시 항상 첫 항목이 선택되어 불편하다.
				ListView_SetItemState(hListSt,oldidx,
					LVIS_SELECTED | LVIS_FOCUSED,LVIS_SELECTED | LVIS_FOCUSED);
				ListView_EnsureVisible(hListSt,oldidx,FALSE);
			}
			bEditByCode=FALSE;
			return TRUE;
		case PSN_APPLY:
			ApplyNow();
			return TRUE;
		case PSN_KILLACTIVE:
			NewOption.bToolBarBig=IsDlgButtonChecked(hDlg,IDC_TOOLBARBIG);
			NewOption.bToolBarText=IsDlgButtonChecked(hDlg,IDC_TOOLBARTEXT);
			for (i=0;i<STNUM;i++) {
				ListView_GetItemText(hListSt,i,0,szTemp,32);
				for (idx=0;idx<STNUM;idx++) {
					if (lstrcmp(szTemp,arStName[idx])==0)
						break;
				}
				NewOption.StMask[idx].bShow=(ListView_GetCheckState(hListSt,i) ? true:false);
				NewOption.StMask[idx].Order=i;
				ListView_GetItemText(hListSt,i,1,szTemp,32);
				NewOption.StMask[idx].Width=atoi(szTemp);
				ListView_GetItemText(hListSt,i,2,szTemp,32);
				if (lstrcmp(szTemp,"왼쪽")==0) {
					NewOption.StMask[idx].Align=0;
				} else if (lstrcmp(szTemp,"중앙")==0) {
					NewOption.StMask[idx].Align=1;
				} else {
					NewOption.StMask[idx].Align=2;
				}
				ListView_GetItemText(hListSt,i,3,szTemp,32);
				NewOption.StMask[idx].bPop=(lstrcmp(szTemp,"양각")==0);
			}
			return TRUE;
		}
		break;
	}
	return FALSE;
}

COLORREF& GetColorFromID(int ID)
{
	return NewOption.Color[ID-IDC_MARCOLOR1];
}

void InitSysColor()
{
	arSysColor[6]=GetSysColor(COLOR_WINDOWTEXT);
	arSysColor[7]=arSysColor[10]=GetSysColor(COLOR_WINDOW);
	arSysColor[8]=GetSysColor(COLOR_HIGHLIGHTTEXT);
	arSysColor[9]=GetSysColor(COLOR_HIGHLIGHT);
}

BOOL CALLBACK ColorDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	int i,idx;
	LPMEASUREITEMSTRUCT lpmis;
	LPDRAWITEMSTRUCT lpdis;
	HBRUSH bkBrush, Brush, OldBrush;
	int OldMode;
	CHOOSECOLOR COL;
	static COLORREF crTemp[16];
	COLORREF Color;

	switch(iMessage) {
	case WM_INITDIALOG:
		for (idx=IDC_MARCOLOR1;idx<=IDC_SELFRAMECOLOR;idx++) {
			for (i=0;i<sizeof(arPreColor)/sizeof(arPreColor[0]);i++) {
				SendDlgItemMessage(hDlg,idx,CB_ADDSTRING,0,0);
			}
		}
		return TRUE;
	case WM_MEASUREITEM:
		lpmis=(LPMEASUREITEMSTRUCT)lParam;
		lpmis->itemHeight=18;
		return TRUE;
	case WM_DRAWITEM:
		Color=GetColorFromID(wParam);
		lpdis=(LPDRAWITEMSTRUCT)lParam;

		if (lpdis->itemState & ODS_SELECTED) {
			bkBrush=CreateSolidBrush(RGB(0,0,255));
		}
		else {
			bkBrush=CreateSolidBrush(RGB(255,255,255));
		}
		FillRect(lpdis->hDC, &lpdis->rcItem, bkBrush);

		switch (lpdis->itemID) {
		case 0:
			Brush=CreateSolidBrush(arSysColor[wParam-IDC_MARCOLOR1]);
			break;
		case 1:
			Brush=CreateSolidBrush(Color & 0xffffff);
			break;
		default:
			Brush=CreateSolidBrush(arPreColor[lpdis->itemID].Color);
			break;
		}
		OldBrush=(HBRUSH)SelectObject(lpdis->hDC, Brush);
		if (lpdis->itemID != 2) {
			Rectangle(lpdis->hDC,lpdis->rcItem.left+2,lpdis->rcItem.top+1,
				lpdis->rcItem.left+20, lpdis->rcItem.bottom-1);
		}

		OldMode=SetBkMode(lpdis->hDC,TRANSPARENT);
		TextOut(lpdis->hDC,lpdis->rcItem.left+25,lpdis->rcItem.top+2,
			arPreColor[lpdis->itemID].Name,lstrlen(arPreColor[lpdis->itemID].Name));

		SetBkMode(lpdis->hDC,OldMode);
		SelectObject(lpdis->hDC, OldBrush);
		DeleteObject(bkBrush);
		DeleteObject(Brush);
		return TRUE;
		break;
	case WM_COMMAND:
		switch (HIWORD(wParam)) {
		case CBN_SELCHANGE:
			idx=SendDlgItemMessage(hDlg,LOWORD(wParam),CB_GETCURSEL,0,0);
			if (idx==2) {
				memset(&COL, 0, sizeof(CHOOSECOLOR));
				COL.lStructSize = sizeof(CHOOSECOLOR);
				COL.hwndOwner=hDlg;
				COL.lpCustColors=crTemp;
				if (ChooseColor(&COL)!=0) {
					GetColorFromID(LOWORD(wParam))=COL.rgbResult | 0x01000000;
					InvalidateRect(GetDlgItem(hDlg,LOWORD(wParam)),NULL,TRUE);
				}
				SendDlgItemMessage(hDlg,LOWORD(wParam),CB_SETCURSEL,1,0);
			} else {
				GetColorFromID(LOWORD(wParam))=idx << 24 | arPreColor[idx].Color;
			}
			PropSheet_Changed(GetParent(hDlg),hDlg);
			break;
		}
		return TRUE;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code) {
		case PSN_SETACTIVE:
			g_StartPage=4;
			for (idx=IDC_MARCOLOR1;idx<=IDC_SELFRAMECOLOR;idx++) {
				SendDlgItemMessage(hDlg,idx,CB_SETCURSEL,GetColorFromID(idx) >> 24,0);
			}
			return TRUE;
		case PSN_APPLY:
			ApplyNow();
			return TRUE;
		case PSN_KILLACTIVE:
			return TRUE;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK EditDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HWND hCon;
	int tint;

	switch(iMessage) {
	case WM_INITDIALOG:
		hCon=GetDlgItem(hDlg,IDC_TABWIDTH);
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"1");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"2");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"4");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"8");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"16");

		hCon=GetDlgItem(hDlg,IDC_UNDOLIMIT);
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"최저(10K)");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"작음(100K)");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"보통(1M)");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"큼(5M)");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"최대(10M)");

		hCon=GetDlgItem(hDlg,IDC_FINDDLGPOS);
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"메인 윈도우의 왼쪽 위");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"메인 윈도우의 오른쪽 위");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"메인 윈도우의 오른쪽 아래");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"메인 윈도우 중앙");
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))	{
		case IDC_TABWIDTH:
		case IDC_UNDOLIMIT:
		case IDC_FINDDLGPOS:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
			case CBN_EDITCHANGE:
				PropSheet_Changed(GetParent(hDlg),hDlg);
				break;
			}
			return TRUE;
		case IDC_ALLOWDRAG:
		case IDC_USELINEEND:
		case IDC_INITFIND:
		case IDC_INITNEXTFIND:
		case IDC_SOUNDFINDFAIL:
		case IDC_SPACEFORTAB:
		case IDC_AUTOINDENT:
		case IDC_BLOCKINDENTWITHTAB:
		case IDC_HOMETOFIRSTCHAR:
		case IDC_CALCTABWITHAVG:
		case IDC_SWAPDRAGBTN:
			PropSheet_Changed(GetParent(hDlg),hDlg);
			return TRUE;
		}
		break;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code) {
		case PSN_SETACTIVE:
			g_StartPage=5;
			SetDlgItemInt(hDlg,IDC_TABWIDTH,NewOption.TabWidth,FALSE);
			CheckDlgButton(hDlg,IDC_ALLOWDRAG,NewOption.TestStyle(AES_ALLOWDRAG));
			CheckDlgButton(hDlg,IDC_USELINEEND,NewOption.TestStyle(AES_USELINEEND));
			SendDlgItemMessage(hDlg,IDC_UNDOLIMIT,CB_SETCURSEL,NewOption.UndoLimit,0);
			SendDlgItemMessage(hDlg,IDC_FINDDLGPOS,CB_SETCURSEL,NewOption.FindDlgPos,0);
			CheckDlgButton(hDlg,IDC_INITFIND,NewOption.bInitFind);
			CheckDlgButton(hDlg,IDC_INITNEXTFIND,NewOption.bInitNextFind);
			CheckDlgButton(hDlg,IDC_SOUNDFINDFAIL,NewOption.bSoundFindFail);
			CheckDlgButton(hDlg,IDC_SPACEFORTAB,NewOption.TestStyle(AES_SPACEFORTAB));
			CheckDlgButton(hDlg,IDC_AUTOINDENT,NewOption.TestStyle(AES_AUTOINDENT));
			CheckDlgButton(hDlg,IDC_BLOCKINDENTWITHTAB,NewOption.TestStyle(AES_BLOCKINDENTWITHTAB));
			CheckDlgButton(hDlg,IDC_HOMETOFIRSTCHAR,NewOption.TestStyle(AES_HOMETOFIRSTCHAR));
			CheckDlgButton(hDlg,IDC_CALCTABWITHAVG,NewOption.TestStyle(AES_CALCTABWITHAVG));
			CheckDlgButton(hDlg,IDC_SWAPDRAGBTN,NewOption.TestStyle(AES_SWAPDRAGBUTTON));
			return TRUE;
		case PSN_APPLY:
			ApplyNow();
			return TRUE;
		case PSN_KILLACTIVE:
			tint=GetDlgItemInt(hDlg,IDC_TABWIDTH,NULL,FALSE);
			if (tint < 1 || tint > 32) {
				MessageBox(hDlg,"탭은 1~32사이의 값을 지정해야 합니다","알림",MB_OK);
				SetWindowLongPtr(hDlg,DWLP_MSGRESULT,TRUE);
				return TRUE;
			}

			NewOption.TabWidth=GetDlgItemInt(hDlg,IDC_TABWIDTH,NULL,FALSE);
			NewOption.ChangeStyle(AES_ALLOWDRAG,IsDlgButtonChecked(hDlg,IDC_ALLOWDRAG));
			NewOption.ChangeStyle(AES_USELINEEND,IsDlgButtonChecked(hDlg,IDC_USELINEEND));
			NewOption.UndoLimit=SendDlgItemMessage(hDlg,IDC_UNDOLIMIT,CB_GETCURSEL,0,0);
			NewOption.FindDlgPos=SendDlgItemMessage(hDlg,IDC_FINDDLGPOS,CB_GETCURSEL,0,0);
			NewOption.bInitFind=IsDlgButtonChecked(hDlg,IDC_INITFIND);
			NewOption.bInitNextFind=IsDlgButtonChecked(hDlg,IDC_INITNEXTFIND);
			NewOption.bSoundFindFail=IsDlgButtonChecked(hDlg,IDC_SOUNDFINDFAIL);
			NewOption.ChangeStyle(AES_SPACEFORTAB,IsDlgButtonChecked(hDlg,IDC_SPACEFORTAB));
			NewOption.ChangeStyle(AES_AUTOINDENT,IsDlgButtonChecked(hDlg,IDC_AUTOINDENT));
			NewOption.ChangeStyle(AES_BLOCKINDENTWITHTAB,IsDlgButtonChecked(hDlg,IDC_BLOCKINDENTWITHTAB));
			NewOption.ChangeStyle(AES_HOMETOFIRSTCHAR,IsDlgButtonChecked(hDlg,IDC_HOMETOFIRSTCHAR));
			NewOption.ChangeStyle(AES_CALCTABWITHAVG,IsDlgButtonChecked(hDlg,IDC_CALCTABWITHAVG));
			NewOption.ChangeStyle(AES_SWAPDRAGBUTTON,IsDlgButtonChecked(hDlg,IDC_SWAPDRAGBTN));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK SyntaxDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HWND hCon;
	LVCOLUMN COL;
	LVITEM LI;
	int i,idx;
	LPDRAWITEMSTRUCT dis;
	HBRUSH hBrush, hOldBrush;
	static int mi,msubi;
	CHOOSECOLOR COLDLG;
	static COLORREF crTemp[16];
	int tx,ty;
	TCHAR *tstr;
	COLORREF OldBk, OldColor;
	DWORD OldMode;
	COLORREF fore,back;
	CParse *Parser;

	switch(iMessage) {
	case WM_INITDIALOG:
		hCon=GetDlgItem(hDlg,IDC_PARSER);
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"C/C++");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"HTML");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"SQL");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"Java");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"C#");
		SendMessage(hCon,CB_SETCURSEL,0,0);

		hCon=GetDlgItem(hDlg,IDC_STYLE);
		COL.mask=LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		COL.fmt=LVCFMT_LEFT;
		COL.cx=100;
		COL.pszText="스타일";
		COL.iSubItem=0;
		ListView_InsertColumn(hCon,0,&COL);

		COL.cx=80;
		COL.pszText="글자색";
		COL.iSubItem=1;
		ListView_InsertColumn(hCon,1,&COL);

		COL.cx=80;
		COL.pszText="배경색";
		COL.iSubItem=2;
		ListView_InsertColumn(hCon,2,&COL);
		SendMessage(GetDlgItem(hDlg,IDC_EXT),EM_LIMITTEXT,249,0);
		return TRUE;
	case WM_DRAWITEM:
		dis=(LPDRAWITEMSTRUCT)lParam;
		if (wParam==IDC_STYLE) {
			idx=SendDlgItemMessage(hDlg,IDC_PARSER,CB_GETCURSEL,0,0);
			fore=NewOption.arStyle[idx+1][dis->itemID].fore;
			back=NewOption.arStyle[idx+1][dis->itemID].back;

			if (fore != -1) {
				OldColor=SetTextColor(dis->hDC,fore);
			}
			if (back != -1) {
				OldBk=SetBkColor(dis->hDC, back);
			}
			tstr=NewOption.arStyle[idx+1][dis->itemID].name;
			OldMode=SetBkMode(dis->hDC,OPAQUE);
			TextOut(dis->hDC,dis->rcItem.left, dis->rcItem.top+3,
				tstr,lstrlen(tstr));
			SetBkMode(dis->hDC,OldMode);

			if (fore != -1) {
				hBrush=CreateSolidBrush(fore);
				hOldBrush=(HBRUSH)SelectObject(dis->hDC,hBrush);
				SetTextColor(dis->hDC,OldColor);
			}
			Rectangle(dis->hDC,dis->rcItem.left+105, dis->rcItem.top+1,
				dis->rcItem.left+175, dis->rcItem.bottom-1);
			if (fore == -1) {
				TextOut(dis->hDC,dis->rcItem.left+125, dis->rcItem.top+3,"기본",4);
			} else {
				DeleteObject(SelectObject(dis->hDC,hOldBrush));
			}

			if (back != -1) {
				hBrush=CreateSolidBrush(back);
				hOldBrush=(HBRUSH)SelectObject(dis->hDC,hBrush);
				SetBkColor(dis->hDC, OldBk);
			}
			Rectangle(dis->hDC,dis->rcItem.left+185, dis->rcItem.top+1,
				dis->rcItem.left+255, dis->rcItem.bottom-1);
			if (back == -1) {
				TextOut(dis->hDC,dis->rcItem.left+205, dis->rcItem.top+3,"기본",4);
			} else {
				DeleteObject(SelectObject(dis->hDC,hOldBrush));
			}
		} else {
			if (dis->itemState & ODS_SELECTED) {
				hBrush=CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
				OldBk=SetBkColor(dis->hDC, GetSysColor(COLOR_HIGHLIGHT));
				OldColor=SetTextColor(dis->hDC,GetSysColor(COLOR_HIGHLIGHTTEXT));
				OldMode=SetBkMode(dis->hDC,TRANSPARENT);
			}
			else {
				hBrush=CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
			}
			FillRect(dis->hDC, &dis->rcItem, hBrush);
			DeleteObject(hBrush);
			hBrush=CreateSolidBrush(arPreColor[dis->itemID].Color);
			hOldBrush=(HBRUSH)SelectObject(dis->hDC,hBrush);
			if (dis->itemID > 1) {
				Rectangle(dis->hDC,dis->rcItem.left+2,dis->rcItem.top+1,
					dis->rcItem.left+23,dis->rcItem.bottom-1);
				tx=dis->rcItem.left+30;
			} else {
				tx=dis->rcItem.left+3;
			}
			ty=dis->rcItem.top+3;
			if (dis->itemID == 0) {
				tstr="기본색";
			} else if (dis->itemID == 1) {
				tstr="색상선택...";
			} else {
				tstr=arPreColor[dis->itemID].Name;
			}
			TextOut(dis->hDC,tx,ty,tstr,lstrlen(tstr));

			if (dis->itemState & ODS_SELECTED) {
				SetBkColor(dis->hDC, OldBk);
				SetTextColor(dis->hDC,OldColor);
				SetBkMode(dis->hDC,OldMode);
			}
			DeleteObject(SelectObject(dis->hDC,hOldBrush));
		}
		break;
	case WM_COMMAND:
		idx=SendDlgItemMessage(hDlg,IDC_PARSER,CB_GETCURSEL,0,0);
		if (LOWORD(wParam) > 1 && LOWORD(wParam) < sizeof(arPreColor)/sizeof(arPreColor[0])) {
			if (msubi==1) {
				NewOption.arStyle[idx+1][mi].fore=arPreColor[LOWORD(wParam)].Color;
			} else {
				NewOption.arStyle[idx+1][mi].back=arPreColor[LOWORD(wParam)].Color;
			}
			InvalidateRect(GetDlgItem(hDlg,IDC_STYLE),NULL,TRUE);
			PropSheet_Changed(GetParent(hDlg),hDlg);
			return TRUE;
		}

		switch (LOWORD(wParam))	{
		case 0:
			if (msubi==1) {
				NewOption.arStyle[idx+1][mi].fore=-1;
			} else {
				NewOption.arStyle[idx+1][mi].back=-1;
			}
			InvalidateRect(GetDlgItem(hDlg,IDC_STYLE),NULL,TRUE);
			PropSheet_Changed(GetParent(hDlg),hDlg);
			return TRUE;
		case 1:
			memset(&COLDLG, 0, sizeof(CHOOSECOLOR));
			COLDLG.lStructSize = sizeof(CHOOSECOLOR);
			COLDLG.hwndOwner=hDlg;
			COLDLG.lpCustColors=crTemp;
			COLDLG.Flags=CC_FULLOPEN;
			if (ChooseColor(&COLDLG)!=0) {
				if (msubi==1) {
					NewOption.arStyle[idx+1][mi].fore=COLDLG.rgbResult;
				} else {
					NewOption.arStyle[idx+1][mi].back=COLDLG.rgbResult;
				}
				InvalidateRect(GetDlgItem(hDlg,IDC_STYLE),NULL,TRUE);
				PropSheet_Changed(GetParent(hDlg),hDlg);
			}
			return TRUE;
		case IDC_PARSER:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				ListView_DeleteAllItems(GetDlgItem(hDlg,IDC_STYLE));
				for (i=0;NewOption.arStyle[idx+1][i].fore!=-2;i++) {
					LI.mask=LVIF_TEXT;
					LI.iSubItem=0;
					LI.iItem=i;
					LI.pszText="zzz";
					ListView_InsertItem(GetDlgItem(hDlg,IDC_STYLE),&LI);
				}
				bEditByCode=TRUE;
				SetDlgItemText(hDlg,IDC_EXT,NewOption.arExt[idx+1]);
				bEditByCode=FALSE;

				switch (idx+1) {
				case 1:
					Parser=new CParseCpp;
					break;
				case 2:
					Parser=new CParseHtml;
					break;
				case 3:
					Parser=new CParseSql;
					break;
				case 4:
					Parser=new CParseJava;
					break;
				case 5:
					Parser=new CParseCS;
					break;
				}

				SetDlgItemText(hDlg,IDC_PARDELI,Parser->GetInfo(1));
				SetDlgItemText(hDlg,IDC_PARCOM,Parser->GetInfo(2));
				SetDlgItemText(hDlg,IDC_PARCOMSTART,Parser->GetInfo(3));
				SetDlgItemText(hDlg,IDC_PARCOMEND,Parser->GetInfo(4));
				SetDlgItemText(hDlg,IDC_PARBLOCKSTART,Parser->GetInfo(5));
				SetDlgItemText(hDlg,IDC_PARBLOCKEND,Parser->GetInfo(6));
				delete Parser;
				break;
			}
			return TRUE;
		case IDC_EXT:
			if (HIWORD(wParam) == EN_CHANGE) {
				if (bEditByCode==FALSE) {
					GetDlgItemText(hDlg,IDC_EXT,NewOption.arExt[idx+1],250);
					PropSheet_Changed(GetParent(hDlg),hDlg);
				}
			}
			return TRUE;
		case IDC_BTNDEFSTYLE:
			switch (idx+1) {
			case 1:
				Parser=new CParseCpp;
				break;
			case 2:
				Parser=new CParseHtml;
				break;
			case 3:
				Parser=new CParseSql;
				break;
			case 4:
				Parser=new CParseJava;
				break;
			case 5:
				Parser=new CParseCS;
				break;
			}

			for (i=0;i<32;i++) {
				Parser->GetStyleColor(i,NewOption.arStyle[idx+1][i].fore,
					NewOption.arStyle[idx+1][i].back);
				if (NewOption.arStyle[idx+1][i].fore==-2)
					break;
			}
			delete Parser;
			InvalidateRect(GetDlgItem(hDlg,IDC_STYLE),NULL,TRUE);
			PropSheet_Changed(GetParent(hDlg),hDlg);
			return TRUE;
		}
		break;
	case WM_MEASUREITEM:
		LPMEASUREITEMSTRUCT lpmis;
		lpmis = (LPMEASUREITEMSTRUCT)lParam;
		lpmis->itemWidth=80;
		lpmis->itemHeight=18;
		return TRUE;
	case WM_NOTIFY:
		LPNMHDR hdr;
		hdr=(LPNMHDR)lParam;

		if (hdr->hwndFrom == GetDlgItem(hDlg,IDC_STYLE)) {
			switch (hdr->code) {
			case NM_CLICK:
				LVHITTESTINFO info;
				HMENU hPopup;
				POINT pt;
				pt=info.pt=((LPNMITEMACTIVATE)lParam)->ptAction;
				ListView_SubItemHitTest(hdr->hwndFrom,&info);
				if (info.iItem !=-1 && info.iSubItem > 0) {
					mi=info.iItem;
					msubi=info.iSubItem;
					hPopup=CreatePopupMenu();
					AppendMenu(hPopup,MF_STRING | MF_OWNERDRAW,0,0);
					AppendMenu(hPopup,MF_STRING | MF_OWNERDRAW,1,0);
					idx=sizeof(arPreColor)/sizeof(arPreColor[0]);
					for (i=3;i<idx;i++) {
						if (i==(idx-1)/2+1+(idx%2 ? 0:1)) {
							AppendMenu(hPopup,MF_STRING | MF_OWNERDRAW | MF_MENUBARBREAK,i,0);
						} else {
							AppendMenu(hPopup,MF_STRING | MF_OWNERDRAW,i,0);
						}
					}

					ClientToScreen(hdr->hwndFrom,&pt);
					TrackPopupMenu(hPopup, TPM_LEFTALIGN, pt.x,pt.y,
						0, hDlg, NULL);
					DestroyMenu(hPopup);
				}

				break;
			}
			return TRUE;
		}

		switch (hdr->code) {
		case PSN_SETACTIVE:
			g_StartPage=6;
			SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_PARSER,CBN_SELCHANGE),
				(LPARAM)GetDlgItem(hDlg,IDC_PARSER));
			return TRUE;
		case PSN_APPLY:
			ApplyNow();
			return TRUE;
		case PSN_KILLACTIVE:
			return TRUE;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK PrintDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HWND hCon;
	int i,idx;
	int t1,t2,t3,t4;
	TCHAR szFace[32];
	HMENU hMenu, hPopup;
	POINT pt;
	static int PopupID;
	TCHAR szTemp[256];

	switch(iMessage) {
	case WM_INITDIALOG:
		hCon=GetDlgItem(hDlg,IDC_PRTHEADERWIDTH);
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"없음");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"0.1 mm");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"0.2 mm");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"0.3 mm");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"0.5 mm");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"0.8 mm");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"1 mm");

		hCon=GetDlgItem(hDlg,IDC_PRTFOOTERWIDTH);
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"없음");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"0.1 mm");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"0.2 mm");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"0.3 mm");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"0.5 mm");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"0.8 mm");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"1 mm");

		hCon=GetDlgItem(hDlg,IDC_PRTBKTEXT);
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"비밀 문서");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"외부 유출 금지");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"초안 출력");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"최종 출력");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"결제용");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"장기 보관용");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"손대지 마!");

		hCon=GetDlgItem(hDlg,IDC_PRTFONTSIZE);
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"8");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"9");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"10");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"11");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"12");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"15");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"20");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"30");

		hCon=GetDlgItem(hDlg,IDC_PRTWRAP);
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"문서 모양 그대로 출력");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"한글 글자, 영문 글자");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"한글 글자, 영문 단어");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"한글 단어, 영문 단어");

		SendDlgItemMessage(hDlg,IDC_PRTFONTFACE,CB_ADDSTRING,0,(LPARAM)"==시스템==");
		for (i=0;i<FontNum;i++) {
			SendDlgItemMessage(hDlg,IDC_PRTFONTFACE,CB_ADDSTRING,0,(LPARAM)arFont[i].lfFaceName);
		}

		SendDlgItemMessage(hDlg,IDC_PRTLEFTSPIN,UDM_SETRANGE,0,MAKELPARAM(99,1));
		SendDlgItemMessage(hDlg,IDC_PRTTOPSPIN,UDM_SETRANGE,0,MAKELPARAM(99,1));
		SendDlgItemMessage(hDlg,IDC_PRTRIGHTSPIN,UDM_SETRANGE,0,MAKELPARAM(99,1));
		SendDlgItemMessage(hDlg,IDC_PRTBOTTOMSPIN,UDM_SETRANGE,0,MAKELPARAM(9,1));

		SendDlgItemMessage(hDlg,IDC_PRTHEADER,EM_LIMITTEXT,255,0);
		SendDlgItemMessage(hDlg,IDC_PRTFOOTER,EM_LIMITTEXT,255,0);
		SendDlgItemMessage(hDlg,IDC_PRTBKTEXT,CB_LIMITTEXT,63,0);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_PRTFONTSCREEN:
		case IDC_PRTFONTPRINT:
			AdjustDlgUI(7,hDlg);
			PropSheet_Changed(GetParent(hDlg),hDlg);
			return TRUE;
		case IDC_PRTFONTFACE:
			AdjustDlgUI(7,hDlg);
		case IDC_PRTFONTSIZE:
		case IDC_PRTWRAP:
		case IDC_PRTBKTEXT:
		case IDC_PRTHEADERWIDTH:
		case IDC_PRTFOOTERWIDTH:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
			case CBN_EDITCHANGE:
				PropSheet_Changed(GetParent(hDlg),hDlg);
				break;
			}
			return TRUE;
		case IDC_PRTLINENUM:
		case IDC_PRTSYSHEADER:
		case IDC_PRTDIRECT:
		case IDC_PRTPARSE:
		case IDC_PRTLEFTSPIN:
		case IDC_PRTTOPSPIN:
		case IDC_PRTRIGHTSPIN:
		case IDC_PRTBOTTOMSPIN:
			PropSheet_Changed(GetParent(hDlg),hDlg);
			return TRUE;
		case IDC_PRTHEADER:
		case IDC_PRTFOOTER:
		case IDC_PRTLEFT:
		case IDC_PRTTOP:
		case IDC_PRTRIGHT:
		case IDC_PRTBOTTOM:
			switch (HIWORD(wParam)) {
			case EN_CHANGE:
				if (bEditByCode==FALSE) {
					PropSheet_Changed(GetParent(hDlg),hDlg);
				}
				break;
			}
			return TRUE;
		case IDC_PRTBTNHEADER:
		case IDC_PRTBTNFOOTER:
			if (LOWORD(wParam)==IDC_PRTBTNHEADER) {
				PopupID=IDC_PRTHEADER;
			} else {
				PopupID=IDC_PRTFOOTER;
			}
			hMenu=LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_POPUP));
			hPopup=GetSubMenu(hMenu, 3);

			GetCursorPos(&pt);
			TrackPopupMenu(hPopup, TPM_LEFTALIGN, pt.x, pt.y,
				0, hDlg, NULL);
			DestroyMenu(hMenu);
			return TRUE;
		case IDM_PRINT_FILE:
			SendDlgItemMessage(hDlg,PopupID,EM_REPLACESEL,TRUE,(LPARAM)"[파일]");
			SetFocus(GetDlgItem(hDlg,PopupID));
			return TRUE;
		case IDM_PRINT_PATH:
			SendDlgItemMessage(hDlg,PopupID,EM_REPLACESEL,TRUE,(LPARAM)"[경로]");
			SetFocus(GetDlgItem(hDlg,PopupID));
			return TRUE;
		case IDM_PRINT_DATE:
			SendDlgItemMessage(hDlg,PopupID,EM_REPLACESEL,TRUE,(LPARAM)"[날짜]");
			SetFocus(GetDlgItem(hDlg,PopupID));
			return TRUE;
		case IDM_PRINT_TIME:
			SendDlgItemMessage(hDlg,PopupID,EM_REPLACESEL,TRUE,(LPARAM)"[시간]");
			SetFocus(GetDlgItem(hDlg,PopupID));
			return TRUE;
		case IDM_PRINT_PAGE:
			SendDlgItemMessage(hDlg,PopupID,EM_REPLACESEL,TRUE,(LPARAM)"[쪽]");
			SetFocus(GetDlgItem(hDlg,PopupID));
			return TRUE;
		case IDM_PRINT_TOTALPAGE:
			SendDlgItemMessage(hDlg,PopupID,EM_REPLACESEL,TRUE,(LPARAM)"[전체쪽]");
			SetFocus(GetDlgItem(hDlg,PopupID));
			return TRUE;
		case IDM_PRINT_CENTER:
			GetDlgItemText(hDlg,PopupID,szTemp,255);
			if (strstr(szTemp,"[가운데]") != 0) {
				MessageBox(hDlg, "가운데 출력 예약어는 한 번밖에 쓸 수 없습니다","알림",MB_OK);
			} else {
				SendDlgItemMessage(hDlg,PopupID,EM_REPLACESEL,TRUE,(LPARAM)"[가운데]");
				SetFocus(GetDlgItem(hDlg,PopupID));
			}
			return TRUE;
		case IDM_PRINT_RIGHT:
			GetDlgItemText(hDlg,PopupID,szTemp,255);
			if (strstr(szTemp,"[오른쪽]") != 0) {
				MessageBox(hDlg, "오른쪽 출력 예약어는 한 번밖에 쓸 수 없습니다","알림",MB_OK);
			} else {
				SendDlgItemMessage(hDlg,PopupID,EM_REPLACESEL,TRUE,(LPARAM)"[오른쪽]");
				SetFocus(GetDlgItem(hDlg,PopupID));
			}
			return TRUE;
		}
		break;
	case WM_NOTIFY:
		LPNMHDR hdr;
		hdr=(LPNMHDR)lParam;
		switch (hdr->code) {
		case PSN_SETACTIVE:
			g_StartPage=7;
			bEditByCode=TRUE;
			SetDlgItemInt(hDlg,IDC_PRTLEFT,NewOption.prtMargin.left,FALSE);
			SetDlgItemInt(hDlg,IDC_PRTTOP,NewOption.prtMargin.top,FALSE);
			SetDlgItemInt(hDlg,IDC_PRTRIGHT,NewOption.prtMargin.right,FALSE);
			SetDlgItemInt(hDlg,IDC_PRTBOTTOM,NewOption.prtMargin.bottom,FALSE);

			if (NewOption.prtFont.lfHeight==-1) {
				CheckRadioButton(hDlg,IDC_PRTFONTSCREEN,IDC_PRTFONTPRINT,IDC_PRTFONTSCREEN);
				SetDlgItemInt(hDlg,IDC_PRTFONTSIZE,10,FALSE);
				SendDlgItemMessage(hDlg,IDC_PRTFONTFACE,CB_SETCURSEL,0,0);
			} else {
				CheckRadioButton(hDlg,IDC_PRTFONTSCREEN,IDC_PRTFONTPRINT,IDC_PRTFONTPRINT);
				if (NewOption.prtFont.lfHeight == 0) {
					idx=0;
					SetDlgItemInt(hDlg,IDC_PRTFONTSIZE,10,FALSE);
				} else {
					idx=SendDlgItemMessage(hDlg,IDC_PRTFONTFACE,CB_FINDSTRING,-1,
						(LPARAM)NewOption.prtFont.lfFaceName);
					SetDlgItemInt(hDlg,IDC_PRTFONTSIZE,NewOption.prtFont.lfHeight,FALSE);
				}
				SendDlgItemMessage(hDlg,IDC_PRTFONTFACE,CB_SETCURSEL,idx,0);
			}

			SetDlgItemText(hDlg,IDC_PRTHEADER,NewOption.prtHeader);
			SetDlgItemText(hDlg,IDC_PRTFOOTER,NewOption.prtFooter);
			SetDlgItemText(hDlg,IDC_PRTBKTEXT,NewOption.prtBkText);

			SendDlgItemMessage(hDlg,IDC_PRTHEADERWIDTH,CB_SETCURSEL,NewOption.prtnHeader,0);
			SendDlgItemMessage(hDlg,IDC_PRTFOOTERWIDTH,CB_SETCURSEL,NewOption.prtnFooter,0);
			SendDlgItemMessage(hDlg,IDC_PRTWRAP,CB_SETCURSEL,NewOption.prtnWrap,0);

			CheckDlgButton(hDlg,IDC_PRTPARSE,NewOption.prtbParse);
			CheckDlgButton(hDlg,IDC_PRTSYSHEADER,NewOption.prtbSysHeader);
			CheckDlgButton(hDlg,IDC_PRTLINENUM,NewOption.prtbLineNum);
			CheckDlgButton(hDlg,IDC_PRTDIRECT,NewOption.prtbDirect);

			AdjustDlgUI(7,hDlg);
			bEditByCode=FALSE;
			return TRUE;
		case PSN_APPLY:
			ApplyNow();
			return TRUE;
		case PSN_KILLACTIVE:
			t1=GetDlgItemInt(hDlg,IDC_PRTLEFT,NULL,FALSE);
			t2=GetDlgItemInt(hDlg,IDC_PRTTOP,NULL,FALSE);
			t3=GetDlgItemInt(hDlg,IDC_PRTRIGHT,NULL,FALSE);
			t4=GetDlgItemInt(hDlg,IDC_PRTBOTTOM,NULL,FALSE);
			if (t1<0 || t1>100 || t2<0 || t2>100 || t3<0 || t3>100 || t4<0 || t4>100) {
				MessageBox(hDlg,"여백은 0~100사이여야 합니다.","알림",MB_OK);
				SetWindowLongPtr(hDlg,DWLP_MSGRESULT,TRUE);
				return TRUE;
			}

			i=GetDlgItemInt(hDlg,IDC_PRTFONTSIZE,NULL,FALSE);
			if (i < 6 || i > 72) {
				MessageBox(hDlg,"글꼴의 크기는 6~72사이의 값을 지정해야 합니다","알림",MB_OK);
				SetWindowLongPtr(hDlg,DWLP_MSGRESULT,TRUE);
				return TRUE;
			}

			NewOption.prtMargin.left=t1;
			NewOption.prtMargin.top=t2;
			NewOption.prtMargin.right=t3;
			NewOption.prtMargin.bottom=t4;

			if (IsDlgButtonChecked(hDlg,IDC_PRTFONTSCREEN)) {
				NewOption.prtFont.lfHeight=-1;
			} else {
				idx=SendDlgItemMessage(hDlg,IDC_PRTFONTFACE,CB_GETCURSEL,0,0);
				if (idx==0) {
					NewOption.prtFont.lfHeight=0;
				} else {
					SendDlgItemMessage(hDlg,IDC_PRTFONTFACE,CB_GETLBTEXT,idx,(LPARAM)szFace);
					for (i=0;i<FontNum;i++) {
						if (lstrcmp(szFace,arFont[i].lfFaceName) == 0)
							break;
					}
					NewOption.prtFont=arFont[i];
					NewOption.prtFont.lfHeight=GetDlgItemInt(hDlg,IDC_PRTFONTSIZE,NULL,FALSE);
				}
			}

			GetDlgItemText(hDlg,IDC_PRTHEADER,NewOption.prtHeader,256);
			GetDlgItemText(hDlg,IDC_PRTFOOTER,NewOption.prtFooter,256);
			NewOption.prtnHeader=SendDlgItemMessage(hDlg,IDC_PRTHEADERWIDTH,CB_GETCURSEL,0,0);
			NewOption.prtnFooter=SendDlgItemMessage(hDlg,IDC_PRTFOOTERWIDTH,CB_GETCURSEL,0,0);

			NewOption.prtbParse=IsDlgButtonChecked(hDlg,IDC_PRTPARSE);
			NewOption.prtbLineNum=IsDlgButtonChecked(hDlg,IDC_PRTLINENUM);
			NewOption.prtbSysHeader=IsDlgButtonChecked(hDlg,IDC_PRTSYSHEADER);
			NewOption.prtbDirect=IsDlgButtonChecked(hDlg,IDC_PRTDIRECT);

			GetDlgItemText(hDlg,IDC_PRTBKTEXT,NewOption.prtBkText,64);
			NewOption.prtnWrap=SendDlgItemMessage(hDlg,IDC_PRTWRAP,CB_GETCURSEL,0,0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK FileDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HWND hCon;
	int i;
	TCHAR szTemp[MAX_PATH];
	TCHAR Folder[MAX_PATH];
	TCHAR Drive[_MAX_DRIVE];
	TCHAR Dir[_MAX_DIR];

	switch(iMessage) {
	case WM_INITDIALOG:
		hCon=GetDlgItem(hDlg,IDC_BACKUPLEVEL);
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"0(백업하지 않음)");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"1(한 단계만 백업)");
		for (i=2;i<32;i++) {
			wsprintf(szTemp,"%d단계 백업",i);
			SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)szTemp);
		}

		hCon=GetDlgItem(hDlg,IDC_BACKUPNAMING);
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"문서.txt -> 문서.bak");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"문서.txt -> 문서_bak.txt");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"문서.txt -> 문서_백업.txt");
		SendMessage(hCon,CB_ADDSTRING,0,(LPARAM)"문서.txt -> 문서.txt.bak");

		SendDlgItemMessage(hDlg,IDC_SPINAUTOSAVE,UDM_SETRANGE,0,MAKELPARAM(1440,0));
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))	{
		case IDC_BRBACKUPFOLDER:
			GetWindowText(GetDlgItem(hDlg,IDC_BACKUPFOLDER),szTemp,MAX_PATH);
			if (BrowseFolder(hDlg,"백업 파일을 저장할 폴더를 선택하십시오.",
				szTemp,Folder)==TRUE) {
				SetDlgItemText(hDlg,IDC_BACKUPFOLDER,Folder);
			}
			PropSheet_Changed(GetParent(hDlg),hDlg);
			break;
		case IDC_BRDGBACKUP:
			GetModuleFileName(g_hInst,Folder,MAX_PATH);
			_splitpath(Folder,Drive,Dir,NULL,NULL);
			wsprintf(Folder,"%s%sBackUp",Drive,Dir);
			if (_access(Folder,0) != 0) {
				if (MessageBox(hDlg,"이 폴더를 지금 만드시겠습니까?",
					"질문",MB_YESNO)==IDNO)
					break;
				CreateDirectory(Folder,NULL);
			}
			SetDlgItemText(hDlg,IDC_BACKUPFOLDER,Folder);
			PropSheet_Changed(GetParent(hDlg),hDlg);
			break;
		case IDC_BREDITINFOFOLDER:
			GetWindowText(GetDlgItem(hDlg,IDC_EDITINFOFOLDER),szTemp,MAX_PATH);
			if (BrowseFolder(hDlg,"편집 정보 파일을 저장할 폴더를 선택하십시오.",
				szTemp,Folder)==TRUE) {
				SetDlgItemText(hDlg,IDC_EDITINFOFOLDER,Folder);
			}
			PropSheet_Changed(GetParent(hDlg),hDlg);
			break;
		case IDC_BRDGEDITINFO:
			GetModuleFileName(g_hInst,Folder,MAX_PATH);
			_splitpath(Folder,Drive,Dir,NULL,NULL);
			wsprintf(Folder,"%s%sEditInfo",Drive,Dir);
			if (_access(Folder,0) != 0) {
				if (MessageBox(hDlg,"이 폴더를 지금 만드시겠습니까?",
					"질문",MB_YESNO)==IDNO)
					break;
				CreateDirectory(Folder,NULL);
			}
			SetDlgItemText(hDlg,IDC_EDITINFOFOLDER,Folder);
			PropSheet_Changed(GetParent(hDlg),hDlg);
			break;
		case IDC_MAKEEDITINFO:
			AdjustDlgUI(8,hDlg);
		case IDC_UPDATEEDITINFO:
		case IDC_EIOFFSET:
		case IDC_EISELECTION:
		case IDC_EIBOOKMARK:
		case IDC_EIUNDORECORD:
		case IDC_EISETTING:
		case IDC_USEEDITINFO:
			PropSheet_Changed(GetParent(hDlg),hDlg);
			break;
		}
		return TRUE;
	case WM_NOTIFY:
		LPNMHDR hdr;
		hdr=(LPNMHDR)lParam;
		switch (hdr->code) {
		case PSN_SETACTIVE:
			g_StartPage=8;
			SendDlgItemMessage(hDlg,IDC_BACKUPLEVEL,CB_SETCURSEL,NewOption.BackupLevel,0);
			SendDlgItemMessage(hDlg,IDC_BACKUPNAMING,CB_SETCURSEL,NewOption.BackupNaming,0);
			SetDlgItemText(hDlg,IDC_BACKUPFOLDER,NewOption.BackupFolder);
			SetDlgItemInt(hDlg,IDC_AUTOSAVE,NewOption.AutoSaveInterval,FALSE);
			CheckDlgButton(hDlg,IDC_MAKEEDITINFO,NewOption.bMakeEditInfo);
			CheckDlgButton(hDlg,IDC_UPDATEEDITINFO,NewOption.bUpdateEditInfo);
			CheckDlgButton(hDlg,IDC_USEEDITINFO,NewOption.bUseEditInfo);
			CheckDlgButton(hDlg,IDC_EIOFFSET,NewOption.EditInfoMask & EI_OFFSET);
			CheckDlgButton(hDlg,IDC_EISELECTION,NewOption.EditInfoMask & EI_SELECTION);
			CheckDlgButton(hDlg,IDC_EIBOOKMARK,NewOption.EditInfoMask & EI_BOOKMARK);
			CheckDlgButton(hDlg,IDC_EIUNDORECORD,NewOption.EditInfoMask & EI_UNDORECORD);
			CheckDlgButton(hDlg,IDC_EISETTING,NewOption.EditInfoMask & EI_SETTING);
			SetDlgItemText(hDlg,IDC_EDITINFOFOLDER,NewOption.EditInfoFolder);
			AdjustDlgUI(8,hDlg);
			return TRUE;
		case PSN_APPLY:
			ApplyNow();
			return TRUE;
		case PSN_KILLACTIVE:
			NewOption.BackupLevel=SendDlgItemMessage(hDlg,IDC_BACKUPLEVEL,CB_GETCURSEL,0,0);
			NewOption.BackupNaming=SendDlgItemMessage(hDlg,IDC_BACKUPNAMING,CB_GETCURSEL,0,0);
			GetDlgItemText(hDlg,IDC_BACKUPFOLDER,NewOption.BackupFolder,MAX_PATH);
			NewOption.AutoSaveInterval=GetDlgItemInt(hDlg,IDC_AUTOSAVE,NULL,FALSE);
			NewOption.bMakeEditInfo=IsDlgButtonChecked(hDlg,IDC_MAKEEDITINFO);
			NewOption.bUpdateEditInfo=IsDlgButtonChecked(hDlg,IDC_UPDATEEDITINFO);
			NewOption.bUseEditInfo=IsDlgButtonChecked(hDlg,IDC_USEEDITINFO);
			NewOption.EditInfoMask=0;
			if (IsDlgButtonChecked(hDlg,IDC_EIOFFSET)) NewOption.EditInfoMask |= EI_OFFSET;
			if (IsDlgButtonChecked(hDlg,IDC_EISELECTION)) NewOption.EditInfoMask |= EI_SELECTION;
			if (IsDlgButtonChecked(hDlg,IDC_EIBOOKMARK)) NewOption.EditInfoMask |= EI_BOOKMARK;
			if (IsDlgButtonChecked(hDlg,IDC_EIUNDORECORD)) NewOption.EditInfoMask |= EI_UNDORECORD;
			if (IsDlgButtonChecked(hDlg,IDC_EISETTING)) NewOption.EditInfoMask |= EI_SETTING;
			GetDlgItemText(hDlg,IDC_EDITINFOFOLDER,NewOption.EditInfoFolder,MAX_PATH);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK AssociateDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	int i;
	FILETIME FileTime;
	TCHAR szTemp[MAX_PATH];
	TCHAR lpSubKey[MAX_PATH];
	LONG Result;
	HKEY key;
	DWORD Size;

	switch(iMessage) {
	case WM_INITDIALOG:
		RegOpenKeyEx(SHCR,NULL,0,KEY_READ,&key);
		Result=ERROR_SUCCESS;
		for (i=0;Result==ERROR_SUCCESS;i++) {
			Size=MAX_PATH;
			Result=RegEnumKeyEx(key,i,lpSubKey,&Size,NULL,NULL,NULL,&FileTime);
			if (Result==ERROR_SUCCESS) {
				if (lpSubKey[0]=='.') {
					SendDlgItemMessage(hDlg,IDC_EXTENSIONS,CB_ADDSTRING,0,(LPARAM)lpSubKey);
				}
			}
		}
		Result=ERROR_SUCCESS;
		for (i=0;Result==ERROR_SUCCESS;i++) {
			Size=MAX_PATH;
			Result=RegEnumKeyEx(key,i,lpSubKey,&Size,NULL,NULL,NULL,&FileTime);
			if (Result==ERROR_SUCCESS) {
				if (strncmp(lpSubKey,"Dangeun.",8)==0) {
					SendDlgItemMessage(hDlg,IDC_ASSOEXTENSION,LB_ADDSTRING,0,(LPARAM)lpSubKey+7);
				}
			}
		}
		RegCloseKey(key);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))	{
		case IDC_EXTENSIONS:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				i=SendDlgItemMessage(hDlg,IDC_EXTENSIONS,CB_GETCURSEL,0,0);
				SendDlgItemMessage(hDlg,IDC_EXTENSIONS,CB_GETLBTEXT,i,(LPARAM)lpSubKey);
				SHRegReadString(SHCR,lpSubKey,NULL,"",szTemp,MAX_PATH);
				SetDlgItemText(hDlg,IDC_EXTDESC,szTemp);
				break;
			}
			break;
		case IDC_MAKEASSOCIATE:
			GetDlgItemText(hDlg,IDC_EXTENSIONS,lpSubKey,MAX_PATH);
			GetDlgItemText(hDlg,IDC_EXTDESC,szTemp,MAX_PATH);
			if (lpSubKey[0]=='.') {
				MakeAssociate(lpSubKey,szTemp);
				SendDlgItemMessage(hDlg,IDC_ASSOEXTENSION,LB_ADDSTRING,0,(LPARAM)lpSubKey);
			}
			break;
		case IDC_UNASSOCIATE:
			i=SendDlgItemMessage(hDlg,IDC_ASSOEXTENSION,LB_GETCURSEL,0,0);
			if (i != -1) {
				SendDlgItemMessage(hDlg,IDC_ASSOEXTENSION,LB_GETTEXT,i,(LPARAM)szTemp);
				UnAssociate(szTemp);
				SendDlgItemMessage(hDlg,IDC_ASSOEXTENSION,LB_DELETESTRING,i,0);
			}
			break;
		}
		return TRUE;
	case WM_NOTIFY:
		LPNMHDR hdr;
		hdr=(LPNMHDR)lParam;
		switch (hdr->code) {
		case PSN_SETACTIVE:
			g_StartPage=9;
			return TRUE;
		case PSN_APPLY:
			ApplyNow();
			return TRUE;
		case PSN_KILLACTIVE:
			return TRUE;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK OperationDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HWND hCon[4];
	int i;

	switch(iMessage) {
	case WM_INITDIALOG:
		hCon[0]=GetDlgItem(hDlg,IDC_WHEEL);
		hCon[1]=GetDlgItem(hDlg,IDC_CTRLWHEEL);
		hCon[2]=GetDlgItem(hDlg,IDC_SHIFTWHEEL);
		hCon[3]=GetDlgItem(hDlg,IDC_CTRLSHIFTWHEEL);
		for (i=0;i<4;i++) {
			SendMessage(hCon[i],CB_ADDSTRING,0,(LPARAM)"동작 없음");
			SendMessage(hCon[i],CB_ADDSTRING,0,(LPARAM)"상하 스크롤");
			SendMessage(hCon[i],CB_ADDSTRING,0,(LPARAM)"한줄씩 스크롤");
			SendMessage(hCon[i],CB_ADDSTRING,0,(LPARAM)"페이지 단위 스크롤");
			SendMessage(hCon[i],CB_ADDSTRING,0,(LPARAM)"반 페이지 단위 스크롤");
			SendMessage(hCon[i],CB_ADDSTRING,0,(LPARAM)"줄간 조정");
			SendMessage(hCon[i],CB_ADDSTRING,0,(LPARAM)"글꼴 크기 조정");
			SendMessage(hCon[i],CB_ADDSTRING,0,(LPARAM)"편집 취소/재실행");
		}
		hCon[0]=GetDlgItem(hDlg,IDC_CBCURSOREDIT);
		SendMessage(hCon[0],CB_ADDSTRING,0,(LPARAM)"처리하지 않음");
		SendMessage(hCon[0],CB_ADDSTRING,0,(LPARAM)"숨기기");
		SendMessage(hCon[0],CB_ADDSTRING,0,(LPARAM)"오른쪽 위로 추방");
		SendMessage(hCon[0],CB_ADDSTRING,0,(LPARAM)"오른쪽 아래로 추방");
		SendMessage(hCon[0],CB_ADDSTRING,0,(LPARAM)"캐럿 따라 다니기");
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))	{
		case IDC_WHEEL:
		case IDC_CTRLWHEEL:
		case IDC_SHIFTWHEEL:
		case IDC_CTRLSHIFTWHEEL:
		case IDC_CBCURSOREDIT:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				PropSheet_Changed(GetParent(hDlg),hDlg);
				return TRUE;
			}
			break;
		case IDC_USEAUTOCLIP:
			AdjustDlgUI(10,hDlg);
		case IDC_WORDWRAP:
		case IDC_HANGUL:
		case IDC_AUTOCLIPBYTAB:
		case IDC_PREVIEWCLIP:
		case IDC_CHKSMARTSPACE:
			PropSheet_Changed(GetParent(hDlg),hDlg);
			break;
		}
		return TRUE;
	case WM_NOTIFY:
		LPNMHDR hdr;
		hdr=(LPNMHDR)lParam;
		switch (hdr->code) {
		case PSN_SETACTIVE:
			g_StartPage=10;
			SendDlgItemMessage(hDlg,IDC_WHEEL,CB_SETCURSEL,NewOption.nWheel,0);
			SendDlgItemMessage(hDlg,IDC_CTRLWHEEL,CB_SETCURSEL,NewOption.nCtrlWheel,0);
			SendDlgItemMessage(hDlg,IDC_SHIFTWHEEL,CB_SETCURSEL,NewOption.nShiftWheel,0);
			SendDlgItemMessage(hDlg,IDC_CTRLSHIFTWHEEL,CB_SETCURSEL,NewOption.nCtrlShiftWheel,0);
			CheckDlgButton(hDlg,IDC_WORDWRAP,NewOption.bWrap);
			CheckDlgButton(hDlg,IDC_HANGUL,NewOption.Hangul);
			CheckDlgButton(hDlg,IDC_AUTOCONVERT,NewOption.TestStyle(AES_AUTOCONVERT));
			CheckDlgButton(hDlg,IDC_PREVIEWCLIP,NewOption.bPreviewClip);
			CheckDlgButton(hDlg,IDC_USEAUTOCLIP,NewOption.TestStyle(AES_USEAUTOCLIP));
			CheckDlgButton(hDlg,IDC_AUTOCLIPBYTAB,NewOption.TestStyle(AES_AUTOCLIPBYTAB));
			SendDlgItemMessage(hDlg,IDC_CBCURSOREDIT,CB_SETCURSEL,NewOption.CursorEdit,0);
			CheckDlgButton(hDlg,IDC_CHKSMARTSPACE,NewOption.TestStyle(AES_SMARTSPACE));
			AdjustDlgUI(10,hDlg);
			return TRUE;
		case PSN_APPLY:
			ApplyNow();
			return TRUE;
		case PSN_KILLACTIVE:
			NewOption.nWheel=SendDlgItemMessage(hDlg,IDC_WHEEL,CB_GETCURSEL,0,0);
			NewOption.nCtrlWheel=SendDlgItemMessage(hDlg,IDC_CTRLWHEEL,CB_GETCURSEL,0,0);
			NewOption.nShiftWheel=SendDlgItemMessage(hDlg,IDC_SHIFTWHEEL,CB_GETCURSEL,0,0);
			NewOption.nCtrlShiftWheel=SendDlgItemMessage(hDlg,IDC_CTRLSHIFTWHEEL,CB_GETCURSEL,0,0);
			NewOption.bWrap=IsDlgButtonChecked(hDlg,IDC_WORDWRAP);
			NewOption.Hangul=IsDlgButtonChecked(hDlg,IDC_HANGUL);
			NewOption.ChangeStyle(AES_AUTOCONVERT,IsDlgButtonChecked(hDlg,IDC_AUTOCONVERT));
			NewOption.bPreviewClip=IsDlgButtonChecked(hDlg,IDC_PREVIEWCLIP);
			NewOption.ChangeStyle(AES_USEAUTOCLIP,IsDlgButtonChecked(hDlg,IDC_USEAUTOCLIP));
			NewOption.ChangeStyle(AES_AUTOCLIPBYTAB,IsDlgButtonChecked(hDlg,IDC_AUTOCLIPBYTAB));
			NewOption.CursorEdit=SendDlgItemMessage(hDlg,IDC_CBCURSOREDIT,CB_GETCURSEL,0,0);
			NewOption.ChangeStyle(AES_SMARTSPACE,IsDlgButtonChecked(hDlg,IDC_CHKSMARTSPACE));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

void AdjustDlgUI(int Page,HWND hDlg)
{
	switch (Page) {
	case 0:
		if (IsDlgButtonChecked(hDlg,IDC_WATCHCHANGE)) {
			EnableWindow(GetDlgItem(hDlg,IDC_RELOADNOASK),TRUE);
		} else {
			EnableWindow(GetDlgItem(hDlg,IDC_RELOADNOASK),FALSE);
		}
		break;
	case 1:
		if (SendDlgItemMessage(hDlg,IDC_FONTFACE,CB_GETCURSEL,0,0)==0) {
			EnableWindow(GetDlgItem(hDlg,IDC_FONTSIZE),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_EDCHARRATIO),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_SPINCHARRATIO),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_CHKBOLD),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_CHKITALIC),FALSE);
		} else {
			EnableWindow(GetDlgItem(hDlg,IDC_FONTSIZE),TRUE);
			EnableWindow(GetDlgItem(hDlg,IDC_EDCHARRATIO),TRUE);
			EnableWindow(GetDlgItem(hDlg,IDC_SPINCHARRATIO),TRUE);
			EnableWindow(GetDlgItem(hDlg,IDC_CHKBOLD),TRUE);
			EnableWindow(GetDlgItem(hDlg,IDC_CHKITALIC),TRUE);
		}
		break;
	case 2:
		if (IsDlgButtonChecked(hDlg,IDC_MARGIN)) {
			EnableWindow(GetDlgItem(hDlg,IDC_LINENUM),TRUE);
		} else {
			EnableWindow(GetDlgItem(hDlg,IDC_LINENUM),FALSE);
		}
		break;
	case 7:
		if (IsDlgButtonChecked(hDlg,IDC_PRTFONTSCREEN)) {
			EnableWindow(GetDlgItem(hDlg,IDC_PRTFONTFACE),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_PRTFONTSIZE),FALSE);
		} else {
			EnableWindow(GetDlgItem(hDlg,IDC_PRTFONTFACE),TRUE);
			if (SendDlgItemMessage(hDlg,IDC_PRTFONTFACE,CB_GETCURSEL,0,0)==0) {
				EnableWindow(GetDlgItem(hDlg,IDC_PRTFONTSIZE),FALSE);
			} else {
				EnableWindow(GetDlgItem(hDlg,IDC_PRTFONTSIZE),TRUE);
			}
		}
		break;
	case 8:
		if (IsDlgButtonChecked(hDlg,IDC_MAKEEDITINFO) == BST_CHECKED) {
			EnableWindow(GetDlgItem(hDlg,IDC_UPDATEEDITINFO),TRUE);
			EnableWindow(GetDlgItem(hDlg,IDC_EIOFFSET),TRUE);
			EnableWindow(GetDlgItem(hDlg,IDC_EISELECTION),TRUE);
			EnableWindow(GetDlgItem(hDlg,IDC_EIBOOKMARK),TRUE);
			EnableWindow(GetDlgItem(hDlg,IDC_EIUNDORECORD),TRUE);
			EnableWindow(GetDlgItem(hDlg,IDC_EISETTING),TRUE);
		} else {
			EnableWindow(GetDlgItem(hDlg,IDC_UPDATEEDITINFO),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_EIOFFSET),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_EISELECTION),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_EIBOOKMARK),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_EIUNDORECORD),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_EISETTING),FALSE);
		}
		break;
	case 10:
		if (IsDlgButtonChecked(hDlg,IDC_USEAUTOCLIP) == BST_CHECKED) {
			EnableWindow(GetDlgItem(hDlg,IDC_AUTOCLIPBYTAB),TRUE);
		} else {
			EnableWindow(GetDlgItem(hDlg,IDC_AUTOCLIPBYTAB),FALSE);
		}
		break;
	}
}

void ApplyNow()
{
	HWND hChild;
	SInfo *pSi;
	HDC hdc;
	LOGFONT tFont;
	BOOL bChangeStyle;
	BOOL bRelayout=FALSE;
	BOOL bChangeBackBit=FALSE;
	BOOL bChangeStatus=FALSE;
	int i;

	if (memcmp(Option.arStyle,NewOption.arStyle,sizeof(NewOption.arStyle))!=0) {
		bChangeStyle=TRUE;
	} else {
		bChangeStyle=FALSE;
	}

	if (memcmp(Option.StMask,NewOption.StMask,sizeof(SStMask)*STMAX)!=0) {
		bChangeStatus=TRUE;
	}

	if (Option.bAllowMulti != NewOption.bAllowMulti) {
		SHRegWriteInt(SHCU,KEY"Setting","bAllowMulti",NewOption.bAllowMulti);
	}

	if (Option.bExplorerPopup != NewOption.bExplorerPopup) {
		RegisterPopup(NewOption.bExplorerPopup);
	}

	if (Option.MaxMru != NewOption.MaxMru) {
		Mru.ChangeMaxMru(NewOption.GetMaxMru());
	}

	if (Option.bToolBarBig != NewOption.bToolBarBig ||
		Option.bToolBarText != NewOption.bToolBarText) {
		CreateToolBar(NewOption.bToolBarText,NewOption.bToolBarBig);
		bRelayout=TRUE;
		InvalidateRect(hFileWnd,NULL,TRUE);
	}

	if (Option.bShowFileTab != NewOption.bShowFileTab ||
		Option.bShowFileWnd != NewOption.bShowFileWnd ||
		Option.bShowStatus != NewOption.bShowStatus ||
		Option.bShowToolBar != NewOption.bShowToolBar ||
		Option.bShowClipWnd != NewOption.bShowClipWnd) {
		bRelayout=TRUE;
	}

	if (Option.AutoSaveInterval != NewOption.AutoSaveInterval) {
		if (Option.AutoSaveInterval) {
			SetTimer(g_hFrameWnd,1,Option.AutoSaveInterval*1000*60,NULL);
		} else {
			KillTimer(g_hFrameWnd,1);
		}
	}

	if (lstrcmpi(Option.BackBit,NewOption.BackBit)!=0) {
		if (hBackBit) {
			DeleteObject(hBackBit);
			hBackBit=NULL;
		}
		if (IsStrEmpty(NewOption.BackBit)==FALSE) {
			if (strncmp(NewOption.BackBit,"샘플:",5) == 0) {
				if (strcmp(NewOption.BackBit+5,"국화")==0) 
					hBackBit=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_SAMPLE1));
				if (strcmp(NewOption.BackBit+5,"콩")==0) 
					hBackBit=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_SAMPLE2));
				if (strcmp(NewOption.BackBit+5,"아가")==0) 
					hBackBit=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_SAMPLE3));
			} else {
				hBackBit=MakeDDBFromDIB(NewOption.BackBit);
			}
		} 
		bChangeBackBit=TRUE;
	}

	hChild=GetWindow(g_hMDIClient,GW_CHILD);
	while (hChild) {
		pSi=(SInfo *)GetWindowLongPtr(hChild,0);

		if (Option.RightWrap != NewOption.RightWrap) {
			pSi->Ae.SetRightWrap(NewOption.RightWrap);
		}

		if (Option.ColMark != NewOption.ColMark) {
			pSi->Ae.SetColMark(NewOption.ColMark);
		}

		if (Option.LineRatio != NewOption.LineRatio) {
			pSi->Ae.SetLineRatio(NewOption.LineRatio);
		}

		if (Option.nShowCurLine != NewOption.nShowCurLine) {
			pSi->Ae.SetShowCurLine(NewOption.nShowCurLine);
		}

		if (Option.HideSelType != NewOption.HideSelType) {
			pSi->Ae.SetHideSelType(NewOption.HideSelType);
		}

		if (lstrcmp(Option.logfont.lfFaceName,NewOption.logfont.lfFaceName) != 0 ||
			Option.logfont.lfHeight != NewOption.logfont.lfHeight ||
			Option.logfont.lfWeight != NewOption.logfont.lfWeight ||
			Option.logfont.lfItalic != NewOption.logfont.lfItalic ) {
			tFont=NewOption.logfont;
			hdc=GetDC(NULL);
			tFont.lfHeight=NewOption.logfont.lfHeight*GetDeviceCaps(hdc,LOGPIXELSY)/72;
			tFont.lfWidth=0;
			ReleaseDC(NULL,hdc);
			pSi->Ae.SetFont(&tFont);
		}

		// 변경된 색상 적용. 상위 1바이트가 0이면 기본 색상이다.
		for (i=0;i<AECOLORS;i++) {
			if (Option.Color[i] != NewOption.Color[i]) {
				pSi->Ae.SetColor(i,NewOption.Color[i] & 0xff000000 ? 
					NewOption.Color[i] & 0xffffff:arSysColor[i]);
			}

		}

		if (Option.CaretWidth != NewOption.CaretWidth) {
			pSi->Ae.SetCaretWidth(NewOption.CaretWidth);
		}

		if (Option.TabWidth != NewOption.TabWidth) {
			pSi->Ae.SetTabWidth(NewOption.TabWidth);
		}

		// 스타일이 변경되었으면 적용한다.
		if (Option.GetStyle() != NewOption.GetStyle()) {
			Option.SetStyle(NewOption.GetStyle());
			pSi->Ae.SetStyle(NewOption.GetStyle());
		}

		if (Option.nWrap != NewOption.nWrap) {
			if (pSi->Ae.GetWrap() != 0) {
				pSi->Ae.SetWrap(NewOption.nWrap);
			}
		}

		if (Option.UndoLimit != NewOption.UndoLimit) {
			pSi->Ae.SetUndoSize(NewOption.GetUndoLimit());
		}

		if (bChangeStyle) {
			for (int aet=0;aet<4 && pSi->Ae.arView[aet];aet++) {
				NewOption.SetStyleColor(pSi->Ae.arView[aet]->GetParser());
			}
			pSi->Ae.Invalidate(-1);
		}

		if (Option.bpl != NewOption.bpl) {
			pSi->Ae.SetBpl(NewOption.GetBpl());
		}

		if (Option.bCapital != NewOption.bCapital) {
			pSi->Ae.SetCapital(NewOption.bCapital);
		}

		if (Option.nHexSep != NewOption.nHexSep) {
			pSi->Ae.SetHexSep(NewOption.GetHexSep());
		}

		if (EqualRect(&Option.InnerRect,&NewOption.InnerRect)==FALSE) {
			pSi->Ae.SetInnerRect(NewOption.InnerRect);
		}

		if (Option.VAlign != NewOption.VAlign) {
			pSi->Ae.SetVAlign(NewOption.VAlign);
		}

		if (Option.SelBlockType != NewOption.SelBlockType) {
			pSi->Ae.SetSelBlockType(NewOption.SelBlockType);
		}

		if (bChangeBackBit) {
			pSi->Ae.SetBackBit(hBackBit);
		}

		if (Option.BackBitPos != NewOption.BackBitPos) {
			pSi->Ae.SetBackBitPos(NewOption.BackBitPos);
		}

		if (Option.nWheel != NewOption.nWheel) {
			pSi->Ae.SetWheel(NewOption.nWheel);
		}

		if (Option.nCtrlWheel != NewOption.nCtrlWheel) {
			pSi->Ae.SetCtrlWheel(NewOption.nCtrlWheel);
		}

		if (Option.nShiftWheel != NewOption.nShiftWheel) {
			pSi->Ae.SetShiftWheel(NewOption.nShiftWheel);
		}

		if (Option.nCtrlShiftWheel != NewOption.nCtrlShiftWheel) {
			pSi->Ae.SetCtrlShiftWheel(NewOption.nCtrlShiftWheel);
		}

		if (Option.CharExtra != NewOption.CharExtra) {
			pSi->Ae.SetCharExtra(NewOption.CharExtra);
		}

		if (Option.WordExtra != NewOption.WordExtra) {
			pSi->Ae.SetWordExtra(NewOption.WordExtra);
		}

		if (Option.CharRatio != NewOption.CharRatio) {
			pSi->Ae.SetCharRatio(NewOption.CharRatio);
		}

		if (Option.CursorEdit != NewOption.CursorEdit) {
			pSi->Ae.SetCursorEdit(NewOption.CursorEdit);
		}

		hChild=GetWindow(hChild,GW_HWNDNEXT);
	}

	Option=NewOption;
	if (bRelayout) {
		Relayout();
	}
	if (bChangeStatus) {
		SetStatusPart();
		SetStatusText(0xffff);
	}
}

void SetSetting(CApiEdit &Ae)
{
	SOption Ori;
	HDC hdc;
	LOGFONT tFont;
	int i;

	if (Option.Hangul) {
		MySetImeMode(Ae.hWnd,TRUE);
	}

	Ae.SetUndoSize(Option.GetUndoLimit());
	
	if (Option.RightWrap != Ori.RightWrap) {
		Ae.SetRightWrap(Option.RightWrap);
	}

	if (Option.ColMark != Ori.ColMark) {
		Ae.SetColMark(Option.ColMark);
	}

	if (Option.LineRatio != Ori.LineRatio) {
		Ae.SetLineRatio(Option.LineRatio);
	}

	if (Option.bWrap == TRUE) {
		if (Option.nWrap != Ae.GetWrap()) {
			Ae.SetWrap(Option.nWrap);
		}
	} else {
		Ae.SetWrap(0);
	}

	if (Option.HideSelType != Ori.HideSelType) {
		Ae.SetHideSelType(Option.HideSelType);
	}

	if (Option.nShowCurLine != Ori.nShowCurLine) {
		Ae.SetShowCurLine(Option.nShowCurLine);
	}

	if (Option.TabWidth != Ori.TabWidth) {
		Ae.SetTabWidth(Option.TabWidth);
	}

	// 컨트롤의 디폴트와 다른 스타일을 적용한다.
	if (Option.GetStyle() != Ori.GetStyle()) {
		Ae.SetStyle(Option.GetStyle());
	}

	if (Option.CaretWidth != Ori.CaretWidth) {
		Ae.SetCaretWidth(Option.CaretWidth);
	}

	if (Option.logfont.lfHeight != 0) {
		tFont=Option.logfont;
		hdc=GetDC(NULL);
		tFont.lfHeight=Option.logfont.lfHeight*GetDeviceCaps(hdc,LOGPIXELSY)/72;
		tFont.lfWidth=0;
		ReleaseDC(NULL,hdc);
		Ae.SetFont(&tFont);
	}

	// 색상 지정. 기본 색이 아니면 색상을 변경한다.
	for (i=0;i<AECOLORS;i++) {
		if (Option.Color[i] & 0xff000000) {
			Ae.SetColor(i,Option.Color[i] & 0xffffff);
		}
	}

	if (EqualRect(&Option.InnerRect,&Ori.InnerRect)==FALSE) {
		Ae.SetInnerRect(Option.InnerRect);
	}

	if (Option.VAlign != Ori.VAlign) {
		Ae.SetVAlign(Option.VAlign);
	}

	if (Option.SelBlockType != Ori.SelBlockType) {
		Ae.SetSelBlockType(Option.SelBlockType);
	}

	if (hBackBit) {
		Ae.SetBackBit(hBackBit);
	}

	if (Option.BackBitPos != Ori.BackBitPos) {
		Ae.SetBackBitPos(Option.BackBitPos);
	}

	if (Option.nWheel != Ori.nWheel) {
		Ae.SetWheel(Option.nWheel);
	}

	if (Option.nCtrlWheel != Ori.nCtrlWheel) {
		Ae.SetCtrlWheel(Option.nCtrlWheel);
	}

	if (Option.nShiftWheel != Ori.nShiftWheel) {
		Ae.SetShiftWheel(Option.nShiftWheel);
	}

	if (Option.nCtrlShiftWheel != Ori.nCtrlShiftWheel) {
		Ae.SetCtrlShiftWheel(Option.nCtrlShiftWheel);
	}

	if (Option.CharExtra != Ori.CharExtra) {
		Ae.SetCharExtra(Option.CharExtra);
	}

	if (Option.WordExtra != Ori.WordExtra) {
		Ae.SetWordExtra(Option.WordExtra);
	}
	
	if (Option.CharRatio != Ori.CharRatio) {
		Ae.SetCharRatio(Option.CharRatio);
	}
	
	if (Option.CursorEdit != Ori.CursorEdit) {
		Ae.SetCursorEdit(Option.CursorEdit);
	}
}

// 기본 뷰가 아닌 파생 뷰의 옵션을 설정한다.
void SetSettingView(CApiEditText *pView)
{
	SOption Ori;

	if (pView->ViewType == HEXA_VIEW) {
		CApiEditHexa *pHexaView=(CApiEditHexa *)pView;
		if (Option.bpl != Ori.bpl) {
			pHexaView->SetBpl(Option.GetBpl());
		}
		if (Option.bCapital != Ori.bCapital) {
			pHexaView->SetCapital(Option.bCapital);
		}
		if (Option.nHexSep != Ori. nHexSep) {
			pHexaView->SetHexSep(Option. nHexSep);
		}
	}
}