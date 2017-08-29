#include "stdafx.h"

LRESULT CALLBACK DGFileProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	TVITEM TV;
	HTREEITEM Node;
	TCHAR Path[MAX_PATH];
	TCHAR *szFilter;
	int idx;
	HMENU hMenu, hPopup;
	LPNMHDR hdr;
	LPNMTREEVIEW ntv;
	LPNMTVDISPINFO ndi;
	TCHAR NewPath[MAX_PATH];
	TCHAR Drive[_MAX_DRIVE];
	TCHAR Dir[_MAX_DIR];
	HTREEITEM tNode, tNode2;
	TVHITTESTINFO ht;

	switch(iMessage) {
	case WM_CREATE:
		hFilter=CreateWindow("combobox",NULL,WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
			0,0,0,0,hWnd,(HMENU)0,g_hInst,NULL);

		SendMessage(hFilter,WM_SETFONT,(WPARAM)hGul9,(LPARAM)FALSE);

		idx=SendMessage(hFilter,CB_ADDSTRING,0,(LPARAM)"모든 텍스트 파일");
		SendMessage(hFilter,CB_SETITEMDATA,idx,(LPARAM)"*.txt;*.c;*.cpp;*.h;*.hpp;"
			"*.html;*.htm;*.asp;*.asa;*.php;*.php3;*.php4;*.shtml;*.sql;*.ini;"
			"*.log;*.pas;*.java;*.xml");
		idx=SendMessage(hFilter,CB_ADDSTRING,0,(LPARAM)"모든 파일");
		SendMessage(hFilter,CB_SETITEMDATA,idx,(LPARAM)"*.*");
		idx=SendMessage(hFilter,CB_ADDSTRING,0,(LPARAM)"텍스트 파일");
		SendMessage(hFilter,CB_SETITEMDATA,idx,(LPARAM)"*.txt");
		idx=SendMessage(hFilter,CB_ADDSTRING,0,(LPARAM)"C/C++ 파일");
		SendMessage(hFilter,CB_SETITEMDATA,idx,(LPARAM)"*.c;*.cpp;*.h;*.hpp;*.inl");
		idx=SendMessage(hFilter,CB_ADDSTRING,0,(LPARAM)"HTML 파일");
		SendMessage(hFilter,CB_SETITEMDATA,idx,(LPARAM)"*.html;*.htm;*.asp;*.asa;*.php;*.php3;*.php4;*.shtml");
		idx=SendMessage(hFilter,CB_ADDSTRING,0,(LPARAM)"SQL 파일");
		SendMessage(hFilter,CB_SETITEMDATA,idx,(LPARAM)"*.sql");
		SendMessage(hFilter,CB_SETCURSEL,Option.FilterIndex,0);

		hFileTree=CreateWindow(WC_TREEVIEW, "", WS_CHILD | WS_VISIBLE | 
			TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_EDITLABELS | TVS_SHOWSELALWAYS,
			0,0,0,0,hWnd,(HMENU)1,g_hInst,NULL);

		hFileImg=ImageList_LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_FILES), 16, 1, RGB(255,255,255));
		SendMessage(hFileTree, TVM_SETIMAGELIST, (WPARAM)TVSIL_NORMAL, (LPARAM)hFileImg);

		InitTree();
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 0:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				Option.FilterIndex=SendMessage(hFilter,CB_GETCURSEL,0,0);
				RefreshTree();
				break;
			}
			break;
		case IDM_FILEWND_CLOSE:
			Option.bShowFileWnd=FALSE;
			Relayout();
			break;
		case IDM_FILEWND_REFRESH:
			RefreshTree();
			break;
		case IDM_FILEWND_HIDDEN:
			Option.bShowHidden=!Option.bShowHidden;
			RefreshTree();
			break;
		case IDM_FILEWND_BROWSE:
			Option.bBrowseMode=!Option.bBrowseMode;
			break;
		case IDM_FILEWND_DELETE:
			Node=TreeView_GetSelection(hFileTree);
			GetNodePath(Node,Path);
			Path[lstrlen(Path)+1]=0;
			SHFILEOPSTRUCT sf;
			sf.hwnd=hWnd;
			sf.wFunc=FO_DELETE;
			sf.pFrom=Path;
			sf.pTo=NULL;
			sf.fFlags=FOF_ALLOWUNDO;
			sf.lpszProgressTitle="";
			if (SHFileOperation(&sf)==0) {
				if (sf.fAnyOperationsAborted == FALSE) {
					TreeView_DeleteItem(hFileTree,Node);
				}
			}
			break;
		case IDM_FILEWND_RENAME:
			Node=TreeView_GetSelection(hFileTree);
			TreeView_EditLabel(hFileTree,Node);
			break;
		case IDM_PROJECT_EDIT:
			Node=TreeView_GetSelection(hFileTree);
			for (tNode=Node;tNode2=TreeView_GetParent(hFileTree,tNode);tNode=tNode2);
			TV.mask=TVIF_PARAM;
			TV.hItem=tNode;
			TreeView_GetItem(hFileTree,&TV);
			lstrcpy(Path,(TCHAR *)TV.lParam);
			DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_PROJECT), 
				g_hFrameWnd, DgProjectProc,(LPARAM)Path);
			break;
		case IDM_PROJECT_CLOSE:
			Node=TreeView_GetSelection(hFileTree);
			for (tNode=Node;tNode2=TreeView_GetParent(hFileTree,tNode);tNode=tNode2);
			TreeView_DeleteItem(hFileTree,tNode);
			break;
		}
		return 0;
	case WM_NOTIFY:
		hdr=(LPNMHDR)lParam;
		ntv=(LPNMTREEVIEW)lParam;
		ndi=(LPNMTVDISPINFO)lParam;
		if (hdr->hwndFrom == hFileTree) {
			switch (hdr->code) {
			case TVN_ITEMEXPANDING:
				if (ntv->action==TVE_EXPAND) {
					if (TreeView_GetChild(hFileTree,ntv->itemNew.hItem)==NULL) {
						GetNodePath(ntv->itemNew.hItem,Path);
						szFilter=(TCHAR *)SendMessage(hFilter,CB_GETITEMDATA,
							SendMessage(hFilter,CB_GETCURSEL,0,0),0);
						bContFIF=TRUE;
						FindInFiles(Path,szFilter,Option.bShowHidden ? FIF_INCHID:0,
							OnFindNode,ntv->itemNew.hItem);
					}
				}
				break;
			case TVN_SELCHANGED:
				if (Option.bBrowseMode) {
					Node=ntv->itemNew.hItem;
					TV.mask=TVIF_IMAGE;
					TV.hItem=Node;
					TreeView_GetItem(hFileTree,&TV);
					if (TV.iImage==DGI_FILE) {
						GetNodePath(Node,Path);
						OpenFromFile(Path,FALSE,TRUE);
					}
				}
				break;
			case NM_DBLCLK:
				Node=TreeView_GetSelection(hFileTree);
				if (Node) {
					TV.mask=TVIF_IMAGE;
					TV.hItem=Node;
					TreeView_GetItem(hFileTree,&TV);
					if (TV.iImage==DGI_FILE) {
						GetNodePath(Node,Path);
						OpenFromFile(Path);
					}
				}
				break;
			case TVN_BEGINLABELEDIT:
				for (tNode=ndi->item.hItem;tNode2=TreeView_GetParent(hFileTree,tNode);tNode=tNode2);
				TV.mask=TVIF_IMAGE;
				TV.hItem=tNode;
				TreeView_GetItem(hFileTree,&TV);
				if (tNode==ndi->item.hItem || TV.iImage != DGI_DRIVE) {
					return TRUE;
				} else {
					return FALSE;
				}
			case TVN_ENDLABELEDIT:
				ndi=(LPNMTVDISPINFO)lParam;
				if (ndi->item.pszText == NULL)
					return FALSE;
				GetNodePath(ndi->item.hItem,Path);
				_splitpath(Path,Drive,Dir,NULL,NULL);
				wsprintf(NewPath,"%s%s%s",Drive,Dir,ndi->item.pszText);
				if (MoveFile(Path,NewPath)==FALSE) {
					return FALSE;
				}
				return TRUE;
			case TVN_DELETEITEM:
				if (ntv->itemOld.lParam) {
					free((LPVOID)ntv->itemOld.lParam);
				}
				break;
			}
		}
		return 0;
	case WM_CONTEXTMENU:
		POINT pt;
		hMenu=LoadMenu(g_hInst, MAKEINTRESOURCE(IDR_POPUP));

		ht.pt.x=LOWORD(lParam);
		ht.pt.y=HIWORD(lParam);
		ScreenToClient(hWnd,&ht.pt);
		TreeView_HitTest(hFileTree,&ht);
		TreeView_SelectItem(hFileTree,ht.hItem);

		for (tNode=ht.hItem;tNode2=TreeView_GetParent(hFileTree,tNode);tNode=tNode2);
		TV.mask=TVIF_IMAGE;
		TV.hItem=tNode;
		TreeView_GetItem(hFileTree,&TV);

		if (TV.iImage == DGI_PROJECT) {
			hPopup=GetSubMenu(hMenu, 2);
		} else {
			hPopup=GetSubMenu(hMenu, 1);
			if (Option.bShowHidden) {
				CheckMenuItem(hMenu, IDM_FILEWND_HIDDEN, MF_BYCOMMAND | MF_CHECKED);
			} else {
				CheckMenuItem(hMenu, IDM_FILEWND_HIDDEN, MF_BYCOMMAND | MF_UNCHECKED);
			}

			if (TreeView_GetParent(hFileTree,ht.hItem)==NULL) {
				EnableMenuItem(hMenu, IDM_FILEWND_DELETE, MF_BYCOMMAND | MF_GRAYED);
				EnableMenuItem(hMenu, IDM_FILEWND_RENAME, MF_BYCOMMAND | MF_GRAYED);
			} else {
				EnableMenuItem(hMenu, IDM_FILEWND_DELETE, MF_BYCOMMAND | MF_ENABLED);
				EnableMenuItem(hMenu, IDM_FILEWND_RENAME, MF_BYCOMMAND | MF_ENABLED);
			}
		}

		if (LOWORD(lParam)==65535) {
			pt.x=20;
			pt.y=20;
			ClientToScreen(hWnd,&pt);
			lParam=MAKELPARAM(pt.x,pt.y);
		}
		if (Option.bBrowseMode) {
			CheckMenuItem(hPopup, IDM_FILEWND_BROWSE, MF_BYCOMMAND | MF_CHECKED);
		} else {
			CheckMenuItem(hPopup, IDM_FILEWND_BROWSE, MF_BYCOMMAND | MF_UNCHECKED);
		}

		TrackPopupMenu(hPopup, TPM_LEFTALIGN, (short)LOWORD(lParam), (short)HIWORD(lParam), 
			0, hWnd, NULL);
		DestroyMenu(hMenu);
		return 0;
	case WM_SIZE:
		MoveWindow(hFileTree,0,0,LOWORD(lParam),HIWORD(lParam)-26,TRUE);
		MoveWindow(hFilter,0,HIWORD(lParam)-23,LOWORD(lParam),350,TRUE);
		return 0;
	case WM_DESTROY:
		ImageList_Destroy(hFileImg);
		return 0;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}

