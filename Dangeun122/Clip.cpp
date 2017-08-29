#include "stdafx.h"

CClipRecord::CClipRecord()
{
	memset(this,0,sizeof(CClipRecord));
}

// 완전히 텅 빈 상태로 생성
CClipFile::CClipFile()
{
	memset(&Header,0,sizeof(SClipHeader));
	lstrcpy(Header.Text,"당근 상용구 파일입니다.Dangeun Clip File");
	Header.Ver=120;
	arRecord=NULL;
}

CClipFile::~CClipFile()
{
	int i;

	// 상용구의 내용도 해제한다.
	if (arRecord) {
		for (i=0;i<Header.Num;i++) {
			if (arRecord[i].Content) {
				free(arRecord[i].Content);
			}
		}
		free(arRecord);
	}
}

// 파일에서 읽어서 메모리로 올림
int CClipFile::Load(TCHAR *Path)
{
	HANDLE hFile;
	DWORD dwRead;
	BOOL Result=0;
	int i;

	hFile=CreateFile(Path,GENERIC_READ,0,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile==INVALID_HANDLE_VALUE) {
		return -1;
	}

	// 헤더 먼저 읽음
	ReadFile(hFile,&Header,sizeof(SClipHeader),&dwRead,NULL);
	if (Header.Ver != 120) {
		Result=-2;
		goto end;
	}

	// 레코드가 없으면 더 읽을 거 없음
	if (Header.Num == 0) {
		goto end;
	}

	// 레코드수만큼 메모리 할당하고 레코드를 통째로 읽어들임
	arRecord=(CClipRecord *)malloc(Header.Num*sizeof(CClipRecord));
	ReadFile(hFile,arRecord,Header.Num*sizeof(CClipRecord),&dwRead,NULL);

	// 내용 읽음
	for (i=0;i<Header.Num;i++) {
		if (arRecord[i].Type == DGCT_CLIP) {
			arRecord[i].Content=(TCHAR *)calloc(arRecord[i].len+1,1);
			ReadFile(hFile,arRecord[i].Content,arRecord[i].len,&dwRead,NULL);
		}
	}

end:
	CloseHandle(hFile);
	return Result;
}

