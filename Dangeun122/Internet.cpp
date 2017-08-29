#include "stdafx.h"

static HWND hDlgDown;

BOOL CALLBACK DGHttpProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	static TCHAR *Path;

	switch(iMessage) {
	case WM_INITDIALOG:
		MoveToParentCenter(hDlg);
		SetDlgItemText(hDlg,IDC_HTTPFILE,"http://");
		MySetImeMode(GetDlgItem(hDlg,IDC_HTTPFILE),FALSE);
		Path=(TCHAR *)lParam;
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			GetDlgItemText(hDlg,IDC_HTTPFILE,Path,MAX_PATH);
			EndDialog(hDlg,IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg,IDCANCEL);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

int DgHttpDown(TCHAR *URL, TCHAR *&Text)
{
	HINTERNET hInternet, hURL;
	int len,extra;
	TCHAR *p;
	DWORD Size;
	DWORD dwRead, TotalRead;
	int toff;
	BOOL Result=FALSE;
	MSG msg;
	TCHAR Mes[400];
	HWND hDlgDown;

	bContDown=TRUE;
	hDlgDown=CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DOWNLOAD), 
		g_hFrameWnd, (DLGPROC)DGDownProc);
	SetDlgItemText(hDlgDown,IDC_STDOWN1,"���ͳ� ������...");
	SetWindowText(hDlgDown,"HTTP �ٿ�ε�");
	ShowWindow(hDlgDown,SW_SHOW);
	UpdateWindow(hDlgDown);
	EnableWindow(g_hFrameWnd, FALSE);

	hInternet=InternetOpen("Dangeun", INTERNET_OPEN_TYPE_PRECONFIG,
		NULL, NULL, 0);
	if (hInternet == NULL) {
		MessageBox(g_hFrameWnd, "���ͳݿ� ������ �� �����ϴ�","�˸�",MB_OK);
		goto NetFail;
	}

	hURL=InternetOpenUrl(hInternet,URL,NULL,0,INTERNET_FLAG_RELOAD,0);
	if (hURL==NULL) {
		MessageBox(g_hFrameWnd, "URL�� ã�� �� �����ϴ�","�˸�",MB_OK);
		goto EndDown;
	}
	wsprintf(Mes,"��ġ : %s",URL);
	SetDlgItemText(hDlgDown,IDC_STDOWN1,Mes);

	len=10000;
	extra=10000;
	Text=(TCHAR *)malloc(len);
	memset(Text,0,len);
	p=Text;
	TotalRead=0;

	do {
		InternetQueryDataAvailable(hURL,&Size,0,0);
		if (extra < int(Size+2)) {
			len+=(Size+2+10000);
			extra+=(Size+2+10000);
			toff=p-Text;
			Text=(TCHAR *)realloc(Text,len);
			memset(Text+len-(Size+2+10000),0,(Size+2+10000));
			p=Text+toff;
		}
		Result=InternetReadFile(hURL,p,Size,&dwRead);
		if (Result==FALSE) {
			MessageBox(g_hFrameWnd, "HTTP ������ ������ ���� �� �����ϴ�","�˸�",MB_OK);
			break;
		}
		if (bContDown==FALSE) {
			break;
		}
		p+=dwRead;
		TotalRead+=dwRead;
		extra-=dwRead;
		SendMessage(hDlgDown,WM_USER+1,(WPARAM)TotalRead,(LPARAM)-1);
		while (PeekMessage(&msg, NULL,0,0,PM_REMOVE)) {
			if (!IsDialogMessage(hDlgDown, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	} while (dwRead != 0);

	InternetCloseHandle(hURL);
EndDown:
	InternetCloseHandle(hInternet);
NetFail:
	EnableWindow(g_hFrameWnd, TRUE);
	DestroyWindow(hDlgDown);
	if (Result == TRUE && bContDown == TRUE) {
		return TotalRead;
	} else {
		return -1;
	}
}

BOOL CALLBACK DGDownProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	TCHAR Mes[128];
	
	switch(iMessage) {
	case WM_INITDIALOG:
		MoveToParentCenter(hDlg);
		SendDlgItemMessage(hDlg, IDC_PRODOWN, PBM_SETRANGE, 0, MAKELPARAM(0,100));
		SendDlgItemMessage(hDlg, IDC_PRODOWN, PBM_SETPOS, 0, 0);
		return TRUE;
	case WM_USER+1:
		if (lParam==-1) {
			wsprintf(Mes,"%d ����Ʈ �ٿ�ε� ��(��ü ũ��� �˼� ����)",wParam);
		} else {
			wsprintf(Mes,"%d ����Ʈ �� %d ����Ʈ ���� ��(%d%%)",
				lParam,wParam,wParam*100/lParam);
			SendDlgItemMessage(hDlg, IDC_PRODOWN, PBM_SETPOS, wParam*100/lParam, 0);
		}
		SetDlgItemText(hDlg,IDC_STDOWN2,Mes);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDCANCEL:
			if (MessageBox(hDlg,"������ ����Ͻðڽ��ϱ�? ���ε��߿� ����Ͻø� ������ "
				"�Ϻθ� �����ǹǷ� �����մϴ�.","����",MB_YESNO)==IDYES) {
				bContDown=FALSE;
				EndDialog(hDlg,IDCANCEL);
			}
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CALLBACK DGFtpConfigProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	static int idx;
	TCHAR szTemp[256];

	switch(iMessage) {
	case WM_INITDIALOG:
		MoveToParentCenter(hDlg);
		idx=(int)lParam;
		if (idx == -1) {
			SetWindowText(hDlg,"�� FTP ���� ���");
			SetDlgItemText(hDlg,IDC_FTPPORT,"21");
			SetDlgItemText(hDlg,IDC_FTPUSER,"Anonymous");
		} else {
			SetWindowText(hDlg,"FTP ���� ���� ����");
			SetDlgItemText(hDlg,IDC_FTPNAME,Option.arFtp[idx].Name);
			SetDlgItemText(hDlg,IDC_FTPADDR,Option.arFtp[idx].Addr);
			SetDlgItemText(hDlg,IDC_FTPUSER,Option.arFtp[idx].User);
			SetDlgItemText(hDlg,IDC_FTPPASS,Option.arFtp[idx].Pass);
			SetDlgItemInt(hDlg,IDC_FTPPORT,Option.arFtp[idx].Port,FALSE);
		}
		SendDlgItemMessage(hDlg,IDC_FTPNAME,EM_SETLIMITTEXT,31,0);
		SendDlgItemMessage(hDlg,IDC_FTPADDR,EM_SETLIMITTEXT,255,0);
		SendDlgItemMessage(hDlg,IDC_FTPUSER,EM_SETLIMITTEXT,31,0);
		SendDlgItemMessage(hDlg,IDC_FTPPASS,EM_SETLIMITTEXT,31,0);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			GetDlgItemText(hDlg,IDC_FTPNAME,szTemp,32);
			if (IsStrEmpty(szTemp)) {
				MessageBox(hDlg,"���Ӹ��� �Է��� �ֽʽÿ�","�˸�",MB_OK);
				return TRUE;
			}
			GetDlgItemText(hDlg,IDC_FTPADDR,szTemp,256);
			if (IsStrEmpty(szTemp)) {
				MessageBox(hDlg,"������ �ּҸ� �Է��� �ֽʽÿ�","�˸�",MB_OK);
				return TRUE;
			}
			GetDlgItemText(hDlg,IDC_FTPUSER,szTemp,32);
			if (IsStrEmpty(szTemp)) {
				MessageBox(hDlg,"����� ID�� �Է��� �ֽʽÿ�","�˸�",MB_OK);
				return TRUE;
			}
			GetDlgItemText(hDlg,IDC_FTPPASS,szTemp,32);
			if (IsStrEmpty(szTemp)) {
				MessageBox(hDlg,"��й�ȣ�� �Է��� �ֽʽÿ�","�˸�",MB_OK);
				return TRUE;
			}
			GetDlgItemText(hDlg,IDC_FTPPORT,szTemp,32);
			if (IsStrEmpty(szTemp)) {
				MessageBox(hDlg,"���� ��Ʈ ��ȣ�� �Է��� �ֽʽÿ�","�˸�",MB_OK);
				return TRUE;
			}
			if (idx==-1) {
				for (idx=0;;idx++) {
					if (lstrlen(Option.arFtp[idx].Name)==0)
						break;
				}
			}
			GetDlgItemText(hDlg,IDC_FTPNAME,Option.arFtp[idx].Name,32);
			StringNormalize(Option.arFtp[idx].Name,NULL);
			GetDlgItemText(hDlg,IDC_FTPADDR,Option.arFtp[idx].Addr,256);
			GetDlgItemText(hDlg,IDC_FTPUSER,Option.arFtp[idx].User,32);
			GetDlgItemText(hDlg,IDC_FTPPASS,Option.arFtp[idx].Pass,32);
			Option.arFtp[idx].Port=GetDlgItemInt(hDlg,IDC_FTPPORT,NULL,FALSE);
			EndDialog(hDlg,IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg,IDCANCEL);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CALLBACK DGFtpProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	int idx;
	int iItem;
	TCHAR Mes[256];
	LVCOLUMN COL;
	LVITEM LI;
	static HWND hList;
	static HIMAGELIST hImgFtp;
	TCHAR Path[MAX_PATH];
	static TCHAR *FtpPath;
	static TCHAR Dir[MAX_PATH];
	TCHAR *p;

	switch(iMessage) {
	case WM_INITDIALOG:
		MoveToParentCenter(hDlg);
		if (hImgFtp==NULL) {
			hImgFtp=ImageList_LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_FTP), 
				16, 1, RGB(255,255,255));
		}
		hList=GetDlgItem(hDlg,IDC_FTPFILE);
		ListView_SetImageList(hList, hImgFtp, LVSIL_SMALL);
		ListView_SetExtendedListViewStyle(hList, LVS_EX_FULLROWSELECT);

		COL.mask=LVCF_FMT | LVCF_WIDTH | LVCF_TEXT |LVCF_SUBITEM;
		COL.fmt=LVCFMT_LEFT;
		COL.cx=250;
		COL.pszText="�̸�";
		COL.iSubItem=0;
		ListView_InsertColumn(hList,0,&COL);

		COL.pszText="ũ��";
		COL.cx=80;
		COL.iSubItem=1;
		ListView_InsertColumn(hList,1,&COL);

		COL.cx=150;
		COL.pszText="��¥";
		COL.iSubItem=2;
		ListView_InsertColumn(hList,2,&COL);

		LoadFtpServerList(hDlg);
		FtpPath=(TCHAR *)lParam;
		DragAcceptFiles(hDlg,TRUE);
		return TRUE;
	case WM_DROPFILES:
		FtpUpLoad(hDlg,hList,(HDROP)wParam);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BTNFTPADD:
			if (SendDlgItemMessage(hDlg,IDC_FTPSERVER,CB_GETCOUNT,0,0) 
				== sizeof(Option.arFtp)/sizeof(Option.arFtp[0])-1) {
				MessageBox(hDlg,"FTP ������ �� ����� �� �����ϴ�.","�˸�",MB_OK);
				return TRUE;
			}
			if (DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_FTPCONFIG), 
				hDlg, DGFtpConfigProc,(LPARAM)-1)==IDOK) {
				LoadFtpServerList(hDlg);
			}
			return TRUE;
		case IDC_BTNFTPEDIT:
			idx=SendDlgItemMessage(hDlg,IDC_FTPSERVER,CB_GETCURSEL,0,0);
			if (DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_FTPCONFIG), 
				hDlg, DGFtpConfigProc,(LPARAM)idx)==IDOK) {
				LoadFtpServerList(hDlg);
			}
			return TRUE;
		case IDC_BTNFTPDELETE:
			idx=SendDlgItemMessage(hDlg,IDC_FTPSERVER,CB_GETCURSEL,0,0);
			wsprintf(Mes,"%s FTP ������ ������ �����Ͻðڽ��ϱ�?",Option.arFtp[idx].Name);
			if (MessageBox(hDlg,Mes,"����",MB_YESNO)==IDYES) {
				for (;;idx++) {
					Option.arFtp[idx]=Option.arFtp[idx+1];
					if (lstrlen(Option.arFtp[idx].Name)==0) {
						break;
					}
				}
				LoadFtpServerList(hDlg);
			}
			return TRUE;
		case IDC_BTNFTPCONNECT:
			if (hFtp==NULL) {
				idx=SendDlgItemMessage(hDlg,IDC_FTPSERVER,CB_GETCURSEL,0,0);
				if (DgFtpConnect(Option.arFtp[idx].Addr,Option.arFtp[idx].User,
					Option.arFtp[idx].Pass,Option.arFtp[idx].Port)==FALSE) {
					MessageBox(hDlg,"FTP ������ ������ �� �����ϴ�","�˸�",MB_OK);
				} else {
					SetWindowText(GetDlgItem(hDlg,IDC_BTNFTPCONNECT),"���� ����(&C)");
					DgFtpEnumFiles(hDlg);
					lstrcpy(Dir,"");
					wsprintf(Mes, "���� ��ġ : %s", Dir);
					SetDlgItemText(hDlg,IDC_STFTPSTATUS,Mes);
					EnableFtpButton(hDlg,TRUE);
				}
			} else {
				DgFtpUnConnect();
				SetWindowText(GetDlgItem(hDlg,IDC_BTNFTPCONNECT),"����(&C)");
				SetDlgItemText(hDlg,IDC_STFTPSTATUS,"���� ���� : ���� �ȵǾ���");
				ListView_DeleteAllItems(GetDlgItem(hDlg,IDC_FTPFILE));
				EnableFtpButton(hDlg,FALSE);
			}
			return TRUE;
		case IDCANCEL:
			if (hFtp) {
				DgFtpUnConnect();
				hImgFtp=NULL;
			}
			EndDialog(hDlg,0);
			return TRUE;
		case IDC_BTNFTPOPEN:
			if (hFtp == NULL) {
				return TRUE;
			}
			iItem=ListView_GetNextItem(hList,-1,LVNI_ALL | LVNI_SELECTED);
			if (iItem == -1) {
				return TRUE;
			}
			if (iItem == 0) {
				if (FtpSetCurrentDirectory(hFtp,"..")==TRUE) {
					DgFtpEnumFiles(hDlg);
					if (lstrlen(Dir)!=0) {
						*(strrchr(Dir, '/'))=0;
					}
					wsprintf(Mes, "���� ��ġ : %s", Dir);
					SetDlgItemText(hDlg,IDC_STFTPSTATUS,Mes);
				}
				return TRUE;
			}

			// ���õ� ù��° �׸��� ���丮�� ���, ������ ����.
			LI.iItem=iItem;
			LI.iSubItem=0;
			LI.pszText=Path;
			LI.cchTextMax=MAX_PATH;
			LI.mask=LVIF_IMAGE | LVIF_TEXT;
			ListView_GetItem(hList,&LI);
			if (LI.iImage == 0) {
				if (FtpSetCurrentDirectory(hFtp,Path)==TRUE) {
					DgFtpEnumFiles(hDlg);
					lstrcat(Dir,"/");
					lstrcat(Dir,Path);
					wsprintf(Mes, "���� ��ġ : %s", Dir);
					SetDlgItemText(hDlg,IDC_STFTPSTATUS,Mes);
				}
				return TRUE;
			}

			memset(FtpPath,0,10000);
			idx=SendDlgItemMessage(hDlg,IDC_FTPSERVER,CB_GETCURSEL,0,0);
			// �� ���� ������ ���
			if (ListView_GetSelectedCount(hList) == 1) {
				LI.iItem=iItem;
				LI.iSubItem=0;
				LI.pszText=Path;
				LI.cchTextMax=255;
				LI.mask=LVIF_TEXT;
				ListView_GetItem(hList,&LI);
				wsprintf(FtpPath,"ftp://%s:%s:%s:%d%s/%s",Option.arFtp[idx].User, 
					Option.arFtp[idx].Pass,Option.arFtp[idx].Addr,
					Option.arFtp[idx].Port,Dir,Path);
			// ���� ���� ������ ���
			} else {
				wsprintf(FtpPath,"ftp://%s:%s:%s:%d%s/",Option.arFtp[idx].User, 
					Option.arFtp[idx].Pass,Option.arFtp[idx].Addr,
					Option.arFtp[idx].Port,Dir);
				p=FtpPath+lstrlen(FtpPath)+1;
				while (iItem != -1) {
					LI.iItem=iItem;
					LI.iSubItem=0;
					LI.pszText=Path;
					LI.cchTextMax=255;
					LI.mask=LVIF_IMAGE | LVIF_TEXT;
					ListView_GetItem(hList,&LI);

					// ������ ����
					if (LI.iImage == 1) {
						lstrcat(p,Path);
						p=p+lstrlen(Path)+1;
					}

					if (p-FtpPath > 10000-MAX_PATH) {
						break;
					}
					iItem=ListView_GetNextItem(hList,iItem,LVNI_ALL | LVNI_SELECTED);
				}
			}
			DgFtpUnConnect();
			EndDialog(hDlg,IDOK);
			hImgFtp=NULL;
			UpdateWindow(g_hFrameWnd);
			return TRUE;
		case IDC_BTNFTPNEW:
			FtpNewFile(hDlg,hList,FALSE);
			return TRUE;
		case IDC_BTNFTPNEWFOLDER:
			FtpNewFile(hDlg,hList,TRUE);
			return TRUE;
		case IDC_BTNFTPDEL:
			FtpDelete(hDlg,hList);
			return TRUE;
		case IDC_BTNFTPRENAME:
			iItem=ListView_GetNextItem(hList,-1,LVNI_ALL | LVNI_SELECTED);
			if (iItem == -1) {
				return TRUE;
			}
			// ������ ������ �� �ݵ�� ��Ŀ���� ������ �־�� �Ѵ�.
			SetFocus(hList);
			ListView_EditLabel(hList,iItem);
			return TRUE;
		case IDC_BTNFTPDOWN:
			FtpDownLoad(hDlg,hList);
			return TRUE;
		case IDC_BTNFTPUP:
			MessageBox(hDlg,"���ε��� �����̳� ������ Ž���⿡�� �巡���Ͽ� �� ��ȭ���ڿ� "
				"����߸��ʽÿ�","�˸�",MB_OK);
			return TRUE;
		}
		return FALSE;
	case WM_NOTIFY:
		LPNMHDR hdr;
		LPNMITEMACTIVATE nia;
		NMLVDISPINFO *ndi;
		hdr=(LPNMHDR)lParam;
		nia=(LPNMITEMACTIVATE)lParam;
		ndi=(NMLVDISPINFO *)lParam;
		if (hdr->hwndFrom == hList) {
			switch (hdr->code) {
			case NM_DBLCLK:
				SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_BTNFTPOPEN,BN_CLICKED),0);
				return TRUE;
			case LVN_BEGINLABELEDIT:
				LI.iItem=ndi->item.iItem;
				LI.iSubItem=0;
				LI.mask=LVIF_IMAGE;
				ListView_GetItem(hList,&LI);
				if (LI.iImage!=2) {
					SetWindowLong(hDlg,DWL_MSGRESULT,FALSE);
				} else {
					SetWindowLong(hDlg,DWL_MSGRESULT,TRUE);
				}
				return TRUE;
			case LVN_ENDLABELEDIT:
				if (ndi->item.pszText == NULL) {
					SetWindowLong(hDlg,DWL_MSGRESULT,FALSE);
				} else {
					LI.iItem=ndi->item.iItem;
					LI.iSubItem=0;
					LI.pszText=Path;
					LI.cchTextMax=MAX_PATH;
					LI.mask=LVIF_TEXT;
					ListView_GetItem(hList,&LI);
					if (FtpRenameFile(hFtp,Path,ndi->item.pszText)==FALSE) {
						SetWindowLong(hDlg,DWL_MSGRESULT,FALSE);
					} else {
						SetWindowLong(hDlg,DWL_MSGRESULT,TRUE);
					}
				}
				return TRUE;
			case LVN_KEYDOWN:
				switch (((NMLVKEYDOWN *)lParam)->wVKey) {
				case VK_DELETE:
					if (IsWindowEnabled(GetDlgItem(hDlg,IDC_BTNFTPDEL))) {
						SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_BTNFTPDEL,BN_CLICKED),0);
					}
					break;
				case VK_F2:
					SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_BTNFTPRENAME,BN_CLICKED),0);
					break;
				default:
					break;
				}
				return TRUE;
			}
		}
		break;
	}
	return FALSE;
}

