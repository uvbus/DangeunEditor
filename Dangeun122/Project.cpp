#include "stdafx.h"

BOOL CALLBACK DgProjectProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	static HWND hPrjTree;
	static BOOL Modified;
	static BOOL bAbsolute;
	TCHAR Project[MAX_PATH];
	OPENFILENAME OFN;
	TCHAR *Files;
	TCHAR Dir[MAX_PATH];
	TCHAR Path[MAX_PATH];
	TCHAR *p;
	TVITEM TV;
	TVINSERTSTRUCT TI;
	HTREEITEM Node,NewNode;
	static HIMAGELIST himl;
	static BOOL bDrag;
	static HTREEITEM hDrag,hTarget, hOldTarget;
	TVHITTESTINFO hti;
	POINT pt;
	int hotx,hoty;
	RECT rt;
	BOOL bNext;
	int Count,n;

	switch(iMessage) {
	case WM_INITDIALOG:
		MoveToParentCenter(hDlg);
		hPrjTree=GetDlgItem(hDlg,IDC_PRJTREE);
		SendMessage(hPrjTree, TVM_SETIMAGELIST, (WPARAM)TVSIL_NORMAL, (LPARAM)hFileImg);

		if (lParam) {
			if (OpenProject(hDlg,(TCHAR *)lParam,bAbsolute)==FALSE) {
				lParam=NULL;
			}
		}

		if (lParam == NULL) {
			EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJEDIT),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJTEST),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJFILE),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJFTP),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJHTTP),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJDELETE),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJSUB),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJFOLDER),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJRENAME),FALSE);
		}
		Modified=FALSE;
		bDrag=FALSE;
		DragAcceptFiles(hDlg,TRUE);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BTNPRJNEW:
			if (Modified) {
				if (MessageBox(hDlg,"편집중인 프로젝트를 저장하시겠습니까?","질문",MB_YESNO)==IDYES) {
					GetDlgItemText(hDlg,IDC_PRJFILE,Project,MAX_PATH);
					SaveProject(hDlg,Project);
					LoadProject(Project,TRUE);
				}
			}
			lstrcpy(Project,"");
			if (DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_NEWPROJECT), 
				hDlg, DgNewProjectProc,(LPARAM)Project)==IDOK) {
				OpenProject(hDlg,Project,bAbsolute);
				Modified=FALSE;
			}
			return TRUE;
		case IDC_BTNPRJOPEN:
			if (Modified) {
				if (MessageBox(hDlg,"편집중인 프로젝트를 저장하시겠습니까?","질문",MB_YESNO)==IDYES) {
					GetDlgItemText(hDlg,IDC_PRJFILE,Project,MAX_PATH);
					SaveProject(hDlg,Project);
					LoadProject(Project,TRUE);
				}
			}
			memset(&OFN, 0, sizeof(OPENFILENAME));
			OFN.lStructSize = sizeof(OPENFILENAME);
			OFN.hwndOwner=hDlg;
			OFN.lpstrFilter="당근 프로젝트(*.dgp)\0*.dgp*\0";
			lstrcpy(Project,"");
			OFN.lpstrFile=Project;
			OFN.nMaxFile=MAX_PATH;
			if (GetOpenFileName(&OFN)) {
				if (OpenProject(hDlg,Project,bAbsolute)==TRUE) {
					Modified=FALSE;
				}
			}
			return TRUE;
		case IDC_BTNPRJEDIT:
			GetDlgItemText(hDlg,IDC_PRJFILE,Project,MAX_PATH);
			if (Modified) {
				if (MessageBox(hDlg,"현재까지의 변경 사항을 저장하셔야 프로젝트 속성(경로 설정)"
					"을 편집할 수 있습니다. 저장하시겠습니까?","질문",MB_YESNO)==IDYES) {
					SaveProject(hDlg,Project);
				} else {
					return TRUE;
				}
			}
			if (DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_NEWPROJECT), 
				hDlg, DgNewProjectProc,(LPARAM)Project)==IDOK) {
				OpenProject(hDlg,Project,bAbsolute);
				Modified=TRUE;
			}
			return TRUE;
		case IDC_BTNPRJSUB:
			Modified=TRUE;
			TI.hParent=TreeView_GetSelection(hPrjTree);
			TI.hInsertAfter=TVI_LAST;
			TI.item.mask=TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			TI.item.iImage=DGI_SUB;
			TI.item.iSelectedImage=DGI_SUBSEL;
			TI.item.pszText="새 묶음";
			NewNode=TreeView_InsertItem(hPrjTree, &TI);
			TreeView_SelectItem(hPrjTree,NewNode);
			TreeView_EditLabel(hPrjTree,NewNode);
			return TRUE;
		case IDC_BTNPRJFILE:
			memset(&OFN, 0, sizeof(OPENFILENAME));
			OFN.lStructSize = sizeof(OPENFILENAME);
			OFN.hwndOwner=hDlg;
			OFN.lpstrFilter="모든 파일(*.*)\0*.*\0텍스트 파일\0*.txt\0";
			Files=(TCHAR *)malloc(100000);
			Files[0]=0;
			OFN.lpstrFile=Files;
			OFN.nMaxFile=100000;
			OFN.Flags=OFN_EXPLORER | OFN_ALLOWMULTISELECT;
			GetDlgItemText(hDlg,IDC_PRJFILE,Project,MAX_PATH);
			OFN.lpstrInitialDir=Project;

			if (GetOpenFileName(&OFN)) {
				Modified=TRUE;
				p=Files;
				lstrcpy(Dir,p);
				p=p+lstrlen(Dir)+1;
				if (*p==0) {
					AddFileToProject(hDlg,Dir,bAbsolute);
				} else {
					for (;*p;) {
						wsprintf(Path,"%s\\%s",Dir,p);
						p=p+lstrlen(p)+1;
						AddFileToProject(hDlg,Path,bAbsolute);
					}
				}
			} else {
				if (CommDlgExtendedError()==FNERR_BUFFERTOOSMALL) {
					MessageBox(hDlg,"한 번에 파일을 너무 많이 선택하셨습니다.",
						"알림",MB_OK);
				}
			}
			free(Files);
			return TRUE;
		case IDC_BTNPRJFTP:
			TCHAR *FtpPath,*p;
			TCHAR Dir[MAX_PATH+64];
			FtpPath=(TCHAR *)malloc(10000);
			if (DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_FTP), 
				g_hFrameWnd, DGFtpProc,(LPARAM)FtpPath)==IDOK) {
				Modified=TRUE;
				p=FtpPath;
				lstrcpy(Dir,p);
				p=p+lstrlen(Dir)+1;
				if (*p==0) {
					AddFileToProject(hDlg,Dir,TRUE);
				} else {
					for (;*p;) {
						wsprintf(Path,"%s%s",Dir,p);
						p=p+lstrlen(p)+1;
						AddFileToProject(hDlg,Path,TRUE);
					}
				}
			}
			free(FtpPath);
			return TRUE;
		case IDC_BTNPRJHTTP:
			if (DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_HTTP), 
				hDlg, DGHttpProc,(LPARAM)Path)==IDOK) {
				if (lstrlen(Path)) {
					Modified=TRUE;
					AddFileToProject(hDlg,Path,TRUE);
				}
			}
			return TRUE;
		case IDC_BTNPRJFOLDER:
			GetDlgItemText(hDlg,IDC_PRJFILE,Project,MAX_PATH);
			if (BrowseFolder(hDlg,"프로젝트에 추가할 폴더 링크를 선택하십시오.",Project,Path)==TRUE) {
				Modified=TRUE;
				AddFileToProject(hDlg,Path,bAbsolute);
			}
			return TRUE;
		case IDC_BTNPRJTEST:
			if (TestProjectValid(hDlg,NULL)==TRUE) {
				MessageBox(hDlg,"프로젝트내의 모든 파일들이 존재하며 읽을 수 있습니다","검사 결과",MB_OK);
			}
			return TRUE;
		case IDC_BTNPRJDELETE:
			Modified=TRUE;
			Node=TreeView_GetSelection(hPrjTree);
			TreeView_DeleteItem(hPrjTree,Node);
			return TRUE;
		case IDC_BTNPRJRENAME:
			Node=TreeView_GetSelection(hPrjTree);
			TreeView_EditLabel(hPrjTree,Node);
			return TRUE;
		case IDOK:
			if (TreeView_GetEditControl(hPrjTree)) {
				if (HIWORD(wParam) == 0) {
					TreeView_EndEditLabelNow(hPrjTree,FALSE);
				}
			} else {
				if (Modified) {
					GetDlgItemText(hDlg,IDC_PRJFILE,Project,MAX_PATH);
					SaveProject(hDlg,Project);
					LoadProject(Project,TRUE);
				}
				EndDialog(hDlg,IDOK);
			}
			return TRUE;
		case IDCANCEL:
			if (TreeView_GetEditControl(hPrjTree)) {
				TreeView_EndEditLabelNow(hPrjTree,TRUE);
			} else {
				EndDialog(hDlg,IDCANCEL);
			}
			return TRUE;
		}
		return FALSE;
	case WM_DROPFILES:
		if (GetWindowTextLength(GetDlgItem(hDlg,IDC_PRJFILE))) {
			Count=DragQueryFile((HDROP)wParam,0xffffffff,NULL,0);

			for (n=0;n<Count;n++) {
				Modified=TRUE;
				DragQueryFile((HDROP)wParam,n,Path,MAX_PATH);
				AddFileToProject(hDlg,Path,bAbsolute);
			}
		}
		return TRUE;
	case WM_MOUSEMOVE:
		if (bDrag) {
			pt.x=LOWORD(lParam);
			pt.y=HIWORD(lParam);
			ClientToScreen(hDlg,&pt);
			ImageList_DragMove(pt.x,pt.y);

			GetWindowRect(hPrjTree,&rt);
			ScreenToClient(hDlg,(POINT *)&rt);
			hti.pt.x=LOWORD(lParam)-rt.left;
			hti.pt.y=HIWORD(lParam)-rt.top;
			hTarget=TreeView_HitTest(hPrjTree,&hti);
			if (hTarget != NULL) {
				ImageList_DragLeave(NULL);
				TreeView_SelectDropTarget(hPrjTree,hTarget);
				UpdateWindow(hPrjTree);
				ImageList_DragEnter(NULL,pt.x,pt.y);

				if (hTarget != hOldTarget) {
					hOldTarget=hTarget;
					KillTimer(hDlg,1);
					SetTimer(hDlg,1,1000,NULL);
				}
			}

			GetWindowRect(hPrjTree,&rt);
			if ((short)pt.y < rt.top) {
				SetTimer(hDlg,2,200,NULL);
				ImageList_DragLeave(NULL);
				SendMessage(hPrjTree,WM_VSCROLL,MAKEWPARAM(SB_LINEUP,0),NULL);
				ImageList_DragEnter(NULL,pt.x,pt.y);
			} else {
				KillTimer(hDlg,2);
			}

			if ((short)pt.y > rt.bottom) {
				SetTimer(hDlg,3,200,NULL);
				ImageList_DragLeave(NULL);
				SendMessage(hPrjTree,WM_VSCROLL,MAKEWPARAM(SB_LINEDOWN,0),NULL);
				ImageList_DragEnter(NULL,pt.x,pt.y);
			} else {
				KillTimer(hDlg,3);
			}
		}
		return TRUE;
	case WM_LBUTTONUP:
		if (bDrag) {
			ImageList_DragLeave(NULL);
			ImageList_EndDrag();
			TreeView_SelectDropTarget(hPrjTree,NULL);
			ReleaseCapture();
			bDrag=FALSE;
			ImageList_Destroy(himl);
			KillTimer(hDlg,1);
			KillTimer(hDlg,2);
			KillTimer(hDlg,3);

			if (hDrag == hTarget || hTarget==NULL)
				return TRUE;

			Node=hTarget;
			while (Node != NULL) {
				Node=TreeView_GetParent(hPrjTree,Node);
				if (Node == hDrag)
					return TRUE;
			}

			TV.mask=TVIF_IMAGE;
			TV.hItem=hTarget;
			TreeView_GetItem(hPrjTree,&TV);
			if (TV.iImage==DGI_SUB) {
				switch (MessageBox(hDlg,"항목을 하위 묶음에 떨어뜨렸습니다. "
					"어떻게 하시겠습니까?\r\n\r\n예=항목을 하위 묶음 안에 집어 넣는다.\r\n"
					"아니오=항목을 하위 묶음 다음으로 옮긴다","질문",MB_YESNOCANCEL)) {
				case IDCANCEL:
					return TRUE;
				case IDYES:
					bNext=FALSE;
					break;
				case IDNO:
					bNext=TRUE;
					break;
				}
			}

			if (TV.iImage==DGI_PROJECT) {
				bNext=FALSE;
			}

			LockWindowUpdate(hPrjTree);
			Node=CopyTreeItems(hPrjTree, hDrag, hTarget,bNext);
			TreeView_EnsureVisible(hPrjTree,Node);
			TreeView_DeleteItem(hPrjTree,hDrag);
			LockWindowUpdate(NULL);

			Modified=TRUE;
		}
		return TRUE;
	case WM_TIMER:
		if (wParam == 1) {
			Node=TreeView_GetDropHilight(hPrjTree);
			if ((Node != NULL) && (Node == hTarget)) {
				ImageList_DragShowNolock(FALSE);
				TreeView_Expand(hPrjTree,hTarget,TVE_EXPAND);
				UpdateWindow(hPrjTree);
				ImageList_DragShowNolock(TRUE);
			}
		}
		if (wParam == 2) {
			SendMessage(hPrjTree,WM_VSCROLL,MAKEWPARAM(SB_LINEUP,0),NULL);
		}
		if (wParam == 3) {
			SendMessage(hPrjTree,WM_VSCROLL,MAKEWPARAM(SB_LINEDOWN,0),NULL);
		}
		return TRUE;
	case WM_NOTIFY:
		LPNMHDR hdr;
		LPNMTREEVIEW ntv;
		LPNMTVDISPINFO ndi;
		hdr=(LPNMHDR)lParam;
		ntv=(LPNMTREEVIEW)lParam;
		ndi=(LPNMTVDISPINFO)lParam;
		if (hdr->hwndFrom == hPrjTree) {
			switch (hdr->code) {
			case TVN_BEGINDRAG:
				hDrag=ntv->itemNew.hItem;
				if (hDrag == TreeView_GetRoot(hPrjTree)) {
					return TRUE;
				}
				himl=TreeView_CreateDragImage(hPrjTree,hDrag);

				TreeView_GetItemRect(hPrjTree,hDrag,&rt,TRUE);
				hotx=ntv->ptDrag.x-(rt.left-18);
				hoty=ntv->ptDrag.y-rt.top;
				ImageList_BeginDrag(himl,0,hotx,hoty);

				pt.x=ntv->ptDrag.x;
				pt.y=ntv->ptDrag.y;
				ClientToScreen(hDlg,&pt);
				ImageList_DragEnter(NULL,pt.x,pt.y);

				SetCapture(hDlg);
				bDrag=TRUE;
				return TRUE;
			case TVN_SELCHANGED:
				TV.mask=TVIF_IMAGE;
				TV.hItem=ntv->itemNew.hItem;
				TreeView_GetItem(hPrjTree,&TV);
				if (TV.iImage == DGI_FILE || TV.iImage == DGI_FOLDER) {
					EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJFILE),FALSE);
					EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJFTP),FALSE);
					EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJHTTP),FALSE);
					EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJDELETE),TRUE);
					EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJSUB),FALSE);
					EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJFOLDER),FALSE);
					EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJRENAME),FALSE);
				} else {
					EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJFILE),TRUE);
					EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJFTP),TRUE);
					EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJHTTP),TRUE);
					EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJSUB),TRUE);
					EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJFOLDER),TRUE);
					if (TreeView_GetParent(hPrjTree,ntv->itemNew.hItem)==NULL) {
						EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJDELETE),FALSE);
						EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJRENAME),FALSE);
					} else {
						EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJDELETE),TRUE);
						EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJRENAME),TRUE);
					}
				}
				return TRUE;
			case TVN_BEGINLABELEDIT:
				TV.mask=TVIF_IMAGE;
				TV.hItem=ndi->item.hItem;
				TreeView_GetItem(hPrjTree,&TV);
				if (TV.iImage == DGI_SUB) {
					SendMessage(TreeView_GetEditControl(hPrjTree),EM_LIMITTEXT,31,0);
					SetWindowLongPtr(hDlg,DWLP_MSGRESULT,FALSE);
				} else {
					SetWindowLongPtr(hDlg,DWLP_MSGRESULT,TRUE);
				}
				return TRUE;
			case TVN_ENDLABELEDIT:
				if (ndi->item.pszText == NULL) {
					SetWindowLongPtr(hDlg,DWLP_MSGRESULT,FALSE);
				} else {
					if (strchr(ndi->item.pszText,':') || strchr(ndi->item.pszText,'\\')) {
						MessageBox(hDlg,"묶음 이름에 :과 \\는 쓸 수 없습니다.","알림",MB_OK);
						SetWindowLongPtr(hDlg,DWLP_MSGRESULT,FALSE);
					} else {
						SetWindowLongPtr(hDlg,DWLP_MSGRESULT,TRUE);
						Modified=TRUE;
					}
				}
				return TRUE;
			case TVN_DELETEITEM:
				if (ntv->itemOld.lParam) {
					free((LPVOID)ntv->itemOld.lParam);
				}
				return TRUE;
			case NM_DBLCLK:
				Node=TreeView_GetSelection(hPrjTree);
				if (Node) {
					TV.mask=TVIF_IMAGE;
					TV.hItem=Node;
					TreeView_GetItem(hFileTree,&TV);
					if (TV.iImage==DGI_FILE) {
						GetProjectNodePath(hDlg,hPrjTree,Node,Path);
						OpenFromFile(Path);
						TabCtrl_SetCurSel(hFileTab,FindFileTab(Path));
					}
				}
				return TRUE;
			case TVN_KEYDOWN:
				switch (((NMTVKEYDOWN *)lParam)->wVKey) {
				case VK_DELETE:
					if (IsWindowEnabled(GetDlgItem(hDlg,IDC_BTNPRJDELETE))) {
						SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_BTNPRJDELETE,BN_CLICKED),0);
					}
					break;
				case VK_F2:
					SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_BTNPRJRENAME,BN_CLICKED),0);
					break;
				default:
					break;
				}
				return TRUE;
			}
			return FALSE;
		}
	}
	return FALSE;
}