// 파일로 저장.
BOOL CClipFile::Save(TCHAR *Path)
{
	HANDLE hFile;
	DWORD dwWritten;
	int i;
	int ConOff;
	CClipRecord cr;

	hFile=CreateFile(Path,GENERIC_WRITE,0,NULL,
		CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile==INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	// 헤더 먼저 출력
	WriteFile(hFile,&Header,sizeof(SClipHeader),&dwWritten,NULL);

	// 레코드 출력. 포인터는 오프셋으로 바꾸어 저장한다. 단, 폴더는 제외
	ConOff=sizeof(SClipHeader)+Header.Num*sizeof(CClipRecord);
	for (i=0;i<Header.Num;i++) {
		cr=arRecord[i];
		if (cr.Type == DGCT_CLIP) {
			// 이 오프셋은 임의 접근을 위해 필요함. 순서대로 읽을 때는 사실 필요가 없음
			cr.Content=(TCHAR *)ConOff;
			ConOff+=cr.len;
		}
		WriteFile(hFile,&cr,sizeof(CClipRecord),&dwWritten,NULL);
	}

	// 상용구 문자열 출력
	for (i=0;i<Header.Num;i++) {
		if (arRecord[i].Type == DGCT_CLIP) {
			WriteFile(hFile,arRecord[i].Content,arRecord[i].len,&dwWritten,NULL);
		}
	}
	CloseHandle(hFile);

	return TRUE;
}

// lParam으로 처음 열 상용구 파일의 경로를 전달받는다. NULL이면 빈 상태로 시작한다.
BOOL CALLBACK ClipDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	static HWND hTree;
	static CApiEdit *pAe;
	static HIMAGELIST himl;
	static BOOL bDrag;
	static HTREEITEM hDrag,hTarget, hOldTarget;
	static BOOL Modified;
	RECT brt;
	HTREEITEM NowItem,Node;
	CClipRecord *cr;
	TVINSERTSTRUCT TI;
	TVITEM TV;
	OPENFILENAME OFN;
	TCHAR lpstrFile[MAX_PATH]="";
	TCHAR szTemp[256];
	TVHITTESTINFO hti;
	POINT pt;
	int hotx,hoty;
	RECT rt;
	BOOL bNext;

	switch(iMessage) {
	case WM_INITDIALOG:
		MoveToParentCenter(hDlg);
		SendDlgItemMessage(hDlg,IDC_CLIPNAME,EM_LIMITTEXT,63,0);
		SendDlgItemMessage(hDlg,IDC_CLIPDESC,EM_LIMITTEXT,127,0);

		// 버튼 자리에 ApiEdit를 배치한다. 대부분의 설정은 기본값이며 몇 가지만 수정한다.
		GetWindowRect(GetDlgItem(hDlg,IDC_PLACEAPIEDIT),&brt);
		ScreenToClient(hDlg,(LPPOINT)&brt);
		ScreenToClient(hDlg,(LPPOINT)&brt.right);
		pAe=new CApiEdit;
		pAe->Create(brt.left,brt.top,brt.right-brt.left,brt.bottom-brt.top,
			WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP,0,hDlg);
		SetSetting(*pAe);
		pAe->SetWrap(2);
//		pAe->SetWantTab(FALSE);
		pAe->ChangeStyle(AES_WANTTAB,FALSE);
		pAe->SetReadOnly(TRUE);
		pAe->SetColor(AECOLOR_BACK,RGB(220,220,220));
//		pAe->EnableSplit(0);
		pAe->ChangeStyle(AES_HORZSPLITABLE,FALSE);
		pAe->ChangeStyle(AES_VERTSPLITABLE,FALSE);

		hTree=GetDlgItem(hDlg,IDC_CLIPTREE);
		SendMessage(hTree, TVM_SETIMAGELIST, (WPARAM)TVSIL_NORMAL, (LPARAM)hFileImg);

		// 인수로 파일의 경로가 전달되었으면 연다.
		if (lParam!=NULL) {
			if (LoadClipFileToDlg(hDlg,(TCHAR *)lParam)==FALSE) {
				MessageBox(hDlg,"상용구 파일을 열 수 없습니다.","알림",MB_OK);
				EndDialog(hDlg,IDCANCEL);
			}
		}

		Modified=FALSE;
		SetFocus(hTree);
		return FALSE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BTNCLIPFILENEW:
			// 편집하던 묶음을 먼저 저장한다.
			if (Modified) {
				if (ConfirmSaveClip(hDlg)==IDCANCEL) {
					return TRUE;
				}
			}

			SetDlgItemText(hDlg,IDC_CLIPNAME,"새 상용구 묶음");
			SetDlgItemText(hDlg,IDC_CLIPFILE,"저장되지 않은 파일");
			SetDlgItemText(hDlg,IDC_CLIPDESC,"");

			// 루트 노드만 등록한다.
			TreeView_DeleteAllItems(hTree);
			TI.hParent=NULL;
			TI.hInsertAfter=TVI_LAST;
			TI.item.mask=TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
			TI.item.iImage=DGI_CLIPROOT;
			TI.item.iSelectedImage=DGI_CLIPROOTSEL;
			GetDlgItemText(hDlg,IDC_CLIPNAME,szTemp,64);
			TI.item.pszText=szTemp;
			TI.item.lParam=NULL;
			Node=TreeView_InsertItem(hTree,&TI);
			TreeView_SelectItem(hTree,Node);

			// 프로젝트 이름으로 포커스를 옮긴다.
			Modified=FALSE;
			SetFocus(GetDlgItem(hDlg,IDC_CLIPNAME));
			SendDlgItemMessage(hDlg,IDC_CLIPNAME,EM_SETSEL,0,-1);
			return TRUE;
		case IDC_BTNCLIPFILEOPEN:
			// 편집하던 묶음을 먼저 저장한다.
			if (Modified) {
				if (ConfirmSaveClip(hDlg)==IDCANCEL) {
					return TRUE;
				}
			}

			memset(&OFN, 0, sizeof(OPENFILENAME));
			OFN.lStructSize = sizeof(OPENFILENAME);
			OFN.hwndOwner=hDlg;
			OFN.lpstrFilter="당근 상용구 파일\0*.dgc\0";
			OFN.lpstrFile=lpstrFile;
			OFN.nMaxFile=MAX_PATH;

			if (GetOpenFileName(&OFN)) {
				if (LoadClipFileToDlg(hDlg,lpstrFile)==FALSE) {
					MessageBox(hDlg,"상용구 파일을 열 수 없습니다.","알림",MB_OK);
				} else {
					Modified=FALSE;
				}
			}
			return TRUE;
		case IDC_BTNCLIPADDCLIP:
			// 루트나 폴더 아래만 추가 가능하다.
			NowItem=TreeView_GetSelection(hTree);
			if (NowItem == NULL) {
				return TRUE;
			}

			// 새 상용구 하나를 편집한다.
			cr=new CClipRecord;
			cr->Type=DGCT_CLIP;
			if (DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_EDITCLIP),hDlg,
				EditClipProc,(LPARAM)cr)==IDCANCEL) {
				delete cr;
				return TRUE;
			}

			// 추가된 상용구를 트리에 추가한다.
			TI.hParent=NowItem;
			TI.hInsertAfter=TVI_LAST;
			TI.item.mask=TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
			TI.item.iImage=DGI_CLIP;
			TI.item.iSelectedImage=DGI_CLIPSEL;
			TI.item.pszText=cr->Name;
			TI.item.lParam=(LPARAM)cr;
			TreeView_InsertItem(hTree,&TI);
			TreeView_Expand(hTree,NowItem,TVE_EXPAND);
			Modified=TRUE;
			return TRUE;
		case IDC_BTNCLIPADDFOLDER:
			NowItem=TreeView_GetSelection(hTree);
			if (NowItem == NULL) {
				return TRUE;
			}

			cr=new CClipRecord;
			cr->Type=DGCT_FOLDER;
			lstrcpy(cr->Name,"새 폴더");

			TI.hParent=NowItem;
			TI.hInsertAfter=TVI_LAST;
			TI.item.mask=TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
			TI.item.iImage=DGI_CLIPFOLDER;
			TI.item.iSelectedImage=DGI_CLIPFOLDERSEL;
			TI.item.pszText=cr->Name;
			TI.item.lParam=(LPARAM)cr;
			Node=TreeView_InsertItem(hTree, &TI);
			TreeView_SelectItem(hTree,Node);
			TreeView_EditLabel(hTree,Node);
			Modified=TRUE;
			return TRUE;
		case IDC_BTNCLIPRENAME:
			NowItem=TreeView_GetSelection(hTree);
			if (NowItem == NULL) {
				return TRUE;
			}
			TV.mask=TVIF_IMAGE;
			TV.hItem=NowItem;
			TreeView_GetItem(hTree,&TV);
			if (TV.iImage != DGI_CLIPROOT) {
				TreeView_EditLabel(hTree,NowItem);
			}
			return TRUE;
		case IDC_BTNCLIPDELETE:
			NowItem=TreeView_GetSelection(hTree);
			if (NowItem == NULL) {
				return TRUE;
			}
			TV.mask=TVIF_IMAGE;
			TV.hItem=NowItem;
			TreeView_GetItem(hTree,&TV);
			if (TV.iImage != DGI_CLIPROOT) {
				// 노드만 지우면 노드에 포함된 메모리도 모두 해제된다.
				TreeView_DeleteItem(hTree,NowItem);
			}
			Modified=TRUE;
			return TRUE;
		case IDC_BTNCLIPEDIT:
			NowItem=TreeView_GetSelection(hTree);
			if (NowItem == NULL) {
				return TRUE;
			}

			// 선택한 상용구를 편집한다.
			TV.mask=TVIF_PARAM | TVIF_IMAGE;
			TV.hItem=NowItem;
			TreeView_GetItem(hTree,&TV);

			// 폴더는 더블클릭해도 편집할 수 없다.
			if (TV.iImage != DGI_CLIP) {
				return TRUE;
			}
			cr=(CClipRecord *)TV.lParam;
			if (DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_EDITCLIP),hDlg,
				EditClipProc,(LPARAM)cr)==IDCANCEL) {
				return TRUE;
			}

			// 노드의 텍스트와 미리 보기를 수정한다.
			TV.mask=TVIF_TEXT;
			TV.pszText=cr->Name;
			TV.hItem=NowItem;
			TreeView_SetItem(hTree,&TV);
			DisplayClipPreview(pAe,cr);
			Modified=TRUE;
			return TRUE;
		case IDOK:
			if (TreeView_GetEditControl(hTree)) {
				if (HIWORD(wParam) == 0) {
					TreeView_EndEditLabelNow(hTree,FALSE);
				}
			} else {
				// 이름이 주어져 있으면 질문없이 저장한다.
				if (Modified) {
					GetDlgItemText(hDlg,IDC_CLIPFILE,szTemp,256);
					if (lstrcmp(szTemp,"저장되지 않은 파일")!=0) {
						SaveClipFileFromDlg(hDlg);
					} else {
						if (ConfirmSaveClip(hDlg)==IDCANCEL) {
							return TRUE;
						}
					}
				}
				DestroyWindow(pAe->hWnd);
				delete pAe;
				EndDialog(hDlg,IDOK);
				return TRUE;
			}
			return TRUE;
		case IDCANCEL:
			if (TreeView_GetEditControl(hTree)) {
				TreeView_EndEditLabelNow(hTree,TRUE);
			} else {
				DestroyWindow(pAe->hWnd);
				delete pAe;
				EndDialog(hDlg,IDCANCEL);
			}
			return TRUE;
		case IDC_CLIPNAME:
			switch (HIWORD(wParam)) {
			case EN_CHANGE:
				Node=TreeView_GetRoot(hTree);
				if (Node) {
					GetDlgItemText(hDlg,IDC_CLIPNAME,szTemp,256);
					TV.mask=TVIF_TEXT;
					TV.pszText=szTemp;
					TV.hItem=Node;
					TreeView_SetItem(hTree,&TV);
					Modified=TRUE;
				}
				break;
			}
			return TRUE;
		case IDC_CLIPDESC:
			switch (HIWORD(wParam)) {
			case EN_CHANGE:
				Modified=TRUE;
				break;
			}
			return TRUE;
		}
		return FALSE;
	case WM_MOUSEMOVE:
		if (bDrag) {
			pt.x=LOWORD(lParam);
			pt.y=HIWORD(lParam);
			ClientToScreen(hDlg,&pt);
			ImageList_DragMove(pt.x,pt.y);

			GetWindowRect(hTree,&rt);
			ScreenToClient(hDlg,(POINT *)&rt);
			hti.pt.x=LOWORD(lParam)-rt.left;
			hti.pt.y=HIWORD(lParam)-rt.top;
			hTarget=TreeView_HitTest(hTree,&hti);
			if (hTarget != NULL) {
				ImageList_DragLeave(NULL);
				TreeView_SelectDropTarget(hTree,hTarget);
				UpdateWindow(hTree);
				ImageList_DragEnter(NULL,pt.x,pt.y);

				if (hTarget != hOldTarget) {
					hOldTarget=hTarget;
					KillTimer(hDlg,1);
					SetTimer(hDlg,1,1000,NULL);
				}
			}

			GetWindowRect(hTree,&rt);
			if ((short)pt.y < rt.top) {
				SetTimer(hDlg,2,200,NULL);
				ImageList_DragLeave(NULL);
				SendMessage(hTree,WM_VSCROLL,MAKEWPARAM(SB_LINEUP,0),NULL);
				ImageList_DragEnter(NULL,pt.x,pt.y);
			} else {
				KillTimer(hDlg,2);
			}

			if ((short)pt.y > rt.bottom) {
				SetTimer(hDlg,3,200,NULL);
				ImageList_DragLeave(NULL);
				SendMessage(hTree,WM_VSCROLL,MAKEWPARAM(SB_LINEDOWN,0),NULL);
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
			TreeView_SelectDropTarget(hTree,NULL);
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
				Node=TreeView_GetParent(hTree,Node);
				if (Node == hDrag)
					return TRUE;
			}

			TV.mask=TVIF_IMAGE;
			TV.hItem=hTarget;
			TreeView_GetItem(hTree,&TV);
			if (TV.iImage==DGI_CLIPFOLDER) {
				switch (MessageBox(hDlg,"상용구를 폴더에 떨어뜨렸습니다. "
					"어떻게 하시겠습니까?\r\n\r\n예=상용구를 폴더 안에 집어 넣는다.\r\n"
					"아니오=상용구를 폴더 다음으로 옮긴다","질문",MB_YESNOCANCEL)) {
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

			if (TV.iImage==DGI_CLIPROOT) {
				bNext=FALSE;
			}

			LockWindowUpdate(hTree);
			Node=CopyTreeItems2(hTree, hDrag, hTarget,bNext);
			TreeView_EnsureVisible(hTree,Node);
			TreeView_DeleteItem(hTree,hDrag);
			LockWindowUpdate(NULL);

			Modified=TRUE;
		}
		return TRUE;
	case WM_TIMER:
		if (wParam == 1) {
			Node=TreeView_GetDropHilight(hTree);
			if ((Node != NULL) && (Node == hTarget)) {
				ImageList_DragShowNolock(FALSE);
				TreeView_Expand(hTree,hTarget,TVE_EXPAND);
				UpdateWindow(hTree);
				ImageList_DragShowNolock(TRUE);
			}
		}
		if (wParam == 2) {
			SendMessage(hTree,WM_VSCROLL,MAKEWPARAM(SB_LINEUP,0),NULL);
		}
		if (wParam == 3) {
			SendMessage(hTree,WM_VSCROLL,MAKEWPARAM(SB_LINEDOWN,0),NULL);
		}
		return TRUE;
	case WM_NOTIFY:
		LPNMHDR hdr;
		LPNMTREEVIEW ntv;
		LPNMTVDISPINFO ndi;
		hdr=(LPNMHDR)lParam;
		ntv=(LPNMTREEVIEW)lParam;
		ndi=(LPNMTVDISPINFO)lParam;
		if (hdr->hwndFrom == hTree) {
			switch (hdr->code) {
			case TVN_BEGINDRAG:
				hDrag=ntv->itemNew.hItem;
				if (hDrag == TreeView_GetRoot(hTree)) {
					return TRUE;
				}
				himl=TreeView_CreateDragImage(hTree,hDrag);

				TreeView_GetItemRect(hTree,hDrag,&rt,TRUE);
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
				TV.mask=TVIF_IMAGE | TVIF_PARAM;
				TV.hItem=ntv->itemNew.hItem;
				TreeView_GetItem(hTree,&TV);
				cr=(CClipRecord *)TV.lParam;
				if (TV.iImage == DGI_CLIP) {
					EnableWindow(GetDlgItem(hDlg,IDC_BTNCLIPADDCLIP),FALSE);
					EnableWindow(GetDlgItem(hDlg,IDC_BTNCLIPADDFOLDER),FALSE);
					EnableWindow(GetDlgItem(hDlg,IDC_BTNCLIPEDIT),TRUE);
					// 미리 보기 출력
					DisplayClipPreview(pAe,cr);
				} else {
					EnableWindow(GetDlgItem(hDlg,IDC_BTNCLIPADDCLIP),TRUE);
					EnableWindow(GetDlgItem(hDlg,IDC_BTNCLIPADDFOLDER),TRUE);
					EnableWindow(GetDlgItem(hDlg,IDC_BTNCLIPEDIT),FALSE);
					pAe->SetText("",0);
				}
				if (TV.iImage == DGI_CLIPROOT) {
					EnableWindow(GetDlgItem(hDlg,IDC_BTNCLIPRENAME),FALSE);
					EnableWindow(GetDlgItem(hDlg,IDC_BTNCLIPDELETE),FALSE);
				} else {
					EnableWindow(GetDlgItem(hDlg,IDC_BTNCLIPRENAME),TRUE);
					EnableWindow(GetDlgItem(hDlg,IDC_BTNCLIPDELETE),TRUE);
				}
				return TRUE;
			case TVN_BEGINLABELEDIT:
				TV.mask=TVIF_IMAGE;
				TV.hItem=ndi->item.hItem;
				TreeView_GetItem(hTree,&TV);
				if (TV.iImage != DGI_CLIPROOT) {
					SendMessage(TreeView_GetEditControl(hTree),EM_LIMITTEXT,31,0);
					SetWindowLong(hDlg,DWL_MSGRESULT,FALSE);
				} else {
					SetWindowLong(hDlg,DWL_MSGRESULT,TRUE);
				}
				return TRUE;
			case TVN_ENDLABELEDIT:
				if (ndi->item.pszText == NULL) {
					SetWindowLong(hDlg,DWL_MSGRESULT,FALSE);
				} else {
					if (strchr(ndi->item.pszText,':') || strchr(ndi->item.pszText,'\\')) {
						MessageBox(hDlg,"묶음 이름에 :과 \\는 쓸 수 없습니다.","알림",MB_OK);
						SetWindowLong(hDlg,DWL_MSGRESULT,FALSE);
					} else {
						// 실제 이름을 변경해야 한다. 저장할 때 Name을 어디서 얻는가에 따라 문제가 달라진다.
						TV.mask=TVIF_PARAM;
						TV.hItem=ndi->item.hItem;
						TreeView_GetItem(hTree,&TV);
						cr=(CClipRecord *)TV.lParam;
						lstrcpy(cr->Name,ndi->item.pszText);
						SetWindowLongPtr(hDlg,DWLP_MSGRESULT,TRUE);
						Modified=TRUE;
					}
				}
				return TRUE;
			// lParam에 기억된 레코드를 모두 삭제한다.
			case TVN_DELETEITEM:
				TV.mask=TVIF_IMAGE;
				TV.hItem=ntv->itemOld.hItem;
				TreeView_GetItem(hTree,&TV);
				if (TV.iImage == DGI_CLIPROOT) {
					free((TCHAR *)TV.lParam);
				} else {
					cr=(CClipRecord *)ntv->itemOld.lParam;
					if (cr) {
						if (cr->Content) {
							free(cr->Content);
						}
						delete cr;
					}
				}
				return TRUE;
			// 더블클릭은 상용구 편집과 같다.
			case NM_DBLCLK:
				PostMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_BTNCLIPEDIT,0),0);
				return TRUE;
			case TVN_KEYDOWN:
				switch (((NMTVKEYDOWN *)lParam)->wVKey) {
				case VK_DELETE:
					if (IsWindowEnabled(GetDlgItem(hDlg,IDC_BTNCLIPDELETE))) {
						SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_BTNCLIPDELETE,BN_CLICKED),0);
					}
					break;
				case VK_F2:
					SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_BTNCLIPRENAME,BN_CLICKED),0);
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