BOOL IsDirEmpty(TCHAR *Path)
{
	TCHAR SrchPath[MAX_PATH];
	WIN32_FIND_DATA wfd;
	HANDLE hSrch;
	BOOL nResult=TRUE;
	BOOL Result=TRUE;
	TCHAR *szFilter;

	lstrcpy(SrchPath, Path);
	lstrcat(SrchPath, "\\*.*");
	hSrch=FindFirstFile(SrchPath,&wfd);
	szFilter=(TCHAR *)SendMessage(hFilter,CB_GETITEMDATA,SendMessage(hFilter,CB_GETCURSEL,0,0),0);

	while (nResult) {
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (!(lstrcmp(wfd.cFileName,".") && lstrcmp(wfd.cFileName,".."))) {
				Result=FALSE;
				break;
			}
		} else {
			if (IsMatch(wfd.cFileName,szFilter)) {
				Result=FALSE;
				break;
			}
		}
		nResult=FindNextFile(hSrch,&wfd);
	}
	FindClose(hSrch);
	return Result;
}

HTREEITEM FindInsAfter(HTREEITEM hParent, int iImage, TCHAR *Name)
{
	HTREEITEM Node;
	TVITEM TV;
	TCHAR Text[MAX_PATH];

	Node=TreeView_GetNextItem(hFileTree,hParent,TVGN_CHILD);
	if (Node==NULL) {
		return TVI_FIRST;
	}
	if (iImage == DGI_FILE) {
		for (;;) {
			TV.mask=TVIF_IMAGE;
			TV.hItem=Node;
			TreeView_GetItem(hFileTree,&TV);
			if (TV.iImage== DGI_FILE) {
				break;
			}
			Node=TreeView_GetNextSibling(hFileTree,Node);
			if (Node==NULL) {
				return TVI_LAST;
			}
		}
	}

	for (;;) {
		TV.mask=TVIF_TEXT | TVIF_IMAGE;
		TV.hItem=Node;
		TV.pszText=Text;
		TV.cchTextMax=MAX_PATH;
		TreeView_GetItem(hFileTree,&TV);
		if (iImage==DGI_FOLDER && TV.iImage==DGI_FILE) {
			break;
		}
		if (lstrcmpi(Name,Text) < 0) {
			break;
		}
		Node=TreeView_GetNextSibling(hFileTree,Node);
		if (Node==NULL) {
			return TVI_LAST;
		}
	}

	Node=TreeView_GetPrevSibling(hFileTree,Node);
	if (Node==NULL) {
		return TVI_FIRST;
	} else {
		return Node;
	}
}