BOOL DgFtpConnect(TCHAR *Server, TCHAR *User, TCHAR *Pass, int Port)
{
	SetCursor(LoadCursor(NULL,IDC_WAIT));
	hInternet=InternetOpen("Dangeun", INTERNET_OPEN_TYPE_PRECONFIG,
		NULL, NULL, 0);
	if (hInternet == NULL) {
		return FALSE;
	}

	hFtp=InternetConnect(hInternet,Server,Port,User,Pass,INTERNET_SERVICE_FTP,0,0);
	if (hFtp==NULL) {
		InternetCloseHandle(hInternet);
		return FALSE;
	}
	return TRUE;
}

void DgFtpUnConnect()
{
	InternetCloseHandle(hFtp);
	InternetCloseHandle(hInternet);
	hFtp=NULL;
	hInternet=NULL;
}

void DgFtpAddFile(HWND hDlg,WIN32_FIND_DATA wfd)
{
	HWND hList;
	LVITEM LI,LI2;
	int idx;
	TCHAR szTmp[50];
	SYSTEMTIME st;
	TCHAR Text[MAX_PATH];

	hList=GetDlgItem(hDlg,IDC_FTPFILE);
	LI.mask=LVIF_TEXT | LVIF_IMAGE;

	if (strcmp(wfd.cFileName,"���� ���丮�� �̵�")!=0) {
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			LI.iImage=0;
		else
			LI.iImage=1;
	} else {
		LI.iImage=2;
	}
	LI.iSubItem=0;
	LI.pszText=wfd.cFileName;

	if (LI.iImage == 2) {
		idx=0;
	} else {
		idx=1;
		if (LI.iImage == 1) {
			for (;;idx++) {
				LI2.mask=LVIF_IMAGE;
				LI2.iItem=idx;
				LI2.iSubItem=0;
				if (ListView_GetItem(hList,&LI2)==FALSE) {
					break;
				}
				if (LI2.iImage == 1) {
					break;
				}
			}
		}

		for (;;idx++) {
			LI2.mask=LVIF_TEXT | LVIF_IMAGE;
			LI2.iItem=idx;
			LI2.iSubItem=0;
			LI2.pszText=Text;
			LI2.cchTextMax=MAX_PATH;
			if (ListView_GetItem(hList,&LI2)==FALSE) {
				break;
			}
			if (LI.iImage == 0 && LI2.iImage == 1) {
				break;
			}
			if (lstrcmpi(wfd.cFileName, Text) < 0) {
				break;
			}
		}
	}

	LI.iItem=idx;
	ListView_InsertItem(hList,&LI);

	if (strcmp(wfd.cFileName,"���� ���丮�� �̵�")!=0) {
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			lstrcpy(szTmp,"<���丮>");
		} else {
			itoa(wfd.nFileSizeLow,szTmp,10);
		}
		ListView_SetItemText(hList,idx,1,szTmp);
		FileTimeToSystemTime(&wfd.ftLastWriteTime,&st);
		wsprintf(szTmp,"%d�� %d�� %d�� %d�� %d��",st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
		ListView_SetItemText(hList,idx,2,szTmp);
	}
	UpdateWindow(hList);
}

