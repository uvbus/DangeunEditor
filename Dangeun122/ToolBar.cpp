#include "stdafx.h"

void CreateToolBar(BOOL bText, BOOL bBig)
{
	TBBUTTON ToolBtn[]={
		{0,IDM_FILE_NEW,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
		{1,IDM_FILE_OPEN,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,1},
		{2,IDM_FILE_SAVE,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,2},
		{4,0,0,TBSTYLE_SEP,0,0},
		{3,IDM_EDIT_CUT,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,3},
		{4,IDM_EDIT_COPY,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,4},
		{5,IDM_EDIT_PASTE,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,5},
		{4,0,0,TBSTYLE_SEP,0,0},
		{6,IDM_EDIT_UNDO,TBSTATE_ENABLED,TBSTYLE_DROPDOWN,0,0,0,6},
		{7,IDM_EDIT_REDO,TBSTATE_ENABLED,TBSTYLE_DROPDOWN,0,0,0,7},
		{4,0,0,TBSTYLE_SEP,0,0},
		{8,IDM_VIEW_WORDWRAP,TBSTATE_ENABLED,TBSTYLE_CHECK,0,0,0,8},
		{9,IDM_VIEW_HEXA,TBSTATE_ENABLED,TBSTYLE_CHECK,0,0,0,9},
		{10,IDM_EDIT_COLUMN,TBSTATE_ENABLED,TBSTYLE_CHECK,0,0,0,10},
		{11,IDM_EDIT_COMMODE,TBSTATE_ENABLED,TBSTYLE_CHECK,0,0,0,11},
		{12,IDM_SEARCH_FIND,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,12},
	};
	TCHAR *szToolText="������\0����\0����\0�ڸ���\0����\0���̱�\0���\0�����"
		"\0����\0����\0�����\0���\0ã��\0";
	int bx,by;
	UINT_PTR wID;
	
	if (hToolBar) {
		DestroyWindow(hToolBar);
	}

	if (bBig) {
		wID=IDB_TOOLBARBIG;
		bx=24;
		by=22;
	} else {
		wID=IDB_TOOLBAR;
		bx=16;
		by=15;
	}

	hToolBar=CreateToolbarEx(g_hFrameWnd, WS_CHILD | WS_VISIBLE
		| TBSTYLE_TOOLTIPS | TBSTYLE_FLAT ,
		99, 13, g_hInst, wID, ToolBtn, 16,
		bx,by,bx,by,sizeof(TBBUTTON));

	if (bText) {
		SendMessage(hToolBar,TB_ADDSTRING,NULL,(LPARAM)szToolText);
	}
	SendMessage(hToolBar,TB_AUTOSIZE,0,0);
	SendMessage(hToolBar,TB_SETEXTENDEDSTYLE,0,(LPARAM)TBSTYLE_EX_DRAWDDARROWS);
}

void SetStatusPart()
{
	int SBPart[STMAX];
	RECT crt;
	int i,Num,idx;
	int TotalWidth;

	GetClientRect(g_hFrameWnd,&crt);
	crt.right-=GetSystemMetrics(SM_CYVSCROLL);
	if (g_ChildNum) {
		// ���̴� ��� ��Ʈ�� ���� ����
		for (i=0,TotalWidth=0;i<STNUM;i++) {
			if (Option.StMask[i].bShow) {
				TotalWidth+=Option.StMask[i].Width;
			}
		}
		SBPart[0]=max(200,crt.right-TotalWidth);

		// ������ ���� ������ ����.
		for (i=0,Num=0;i<STNUM;i++) {
			idx=Option.GetStNumByOrder(i);
			if (idx != -1 && Option.StMask[idx].bShow) {
				Num++;
				SBPart[Num]=SBPart[Num-1]+Option.StMask[idx].Width;
			}
		}
		SendMessage(hStatus, SB_SETPARTS, Num+1, (LPARAM)SBPart);
	} else {
		SBPart[0]=crt.right;
		SendMessage(hStatus, SB_SETPARTS, 1, (LPARAM)SBPart);
	}
}

void SetStatusText(int mask,LPCTSTR Mes/*=NULL*/)
{
	TCHAR Text[128];
	HWND hActive;
	SInfo *pSi;
	int byte;
	TCITEM tie;
	int idx;
	int i,stidx,partnum;
	TCHAR AlignText[128];
	SYSTEMTIME st;
	static TCHAR *arYoil[]={"��","��","ȭ","��","��","��","��","��"};
	int s,e;
	HGLOBAL hmem;
	TCHAR *ptr;

	hActive=(HWND)SendMessage(g_hMDIClient,WM_MDIGETACTIVE,0,NULL);
	if (hActive==NULL) {
		return;
	}
	pSi=(SInfo *)GetWindowLong(hActive,0);

	if (mask & 1) {
		if (Mes==NULL) {
			SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)"��� �������Դϴ�");
		} else {
			SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)Mes);
		}
	}

	// �������� �̹����� ���� ���� ���� ���¶��� ���̱�ʹ� ������� ó���Ѵ�.
	if (mask & (1 << (2+1))) {
		tie.mask=TCIF_IMAGE;
		tie.iImage=pSi->Ae.GetModified();
		idx=FindFileTab(pSi->NowFile);
		if (idx!=-1) {
			TabCtrl_SetItem(hFileTab,idx,&tie);
		}
	}

	// 0�� ���¶�. stidx=���� ��ȣ, bitmask=mask���� �ش� ������ ��ġ
	for (stidx=0;stidx<STNUM;stidx++) {
		if (!(mask & (1 << (stidx+1)) && Option.StMask[stidx].bShow)) {
			continue;
		}
		switch (stidx) {
		case 0:
			wsprintf(Text, "%d�� %dĭ",pSi->Ae.GetInfoFromOff(AE_INFO_PARA)+1,
				pSi->Ae.GetInfoFromOff(AE_INFO_PARACOL)+1);
			break;
		case 1:
			if (pSi->Ae.GetFormat() != AE_ENC_BINARY) {
				byte=pSi->Ae.GetTextLength();
				if (byte < 10240) {
					wsprintf(Text, "�� %d�� %d ����Ʈ",
						pSi->Ae.GetDocInfo(AE_DOC_TOTALPARA)+1,byte);
				} else {
					wsprintf(Text, "�� %d�� %d K����Ʈ",
						pSi->Ae.GetDocInfo(AE_DOC_TOTALPARA)+1,byte/1024);
				}
			} else {
				wsprintf(Text, "�� %d��",pSi->Ae.GetDocInfo(AE_DOC_TOTALPARA)+1);
			}
			break;
		case 2:
			if (pSi->Ae.GetModified()) {
				lstrcpy(Text,"�����Ǿ���");
			} else {
				lstrcpy(Text,"������");
			}
			break;
		case 3:
			switch (pSi->Ae.GetFormat() & 0xffff) {
			case AE_ENC_UNKNOWN:
				lstrcpy(Text,"Unknown");
				break;
			case AE_ENC_ANSI:
				lstrcpy(Text,"Ansi");
				break;
			case AE_ENC_UNI:
				lstrcpy(Text,"Unicode");
				break;
			case AE_ENC_UNIBIG:
				lstrcpy(Text,"UniBig");
				break;
			case AE_ENC_UNINOBOM:
				lstrcpy(Text,"UniNB");
				break;
			case AE_ENC_UNIBIGNOBOM:
				lstrcpy(Text,"UniBigNB");
				break;
			case AE_ENC_UTF8:
				lstrcpy(Text,"Utf8");
				break;
			case AE_ENC_UTF8NOBOM:
				lstrcpy(Text,"Utf8NB");
				break;
			case AE_ENC_BINARY:
				lstrcpy(Text,"Binary");
				break;
			}
			switch (pSi->Ae.GetFormat() & 0xffff0000) {
			case AE_ENTER_UNKNOWN:
			case AE_ENTER_CRLF:
				break;
			case AE_ENTER_CR:
				lstrcat(Text,"(CR)");
				break;
			case AE_ENTER_LF:
				lstrcat(Text,"(LF)");
				break;
			}
			break;
		case 4:
			if (pSi->Ae.GetInsMode()) {
				lstrcpy(Text,"��ħ���");
			} else {
				lstrcpy(Text,"���Ը��");
			}
			break;
		case 5:
			if ((GetKeyState(VK_CAPITAL) & 0x1) != 0) {
				lstrcpy(Text,"�빮��");
			} else {
				lstrcpy(Text,"�ҹ���");
			}
			break;
		case 6:
			if (MyGetImeMode(pSi->Ae.hWnd)) {
				lstrcpy(Text,"�ѱ�");
			} else {
				lstrcpy(Text,"����");
			}
			break;
		case 7:
			s=pSi->Ae.GetOffset();
			pSi->Ae.GetText(AlignText,10,s,s+2);
			if (IsDBCSLeadByte(AlignText[0])) {
				e=*(WORD *)AlignText;
			} else {
				e=*(BYTE *)AlignText;
			}
			wsprintf(Text,"%d(%x)",e,e);
			break;
		case 8:
			pSi->Ae.GetSelect(s,e);
			if (s==e) {
				lstrcpy(Text,"���þ���");
			} else {
				wsprintf(Text,"����:%d",e-s);
			}
			break;
		case 9:
			if (IsClipboardFormatAvailable(CF_TEXT)) {
				OpenClipboard(g_hFrameWnd);
				hmem=GetClipboardData(CF_TEXT);
				ptr=(TCHAR *)GlobalLock(hmem);
				lstrcpyn(Text,ptr,64);
				ReplaceCtrlString(Text,lstrlen(Text),NULL,0);
				GlobalUnlock(hmem);
				CloseClipboard();
			} else {
				lstrcpy(Text,"�������");
			}
			break;
		case 10:
			if (pSi->Ae.GetReadOnly()) {
				lstrcpy(Text,"�б�����");
			} else {
				lstrcpy(Text,"���Ⱑ��");
			}
			break;
		case 11:
			GetLocalTime(&st);
			wsprintf(Text,"%d�� %d��",st.wHour,st.wMinute);
			break;
		case 12:
			GetLocalTime(&st);
			wsprintf(Text,"%d�� %d��(%s)",st.wMonth,st.wDay, arYoil[st.wDayOfWeek]);
			break;
		}

		switch (Option.StMask[stidx].Align) {
		case 0:
			lstrcpy(AlignText,"");
			break;
		case 1:
			lstrcpy(AlignText,"\t");
			break;
		case 2:
			lstrcpy(AlignText,"\t\t");
			break;
		}
		lstrcat(AlignText,Text);

		for (i=0,partnum=1;i<STNUM;i++) {
			if (Option.StMask[i].bShow && Option.StMask[stidx].Order > Option.StMask[i].Order)
				partnum++;
		}
		SendMessage(hStatus, SB_SETTEXT, partnum | 
			(Option.StMask[stidx].bPop ? SBT_POPOUT:0), (LPARAM)AlignText);
	}
}