// 상용구 추가 및 수정 대화상자. lParam으로 CClipRecord의 포인터를 전달받음.
BOOL CALLBACK EditClipProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	static CApiEdit *pAe;
	static CClipRecord *cr;
	RECT brt;
	int len;
	TCHAR szTemp[128];
	const TCHAR *s;
	TCHAR *d;

	switch(iMessage) {
	case WM_INITDIALOG:
		MoveToParentCenter(hDlg);
		SendDlgItemMessage(hDlg,IDC_EDCLIPNAME,EM_LIMITTEXT,31,0);
		SendDlgItemMessage(hDlg,IDC_EDCLIPAUTONAME,EM_LIMITTEXT,15,0);

		// 버튼 자리에 ApiEdit를 배치한다. 대부분의 설정은 기본값이며 몇 가지만 수정한다.
		GetWindowRect(GetDlgItem(hDlg,IDC_PLACEAPIEDIT),&brt);
		ScreenToClient(hDlg,(LPPOINT)&brt);
		ScreenToClient(hDlg,(LPPOINT)&brt.right);
		pAe=new CApiEdit;
		pAe->Create(brt.left,brt.top,brt.right-brt.left,brt.bottom-brt.top,
			WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP,0,hDlg);
		SetSetting(*pAe);
		pAe->ChangeStyle(AES_SHOWMARGIN,TRUE);
		pAe->ChangeStyle(AES_SHOWLINENUM,TRUE);
//		pAe->SetShowState(SHOWMARGIN,TRUE);
//		pAe->SetShowState(SHOWLINENUM,TRUE);
		pAe->SetWrap(2);

		SendDlgItemMessage(hDlg,IDC_CBCLIPSYNTAX,CB_ADDSTRING,0,(LPARAM)"없음");
		SendDlgItemMessage(hDlg,IDC_CBCLIPSYNTAX,CB_ADDSTRING,0,(LPARAM)"C/C++");
		SendDlgItemMessage(hDlg,IDC_CBCLIPSYNTAX,CB_ADDSTRING,0,(LPARAM)"HTML");
		SendDlgItemMessage(hDlg,IDC_CBCLIPSYNTAX,CB_ADDSTRING,0,(LPARAM)"SQL");

		SendDlgItemMessage(hDlg,IDC_CBCLIPDEFACTION,CB_ADDSTRING,0,(LPARAM)"삽입");
		SendDlgItemMessage(hDlg,IDC_CBCLIPDEFACTION,CB_ADDSTRING,0,(LPARAM)"복사");
		SendDlgItemMessage(hDlg,IDC_CBCLIPDEFACTION,CB_ADDSTRING,0,(LPARAM)"새 파일로");

		// 상용구 내용 출력
		cr=(CClipRecord *)lParam;
		SetDlgItemText(hDlg,IDC_EDCLIPNAME,cr->Name);
		SetDlgItemText(hDlg,IDC_EDCLIPAUTONAME,cr->AutoName);
		SendDlgItemMessage(hDlg,IDC_CBCLIPSYNTAX,CB_SETCURSEL,cr->nParser,0);
		SendDlgItemMessage(hDlg,IDC_CBCLIPDEFACTION,CB_SETCURSEL,cr->DefAction,0);
		if (cr->Content) {
			pAe->SetParser(cr->nParser);
			pAe->SetText(cr->Content,lstrlen(cr->Content));
		}
		SetFocus(pAe->hWnd);
		return FALSE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_CBCLIPSYNTAX:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				pAe->SetParser(SendDlgItemMessage(hDlg,IDC_CBCLIPSYNTAX,CB_GETCURSEL,0,0));
				return TRUE;
			}
			return FALSE;
		case IDOK:
			// 편집 결과를 메모리를 할당해서 돌려 준다.
			len=pAe->GetTextLength();
			if (len==0) {
				MessageBox(hDlg,"상용구 내용을 입력해 주십시오.","알림",MB_OK);
				return TRUE;
			}
			GetDlgItemText(hDlg,IDC_EDCLIPAUTONAME,szTemp,16);
			if (strchr(szTemp,' ')) {
				MessageBox(hDlg,"자동 삽입명에는 공백을 쓸 수 없습니다.","알림",MB_OK);
				return TRUE;
			}

			if (cr->Content!=NULL) {
				free(cr->Content);
			}
			cr->Content=(TCHAR *)calloc(len+1,1);
			pAe->GetText(cr->Content,len+1);

			// 이름이 없을 경우 본문중 일부를 알아서 취한다.
			// 앞쪽 공백 무시. DBCS 반쪽 금지. 24자 이하. 개행이나 문서 끝 인식. 탭 문서는 공백으로
			GetDlgItemText(hDlg,IDC_EDCLIPNAME,szTemp,32);
			if (IsStrEmpty(szTemp)) {
				s=cr->Content;
				SkipSpace(s);
				for (d=szTemp;;) {
					if (d-szTemp > 24 || *s == NULL || *s == '\r') {
						break;
					}
					if (IsDBCSLeadByte(*s)) {
						*d++=*s++;
						*d++=*s++;
					} else {
						if (*s == '\t') {
							*d++=' ';
							s++;
						} else {
							*d++=*s++;
						}
					}
				}
				*d=0;
			}
			StringNormalize(szTemp,cr->Name);
			GetDlgItemText(hDlg,IDC_EDCLIPAUTONAME,cr->AutoName,16);
			cr->nParser=SendDlgItemMessage(hDlg,IDC_CBCLIPSYNTAX,CB_GETCURSEL,0,0);
			cr->DefAction=SendDlgItemMessage(hDlg,IDC_CBCLIPDEFACTION,CB_GETCURSEL,0,0);

			DestroyWindow(pAe->hWnd);
			delete pAe;
			EndDialog(hDlg,IDOK);
			return TRUE;
		case IDCANCEL:
			DestroyWindow(pAe->hWnd);
			delete pAe;
			EndDialog(hDlg,IDCANCEL);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