void DgFtpEnumFiles(HWND hDlg)
{
	WIN32_FIND_DATA wfd;
	BOOL bResult=TRUE;
	HINTERNET hFind;

	if (hFtp==NULL)
		return;

	SetCursor(LoadCursor(NULL,IDC_WAIT));
	ListView_DeleteAllItems(GetDlgItem(hDlg,IDC_FTPFILE));
	lstrcpy(wfd.cFileName,"���� ���丮�� �̵�");
	DgFtpAddFile(hDlg,wfd);

	hFind=FtpFindFirstFile(hFtp,"*.*",&wfd,0,0);
	if (hFind!=NULL) {
		DgFtpAddFile(hDlg,wfd);
		while (bResult) {
			bResult=InternetFindNextFile(hFind,&wfd);
			if (bResult==TRUE) {
				DgFtpAddFile(hDlg,wfd);
			}
		}
		InternetCloseHandle(hFind);
	}
}

void LoadFtpServerList(HWND hDlg)
{
	int idx;
	BOOL bEnable;

	SendDlgItemMessage(hDlg,IDC_FTPSERVER,CB_RESETCONTENT,0,0);
	for (idx=0;;idx++) {
		if (lstrlen(Option.arFtp[idx].Name)==0) {
			break;
		}
		SendDlgItemMessage(hDlg,IDC_FTPSERVER,CB_ADDSTRING,0,(LPARAM)Option.arFtp[idx].Name);
	}
	SendDlgItemMessage(hDlg,IDC_FTPSERVER,CB_SETCURSEL,0,0);

	if (SendDlgItemMessage(hDlg,IDC_FTPSERVER,CB_GETCOUNT,0,0) == 0) {
		bEnable=FALSE;
	} else {
		bEnable=TRUE;
	}
	EnableWindow(GetDlgItem(hDlg,IDC_BTNFTPCONNECT),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_BTNFTPEDIT),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_BTNFTPDELETE),bEnable);
}