LRESULT CALLBACK DGTabProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	HPEN hPen,OldPen;
	RECT crt;
	LPNMHDR hdr;
	int idx;
	TCITEM tie;
	HWND hChild;

	switch(iMessage) {
	case WM_CREATE:
		hFileTab=CreateWindow(WC_TABCONTROL,"",WS_CHILD | WS_VISIBLE
			|WS_CLIPSIBLINGS | TCS_FOCUSNEVER,
			0,0,0,0,hWnd,NULL,g_hInst,NULL);
		SendMessage(hFileTab,WM_SETFONT,(WPARAM)hGul9,(LPARAM)FALSE);
		hTabImg=ImageList_LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_TABS), 
			12, 1, RGB(255,255,255));
		TabCtrl_SetImageList(hFileTab,hTabImg);
		return 0;
	case WM_SIZE:
		MoveWindow(hFileTab,0,2,LOWORD(lParam),HIWORD(lParam)-2,TRUE);
		return 0;
	case WM_PAINT:
		hdc=BeginPaint(hWnd, &ps);
		GetClientRect(hWnd,&crt);
		hPen=CreatePen(PS_SOLID,1,GetSysColor(COLOR_3DSHADOW));
		OldPen=(HPEN)SelectObject(hdc,hPen);
		MoveToEx(hdc,0,0,NULL);
		LineTo(hdc,crt.right,0);
		DeleteObject(SelectObject(hdc,OldPen));
		hPen=CreatePen(PS_SOLID,1,GetSysColor(COLOR_3DHILIGHT));
		SelectObject(hdc,hPen);
		MoveToEx(hdc,0,1,NULL);
		LineTo(hdc,crt.right,1);
		DeleteObject(SelectObject(hdc,OldPen));
		EndPaint(hWnd, &ps);
		return 0;
	case WM_NOTIFY:
		hdr=(LPNMHDR)lParam;
		switch (hdr->code) {
		case TCN_SELCHANGE:
			idx=TabCtrl_GetCurSel(hFileTab);
			tie.mask=TCIF_PARAM;
			TabCtrl_GetItem(hFileTab,idx,&tie);
			hChild=FindChildWithFile((TCHAR *)tie.lParam);
			SendMessage(g_hMDIClient,WM_MDIACTIVATE,(WPARAM)hChild,0);
			break;
		}
		return 0;
	case WM_DESTROY:
		ImageList_Destroy(hTabImg);
		return 0;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}

