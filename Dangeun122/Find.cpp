#include "stdafx.h"

void RefillHistory(HWND hCombo, CHistory &Hist)
{
	int i;

	SendMessage(hCombo,CB_RESETCONTENT,0,0);
	for (i=0;i<Hist.num;i++) {
		SendMessage(hCombo,CB_ADDSTRING,0,(LPARAM)Hist.Get(i));
	}
	SendMessage(hCombo,CB_SETCURSEL,0,0);
}

BOOL CALLBACK FindDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	TCHAR szTemp[256];
	static RECT drt;
	RECT wrt;
	int dx,dy;
	RECT irt;
	int dHeight;
	HWND hActive;
	SInfo *pSi;
	int s,e;
	HMENU hMenu, hPopup;
	POINT pt;
	static int PopupID;

	switch(iMessage) {
	case WM_INITDIALOG:
		if (drt.left==0) {
			GetWindowRect(GetParent(hDlg),&wrt);
			GetWindowRect(hDlg,&drt);
			switch (Option.FindDlgPos) {
			case 0:
				dx=wrt.left+10;
				dy=wrt.top+10;
			default:
			case 1:
				dx=wrt.right-(drt.right-drt.left)-10;
				dy=wrt.top+10;
				break;
			case 2:
				dx=wrt.right-(drt.right-drt.left)-10;
				dy=wrt.bottom-(drt.bottom-drt.top)-10;
				break;
			case 3:
				dx=wrt.left+(wrt.right-wrt.left)/2-(drt.right-drt.left)/2;
				dy=wrt.top+(wrt.bottom-wrt.top)/2-(drt.bottom-drt.top)/2;
				break;
			}
			if (GetSystemMetrics(80/*SM_CMONITORS*/) <= 1) {
				if (dx+(drt.right-drt.left) > GetSystemMetrics(SM_CXSCREEN)) {
					dx=GetSystemMetrics(SM_CXSCREEN)-(drt.right-drt.left);
				}
				if (dy+(drt.bottom-drt.top) > GetSystemMetrics(SM_CYSCREEN)) {
					dy=GetSystemMetrics(SM_CYSCREEN)-(drt.bottom-drt.top);
				}
			}
			SetWindowPos(hDlg,HWND_NOTOPMOST,dx,dy,0,0,SWP_NOSIZE);
		} else {
			SetWindowPos(hDlg,HWND_NOTOPMOST,drt.left,drt.top,0,0,SWP_NOSIZE);
		}
		
		if (FindFlag & AE_FIND_UP) {
			CheckDlgButton(hDlg,IDC_FIND_UP,BST_CHECKED);
		}

		if (FindFlag & AE_FIND_WHOLEWORD) {
			CheckDlgButton(hDlg,IDC_FIND_WHOLEWORD,BST_CHECKED);
		}

		if (FindFlag & AE_FIND_MATCHCASE) {
			CheckDlgButton(hDlg,IDC_FIND_MATCHCASE,BST_CHECKED);
		}

		if (FindFlag & AE_FIND_WRAP) {
			CheckDlgButton(hDlg,IDC_FIND_WRAP,BST_CHECKED);
		}

		if (FindFlag & AE_FIND_CLOSE) {
			CheckDlgButton(hDlg,IDC_FIND_CLOSE,BST_CHECKED);
		}

		RefillHistory(GetDlgItem(hDlg,IDC_FIND_WHAT),arFind[0]);
		if (Option.bInitFind) {
			hActive=(HWND)SendMessage(g_hMDIClient,WM_MDIGETACTIVE,0,NULL);
			pSi=(SInfo *)GetWindowLongPtr(hActive,0);
			pSi->Ae.GetSelect(s,e);
			if (s==e) {
				pSi->Ae.GetActiveView()->GetNowWord(pSi->Ae.GetOffset(),s,e);
			}
			if (e-s <= 255 && e-s > 0) {
				pSi->Ae.GetText(szTemp,256,s,e);
				szTemp[e-s]=0;
				SetDlgItemText(hDlg,IDC_FIND_WHAT,szTemp);
			}
		}
		RefillHistory(GetDlgItem(hDlg,IDC_FIND_TO),arFind[1]);
		SendMessage(GetDlgItem(hDlg,IDC_FIND_WHAT), CB_LIMITTEXT, (WPARAM)255, 0);
		SendMessage(GetDlgItem(hDlg,IDC_FIND_TO), CB_LIMITTEXT, (WPARAM)255, 0);
		SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_FIND_WHAT,CBN_EDITCHANGE),0);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_FIND_WHAT:
			switch (HIWORD(wParam)) {
			case CBN_EDITCHANGE:
				if (GetWindowTextLength(GetDlgItem(hDlg,IDC_FIND_WHAT)) == 0) {
					EnableWindow(GetDlgItem(hDlg,IDC_BTNFIND),FALSE);
					EnableWindow(GetDlgItem(hDlg,IDC_BTNFINDALL),FALSE);
				} else {
					EnableWindow(GetDlgItem(hDlg,IDC_BTNFIND),TRUE);
					EnableWindow(GetDlgItem(hDlg,IDC_BTNFINDALL),TRUE);
				}
				SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_FIND_TO,CBN_EDITCHANGE),0);
				break;
			case CBN_SELCHANGE:
				PostMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_FIND_WHAT,CBN_EDITCHANGE),0);					
				break;
			}
			break;
		case IDC_FIND_TO:
			switch (HIWORD(wParam)) {
			case CBN_EDITCHANGE:
				if (GetWindowTextLength(GetDlgItem(hDlg,IDC_FIND_WHAT)) == 0) {
					EnableWindow(GetDlgItem(hDlg,IDC_BTNREPLACE),FALSE);
					EnableWindow(GetDlgItem(hDlg,IDC_BTNREPLACEALL),FALSE);
				} else {
					EnableWindow(GetDlgItem(hDlg,IDC_BTNREPLACE),TRUE);
					EnableWindow(GetDlgItem(hDlg,IDC_BTNREPLACEALL),TRUE);
				}
				break;
			case CBN_SELCHANGE:
				PostMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_FIND_TO,CBN_EDITCHANGE),0);
				break;
			}
			break;
		case IDCANCEL:
			GetWindowRect(hDlg,&drt);
			DestroyWindow(hDlg);
			break;
		case IDC_BTNFIND:
		case IDC_BTNREPLACE:
		case IDC_BTNREPLACEALL:
		case IDC_BTNFINDALL:
			if (IsDlgButtonChecked(hDlg,IDC_FIND_UP)) {
				FindFlag |= AE_FIND_UP;
			} else {
				FindFlag &= ~AE_FIND_UP;
			}

			if (IsDlgButtonChecked(hDlg,IDC_FIND_WHOLEWORD)) {
				FindFlag |= AE_FIND_WHOLEWORD;
			} else {
				FindFlag &= ~AE_FIND_WHOLEWORD;
			}

			if (IsDlgButtonChecked(hDlg,IDC_FIND_MATCHCASE)) {
				FindFlag |= AE_FIND_MATCHCASE;
			} else {
				FindFlag &= ~AE_FIND_MATCHCASE;
			}

			if (IsDlgButtonChecked(hDlg,IDC_FIND_WRAP)) {
				FindFlag |= AE_FIND_WRAP;
			} else {
				FindFlag &= ~AE_FIND_WRAP;
			}

			if (IsDlgButtonChecked(hDlg,IDC_FIND_CLOSE)) {
				FindFlag |= AE_FIND_CLOSE;
			} else {
				FindFlag &= ~AE_FIND_CLOSE;
			}

			GetDlgItemText(hDlg,IDC_FIND_WHAT,szTemp,255);
			if (lstrlen(szTemp)) {
				arFind[0].Add(szTemp);
				RefillHistory(GetDlgItem(hDlg,IDC_FIND_WHAT),arFind[0]);
			}

			GetDlgItemText(hDlg,IDC_FIND_TO,szTemp,255);
			arFind[1].Add(szTemp);
			RefillHistory(GetDlgItem(hDlg,IDC_FIND_TO),arFind[1]);

			switch (LOWORD(wParam)) {
			case IDC_BTNFIND:
				SendMessage(GetParent(hDlg),WM_USER+2,1,0);
				if (FindFlag & AE_FIND_CLOSE) {
					GetWindowRect(hDlg,&drt);
					DestroyWindow(hDlg);
				}
				break;
			case IDC_BTNREPLACE:
				SendMessage(GetParent(hDlg),WM_USER+2,2,0);
				break;
			case IDC_BTNREPLACEALL:
				SendMessage(GetParent(hDlg),WM_USER+2,3,0);
				break;
			case IDC_BTNFINDALL:
				SendMessage(GetParent(hDlg),WM_USER+2,4,0);
				if (FindFlag & AE_FIND_CLOSE) {
					GetWindowRect(hDlg,&drt);
					DestroyWindow(hDlg);
				}
				break;
			}
			return TRUE;
		case IDC_BTNINSCODE:
		case IDC_BTNINSCODE2:
			if (LOWORD(wParam)==IDC_BTNINSCODE) {
				PopupID=IDC_FIND_WHAT;
			} else {
				PopupID=IDC_FIND_TO;
			}
			hMenu=LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_POPUP));
			hPopup=GetSubMenu(hMenu, 5);

			GetCursorPos(&pt);
			TrackPopupMenu(hPopup, TPM_LEFTALIGN, pt.x, pt.y,
				0, hDlg, NULL);
			DestroyMenu(hMenu);
			return TRUE;
		case IDM_CODE_ENTER:
		case IDM_CODE_TAB:
		case IDM_CODE_HEXA:
		case IDM_CODE_DECIMAL:
		case IDM_CODE_BACKSLASH:
			GetDlgItemText(hDlg,PopupID,szTemp,256);
			switch (LOWORD(wParam)) {
				case IDM_CODE_ENTER:
					lstrcat(szTemp,"\\n");
					break;
				case IDM_CODE_TAB:
					lstrcat(szTemp,"\\t");
					break;
				case IDM_CODE_HEXA:
					lstrcat(szTemp,"\\x");
					break;
				case IDM_CODE_DECIMAL:
					lstrcat(szTemp,"\\i");
					break;
				case IDM_CODE_BACKSLASH:
					lstrcat(szTemp,"\\\\");
					break;
			}
			SetDlgItemText(hDlg,PopupID,szTemp);
			SetFocus(GetDlgItem(hDlg,PopupID));
			SendDlgItemMessage(hDlg,PopupID,CB_SETEDITSEL,0,MAKELPARAM(-1,0));
			PostMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_FIND_WHAT,CBN_EDITCHANGE),0);
			return TRUE;
		}
		return FALSE;
	case WM_USER+1:
		wrt=*(LPRECT)wParam;
		GetWindowRect(hDlg,&drt);
		IntersectRect(&irt,&wrt,&drt);
		if (IsRectEmpty(&irt))
			return TRUE;

		InflateRect(&wrt,32,32);
		dHeight=drt.bottom-drt.top;
		if (wrt.top-dHeight > 0) {
			drt.top=wrt.top-dHeight;
			SetWindowPos(hDlg,HWND_NOTOPMOST,drt.left, drt.top,0,0,SWP_NOSIZE);
		} else if (wrt.bottom+dHeight < GetSystemMetrics(SM_CYSCREEN)) {
			drt.top=wrt.bottom;
			SetWindowPos(hDlg,HWND_NOTOPMOST,drt.left, drt.top,0,0,SWP_NOSIZE);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CALLBACK GotoLineDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HWND hActive;
	SInfo *pSi;
	int nPara;

	switch(iMessage) {
	case WM_INITDIALOG:
		MoveToParentCenter(hDlg);
		hActive=(HWND)SendMessage(g_hMDIClient,WM_MDIGETACTIVE,0,NULL);
		pSi=(SInfo *)GetWindowLongPtr(hActive,0);
		nPara=pSi->Ae.GetInfoFromOff(AE_INFO_PARA)+1;
		SetDlgItemInt(hDlg,IDC_GOLINENUM,nPara,FALSE);
		SendDlgItemMessage(hDlg,IDC_GOTOLINESPIN,UDM_SETRANGE,0,MAKELPARAM(UD_MAXVAL,1));
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			EndDialog(hDlg,GetDlgItemInt(hDlg,IDC_GOLINENUM,NULL,FALSE));
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg,-1);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

LRESULT CALLBACK DGOutputProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	LVCOLUMN COL;
	HMENU hMenu, hPopup;
	int idx;

	switch(iMessage) {
	case WM_CREATE:
		hList=CreateWindow(WC_LISTVIEW,NULL,WS_CHILD | WS_VISIBLE |
			LVS_REPORT | LVS_SHOWSELALWAYS,
			0,0,0,0,hWnd,NULL,g_hInst,NULL);
		ListView_SetExtendedListViewStyle(hList,LVS_EX_FULLROWSELECT);

		COL.mask=LVCF_FMT | LVCF_WIDTH | LVCF_TEXT |LVCF_SUBITEM;
		COL.fmt=LVCFMT_LEFT;
		COL.cx=40;
		COL.pszText="번호";
		COL.iSubItem=0;
		ListView_InsertColumn(hList,0,&COL);

		COL.cx=200;
		COL.pszText="위치";
		COL.iSubItem=1;
		ListView_InsertColumn(hList,1,&COL);

		COL.cx=500;
		COL.pszText="내용";
		COL.iSubItem=2;
		ListView_InsertColumn(hList,2,&COL);
		return 0;
	case WM_SIZE:
		MoveWindow(hList,0,0,LOWORD(lParam),HIWORD(lParam),TRUE);
		return 0;
	case WM_NOTIFY:
		LPNMHDR hdr;
		LPNMLISTVIEW nlv;
		hdr=(LPNMHDR)lParam;
		nlv=(LPNMLISTVIEW)lParam;
		LPNMITEMACTIVATE nia;

		if (hdr->hwndFrom == hList) {
			switch (hdr->code) {
			case NM_DBLCLK:
				nia = (LPNMITEMACTIVATE)lParam;
				if (nia->iItem != -1) {
					OpenFileFromOutput(nia->iItem);
				}
				return TRUE;
			}
		}
		break;
	case WM_CONTEXTMENU:
		LVHITTESTINFO pinfo;
		POINT pt;
		hMenu=LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_POPUP));
		hPopup=GetSubMenu(hMenu, 0);

		pinfo.pt.x=LOWORD(lParam);
		pinfo.pt.y=HIWORD(lParam);
		ScreenToClient(hList,&pinfo.pt);
		ListView_HitTest(hList,&pinfo);
		if (pinfo.iItem == -1) {
			EnableMenuItem(hPopup,IDM_OUTPUT_GO,MF_BYCOMMAND | MF_GRAYED);
		}

		if (LOWORD(lParam)==65535) {
			pt.x=20;
			pt.y=20;
			ClientToScreen(hWnd,&pt);
			lParam=MAKELPARAM(pt.x,pt.y);
		}
		TrackPopupMenu(hPopup, TPM_LEFTALIGN, (short)LOWORD(lParam), (short)HIWORD(lParam), 
			0, hWnd, NULL);
		DestroyMenu(hMenu);
		return 0;
	case WM_COMMAND:
		switch(LOWORD(wParam)) {
		case IDM_OUTPUT_GO:
			idx=ListView_GetNextItem(hList,-1,LVNI_ALL | LVNI_SELECTED);
			if (idx != -1) {
				OpenFileFromOutput(idx);
			}
			break;
		case IDM_OUTPUT_COPY:
			CopyOutputToText();
			break;
		case IDM_OUTPUT_SEND:
			SendOutputToWindow();
			break;
		case IDM_OUTPUT_SENDNEW:
			NewChild();
			SendOutputToWindow();
			break;
		case IDM_OUTPUT_CLOSE:
			bShowOutput=FALSE;
			Relayout();
			break;
		}
		return 0;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}