int DgFtpDown(TCHAR *Path, TCHAR *&Text)
{
	HINTERNET hRemote;
	DWORD FileSize,Size;
	DWORD dwRead;
	BOOL Result=FALSE;
	TCHAR Server[256];
	TCHAR User[32];
	TCHAR Pass[32];
	TCHAR Dir[MAX_PATH];
	int Port;
	TCHAR *p,*p2;
	MSG msg;
	TCHAR Mes[400];
	HWND hDlgDown;
	TCHAR EncPath[MAX_PATH];

	bContDown=TRUE;
	hDlgDown=CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DOWNLOAD), 
		g_hFrameWnd, (DLGPROC)DGDownProc);
	SetDlgItemText(hDlgDown,IDC_STDOWN1,"���ͳ� ������...");
	SetWindowText(hDlgDown,"FTP �ٿ�ε�");
	ShowWindow(hDlgDown,SW_SHOW);
	UpdateWindow(hDlgDown);
	EnableWindow(g_hFrameWnd, FALSE);

	ParseFtpInfo(Path,Server,User,Pass,p,Port);
	if (DgFtpConnect(Server,User,Pass,Port)==FALSE) {
		MessageBox(g_hFrameWnd, "FTP �ٿ�ε带 ���� ���ͳݿ� ������ �� �����ϴ�","�˸�",MB_OK);
		goto NetFail;
	}

	for (;;) {
		p2=strchr(p,'/');
		if (p2==NULL) {
			break;
		}
		lstrcpyn(Dir,p,p2-p+1);
		if (FtpSetCurrentDirectory(hFtp,Dir)==FALSE) {
			MessageBox(g_hFrameWnd,"FTP �������� ���丮�� ã�� �� �����ϴ�.","�˸�",MB_OK);
			goto EndDown;
		}
		p=p2+1;
	}

	WIN32_FIND_DATA wfd;
	HINTERNET hFind;
	hFind=FtpFindFirstFile(hFtp,p,&wfd,0,0);
	if (hFind==NULL) {
		goto EndDown;
	}
	FileSize=wfd.nFileSizeLow;
	InternetCloseHandle(hFind);

	hRemote=FtpOpenFile(hFtp,p,GENERIC_READ,FTP_TRANSFER_TYPE_BINARY,0);
	if (hRemote == NULL) {
		MessageBox(g_hFrameWnd,"FTP �������� ������ ã�� �� ���ų� ������ �����ϴ�.","�˸�",MB_OK);
		goto EndDown;
	}

	if (FileSize > 1048576 * 30) {
		MessageBox(g_hFrameWnd,"FTP�δ� �ִ� 30M������ �� �� �ֽ��ϴ�.","�˸�",MB_OK);
		goto EndDown;
	}

	EncodePath(Path,EncPath);
	wsprintf(Mes,"��ġ : %s",EncPath);
	SetDlgItemText(hDlgDown,IDC_STDOWN1,Mes);

	Text=(TCHAR *)malloc(FileSize+2);
	memset(Text,0,FileSize+2);
	p=Text;

	for (;;) {
		InternetQueryDataAvailable(hRemote,&Size,0,0);
		Result=InternetReadFile(hRemote,p,Size,&dwRead);
		if (Result==FALSE) {
			MessageBox(g_hFrameWnd, "FTP ������ ������ ���� �� �����ϴ�","�˸�",MB_OK);
			break;
		}
		if (bContDown==FALSE) {
			break;
		}
		if (dwRead==0) {
			break;
		}
		p+=dwRead;
		SendMessage(hDlgDown,WM_USER+1,(WPARAM)(p-Text),(LPARAM)FileSize);
		while (PeekMessage(&msg, NULL,0,0,PM_REMOVE)) {
			if (!IsDialogMessage(hDlgDown, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	InternetCloseHandle(hRemote);
EndDown:
	DgFtpUnConnect();
NetFail:
	EnableWindow(g_hFrameWnd, TRUE);
	DestroyWindow(hDlgDown);
	if (Result == TRUE && bContDown == TRUE) {
		return FileSize;
	} else {
		return -1;
	}
}

BOOL DgFtpUp(TCHAR *Path, TCHAR *Text)
{
	HINTERNET hRemote;
	DWORD FileSize,Size, RemSize;
	DWORD dwWritten;
	BOOL Result=FALSE;
	TCHAR Server[256];
	TCHAR User[32];
	TCHAR Pass[32];
	TCHAR Dir[MAX_PATH];
	int Port;
	TCHAR *p,*p2;
	MSG msg;
	TCHAR Mes[400];
	HWND hDlgDown;
	TCHAR EncPath[MAX_PATH];

	bContDown=TRUE;
	hDlgDown=CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DOWNLOAD), 
		g_hFrameWnd, (DLGPROC)DGDownProc);
	SetDlgItemText(hDlgDown,IDC_STDOWN1,"���ͳ� ������...");
	SetWindowText(hDlgDown,"FTP ���ε�");
	ShowWindow(hDlgDown,SW_SHOW);
	UpdateWindow(hDlgDown);
	EnableWindow(g_hFrameWnd, FALSE);

	ParseFtpInfo(Path,Server,User,Pass,p,Port);
	if (DgFtpConnect(Server,User,Pass,Port)==FALSE) {
		MessageBox(g_hFrameWnd, "FTP ���ε带 ���� ���ͳݿ� ������ �� �����ϴ�","�˸�",MB_OK);
		goto NetFail;
	}

	for (;;) {
		p2=strchr(p,'/');
		if (p2==NULL) {
			break;
		}
		lstrcpyn(Dir,p,p2-p+1);
		if (FtpSetCurrentDirectory(hFtp,Dir)==FALSE) {
			MessageBox(g_hFrameWnd,"FTP �������� ���丮�� ã�� �� �����ϴ�.","�˸�",MB_OK);
			goto EndUp;
		}
		p=p2+1;
	}

	hRemote=FtpOpenFile(hFtp,p,GENERIC_WRITE,FTP_TRANSFER_TYPE_BINARY,0);
	if (hRemote == NULL) {
		MessageBox(g_hFrameWnd,"FTP �������� ������ ã�� �� ���ų� ���� ������ �����ϴ�.","�˸�",MB_OK);
		goto EndUp;
	}

	EncodePath(Path,EncPath);
	wsprintf(Mes,"��ġ : %s",EncPath);
	SetDlgItemText(hDlgDown,IDC_STDOWN1,Mes);

	p=Text;
	FileSize=RemSize=lstrlen(Text);
	for (;;) {
		Size=min(10000,RemSize);
		if (Size==0) {
			Result=TRUE;
			break;
		}
		Result=InternetWriteFile(hRemote,p,Size,&dwWritten);
		if (Result==FALSE) {
			MessageBox(g_hFrameWnd, "FTP ������ ������ �� �� �����ϴ�","�˸�",MB_OK);
			break;
		}
		if (bContDown==FALSE) {
			Result=FALSE;
			break;
		}
		p+=dwWritten;
		RemSize-=dwWritten;
		SendMessage(hDlgDown,WM_USER+1,(WPARAM)(p-Text),(LPARAM)FileSize);
		while (PeekMessage(&msg, NULL,0,0,PM_REMOVE)) {
			if (!IsDialogMessage(hDlgDown, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	InternetCloseHandle(hRemote);
EndUp:
	DgFtpUnConnect();
NetFail:
	EnableWindow(g_hFrameWnd, TRUE);
	DestroyWindow(hDlgDown);
	return Result;
}

void EnableFtpButton(HWND hDlg,BOOL bEnable)
{
	EnableWindow(GetDlgItem(hDlg,IDC_BTNFTPOPEN),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_BTNFTPNEW),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_BTNFTPNEWFOLDER),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_BTNFTPRENAME),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_BTNFTPDEL),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_BTNFTPDOWN),bEnable);
	EnableWindow(GetDlgItem(hDlg,IDC_BTNFTPUP),bEnable);
}