// 분석기를 변경하고 미리보기 출력
void DisplayClipPreview(CApiEdit *pAe,CClipRecord *cr)
{
	pAe->SetParser(cr->nParser);
	pAe->SetText(cr->Content,lstrlen(cr->Content));
	// 항상 읽기 전용
	pAe->SetReadOnly(TRUE);
}

// 편집하던 파일의 저장을 확인한다.
int ConfirmSaveClip(HWND hDlg)
{
	int Result;

	Result=MessageBox(hDlg,"편집하던 파일을 저장하시겠습니까?","질문",MB_YESNOCANCEL);
	if (Result==IDCANCEL || Result==IDNO) {
		return Result;
	}

	if (SaveClipFileFromDlg(hDlg)==FALSE) {
		return IDCANCEL;
	}

	return IDYES;
}

// Path 파일을 대화상자로 읽어들인다.
BOOL LoadClipFileToDlg(HWND hDlg,TCHAR *Path)
{
	HWND hTree;
	CClipFile *cf;
	BOOL Result;

	cf=new CClipFile;
	// 에러 처리할 것
	if (cf->Load(Path)==0) {
		hTree=GetDlgItem(hDlg,IDC_CLIPTREE);
		TreeView_DeleteAllItems(hTree);
		AddClipFileToTree(hTree,TVI_LAST,"New Clip",cf);
		SetDlgItemText(hDlg,IDC_CLIPFILE,Path);
		SetDlgItemText(hDlg,IDC_CLIPNAME,cf->Header.Name);
		SetDlgItemText(hDlg,IDC_CLIPDESC,cf->Header.Desc);
		Result=TRUE;
	} else {
		Result=FALSE;
	}
	delete cf;
	return Result;
}

