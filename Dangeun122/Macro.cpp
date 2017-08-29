#include "stdafx.h"

SMacVerb arHostVerb[]={
	{"call","매크로 호출",0x00},
	{"clip","상용구 삽입",0x00},
	{"navi","윈도우 순환",0x02},
	{"set","설정 변경",0x02},
	{"toggle","설정 토글",0x02},
	{"","",0x00},
};

SMacObject arHostObject[]={
	{"next","다음 윈도우"},						// navi:0
	{"prev","이전 윈도우"},
	{NULL,NULL},
	{"showenter","개행 코드 보기"},					// set:3
	{"showtab","탭 코드 보기"},
	{"showspace","공백 보기"},
	{"showlinenum","줄번호 보기"},
	{"showmargin","마진 보기"},
	{"showcurline","현재행 강조"},
	{"hideseltype","선택영역 숨기기"},
	{NULL,NULL},
	{"showenter","개행 코드 보기"},					// toggle:11
	{"showtab","탭 코드 보기"},
	{"showspace","공백 보기"},
	{"showlinenum","줄번호 보기"},
	{"showmargin","마진 보기"},
	{NULL,NULL}
};

int arObjOfVerbHost[]={-1,-1,0,3,11};

// 생성자. 완전히 빈 상태로 객체 생성
CMacro::CMacro()
{
	Size=0;
	Num=0;
	arMacro=NULL;
	bModified=FALSE;
}

CMacro::~CMacro()
{
	int i;

	for (i=0;i<Num;i++) {
		free(arMacro[i].Content);
	}
	if (arMacro) {
		free(arMacro);
	}
}

// 이름으로부터 매크로 번호 찾음. 없으면 -1
int CMacro::Find(const TCHAR *Name)
{
	int i;

	for (i=0;i<Num;i++) {
		if (strcmpi(Name,arMacro[i].Name)==0) {
			return i;
		}
	}
	return -1;
}

// 매크로 내용 대입. 대입전에 크기 점검
void CMacro::Set(int idx,const TCHAR *szCon)
{
	unsigned size;

	// 매크로 버퍼의 크기 점검
	size=lstrlen(szCon)+1;
	if (arMacro[idx].Content==NULL || _msize(arMacro[idx].Content) < size) {
		arMacro[idx].Content=(TCHAR *)realloc(arMacro[idx].Content,size);
	}

	lstrcpy(arMacro[idx].Content,szCon);
}

void CMacro::Add(const TCHAR *Name,const TCHAR *szCon)
{
	// 같을 때 재할당. 한번에 1이상 커지면 안됨. 새로 할당된 매크로는 크기 0으로 주어 다음에 바로 재할당되도록 함
	if (Size == Num) {
		Size++;
		arMacro=(SMacro *)realloc(arMacro,sizeof(SMacro)*Size);
		arMacro[Num].Content=NULL;
	}
	lstrcpy(arMacro[Num].Name,Name);
	Set(Num,szCon);
	Num++;
}

void CMacro::Delete(int idx)
{
	int i;

	free(arMacro[idx].Content);
	for (i=idx;i<Num-1;i++) {
		arMacro[i]=arMacro[i+1];
	}
	// 마지막 요소는 비워서 다음에 재할당되도록 한다.
	arMacro[Num-1].Content=NULL;
	Num--;
}

void CMacro::LoadFromReg(TCHAR *Key)
{
	int i,count;
	TCHAR *szMacro;
	TCHAR szKey[MAX_PATH];
	TCHAR szSub[12];
	TCHAR szName[32];

	lstrcpy(szKey,Key);
	lstrcat(szKey,"Macro");
	
	count=SHRegReadInt(SHCU,szKey,"nMacro",0);
	szMacro=(TCHAR *)malloc(16384);
	for (i=0;i<count;i++) {
		wsprintf(szSub,"Macro%d",i);
		SHRegReadString(SHCU,szKey,szSub,"",szMacro,16384);
		wsprintf(szSub,"Name%d",i);
		SHRegReadString(SHCU,szKey,szSub,"",szName,32);
		Add(szName,szMacro);
	}
	free(szMacro);
}

void CMacro::SaveToReg(TCHAR *Key)
{
	int i;
	TCHAR szKey[MAX_PATH];
	TCHAR szSub[12];

	lstrcpy(szKey,Key);
	lstrcat(szKey,"Macro");

	SHRegWriteInt(SHCU,szKey,"nMacro",Num);
	for (i=0;i<Num;i++) {
		wsprintf(szSub,"Macro%d",i);
		SHRegWriteString(SHCU,szKey,szSub,arMacro[i].Content);
		wsprintf(szSub,"Name%d",i);
		SHRegWriteString(SHCU,szKey,szSub,arMacro[i].Name);
	}
}

void CMacro::LoadFromFile(TCHAR *Path)
{
}

void CMacro::SaveToFile(TCHAR *Path)
{
}