void RePathProject(HANDLE hFile,TCHAR *Project,BOOL bAbsolute)
{
	ProjectRecord *Record;
	DWORD Size;
	LPVOID buf;
	int i,len;
	TCHAR Drive[_MAX_DRIVE];
	TCHAR Dir[_MAX_DIR];
	DWORD dwRead, dwWritten;
	TCHAR szTemp[MAX_PATH];

	Size=GetFileSize(hFile,NULL)-sizeof(ProjectHeader);
	buf=malloc(Size);
	ReadFile(hFile,buf,Size,&dwRead,NULL);
	Record=(ProjectRecord *)buf;
	_splitpath(Project,Drive,Dir,NULL,NULL);
	len=lstrlen(Drive)+lstrlen(Dir);

	for (i=0;;i++) {
		if (lstrlen(Record[i].Path)==0) {
			break;
		}
		if (bAbsolute) {
			if (strchr(Record[i].Path,':') == NULL) {
				wsprintf(szTemp,"%s%s%s",Drive,Dir,Record[i].Path);
				lstrcpy(Record[i].Path,szTemp);
			}
		} else {
			if (strnicmp(Record[i].Path,Project,len) == 0) {
				lstrcpy(szTemp,Record[i].Path+len);
				lstrcpy(Record[i].Path,szTemp);
			}
		}
	}

	SetFilePointer(hFile,sizeof(ProjectHeader),NULL,FILE_BEGIN);
	WriteFile(hFile,buf,Size,&dwWritten,NULL);
	free(buf);
}