// cf의 내용을 hTree 루트에 추가한다.
void AddClipFileToTree(HWND hTree,HTREEITEM hPrev,TCHAR *Path,CClipFile *cf)
{
	TVINSERTSTRUCT TI;
	HTREEITEM hRoot;
	TCHAR *p;

	TI.hParent=TVI_ROOT;
	TI.hInsertAfter=hPrev;
	TI.item.mask=TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	TI.item.iImage=DGI_CLIPROOT;
	TI.item.iSelectedImage=DGI_CLIPROOTSEL;
	TI.item.pszText=cf->Header.Name;
	// 루트에는 경로를 저장해 둔다.
	p=(TCHAR *)malloc(MAX_PATH);
	lstrcpy(p,Path);
	TI.item.lParam=(LPARAM)p;
	hRoot=TreeView_InsertItem(hTree,&TI);

	AddClipRecordToTree(hTree,hRoot,cf,-1);

	TreeView_SelectItem(hTree,hRoot);
	TreeView_Expand(hTree,hRoot,TVE_EXPAND);
}

// pid 차일드를 pNode에 추가한다.
void AddClipRecordToTree(HWND hTree,HTREEITEM pNode,CClipFile *cf,int pid)
{
	int i;
	TVINSERTSTRUCT TI;
	HTREEITEM Node;
	CClipRecord *cr;

	for (i=0;i<cf->Header.Num;i++) {
		if (cf->arRecord[i].Parent != pid) {
			continue;
		}
		TI.hParent=pNode;
		TI.hInsertAfter=TVI_LAST;
		TI.item.mask=TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
		if (cf->arRecord[i].Type == DGCT_FOLDER) {
			TI.item.iImage=DGI_CLIPFOLDER;
			TI.item.iSelectedImage=DGI_CLIPFOLDERSEL;
		} else {
			TI.item.iImage=DGI_CLIP;
			TI.item.iSelectedImage=DGI_CLIPSEL;
		}
		TI.item.pszText=cf->arRecord[i].Name;

		// 레코드의 사본을 새로 생성해서 lParam에 기억시킨다.
		cr=new CClipRecord;
		*cr=cf->arRecord[i];
		if (cf->arRecord[i].Content) {
			cr->Content=(TCHAR *)calloc(cf->arRecord[i].len+1,1);
			lstrcpy(cr->Content,cf->arRecord[i].Content);
		}
		TI.item.lParam=(LPARAM)cr;
		Node=TreeView_InsertItem(hTree,&TI);

		if (cf->arRecord[i].Type == DGCT_FOLDER) {
			AddClipRecordToTree(hTree,Node,cf,i);
		}
	}
}