// 명령 실행.
// 0: 정상 실행
// -1:동사 없음
// -2:지원되지 않는 목적어
// -3:닫는 따옴표 없음
int DoCommand(const TCHAR *szCom)
{
	int comlen;
	const TCHAR *pc, *ps, *p;
	TCHAR *Com, *pCom ;
	int Verb, Object;
	HWND hActive;
	SInfo *pSi;
	int Result=0;
	TCHAR *str;
	int count,Value;
	int i;

	// 선행 공백은 무시한다.
	ps=szCom;
	SkipSpace(ps);

	// 주석이면 명령 전체를 무시한다.
	if (*ps=='/' && *(ps+1)=='/') {
		return 0;
	}

	// 명령어의 길이를 구한다. 널, 개행, 세미콜론이 문장의 끝이다.
	for (pc=ps;;pc++) {
		if (*pc==NULL || *pc=='\r' || *pc==';') {
			break;
		}
		if (SkipString(pc)==FALSE) {
			return -3;
		}
	}
	comlen=pc-ps;

	// szCom의 모든 문자는 소문자로, 단 따옴표 안은 제외. 닫는 따옴표가 있는지는 이미 체크했음
	Com=(TCHAR *)malloc(comlen+1);
	lstrcpyn(Com,ps,comlen+1);
	for (pCom=Com;*pCom;pCom++) {
		SkipString(const_cast<const TCHAR *&>(pCom));
		if (IsCharUpper(*pCom)) {
			*pCom+=0x20;
		}
	}

	hActive=(HWND)SendMessage(g_hMDIClient,WM_MDIGETACTIVE,0,(LPARAM)NULL);
	if (hActive) {
		pSi=(SInfo *)GetWindowLong(hActive,0);
	} else {
		return 0;
	}

	// 동사를 먼저 찾는다. 없으면 에러로 처리한다.
	p=Com;
	Verb=GetComVerb(p,arHostVerb);
	if (Verb==-1) {
		Result=-1;
		goto End;
	}

	switch (Verb) {
	case DGC_CALL:
		GetComString(p,str);
		count=GetComNumber(p,1);
		if (str) {
			for (i=0;i<count;i++) {
				Result=ExecMacro(str);
				if (Result != 0) {
					break;
				}
			}
			free(str);
		} else {
			Result=-3;
			goto End;
		}
		break;
	case DGC_CLIP:
		CClipRecord *cr;
		GetComString(p,str);
		count=GetComNumber(p,1);
		if (str) {
			cr=FindClipRecord(str,FALSE);
			if (cr) {
				for (i=0;i<count;i++) {
					pSi->Ae.GetActiveView()->InsertClipText(cr->Content);
				}
			}
			free(str);
		} else {
			Result=-3;
			goto End;
		}
		break;
	case DGC_NAVI:
		// 매크로 실행중에는 사용할 수 없다.
		if (pSi->Ae.GetActiveView()->AeMc.runlevel==0) {
			Object=GetComObject(Verb,p,arHostObject,arObjOfVerbHost);
			switch (Object) {
			case 0:
				SendMessage(g_hMDIClient,WM_MDINEXT,NULL,(LPARAM)TRUE);
				break;
			case 1:
				SendMessage(g_hMDIClient,WM_MDINEXT,NULL,(LPARAM)FALSE);
				break;
			}
		}
		break;
	case DGC_SET:
		Object=GetComObject(Verb,p,arHostObject,arObjOfVerbHost);
		Value=GetComNumber(p,0);
		switch (Object) {
		case 0:
			pSi->Ae.ChangeStyle(AES_SHOWENTER,Value!=0);
			break;
		case 1:
			pSi->Ae.ChangeStyle(AES_SHOWTAB,Value!=0);
			break;
		case 2:
			pSi->Ae.ChangeStyle(AES_SHOWSPACE,Value!=0);
			break;
		case 3:
			pSi->Ae.ChangeStyle(AES_SHOWLINENUM,Value!=0);
			break;
		case 4:
			pSi->Ae.ChangeStyle(AES_SHOWMARGIN,Value!=0);
			break;
		case 5:
			pSi->Ae.SetShowCurLine(Value);
			break;
		case 6:
			pSi->Ae.SetHideSelType(Value);
			break;
		}
		break;
	case DGC_TOGGLE:
		Object=GetComObject(Verb,p,arHostObject,arObjOfVerbHost);
		switch (Object) {
		case 0:
			pSi->Ae.ChangeStyle(AES_SHOWENTER,!pSi->Ae.TestStyle(AES_SHOWENTER));
			break;
		case 1:
			pSi->Ae.ChangeStyle(AES_SHOWTAB,!pSi->Ae.TestStyle(AES_SHOWTAB));
			break;
		case 2:
			pSi->Ae.ChangeStyle(AES_SHOWSPACE,!pSi->Ae.TestStyle(AES_SHOWSPACE));
			break;
		case 3:
			pSi->Ae.ChangeStyle(AES_SHOWLINENUM,!pSi->Ae.TestStyle(AES_SHOWLINENUM));
			break;
		case 4:
			pSi->Ae.ChangeStyle(AES_SHOWMARGIN,!pSi->Ae.TestStyle(AES_SHOWMARGIN));
			break;
		}
		break;
	}

End:
	free(Com);
	return Result;
}