BOOL CALLBACK FileFindDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	TCHAR szTemp[256];
	TCHAR Folder[MAX_PATH];
	HWND hActive;
	SInfo *pSi;
	int s,e;
	HMENU hMenu, hPopup;
	POINT pt;
	static int PopupID;

	switch(iMessage) {
	case WM_INITDIALOG:
		MoveToParentCenter(hDlg);

		if (FindFlag & AE_FIND_WHOLEWORD) {
			CheckDlgButton(hDlg,IDC_FIND_WHOLEWORD,BST_CHECKED);
		}

		if (FindFlag & AE_FIND_MATCHCASE) {
			CheckDlgButton(hDlg,IDC_FIND_MATCHCASE,BST_CHECKED);
		}

		if (FindFlag & AE_FIND_RECURSIVE) {
			CheckDlgButton(hDlg,IDC_FIND_RECURSIVE,BST_CHECKED);
		}

		if (FindFlag & AE_FIND_SHORTPATH) {
			CheckDlgButton(hDlg,IDC_FIND_SHORTPATH,BST_CHECKED);
		}

		RefillHistory(GetDlgItem(hDlg,IDC_FIND_WHAT),arFind[0]);
		if (Option.bInitFind) {
			hActive=(HWND)SendMessage(g_hMDIClient,WM_MDIGETACTIVE,0,NULL);
			if (hActive != NULL) {
				pSi=(SInfo *)GetWindowLongPtr(hActive,0);
				pSi->Ae.GetSelect(s,e);
				if (s==e) {
					pSi->Ae.GetActiveView()->GetNowWord(pSi->Ae.GetOffset(),s,e);
				}
				if (e-s <= 255 && e-s > 0) {
					pSi->Ae.GetText(szTemp,256,s,e);
					szTemp[e-s]=0;
					SetDlgItemText(hDlg,IDC_FIND_WHAT,szTemp);
				}
			}
		}
		RefillHistory(GetDlgItem(hDlg,IDC_FIND_FOLDER),arFind[2]);
		RefillHistory(GetDlgItem(hDlg,IDC_FIND_TYPE),arFind[3]);
		RefillHistory(GetDlgItem(hDlg,IDC_FIND_TO),arFind[1]);
		SendMessage(GetDlgItem(hDlg,IDC_FIND_WHAT), CB_LIMITTEXT, (WPARAM)255, 0);
		SendMessage(GetDlgItem(hDlg,IDC_FIND_FOLDER), CB_LIMITTEXT, (WPARAM)MAX_PATH, 0);
		SendMessage(GetDlgItem(hDlg,IDC_FIND_TYPE), CB_LIMITTEXT, (WPARAM)255, 0);
		SendMessage(GetDlgItem(hDlg,IDC_FIND_TO), CB_LIMITTEXT, (WPARAM)255, 0);
		SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_FIND_WHAT,CBN_EDITCHANGE),0);
		SendDlgItemMessage(hDlg,IDC_FIND_FOLDER,CB_SETDROPPEDWIDTH,500,0);
		DragAcceptFiles(hDlg,TRUE);
		return TRUE;
	case WM_DROPFILES:
		DragQueryFile((HDROP)wParam,0,Folder,MAX_PATH);
		if ((GetFileAttributes(Folder) & FILE_ATTRIBUTE_DIRECTORY)!=0) {
			SetDlgItemText(hDlg,IDC_FIND_FOLDER,Folder);
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_FIND_WHAT:
		case IDC_FIND_FOLDER:
		case IDC_FIND_TYPE:
		case IDC_FIND_TO:
			switch (HIWORD(wParam)) {
			case CBN_EDITCHANGE:
				if (GetWindowTextLength(GetDlgItem(hDlg,IDC_FIND_WHAT)) == 0 ||
					GetWindowTextLength(GetDlgItem(hDlg,IDC_FIND_FOLDER)) == 0 ||
					GetWindowTextLength(GetDlgItem(hDlg,IDC_FIND_TYPE)) == 0) {
					EnableWindow(GetDlgItem(hDlg,IDC_BTNFIND),FALSE);
					EnableWindow(GetDlgItem(hDlg,IDC_BTNREPLACEALL),FALSE);
				} else {
					EnableWindow(GetDlgItem(hDlg,IDC_BTNFIND),TRUE);
					EnableWindow(GetDlgItem(hDlg,IDC_BTNREPLACEALL),TRUE);
				}
				break;
			case CBN_SELCHANGE:
				PostMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_FIND_WHAT,CBN_EDITCHANGE),0);
				break;
			}
			break;
		case IDC_BTN_BRFOLDER:
			GetDlgItemText(hDlg,IDC_FIND_FOLDER,szTemp,255);
			if (BrowseFolder(hDlg,"검색을 시작할 폴더를 선택하십시오.",szTemp,Folder)==TRUE) {
				SetDlgItemText(hDlg,IDC_FIND_FOLDER,Folder);
				SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_FIND_WHAT,CBN_EDITCHANGE),0);
			}
			return TRUE;
		case IDC_BTNFIND:
			if (IsDlgButtonChecked(hDlg,IDC_FIND_WHOLEWORD)) {
				FindFlag |= AE_FIND_WHOLEWORD;
			} else {
				FindFlag &= ~AE_FIND_WHOLEWORD;
			}

			if (IsDlgButtonChecked(hDlg,IDC_FIND_MATCHCASE)) {
				FindFlag |= AE_FIND_MATCHCASE;
			} else {
				FindFlag &= ~AE_FIND_MATCHCASE;
			}

			if (IsDlgButtonChecked(hDlg,IDC_FIND_RECURSIVE)) {
				FindFlag |= AE_FIND_RECURSIVE;
			} else {
				FindFlag &= ~AE_FIND_RECURSIVE;
			}

			if (IsDlgButtonChecked(hDlg,IDC_FIND_SHORTPATH)) {
				FindFlag |= AE_FIND_SHORTPATH;
			} else {
				FindFlag &= ~AE_FIND_SHORTPATH;
			}

			GetDlgItemText(hDlg,IDC_FIND_WHAT,szTemp,255);
			if (lstrlen(szTemp)) {
				arFind[0].Add(szTemp);
			}
			GetDlgItemText(hDlg,IDC_FIND_FOLDER,szTemp,255);
			if (lstrlen(szTemp)) {
				arFind[2].Add(szTemp);
			}
			GetDlgItemText(hDlg,IDC_FIND_TYPE,szTemp,255);
			if (lstrlen(szTemp)) {
				arFind[3].Add(szTemp);
			}
			GetDlgItemText(hDlg,IDC_FIND_TO,szTemp,255);
			arFind[1].Add(szTemp);
			EndDialog(hDlg,IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg,IDCANCEL);
			return TRUE;
		case IDC_BTNINSCODE:
		case IDC_BTNINSCODE2:
			if (LOWORD(wParam)==IDC_BTNINSCODE) {
				PopupID=IDC_FIND_WHAT;
			} else {
				PopupID=IDC_FIND_TO;
			}
			hMenu=LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_POPUP));
			hPopup=GetSubMenu(hMenu, 5);

			GetCursorPos(&pt);
			TrackPopupMenu(hPopup, TPM_LEFTALIGN, pt.x, pt.y,
				0, hDlg, NULL);
			DestroyMenu(hMenu);
			return TRUE;
		case IDM_CODE_ENTER:
		case IDM_CODE_TAB:
		case IDM_CODE_HEXA:
		case IDM_CODE_DECIMAL:
		case IDM_CODE_BACKSLASH:
			GetDlgItemText(hDlg,PopupID,szTemp,256);
			switch (LOWORD(wParam)) {
				case IDM_CODE_ENTER:
					lstrcat(szTemp,"\\n");
					break;
				case IDM_CODE_TAB:
					lstrcat(szTemp,"\\t");
					break;
				case IDM_CODE_HEXA:
					lstrcat(szTemp,"\\x");
					break;
				case IDM_CODE_DECIMAL:
					lstrcat(szTemp,"\\i");
					break;
				case IDM_CODE_BACKSLASH:
					lstrcat(szTemp,"\\\\");
					break;
			}
			SetDlgItemText(hDlg,PopupID,szTemp);
			SetFocus(GetDlgItem(hDlg,PopupID));
			SendDlgItemMessage(hDlg,PopupID,CB_SETEDITSEL,0,MAKELPARAM(-1,0));
			PostMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_FIND_WHAT,CBN_EDITCHANGE),0);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