// pNode의 자식들을 cf에 추가한다.
void AddClipRecordToFile(HWND hTree,HTREEITEM hParent,int pid,CClipFile *cf)
{
	HTREEITEM hChild;
	CClipRecord *cr,*ncr;
	TVITEM TV;
	int self;

	// 루트가 아닌 폴더 레코드를 먼저 기록한다.
	if (hParent==NULL) {
		hParent=TreeView_GetRoot(hTree);
		self=pid;
	} else {
		TV.mask=TVIF_PARAM | TVIF_IMAGE;
		TV.hItem=hParent;
		TreeView_GetItem(hTree,&TV);
		cr=(CClipRecord *)TV.lParam;

		// 사본 레코드를 작성.Type,Parent는 트리의 정보로부터 얻는다. 폴더이므로 Content는 없다.
		ncr=new CClipRecord;
		*ncr=*cr;
		ncr->Parent=pid;
		ncr->Type=DGCT_FOLDER;
		self=cf->Header.Num;
		cf->arRecord[self]=*ncr;
		cf->Header.Num++;
	}

	hChild=TreeView_GetChild(hTree,hParent);
	while (hChild) {
		TV.mask=TVIF_PARAM | TVIF_IMAGE;
		TV.hItem=hChild;
		TreeView_GetItem(hTree,&TV);
		cr=(CClipRecord *)TV.lParam;
		if (TV.iImage==DGI_CLIPFOLDER) {
			AddClipRecordToFile(hTree,hChild,self,cf);
		} else {
			// 사본 레코드를 작성하고 arRecord에 추가한다. Content는 새로 할당해서 복사하고
			// len,Type,Parent는 트리의 정보로부터 얻는다.
			ncr=new CClipRecord;
			*ncr=*cr;
			ncr->len=lstrlen(ncr->Content);
			ncr->Parent=self;
			ncr->Content=(TCHAR *)calloc(ncr->len+1,1);
			lstrcpy(ncr->Content,cr->Content);
			ncr->Type=DGCT_CLIP;
			cf->arRecord[cf->Header.Num]=*ncr;
			cf->Header.Num++;
		}
		hChild=TreeView_GetNextSibling(hTree,hChild);
	}
}

// 현재 대화상자의 내용대로 묶음 파일을 작성한다.
BOOL SaveClipFileFromDlg(HWND hDlg)
{
	HWND hTree;
	CClipFile *cf;
	int RecNum;
	OPENFILENAME OFN;
	TCHAR Path[MAX_PATH];
	TCHAR szTemp[64];
	BOOL Result=TRUE;

	GetDlgItemText(hDlg,IDC_CLIPFILE,Path,MAX_PATH);
	if (lstrcmp(Path,"저장되지 않은 파일")==0) {
		memset(&OFN, 0, sizeof(OPENFILENAME));
		OFN.lStructSize = sizeof(OPENFILENAME);
		OFN.hwndOwner=hDlg;
		OFN.lpstrFilter="당근 상용구 파일\0*.dgc\0";
		Path[0]=0;
		OFN.lpstrFile=Path;
		OFN.nMaxFile=MAX_PATH;
		OFN.lpstrDefExt="dgc";
		if (GetSaveFileName(&OFN)==FALSE) {
			return FALSE;
		}
	}

	hTree=GetDlgItem(hDlg,IDC_CLIPTREE);
	GetDlgItemText(hDlg,IDC_CLIPNAME,szTemp,64);
	if (IsStrEmpty(szTemp)) {
		MessageBox(hDlg,"묶음의 이름을 입력해 주십시오","알림",MB_OK);
		return FALSE;
	}

	// 헤더 정보 먼저 작성. 레코드 배열 미리 할당해 놓음
	cf=new CClipFile;
	StringNormalize(szTemp,cf->Header.Name);
	GetDlgItemText(hDlg,IDC_CLIPDESC,cf->Header.Desc,128);
	RecNum=TreeView_GetCount(hTree)-1;
	cf->arRecord=(CClipRecord *)malloc(sizeof(CClipRecord)*RecNum);

	AddClipRecordToFile(hTree,NULL,-1,cf);

	// 상용구 파일을 저장한 후 상용구 창을 업데이트한다.
	if (cf->Save(Path)==FALSE) {
		Result=FALSE;
	} else {
		LoadClipFile(Path,TRUE);
	}

	delete cf;
	return Result;
}