int OnFindNode(TCHAR *Path,DWORD Attr,LPVOID pCustom)
{
	TVINSERTSTRUCT TI;
	TVITEM TV;
	HTREEITEM hParent=(HTREEITEM)pCustom;
	HTREEITEM Node;
	TCHAR Ext[_MAX_EXT];
	TCHAR Name[MAX_PATH];

	_splitpath(Path,NULL,NULL,Name,Ext);
	lstrcat(Name,Ext);

	TI.hParent=hParent;
	TI.item.mask=TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	if (Attr & FILE_ATTRIBUTE_DIRECTORY) {
		if  (Name[0]=='.') {
			return 0;
		}
		TI.item.iImage=DGI_FOLDER;
		TI.item.iSelectedImage=DGI_FOLDERSEL;
	} else {
		TI.item.iImage=DGI_FILE;
		TI.item.iSelectedImage=DGI_FILESEL;
	}
	TI.hInsertAfter=FindInsAfter(hParent,TI.item.iImage,Name);
	TI.item.pszText=Name;
	Node=TreeView_InsertItem(hFileTree,&TI);

	if ((Attr & FILE_ATTRIBUTE_DIRECTORY) && IsDirEmpty(Path) == FALSE) {
		TV.mask=TVIF_CHILDREN;
		TV.hItem=Node;
		TV.cChildren=1;

		TreeView_SetItem(hFileTree,&TV);
	}
	return 0;
}