void FtpNewFile(HWND hDlg,HWND hList,BOOL bFolder)
{
	TCHAR Name[10];
	TCHAR Path[MAX_PATH];
	LVFINDINFO fi;
	int i,idx;
	HINTERNET hRemote;

	if (bFolder) {
		lstrcpy(Name,"������");
	} else {
		lstrcpy(Name,"������");
	}

	// �̸��� ã�´�
	fi.flags=LVFI_STRING;
	for (i=0;;i++) {
		if (i==0) {
			lstrcpy(Path,Name);
		} else {
			wsprintf(Path,"%s%d",Name,i);
		}
		fi.psz=Path;
		idx=ListView_FindItem(hList,-1,&fi);
		if (idx==-1) {
			break;
		}
	}
	if (bFolder) {
		if (FtpCreateDirectory(hFtp,Path)==FALSE) {
			MessageBox(hDlg,"FTP ������ ������ ������ �� �����ϴ�","�˸�",MB_OK);
			return;
		}
	} else {
		hRemote=FtpOpenFile(hFtp,Path,GENERIC_WRITE,FTP_TRANSFER_TYPE_BINARY,0);
		if (hRemote==NULL) {
			MessageBox(hDlg,"FTP ������ ������ ������ �� �����ϴ�","�˸�",MB_OK);
			return;
		}
		InternetCloseHandle(hRemote);
	}
	DgFtpEnumFiles(hDlg);
	idx=ListView_FindItem(hList,-1,&fi);
	SetFocus(hList);
	ListView_EditLabel(hList,idx);
}