HTREEITEM CopyTreeItem2(HWND hTree, HTREEITEM hSrc, HTREEITEM hDest, BOOL bNext)
{
	HTREEITEM hNew;
	TVINSERTSTRUCT TI;
	TVITEM TV;
	TCHAR Text[MAX_PATH];
	CClipRecord *cr,*ncr;

	TV.hItem=hSrc;
	TV.mask=TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	TV.pszText=Text;
	TV.cchTextMax=256;
	TreeView_GetItem(hTree,&TV);

	TI.itemex.mask=TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	TI.itemex.pszText=Text;
	cr=(CClipRecord *)TV.lParam;
	// lParam은 다시 할당해서 등록한다 - 프로젝트와는 다르다.
	ncr=new CClipRecord;
	*ncr=*cr;
	if (ncr->Content) {
		ncr->len=lstrlen(ncr->Content);
		ncr->Content=(TCHAR *)calloc(ncr->len+1,1);
		lstrcpy(ncr->Content,cr->Content);
	}
	if (TV.iImage==DGI_CLIP) {
		ncr->Type=DGCT_CLIP;
	} else {
		ncr->Type=DGCT_FOLDER;
	}
	TI.itemex.lParam=(LPARAM)ncr;
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

HTREEITEM CopyTreeItems2(HWND hTree, HTREEITEM hSrc, HTREEITEM hDest,BOOL bNext)
{
	HTREEITEM hNew, hChild;

	hNew=CopyTreeItem2(hTree, hSrc, hDest,bNext);

	hChild=TreeView_GetChild(hTree, hSrc);
	while (hChild != NULL) {
		CopyTreeItems2(hTree,hChild, hNew,FALSE);
		hChild=TreeView_GetNextSibling(hTree,hChild);
	}

	return hNew;
}

LRESULT CALLBACK DGClipProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	RECT crt;
	TVITEM TV;
	CClipRecord *cr;
	HTREEITEM Node,tNode,tNode2;
	HWND hActive;
	SInfo *pSi;
	HMENU hMenu, hPopup;
	TVHITTESTINFO ht;
	TCHAR Path[MAX_PATH];

	switch(iMessage) {
	case WM_CREATE:
		hClipTree=CreateWindow(WC_TREEVIEW, "", WS_CHILD | WS_VISIBLE | 
			TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | 
			TVS_SHOWSELALWAYS | TVS_INFOTIP,
			0,0,0,0,hWnd,(HMENU)1,g_hInst,NULL);
		SendMessage(hClipTree, TVM_SETIMAGELIST, (WPARAM)TVSIL_NORMAL, (LPARAM)hFileImg);
		return 0;
	case WM_SIZE:
		GetClientRect(hWnd,&crt);
		MoveWindow(hClipTree,0,0,crt.right,crt.bottom,TRUE);
		return 0;
	case WM_CONTEXTMENU:
		POINT pt;
		hMenu=LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_POPUP));
		hPopup=GetSubMenu(hMenu, 4);

		ht.pt.x=LOWORD(lParam);
		ht.pt.y=HIWORD(lParam);
		ScreenToClient(hWnd,&ht.pt);
		TreeView_HitTest(hClipTree,&ht);
		TreeView_SelectItem(hClipTree,ht.hItem);

		if (ht.hItem) {
			TV.mask=TVIF_IMAGE | TVIF_PARAM;
			TV.hItem=ht.hItem;
			TreeView_GetItem(hClipTree,&TV);
			if (TV.iImage != DGI_CLIP) {
				EnableMenuItem(hMenu, IDM_CLIP_INSERT, MF_BYCOMMAND | MF_GRAYED);
				EnableMenuItem(hMenu, IDM_CLIP_COPY, MF_BYCOMMAND | MF_GRAYED);
				EnableMenuItem(hMenu, IDM_CLIP_NEWFILE, MF_BYCOMMAND | MF_GRAYED);
			}
		} else {
			EnableMenuItem(hMenu, IDM_CLIP_INSERT, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(hMenu, IDM_CLIP_COPY, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(hMenu, IDM_CLIP_NEWFILE, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(hMenu, IDM_CLIP_CLOSE, MF_BYCOMMAND | MF_GRAYED);
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
		switch (LOWORD(wParam)) {
		case IDM_CLIP_CLOSEWND:
			Option.bShowClipWnd=FALSE;
			Relayout();
			break;
		case IDM_CLIP_INSERT:
		case IDM_CLIP_NEWFILE:
			Node=TreeView_GetSelection(hClipTree);
			TV.mask=TVIF_IMAGE | TVIF_PARAM;
			TV.hItem=Node;
			TreeView_GetItem(hClipTree,&TV);
			if (TV.iImage==DGI_CLIP) {
				cr=(CClipRecord *)TV.lParam;
				if (LOWORD(wParam)==IDM_CLIP_INSERT) {
					hActive=(HWND)SendMessage(g_hMDIClient,WM_MDIGETACTIVE,0,(LPARAM)NULL);
				} else {
					hActive=NewChild();
				}
				if (hActive!=NULL) {
					pSi=(SInfo *)GetWindowLong(hActive,0);
					pSi->Ae.GetActiveView()->InsertClipText(cr->Content);
					if (LOWORD(wParam)==IDM_CLIP_NEWFILE) {
						pSi->Ae.SetParser(cr->nParser);
					}
					// 삽입 후 활성창으로 포커스를 강제로 넘긴다.
					SetFocus(hActive);
				}
			}
			break;
		case IDM_CLIP_COPY:
			HGLOBAL hmem;
			TCHAR *ptr;
			TCHAR *s,*d;
			int len;

			Node=TreeView_GetSelection(hClipTree);
			TV.mask=TVIF_IMAGE | TVIF_PARAM;
			TV.hItem=Node;
			TreeView_GetItem(hClipTree,&TV);
			if (TV.iImage==DGI_CLIP) {
				cr=(CClipRecord *)TV.lParam;
				hmem=GlobalAlloc(GHND, lstrlen(cr->Content)+1);
				ptr=(TCHAR *)GlobalLock(hmem);

				// 표식은 빼고 복사해야 한다.
				len=lstrlen(cr->Content);
				for (s=cr->Content,d=ptr;*s;) {
					if (s-cr->Content <= len-3 && s[0]=='^' && s[1]=='|' && s[2]=='^') {
						s+=3;
					} else {
						*d++=*s++;
					}
				}
				*d=0;
				GlobalUnlock(hmem);
				if (OpenClipboard(g_hFrameWnd)) {
					EmptyClipboard();
					SetClipboardData(CF_TEXT,hmem);
					CloseClipboard();
				}
			}
			break;
		case IDM_CLIP_EDIT:
			Node=TreeView_GetSelection(hClipTree);
			if (Node == NULL) {
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_CLIPFILE), 
					g_hFrameWnd, ClipDlgProc,(LPARAM)NULL);
			} else {
				for (tNode=Node;tNode2=TreeView_GetParent(hClipTree,tNode);tNode=tNode2);
				TV.mask=TVIF_PARAM;
				TV.hItem=tNode;
				TreeView_GetItem(hClipTree,&TV);
				lstrcpy(Path,(TCHAR *)TV.lParam);
				DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_CLIPFILE), 
					g_hFrameWnd, ClipDlgProc,(LPARAM)Path);
			}
			break;
		case IDM_CLIP_CLOSE:
			Node=TreeView_GetSelection(hClipTree);
			for (tNode=Node;tNode2=TreeView_GetParent(hClipTree,tNode);tNode=tNode2);
			TreeView_DeleteItem(hClipTree,tNode);
			break;
		}
		return 0;
	case WM_NOTIFY:
		LPNMHDR hdr;
		LPNMTREEVIEW ntv;
		LPNMTVDISPINFO ndi;
		LPNMTVGETINFOTIP nit;
		hdr=(LPNMHDR)lParam;
		ntv=(LPNMTREEVIEW)lParam;
		ndi=(LPNMTVDISPINFO)lParam;
		nit=(LPNMTVGETINFOTIP)lParam;
		if (hdr->hwndFrom == hClipTree) {
			switch (hdr->code) {
			case NM_DBLCLK:
				Node=TreeView_GetSelection(hClipTree);
				TV.mask=TVIF_PARAM;
				TV.hItem=Node;
				TreeView_GetItem(hClipTree,&TV);
				cr=(CClipRecord *)TV.lParam;
				switch (cr->DefAction) {
				case 0:
					// SendMessage로 보내면 포커스가 활성창으로 가지 않음
					PostMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_CLIP_INSERT,0),0);
					break;
				case 1:
					PostMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_CLIP_COPY,0),0);
					break;
				case 2:
					PostMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_CLIP_NEWFILE,0),0);
					break;
				}
				break;
			case TVN_DELETEITEM:
				TV.mask=TVIF_IMAGE;
				TV.hItem=ntv->itemOld.hItem;
				TreeView_GetItem(hClipTree,&TV);
				if (TV.iImage == DGI_CLIPROOT) {
					free((TCHAR *)TV.lParam);
				} else {
					cr=(CClipRecord *)ntv->itemOld.lParam;
					if (cr) {
						if (cr->Content) {
							free(cr->Content);
						}
						delete cr;
					}
				}
				break;
			case TVN_GETINFOTIP:
				if (Option.bPreviewClip) {
					TV.mask=TVIF_PARAM | TVIF_IMAGE;
					TV.hItem=nit->hItem;
					TreeView_GetItem(hClipTree,&TV);
					if (TV.iImage == DGI_CLIP) {
						cr=(CClipRecord *)TV.lParam;
						if (cr->Content) {
							lstrcpyn(nit->pszText,cr->Content,min(256,nit->cchTextMax));
						}
					}
				}
				break;
			}
		}
		return 0;
	case WM_DESTROY:
		return 0;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}