void GetNodePath(HTREEITEM pNode, TCHAR *Path)
{
	HTREEITEM tNode, tNode2, hRoot;
	TVITEM TV;
	TCHAR Text[MAX_PATH];
	TCHAR szTemp[MAX_PATH];
	TCHAR Drive[_MAX_DRIVE];
	TCHAR Dir[_MAX_DIR];

	TV.mask=TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE;
	TV.pszText=Text;
	TV.cchTextMax=MAX_PATH;
	Path[0]=0;

	for (hRoot=pNode;tNode2=TreeView_GetParent(hFileTree,hRoot);hRoot=tNode2);
	TV.hItem=hRoot;
	TreeView_GetItem(hFileTree,&TV);
	if (TV.lParam) {
		_splitpath((TCHAR *)TV.lParam,Drive,Dir,NULL,NULL);
	}

	if (TV.iImage == DGI_PROJECT) {
		TV.hItem=pNode;
		TreeView_GetItem(hFileTree,&TV);
		if (strchr(Text,':')) {
			if (strnicmp(Text,"ftp",3) == 0) {
				lstrcpy(Path,(TCHAR *)TV.lParam);
			} else {
				lstrcpy(Path,Text);
			}
		} else {
			lstrcpy(Path,Text);
			for (tNode=pNode;;) {
				tNode=TreeView_GetParent(hFileTree,tNode);
				if (tNode==hRoot) {
					wsprintf(Path,"%s%s%s",Drive,Dir,Text);
					break;
				}
				TV.mask=TVIF_TEXT | TVIF_IMAGE;
				TV.hItem=tNode;
				TV.pszText=szTemp;
				TreeView_GetItem(hFileTree,&TV);
				lstrcat(szTemp,"\\");
				lstrcat(szTemp,Path);
				lstrcpy(Path,szTemp);
				if (TV.iImage==DGI_FOLDER) {
					if (strchr(szTemp,':')) {
						break;
					}
					TV.mask=TVIF_IMAGE;
					TV.hItem=TreeView_GetParent(hFileTree,tNode);
					TreeView_GetItem(hFileTree,&TV);
					if (TV.iImage != DGI_FOLDER) {
						lstrcpy(szTemp,Path);
						wsprintf(Path,"%s%s%s",Drive,Dir,szTemp);
						break;
					}
				}
			}
		}
	} else {
		for (tNode=pNode;tNode!=NULL;) {
			TV.hItem=tNode;
			TreeView_GetItem(hFileTree,&TV);

			if (TreeView_GetParent(hFileTree,tNode)==NULL) {
				Text[1]=':';
				Text[2]=0;
				wsprintf(szTemp,"%s%s",Text,Path);
			} else {
				wsprintf(szTemp,"\\%s%s",Text,Path);
			}
			lstrcpy(Path,szTemp);
			tNode=TreeView_GetParent(hFileTree,tNode);
		}
	}
}