void FtpDelete(HWND hDlg,HWND hList)
{
	int iItem;
	LVITEM LI;
	TCHAR Path[MAX_PATH];

	if (MessageBox(hDlg,"������ ���ϵ��� FTP �������� �����Ͻðڽ��ϱ�?\r\n"
		"�������� ������ ������ �ٽ� ������ �� �����ϴ�.",
		"����",MB_YESNO)==IDNO) {
		return;
	}

	SetCursor(LoadCursor(NULL,IDC_WAIT));
	iItem=ListView_GetNextItem(hList,-1,LVNI_ALL | LVNI_SELECTED);
	while (iItem!=-1) {
		LI.iItem=iItem;
		LI.iSubItem=0;
		LI.pszText=Path;
		LI.cchTextMax=MAX_PATH;
		LI.mask=LVIF_IMAGE | LVIF_TEXT;
		ListView_GetItem(hList,&LI);
		// ��Ʈ�� ���õǾ� �־ �ƹ��͵� ���� �ʴ´�.
		if (LI.iImage==0) {
			if (FtpDeleteFolder(Path)==FALSE) {
				break;
			}
		} else if (LI.iImage==1) {
			if (FtpDeleteFile(hFtp,Path)==FALSE) {
				break;
			}
		}
		iItem=ListView_GetNextItem(hList,iItem,LVNI_ALL | LVNI_SELECTED);
	}
	DgFtpEnumFiles(hDlg);
}

BOOL FtpDeleteFolder(TCHAR *Path)
{
	WIN32_FIND_DATA wfd, *arwfd;
	int wfdsize,n,i;
	BOOL bResult=TRUE;
	BOOL Result=TRUE;
	HINTERNET hFind;

	FtpSetCurrentDirectory(hFtp,Path);

	hFind=FtpFindFirstFile(hFtp,"*.*",&wfd,0,0);
	if (hFind==NULL) {
		FtpSetCurrentDirectory(hFtp,"..");
		return FALSE;
	}
	wfdsize=100;
	arwfd=(WIN32_FIND_DATA *)malloc(wfdsize*sizeof(WIN32_FIND_DATA));
	for (n=0;bResult;n++) {
		if (wfdsize >= n) {
			wfdsize+=100;
			arwfd=(WIN32_FIND_DATA *)realloc(arwfd,wfdsize*sizeof(WIN32_FIND_DATA));
		}
		arwfd[n]=wfd;
		bResult=InternetFindNextFile(hFind,&wfd);
	}
	InternetCloseHandle(hFind);

	for (i=0;i<n;i++) {
		if (arwfd[i].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (FtpDeleteFolder(arwfd[i].cFileName)==FALSE) {
				Result=FALSE;
				break;
			}
		} else {
			if (FtpDeleteFile(hFtp,arwfd[i].cFileName)==FALSE) {
				Result=FALSE;
				break;
			}
		}
	}
	free(arwfd);

	// ���� ���丮�� �̵��� �� �ڱ� �ڽ� ����
	FtpSetCurrentDirectory(hFtp,"..");
	if (FtpRemoveDirectory(hFtp,Path)==FALSE) {
		Result=FALSE;
	}
	return Result;
}