BOOL LoadClipFile(TCHAR *Path,BOOL bUpdate)
{
	CClipFile *cf;
	HTREEITEM hChild, hPrev;
	TVITEM TV;
	BOOL Result;

	// 이미 열려 있는지 확인한다.
	hPrev=TVI_LAST;
	hChild=TreeView_GetRoot(hClipTree);
	while (hChild) {
		TV.mask=TVIF_PARAM;
		TV.hItem=hChild;
		TreeView_GetItem(hClipTree,&TV);
		if (lstrcmpi((TCHAR *)TV.lParam,Path) == 0) {
			hPrev=TreeView_GetPrevSibling(hClipTree,hChild);
			if (hPrev==NULL) {
				hPrev=TVI_FIRST;
			}
			TreeView_DeleteItem(hClipTree,hChild);
			break;
		}
		hChild=TreeView_GetNextSibling(hClipTree,hChild);
	}

	// 열려 있지 않고 업데이트만 하면 아무 것도 하지 않음
	// 즉 업데이트는 열려 있는 파일만 대상으로 한다.
	if (hChild == NULL && bUpdate==TRUE) {
		return TRUE;
	}

	cf=new CClipFile;
	if (cf->Load(Path)==0) {
		AddClipFileToTree(hClipTree,hPrev,Path,cf);
		Result=TRUE;
	} else {
		Result=FALSE;
	}
	delete cf;
	return Result;
}

// 열려진 상용구에서 상용구를 검색한다.
CClipRecord *FindClipRecord(TCHAR *Name,BOOL bAutoName)
{
	HTREEITEM hRoot;
	CClipRecord *cr;

	hRoot=TreeView_GetRoot(hClipTree);
	while (hRoot) {
		cr=FindClipRecordSub(hRoot,Name,bAutoName);
		if (cr) {
			return cr;
		}
		hRoot=TreeView_GetNextSibling(hClipTree,hRoot);
	}
	return NULL;
}

// hRoot 트리에서 상용구를 검색한다.
CClipRecord *FindClipRecordSub(HTREEITEM hRoot,TCHAR *Name,BOOL bAutoName)
{
	HTREEITEM hChild;
	TVITEM TV;
	CClipRecord *cr;

	hChild=TreeView_GetChild(hClipTree,hRoot);
	while (hChild) {
		TV.mask=TVIF_PARAM | TVIF_IMAGE;
		TV.hItem=hChild;
		TreeView_GetItem(hClipTree,&TV);
		// 폴더이면 재귀호출한다.
		if (TV.iImage==DGI_CLIPFOLDER) {
			cr=FindClipRecordSub(hChild,Name,bAutoName);
			if (cr) {
				return cr;
			}
		} else {
			// 자동 삽입명 또는 이름으로 검색한다.
			cr=(CClipRecord *)TV.lParam;
			if (bAutoName) {
				if (lstrcmpi(cr->AutoName,Name) == 0) {
					return cr;
				}
			} else {
				if (lstrcmpi(cr->Name,Name) == 0) {
					return cr;
				}
			}
		}
		hChild=TreeView_GetNextSibling(hClipTree,hChild);
	}
	return NULL;
}