// 매크로 실행.
int ExecMacro(const TCHAR *szMacro)
{
	int idx;
	HWND hActive;
	SInfo *pSi;

	idx=DgMacro.Find(szMacro);
	if (idx==-1) {
		return -1;
	}

	hActive=(HWND)SendMessage(g_hMDIClient,WM_MDIGETACTIVE,0,(LPARAM)NULL);
	if (hActive==NULL) {
		return -1;
	}
	pSi=(SInfo *)GetWindowLong(hActive,0);
	pSi->Ae.ExecMacro(DgMacro.arMacro[idx].Content);
	return 0;
}

BOOL CALLBACK MacroDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	static HWND hRecCom;
	static CApiEdit *pAe;
	static CApiEditText *pView,*pAct;
	static int oldidx=-1;
	static HWND hActive;
	SInfo *pSi;
	LVCOLUMN COL;
	int i,count,idx,len;
	LVITEM LI;
	TCHAR szName[65];
	RECT brt;
	BOOL bEnable;
	TCHAR *szMacro;
	const TCHAR *p;
	int Verb,Object;
	TCHAR szDesc[128];

	switch(iMessage) {
	case WM_INITDIALOG:
		MoveToParentCenter(hDlg);
		hRecCom=GetDlgItem(hDlg,IDC_RECENTCOM);
		COL.mask=LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		COL.fmt=LVCFMT_LEFT;
		COL.cx=40;
		COL.pszText="순서";
		COL.iSubItem=0;
		ListView_InsertColumn(hRecCom,0,&COL);

		COL.cx=150;
		COL.pszText="명령";
		COL.iSubItem=1;
		ListView_InsertColumn(hRecCom,1,&COL);

		COL.cx=100;
		COL.pszText="설명";
		COL.iSubItem=2;
		ListView_InsertColumn(hRecCom,2,&COL);
		ListView_SetExtendedListViewStyle(hRecCom,LVS_EX_FULLROWSELECT);

		SetDlgItemInt(hDlg,IDC_MACROCOUNT,1,FALSE);
		SendDlgItemMessage(hDlg,IDC_SPINMACROCOUNT,UDM_SETRANGE,0,MAKELPARAM(1000000,1));

		// 버튼 자리에 ApiEdit를 배치한다.대부분의 설정은 기본 설정대로, 몇가지만 고정 설정 사용
		GetWindowRect(GetDlgItem(hDlg,IDC_PLACEAPIEDIT),&brt);
		ScreenToClient(hDlg,(LPPOINT)&brt);
		ScreenToClient(hDlg,(LPPOINT)&brt.right);
		pAe=new CApiEdit;
		pAe->Create(brt.left,brt.top,brt.right-brt.left,brt.bottom-brt.top,
			WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP,0,hDlg);
		SetSetting(*pAe);
//		pAe->SetShowState(SHOWMARGIN,TRUE);
//		pAe->SetShowState(SHOWLINENUM,TRUE);
		pAe->ChangeStyle(AES_SHOWMARGIN,TRUE);
		pAe->ChangeStyle(AES_SHOWLINENUM,TRUE);
		pAe->SetWrap(0);
//		pAe->SetWantTab(FALSE);
		pAe->ChangeStyle(AES_WANTTAB,FALSE);
		pAct=pAe->GetActiveView();
		EnableWindow(pAe->hWnd,FALSE);

		// 최근 명령을 읽어온다. 활성창이 없으면 말고
		hActive=(HWND)SendMessage(g_hMDIClient,WM_MDIGETACTIVE,0,(LPARAM)NULL);
		if (hActive) {
			pSi=(SInfo *)GetWindowLong(hActive,0);
			pView=pSi->Ae.GetActiveView();
			for (i=0;i<pView->AeMc.nowmc;i++) {
				LI.mask=LVIF_TEXT;
				LI.iSubItem=0;
				LI.iItem=i;
				itoa(i,szName,10);
				LI.pszText=szName;
				ListView_InsertItem(hRecCom,&LI);

				p=pView->AeMc.arMc[i];
				ListView_SetItemText(hRecCom,i,1,(LPSTR)p);

				Verb=GetComVerb(p,arComVerb);
				if (Verb==AEC_MOVE || Verb==AEC_MOVESEL || Verb==AEC_CONVERT) {
					Object=GetComObject(Verb,p,arComObject,arObjOfVerb);
					wsprintf(szDesc,"%s(%s)",arComVerb[Verb].Desc,
						arComObject[arObjOfVerb[Verb]+Object].Desc);
				} else {
					lstrcpy(szDesc,arComVerb[Verb].Desc);
				}
				ListView_SetItemText(hRecCom,i,2,szDesc);
			}
			ListView_EnsureVisible(hList,i,FALSE);
		}

		// 매크로 목록을 읽어온다. 첫번째 매크로를 선택해 준다.
		for (i=0;i<DgMacro.Num;i++) {
			SendDlgItemMessage(hDlg,IDC_CBMACRO,CB_ADDSTRING,0,(LPARAM)DgMacro.arMacro[i].Name);
		}
		if (oldidx==-1) {
			oldidx=SendDlgItemMessage(hDlg,IDC_CBMACRO,CB_SETCURSEL,0,0);
		} else {
			oldidx=SendDlgItemMessage(hDlg,IDC_CBMACRO,CB_SETCURSEL,oldidx,0);
		}
		SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_CBMACRO,CBN_SELCHANGE),0);
		SendMessage(hDlg,WM_USER+1,0,0);
		return TRUE;
	// 매크로 개수에 따라 컨트롤 사용 여부 조정
	case WM_USER+1:
		if (SendDlgItemMessage(hDlg,IDC_CBMACRO,CB_GETCOUNT,0,0) == 0) {
			bEnable=FALSE;
		} else {
			bEnable=TRUE;
		}
		EnableWindow(pAe->hWnd,bEnable);
		EnableWindow(GetDlgItem(hDlg,IDC_BTNEDITMACRO),bEnable);
		EnableWindow(GetDlgItem(hDlg,IDC_BTNDELETEMACRO),bEnable);
		EnableWindow(GetDlgItem(hDlg,IDC_BTNMAKECOMMAND),bEnable);
		if (hActive==NULL) {
			bEnable=FALSE;
		}
		EnableWindow(GetDlgItem(hDlg,IDC_BTNRUNMACRO),bEnable);
		EnableWindow(GetDlgItem(hDlg,IDC_MACROCOUNT),bEnable);
		EnableWindow(GetDlgItem(hDlg,IDC_SPINMACROCOUNT),bEnable);
		EnableWindow(GetDlgItem(hDlg,IDC_BTNCOPYRECENT),bEnable);
		return TRUE;
	// 현재 편집중인 매크로를 wParam번에 저장한다.
	case WM_USER+2:
		len=pAe->GetTextLength();
		szMacro=(TCHAR *)malloc(len+2);
		pAe->GetText(szMacro,len+2);
		szMacro[len]=0;
		pAe->SetModified(FALSE);
		DgMacro.Set(wParam,szMacro);
		free(szMacro);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_CBMACRO:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				// 이전 매크로가 변경되었으면 먼저 저장한다.
				if (oldidx!=-1 && pAe->GetModified()) {
					SendMessage(hDlg,WM_USER+2,oldidx,0);
				}
				idx=SendDlgItemMessage(hDlg,IDC_CBMACRO,CB_GETCURSEL,0,0);
				if (idx != -1) {
					pAe->SetText(DgMacro.arMacro[idx].Content,lstrlen(DgMacro.arMacro[idx].Content));
					pAe->SetModified(FALSE);
					oldidx=idx;
				} else {
					pAe->InitDoc();
				}
				break;
			}
			return TRUE;
		case IDC_BTNNEWMACRO:
			// 이전 매크로가 변경되었으면 먼저 저장한다.
			if (oldidx!=-1 && pAe->GetModified()) {
				SendMessage(hDlg,WM_USER+2,oldidx,0);
			}
			lstrcpy(szName,"");
			if (DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_EDITMACRO),hDlg,
				MacroEditDlgProc,(LPARAM)szName)==IDCANCEL) {
				return TRUE;
			}

			if (DgMacro.Find(szName) != -1) {
				MessageBox(hDlg,"이미 등록된 이름입니다. 다른 이름을 선택해 주십시오","알림",MB_OK);
				return TRUE;
			}

			idx=SendDlgItemMessage(hDlg,IDC_CBMACRO,CB_ADDSTRING,0,(LPARAM)szName);
			oldidx=idx;
			SendDlgItemMessage(hDlg,IDC_CBMACRO,CB_SETCURSEL,idx,0);
			pAe->InitDoc();
			SendMessage(hDlg,WM_USER+1,0,0);
			// 새로 만든 매크로 일단 추가. 그래야 메모리가 할당되고 SelChange가 발생해도 에러 안남
			DgMacro.Add(szName,"");
			return TRUE;
		case IDC_BTNEDITMACRO:
			idx=SendDlgItemMessage(hDlg,IDC_CBMACRO,CB_GETCURSEL,0,0);
			lstrcpy(szName,DgMacro.arMacro[idx].Name);
			if (DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_EDITMACRO),hDlg,
				MacroEditDlgProc,(LPARAM)szName)==IDCANCEL) {
				return TRUE;
			}

			SendDlgItemMessage(hDlg,IDC_CBMACRO,CB_DELETESTRING,idx,0);
			SendDlgItemMessage(hDlg,IDC_CBMACRO,CB_INSERTSTRING,idx,(LPARAM)szName);
			SendDlgItemMessage(hDlg,IDC_CBMACRO,CB_SETCURSEL,idx,0);
			lstrcpy(DgMacro.arMacro[idx].Name,szName);
			return TRUE;
		case IDC_BTNDELETEMACRO:
			idx=SendDlgItemMessage(hDlg,IDC_CBMACRO,CB_GETCURSEL,0,0);
			DgMacro.Delete(idx);
			SendDlgItemMessage(hDlg,IDC_CBMACRO,CB_DELETESTRING,idx,0);
			if (SendDlgItemMessage(hDlg,IDC_CBMACRO,CB_GETCOUNT,0,0) == 0) {
				SendMessage(hDlg,WM_USER+1,0,0);
				idx=-1;
			} else {
				idx=0;
			}
			SendDlgItemMessage(hDlg,IDC_CBMACRO,CB_SETCURSEL,idx,0);
			oldidx=-1;
			SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_CBMACRO,CBN_SELCHANGE),0);
			return TRUE;
		case IDC_BTNMAKECOMMAND:
			szMacro=(TCHAR *)malloc(256);
			if (DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_ADDCOMMAND),hDlg,
				AddCommandDlgProc,(LPARAM)szMacro)==IDOK) {
				pAct->SetOffset(pAe->FindParaStart(pAct->GetOffset()));
				pAct->InsertText(szMacro);
				pAct->InsertText("\r\n");
			}
			free(szMacro);
			return TRUE;
		case IDC_BTNCOPYRECENT:
			count=ListView_GetItemCount(hRecCom);
			for (i=0;i<count;i++) {
				if (ListView_GetItemState(hRecCom,i,LVIS_SELECTED)==LVIS_SELECTED) {
					pAct->SetOffset(pAe->FindParaStart(pAct->GetOffset()));
					pAct->InsertText(pView->AeMc.arMc[i]);
					pAct->InsertText("\r\n");
				}
			}
			return TRUE;
		case IDC_BTNRUNMACRO:
			idx=SendDlgItemMessage(hDlg,IDC_CBMACRO,CB_GETCURSEL,0,0);
			if (pAe->GetModified()) {
				SendMessage(hDlg,WM_USER+2,idx,0);
			}
			count=GetDlgItemInt(hDlg,IDC_MACROCOUNT,NULL,FALSE);
			PostMessage(g_hFrameWnd,WM_USER+4,count,(LPARAM)DgMacro.arMacro[idx].Name);
			// no break.
		case IDCANCEL:
			idx=SendDlgItemMessage(hDlg,IDC_CBMACRO,CB_GETCURSEL,0,0);
			if (pAe->GetModified()) {
				if (MessageBox(hDlg,"수정된 매크로를 저장하시겠습니까?","질문",MB_YESNO) == IDYES) {
					SendMessage(hDlg,WM_USER+2,idx,0);
				}
			}
			DestroyWindow(pAe->hWnd);
			delete pAe;
			EndDialog(hDlg,IDCANCEL);
			oldidx=idx;
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CALLBACK MacroEditDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	static TCHAR *pName;
	int i;
	TCHAR NewMacro[256];
	TCHAR szCom[1024];
	TCHAR *Com;
	TCHAR Mes[2048];
	static int objidx;
	int oriidx;
	BOOL bEnable,bShortCut;
	SShortCut sc;

	switch(iMessage) {
	case WM_INITDIALOG:
		MoveToParentCenter(hDlg);
		SendDlgItemMessage(hDlg,IDC_MACRONAME,EM_LIMITTEXT,64,0);
		pName=(TCHAR *)lParam;
		for (i=0;i<59/*ARSIZE(arVirtKey)*/;i++) {
			SendDlgItemMessage(hDlg,IDC_CBKEY,CB_ADDSTRING,0,(LPARAM)arVirtKey[i].Name);
		}
		if (lstrlen(pName)==0) {
			SetWindowText(hDlg,"새 매크로");
			for (i=1;;i++) {
				wsprintf(NewMacro,"매크로%d",i);
				if (DgMacro.Find(NewMacro)==-1) {
					break;
				}
			}
			SetDlgItemText(hDlg,IDC_MACRONAME,NewMacro);
			CheckDlgButton(hDlg,IDC_CHKMACROSHORTCUT,BST_UNCHECKED);
		} else {
			SetWindowText(hDlg,"매크로 이름 변경");
			SetDlgItemText(hDlg,IDC_MACRONAME,pName);
			// 지정된 단축키가 있는지 조사해 본다.
			wsprintf(szCom,"call \"%s\"",pName);
			objidx=DgShortCut.Find(0,szCom);
			if (objidx==-1) {
				CheckDlgButton(hDlg,IDC_CHKMACROSHORTCUT,BST_UNCHECKED);
			} else {
				CheckDlgButton(hDlg,IDC_CHKMACROSHORTCUT,BST_CHECKED);
				SetDlgShortCut(hDlg,objidx);
			}
		}
		SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_CHKMACROSHORTCUT,0),0);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_CHKMACROSHORTCUT:
			if (IsDlgButtonChecked(hDlg,IDC_CHKMACROSHORTCUT)) {
				bEnable=TRUE;
			} else {
				bEnable=FALSE;
			}
			EnableWindow(GetDlgItem(hDlg,IDC_CBKEY),bEnable);
			EnableWindow(GetDlgItem(hDlg,IDC_CHKCTRL),bEnable);
			EnableWindow(GetDlgItem(hDlg,IDC_CHKALT),bEnable);
			EnableWindow(GetDlgItem(hDlg,IDC_CHKSHIFT),bEnable);
			return TRUE;
		case IDOK:
			GetDlgItemText(hDlg,IDC_MACRONAME,pName,64);
			StringNormalize(pName,NULL);
			if (IsStrEmpty(pName)) {
				MessageBox(hDlg,"매크로 이름을 지정해 주십시오","알림",MB_OK);
				return TRUE;
			}
			if (strchr(pName,'\"') || strchr(pName,'\\')) {
				MessageBox(hDlg,"매크로 이름에 \"와 \\는 쓸 수 없습니다.","알림",MB_OK);
				return TRUE;
			}

			// 단축키가 지정되었는지 조사
			if (IsDlgButtonChecked(hDlg,IDC_CHKMACROSHORTCUT)) {
				bShortCut=TRUE;
			} else {
				bShortCut=FALSE;
			}

			// 일단 작성한 단축키 정보를 조사해 놓는다.
			if (bShortCut) {
				i=SendDlgItemMessage(hDlg,IDC_CBKEY,CB_GETCURSEL,0,0);
				if (i==-1) {
					MessageBox(hDlg,"키를 선택해 주십시오","알림",MB_OK);
					return TRUE;
				}
				sc.vk=arVirtKey[i].vk;
				sc.Flag=0;

				if (IsDlgButtonChecked(hDlg,IDC_CHKCTRL))
					sc.Flag |= SC_CTRL;
				if (IsDlgButtonChecked(hDlg,IDC_CHKALT))
					sc.Flag |= SC_ALT;
				if (IsDlgButtonChecked(hDlg,IDC_CHKSHIFT))
					sc.Flag |= SC_SHIFT;

				sc.id=0;
				wsprintf(szCom,"call \"%s\"",pName);
				sc.Com=szCom;
			}

			// 지금 지정한 단축키가 중복된 것인지 보고 경고 문구를 미리 만들어 놓는다.
			oriidx=GetDlgShortCut(hDlg);
			if (oriidx != -1) {
				if (DgShortCut.arShortCut[oriidx].id != 0) {
					Com=arMenuCom[GetMenuComIndex(DgShortCut.arShortCut[oriidx].id)].Name;
				} else {
					Com=DgShortCut.arShortCut[oriidx].Com;
				}
				wsprintf(Mes,"이 단축키는 이미 [%s] 명령으로 사용하고 있습니다. "
					"기존 단축키를 삭제하고 새로 만드시겠습니까?",Com);
			}

			if (objidx==-1) {
				// 원래 없었는데 새로 지정된 경우
				if (bShortCut) {
					// 기존 단축키와 중복된 경우는 수정, 아니면 추가
					if (oriidx!=-1) {
						if (MessageBox(hDlg,Mes,"질문",MB_YESNO)==IDYES) {
							DgShortCut.Modify(oriidx,&sc);
						} else {
							return TRUE;
						}
					} else {
						DgShortCut.Add(&sc);
					}
				}
			} else {
				// 원래 있었는데 삭제된 경우
				if (!bShortCut) {
					DgShortCut.Delete(objidx);
				} else {
					// 수정된 경우
					if (oriidx!=-1) {
						if (MessageBox(hDlg,Mes,"질문",MB_YESNO)==IDYES) {
							DgShortCut.Modify(oriidx,&sc);
						} else {
							return TRUE;
						}
					// 새로 추가된 경우
					} else {
						DgShortCut.Add(&sc);
					}
				}
			}
			EndDialog(hDlg,IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg,IDCANCEL);
			return TRUE;
		// 단축키 지정 컨트롤이 변경되면 스태틱에 이 키의 현재 상태를 출력한다.
		case IDC_CBKEY:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				DisplayShortCutStatus(hDlg);
			}
			return TRUE;
		case IDC_CHKCTRL:
		case IDC_CHKALT:
		case IDC_CHKSHIFT:
			DisplayShortCutStatus(hDlg);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