void FtpDownLoad(HWND hDlg,HWND hList)
{
	int iItem;
	LVITEM LI;
	TCHAR Path[MAX_PATH];
	TCHAR DownPath[MAX_PATH];
	TCHAR szSize[16];

	if (BrowseFolder(hDlg,"�������� ������ ������ ������ ������ �ֽʽÿ�",
		"c:\\",DownPath)==FALSE) {
		return;
	}

	// ������ ��ȭ���� ǥ��
	bContDown=TRUE;
	hDlgDown=CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DOWNLOAD), 
		g_hFrameWnd, (DLGPROC)DGDownProc);
	SetDlgItemText(hDlgDown,IDC_STDOWN1,"���ͳ� ������...");
	SetWindowText(hDlgDown,"FTP �ٿ�ε�");
	ShowWindow(hDlgDown,SW_SHOW);
	UpdateWindow(hDlgDown);
	EnableWindow(hDlg, FALSE);

	iItem=ListView_GetNextItem(hList,-1,LVNI_ALL | LVNI_SELECTED);
	while (iItem!=-1) {
		LI.iItem=iItem;
		LI.iSubItem=0;
		LI.pszText=Path;
		LI.cchTextMax=MAX_PATH;
		LI.mask=LVIF_IMAGE | LVIF_TEXT;
		ListView_GetItem(hList,&LI);
		// ��Ʈ�� ���õǾ� �־ �ƹ��͵� ���� �ʴ´�.
		if (LI.iImage==0) {
			if (FtpDownLoadFolder(Path,DownPath)==FALSE) {
				break;
			}
		} else if (LI.iImage==1) {
			// ũ��� ���� ���� ��� �Ѵ�. FtpFirstFile�� �ѹ��ۿ� ȣ�� ����
			ListView_GetItemText(hList,iItem,2,szSize,16);
			if (FtpDownLoadFile(Path,DownPath,atoi(szSize))==FALSE) {
				break;
			}
		}
		iItem=ListView_GetNextItem(hList,iItem,LVNI_ALL | LVNI_SELECTED);
	}

	EnableWindow(hDlg, TRUE);
	DestroyWindow(hDlgDown);
}