void InitTree()
{
	TVINSERTSTRUCT TI;
	TVITEM TV;
	HTREEITEM Node;
	TCHAR c;
	TCHAR Caption[MAX_PATH];
	UINT DriveType;

	for (;;) {
		Node=TreeView_GetRoot(hFileTree);
		if (Node==NULL) {
			break;
		}
		TV.mask=TVIF_IMAGE;
		TV.hItem=Node;
		TreeView_GetItem(hFileTree,&TV);
		if (TV.iImage != DGI_DRIVE) {
			break;
		}
		TreeView_DeleteItem(hFileTree,Node);
	}

	TI.hParent=0;
	Node=TVI_FIRST;
	TI.item.mask=TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	TI.item.iImage=DGI_DRIVE;
	TI.item.iSelectedImage=DGI_DRIVESEL;
	for (c='C';c<='Z';c++) {
		wsprintf(Caption,"%c:\\",c);
		DriveType=GetDriveType(Caption);
		if (DriveType==DRIVE_FIXED || DriveType==DRIVE_REMOTE) {
			wsprintf(Caption,"%c 드라이브",c);
			TI.item.pszText=Caption;
			TI.hInsertAfter=Node;
			Node=TreeView_InsertItem(hFileTree,&TI);

			TV.mask=TVIF_CHILDREN;
			TV.hItem=Node;
			TV.cChildren=1;
			TreeView_SetItem(hFileTree,&TV);
		}
	}

	for (;;) {
		Node=TreeView_GetNextSibling(hFileTree,Node);
		if (Node==NULL) {
			break;
		}
		TV.mask=TVIF_IMAGE | TVIF_PARAM;
		TV.hItem=Node;
		TreeView_GetItem(hFileTree,&TV);
		if (TV.iImage != DGI_PROJECT) {
			break;
		}
		lstrcpy(Caption,(TCHAR *)TV.lParam);
		LoadProject(Caption,TRUE);
	}
}