void FindOrReplaceInFiles(BOOL bReplace)
{
	BOOL bDeep;
	DWORD ThreadID;

	if ((lstrlen(arFind[0].Get(0))==0) ||
		(bReplace && lstrlen(arFind[1].Get(0))==0) ||
		(lstrlen(arFind[2].Get(0))==0 || _access(arFind[2].Get(0),0)!=0) ||
		(lstrlen(arFind[3].Get(0))==0)) {
		return;
	}

	if (FindFlag & AE_FIND_RECURSIVE) {
		bDeep=TRUE;
	} else {
		bDeep=FALSE;
	}

	TotalFind=0;
	if (bShowOutput==FALSE) {
		bShowOutput=TRUE;
		Relayout();
	}
	SendMessage(hList,LVM_DELETEALLITEMS,0,0);

	SetStatusText(1,"파일 검색중...");
	if (bReplace) {
		lstrcpy(LastFIF,arFind[1].Get(0));
		hFIFThread=CreateThread(NULL, 0, FIFThread,(LPVOID)OnReplaceFile,0,&ThreadID);
	} else {
		lstrcpy(LastFIF,arFind[0].Get(0));
		hFIFThread=CreateThread(NULL, 0, FIFThread,(LPVOID)OnFindFile,0,&ThreadID);
	} 
}