BOOL FtpDownLoadFile(TCHAR *Path,TCHAR *DownPath,unsigned FileSize)
{
	HINTERNET hRemote=NULL;
	TCHAR tPath[MAX_PATH];
	HANDLE hLocal=INVALID_HANDLE_VALUE;
	DWORD Size,DnSize=0;
	DWORD dwRead,dwWritten;
	BOOL Result=FALSE;
	PVOID pbuf=NULL;
	MSG msg;
	DWORD OldTick;
	TCHAR Mes[MAX_PATH+64];

	hRemote=FtpOpenFile(hFtp,Path,GENERIC_READ,FTP_TRANSFER_TYPE_BINARY,0);
	if (hRemote == NULL) {
		Result=FALSE;
		goto EndDown;
	}

	if (lstrlen(DownPath)==3) {
		DownPath[2]=0;
	}
	wsprintf(tPath,"%s\\%s",DownPath,Path);
	hLocal=CreateFile(tPath,GENERIC_WRITE,0,NULL,
		CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hLocal==INVALID_HANDLE_VALUE) {
		Result=FALSE;
		goto EndDown;
	}

	wsprintf(Mes,"��� ���� : %s",Path);
	SetDlgItemText(hDlgDown,IDC_STDOWN1,Mes);

	OldTick=GetTickCount();
	for (;;) {
		InternetQueryDataAvailable(hRemote,&Size,0,0);
		if (pbuf==NULL || _msize(pbuf) < Size) {
			pbuf=(PVOID)realloc(pbuf,Size);
		}
		Result=InternetReadFile(hRemote,pbuf,Size,&dwRead);
		if (Result==FALSE) {
			goto EndDown;
		}
		if (bContDown==FALSE) {
			Result=FALSE;
			goto EndDown;
		}
		if (dwRead==0) {
			goto EndDown;
		}
		WriteFile(hLocal,pbuf,dwRead,&dwWritten,NULL);
		DnSize+=dwRead;

		// �ʹ� ���� �������� �ʵ��� �Ѵ�.
		if (GetTickCount()!=OldTick) {
			SendMessage(hDlgDown,WM_USER+1,(WPARAM)DnSize,(LPARAM)FileSize);
			OldTick=GetTickCount();
		}
		while (PeekMessage(&msg, NULL,0,0,PM_REMOVE)) {
			if (!IsDialogMessage(hDlgDown, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

EndDown:
	if (hRemote) InternetCloseHandle(hRemote);
	if (hLocal != INVALID_HANDLE_VALUE) CloseHandle(hLocal);
	if (pbuf) free(pbuf);
	return Result;
}

BOOL FtpDownLoadFolder(TCHAR *Path,TCHAR *DownPath)
{
	WIN32_FIND_DATA wfd, *arwfd;
	int wfdsize,n,i;
	BOOL bResult=TRUE;
	BOOL Result=TRUE;
	HINTERNET hFind;
	TCHAR NewPath[MAX_PATH];

	// ���丮 ������ ���� ���ÿ� ������ �����.
	FtpSetCurrentDirectory(hFtp,Path);
	if (lstrlen(DownPath)==3) {
		wsprintf(NewPath,"%s%s",DownPath,Path);
	} else {
		wsprintf(NewPath,"%s\\%s",DownPath,Path);
	}
	CreateDirectory(NewPath,NULL);

	// �ϴ� ���� ��ȸ�ϸ鼭 ����� �����Ѵ�.
	hFind=FtpFindFirstFile(hFtp,"*.*",&wfd,0,0);
	if (hFind==NULL) {
		FtpSetCurrentDirectory(hFtp,"..");
		return FALSE;
	}
	wfdsize=100;
	arwfd=(WIN32_FIND_DATA *)malloc(wfdsize*sizeof(WIN32_FIND_DATA));
	for (n=0;bResult;n++) {
		if (wfdsize >= n) {
			wfdsize+=100;
			arwfd=(WIN32_FIND_DATA *)realloc(arwfd,wfdsize*sizeof(WIN32_FIND_DATA));
		}
		arwfd[n]=wfd;
		bResult=InternetFindNextFile(hFind,&wfd);
	}
	InternetCloseHandle(hFind);

	for (i=0;i<n;i++) {
		if (arwfd[i].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (FtpDownLoadFolder(arwfd[i].cFileName,NewPath)==FALSE) {
				Result=FALSE;
				break;
			}
		} else {
			if (FtpDownLoadFile(arwfd[i].cFileName,NewPath,arwfd[i].nFileSizeLow)==FALSE) {
				Result=FALSE;
				break;
			}
		}
	}
	free(arwfd);

	// ���� ���丮�� �ٽ� �̵�
	FtpSetCurrentDirectory(hFtp,"..");
	return Result;
}

void FtpUpLoad(HWND hDlg,HWND hList,HDROP hDrop)
{
	TCHAR Path[MAX_PATH];
	int Count,n;

	// ������ ��ȭ���� ǥ��
	bContDown=TRUE;
	hDlgDown=CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DOWNLOAD), 
		g_hFrameWnd, (DLGPROC)DGDownProc);
	SetDlgItemText(hDlgDown,IDC_STDOWN1,"���ͳ� ������...");
	SetWindowText(hDlgDown,"FTP ���ε�");
	ShowWindow(hDlgDown,SW_SHOW);
	UpdateWindow(hDlgDown);
	EnableWindow(hDlg, FALSE);

	Count=DragQueryFile(hDrop,0xffffffff,NULL,0);
	for (n=0;n<Count;n++) {
		DragQueryFile(hDrop,n,Path,MAX_PATH);
		if (GetFileAttributes(Path) & FILE_ATTRIBUTE_DIRECTORY) {
			if (FtpUpLoadFolder(Path)==FALSE) {
				break;
			}
		} else {
			if (FtpUpLoadFile(Path)==FALSE) {
				break;
			}
		}
	}
	EnableWindow(hDlg, TRUE);
	DestroyWindow(hDlgDown);
	DgFtpEnumFiles(hDlg);
}

BOOL FtpUpLoadFile(TCHAR *Path)
{
	HINTERNET hRemote=NULL;
	HANDLE hLocal=INVALID_HANDLE_VALUE;
	DWORD FileSize,Size, RemSize;
	DWORD dwRead,dwWritten;
	BOOL Result=FALSE;
	PVOID pbuf=NULL;
	MSG msg;
	DWORD OldTick;
	TCHAR Mes[MAX_PATH+64];
	TCHAR Ext[_MAX_EXT];
	TCHAR Name[MAX_PATH];

	hLocal=CreateFile(Path,GENERIC_READ,0,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hLocal==INVALID_HANDLE_VALUE) {
		Result=FALSE;
		goto EndUp;
	}
	
	_splitpath(Path,NULL,NULL,Name,Ext);
	lstrcat(Name,Ext);
	hRemote=FtpOpenFile(hFtp,Name,GENERIC_WRITE,FTP_TRANSFER_TYPE_BINARY,0);
	if (hRemote == NULL) {
		Result=FALSE;
		goto EndUp;
	}

	wsprintf(Mes,"��� ���� : %s",Path);
	SetDlgItemText(hDlgDown,IDC_STDOWN1,Mes);

	FileSize=RemSize=GetFileSize(hLocal,NULL);
	pbuf=malloc(10000);
	OldTick=GetTickCount();
	for (;;) {
		Size=min(10000,RemSize);
		if (Size==0) {
			Result=TRUE;
			break;
		}
		ReadFile(hLocal,pbuf,Size,&dwRead,NULL);
		Result=InternetWriteFile(hRemote,pbuf,Size,&dwWritten);
		if (Result==FALSE) {
			Result=FALSE;
			goto EndUp;
		}
		if (bContDown==FALSE) {
			Result=FALSE;
			goto EndUp;
		}
		RemSize-=dwWritten;
		// �ʹ� ���� �������� �ʵ��� �Ѵ�.
		if (GetTickCount()!=OldTick) {
			SendMessage(hDlgDown,WM_USER+1,(WPARAM)FileSize-RemSize,(LPARAM)FileSize);
			OldTick=GetTickCount();
		}
		while (PeekMessage(&msg, NULL,0,0,PM_REMOVE)) {
			if (!IsDialogMessage(hDlgDown, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

EndUp:
	if (hRemote) InternetCloseHandle(hRemote);
	if (hLocal != INVALID_HANDLE_VALUE) CloseHandle(hLocal);
	if (pbuf) free(pbuf);
	return Result;
}

BOOL FtpUpLoadFolder(TCHAR *Path)
{
	WIN32_FIND_DATA wfd;
	BOOL bResult=TRUE;
	BOOL Result=TRUE;
	TCHAR SrchPath[MAX_PATH];
	TCHAR NewPath[MAX_PATH];
	TCHAR Ext[_MAX_EXT];
	TCHAR Name[MAX_PATH];
	HANDLE hFind;

	// ���� ���� �˻� ����
	wsprintf(SrchPath,"%s\\*.*",Path);
	hFind=FindFirstFile(SrchPath,&wfd);
	if (hFind == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	// ������ ����� ������ ����.
	_splitpath(Path,NULL,NULL,Name,Ext);
	lstrcat(Name,Ext);
	FtpCreateDirectory(hFtp,Name);
	FtpSetCurrentDirectory(hFtp,Name);

	while (bResult) {
		wsprintf(NewPath,"%s\\%s",Path,wfd.cFileName);
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (wfd.cFileName[0]!='.') {
				if (FtpUpLoadFolder(NewPath)==FALSE) {
					Result=FALSE;
					break;
				}
			}
		} else {
			if (FtpUpLoadFile(NewPath)==FALSE) {
				Result=FALSE;
				break;
			}
		}
		bResult=FindNextFile(hFind,&wfd);
	}
	FindClose(hFind);

	// ���� ���丮�� �ٽ� �̵�
	FtpSetCurrentDirectory(hFtp,"..");
	return Result;
}