// 작성중인 명령을 명령 에디트 박스에 보여준다.
void DisplayMakingCommand(HWND hDlg)
{
	int Verb, Object=0;
	TCHAR szCommand[256];
	TCHAR szTemp[256];
	int Count;
	SMacVerb *arVerb;
	SMacObject *arObject;
	int *arObj;

	// 일단 동사부터
	Verb=SendDlgItemMessage(hDlg,IDC_LISTVERB,LB_GETCURSEL,0,0);
	if (Verb==-1) {
		return;
	}
	// 컨트롤의 명령인지, 호스트의 명령인지 구분한다.
	if (Verb >= AEVERBNUM) {
		arVerb=arHostVerb;
		arObject=arHostObject;
		arObj=arObjOfVerbHost;
		Verb-=AEVERBNUM;
	} else {
		arVerb=arComVerb;
		arObject=arComObject;
		arObj=arObjOfVerb;
	}
	lstrcpy(szCommand,arVerb[Verb].Name);

	// 목적어가 있으면 목적어를 기록한다. 
	if (arObj[Verb]!=-1) {
		Object=SendDlgItemMessage(hDlg,IDC_LISTOBJECT,LB_GETCURSEL,0,0);
		if (Object!=-1) {
			wsprintf(szTemp," %s",arObject[arObj[Verb]+Object].Name);
			lstrcat(szCommand,szTemp);
		}
	}

	// 문자열 인수 처리
	if (arVerb==arComVerb) {
		if (Verb==AEC_INSERT || Verb==AEC_FIND || Verb==AEC_REPLACE) {
			GetDlgItemText(hDlg,IDC_EDSTRING1,szTemp,256);
			lstrcat(szCommand," \"");
			lstrcat(szCommand,szTemp);
			lstrcat(szCommand,"\"");
			if (Verb==AEC_REPLACE) {
				GetDlgItemText(hDlg,IDC_EDSTRING2,szTemp,256);
				lstrcat(szCommand," \"");
				lstrcat(szCommand,szTemp);
				lstrcat(szCommand,"\"");
			}
		}
	} else {
		if (Verb==DGC_CALL || Verb==DGC_CLIP) {
			GetDlgItemText(hDlg,IDC_EDSTRING1,szTemp,256);
			lstrcat(szCommand," \"");
			lstrcat(szCommand,szTemp);
			lstrcat(szCommand,"\"");
		}
	}

	if (arVerb==arComVerb) {
		if (Verb==AEC_FIND || Verb==AEC_REPLACE) {
			if (IsDlgButtonChecked(hDlg,IDC_FIND_UP)) lstrcat(szCommand," up");
			if (IsDlgButtonChecked(hDlg,IDC_FIND_WHOLEWORD)) lstrcat(szCommand," word");
			if (IsDlgButtonChecked(hDlg,IDC_FIND_MATCHCASE)) lstrcat(szCommand," case");
			if (IsDlgButtonChecked(hDlg,IDC_FIND_WRAP)) lstrcat(szCommand," wrap");
		}
	}

	// 반복 가능한 명령이면 반복 회수 기록. 1이상이고 목적어가 선택되어 있을 때만. move 5 따위의 문자열을 만들지 않음
	if (arVerb[Verb].Flag & 0x1) {
		Count=GetDlgItemInt(hDlg,IDC_EDCOUNT,NULL,FALSE);
		if (Count != 1 && Object!=-1) {
			wsprintf(szTemp, " %d",Count);
			lstrcat(szCommand,szTemp);
		}
	}

	SetDlgItemText(hDlg,IDC_EDCOMMAND,szCommand);
}