int OnFindFile(TCHAR *Path,DWORD Attr,LPVOID pCustom)
{
	TCHAR what[256];
	HANDLE hFile;
	DWORD size, dwRead;
	TCHAR *buf, *pbuf;
	TCHAR *p;
	int line,col;
	TCHAR Text[501];
	TCHAR Mes[2048];
	LVITEM LI;
	int idx;
	int len;

	if (Attr & FILE_ATTRIBUTE_DIRECTORY)
		return 0;

	if (strchr(arFind[0].Get(0),'\\')==NULL) {
		lstrcpy(what,arFind[0].Get(0));
		len=lstrlen(what);
	} else {
		len=AeEscapeString(arFind[0].Get(0),what);
	}

	hFile=CreateFile(Path,GENERIC_READ,0,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile==INVALID_HANDLE_VALUE) {
		return -1;
	}
	size=GetFileSize(hFile,NULL);
	size=min(size,30*1048576);
	buf=(TCHAR *)malloc(size);
	ReadFile(hFile,buf,size,&dwRead,NULL);

	for (pbuf=buf;;) {
		p=FindString(pbuf,size-(pbuf-buf),0,what,len,TRUE,(FindFlag & AE_FIND_MATCHCASE)!=0);
		if (p == NULL)
			break;

		if (FindFlag & AE_FIND_WHOLEWORD) {
			if (!((p==buf || IsDelimiter(*(p-1))) && IsDelimiter(*(p+len)))) {
				pbuf=p+len;
				continue;
			}
		}

		TotalFind++;
		GetLineNumAndText(buf,size,p,Text,line,col);

		LI.mask=LVIF_TEXT;
		LI.state=0;
		LI.stateMask=0;
		LI.iSubItem=0;
		LI.iItem=ListView_GetItemCount(hList);
		itoa(TotalFind,Mes,10);
		LI.pszText=Mes;
		idx=ListView_InsertItem(hList,&LI);

		if (FindFlag & AE_FIND_SHORTPATH) {
			wsprintf(Mes,"%s(%d,%d)",Path+lstrlen(arFind[2].Get(0))+1,line,col);
		} else {
			wsprintf(Mes,"%s(%d,%d)",Path,line,col);
		}
		ListView_SetItemText(hList,idx,1,Mes);

		ListView_SetItemText(hList,idx,2,Text);
		ListView_EnsureVisible(hList,idx,FALSE);
		UpdateWindow(hList);

		pbuf=p+len;
		if (bContFIF==FALSE)
			break;
	}

	free(buf);
	CloseHandle(hFile);
	return 0;
}