BOOL CALLBACK DgNewProjectProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	OPENFILENAME OFN;
	TCHAR lpstrFile[MAX_PATH]="";
	ProjectHeader Header;
	ProjectRecord Node;
	static TCHAR *Project;
	HANDLE hFile;
	DWORD dwRead, dwWritten;
	static BOOL OldAbsolute, bNew;

	switch(iMessage) {
	case WM_INITDIALOG:
		MoveToParentCenter(hDlg);
		Project=(TCHAR *)lParam;
		SendDlgItemMessage(hDlg,IDC_PRJNEWFILE,EM_LIMITTEXT,MAX_PATH-1,0);
		SendDlgItemMessage(hDlg,IDC_PRJNEWNAME,EM_LIMITTEXT,31,0);
		SendDlgItemMessage(hDlg,IDC_PRJNEWDESC,EM_LIMITTEXT,127,0);

		if (lstrlen(Project) != 0) {
			SetWindowText(hDlg,"프로젝트 정보 수정");
			hFile=CreateFile(Project,GENERIC_READ,0,NULL,
				OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
			if (hFile==INVALID_HANDLE_VALUE) {
			}
			ReadFile(hFile,&Header,sizeof(Header),&dwRead,NULL);
			CloseHandle(hFile);
			SetDlgItemText(hDlg,IDC_PRJNEWFILE,Project);
			SetDlgItemText(hDlg,IDC_PRJNEWNAME,Header.Name);
			SetDlgItemText(hDlg,IDC_PRJNEWDESC,Header.Desc);
			CheckRadioButton(hDlg,IDC_PRJABSOLUTE, IDC_PRJRELATIVE,
				Header.bAbsolute ? IDC_PRJABSOLUTE : IDC_PRJRELATIVE);
			OldAbsolute=Header.bAbsolute;
			EnableWindow(GetDlgItem(hDlg,IDC_PRJNEWFILE),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_BTNBRPROJECT),FALSE);
			bNew=FALSE;
		} else {
			SetWindowText(hDlg,"새 프로젝트");
			SetDlgItemText(hDlg,IDC_PRJNEWFILE,"이름과 저장 위치를 먼저 선택해주십시오. ->");
			EnableWindow(GetDlgItem(hDlg,IDC_PRJNEWNAME),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_PRJNEWDESC),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_PRJABSOLUTE),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_PRJRELATIVE),FALSE);
			CheckRadioButton(hDlg,IDC_PRJABSOLUTE,IDC_PRJRELATIVE,IDC_PRJRELATIVE);
			bNew=TRUE;
			SetFocus(GetDlgItem(hDlg,IDC_BTNBRPROJECT));
			return FALSE;
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))	{
		case IDC_BTNBRPROJECT:
			memset(&OFN, 0, sizeof(OPENFILENAME));
			OFN.lStructSize = sizeof(OPENFILENAME);
			OFN.hwndOwner=hDlg;
			OFN.lpstrFilter="당근 프로젝트(*.dgp)\0*.dgp*\0";
			OFN.lpstrTitle="새로 만들 프로젝트의 저장 위치와 이름을 선택하십시오";
			OFN.lpstrFile=lpstrFile;
			OFN.nMaxFile=MAX_PATH;
			OFN.lpstrDefExt="dgp";
			if (GetSaveFileName(&OFN)==TRUE) {
				SetDlgItemText(hDlg,IDC_PRJNEWFILE,lpstrFile);
				EnableWindow(GetDlgItem(hDlg,IDC_PRJNEWNAME),TRUE);
				EnableWindow(GetDlgItem(hDlg,IDC_PRJNEWDESC),TRUE);
				EnableWindow(GetDlgItem(hDlg,IDC_PRJABSOLUTE),TRUE);
				EnableWindow(GetDlgItem(hDlg,IDC_PRJRELATIVE),TRUE);
			}
			return TRUE;
		case IDOK:
			if (bNew) {
				GetDlgItemText(hDlg,IDC_PRJNEWFILE,Project,MAX_PATH);
				if (lstrlen(Project)==0) {
					MessageBox(hDlg,"프로젝트의 저장 경로를 선택해 주십시오.","알림",MB_OK);
					return TRUE;
				}
				if (_access(Project,0)==0) {
					if (MessageBox(hDlg,"프로젝트 파일이 이미 있습니다. 덮어 쓰시겠습니까?",
						"질문",MB_YESNO)==IDNO)
						return TRUE;
				}
			}

			memset(&Header,0,sizeof(Header));
			lstrcpy(Header.Text, "당근 편집기의 프로젝트 파일입니다");
			Header.Ver=100;
			GetDlgItemText(hDlg,IDC_PRJNEWNAME,Header.Name,32);
			if (IsStrEmpty(Header.Name)) {
				MessageBox(hDlg,"프로젝트의 이름은 반드시 입력해야 합니다","알림",MB_OK);
				return TRUE;
			}
			StringNormalize(Header.Name,NULL);
			if (strchr(Header.Name,':') || strchr(Header.Name,'\\')) {
				MessageBox(hDlg,"프로젝트의 이름에 :과 \\는 쓸 수 없습니다.","알림",MB_OK);
				return TRUE;
			}
			GetDlgItemText(hDlg,IDC_PRJNEWDESC,Header.Desc,128);
			Header.bAbsolute=IsDlgButtonChecked(hDlg,IDC_PRJABSOLUTE);

			if (bNew) {
				hFile=CreateFile(Project,GENERIC_WRITE,0,NULL,
					CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
			} else {
				hFile=CreateFile(Project,GENERIC_WRITE | GENERIC_READ,0,NULL,
					OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
			}

			WriteFile(hFile,&Header,sizeof(Header),&dwWritten,NULL);

			if (bNew) {
				memset(&Node,0,sizeof(Node));
				WriteFile(hFile,&Node,sizeof(Node),&dwRead,NULL);
			} else if (Header.bAbsolute != OldAbsolute) {
				RePathProject(hFile,Project,Header.bAbsolute);
			}
			CloseHandle(hFile);
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

BOOL OpenProject(HWND hDlg,TCHAR *Project,BOOL &bAbsolute)
{
	ProjectHeader Header;
	ProjectRecord *Record;
	HANDLE hFile;
	DWORD dwRead;
	DWORD Size;
	LPVOID buf;
	HWND hPrjTree;
	TVITEM TV;
	TVINSERTSTRUCT TI;
	HTREEITEM hChild, hChild2, hRoot;

	hPrjTree=GetDlgItem(hDlg,IDC_PRJTREE);
	hFile=CreateFile(Project,GENERIC_READ,0,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile==INVALID_HANDLE_VALUE) {
		MessageBox(hDlg,"프로젝트 파일을 열 수 없습니다","알림",MB_OK);
		return FALSE;
	}
	Size=GetFileSize(hFile,NULL);
	buf=malloc(Size);
	ReadFile(hFile,buf,Size,&dwRead,NULL);
	CloseHandle(hFile);

	memcpy(&Header,buf,sizeof(Header));
	if (strnicmp(Header.Text,"당근",4) != 0 || Header.Ver != 100) {
		MessageBox(hDlg,"당근 1.0의 프로젝트 파일이 아닙니다","알림",MB_OK);
		free(buf);
		return FALSE;
	}
	SetDlgItemText(hDlg,IDC_PRJFILE,Project);
	SetDlgItemText(hDlg,IDC_PRJNAME,Header.Name);
	SetDlgItemText(hDlg,IDC_PRJDESC,Header.Desc);
	bAbsolute=Header.bAbsolute;

	Record=(ProjectRecord *)((BYTE *)buf+sizeof(Header));
		
	hRoot=TreeView_GetRoot(hPrjTree);
	if (hRoot) {
		hChild=TreeView_GetChild(hPrjTree,hRoot);

		while (hChild) {
			hChild2=TreeView_GetNextSibling(hPrjTree,hChild);
			TreeView_DeleteItem(hPrjTree,hChild);
			hChild=hChild2;
		}
		TV.mask=TVIF_TEXT;
		TV.pszText=Header.Name;
		TV.hItem=hRoot;
		TreeView_SetItem(hPrjTree,&TV);
	} else {
		TI.hParent=TVI_ROOT;
		TI.hInsertAfter=TVI_LAST;
		TI.item.mask=TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		TI.item.iImage=DGI_PROJECT;
		TI.item.iSelectedImage=DGI_PROJECTSEL;
		TI.item.pszText=Header.Name;
		hRoot=TreeView_InsertItem(hPrjTree, &TI);
	}
	InsertProjectRecord(hPrjTree,Record,(HTREEITEM)hRoot,-1);

	TreeView_SelectItem(hPrjTree,hRoot);
	TreeView_Expand(hPrjTree,hRoot,TVE_EXPAND);
	EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJEDIT),TRUE);
	EnableWindow(GetDlgItem(hDlg,IDC_BTNPRJTEST),TRUE);
	free(buf);
	return TRUE;
}

void InsertProjectRecord(HWND hTree,ProjectRecord *Record,HTREEITEM pNode,int pid)
{
	TVINSERTSTRUCT TI;
	TVITEM TV;
	HTREEITEM Node;
	TCHAR EncPath[MAX_PATH];
	TCHAR Text[MAX_PATH];
	int i;

	for (i=0;;i++) {
		if (lstrlen(Record[i].Path) == 0) {
			return;
		}
		if (Record[i].Parent==pid) {
			TI.hParent=pNode;
			TI.hInsertAfter=TVI_LAST;
			TI.item.mask=TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
			switch (Record[i].Type) {
			case DGP_SUB:
				TI.item.iImage=DGI_SUB;
				TI.item.iSelectedImage=DGI_SUBSEL;
				break;
			case DGP_FILE:
				TI.item.iImage=DGI_FILE;
				TI.item.iSelectedImage=DGI_FILESEL;
				break;
			case DGP_FOLDER:
				TI.item.iImage=DGI_FOLDER;
				TI.item.iSelectedImage=DGI_FOLDERSEL;
				break;
			}
			if (strnicmp(Record[i].Path,"Enco",4)==0) {
				DecodeText(Record[i].Path,Text);
				EncodePath(Text,EncPath);
				TI.item.lParam=(LPARAM)malloc(MAX_PATH);
				lstrcpy((TCHAR *)TI.item.lParam,Text);
			} else {
				lstrcpy(EncPath,Record[i].Path);
				TI.item.lParam=NULL;
			}
			TI.item.pszText=EncPath;
			Node=TreeView_InsertItem(hTree, &TI);
			if (Record[i].Type == DGP_SUB) {
				InsertProjectRecord(hTree,Record,Node,i);
			}
			if (Record[i].Type == DGP_FOLDER && hTree==hFileTree) {
				TV.mask=TVIF_CHILDREN;
				TV.hItem=Node;
				TV.cChildren=1;

				TreeView_SetItem(hFileTree,&TV);
			}
		}
	}
}

void AddFileToProject(HWND hDlg,TCHAR *Path,BOOL bAbsolute)
{
	HTREEITEM Node;
	TVINSERTSTRUCT TI;
	HWND hPrjTree;
	TVITEM TV;
	TCHAR Project[MAX_PATH];
	TCHAR Drive[_MAX_DRIVE];
	TCHAR Dir[_MAX_DIR];
	TCHAR Text[MAX_PATH];
	TCHAR EncPath[MAX_PATH];
	TCHAR Mes[512];

	hPrjTree=GetDlgItem(hDlg,IDC_PRJTREE);

	Node=TreeView_GetSelection(hPrjTree);
	if (Node==NULL) {
		Node=TreeView_GetRoot(hPrjTree);
	}
	TV.mask=TVIF_IMAGE;
	TV.hItem=Node;
	TreeView_GetItem(hPrjTree,&TV);
	if (TV.iImage != DGI_PROJECT && TV.iImage != DGI_SUB) {
		Node=TreeView_GetRoot(hPrjTree);
	}

	if (bAbsolute) {
		lstrcpy(Text,Path);
	} else {
		GetDlgItemText(hDlg,IDC_PRJFILE,Project,MAX_PATH);
		_splitpath(Project,Drive,Dir,NULL,NULL);
		if (strnicmp(Path,Project,lstrlen(Drive)+lstrlen(Dir)) == 0) {
			lstrcpy(Text,Path+lstrlen(Drive)+lstrlen(Dir));
		} else {
			lstrcpy(Text,Path);
		}
	}

	EncodePath(Text,EncPath);

	if (FindTreeItem(hPrjTree,TreeView_GetRoot(hPrjTree),EncPath)) {
		wsprintf(Mes,"%s 파일은 이미 등록되어 있습니다",EncPath);
		MessageBox(hDlg,Mes,"알림",MB_OK);
		return;
	}

	TI.hParent=Node;
	TI.hInsertAfter=TVI_LAST;
	TI.item.mask=TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	if (strnicmp(Path,"http",4) != 0 && strnicmp(Path,"ftp",3) != 0 &&
		(GetFileAttributes(Path) & FILE_ATTRIBUTE_DIRECTORY) != 0) {
		TI.item.iImage=DGI_FOLDER;
		TI.item.iSelectedImage=DGI_FOLDERSEL;
	} else {
		TI.item.iImage=DGI_FILE;
		TI.item.iSelectedImage=DGI_FILESEL;
	}
	TI.item.pszText=EncPath;
	if (strnicmp(Text,"ftp",3)==0) {
		TI.item.lParam=(LPARAM)malloc(MAX_PATH);
		lstrcpy((TCHAR *)TI.item.lParam,Text);
	} else {
		TI.item.lParam=NULL;
	}
	Node=TreeView_InsertItem(hPrjTree, &TI);
	TreeView_EnsureVisible(hPrjTree,Node);
}

void SaveProject(HWND hDlg, TCHAR *Project)
{
	ProjectHeader Header;
	ProjectRecord Record;
	HANDLE hFile;
	DWORD dwWritten;

	hFile=CreateFile(Project,GENERIC_READ,0,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	ReadFile(hFile,&Header,sizeof(Header),&dwWritten,NULL);
	CloseHandle(hFile);

	hFile=CreateFile(Project,GENERIC_WRITE,0,NULL,
		CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	WriteFile(hFile,&Header,sizeof(Header),&dwWritten,NULL);

	SaveProjectRecord(hDlg,hFile,(HTREEITEM)NULL,-1);

	memset(&Record,0,sizeof(Record));
	WriteFile(hFile,&Record,sizeof(Record),&dwWritten,NULL);
	CloseHandle(hFile);
}

void SaveProjectRecord(HWND hDlg, HANDLE hFile,HTREEITEM hParent,int pid)
{
	ProjectRecord Record;
	HTREEITEM hChild;
	TVITEM TV;
	TCHAR Text[MAX_PATH];
	int self;
	static int id;
	DWORD dwWritten;
	HWND hPrjTree;

	hPrjTree=GetDlgItem(hDlg,IDC_PRJTREE);

	if (hParent==NULL) {
		hParent=TreeView_GetRoot(hPrjTree);
		id=0;
		self=pid;
	} else {
		memset(&Record,0,sizeof(Record));
		Record.Parent=pid;
		Record.Type=1;
		TV.mask=TVIF_TEXT;
		TV.hItem=hParent;
		TV.pszText=Text;
		TV.cchTextMax=MAX_PATH;
		TreeView_GetItem(hPrjTree,&TV);
		lstrcpy(Record.Path,Text);
		WriteFile(hFile,&Record,sizeof(Record),&dwWritten,NULL);
		self=id;
		id++;
	}

	hChild=TreeView_GetChild(hPrjTree,hParent);
	while (hChild) {
		TV.mask=TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM;
		TV.hItem=hChild;
		TV.pszText=Text;
		TV.cchTextMax=MAX_PATH;
		TreeView_GetItem(hPrjTree,&TV);

		if (TV.iImage == DGI_FILE || TV.iImage == DGI_FOLDER) {
			memset(&Record,0,sizeof(Record));
			Record.Parent=self;
			if (TV.iImage == DGI_FILE) {
				Record.Type=DGP_FILE;
			} else {
				Record.Type=DGP_FOLDER;
			}
			if (strnicmp(Text,"ftp",3) == 0) {
				EncodeText((TCHAR *)TV.lParam,Text);
			}
			lstrcpy(Record.Path,Text);

			WriteFile(hFile,&Record,sizeof(Record),&dwWritten,NULL);
			id++;
		} else {
			SaveProjectRecord(hDlg,hFile,hChild,self);
		}
		hChild=TreeView_GetNextSibling(hPrjTree,hChild);
	}
}

BOOL LoadProject(TCHAR *Project,BOOL bUpdate)
{
	ProjectHeader Header;
	ProjectRecord *Record;
	HANDLE hFile;
	DWORD Size,dwRead;
	LPVOID buf;
	TVINSERTSTRUCT TI;
	HTREEITEM hRoot, hChild, hPrev;
	TVITEM TV;

	hPrev=TVI_LAST;
	hChild=TreeView_GetRoot(hFileTree);
	while (hChild) {
		TV.mask=TVIF_PARAM;
		TV.hItem=hChild;
		TreeView_GetItem(hFileTree,&TV);
		if (lstrcmpi((TCHAR *)TV.lParam,Project) == 0) {
			hPrev=TreeView_GetPrevSibling(hFileTree,hChild);
			if (hPrev==NULL) {
				hPrev=TVI_FIRST;
			}
			TreeView_DeleteItem(hFileTree,hChild);
			break;
		}
		hChild=TreeView_GetNextSibling(hFileTree,hChild);
	}

	if (hChild == NULL && bUpdate==TRUE) {
		return TRUE;
	}

	hFile=CreateFile(Project,GENERIC_READ,0,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile==INVALID_HANDLE_VALUE) {
		MessageBox(g_hFrameWnd,"프로젝트 파일을 열 수 없습니다","알림",MB_OK);
		return FALSE;
	}
	Size=GetFileSize(hFile,NULL);
	buf=malloc(Size);
	ReadFile(hFile,buf,Size,&dwRead,NULL);
	CloseHandle(hFile);
	memcpy(&Header,buf,sizeof(Header));
	if (strnicmp(Header.Text,"당근",4) != 0 || Header.Ver != 100) {
		MessageBox(g_hFrameWnd,"당근 1.0의 프로젝트 파일이 아닙니다","알림",MB_OK);
		free(buf);
		return FALSE;
	}
	Record=(ProjectRecord *)((BYTE *)buf+sizeof(Header));

	TI.hParent=TVI_ROOT;
	TI.hInsertAfter=hPrev;
	TI.item.mask=TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	TI.item.iImage=DGI_PROJECT;
	TI.item.iSelectedImage=DGI_PROJECTSEL;
	TI.item.lParam=(LPARAM)malloc(MAX_PATH);
	lstrcpy((TCHAR *)TI.item.lParam,Project);
	TI.item.pszText=Header.Name;
	hRoot=TreeView_InsertItem(hFileTree, &TI);

	InsertProjectRecord(hFileTree,Record,(HTREEITEM)hRoot,-1);
	TreeView_Expand(hFileTree,hRoot,TVE_EXPAND);
	free(buf);
	return TRUE;
}

HTREEITEM FindTreeItem(HWND hTree, HTREEITEM hParent, TCHAR *Text)
{
	HTREEITEM hChild,hChild2;
	TVITEM TV;
	TCHAR ChildText[MAX_PATH];

	hChild=TreeView_GetChild(hTree,hParent);
	while (hChild) {
		TV.mask=TVIF_TEXT | TVIF_IMAGE;
		TV.hItem=hChild;
		TV.pszText=ChildText;
		TV.cchTextMax=MAX_PATH;
		TreeView_GetItem(hTree,&TV);
		if (lstrcmpi(Text,ChildText)==0) {
			return hChild;
		}
		if (TV.iImage == DGI_SUB) {
			hChild2=FindTreeItem(hTree,hChild,Text);
			if (hChild2) {
				return hChild;
			}
		}
		hChild=TreeView_GetNextSibling(hTree,hChild);
	}
	return NULL;
}

int GetProjectNodePath(HWND hDlg,HWND hPrjTree,HTREEITEM hItem,TCHAR *Path)
{
	TVITEM TV;
	TCHAR Text[MAX_PATH];
	TCHAR Project[MAX_PATH];
	TCHAR Drive[_MAX_DRIVE];
	TCHAR Dir[_MAX_DIR];

	TV.mask=TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM;
	TV.hItem=hItem;
	TV.pszText=Text;
	TV.cchTextMax=MAX_PATH;
	TreeView_GetItem(hPrjTree,&TV);

	if (TV.iImage == DGI_FILE || TV.iImage == DGI_FOLDER) {
		GetDlgItemText(hDlg,IDC_PRJFILE,Project,MAX_PATH);
		_splitpath(Project,Drive,Dir,NULL,NULL);

		if (strchr(Text,':') == NULL) {
			wsprintf(Path,"%s%s%s",Drive,Dir,Text);
		} else {
			if (strnicmp(Text,"ftp",3)==0) {
				lstrcpy(Path,(TCHAR *)TV.lParam);
			} else {
				lstrcpy(Path,Text);
			}
		}
	}
	return TV.iImage;
}

BOOL TestProjectValid(HWND hDlg, HTREEITEM hParent)
{
	HWND hPrjTree;
	HTREEITEM hChild;
	TCHAR Text[MAX_PATH];
	TCHAR Path[MAX_PATH];
	TCHAR Mes[512];
	BOOL bResult=TRUE, bValid;
	TCHAR *pDummy;
	int iImage;

	hPrjTree=GetDlgItem(hDlg,IDC_PRJTREE);
	if (hParent) {
		hChild=TreeView_GetChild(hPrjTree,hParent);
	} else {
		hChild=TreeView_GetChild(hPrjTree,TreeView_GetRoot(hPrjTree));
	}

	while (hChild) {
		bValid=TRUE;
		pDummy=NULL;
		iImage=GetProjectNodePath(hDlg,hPrjTree,hChild,Path);
		if (iImage==DGI_FILE || iImage==DGI_FOLDER) {
			if (strnicmp(Path,"ftp",3)==0) {
				bValid=(DgFtpDown(Path,pDummy) != -1);
			} else if (strnicmp(Path,"http",4)==0) {
				bValid=(DgHttpDown(Path,pDummy) != -1);
			} else {
				bValid=(_access(Path,0) == 0);
			}

			if (!bValid) {
				EncodePath(Path,Text);
				wsprintf(Mes,"%s 파일이 존재하지 않습니다. 이 파일이 삭제되었거나 이름이 변경되었습니다"
					"\r\n네트워크 문제로 인한 일시적인 현상일 수도 있습니다.",Text);
				MessageBox(hDlg,Mes,"알림",MB_OK);
				bResult=FALSE;
			} else {
				if (pDummy)
					free(pDummy);
			}
		} else if (iImage == DGI_SUB) {
			if (TestProjectValid(hDlg,hChild)==FALSE) {
				bResult=FALSE;
			}
		}
		hChild=TreeView_GetNextSibling(hPrjTree,hChild);
	}
	return bResult;
}

HTREEITEM CopyTreeItem(HWND hTree, HTREEITEM hSrc, HTREEITEM hDest, BOOL bNext)
{
	HTREEITEM hNew;
	TVINSERTSTRUCT TI;
	TVITEM TV;
	TCHAR Text[MAX_PATH];

	TV.hItem=hSrc;
	TV.mask=TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	TV.pszText=Text;
	TV.cchTextMax=256;
	TreeView_GetItem(hTree,&TV);

	TI.itemex.mask=TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	TI.itemex.pszText=Text;
	if (TV.lParam) {
		TI.itemex.lParam=(LPARAM)malloc(MAX_PATH);
		lstrcpy((TCHAR *)TI.itemex.lParam, (TCHAR *)TV.lParam);
	} else {
		TI.itemex.lParam=NULL;
	}
	TI.itemex.iImage=TV.iImage;
	TI.itemex.iSelectedImage=TV.iSelectedImage;
	if (bNext) {
		TI.hParent=TreeView_GetParent(hTree,hDest);
		TI.hInsertAfter=hDest;
	} else {
		TI.hParent=hDest;
		TI.hInsertAfter=TVI_LAST;
	}
	hNew=TreeView_InsertItem(hTree,&TI);
	return hNew;
}

HTREEITEM CopyTreeItems(HWND hTree, HTREEITEM hSrc, HTREEITEM hDest,BOOL bNext)
{
	HTREEITEM hNew, hChild;

	hNew=CopyTreeItem(hTree, hSrc, hDest,bNext);

	hChild=TreeView_GetChild(hTree, hSrc);
	while (hChild != NULL) {
		CopyTreeItems(hTree,hChild, hNew,FALSE);
		hChild=TreeView_GetNextSibling(hTree,hChild);
	}

	return hNew;
}