void AddFileTab(TCHAR *Path)
{
	TCITEM tie;
	int count;
	TCHAR Ext[_MAX_EXT];
	TCHAR Name[MAX_PATH];
	TCHAR *Param;

	Param=(TCHAR *)malloc(MAX_PATH);
	lstrcpy(Param,Path);
	_splitpath(Path,NULL,NULL,Name,Ext);
	lstrcat(Name,Ext);
	count=TabCtrl_GetItemCount(hFileTab);
	tie.mask=TCIF_TEXT | TCIF_PARAM | TCIF_IMAGE;
	tie.pszText=Name;
	tie.lParam=(LPARAM)Param;
	tie.iImage=0;
	TabCtrl_InsertItem(hFileTab,count,&tie);
	TabCtrl_SetCurSel(hFileTab,count);
	if (count==0) {
		InvalidateRect(hFileTab,NULL,TRUE);
	}
}

int FindFileTab(TCHAR *Path)
{
	int idx;
	int count;
	TCITEM tie;

	count=TabCtrl_GetItemCount(hFileTab);
	for (idx=0;idx<count;idx++) {
		tie.mask=TCIF_PARAM;
		TabCtrl_GetItem(hFileTab,idx,&tie);
		if (lstrcmpi(Path,(TCHAR *)tie.lParam)==0) {
			return idx;
		}
	}
	return -1;
}

void DeleteFileTab(TCHAR *Path)
{
	int idx;
	TCITEM tie;

	idx=FindFileTab(Path);
	tie.mask=TCIF_PARAM;
	TabCtrl_GetItem(hFileTab,idx,&tie);
	free((LPVOID)tie.lParam);
	TabCtrl_DeleteItem(hFileTab,idx);
}

void ChangeFileTab(int idx, TCHAR *Path)
{
	TCITEM tie;
	TCHAR Ext[_MAX_EXT];
	TCHAR Name[MAX_PATH];
	TCHAR *Param;

	tie.mask=TCIF_PARAM;
	TabCtrl_GetItem(hFileTab,idx,&tie);
	Param=(TCHAR *)tie.lParam;
	lstrcpy(Param,Path);

	_splitpath(Path,NULL,NULL,Name,Ext);
	lstrcat(Name,Ext);
	tie.mask=TCIF_TEXT | TCIF_IMAGE;
	tie.pszText=Name;
	tie.iImage=0;
	TabCtrl_SetItem(hFileTab,idx,&tie);
}