void GetLineNumAndText(TCHAR *szBuf, int buflen, TCHAR *ptr, TCHAR *Text, int &line, int &col)
{
	TCHAR *p=szBuf;
	TCHAR *t=Text;
	line=1;

	while (p != ptr) {
		if (*p == '\n') {
			line++;
		}
		p++;
	}

	while ((p != szBuf-1) && (*p != '\r') && (*p != '\n')) p--;
	p++;

	col=ptr-p+1;

	while ((p-szBuf < buflen) && (*p != '\r') && (*p != '\n') && (t-Text < 499)) {
		if (*p == '\t') {
			*t++=' ';
			*t++=' ';
			*t++=' ';
			*t=' ';
		} else {
			if (IsDBCSLeadByte(*p)) {
				*t++=*p++;
			}
			*t=*p;
		}
		t++;
		p++;
	}
	*t=0;
}

BOOL IsDelimiter(TCHAR t)
{
	return (strchr(" \t\r\n\"\'.,<>:;/()[]{}",t) || t==0);
}

int OnReplaceFile(TCHAR *Path, DWORD Attr, LPVOID pCustom)
{
	TCHAR what[256];
	TCHAR to[256];
	HANDLE hFile;
	DWORD size, dwRead;
	TCHAR *buf, *pbuf;
	TCHAR *p;
	int line,col;
	TCHAR Text[501];
	TCHAR Mes[512];
	LVITEM LI;
	int idx;
	BOOL bReplace=FALSE;
	int len1,len2;
	int extra, memsize;
	int tempoff;

	if ((Attr & FILE_ATTRIBUTE_DIRECTORY) || (Attr & FILE_ATTRIBUTE_READONLY)) {
		return 0;
	}

	if (FindChildWithFile(Path) != NULL) {
		wsprintf(Mes, "%s 파일은 열려 있으므로 바꾸기를 수행할 수 없습니다.",Path);
		MessageBox(g_hFrameWnd,Mes,"알림",MB_OK);
		return -1;
	}

	if (strchr(arFind[0].Get(0),'\\')==NULL) {
		lstrcpy(what,arFind[0].Get(0));
		len1=lstrlen(what);
	} else {
		len1=AeEscapeString(arFind[0].Get(0),what);
	}
	if (strchr(arFind[1].Get(0),'\\')==NULL) {
		lstrcpy(to,arFind[1].Get(0));
		len2=lstrlen(to);
	} else {
		len2=AeEscapeString(arFind[1].Get(0),to);
	}

	hFile=CreateFile(Path,GENERIC_READ,0,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile==INVALID_HANDLE_VALUE) {
		return -1;
	}
	size=GetFileSize(hFile,NULL);
	if (size > 30*1048576) {
		CloseHandle(hFile);
		return -1;
	}
	extra=10000;

	memsize=size+extra+1;
	buf=(TCHAR *)malloc(memsize);
	ReadFile(hFile,buf,size,&dwRead,NULL);

	for (pbuf=buf;;) {
		if (extra < 256) {
			tempoff=pbuf-buf;
			memsize+=10000;
			extra+=10000;
			buf=(TCHAR *)realloc(buf,memsize);
			pbuf=buf+tempoff;
		}

		p=FindString(pbuf,size-(pbuf-buf),0,what,len1,TRUE,(FindFlag & AE_FIND_MATCHCASE)!=0);
		if (p == NULL)
			break;

		if (FindFlag & AE_FIND_WHOLEWORD) {
			if (!((p==buf || IsDelimiter(*(p-1))) && IsDelimiter(*(p+len1)))) {
				pbuf=p+len1;
				continue;
			}
		}

		bReplace=TRUE;
		memmove(p+len2,p+len1,size-(p+len1-buf));
		memcpy(p,to,len2);
		size+=(len2-len1);

		TotalFind++;
		GetLineNumAndText(buf,size,p,Text,line,col);

		LI.mask=LVIF_TEXT;
		LI.state=0;
		LI.stateMask=0;
		LI.iSubItem=0;
		LI.iItem=ListView_GetItemCount(hList);
		itoa(TotalFind,Mes,10);
		LI.pszText=Mes;
		idx=ListView_InsertItem(hList,&LI);

		if (FindFlag & AE_FIND_SHORTPATH) {
			wsprintf(Mes,"%s(%d,%d)",Path+lstrlen(arFind[2].Get(0))+1,line,col);
		} else {
			wsprintf(Mes,"%s(%d,%d)",Path,line,col);
		}
		ListView_SetItemText(hList,idx,1,Mes);

		ListView_SetItemText(hList,idx,2,Text);
		ListView_EnsureVisible(hList,idx,FALSE);
		UpdateWindow(hList);

		pbuf=p+len2;
		extra -= (len2-len1);
		if (bContFIF==FALSE) {
			bReplace=FALSE;
			break;
		}
	}

	CloseHandle(hFile);
	if (bReplace) {
		hFile=CreateFile(Path,GENERIC_WRITE,0,NULL,
			CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if (hFile!=INVALID_HANDLE_VALUE) {
			WriteFile(hFile,buf,size,&dwRead,NULL);
			CloseHandle(hFile);
		}
	}

	free(buf);
	return 0;
}

void OpenFileFromOutput(int idx)
{
	TCHAR Text[MAX_PATH];
	TCHAR Path[MAX_PATH];
	int line,col;
	int toff;
	TCHAR *p;
	HWND hActive;
	SInfo *pSi;
	TCHAR LastFIF2[256];
	int len;
	int OldyPos;
	LVITEM LI;

	ListView_GetItemText(hList,idx,1,Text,MAX_PATH);
	p=strrchr(Text,',');
	col=atoi(p+1)-1;
	p=strrchr(Text,'(');
	line=atoi(p+1)-1;
	*p=0;

	LI.mask=LVIF_PARAM;
	LI.iItem=idx;
	LI.iSubItem=0;
	ListView_GetItem(hList,&LI);
	if (LI.lParam) {
		hActive=(HWND)LI.lParam;
		pSi=(SInfo *)GetWindowLong(hActive,0);
		// 없는 윈도우이거나 파일명이 다르면 아무것도 하지 않음
		if (pSi==NULL || strcmp(Text,pSi->NowFile)!=0) {
			MessageBox(g_hFrameWnd,"검색 대상 문서가 존재하지 않습니다. "
				"다시 검색해 주십시오","알림",MB_OK);
			return;
		}
	} else {
		if (FindFlag & AE_FIND_SHORTPATH) {
			wsprintf(Path,"%s\\%s",arFind[2].Get(0),Text);
		} else {
			lstrcpy(Path,Text);
		}

		if (OpenFromFile(Path)==FALSE) {
			return;
		}
		hActive=(HWND)SendMessage(g_hMDIClient,WM_MDIGETACTIVE,0,NULL);
		pSi=(SInfo *)GetWindowLong(hActive,0);
	}

	if (pSi->Ae.GetActiveView()->ViewType==HEXA_VIEW) {
		MessageBox(g_hFrameWnd,"활성 뷰가 헥사 뷰일 때는 검색 결과를 찾아갈 수 "
			"없습니다.","알림",MB_OK);
		return;
	}
	toff=pSi->Ae.GetOffFromPara(line,col);
	if (strchr(LastFIF,'\\')==NULL) {
		lstrcpy(LastFIF2,LastFIF);
		len=lstrlen(LastFIF2);
	} else {
		len=AeEscapeString(LastFIF,LastFIF2);
	}
	pSi->Ae.GetText(Text,MAX_PATH,toff,toff+len);
	Text[len]=0;
	if (stricmp(Text,LastFIF) != 0) {
		MessageBox(g_hFrameWnd,"검색 후에 파일이 수정되었습니다. "
			"검색된 줄의 첫 위치로 이동합니다.","알림",MB_OK);
		pSi->Ae.GotoLine(line,0,1);
	} else {
		OldyPos=pSi->Ae.GetActiveView()->GetYPos();
		pSi->Ae.GotoLine(line,col,1);
		pSi->Ae.SetSelect(toff,toff+len,TRUE);
		if (OldyPos != pSi->Ae.GetActiveView()->GetYPos()) {
			pSi->Ae.GetActiveView()->ArrangeVerticalCenter(toff);
		}
	}
}

void CopyOutputToText()
{
	HGLOBAL hmem;
	TCHAR *ptr;
	TCHAR *buf;

	ExtractOutputToText(buf);
	hmem=GlobalAlloc(GHND, strlen(buf)+1);
	ptr=(TCHAR *)GlobalLock(hmem);
	memcpy(ptr,buf,lstrlen(buf)+1);
	GlobalUnlock(hmem);
	if (OpenClipboard(g_hFrameWnd)) {
		EmptyClipboard();
		SetClipboardData(CF_TEXT,hmem);
		CloseClipboard();
	}
	free(buf);
}

void SendOutputToWindow()
{
	TCHAR *buf;
	HWND hActive;
	SInfo *pSi;

	hActive=(HWND)SendMessage(g_hMDIClient,WM_MDIGETACTIVE,0,NULL);
	if (hActive==NULL)
		return;

	pSi=(SInfo *)GetWindowLongPtr(hActive,0);
	ExtractOutputToText(buf);
	pSi->Ae.GetActiveView()->Insert(pSi->Ae.GetOffset(),buf,lstrlen(buf));
	pSi->Ae.Invalidate(-1);
	free(buf);
}

void ExtractOutputToText(TCHAR *&buf)
{
	int buflen;
	int idx,count;
	TCHAR Num[16];
	TCHAR Path[MAX_PATH];
	TCHAR Text[4096];

	buflen=10000;
	buf=(TCHAR *)malloc(buflen);
	buf[0]=0;

	count=ListView_GetItemCount(hList);
	for (idx=0;idx<count;idx++) {
		if (buflen-lstrlen(buf) < 5000) {
			buflen+=10000;
			buf=(TCHAR *)realloc(buf,buflen);
		}
		ListView_GetItemText(hList,idx,0,Num,16);
		lstrcat(buf,Num);
		lstrcat(buf," : ");
		ListView_GetItemText(hList,idx,1,Path,MAX_PATH);
		lstrcat(buf,Path);
		lstrcat(buf," ==> ");
		memset(Text,0,4096);
		ListView_GetItemText(hList,idx,2,Text,4000);
		lstrcat(buf,Text);
		lstrcat(buf,"\r\n");
	}
}

DWORD WINAPI FIFThread(LPVOID pCallback)
{
	TCHAR Mes[128];

	bContFIF=TRUE;
	FindInFiles(arFind[2].Get(0),arFind[3].Get(0),FIF_DEEP | FIF_INCHID,
		(FIFCALLBACK)pCallback,(LPVOID)NULL);

	if (TotalFind != 0) {
		wsprintf(Mes,"파일 검색 완료 : %d 개 발견",TotalFind);
	} else {
		if (Option.bSoundFindFail) {
			MessageBeep(MB_ICONASTERISK);
		}
		wsprintf(Mes,"찾는 문자열이 없습니다.");
	}
	SetStatusText(1,Mes);
	return 0;
}

BOOL TestFIFThread()
{
	DWORD dwExit;
	MSG Message;

	GetExitCodeThread(hFIFThread,&dwExit);
	if (dwExit==STILL_ACTIVE) {
		if (MessageBox(g_hFrameWnd,"파일 검색이 이미 진행중입니다. "
			"검색을 중지하고 새로운 검색을 하시겠습니까?","질문",MB_YESNO)==IDNO)
			return FALSE;
		bContFIF=FALSE;
		while (WaitForSingleObject(hFIFThread,0)==WAIT_TIMEOUT) {
			if (PeekMessage(&Message,NULL,0,0,PM_REMOVE)) {
				DispatchMessage(&Message);
			}
		}
	}
	return TRUE;
}