void GetCurrentFolder(TCHAR *Path)
{
	TVITEM TV;
	HTREEITEM Node;
	HTREEITEM hRoot,Node2;

	Node=TreeView_GetSelection(hFileTree);
	if (Node==NULL) {
		lstrcpy(Path,"");
		return;
	}
	
	for (hRoot=Node;Node2=TreeView_GetParent(hFileTree,hRoot);hRoot=Node2);
	TV.mask=TVIF_IMAGE | TVIF_TEXT;
	TV.hItem=hRoot;
	TV.pszText=Path;
	TV.cchTextMax=MAX_PATH;
	TreeView_GetItem(hFileTree,&TV);
	if (TV.iImage == DGI_PROJECT) {
		return;
	}

	TV.mask=TVIF_IMAGE;
	TV.hItem=Node;
	TreeView_GetItem(hFileTree,&TV);
	if (TV.iImage == DGI_FILE) {
		Node=TreeView_GetParent(hFileTree,Node);
	}

	GetNodePath(Node,Path);
}

void GotoFolderInTree(TCHAR *Path)
{
	TVITEM TV;
	HTREEITEM Node;
	TCHAR Text[MAX_PATH];
	TCHAR Part[MAX_PATH];
	TCHAR *p,*p2;

	if (lstrlen(Path)==0) {
		return;
	}

	if (strchr(Path,':')==NULL) {
		Node=TreeView_GetRoot(hFileTree);
		while (Node) {
			TV.mask=TVIF_IMAGE | TVIF_TEXT;
			TV.hItem=Node;
			TV.pszText=Text;
			TV.cchTextMax=MAX_PATH;
			TreeView_GetItem(hFileTree,&TV);
			if (TV.iImage==DGI_PROJECT && lstrcmpi(Text,Path)==0) {
				break;
			}
			Node=TreeView_GetNextSibling(hFileTree,Node);
		}
	} else {
		Node=TreeView_GetRoot(hFileTree);
		p=Path;
		for (;;) {
			if (p==Path) {
				wsprintf(Part,"%c 드라이브",Path[0]);
			} else {
				p2=strchr(p,'\\');
				if (p2==NULL) {
					lstrcpy(Part,p);
				} else {
					lstrcpyn(Part,p,p2-p+1);
				}
			}
			for (;;) {
				TV.mask=TVIF_TEXT;
				TV.pszText=Text;
				TV.cchTextMax=MAX_PATH;
				TV.hItem=Node;
				TreeView_GetItem(hFileTree,&TV);
				if (lstrcmpi(Text,Part)==0) {
					break;
				}
				Node=TreeView_GetNextSibling(hFileTree,Node);
				if (Node==NULL) {
					return;
				}
			}
			TreeView_Expand(hFileTree,Node,TVE_EXPAND);
			p=strchr(p,'\\');
			if (p==NULL) {
				break;
			}
			p++;
			Node=TreeView_GetChild(hFileTree,Node);
		}
	}

	if (Node) {
		TreeView_Select(hFileTree,Node,TVGN_CARET);
		TreeView_EnsureVisible(hFileTree,Node);
	}
}

void RefreshTree()
{
	TCHAR LastFolder[MAX_PATH];

	GetCurrentFolder(LastFolder);
	InitTree();
	GotoFolderInTree(LastFolder);
}