BOOL CALLBACK AddCommandDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	static TCHAR *szCommand;
	TCHAR szTemp[256];
	int i,idx;
	SMacVerb *arVerb;
	SMacObject *arObject;
	int *arObj;

	switch(iMessage) {
	case WM_INITDIALOG:
		MoveToParentCenter(hDlg);
		szCommand=(TCHAR *)lParam;
		for (i=0;i<AEVERBNUM;i++) {
			wsprintf(szTemp,"%s(%s)",arComVerb[i].Desc,arComVerb[i].Name);
			SendDlgItemMessage(hDlg,IDC_LISTVERB,LB_ADDSTRING,0,(LPARAM)szTemp);
		}
		// 호스트 수준의 명령도 포함시킨다.
		for (i=0;i<DGVERBNUM;i++) {
			wsprintf(szTemp,"%s(%s)",arHostVerb[i].Desc,arHostVerb[i].Name);
			SendDlgItemMessage(hDlg,IDC_LISTVERB,LB_ADDSTRING,0,(LPARAM)szTemp);
		}
		SetDlgItemInt(hDlg,IDC_EDCOUNT,1,FALSE);
		SendDlgItemMessage(hDlg,IDC_SPINCOUNT,UDM_SETRANGE,0,MAKELPARAM(1000000,1));
		SendDlgItemMessage(hDlg,IDC_EDCOMMAND,EM_LIMITTEXT,256,0);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			GetDlgItemText(hDlg,IDC_EDCOMMAND,szCommand,256);
			EndDialog(hDlg,IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg,IDCANCEL);
			return TRUE;
		case IDC_LISTVERB:
			switch (HIWORD(wParam)) {
			case LBN_SELCHANGE:
				SendDlgItemMessage(hDlg,IDC_LISTOBJECT,LB_RESETCONTENT,0,0);
				DisplayMakingCommand(hDlg);;
				// 일단 모두 숨긴다.
				ShowWindow(GetDlgItem(hDlg,IDC_LISTOBJECT),SW_HIDE);
				ShowWindow(GetDlgItem(hDlg,IDC_STOBJECT),SW_HIDE);
				ShowWindow(GetDlgItem(hDlg,IDC_EDSTRING1),SW_HIDE);
				ShowWindow(GetDlgItem(hDlg,IDC_EDSTRING2),SW_HIDE);
				ShowWindow(GetDlgItem(hDlg,IDC_STSTRING1),SW_HIDE);
				ShowWindow(GetDlgItem(hDlg,IDC_STSTRING2),SW_HIDE);
				ShowWindow(GetDlgItem(hDlg,IDC_EDCOUNT),SW_HIDE);
				ShowWindow(GetDlgItem(hDlg,IDC_STCOUNT),SW_HIDE);
				ShowWindow(GetDlgItem(hDlg,IDC_SPINCOUNT),SW_HIDE);
				ShowWindow(GetDlgItem(hDlg,IDC_FIND_UP),SW_HIDE);
				ShowWindow(GetDlgItem(hDlg,IDC_FIND_WHOLEWORD),SW_HIDE);
				ShowWindow(GetDlgItem(hDlg,IDC_FIND_MATCHCASE),SW_HIDE);
				ShowWindow(GetDlgItem(hDlg,IDC_FIND_WRAP),SW_HIDE);

				idx=SendDlgItemMessage(hDlg,IDC_LISTVERB,LB_GETCURSEL,0,0);
				// 컨트롤의 명령인지, 호스트의 명령인지 구분한다.
				if (idx >= AEVERBNUM) {
					arVerb=arHostVerb;
					arObject=arHostObject;
					arObj=arObjOfVerbHost;
					idx-=AEVERBNUM;
				} else {
					arVerb=arComVerb;
					arObject=arComObject;
					arObj=arObjOfVerb;
				}

				// 반복이 가능한 명령들
				if (arVerb[idx].Flag & 0x1) {
					ShowWindow(GetDlgItem(hDlg,IDC_EDCOUNT),SW_SHOW);
					ShowWindow(GetDlgItem(hDlg,IDC_STCOUNT),SW_SHOW);
					ShowWindow(GetDlgItem(hDlg,IDC_SPINCOUNT),SW_SHOW);
				}

				// 문자열 입력형은 특별하게 다룬다
				if (arVerb==arComVerb) {
					if (idx==AEC_INSERT) {
						ShowWindow(GetDlgItem(hDlg,IDC_EDSTRING1),SW_SHOW);
						ShowWindow(GetDlgItem(hDlg,IDC_STSTRING1),SW_SHOW);
						SetWindowText(GetDlgItem(hDlg,IDC_STSTRING1),"입력 내용(&I)");
						return TRUE;
					}
					if (idx==AEC_FIND || idx==AEC_REPLACE) {
						ShowWindow(GetDlgItem(hDlg,IDC_EDSTRING1),SW_SHOW);
						ShowWindow(GetDlgItem(hDlg,IDC_STSTRING1),SW_SHOW);
						SetWindowText(GetDlgItem(hDlg,IDC_STSTRING1),"찾을 내용(&F)");
						ShowWindow(GetDlgItem(hDlg,IDC_FIND_UP),SW_SHOW);
						ShowWindow(GetDlgItem(hDlg,IDC_FIND_WHOLEWORD),SW_SHOW);
						ShowWindow(GetDlgItem(hDlg,IDC_FIND_MATCHCASE),SW_SHOW);
						ShowWindow(GetDlgItem(hDlg,IDC_FIND_WRAP),SW_SHOW);
						if (idx==AEC_REPLACE) {
							ShowWindow(GetDlgItem(hDlg,IDC_EDSTRING2),SW_SHOW);
							ShowWindow(GetDlgItem(hDlg,IDC_STSTRING2),SW_SHOW);
							SetWindowText(GetDlgItem(hDlg,IDC_STSTRING2),"바꿀 내용(&T)");
						}
						return TRUE;
					}
				} else {
					if (idx==DGC_CALL || idx==DGC_CLIP) {
						ShowWindow(GetDlgItem(hDlg,IDC_EDSTRING1),SW_SHOW);
						ShowWindow(GetDlgItem(hDlg,IDC_STSTRING1),SW_SHOW);
						if (idx==DGC_CALL) {
							SetWindowText(GetDlgItem(hDlg,IDC_STSTRING1),"매크로 이름(&I)");
						} else {
							SetWindowText(GetDlgItem(hDlg,IDC_STSTRING1),"상용구 이름(&I)");
						}
						return TRUE;
					}
				}

				// 목적어가 없는 것들(insert,find, replace는 제외)
				if ((arVerb[idx].Flag & 0x2)==0) {
					return TRUE;
				} else {
					ShowWindow(GetDlgItem(hDlg,IDC_LISTOBJECT),SW_SHOW);
					ShowWindow(GetDlgItem(hDlg,IDC_STOBJECT),SW_SHOW);
					for (i=arObj[idx];arObject[i].Name!=NULL;i++) {
						wsprintf(szTemp,"%s(%s)",arObject[i].Desc,arObject[i].Name);
							SendDlgItemMessage(hDlg,IDC_LISTOBJECT,LB_ADDSTRING,0,(LPARAM)szTemp);
					}
					return TRUE;
				}
				break;
			}
			break;
		case IDC_LISTOBJECT:
			switch (HIWORD(wParam)) {
			case LBN_SELCHANGE:
				DisplayMakingCommand(hDlg);
				break;
			}
			break;
		case IDC_FIND_UP:
		case IDC_FIND_WHOLEWORD:
		case IDC_FIND_MATCHCASE:
		case IDC_FIND_WRAP:
			DisplayMakingCommand(hDlg);
			break;
		case IDC_EDSTRING1:
		case IDC_EDSTRING2:
		case IDC_EDCOUNT:
			switch (HIWORD(wParam)) {
			case EN_CHANGE:
				DisplayMakingCommand(hDlg);
				break;
			}
			break;
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CALLBACK RunMacroDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	TCHAR str[256];

	switch(iMessage) {
	case WM_INITDIALOG:
		MoveToParentCenter(hDlg);
		wsprintf(str,"매크로 이름 : %s", (TCHAR *)lParam);
		SetDlgItemText(hDlg,IDC_MACRONAME,str);
		return TRUE;
	case WM_USER+1:
		wsprintf(str,"총 %d 회 중 %d 번째 실행",wParam,lParam);
		SetDlgItemText(hDlg,IDC_MACRONUM,str);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDCANCEL:
			DestroyWindow(hDlg);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

