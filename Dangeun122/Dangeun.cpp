#include "stdafx.h"

static LPSTR lpszClass="Dangeun";
static HMENU hMenu1,hMenu2;
static int iDrag;
static int DragOriginal;
static BOOL NewPhase;
static FileInfo *pEditings;
static HWND hEdRecDlg;
static HWND hInstEdit;
static WNDPROC OldInstEditProc;

HINSTANCE g_hInst;
HWND g_hFrameWnd;
HWND g_hMDIClient;
int g_NewNo=0;
int g_ChildNum=0;
SOption Option, NewOption;
LOGFONT *arFont;
int FontNum;
int g_StartPage=0;
CMru Mru;
BOOL bUninstall=FALSE;
HWND g_FindDlg;
CHistory arFind[4];
DWORD FindFlag;
HWND hOutput;
HWND hList;
int TotalFind;
TCHAR LastFIF[256];
HANDLE hFIFThread;
HWND hToolBar;
HWND hStatus;
HWND hFileWnd;
HWND hTabFrame;
HWND hFileTab;
HCURSOR hCurHorz, hCurVert;
HWND hFileTree, hFilter;
HIMAGELIST hFileImg;
HIMAGELIST hTabImg;
HFONT hGul9;
BOOL bContDown;
HINTERNET hInternet, hFtp;
BOOL bPrinting;
HWND hDlgCancel;
BOOL bShowOutput;
HHOOK hCbtHook;
CMacro DgMacro;
CShortCut DgShortCut;
BOOL bComMode;
HWND hClipWnd;
HWND hClipTree;
HBITMAP hBackBit;
HWND hParaTab;
SParaOption ParaOption;

LOGFONT arCustomFont[10]={
	{0,0,0,0,0,0,0,0,0,0,0,0,0,""},
	{10,0,0,0,0,0,0,0,HANGEUL_CHARSET,3,2,1,VARIABLE_PITCH | FF_MODERN,"����"},
	{12,0,0,0,0,0,0,0,HANGEUL_CHARSET,3,2,1,VARIABLE_PITCH | FF_MODERN,"����"},
	{10,0,0,0,0,0,0,0,HANGEUL_CHARSET,3,2,1,VARIABLE_PITCH | FF_MODERN,"����ü"},
	{12,0,0,0,0,0,0,0,HANGEUL_CHARSET,3,2,1,VARIABLE_PITCH | FF_MODERN,"����ü"},
	{12,0,0,0,0,0,0,0,HANGEUL_CHARSET,3,2,1,VARIABLE_PITCH | FF_MODERN,"�ü�"},
	{15,0,0,0,0,0,0,0,HANGEUL_CHARSET,3,2,1,VARIABLE_PITCH | FF_MODERN,"�ü�"},
	{20,0,0,0,0,0,0,0,HANGEUL_CHARSET,3,2,1,VARIABLE_PITCH | FF_MODERN,"�ü�"},
	{12,0,0,0,0,0,0,0,HANGEUL_CHARSET,3,2,1,VARIABLE_PITCH | FF_MODERN,"����"},
	{15,0,0,0,0,0,0,0,HANGEUL_CHARSET,3,2,1,VARIABLE_PITCH | FF_MODERN,"����"}
};

TCHAR arGroupDesc[50][32]={"",
	"���� ��ü",
	"���� ��ü �ٿ� �ֱ�",
	"��ҹ��� ����",
	"���� ���� ����",
	"��� �鿩����",				// 5
	"��� �ٲٱ�",
	"���� ����Ʈ ����",
	"�ڵ� �鿩����",
	"���� �鿩����",
	"���� �����",				// 10
	"�ٲٱ�",
	"���� ��������",
	"������ ������",
	"�ּ�����",
	"�ּ� ����",					// 15
	"�� ����",
	"�� �ٿ��ֱ�",
	"�� ����",
	"�� �̵�",
	"�� ��ҹ��� ����",				// 20
	"��ũ�� ����",
	"��뱸 ����",
	"�ڵ� ��뱸 ����",
	"����-����Ʈ ����",
	"�ٿ� �ֱ�",					// 25
	"����",
	"�̵�",
	"���� ����",
	"���� ����",
	"���� ���� ����",				// 30
	"���� ũ��� ����",
	"���� ���� ����",
};

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance
		  ,LPSTR lpszCmdParam,int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClassEx;
	HANDLE hMutex;
	g_hInst=hInstance;
	BOOL AllowIdle=TRUE;

	Option.bAllowMulti=SHRegReadInt(SHCU,KEY"Setting","bAllowMulti",0);
	if (Option.bAllowMulti==FALSE) {
#ifdef _DEBUG
		hMutex=CreateMutex(NULL,FALSE,"Dangeun120_Mutex_For_Debug");
#else
		hMutex=CreateMutex(NULL,FALSE,"Dangeun120_Mutex_For_Allow_Unique_Instance");
#endif
		if (GetLastError() == ERROR_ALREADY_EXISTS) {
			CloseHandle(hMutex);
			ActivateBrood();
			return 0;
		}
	}

	CoInitialize(NULL);
	hMenu1=LoadMenu(g_hInst,MAKEINTRESOURCE(IDR_MENU1));
	hMenu2=LoadMenu(g_hInst,MAKEINTRESOURCE(IDR_MENU2));

	WndClassEx.cbSize=sizeof(WNDCLASSEX);
	WndClassEx.cbClsExtra=0;
	WndClassEx.cbWndExtra=0;
	WndClassEx.hbrBackground=(HBRUSH)(COLOR_BTNFACE+1);
	WndClassEx.hCursor=LoadCursor(NULL,IDC_ARROW);
	WndClassEx.hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDI_DANGEUN));
	WndClassEx.hIconSm=(HICON)LoadImage(hInstance,MAKEINTRESOURCE(IDI_DANGEUN),
		IMAGE_ICON,GetSystemMetrics(SM_CXSMICON),
		GetSystemMetrics(SM_CXSMICON),LR_DEFAULTCOLOR);
	WndClassEx.hInstance=hInstance;
	WndClassEx.lpfnWndProc=(WNDPROC)DGWndProc;
	WndClassEx.lpszClassName=lpszClass;
	WndClassEx.lpszMenuName=MAKEINTRESOURCE(IDR_MENU2);
	WndClassEx.style=0;
	RegisterClassEx(&WndClassEx);

	WndClassEx.lpszClassName="DGChild";
	WndClassEx.lpfnWndProc=(WNDPROC)DGChildProc;
	WndClassEx.hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDI_CHILD));
	WndClassEx.hIconSm=(HICON)LoadImage(hInstance,MAKEINTRESOURCE(IDI_CHILD),
		IMAGE_ICON,GetSystemMetrics(SM_CXSMICON),
		GetSystemMetrics(SM_CXSMICON),LR_DEFAULTCOLOR);
	WndClassEx.lpszMenuName=NULL;
	WndClassEx.cbWndExtra=sizeof(LONG_PTR);
	RegisterClassEx(&WndClassEx);

	WndClassEx.lpszClassName="DGOutput";
	WndClassEx.lpfnWndProc=(WNDPROC)DGOutputProc;
	WndClassEx.hIcon=NULL;
	WndClassEx.hIconSm=NULL;
	WndClassEx.cbWndExtra=0;
	RegisterClassEx(&WndClassEx);

	WndClassEx.lpszClassName="DGFile";
	WndClassEx.lpfnWndProc=(WNDPROC)DGFileProc;
	RegisterClassEx(&WndClassEx);

	WndClassEx.lpszClassName="DGTabFrame";
	WndClassEx.lpfnWndProc=(WNDPROC)DGTabProc;
	RegisterClassEx(&WndClassEx);

	WndClassEx.lpszClassName="DGClipFrame";
	WndClassEx.lpfnWndProc=(WNDPROC)DGClipProc;
	RegisterClassEx(&WndClassEx);

	WndClassEx.lpszClassName="DGStringGrid";
	WndClassEx.lpfnWndProc=(WNDPROC)DGStringGridProc;
	WndClassEx.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
	WndClassEx.style=CS_DBLCLKS;
	RegisterClassEx(&WndClassEx);

	if (SHRegReadInt(SHCU,KEY"Setting","StartAction",1000)==1000) {
		if (DialogBox(g_hInst, MAKEINTRESOURCE(IDD_SETUP), 
			HWND_DESKTOP, SetupDlgProc) == IDCANCEL) {
			CloseHandle(hMutex);
			return 0;
		}
	}
//////////////////////////////////////���⼭ Ÿ��Ʋ�̸� ����/////////////////////////////////////////
	hWnd=CreateWindow(lpszClass,"���V122",WS_OVERLAPPEDWINDOW | WS_VISIBLE | 
		WS_CLIPCHILDREN,
		CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
		NULL,(HMENU)NULL,hInstance,NULL);

	SYSTEMTIME st;
	DWORD Day,Today;;
	GetLocalTime(&st);
	Today=MyGetAbsDay(st);
	Day=SHRegReadInt(SHCU,"Software\\Miyoungsoft","DgInstDay120",0);
	MyAbsToSystem(Day,st);
	if (Today-Day >= 365 || Day > Today) {
		if (DialogBox(g_hInst,MAKEINTRESOURCE(IDD_SHARENOTICE),hWnd,
			ShareNoticeProc)==IDCANCEL) {
			DestroyWindow(hWnd);
		}
	}

	for (;;) {
		do {
			if (!GetMessage(&Message,NULL,0,0))
				goto endloop;
			if (Message.message != 0x118/*WM_SYSTIMER*/) {
				AllowIdle=TRUE;
			}
			if ((!IsWindow(g_FindDlg) || !IsDialogMessage(g_FindDlg,&Message)) &&
				(!IsWindow(hEdRecDlg) || !IsDialogMessage(hEdRecDlg,&Message)) ) {
				if (!TranslateMDISysAccel(g_hMDIClient, &Message)) {
					if (!TestShortCut(hWnd,&Message)) {
						TranslateMessage(&Message);
						DispatchMessage(&Message);
					}
				}
			}
		} while (PeekMessage(&Message,NULL,0,0,PM_NOREMOVE));

		if (AllowIdle) {
			OnIdle();
			AllowIdle=FALSE;
		}
	}

endloop:
	if (hMutex) {
		CloseHandle(hMutex);
	}
	CoUninitialize();
	if (bUninstall) {
		Uninstall();
	}
	return (int)Message.wParam;
}

LRESULT CALLBACK DGWndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage) {
		case WM_CREATE:return OnCreate(hWnd,wParam,lParam);
		case WM_DESTROY:OnDestroy(hWnd,wParam,lParam);return 0;
		case WM_COMMAND:OnCommand(hWnd,wParam,lParam);break;
		case WM_TIMER:OnTimer(hWnd,wParam,lParam);return 0;
		case WM_CLOSE:if (OnClose(hWnd,wParam,lParam)) return 0;break;
		case WM_QUERYENDSESSION:if (OnQueryEndSession(hWnd,wParam,lParam)) return 0;break;
		case WM_INITMENU:OnInitMenu(hWnd,wParam,lParam);return 0;
		case WM_DROPFILES:OnDropFiles(hWnd,wParam,lParam);return 0;
		case WM_FONTCHANGE:OnFontChange(hWnd,wParam,lParam);return 0;
		case WM_ACTIVATEAPP:OnActivateApp(hWnd,wParam,lParam);return 0;
		case WM_USER+1:OnUser1(hWnd,wParam,lParam);return 0;
		case WM_USER+2:OnUser2(hWnd,wParam,lParam);return 0;
		case WM_SIZE:if (OnSize(hWnd,wParam,lParam)) return 0;break;
		case WM_SETCURSOR:if (OnSetCursor(hWnd,wParam,lParam)) return 0;break;
		case WM_LBUTTONDOWN:OnLButtonDown(hWnd,wParam,lParam);return 0;
		case WM_MOUSEMOVE:OnMouseMove(hWnd,wParam,lParam);return 0;
		case WM_LBUTTONUP:OnLButtonUp(hWnd,wParam,lParam);return 0;
		case WM_NOTIFY:OnNotify(hWnd,wParam,lParam);return 0;
		case WM_SYSCOLORCHANGE:OnSysColorChange(hWnd,wParam,lParam);return 0;
		case WM_USER+3:OnUser3(hWnd,wParam,lParam);return 0;
		case WM_USER+4:OnUser4(hWnd,wParam,lParam);return 0;
	}
	return(DefFrameProc(hWnd,g_hMDIClient,iMessage,wParam,lParam));
}

int OnCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	CLIENTCREATESTRUCT ccs;
	INITCOMMONCONTROLSEX icex;
	int i,j;
	HDC hdc;
	TCHAR szTemp[256];
	TCHAR szKey[16];

	g_hFrameWnd=hWnd;
	icex.dwSize=sizeof(icex);
	icex.dwICC=ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icex);

	ccs.hWindowMenu=GetSubMenu(GetMenu(hWnd),5);
	ccs.idFirstChild=IDM_WINDOWCHILD;
	g_hMDIClient=CreateWindowEx(WS_EX_CLIENTEDGE,"MDICLIENT", NULL, 
		WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,	
		0,0,0,0,hWnd,(HMENU)NULL, g_hInst, (LPSTR)&ccs);

	arFind[0].Init(256,50);
	arFind[1].Init(256,20);
	arFind[2].Init(256,20);
	arFind[3].Init(256,20);

	if (SHRegReadInt(SHCU,KEY"Setting","StartAction",1000) == 1000) {
		Option.GetStyleColor();
		DgShortCut.Init();
		FindFlag=AE_FIND_WRAP | AE_FIND_RECURSIVE | AE_FIND_SHORTPATH;
		arFind[3].num=2;
		arFind[3].Set(0,"*.*");
		arFind[3].Set(1,"*.cpp;*.hpp;*.c;*.h;*.inl");
		Option.bModified=TRUE;
		DgMacro.bModified=TRUE;
		DgShortCut.bModified=TRUE;
	} else {
		Option.Load(KEY);
		DgShortCut.Load(KEY);
		DgMacro.LoadFromReg(KEY);
		FindFlag=SHRegReadInt(SHCU,KEY"Search","Flag",0);
		for (i=0;i<4;i++) {
			wsprintf(szKey,"Find%d-num",i);
			arFind[i].num=SHRegReadInt(SHCU,KEY"Search",szKey,0);
			for (j=0;j<arFind[i].num;j++) {
				wsprintf(szKey,"Find%d-%d",i,j);
				SHRegReadString(SHCU,KEY"Search",szKey,"",szTemp,256);
				arFind[i].Set(j,szTemp);
			}
		}
		RearrangePath();
	}

	if (Option.AutoSaveInterval) {
		SetTimer(hWnd,1,Option.AutoSaveInterval*1000*60,NULL);
	}

	hCurHorz=LoadCursor(g_hInst,MAKEINTRESOURCE(IDC_SIZEHORZ));
	hCurVert=LoadCursor(g_hInst,MAKEINTRESOURCE(IDC_SIZEVERT));

	hdc=GetDC(NULL);
	for (i=1;i<10;i++) {
		arCustomFont[i].lfHeight=arCustomFont[i].lfHeight*GetDeviceCaps(hdc,LOGPIXELSY)/72;
	}
	hGul9=CreateFont(9*GetDeviceCaps(hdc,LOGPIXELSY)/72,0,0,0,0,0,0,0,HANGEUL_CHARSET,3,2,1,
		VARIABLE_PITCH | FF_ROMAN,"����");
	ReleaseDC(NULL,hdc);
	
	hOutput=CreateWindowEx(WS_EX_CLIENTEDGE,"DGOutput",NULL,WS_CHILD | WS_VISIBLE,
		0,0,0,0,hWnd,NULL,g_hInst,NULL);
	CreateToolBar(Option.bToolBarText, Option.bToolBarBig);
	hStatus=CreateStatusWindow(WS_CHILD | WS_VISIBLE, "", hWnd,0);
	hFileWnd=CreateWindowEx(WS_EX_CLIENTEDGE,"DGFile",NULL,WS_CHILD | WS_VISIBLE,
		0,0,0,0,hWnd,NULL,g_hInst,NULL);
	hTabFrame=CreateWindowEx(0,"DGTabFrame",NULL,WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
		0,0,0,25,hWnd,NULL,g_hInst,NULL);
	hClipWnd=CreateWindowEx(WS_EX_CLIENTEDGE,"DGClipFrame",NULL,WS_CHILD | WS_VISIBLE,
		0,0,0,0,hWnd,NULL,g_hInst,NULL);

	DragAcceptFiles(hWnd,TRUE);
	Mru.InitMRU(KEY"MRU",GetSubMenu(GetSubMenu(GetMenu(hWnd),0),7),
		45000,Option.GetMaxMru());
	Mru.LoadMRU();
	g_StartPage=SHRegReadInt(SHCU,KEY"Setting","StartPage",0);
	hCbtHook=SetWindowsHookEx(WH_CBT,DgCbtHookProc,NULL,GetCurrentThreadId());
	// ���� ��Ʈ���� �д´�.
	if (IsStrEmpty(Option.BackBit)==FALSE) {
		if (strncmp(Option.BackBit,"����:",5) == 0) {
			if (strcmp(Option.BackBit+5,"��ȭ")==0) 
				hBackBit=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_SAMPLE1));
			if (strcmp(Option.BackBit+5,"��")==0) 
				hBackBit=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_SAMPLE2));
			if (strcmp(Option.BackBit+5,"�ư�")==0) 
				hBackBit=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_SAMPLE3));
		} else {
			hBackBit=MakeDDBFromDIB(Option.BackBit);
		}
	}
	SetTimer(hWnd,100,1,NULL);
	SetTimer(hWnd,3,3000,NULL);
	return 0;
}

void OnDestroy(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hActive;
	TCHAR szKey[16];
	int nEditing;
	SInfo *pSi;
	BOOL bMax;
	int i,j;
	TCHAR LastFolder[MAX_PATH];
	int count;
	TCITEM tie;
	HTREEITEM hNode;
	TVITEM TV;

	// ������ �ʿ䰡 ���� ���� �����Ѵ�.
	// �ɼ��� ��� ����, ���¶�, ����â�� ���� ���� �� �ɼ� ��ȭ���ڿ��� �������� �ʴ�
	// �͵��� ���� 1.21���� ������ �����ϴ� ������ ������. 
	/*if (Option.bModified)*/ Option.Save(KEY);
	if (DgMacro.bModified) DgMacro.SaveToReg(KEY);
	if (DgShortCut.bModified) DgShortCut.Save(KEY);

	nEditing=0;
	count=TabCtrl_GetItemCount(hFileTab);
	for (i=0;i<count;i++) {
		tie.mask=TCIF_PARAM;
		TabCtrl_GetItem(hFileTab,i,&tie);
		if (strncmp((TCHAR *)tie.lParam,"�̸�����",8)!=0) {
			wsprintf(szKey,"%d",++nEditing);
			SHRegWriteString(SHCU,KEY"Editing",szKey,(TCHAR *)tie.lParam);
		}
	}
	SHRegWriteInt(SHCU,KEY"Editing","Num",nEditing);

	SHRegWriteString(SHCU,KEY"Editing","Active","");
	hActive=(HWND)SendMessage(g_hMDIClient,WM_MDIGETACTIVE,0,(LPARAM)&bMax);
	if (hActive) {
		pSi=(SInfo *)GetWindowLongPtr(hActive,0);
		if (strncmp(pSi->NowFile,"�̸�����",8)!=0) {
			SHRegWriteString(SHCU,KEY"Editing","Active",pSi->NowFile);
			SHRegWriteInt(SHCU,KEY"Editing","ActiveMax",bMax);
		}
	}

	SHRegWriteInt(SHCU,KEY"Search","Flag",FindFlag);
	for (i=0;i<4;i++) {
		wsprintf(szKey,"Find%d-num",i);
		SHRegWriteInt(SHCU,KEY"Search",szKey,arFind[i].num);
		for (j=0;j<arFind[i].num;j++) {
			wsprintf(szKey,"Find%d-%d",i,j);
			SHRegWriteString(SHCU,KEY"Search",szKey,arFind[i].Get(j));
		}
	}

	nEditing=0;
	hNode=TreeView_GetRoot(hFileTree);
	while (hNode) {
		TV.mask=TVIF_PARAM | TVIF_IMAGE;
		TV.hItem=hNode;
		TreeView_GetItem(hFileTree,&TV);
		if (TV.iImage == DGI_PROJECT) {
			wsprintf(szKey,"Project%d",++nEditing);
			SHRegWriteString(SHCU,KEY"Project",szKey,(TCHAR *)TV.lParam);
		}
		hNode=TreeView_GetNextSibling(hFileTree,hNode);
	}
	SHRegWriteInt(SHCU,KEY"Project","Num",nEditing);

	// ���� ���� ��뱸 ��� ����
	nEditing=0;
	hNode=TreeView_GetRoot(hClipTree);
	while (hNode) {
		TV.mask=TVIF_PARAM | TVIF_IMAGE;
		TV.hItem=hNode;
		TreeView_GetItem(hClipTree,&TV);
		if (TV.iImage == DGI_CLIPROOT) {
			wsprintf(szKey,"Clip%d",++nEditing);
			SHRegWriteString(SHCU,KEY"Clip",szKey,(TCHAR *)TV.lParam);
		}
		hNode=TreeView_GetNextSibling(hClipTree,hNode);
	}
	SHRegWriteInt(SHCU,KEY"Clip","Num",nEditing);

	if (arFont)
		free(arFont);
	PostQuitMessage(0);
	SavePosition(hWnd,KEY"Position");
	Mru.SaveMRU();
	DeleteObject(hGul9);
	GetCurrentFolder(LastFolder);
	SHRegWriteString(SHCU,KEY"Setting","LastFolder",LastFolder);
	if (pEditings) {
		free(pEditings);
	}
	SHRegWriteInt(SHCU,KEY"Setting","StartPage",g_StartPage);
	UnhookWindowsHookEx(hCbtHook);
	if (hBackBit) {
		DeleteObject(hBackBit);
	}
	KillTimer(hWnd,1);
}

void OnTimer(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int i;
	HWND hChild;
	TCHAR szKey[16];
	int nEditing;
	TCHAR Path[MAX_PATH];
	BOOL bOldMaxFirstChild;
	TCHAR Drive[_MAX_DRIVE];
	TCHAR Dir[_MAX_DIR];

	switch (wParam) {
	case 1:
		AutoSave();
		break;
	case 2:
		KillTimer(hWnd,2);
		DialogBox(g_hInst,MAKEINTRESOURCE(IDD_CONFIRMRELOAD),
			g_hFrameWnd,ConfirmReloadProc);
		break;
	case 3:
		SetStatusText(0x3000);
		break;
	case 100:
		KillTimer(hWnd,100);
		if (Option.bReloadProject) {
			nEditing=SHRegReadInt(SHCU,KEY"Project","Num",0);
			for (i=1;i<=nEditing;i++) {
				wsprintf(szKey,"Project%d",i);
				SHRegReadString(SHCU,KEY"Project",szKey,"",Path,MAX_PATH);
				LoadProject(Path,FALSE);
			}
		}

		// ����� ��뱸 ����� �ٽ� �д´�.
		if (Option.bReloadClip) {
			nEditing=SHRegReadInt(SHCU,KEY"Clip","Num",0);
			for (i=1;i<=nEditing;i++) {
				wsprintf(szKey,"Clip%d",i);
				SHRegReadString(SHCU,KEY"Clip",szKey,"",Path,MAX_PATH);
				LoadClipFile(Path,FALSE);
			}
		}

		if (__argc > 1) {
			OpenFromFile(__argv[1]);
			break;
		}
		bOldMaxFirstChild=Option.bMaxFirstChild;
		if (SHRegReadInt(SHCU,KEY"Editing","ActiveMax",1)==1) {
			Option.bMaxFirstChild=TRUE;
		} else {
			Option.bMaxFirstChild=FALSE;
		}

		switch (Option.StartAction) {
		case 0:
			break;
		case 1:
			NewChild();
			break;
		case 2:
			nEditing=SHRegReadInt(SHCU,KEY"Editing","Num",0);
			SHRegReadString(SHCU,KEY"Editing","Active","",Path,MAX_PATH);
			if (nEditing && lstrlen(Path) != 0) {
				OpenFromFile(Path);
			} else {
				NewChild();
			}
			break;
		case 3:
			nEditing=SHRegReadInt(SHCU,KEY"Editing","Num",0);
			for (i=1;i<=nEditing;i++) {
				wsprintf(szKey,"%d",i);
				SHRegReadString(SHCU,KEY"Editing",szKey,"",Path,MAX_PATH);
				OpenFromFile(Path);
			}
			SHRegReadString(SHCU,KEY"Editing","Active","",Path,MAX_PATH);
			if (lstrlen(Path)!= 0) {
				hChild=FindChildWithFile(Path);
				SendMessage(g_hMDIClient,WM_MDIACTIVATE,(WPARAM)hChild,0);
			}
			break;
		}

		// ó�� ������ �� ���� ������Ʈ�� ��뱸�� ����
		if (SHRegReadInt(SHCU,KEY"Project","Num",101092)==101092) {
			SHRegReadString(SHCU,KEY"Setting","InstPath","",Path,MAX_PATH);
			_splitpath(Path,Drive,Dir,NULL,NULL);
			wsprintf(Path,"%s%sSample.dgp",Drive,Dir);
			if (_access(Path,0)==0) {
				LoadProject(Path,FALSE);
			}
			wsprintf(Path,"%s%sSample.dgc",Drive,Dir);
			if (_access(Path,0)==0) {
				LoadClipFile(Path,FALSE);
			}
		}

		Option.bMaxFirstChild=bOldMaxFirstChild;
		SHRegReadString(SHCU,KEY"Setting","LastFolder","",Path,MAX_PATH);
		GotoFolderInTree(Path);
		break;
	}
}

void OnCommand(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hActive;
	SInfo *pSi;
	TCHAR Mes[512];
	CHOOSEFONT CFT;
	LOGFONT lf;
	TCHAR Path[MAX_PATH];
	int idx;
	DWORD tFlag;
	int s,e;
	TCHAR szTemp[256];
	SYSTEMTIME st;
	CApiEditText *pView;

	hActive=(HWND)SendMessage(g_hMDIClient,WM_MDIGETACTIVE,0,NULL);
	if (hActive==NULL) {
		if (TestNeedActive(LOWORD(wParam)))
			return;
	} else {
		pSi=(SInfo *)GetWindowLongPtr(hActive,0);
		pView=pSi->Ae.GetActiveView();
	}

	if (LOWORD(wParam) >= Mru.MenuID && LOWORD(wParam) < Mru.MenuID+Mru.MaxMru) {
		idx=LOWORD(wParam)-Mru.MenuID;
		if (Mru.GetFilePath(idx,Path)==FALSE) {
			return;
		}
		if (strnicmp(Path,"http",4) != 0 && strnicmp(Path,"ftp",3) != 0) {
			if (_access(Path,0) != 0) {
				wsprintf(Mes,"%s ������ ã�� �� �����ϴ�. �ֱ� ���� ��Ͽ��� "
					"�� ������ �����մϴ�.",Path);
				MessageBox(g_hFrameWnd, Mes,"�˸�",MB_OK);
				Mru.RemoveMRU(idx);
				return;
			}
		}
		OpenFromFile(Path);
		return;
	}

	switch(LOWORD(wParam)) {
	case IDM_FILE_NEW:
		NewChild();
		break;
	case IDM_FILE_OPEN:
		Open();
		break;
	case IDM_FILE_SAVE:
		Save(hActive);
		break;
	case IDM_FILE_SAVEAS:
		SaveAs(hActive);
		break;
	case IDM_FILE_CLOSE:
		SendMessage(hActive,WM_CLOSE,0,0);
		break;
	case IDM_FILE_SAVEALL:
		SaveAll();
		break;
	case IDM_FILE_CLOSEALL:
		CloseAll();
		break;
	case IDM_FILE_READONLY:
		pSi->Ae.SetReadOnly(!pSi->Ae.GetReadOnly());
		break;
	case IDM_FILE_REVERT:
		wsprintf(Mes,"%s ������ ���� ����� ���·� �ٽ� �����ðڽ��ϱ�?\r\n"
			"���ݱ��� ������ ��� ������ �Ұ� �˴ϴ�.",pSi->NowFile);
		if (MessageBox(g_hFrameWnd,Mes,"����",MB_YESNO)==IDYES) {
			RevertFile(hActive);
		}
		break;
	case IDM_FILE_ANSI:
		pSi->Ae.SetFormat(AE_ENC_ANSI);
		SetStatusText(0x10);
		break;
	case IDM_FILE_UNI:
		pSi->Ae.SetFormat(AE_ENC_UNI);
		SetStatusText(0x10);
		break;
	case IDM_FILE_UNIBIG:
		pSi->Ae.SetFormat(AE_ENC_UNIBIG);
		SetStatusText(0x10);
		break;
	case IDM_FILE_UNINOBOM:
		pSi->Ae.SetFormat(AE_ENC_UNINOBOM);
		SetStatusText(0x10);
		break;
	case IDM_FILE_UNIBIGNOBOM:
		pSi->Ae.SetFormat(AE_ENC_UNIBIGNOBOM);
		SetStatusText(0x10);
		break;
	case IDM_FILE_UTF8:
		pSi->Ae.SetFormat(AE_ENC_UTF8);
		SetStatusText(0x10);
		break;
	case IDM_FILE_UTF8NOBOM:
		pSi->Ae.SetFormat(AE_ENC_UTF8NOBOM);
		SetStatusText(0x10);
		break;
	case IDM_FILE_INFO:
		PrintInfo(pSi->Ae);
		break;
	case IDM_FILE_HTTPOPEN:
		if (DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_HTTP), 
			g_hFrameWnd, DGHttpProc,(LPARAM)Path)==IDOK) {
			if (lstrlen(Path)) {
				OpenFromFile(Path);
			}
		}
		break;
	case IDM_FILE_FTPOPEN:
		TCHAR *FtpPath,*p;
		TCHAR Dir[MAX_PATH+64];
		FtpPath=(TCHAR *)malloc(10000);
		if (DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_FTP), 
			g_hFrameWnd, DGFtpProc,(LPARAM)FtpPath)==IDOK) {
			p=FtpPath;
			lstrcpy(Dir,p);
			p=p+lstrlen(Dir)+1;
			if (*p==0) {
				OpenFromFile(Dir);
			} else {
				for (;*p;) {
					wsprintf(Path,"%s%s",Dir,p);
					p=p+lstrlen(p)+1;
					OpenFromFile(Path);
				}
			}
		}
		free(FtpPath);
		break;
	case IDM_FILE_PROJECT:
		DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_PROJECT), 
			g_hFrameWnd, DgProjectProc,(LPARAM)NULL);
		break;
	case IDM_FILE_PRINT:
		if (Option.prtbDirect) {
			PrintDirect(hActive);
		} else {
			Print(hActive);
		}
		break;
	case IDM_FILE_EXIT:
		SendMessage(hWnd,WM_CLOSE,0,0);
		break;
	case IDM_EDIT_UNDO:
		if (pSi->Ae.CanUndo())
			pSi->Ae.Undo();
		break;
	case IDM_EDIT_REDO:
		if (pSi->Ae.CanRedo())
			pSi->Ae.Redo();
		break;
	case IDM_EDIT_COPY:
		SendMessage(pSi->Ae.hWnd,WM_COMMAND,MAKEWPARAM(IDM_AE_COPY,0),0);
		break;
	case IDM_EDIT_CUT:
		SendMessage(pSi->Ae.hWnd,WM_COMMAND,MAKEWPARAM(IDM_AE_CUT,0),0);
		break;
	case IDM_EDIT_PASTE:
		SendMessage(pSi->Ae.hWnd,WM_COMMAND,MAKEWPARAM(IDM_AE_PASTE,0),0);
		break;
	case IDM_EDIT_SELALL:
		SendMessage(pSi->Ae.hWnd,WM_COMMAND,MAKEWPARAM(IDM_AE_SELALL,0),0);
		break;
	case IDM_EDIT_UPPER:
		pView->ChangeCase(0);
		break;
	case IDM_EDIT_LOWER:
		pView->ChangeCase(1);
		break;
	case IDM_EDIT_REVERSE:
		pView->ChangeCase(2);
		break;
	case IDM_EDIT_UPPERFIRST:
		pView->ChangeCase(3);
		break;
	case IDM_EDIT_UPPERSENT:
		pView->ChangeCase(4);
		break;
	case IDM_EDIT_TOTAB:
		pView->ConvertSpaceTab(FALSE);
		break;
	case IDM_EDIT_TOSPACE:
		pView->ConvertSpaceTab(TRUE);
		break;
	case IDM_EDIT_REMOVESPACE:
		pView->RemoveExtraSpace();
		break;
	case IDM_EDIT_INDENT:
		pView->BlockIndent(FALSE);
		break;
	case IDM_EDIT_UNINDENT:
		pView->BlockIndent(TRUE);
		break;
	case IDM_EDIT_COMMENT:
		pView->InsertComment(FALSE);
		break;
	case IDM_EDIT_UNCOMMENT:
		pView->InsertComment(TRUE);
		break;
	case IDM_EDIT_DATE:
		GetLocalTime(&st);
		wsprintf(szTemp,"%d�� %d�� %d�� ",st.wYear, st.wMonth, st.wDay);
		pView->InsertText(szTemp);
		break;
	case IDM_EDIT_TIME:
		GetLocalTime(&st);
		wsprintf(szTemp,"%d�� %d�� %d�� ",st.wHour, st.wMinute, st.wSecond);
		pView->InsertText(szTemp);
		break;
	case IDM_EDIT_COPYUP:
		pView->CopyChar(0);
		break;
	case IDM_EDIT_COPYDOWN:
		pView->CopyChar(1);
		break;
	case IDM_EDIT_COPYAPPEND:
		SendMessage(pSi->Ae.hWnd,WM_COMMAND,MAKEWPARAM(IDM_AE_COPYAPPEND,0),0);
		break;
	case IDM_EDIT_CUTAPPEND:
		SendMessage(pSi->Ae.hWnd,WM_COMMAND,MAKEWPARAM(IDM_AE_CUTAPPEND,0),0);
		break;
	case IDM_EDIT_VIRT:
		if (pView->ViewType != HEXA_VIEW)
			pView->SetVirtualPos(!pView->GetVirtualPos());
		break;
	case IDM_EDIT_COLUMN:
		if (pView->ViewType != HEXA_VIEW)
			pView->SetColumnMode(!pView->GetColumnMode());
		break;
	case IDM_EDIT_COMMODE:
		bComMode=!bComMode;
		break;
	case IDM_EDIT_INSERTFILE:
		InsertFile(pView);
		break;
	case IDM_EDIT_INSERTSPECIAL:
		DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_SPECIAL),hWnd,
			SpecialDlgProc,(LPARAM)pView);
		break;
	case IDM_VIEW_WORDWRAP:
		pSi->Ae.SetWrap(pSi->Ae.GetWrap()==0 ? Option.nWrap:0);
		break;
	case IDM_VIEW_HEXA:
		if (pView->ViewType == HEXA_VIEW) {
			pSi->Ae.ChangeViewType(-1,TEXT_VIEW);
		} else {
			pSi->Ae.ChangeViewType(-1,HEXA_VIEW);
		}
		break;
	case IDM_VIEW_CODE:
		if (pSi->Ae.TestStyle(AES_SHOWENTER)) {
			pSi->Ae.ChangeStyle(AES_SHOWENTER,FALSE);
			pSi->Ae.ChangeStyle(AES_SHOWTAB,FALSE);
			pSi->Ae.ChangeStyle(AES_SHOWSPACE,FALSE);
		} else {
			pSi->Ae.ChangeStyle(AES_SHOWENTER,TRUE);
			pSi->Ae.ChangeStyle(AES_SHOWTAB,TRUE);
			pSi->Ae.ChangeStyle(AES_SHOWSPACE,TRUE);
		}
		break;
	case IDM_FONT_1:
		pSi->Ae.SetFont(&arCustomFont[0]);
		break;
	case IDM_FONT_2:
		pSi->Ae.SetFont(&arCustomFont[1]);
		break;
	case IDM_FONT_3:
		pSi->Ae.SetFont(&arCustomFont[2]);
		break;
	case IDM_FONT_4:
		pSi->Ae.SetFont(&arCustomFont[3]);
		break;
	case IDM_FONT_5:
		pSi->Ae.SetFont(&arCustomFont[4]);
		break;
	case IDM_FONT_6:
		pSi->Ae.SetFont(&arCustomFont[5]);
		break;
	case IDM_FONT_7:
		pSi->Ae.SetFont(&arCustomFont[6]);
		break;
	case IDM_FONT_8:
		pSi->Ae.SetFont(&arCustomFont[7]);
		break;
	case IDM_FONT_9:
		pSi->Ae.SetFont(&arCustomFont[8]);
		break;
	case IDM_FONT_10:
		pSi->Ae.SetFont(&arCustomFont[9]);
		break;
	case IDM_FONT_11:
		memset(&CFT, 0, sizeof(CHOOSEFONT));
		CFT.lStructSize = sizeof(CHOOSEFONT);
		CFT.hwndOwner=hWnd;
		pSi->Ae.GetFont(&lf);
		CFT.lpLogFont=&lf;
		CFT.Flags=CF_EFFECTS | CF_SCREENFONTS;
		if (ChooseFont(&CFT)) {
			pSi->Ae.SetFont(&lf);
		}
		break;
	case IDM_LINE_1:
		pSi->Ae.SetLineRatio(100);
		break;
	case IDM_LINE_2:
		pSi->Ae.SetLineRatio(120);
		break;
	case IDM_LINE_3:
		pSi->Ae.SetLineRatio(150);
		break;
	case IDM_LINE_4:
		pSi->Ae.SetLineRatio(200);
		break;
	case IDM_LINE_5:
		pSi->Ae.SetLineRatio(300);
		break;
	case IDM_SYN_NONE:
		pSi->Ae.SetParser(0);
		break;
	case IDM_SYN_CPP:
		pSi->Ae.SetParser(1);
		Option.SetStyleColor(pSi->Ae.GetParser());
		break;
	case IDM_SYN_HTML:
		pSi->Ae.SetParser(2);
		Option.SetStyleColor(pSi->Ae.GetParser());
		break;
	case IDM_SYN_SQL:
		pSi->Ae.SetParser(3);
		Option.SetStyleColor(pSi->Ae.GetParser());
		break;
	case IDM_SYN_JAVA:
		pSi->Ae.SetParser(4);
		Option.SetStyleColor(pSi->Ae.GetParser());
		break;
	case IDM_SYN_CS:
		pSi->Ae.SetParser(5);
		Option.SetStyleColor(pSi->Ae.GetParser());
		break;
	case IDM_VIEW_OUTPUT:
		bShowOutput = !bShowOutput;
		Relayout();
		break;
	case IDM_VIEW_TOOLBAR:
		Option.bShowToolBar = !Option.bShowToolBar;
		Relayout();
		break;
	case IDM_VIEW_STATUS:
		Option.bShowStatus = !Option.bShowStatus;
		Relayout();
		break;
	case IDM_VIEW_FILE:
		Option.bShowFileWnd = !Option.bShowFileWnd;
		Relayout();
		break;
	case IDM_VIEW_FILETAB:
		Option.bShowFileTab = !Option.bShowFileTab;
		Relayout();
		break;
	case IDM_VIEW_CLIP:
		Option.bShowClipWnd = !Option.bShowClipWnd;
		Relayout();
		break;
	case IDM_SEARCH_FIND:
		if (!IsWindow(g_FindDlg)) {
			g_FindDlg=CreateDialog(g_hInst,MAKEINTRESOURCE(IDD_FIND),hWnd,FindDlgProc);
			ShowWindow(g_FindDlg,SW_SHOW);
		} else {
			if (GetDlgItem(g_FindDlg,IDC_FIND_TO)) {
				SendMessage(g_FindDlg,WM_COMMAND,MAKEWPARAM(IDCANCEL,0),0);
				SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_SEARCH_FIND,0),0);
			} else {
				SetFocus(g_FindDlg);
			}
		}
		break;
	case IDM_SEARCH_REPLACE:
		if (!IsWindow(g_FindDlg)) {
			g_FindDlg=CreateDialog (g_hInst,MAKEINTRESOURCE(IDD_REPLACE),hWnd,FindDlgProc);
			ShowWindow(g_FindDlg,SW_SHOW);
		} else {
			if (GetDlgItem(g_FindDlg,IDC_FIND_TO)==NULL) {
				SendMessage(g_FindDlg,WM_COMMAND,MAKEWPARAM(IDCANCEL,0),0);
				SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_SEARCH_REPLACE,0),0);
			} else {
				SetFocus(g_FindDlg);
			}
		}
		break;
	case IDM_SEARCH_NEXT:
		if (Option.bInitNextFind) {
			SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_SEARCH_WORD,0),0);
		} else if (lstrlen(arFind[0].Get(0))) {
			tFlag=FindFlag;
			tFlag &= ~AE_FIND_UP;
			if (pSi->Ae.FindText(-1,arFind[0].Get(0), tFlag) == FALSE) {
				wsprintf(Mes, "ã�� ���ڿ� \"%s\"�� �����ϴ�",arFind[0].Get(0));
				if (Option.bSoundFindFail) {
					MessageBeep(MB_ICONASTERISK);
				}
				SetStatusText(1,Mes);
			}
		}
		break;
	case IDM_SEARCH_PREV:
		if (Option.bInitNextFind) {
			SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_SEARCH_WORDUP,0),0);
		} else if (lstrlen(arFind[0].Get(0))) {
			tFlag=FindFlag;
			tFlag |= AE_FIND_UP;
			if (pSi->Ae.FindText(-1,arFind[0].Get(0), tFlag) == FALSE) {
				wsprintf(Mes, "ã�� ���ڿ� \"%s\"�� �����ϴ�",arFind[0].Get(0));
				if (Option.bSoundFindFail) {
					MessageBeep(MB_ICONASTERISK);
				}
				SetStatusText(1,Mes);
			}
		}
		break;
	case IDM_SEARCH_WORD:
	case IDM_SEARCH_WORDUP:
		int nStart;
		pSi->Ae.GetSelect(s,e);
		if (s==e) {
			pView->GetNowWord(pSi->Ae.GetOffset(),s,e);
		}
		if (e-s > 255 || e-s <= 0) {
			break;
		}
		pSi->Ae.GetText(szTemp,256,s,e);
		szTemp[e-s]=0;
		arFind[0].Add(szTemp);

		tFlag=FindFlag;
		tFlag &= ~AE_FIND_WHOLEWORD;
		tFlag |= AE_FIND_WRAP;
		if (LOWORD(wParam) == IDM_SEARCH_WORD) {
			tFlag &= ~AE_FIND_UP;
			nStart=max(s,e);
		} else {
			tFlag |= AE_FIND_UP;
			nStart=min(s,e);
		}
		pSi->Ae.FindText(nStart,arFind[0].Get(0),tFlag);
		break;
	case IDM_SEARCH_TOGGLEMARK:
		pSi->Ae.ToggleBookmark(-1,100);
		break;
	case IDM_SEARCH_NEXTMARK:
		pSi->Ae.GotoBookmark(10);
		break;
	case IDM_SEARCH_PREVMARK:
		pSi->Ae.GotoBookmark(11);
		break;
	case IDM_SEARCH_CLEARMARK:
		pSi->Ae.ClearBookmark();
		break;
	case IDM_SEARCH_GOTOLINE:
		s=DialogBox(g_hInst,MAKEINTRESOURCE(IDD_GOTOLINE),hWnd,GotoLineDlgProc);
		if (s != -1) {
			if (pSi->Ae.GetActiveView()->ViewType != HEXA_VIEW) {
				pSi->Ae.GotoLine(s-1,0,1);
			} else {
				pSi->Ae.GotoLine(s-1,0,0);
			}
		}
		break;
	case IDM_SEARCH_PAREN:
		pView->FindParen(FALSE);
		break;
	case IDM_SEARCH_PARENSEL:
		pView->FindParen(TRUE);
		break;
	case IDM_SEARCH_LASTEDIT:
		pSi->Ae.GotoLastEditOff();
		break;
	case IDM_SEARCH_FILES:
		if (TestFIFThread()==TRUE) {
			if (DialogBox(g_hInst,MAKEINTRESOURCE(IDD_FILEFIND),hWnd,FileFindDlgProc)==IDOK) {
				FindOrReplaceInFiles(FALSE);
			}
		}
		break;
	case IDM_SEARCH_RFILES:
		if (TestFIFThread()==TRUE) {
			if (DialogBox(g_hInst,MAKEINTRESOURCE(IDD_FILEREPLACE),hWnd,FileFindDlgProc)==IDOK) {
				FindOrReplaceInFiles(TRUE);
			}
		}
		break;
	case IDM_SEARCH_INST:
		InstantSearch(hInstEdit==NULL);
		return;
	case IDM_TOOL_CONFIG:
		Config();
		break;
	case IDM_TOOL_UNINSTALL:
		if (g_ChildNum) {
			MessageBox(g_hFrameWnd, "����â�� ���� �ִ� ���¿����� ��ġ ������"
				" �� �� �����ϴ�. ��� ����â�� ���� �� �ٽ� ������ �ֽʽÿ�",
				"�˸�",MB_OK);
			return;
		}
		if (DialogBox(g_hInst, MAKEINTRESOURCE(IDD_UNINSTALL), g_hFrameWnd, 
			UninstallDlgProc)==IDOK) {
			bUninstall=TRUE;
			DestroyWindow(g_hFrameWnd);
		}
		break;
	case IDM_TOOL_MACRO:
		DgMacro.bModified=TRUE;
		DgShortCut.bModified=TRUE;
		DialogBox(g_hInst,MAKEINTRESOURCE(IDD_MACRO),hWnd,MacroDlgProc);
		break;
	case IDM_TOOL_SHORTCUT:
		DgShortCut.bModified=TRUE;
		DialogBox(g_hInst,MAKEINTRESOURCE(IDD_SHORTCUTLIST),hWnd,ShortCutListProc);
		break;
	case IDM_TOOL_CLIP:
		DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_CLIPFILE),
			hWnd,ClipDlgProc,(LPARAM)NULL);
		break;
	case IDM_TOOL_READMODE:
		ReadMode();
		break;
	case IDM_TOOL_PARA:
		if (pView->ViewType != HEXA_VIEW)
			DialogBox(g_hInst,MAKEINTRESOURCE(IDD_PARA),hWnd,ParaDlgProc);
		break;
	case IDM_WIN_VERT:
		SendMessage(g_hMDIClient, WM_MDITILE, 
			(WPARAM)MDITILE_VERTICAL, 0);
		break;
	case IDM_WIN_HORZ:
		SendMessage(g_hMDIClient, WM_MDITILE, 
			(WPARAM)MDITILE_HORIZONTAL, 0);
		break;
	case IDM_WIN_NEXTVIEW:
		pSi->Ae.ChangeActiveView(pView,1);
		break;
	case IDM_WIN_PREVVIEW:
		pSi->Ae.ChangeActiveView(pView,2);
		break;
	case IDM_WIN_CASCADE:
		SendMessage(g_hMDIClient, WM_MDICASCADE, 
			(WPARAM)MDITILE_SKIPDISABLED, 0);
		break;
	case IDM_WIN_ARRANGE:
		SendMessage(g_hMDIClient, WM_MDIICONARRANGE, 0, 0);
		break;
	case IDM_HELP_HOME:
		ShellExecute(hWnd,"open","http://www.soen.kr/project/dangeun/dangeun.htm",NULL,NULL,SW_SHOW);
		break;
	case IDM_HELP_INDEX:
		ShellExecute(hWnd,"open","http://www.soen.kr/project/dangeun/manual/manual.htm",NULL,NULL,SW_SHOW);
		break;
	case IDM_HELP_ABOUT:
		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUT), hWnd, AboutProc);
		break;
	}
}

BOOL OnClose(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
#ifndef _DEBUG
	if (TestNoSaved()) {
		return DialogBox(g_hInst,MAKEINTRESOURCE(IDD_CONFIRMSAVE),
			g_hFrameWnd,ConfirmSaveProc);
	}
#endif
	return FALSE;
}

BOOL OnQueryEndSession(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	BOOL Result;

	if (TestNoSaved()) {
		Result=DialogBox(g_hInst,MAKEINTRESOURCE(IDD_CONFIRMSAVE),
			g_hFrameWnd,ConfirmSaveProc);
		if (Result) {
			return TRUE;
		}
	}
	// �ý��� �������� �ɼ��� �����Ѵ�.
	OnDestroy(hWnd,0,0);
	return FALSE;
}

#define DGCHECKMENUITEM(ID,Con) CheckMenuItem(hMenu,ID, MF_BYCOMMAND | ((Con) ? MF_CHECKED:MF_UNCHECKED))
#define DGENABLEMENUITEM(ID,Con) EnableMenuItem(hMenu,ID, MF_BYCOMMAND | ((Con) ? MF_ENABLED:MF_GRAYED))
void OnInitMenu(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HMENU hMenu;
	HWND hActive;
	SInfo *pSi;
	int s,e;
	CApiEditText *pView;
	BOOL bHexa,bText,bSel;

	hMenu=(HMENU)wParam;
	UpdateMenuCaption(hMenu);
	hActive=(HWND)SendMessage(g_hMDIClient,WM_MDIGETACTIVE,0,NULL);
	if (hActive) {
		pSi=(SInfo *)GetWindowLongPtr(hActive,0);
		pView=pSi->Ae.GetActiveView();

		// ��� ��� ���ÿ��� ���� �̸� ����
		bHexa=(pView->ViewType==HEXA_VIEW);
		bText=(pView->ViewType==TEXT_VIEW);
		pSi->Ae.GetSelect(s,e);
		bSel=(s != e);

		DGENABLEMENUITEM(IDM_EDIT_VIRT,bText);
		DGENABLEMENUITEM(IDM_EDIT_COLUMN,bText);
		DGENABLEMENUITEM(IDM_TOOL_PARA,bText);
		DGCHECKMENUITEM(IDM_EDIT_VIRT,pView->GetVirtualPos() && bText);
		DGCHECKMENUITEM(IDM_EDIT_COLUMN,pView->GetColumnMode() && bText);
		DGCHECKMENUITEM(IDM_VIEW_HEXA,bHexa);

		DGCHECKMENUITEM(IDM_FILE_READONLY,pSi->Ae.GetReadOnly());
		DGENABLEMENUITEM(IDM_FILE_REVERT,pSi->Ae.GetModified() && 
			strncmp(pSi->NowFile,"�̸�����",8)!=0);

		DGENABLEMENUITEM(IDM_EDIT_CUT,bSel);
		DGENABLEMENUITEM(IDM_EDIT_COPY,bSel);
		DGENABLEMENUITEM(IDM_EDIT_CUTAPPEND,bSel);
		DGENABLEMENUITEM(IDM_EDIT_COPYAPPEND,bSel);
		DGENABLEMENUITEM(IDM_EDIT_UPPERSENT,bSel);
		DGENABLEMENUITEM(IDM_EDIT_TOTAB,bSel && !bHexa);
		DGENABLEMENUITEM(IDM_EDIT_TOSPACE,bSel && !bHexa);
		DGENABLEMENUITEM(IDM_EDIT_PASTE,IsClipboardFormatAvailable(CF_TEXT)==FALSE);

		DGCHECKMENUITEM(IDM_VIEW_WORDWRAP,pSi->Ae.GetWrap() != 0);
		DGCHECKMENUITEM(IDM_VIEW_CODE,pSi->Ae.TestStyle(AES_SHOWENTER));

		// �ٰ� ǥ��
		for (s=IDM_LINE_1;s<=IDM_LINE_5;s++) DGCHECKMENUITEM(s,FALSE);
		switch (pSi->Ae.GetLineRatio()) {
		case 100:DGCHECKMENUITEM(IDM_LINE_1,TRUE);break;
		case 120:DGCHECKMENUITEM(IDM_LINE_2,TRUE);break;
		case 150:DGCHECKMENUITEM(IDM_LINE_3,TRUE);break;
		case 200:DGCHECKMENUITEM(IDM_LINE_4,TRUE);break;
		case 300:DGCHECKMENUITEM(IDM_LINE_5,TRUE);break;
		}

		DGENABLEMENUITEM(IDM_EDIT_UNDO,pSi->Ae.CanUndo());
		DGENABLEMENUITEM(IDM_EDIT_REDO,pSi->Ae.CanRedo());

		// ���� ǥ��
		for (s=IDM_SYN_NONE;s<=IDM_SYN_CS;s++) DGCHECKMENUITEM(s,FALSE);
		switch (int(pSi->Ae.GetParser()->GetInfo(0))) {
		case 0:DGCHECKMENUITEM(IDM_SYN_NONE,TRUE);break;
		case 1:DGCHECKMENUITEM(IDM_SYN_CPP,TRUE);break;
		case 2:DGCHECKMENUITEM(IDM_SYN_HTML,TRUE);break;
		case 3:DGCHECKMENUITEM(IDM_SYN_SQL,TRUE);break;
		case 4:DGCHECKMENUITEM(IDM_SYN_JAVA,TRUE);break;
		case 5:DGCHECKMENUITEM(IDM_SYN_CS,TRUE);break;
		}

		DGENABLEMENUITEM(IDM_SEARCH_NEXT,lstrlen(arFind[0].Get(0)) != 0);
		DGENABLEMENUITEM(IDM_SEARCH_PREV,lstrlen(arFind[0].Get(0)) != 0);
		DGENABLEMENUITEM(IDM_EDIT_INDENT,!bHexa);
		DGENABLEMENUITEM(IDM_EDIT_UNINDENT,!bHexa);
		DGENABLEMENUITEM(IDM_EDIT_COPYUP,!bHexa);
		DGENABLEMENUITEM(IDM_EDIT_COPYDOWN,!bHexa);

		DGENABLEMENUITEM(IDM_EDIT_COMMENT,lstrlen(pSi->Ae.GetParser()->GetInfo(2)) && !bHexa);
		DGENABLEMENUITEM(IDM_EDIT_UNCOMMENT,lstrlen(pSi->Ae.GetParser()->GetInfo(2)) && !bHexa);

		// ���� ���� ǥ��
		switch (pSi->Ae.GetFormat() & 0xffff) {
		case AE_ENC_ANSI:
			CheckMenuRadioItem(hMenu,IDM_FILE_ANSI,IDM_FILE_UTF8NOBOM,IDM_FILE_ANSI,MF_BYCOMMAND);
			break;
		case AE_ENC_UNI:
			CheckMenuRadioItem(hMenu,IDM_FILE_ANSI,IDM_FILE_UTF8NOBOM,IDM_FILE_UNI,MF_BYCOMMAND);
			break;
		case AE_ENC_UNIBIG:
			CheckMenuRadioItem(hMenu,IDM_FILE_ANSI,IDM_FILE_UTF8NOBOM,IDM_FILE_UNIBIG,MF_BYCOMMAND);
			break;
		case AE_ENC_UNINOBOM:
			CheckMenuRadioItem(hMenu,IDM_FILE_ANSI,IDM_FILE_UTF8NOBOM,IDM_FILE_UNINOBOM,MF_BYCOMMAND);
			break;
		case AE_ENC_UNIBIGNOBOM:
			CheckMenuRadioItem(hMenu,IDM_FILE_ANSI,IDM_FILE_UTF8NOBOM,IDM_FILE_UNIBIGNOBOM,MF_BYCOMMAND);
			break;
		case AE_ENC_UTF8:
			CheckMenuRadioItem(hMenu,IDM_FILE_ANSI,IDM_FILE_UTF8NOBOM,IDM_FILE_UTF8,MF_BYCOMMAND);
			break;
		case AE_ENC_UTF8NOBOM:
			CheckMenuRadioItem(hMenu,IDM_FILE_ANSI,IDM_FILE_UTF8NOBOM,IDM_FILE_UTF8NOBOM,MF_BYCOMMAND);
			break;
		}
	}

	DGCHECKMENUITEM(IDM_VIEW_OUTPUT,bShowOutput);
	DGCHECKMENUITEM(IDM_VIEW_TOOLBAR,Option.bShowToolBar);
	DGCHECKMENUITEM(IDM_VIEW_STATUS,Option.bShowStatus);
	DGCHECKMENUITEM(IDM_VIEW_FILE,Option.bShowFileWnd);
	DGCHECKMENUITEM(IDM_VIEW_FILETAB,Option.bShowFileTab);
	DGCHECKMENUITEM(IDM_EDIT_COMMODE,bComMode);
	DGCHECKMENUITEM(IDM_VIEW_CLIP,Option.bShowClipWnd);
}

void OnDropFiles(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	TCHAR Path[MAX_PATH];
	int Count,n;

	Count=DragQueryFile((HDROP)wParam,0xffffffff,NULL,0);

	for (n=0;n<Count;n++) {
		DragQueryFile((HDROP)wParam,n,Path,MAX_PATH);
		OpenFromFile(Path);
		SetForegroundWindow(hWnd);
	}
}

void OnFontChange(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	if (arFont)
		free(arFont);
	arFont=NULL;
}

void OnActivateApp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	static BOOL bOnlyOne=TRUE;
	RECT drt;
	SInfo *pSi;
	int idx=0;
	HANDLE hFile;
	DWORD size;
	FILETIME st;
	BOOL bSetTimer=FALSE;

	if (bOnlyOne) {
		if (GetSystemMetrics(SM_CXSCREEN) < 800) {
			SetRect(&drt,10,10,500,300);
		} else {
			SetRect(&drt,10,10,700,500);
		}
		LoadPosition(hWnd,KEY"Position",&drt);
		bOnlyOne=FALSE;
	}

	if (wParam) {
		if (Option.bWatchChange && pEditings) {
			for (idx=0;pEditings[idx].hWnd!=NULL;) {
				pSi=(SInfo *)GetWindowLong(pEditings[idx].hWnd,0);
				hFile=CreateFile(pSi->NowFile,0,0,NULL,
					OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
				if (hFile!=INVALID_HANDLE_VALUE) {
					size=GetFileSize(hFile,NULL);
					GetFileTime(hFile,NULL,NULL,&st);
					if (pEditings[idx].size!=size || 
						CompareFileTime(&pEditings[idx].st,&st)!=0) {
						if (Option.bReloadNoAsk) {
							RevertFile(pEditings[idx].hWnd);
						} else {
							SetTimer(hWnd,2,1,NULL);
							bSetTimer=TRUE;
							CloseHandle(hFile);
							break;
						}
					}
					CloseHandle(hFile);
				}
				idx++;
			}
			if (bSetTimer==FALSE) {
				free(pEditings);
				pEditings=NULL;
			}
		}
	} else {
		SaveEditings();
	}
}

void OnUser1(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	TCHAR Path[MAX_PATH];

	if (wParam) {
		GlobalGetAtomName((ATOM)wParam,Path,MAX_PATH);
		OpenFromFile(Path);
	}
	if (IsIconic(hWnd)) {
		ShowWindow(hWnd,SW_RESTORE);
	}
}

void OnUser2(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hActive;
	SInfo *pSi;
	int i;
	DWORD tFlag;
	TCHAR Mes[512];
	RECT srt;

	if (IsWindowEnabled(hWnd)==FALSE)
		return;

	hActive=(HWND)SendMessage(g_hMDIClient,WM_MDIGETACTIVE,0,NULL);
	if (hActive == NULL) {
		return;
	}
	pSi=(SInfo *)GetWindowLongPtr(hActive,0);
	switch (wParam) {
	case 1:
		if (pSi->Ae.FindText(-1,arFind[0].Get(0), FindFlag) == FALSE) {
			wsprintf(Mes, "ã�� ���ڿ� \"%s\"�� �����ϴ�",arFind[0].Get(0));
			if (Option.bSoundFindFail) {
				MessageBeep(MB_ICONASTERISK);
			}
			SetStatusText(1,Mes);
		} else {
			if (IsWindow(g_FindDlg)) {
				pSi->Ae.GetSelRect(srt);
				ClientToScreen(pSi->Ae.hWnd,(LPPOINT)&srt);
				ClientToScreen(pSi->Ae.hWnd,(LPPOINT)&srt+1);
				SendMessage(g_FindDlg,WM_USER+1,(WPARAM)&srt,0);
			}
		}
		break;
	case 2:
		if (pSi->Ae.ReplaceText(-1,arFind[0].Get(0), FindFlag,arFind[1].Get(0)) == 0) {
			wsprintf(Mes, "ã�� ���ڿ� \"%s\"�� �����ϴ�",arFind[0].Get(0));
			if (Option.bSoundFindFail) {
				MessageBeep(MB_ICONASTERISK);
			}
			SetStatusText(1,Mes);
		} else {
			pSi->Ae.GetSelRect(srt);
			ClientToScreen(pSi->Ae.hWnd,(LPPOINT)&srt);
			ClientToScreen(pSi->Ae.hWnd,(LPPOINT)&srt+1);
			SendMessage(g_FindDlg,WM_USER+1,(WPARAM)&srt,0);
		}
		break;
	case 3:
		int Result;
		pSi->Ae.StartUndoGroup(6);
		for (i=0;;) {
			tFlag=FindFlag;
			tFlag &= ~AE_FIND_WRAP;
			Result=pSi->Ae.ReplaceText(0,arFind[0].Get(0), tFlag,arFind[1].Get(0));
			UpdateWindow(pSi->Ae.hWnd);
			if (Result==0)
				break;
			if (Result==2)
				i++;
		}
		pSi->Ae.EndUndoGroup();
		if (i==0) {
			lstrcpy(Mes,"ã�� ���ڿ��� �����ϴ�");
		} else {
			wsprintf(Mes, "�� %d���� \"%s\"���ڿ��� \"%s\"�� �ٲپ����ϴ�",
				i,arFind[0].Get(0), arFind[1].Get(0));
		}
		SetStatusText(1,Mes);
		break;
	case 4:
		int toff;
		int s,e;
		LVITEM LI;
		int idx;
		CApiEditText *pView;
		int line,col;
		TCHAR Text[501];

		// ��� �信���� ��� ã�⸦ �� �� ����.
		pView=pSi->Ae.GetActiveView();
		if (pView->ViewType==HEXA_VIEW) {
			break;
		}
		lstrcpy(LastFIF,arFind[0].Get(0));
		TotalFind=0;
		if (bShowOutput==FALSE) {
			bShowOutput=TRUE;
			Relayout();
		}
		SendMessage(hList,LVM_DELETEALLITEMS,0,0);
		tFlag=FindFlag;
		tFlag &= ~AE_FIND_WRAP;
		for (toff=0;;) {
			if (pSi->Ae.FindTextSub(toff,arFind[0].Get(0),tFlag,s,e) == FALSE) {
				break;
			}

			TotalFind++;
			pView->GetParaFromOff(s,line,col);
			pView->GetParaText(line,Text,500);

			LI.mask=LVIF_TEXT | LVIF_PARAM;
			LI.state=0;
			LI.stateMask=0;
			LI.iSubItem=0;
			LI.iItem=ListView_GetItemCount(hList);
			itoa(TotalFind,Mes,10);
			LI.pszText=Mes;
			LI.lParam=(LPARAM)hActive;
			idx=ListView_InsertItem(hList,&LI);

			wsprintf(Mes,"%s(%d,%d)",pSi->NowFile,line+1,col+1);
			ListView_SetItemText(hList,idx,1,Mes);

			ListView_SetItemText(hList,idx,2,Text);
			ListView_EnsureVisible(hList,idx,FALSE);
			UpdateWindow(hList);

			// ���� ���������� �̵�
			toff=e;
		}
		if (TotalFind == 0) {
			wsprintf(Mes, "ã�� ���ڿ� \"%s\"�� �����ϴ�",arFind[0].Get(0));
			if (Option.bSoundFindFail) {
				MessageBeep(MB_ICONASTERISK);
			}
		} else {
			wsprintf(Mes, "�� %d���� \"%s\"���ڿ��� ã�ҽ��ϴ�.",
				TotalFind,arFind[0].Get(0));
		}
		SetStatusText(1,Mes);
		break;
	}
}

BOOL OnSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	if (bShowOutput==FALSE && Option.bShowToolBar==FALSE 
		&& Option.bShowStatus==FALSE && Option.bShowFileWnd==FALSE
		&& Option.bShowFileTab==FALSE && Option.bShowClipWnd==FALSE) {
		return FALSE;
	} else {
		Relayout();
		return TRUE;
	}
}

BOOL OnSetCursor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	POINT pt;
	RECT crt;
	int t,s,f,o,w,ch;

	GetClientRect(g_hFrameWnd,&crt);
	GetChildSize(t,s,f,o,w,ch);

	GetCursorPos(&pt);
	ScreenToClient(hWnd,&pt);

	if (LOWORD(lParam)==HTCLIENT) {
		if ((Option.bShowFileWnd || Option.bShowClipWnd) &&
			pt.x >= w && pt.x <= w+GAP &&
			pt.y > t+f && pt.y < crt.bottom-(o+s)) {
			SetCursor(hCurHorz);
			return TRUE;
		}

		if (bShowOutput &&
			pt.y >= crt.bottom-(o+s+GAP) 
			&& pt.y <= crt.bottom-(o+s)) {
			SetCursor(hCurVert);
			return TRUE;
		}

		if (Option.bShowFileWnd && Option.bShowClipWnd && pt.x < w &&
			pt.y >= crt.bottom-(s+o+(o ? GAP:0)+ch+GAP)  && 
			pt.y <= crt.bottom-(s+o+(o ? GAP:0)+ch)) {
			SetCursor(hCurVert);
			return TRUE;
		}
	}
	return FALSE;
}

void OnLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	POINT pt;
	RECT crt;
	int t,s,f,o,w,ch;

	GetClientRect(g_hFrameWnd,&crt);
	GetChildSize(t,s,f,o,w,ch);

	pt.x=LOWORD(lParam);
	pt.y=HIWORD(lParam);

	iDrag=0;
	if ((Option.bShowFileWnd || Option.bShowClipWnd) && 
		pt.x >= w && pt.x <= w+GAP &&
		pt.y > t+f && pt.y < crt.bottom-(o+s)) {
		iDrag=1;
		DragOriginal=Option.FileWndWidth;
	}

	if (bShowOutput &&
		pt.y >= crt.bottom-(o+s+GAP) 
		&& pt.y <= crt.bottom-(o+s)) {
		iDrag=2;
		DragOriginal=o;
	}

	if (Option.bShowFileWnd && Option.bShowClipWnd && pt.x < w &&
		pt.y >= crt.bottom-(s+o+(o ? GAP:0)+ch+GAP)  && 
		pt.y <= crt.bottom-(s+o+(o ? GAP:0)+ch)) {
		iDrag=3;
		DragOriginal=Option.ClipWndRatio;
	}

	if (iDrag) {
		SetCapture(hWnd);
	}
}

void OnMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	RECT crt;
	int Value;
	int t,s,f,o,w,ch;
	int fs,fh,y;

	if (iDrag==0) {
		return;
	}

	GetClientRect(g_hFrameWnd,&crt);
	GetChildSize(t,s,f,o,w,ch);

	if (iDrag==1) {
		Value=(int)(short)LOWORD(lParam);
		Option.FileWndWidth=min(max(5,Value),crt.right-10);
		Relayout();
	}

	if (iDrag==2) {
		Value=crt.bottom-(int)(short)HIWORD(lParam)-s;
		Option.OutputHeight=min(max(5,Value),
			crt.bottom-(t+f+s)-10);
		Relayout();
	}

	if (iDrag==3) {
		fs=t+f;
		fh=crt.bottom-(t+s+f+o+(o ? GAP:0));
		y=(int)(short)HIWORD(lParam);
		Value=(fh-(y-fs))*10000/fh;
		Option.ClipWndRatio=min(max(500,Value),9500);
		Relayout();
	}
}

void OnLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	if (iDrag==1) {
		if (Option.FileWndWidth == 5) {
			Option.FileWndWidth=DragOriginal;
			Option.bShowFileWnd=FALSE;
			Option.bShowClipWnd=FALSE;
			Relayout();
		}
	}
	if (iDrag==2) {
		if (Option.OutputHeight == 5) {
			Option.OutputHeight=DragOriginal;
			SendMessage(g_hFrameWnd,WM_COMMAND,MAKEWPARAM(IDM_VIEW_OUTPUT,0),0);
		}
	}
	if (iDrag==3) {
		if (Option.ClipWndRatio == 500) {
			Option.ClipWndRatio=DragOriginal;
			SendMessage(g_hFrameWnd,WM_COMMAND,MAKEWPARAM(IDM_VIEW_CLIP,0),0);
		}
		if (Option.ClipWndRatio == 9500) {
			Option.ClipWndRatio=DragOriginal;
			SendMessage(g_hFrameWnd,WM_COMMAND,MAKEWPARAM(IDM_VIEW_FILE,0),0);
		}
	}
	iDrag=0;
	ReleaseCapture();
}

void OnNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPNMHDR hdr;
	LPNMTTDISPINFO disp;
	hdr=(LPNMHDR)lParam;
	disp=(LPNMTTDISPINFO)lParam;

	switch (hdr->code) {
	case TTN_GETDISPINFO:
		switch (wParam) {
		case IDM_FILE_NEW:
			disp->lpszText="�� ����(Ctrl+N)";
			break;
		case IDM_FILE_OPEN:
			disp->lpszText="���� ����(Ctrl+O)";
			break;
		case IDM_FILE_SAVE:
			disp->lpszText="���� ����(Ctrl+S)";
			break;
		case IDM_EDIT_CUT:
			disp->lpszText="�߶󳻱�(Ctrl+X)";
			break;
		case IDM_EDIT_COPY:
			disp->lpszText="����(Ctrl+C)";
			break;
		case IDM_EDIT_PASTE:
			disp->lpszText="�ٿ��ֱ�(Ctrl+V)";
			break;
		case IDM_EDIT_UNDO:
			disp->lpszText="�������(Ctrl+Z)";
			break;
		case IDM_EDIT_REDO:
			disp->lpszText="�����(Ctrl+Y)";
			break;
		case IDM_VIEW_WORDWRAP:
			disp->lpszText="�ڵ� ����(Ctrl+W)";
			break;
		case IDM_VIEW_HEXA:
			disp->lpszText="����(Ctrl+H)";
			break;
		case IDM_EDIT_COLUMN:
			disp->lpszText="�����(Alt+C)";
			break;
		case IDM_EDIT_COMMODE:
			disp->lpszText="��ɸ��(Alt+X)";
			break;
		case IDM_SEARCH_FIND:
			disp->lpszText="ã��(Ctrl+F)";
			break;
		}
		break;
	case TBN_DROPDOWN:
		hEdRecDlg=CreateDialogParam(g_hInst, MAKEINTRESOURCE(IDD_EDITRECORD),
			hWnd, (DLGPROC)EditRecordProc,((NMTOOLBAR *)lParam)->iItem);
		break;
	}
}

void OnIdle()
{
	HWND hActive;
	SInfo *pSi;
	int s,e;
	CApiEditText *pView;

	hActive=(HWND)SendMessage(g_hMDIClient,WM_MDIGETACTIVE,0,NULL);
	if (hActive) {
		pSi=(SInfo *)GetWindowLongPtr(hActive,0);
		pView=pSi->Ae.GetActiveView();
		// ���� ��ư�� ������ ������ ���� ���¸� �����Ѵ�.
		if ((SendMessage(hToolBar,TB_GETSTATE,IDM_FILE_SAVE,0) & TBSTATE_ENABLED)==0) {
			SendMessage(hToolBar,TB_SETSTATE,IDM_FILE_SAVE,MAKELONG(TBSTATE_ENABLED,0));
		}

		pSi->Ae.GetSelect(s,e);
		if (s == e) {
			SendMessage(hToolBar,TB_SETSTATE,IDM_EDIT_CUT,MAKELONG(0,0));
			SendMessage(hToolBar,TB_SETSTATE,IDM_EDIT_COPY,MAKELONG(0,0));
		} else {
			SendMessage(hToolBar,TB_SETSTATE,IDM_EDIT_CUT,MAKELONG(TBSTATE_ENABLED,0));
			SendMessage(hToolBar,TB_SETSTATE,IDM_EDIT_COPY,MAKELONG(TBSTATE_ENABLED,0));
		}

		if (IsClipboardFormatAvailable(CF_TEXT)==FALSE) {
			SendMessage(hToolBar,TB_SETSTATE,IDM_EDIT_PASTE,MAKELONG(0,0));
		} else {
			SendMessage(hToolBar,TB_SETSTATE,IDM_EDIT_PASTE,MAKELONG(TBSTATE_ENABLED,0));
		}

		if (pSi->Ae.GetWrap() != 0) {
			SendMessage(hToolBar,TB_SETSTATE,IDM_VIEW_WORDWRAP,
				MAKELONG(TBSTATE_ENABLED | TBSTATE_CHECKED,0));
		} else {
			SendMessage(hToolBar,TB_SETSTATE,IDM_VIEW_WORDWRAP,MAKELONG(TBSTATE_ENABLED,0));
		}

		if (pSi->Ae.CanUndo()) {
			SendMessage(hToolBar,TB_SETSTATE,IDM_EDIT_UNDO,MAKELONG(TBSTATE_ENABLED,0));
		} else {
			SendMessage(hToolBar,TB_SETSTATE,IDM_EDIT_UNDO,MAKELONG(0,0));
		}
		if (pSi->Ae.CanRedo()) {
			SendMessage(hToolBar,TB_SETSTATE,IDM_EDIT_REDO,MAKELONG(TBSTATE_ENABLED,0));
		} else {
			SendMessage(hToolBar,TB_SETSTATE,IDM_EDIT_REDO,MAKELONG(0,0));
		}

		SendMessage(hToolBar,TB_SETSTATE,IDM_SEARCH_FIND,MAKELONG(TBSTATE_ENABLED,0));
		if (pView->ViewType == HEXA_VIEW) {
			SendMessage(hToolBar,TB_SETSTATE,IDM_VIEW_HEXA,
				MAKELONG(TBSTATE_ENABLED | TBSTATE_CHECKED,0));
		} else {
			SendMessage(hToolBar,TB_SETSTATE,IDM_VIEW_HEXA,MAKELONG(TBSTATE_ENABLED,0));
		}
		if (pView->GetColumnMode()) {
			SendMessage(hToolBar,TB_SETSTATE,IDM_EDIT_COLUMN,
				MAKELONG(TBSTATE_ENABLED | TBSTATE_CHECKED,0));
		} else {
			SendMessage(hToolBar,TB_SETSTATE,IDM_EDIT_COLUMN,MAKELONG(TBSTATE_ENABLED,0));
		}
		if (bComMode) {
			SendMessage(hToolBar,TB_SETSTATE,IDM_EDIT_COMMODE,
				MAKELONG(TBSTATE_ENABLED | TBSTATE_CHECKED,0));
		} else {
			SendMessage(hToolBar,TB_SETSTATE,IDM_EDIT_COMMODE,MAKELONG(TBSTATE_ENABLED,0));
		}
		SetStatusText(0xcc0);
	} else {
		SendMessage(hToolBar,TB_SETSTATE,IDM_FILE_SAVE,MAKELONG(0,0));
		SendMessage(hToolBar,TB_SETSTATE,IDM_EDIT_CUT,MAKELONG(0,0));
		SendMessage(hToolBar,TB_SETSTATE,IDM_EDIT_COPY,MAKELONG(0,0));
		SendMessage(hToolBar,TB_SETSTATE,IDM_EDIT_PASTE,MAKELONG(0,0));
		SendMessage(hToolBar,TB_SETSTATE,IDM_EDIT_UNDO,MAKELONG(0,0));
		SendMessage(hToolBar,TB_SETSTATE,IDM_EDIT_REDO,MAKELONG(0,0));
		SendMessage(hToolBar,TB_SETSTATE,IDM_VIEW_WORDWRAP,MAKELONG(0,0));
		SendMessage(hToolBar,TB_SETSTATE,IDM_SEARCH_FIND,MAKELONG(0,0));
		SendMessage(hToolBar,TB_SETSTATE,IDM_VIEW_HEXA,MAKELONG(0,0));
		SendMessage(hToolBar,TB_SETSTATE,IDM_EDIT_COLUMN,MAKELONG(0,0));
		SendMessage(hToolBar,TB_SETSTATE,IDM_EDIT_COMMODE,MAKELONG(0,0));
	}
}

void OnSysColorChange(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hChild;
	SInfo *pSi;

	SendMessage(hToolBar,WM_SYSCOLORCHANGE,wParam,lParam);
	SendMessage(hStatus,WM_SYSCOLORCHANGE,wParam,lParam);
	SendMessage(hFileTree,WM_SYSCOLORCHANGE,wParam,lParam);
	SendMessage(hFileTab,WM_SYSCOLORCHANGE,wParam,lParam);
	SendMessage(hList,WM_SYSCOLORCHANGE,wParam,lParam);
	InitSysColor();

	hChild=GetWindow(g_hMDIClient,GW_CHILD);
	while (hChild) {
		pSi=(SInfo *)GetWindowLongPtr(hChild,0);
		if ((Option.Color[AECOLOR_FORE] & 0xff000000) == 0) {
			pSi->Ae.SetColor(AECOLOR_FORE,-1);
		}
		if ((Option.Color[AECOLOR_BACK] & 0xff000000) == 0) {
			pSi->Ae.SetColor(AECOLOR_BACK,-1);
		}
		if ((Option.Color[AECOLOR_SELFORE] & 0xff000000) == 0) {
			pSi->Ae.SetColor(AECOLOR_SELFORE,-1);
		}
		if ((Option.Color[AECOLOR_SELBACK] & 0xff000000) == 0) {
			pSi->Ae.SetColor(AECOLOR_SELBACK,-1);
		}
		if ((Option.Color[AECOLOR_INNERRECT] & 0xff000000) == 0) {
			pSi->Ae.SetColor(AECOLOR_INNERRECT,-1);
		}
		hChild=GetWindow(hChild,GW_HWNDNEXT);
	}
}

void OnUser3(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	SInfo *pSi;
	HWND hActive;

	hActive=(HWND)SendMessage(g_hMDIClient, WM_MDIGETACTIVE,0,NULL);
	if (hActive) {
		pSi=(SInfo *)GetWindowLong(hActive,0);
		if (wParam == TRUE) {
			pSi->Ae.MultiUndo(lParam);
		} else {
			pSi->Ae.MultiRedo(lParam);
		}
	}
}

// ��ũ�� ����. wParam�� ȸ��, lParam�� ��ũ�� �̸�
void OnUser4(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int i,count;
	HWND hDlg;
	MSG msg;

	count=(int)wParam;

	if (count > 1) {
		hDlg=CreateDialogParam(g_hInst, MAKEINTRESOURCE(IDD_RUNMACRO), 
			hWnd, (DLGPROC)RunMacroDlgProc,lParam);
		ShowWindow(hDlg,SW_SHOW);
		EnableWindow(g_hFrameWnd,FALSE);
	}

	for (i=0;i<count;i++) {
		ExecMacro((const TCHAR *)lParam);
		if (count > 1) {
			SendMessage(hDlg,WM_USER+1,count,i);
			while (PeekMessage(&msg, NULL,0,0,PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if (!IsWindow(hDlg)) {
				break;
			}
		}
	}

	if (count > 1) {
		EnableWindow(g_hFrameWnd,TRUE);
		if (IsWindow(hDlg)) {
			DestroyWindow(hDlg);
		}
	}
}

LRESULT CALLBACK DGChildProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	SInfo *pSi;
	int idx;
	HWND hActive;
	BOOL bMax;
	RECT crt,art;
	CClipRecord *cr;

	pSi=(SInfo *)GetWindowLongPtr(hWnd,0);
	if (pSi && iMessage > WM_USER) {
		// ���� ������ �����϶�.
		if (iMessage == pSi->Ae.GetUserBase()+1) {
			SetSettingView((CApiEditText *)lParam);
			return 0;
		}
		// ��ũ�� ����� �����϶�.
		if (iMessage == pSi->Ae.GetUserBase()+2) {
			return DoCommand((TCHAR *)lParam);
		}
		// lParam���� ���޵� ���Ը��� ��뱸�� �����϶�.
		if (iMessage == pSi->Ae.GetUserBase()+3) {
			cr=FindClipRecord((TCHAR *)lParam,TRUE);
			if (cr) {
				hActive=(HWND)SendMessage(g_hMDIClient, WM_MDIGETACTIVE,0,NULL);
				pSi=(SInfo *)GetWindowLong(hActive,0);
				pSi->Ae.GetActiveView()->ReplaceClipText(cr->Content);
				return 1;
			}
			return 0;
		}
		// ȣ��Ʈ�� �˾� �׸��� ����϶�.
		if (iMessage == pSi->Ae.GetUserBase()+4) {
			InsertMenu((HMENU)wParam,0,MF_BYPOSITION | MF_STRING,0,"�ݱ�(&Q)");
			InsertMenu((HMENU)wParam,1,MF_BYPOSITION | MF_SEPARATOR,0,NULL);
			AppendMenu((HMENU)wParam,MF_SEPARATOR,0,NULL);
			AppendMenu((HMENU)wParam,MF_STRING,1,"��ġ ã��");
			return 0;
		}
		// wParam���� ���޵� ȣ��Ʈ�� �˾� �׸��� �����϶�.
		if (iMessage == pSi->Ae.GetUserBase()+5) {
			switch (wParam) {
			case 0:
				PostMessage(hWnd,WM_CLOSE,0,0);
				break;
			case 1:
				SInfo *pSi;
				pSi=(SInfo *)GetWindowLongPtr(hWnd,0);
				if (strncmp(pSi->NowFile,"�̸�����",8) != 0) {
					TCHAR drive[MAX_PATH];
					TCHAR dir[_MAX_DIR];

					_splitpath(pSi->NowFile, drive, dir, NULL, NULL);
					lstrcat(drive, dir);
					GotoFolderInTree(drive);

					if (Option.bShowFileWnd == FALSE) {
						Option.bShowFileWnd = TRUE;
						Relayout();
					}
				}
				break;
			}
		}
		// lParam���� ���޵� �м����� ������ �����϶�.
		if (iMessage == pSi->Ae.GetUserBase()+6) {
			Option.SetStyleColor((CParse *)lParam);
			return 0;
		}
	}

	switch(iMessage) {
	case WM_CREATE:
		g_ChildNum++;
		if (g_ChildNum == 1) {
			SendMessage(g_hMDIClient,WM_MDISETMENU,(WPARAM)hMenu1,
				(LPARAM)GetSubMenu(hMenu1,5));
			Mru.ChangeMenu(GetSubMenu(GetSubMenu(hMenu1,0),17));
			DrawMenuBar(g_hFrameWnd);
			if (Option.bShowStatus) {
				SetStatusPart();
			}
		}
		pSi=new SInfo;
		SetWindowLongPtr(hWnd,0,(LONG_PTR)pSi);
		pSi->Ae.Create(0,0,0,0,WS_CHILD | WS_VISIBLE,1,hWnd);
		SetSetting(pSi->Ae);
		wsprintf(pSi->NowFile,"");
		return 0;
	case WM_CLOSE:
		if (ConfirmSave(hWnd) == IDCANCEL) {
			return 1;
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 1:
			switch (HIWORD(wParam)) {
			case AEN_MOVE:
				SetStatusText(0x302);
				break;
			case AEN_CHANGE:
				SetStatusText(0x1c);
				break;
			case AEN_INSMODE:
				SetStatusText(0x20);
				break;
			case AEN_CHGMODI:
				SetStatusText(0x8);
				break;
			case AEN_CHGVIEW:
				SetStatusText(0xffff);
			}
			break;
		}
		return 0;
	case WM_SIZE:
		hActive=(HWND)SendMessage(g_hMDIClient, WM_MDIGETACTIVE,0,(LPARAM)&bMax);
		pSi=(SInfo *)GetWindowLongPtr(hWnd,0);
		if (bMax==FALSE || (hActive == hWnd && NewPhase==FALSE)) {
			MoveWindow(pSi->Ae.hWnd,0,0,LOWORD(lParam),HIWORD(lParam),TRUE);
		}
		break;
	case WM_SETFOCUS:
		GetClientRect(hWnd,&crt);
		GetWindowRect(pSi->Ae.hWnd,&art);
		if (art.right-art.left != crt.right-crt.left) {
			MoveWindow(pSi->Ae.hWnd,0,0,crt.right,crt.bottom,TRUE);
		}
		SetFocus(pSi->Ae.hWnd);
		SetStatusText(0xffff);
		idx=FindFileTab(pSi->NowFile);
		TabCtrl_SetCurSel(hFileTab,idx);
		return 0;
	case WM_DESTROY:
		// ���� ������ �����Ѵ�.
		if (Option.bMakeEditInfo && Option.bUpdateEditInfo &&
			strncmp(pSi->NowFile,"�̸�����",8) &&
			pSi->Ae.GetModified()==FALSE) {
			UpdateEditInfo(pSi->Ae,pSi->NowFile);
		}
		DeleteFileTab(pSi->NowFile);
		DestroyWindow(pSi->Ae.hWnd);
		delete pSi;
		g_ChildNum--;
		if (g_ChildNum==0) {
			SendMessage(g_hMDIClient,WM_MDISETMENU,(WPARAM)hMenu2,NULL);
			Mru.ChangeMenu(GetSubMenu(GetSubMenu(hMenu2,0),7));
			DrawMenuBar(g_hFrameWnd);
			if (Option.bShowStatus) {
				SetStatusPart();
			}
		}
		return 0;
	}
	return(DefMDIChildProc(hWnd,iMessage,wParam,lParam));
}

BOOL CALLBACK AboutProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	SYSTEMTIME st;
	DWORD Day,Today;;
	TCHAR szDate[256];

	switch(iMessage) {
	case WM_INITDIALOG:
		if (FALSE) {
		} else {
			SetDlgItemText(hDlg,IDC_ABOUT_USER,"��ϵ��� �ʾҽ��ϴ�");
			GetLocalTime(&st);
			Today=MyGetAbsDay(st);
			Day=SHRegReadInt(SHCU,"Software\\Miyoungsoft","DgInstDay120",0);
			MyAbsToSystem(Day,st);
			wsprintf(szDate,"���� ������ ��¥:%d�� %d�� %d��(���� ��¥:%d��)",
				st.wYear,st.wMonth,st.wDay,Day+365-Today);
			SetDlgItemText(hDlg,IDC_ABOUT_DATE,szDate);
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			EndDialog(hDlg,IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg,IDCANCEL);
			return TRUE;
		case IDC_ABOUT_REGISTER:
			MessageBox(hDlg,"1.2x ������ ����� ���� �ʽ��ϴ�. �׳� ���ø� �˴ϴ�. ���� ","�˸�",MB_OK);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

HWND New()
{
	MDICREATESTRUCT mcs;
	HWND hActive, hChild;
	BOOL bMax;

	hActive=(HWND)SendMessage(g_hMDIClient, WM_MDIGETACTIVE,0,(LPARAM)&bMax);

	mcs.szClass="DGChild";
	mcs.szTitle="";
	mcs.hOwner=g_hInst;
	mcs.x=mcs.y=CW_USEDEFAULT;
	mcs.cx=mcs.cy=CW_USEDEFAULT;
	mcs.style=MDIS_ALLCHILDSTYLES | WS_CLIPCHILDREN;
	if ((hActive == NULL && Option.bMaxFirstChild==TRUE) || bMax == TRUE) {
		 mcs.style |= WS_MAXIMIZE;
	}
	if (hActive && bMax==TRUE) {
		NewPhase=TRUE;
	} else {
		NewPhase=FALSE;
	}
	hChild=(HWND)SendMessage(g_hMDIClient, WM_MDICREATE, 0, 
		(LPARAM)(LPMDICREATESTRUCT)&mcs);
	NewPhase=FALSE;
	return hChild;
}

int ConfirmSave(HWND hChild)
{
	int result=IDNO;
	SInfo *pSi=(SInfo *)GetWindowLongPtr(hChild,0);
	TCHAR Mes[MAX_PATH+64];

	if (pSi->Ae.GetModified()) {
		wsprintf(Mes,"%s ������ ����Ǿ����ϴ�. �����Ͻðڽ��ϱ�?",pSi->NowFile);
		result=MessageBox(g_hFrameWnd,Mes,"�˸�",MB_YESNOCANCEL);
		if (result == IDCANCEL) {
			return IDCANCEL;
		}
		if (result == IDYES) {
			if (Save(hChild) == FALSE)
				return IDCANCEL;
			else 
				return IDYES;
		}
	}
	return result;
}

HWND FindChildWithFile(TCHAR *path)
{
	HWND hChild;
	SInfo *pSi;

	hChild=GetWindow(g_hMDIClient,GW_CHILD);
	while (hChild) {
		pSi=(SInfo *)GetWindowLongPtr(hChild,0);
		if (lstrcmp(pSi->NowFile,path)==0) {
			return hChild;
		}
		hChild=GetWindow(hChild,GW_HWNDNEXT);
	}
	return NULL;
}

void Open()
{
	OPENFILENAME OFN;
	TCHAR *lpstrFile;
	TCHAR Dir[MAX_PATH];
	TCHAR Path[MAX_PATH];
	TCHAR *p;

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner=g_hFrameWnd;
	OFN.lpstrFilter="��� ����(*.*)\0*.*\0�ؽ�Ʈ ����\0*.txt\0��� ������Ʈ(*.dgp)"
		"\0*.dgp*\0��� ��뱸 ����(*.dgc)\0*.dgc";
	lpstrFile=(TCHAR *)malloc(100000);
	lpstrFile[0]=0;
	OFN.lpstrFile=lpstrFile;
	OFN.nMaxFile=100000;
	OFN.Flags=OFN_EXPLORER | OFN_ALLOWMULTISELECT;

	if (GetOpenFileName(&OFN)) {
		p=lpstrFile;
		lstrcpy(Dir,p);
		p=p+lstrlen(Dir)+1;
		if (*p==0) {
			OpenFromFile(Dir, (OFN.Flags & OFN_READONLY));
		} else {
			for (;*p;) {
				wsprintf(Path,"%s\\%s",Dir,p);
				p=p+lstrlen(p)+1;
				OpenFromFile(Path, (OFN.Flags & OFN_READONLY));
			}
		}
	} else {
		if (CommDlgExtendedError()==FNERR_BUFFERTOOSMALL) {
			MessageBox(g_hFrameWnd,"�� ���� ������ �ʹ� ���� �����ϼ̽��ϴ�.",
				"�˸�",MB_OK);
		}
	}
	free(lpstrFile);
}

BOOL OpenFromFile(TCHAR *Path, BOOL bReadOnly/*=FALSE*/, BOOL bBrowse/*=FALSE*/)
{
	HWND hChild;
	SInfo *pSi;
	BOOL bNew=TRUE;
	TCHAR Mes[512];
	int TabIdx;
	TCHAR ext[_MAX_EXT];
	TCHAR Path2[MAX_PATH];

	_splitpath(Path,NULL,NULL,NULL,ext);
	if (stricmp(ext,".lnk")==0) {
		if (ResolveShortCut(Path,Path2)==FALSE) {
			return FALSE;
		}
	} else {
		lstrcpy(Path2,Path);
	}

	_splitpath(Path2,NULL,NULL,NULL,ext);
	if (stricmp(ext,".dgp")==0) {
		if (Option.bShowFileWnd==FALSE) {
			Option.bShowFileWnd=TRUE;
			Relayout();
		}
		LoadProject(Path2,FALSE);
		return TRUE;
	}

	if (stricmp(ext,".dgc")==0) {
		if (Option.bShowClipWnd==FALSE) {
			Option.bShowClipWnd=TRUE;
			Relayout();
		}
		LoadClipFile(Path2,FALSE);
		return TRUE;
	}

	hChild=FindChildWithFile(Path2);
	if (hChild) {
		SendMessage(g_hMDIClient,WM_MDIACTIVATE,(WPARAM)hChild,0);
		return TRUE;
	}

	if (!bBrowse) {
		if (g_ChildNum==1) {
			bNew=FALSE;
			hChild=GetWindow(g_hMDIClient,GW_CHILD);
			pSi=(SInfo *)GetWindowLongPtr(hChild,0);
			if (pSi->Ae.GetModified() || strncmp(pSi->NowFile,"�̸�����",8)) {
				bNew=TRUE;
			}
		}
	} else {
		bNew=FALSE;
		hChild=(HWND)SendMessage(g_hMDIClient,WM_MDIGETACTIVE,0,NULL);
		if (hChild==NULL) {
			bNew=TRUE;
		} else {
			pSi=(SInfo *)GetWindowLongPtr(hChild,0);
			if (pSi->Ae.GetModified()) {
				bNew=TRUE;
			}
		}
	}
	
	if (bNew) {
		hChild=New();
		pSi=(SInfo *)GetWindowLongPtr(hChild,0);
	} else {
		TabIdx=FindFileTab(pSi->NowFile);
	}

	if (OpenFileToChild(hChild,Path2)==FALSE) {
		if (bNew) {
			SendMessage(g_hMDIClient,WM_MDIDESTROY,(WPARAM)hChild,0);
		}
		if (strnicmp(Path2,"http",4) != 0 && strnicmp(Path2,"ftp",3) != 0) {
			wsprintf(Mes,"%s ������ �� �� �����ϴ�. "
				"�ٸ� ���α׷��� ��������� Ȯ���Ͻʽÿ�.",Path2);
			MessageBox(g_hFrameWnd, Mes,"�˸�",MB_OK);
		}
		return FALSE;
	}
	if (strnicmp(Path2,"http",4)!=0 && strnicmp(Path2,"ftp",3)!=0) {
		if (bReadOnly || (GetFileAttributes(Path2) & FILE_ATTRIBUTE_READONLY)) {
			pSi->Ae.SetReadOnly(TRUE);
		}
	}
	
	if (!bBrowse) {
		Mru.AddMRU(Path2);
	}
	if (bNew) {
		AddFileTab(Path2);
	} else {
		ChangeFileTab(TabIdx,Path2);
	}

	return TRUE;
}

BOOL OpenFileToChild(HWND hChild, TCHAR *Path)
{
	SInfo *pSi;
	HANDLE hFile;
	DWORD dwRead,dwSize;
	TCHAR *TextBuf;
	TCHAR EncPath[MAX_PATH];

	pSi=(SInfo *)GetWindowLongPtr(hChild,0);

	SelectParser(pSi->Ae,Path);
	pSi->Ae.InitDoc();

	if (strnicmp(Path,"http",4) == 0) {
		dwSize=DgHttpDown(Path,TextBuf);
		if (dwSize==-1) {
			return FALSE;
		}
	} else if (strnicmp(Path,"ftp",3) == 0) {
		dwSize=DgFtpDown(Path,TextBuf);
		if (dwSize==-1) {
			return FALSE;
		}
	} else {
		hFile=CreateFile(Path,GENERIC_READ,0,NULL,
			OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if (hFile==INVALID_HANDLE_VALUE) {
			return FALSE;
		}
		dwSize=GetFileSize(hFile,NULL);
		if (dwSize > 30*1048576) {
			if (MessageBox(g_hFrameWnd, "�� ������ ũ��� 30M�� �ѽ��ϴ�. ������ ���ðڽ��ϱ�?",
				"����",MB_YESNO)==IDNO) {
				CloseHandle(hFile);
				return FALSE;
			}
		}
		TextBuf=(TCHAR *)malloc(dwSize);
		ReadFile(hFile,TextBuf,dwSize,&dwRead,NULL);
		CloseHandle(hFile);
	}
	pSi->Ae.SetText(TextBuf,dwSize);
	free(TextBuf);

	pSi->Ae.SetModified(FALSE);
	lstrcpy(pSi->NowFile,Path);
	EncodePath(Path,EncPath);
	SetWindowText(hChild,EncPath);

	// ���� ������ �о� �����Ѵ�.
	if (Option.bUseEditInfo) {
		LoadEditInfo(pSi->Ae,Path);
	}
	SetStatusText(0xffff);
	return TRUE;
}

BOOL Save(HWND hChild)
{
	SInfo *pSi=(SInfo *)GetWindowLongPtr(hChild,0);

	if (strncmp(pSi->NowFile,"�̸�����",8)==0) {
		return SaveAs(hChild);
	} else {
		if (pSi->Ae.GetModified() == FALSE) {
			return TRUE;
		}
		return SaveToFile(hChild,pSi->NowFile);
	}
}

BOOL SaveAs(HWND hChild)
{
	OPENFILENAME OFN;
	TCHAR lpstrFile[MAX_PATH]="";
	int TabIdx;
	TCHAR EncPath[MAX_PATH];

	SInfo *pSi=(SInfo *)GetWindowLongPtr(hChild,0);
	TabIdx=FindFileTab(pSi->NowFile);
	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner=g_hFrameWnd;
	OFN.lpstrFilter="��� ����(*.*)\0*.*\0�ؽ�Ʈ ����\0*.txt\0";
	OFN.lpstrFile=lpstrFile;
	OFN.nMaxFile=256;
	OFN.lpstrDefExt=Option.DefExt;
	if (GetSaveFileName(&OFN)==FALSE) {
		return FALSE;
	}

	if (_access(OFN.lpstrFile,0)==0) {
		if (MessageBox(g_hFrameWnd, "������ �̹� �ֽ��ϴ�. ����ðڽ��ϱ�?",
			"�˸�",MB_YESNO)==IDNO)
			return FALSE;
	}

	if (SaveToFile(hChild,OFN.lpstrFile) == FALSE) { 
		return FALSE;
	}

	SelectParser(pSi->Ae,OFN.lpstrFile);
	Mru.AddMRU(OFN.lpstrFile);
	lstrcpy(pSi->NowFile,OFN.lpstrFile);
	EncodePath(pSi->NowFile,EncPath);
	SetWindowText(hChild,EncPath);
	
	ChangeFileTab(TabIdx,pSi->NowFile);
	return TRUE;
}

BOOL SaveToFile(HWND hChild,TCHAR *Path)
{
	HANDLE hFile;
	DWORD dwWritten;
	int len;
	TCHAR *TextBuf;
	SInfo *pSi=(SInfo *)GetWindowLongPtr(hChild,0);
	DWORD err;
	TCHAR Mes[512];
	BOOL Result;
	TCITEM tie;
	int idx;

	if (strnicmp(Path,"http",4)==0) {
		MessageBox(hChild, "HTTP ���������� ���ε带 �������� �����Ƿ� �� �̸����δ� "
			"������ �� �����ϴ�. �� �̸����� �ϵ� ��ũ�� �����Ͻʽÿ�","�˸�",MB_OK);
		return FALSE;
	}

	MakeBackup(Path);
	len=pSi->Ae.GetTextLength();
	TextBuf=(TCHAR *)malloc(len);
	pSi->Ae.GetText(TextBuf,len);

	if (strnicmp(Path,"ftp",3)==0) {
		Result=DgFtpUp(Path,TextBuf);
	} else {
		hFile=CreateFile(Path,GENERIC_WRITE,0,NULL,
			CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if (hFile==INVALID_HANDLE_VALUE) {
			err=GetLastError();
			switch (err) {
			case ERROR_ACCESS_DENIED:
				wsprintf(Mes,"%s ������ ������ �� �����ϴ�. �б� ���� ���� ������ "
					"�׼����� �����Ǿ� �ֽ��ϴ�.",Path);
				break;
			case ERROR_SHARING_VIOLATION:
				wsprintf(Mes,"%s ������ �ٸ� ���α׷��� ����ϰ� �ֽ��ϴ�.",Path);
				break;
			case ERROR_WRITE_PROTECT:
				wsprintf(Mes,"��� ������ ��ġ�Դϴ�.");
				break;
			default:
				wsprintf(Mes,"�˼� ���� �����Դϴ�. �����ڿ��� �������� ���ð� "
					"�˾Ƽ� �ذ��Ͻʽÿ�. ���� �ڵ�=%d",err);
				break;
			}
			MessageBox(g_hFrameWnd, Mes,"�˸�",MB_OK);
			Result=FALSE;
		} else {
			WriteFile(hFile,TextBuf,len,&dwWritten,NULL);
			CloseHandle(hFile);
			Result=TRUE;
		}
	}

	if (Result) {
		lstrcpy(pSi->NowFile,Path);
		pSi->Ae.SetModified(FALSE);
		// ���� ���� ������ �ۼ��Ѵ�.
		if (Option.bMakeEditInfo) {
			SaveEditInfo(pSi->Ae,Path);
		}
		tie.mask=TCIF_IMAGE;
		tie.iImage=0;
		idx=FindFileTab(pSi->NowFile);
		TabCtrl_SetItem(hFileTab,idx,&tie);
	}
	free(TextBuf);
	return Result;
}

BOOL TestNeedActive(WORD ID)
{
	static WORD IDs[]={ IDM_FILE_NEW,IDM_FILE_OPEN,IDM_FILE_EXIT,IDM_FILE_PROJECT,
		IDM_FILE_FTPOPEN, IDM_FILE_HTTPOPEN, IDM_VIEW_OUTPUT,
		IDM_VIEW_TOOLBAR,IDM_VIEW_STATUS,IDM_VIEW_FILE,IDM_VIEW_FILETAB,
		IDM_VIEW_CLIP, IDM_SEARCH_FILES,IDM_SEARCH_RFILES,IDM_TOOL_CONFIG,
		IDM_TOOL_UNINSTALL,IDM_TOOL_MACRO,IDM_TOOL_SHORTCUT,IDM_TOOL_CLIP,
		IDM_HELP_ABOUT,IDM_HELP_INDEX,IDM_HELP_HOME };
	int i;

	for (i=0;i<sizeof(IDs)/sizeof(IDs[0]);i++) {
		if (ID==IDs[i])
			return FALSE;
	}
	if (ID >= Mru.MenuID && ID < Mru.MenuID+Mru.MaxMru) {
		return FALSE;
	}
	return TRUE;
}

void RevertFile(HWND hChild)
{
	SInfo *pSi;
	TCITEM tie;
	int idx;

	pSi=(SInfo *)GetWindowLong(hChild,0);
	OpenFileToChild(hChild,pSi->NowFile);
	if (GetFileAttributes(pSi->NowFile) & FILE_ATTRIBUTE_READONLY) {
		pSi->Ae.SetReadOnly(TRUE);
	}
	tie.mask=TCIF_IMAGE;
	tie.iImage=0;
	idx=FindFileTab(pSi->NowFile);
	TabCtrl_SetItem(hFileTab,idx,&tie);
}

void SaveAll()
{
	HWND hChild;
	hChild=GetWindow(g_hMDIClient,GW_CHILD);
	while (hChild) {
		if (Save(hChild)==FALSE) {
			return;
		}
		hChild=GetWindow(hChild,GW_HWNDNEXT);
	}
}

void CloseAll()
{
	HWND hChild, hChild2;

	hChild=GetWindow(g_hMDIClient,GW_CHILD);
	while (hChild) {
		hChild2=GetWindow(hChild,GW_HWNDNEXT);
		if (SendMessage(hChild,WM_CLOSE,0,0)==1) {
			return;
		}
		hChild=hChild2;
	}
}

void RegisterPopup(BOOL bReg,BOOL bDelete/*=FALSE*/)
{
	TCHAR DllPath[MAX_PATH];
	HRSRC hRSrc;
	DWORD Size;
	HGLOBAL hMem;
	PVOID ptr;
	HANDLE hFile;
	DWORD dwWritten;
	OSVERSIONINFO osv;
	TCHAR Pending[1024],*p;

	GetSystemDirectory(DllPath,MAX_PATH);
	lstrcat(DllPath,"\\DgCmd.dll");
	osv.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	GetVersionEx(&osv);
	if (bReg) {
		// DLL ������ ������ ���ҽ����� �����ؼ� �����.
		if (_access(DllPath,0) != 0) {
			hRSrc=FindResource(g_hInst,MAKEINTRESOURCE(IDR_DLL1),"DLL");
			Size=SizeofResource(g_hInst,hRSrc);
			hMem=LoadResource(g_hInst,hRSrc);
			ptr=LockResource(hMem);
			hFile=CreateFile(DllPath,GENERIC_WRITE,0,NULL,
				CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
			WriteFile(hFile,ptr,Size,&dwWritten,NULL);
			CloseHandle(hFile);
		}
		// ��� �� �缳ġ�� �� ���� ��ϵ� ������ ����Ѵ�.
		if (osv.dwPlatformId == VER_PLATFORM_WIN32_NT) {
			SHRegReadString(SHLM,"SYSTEM\\CurrentControlSet\\Control\\Session Manager",
				"PendingFileRenameOperations","",Pending,1024);
			p=strstr(Pending,"DgCmd.dll");
			if (p) {
				memcpy(p,"zzzzz.zzz",9);
			}
			SHRegWriteString(SHLM,"SYSTEM\\CurrentControlSet\\Control\\Session Manager",
				"PendingFileRenameOperations",Pending);
		} else {
			WritePrivateProfileString("rename","NUL",NULL,"Wininit.ini");
		}

		RegisterSvr(DllPath,TRUE);
	} else {
		RegisterSvr(DllPath,FALSE);
		if (bDelete) {
			if (osv.dwPlatformId == VER_PLATFORM_WIN32_NT) {
				MoveFileEx(DllPath, NULL, MOVEFILE_DELAY_UNTIL_REBOOT | 
					MOVEFILE_REPLACE_EXISTING);
			} else {
				WritePrivateProfileString("rename","NUL",DllPath,"Wininit.ini");
			}
		}
	}
}

void ActivateBrood()
{
	HWND hBrood;
	ATOM hAtom;

	hBrood=FindWindow(lpszClass,NULL);
	if (hBrood==NULL) {
		return;
	}

	if (__argc >1 ) {
		hAtom=GlobalAddAtom(__argv[1]);
		SendMessage(hBrood,WM_USER+1,(WPARAM)hAtom,0);
		GlobalDeleteAtom(hAtom);
	} else {
		SendMessage(hBrood,WM_USER+1,0,0);
	}

	SetForegroundWindow(hBrood);
}

BOOL CALLBACK SetupDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	TCHAR szPath[MAX_PATH];
	TCHAR szLink[MAX_PATH];
	SYSTEMTIME st;
	DWORD Day;

	switch(iMessage) {
	case WM_INITDIALOG:
		SetDlgItemText(hDlg,IDC_IS_MESSAGE,"����� �ؽ�Ʈ�� ���� ������ ��"
			" �ִ� �ؽ�Ʈ ���� ���α׷��Դϴ�.\r\n"
			"ó�� �����ϼ����Ƿ� ���� �ʱ� ������ ������ �ֽʽÿ�.�� ��ȭ���ڴ� ���� "
			"����� �� ���� ��Ÿ���� ���ĺ��� ���� �޴��� �⺻���� ������� ��� �ɼ���"
			" ������ �� �ֽ��ϴ�. �⺻���� �־��� �ɼ��� ���� �����մϴ�.\r\n"
			"�� ���α׷��� ���� ���� �� 1�⵿�� ��� ������� ����� �� �ִ� �ξ���� "
			"�Դϴ�.\r\n\r\n�����մϴ�.");
		CheckDlgButton(hDlg,IDC_IS_POPUP,BST_CHECKED);
		CheckDlgButton(hDlg,IDC_IS_ASSODGP,BST_CHECKED);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			if (IsDlgButtonChecked(hDlg,IDC_IS_POPUP)) {
				RegisterPopup(TRUE);
			} else {
				Option.bExplorerPopup=FALSE;
			}

			if (IsDlgButtonChecked(hDlg,IDC_IS_MAKEICON)) {
				MyGetSpecialFolderPath(hDlg, CSIDL_DESKTOP, szLink);
				lstrcat(szLink, "\\���.lnk");
				GetModuleFileName(g_hInst,szPath,MAX_PATH);
				MyCreateShortCut(szPath, szLink, "", "���");
			}

			if (IsDlgButtonChecked(hDlg,IDC_IS_REGMENU)) {
				MyGetSpecialFolderPath(hDlg, CSIDL_PROGRAMS, szPath);
				lstrcat(szPath, "\\���");
				CreateDirectory(szPath,NULL);

				lstrcpy(szLink, szPath);
				lstrcat(szLink, "\\���.lnk");
				GetModuleFileName(g_hInst,szPath,MAX_PATH);
				MyCreateShortCut(szPath, szLink, "", "���");
			}
			if (IsDlgButtonChecked(hDlg,IDC_IS_ASSODGP)==BST_CHECKED) {
				MakeAssociate(".dgp","��� ������Ʈ ����");
				MakeAssociate(".dgc","��� ��뱸 ����");
			}

			GetModuleFileName(g_hInst,szPath,MAX_PATH);
			SHRegWriteString(SHCU,KEY"Setting","InstPath",szPath);
			Day=SHRegReadInt(SHCU,"Software\\Miyoungsoft","DgInstDay120",0);
			if (Day==0) {
				GetLocalTime(&st);
				Day=MyGetAbsDay(st);
				SHRegWriteInt(SHCU,"Software\\Miyoungsoft","DgInstDay120",Day);
			}
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

BOOL CALLBACK UninstallDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage) {
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
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

void Uninstall()
{
	TCHAR szKey[MAX_PATH];
	TCHAR szPath[MAX_PATH];
	TCHAR szLink[MAX_PATH];
	TCHAR lpSubKey[MAX_PATH];
	FILETIME FileTime;
	DWORD i;
	LONG Result;
	HKEY key;
	DWORD Size;

	RegisterPopup(FALSE,TRUE);
	lstrcpy(szKey,KEY);
	szKey[lstrlen(szKey)]=0;
	SHDeleteKey(SHCU,KEY);

	MyGetSpecialFolderPath(g_hFrameWnd, CSIDL_DESKTOP, szLink);
	lstrcat(szLink, "\\���.lnk");
	if (_access(szLink,0)==0) {
		DeleteFile(szLink);
	}

	MyGetSpecialFolderPath(g_hFrameWnd, CSIDL_PROGRAMS, szPath);
	lstrcat(szPath, "\\���");
	wsprintf(szLink,"%s\\���.lnk",szPath);
	if (_access(szLink,0)==0) {
		DeleteFile(szLink);
		RemoveDirectory(szPath);
	}

	RegOpenKeyEx(SHCR,NULL,0,KEY_READ,&key);
	Result=ERROR_SUCCESS;
	for (i=0;Result==ERROR_SUCCESS;i++) {
		Size=MAX_PATH;
		Result=RegEnumKeyEx(key,i,lpSubKey,&Size,NULL,NULL,NULL,&FileTime);
		if (Result==ERROR_SUCCESS) {
			if (strncmp(lpSubKey,"Dangeun.",8)==0) {
				UnAssociate(lpSubKey+7);
				i--;
			}
		}
	}
	RegCloseKey(key);
}

void SelectParser(CApiEdit &Ae,TCHAR *path)
{
	TCHAR ext[_MAX_EXT+1];
	TCHAR arExt[255];
	int i;
	int ID=0;

	_splitpath(path,NULL,NULL,NULL,ext);
	CharLower(ext);
	lstrcat(ext,";");
	for (i=1;i<PARSENUM;i++) {
		lstrcpy(arExt,Option.arExt[i]);
		CharLower(arExt);
		lstrcat(arExt,";");
		if (strstr(arExt,ext+1)!=0) {
			ID=i;
			break;
		}
	}

	Ae.SetParser(ID);
	Option.SetStyleColor(Ae.GetParser());
}

void Relayout()
{
	RECT crt;
	int t,s,f,o,w,ch;
	int fh;

	GetClientRect(g_hFrameWnd,&crt);
	GetChildSize(t,s,f,o,w,ch);

	if (Option.bShowToolBar) {
		SendMessage(hToolBar,TB_AUTOSIZE,0,0);
		ShowWindow(hToolBar,SW_SHOW);
	} else {
		ShowWindow(hToolBar,SW_HIDE);
	}

	if (Option.bShowStatus) {
		SendMessage(hStatus,WM_SIZE,0,0);
		ShowWindow(hStatus,SW_SHOW);
		SetStatusPart();
	} else {
		ShowWindow(hStatus,SW_HIDE);
	}

	if (Option.bShowFileTab) {
		MoveWindow(hTabFrame,0,t,crt.right,f,TRUE);
		ShowWindow(hTabFrame,SW_SHOW);
	} else {
		ShowWindow(hTabFrame,SW_HIDE);
	}

	if (bShowOutput) {
		MoveWindow(hOutput,0,crt.bottom-o-s,crt.right,o,TRUE);
		ShowWindow(hOutput,SW_SHOW);
	} else {
		ShowWindow(hOutput,SW_HIDE);
	}

	if (Option.bShowFileWnd) {
		fh=crt.bottom-(t+s+f+o+(o ? GAP:0)+ch+(ch ? GAP:0));
		MoveWindow(hFileWnd,0,t+f,w,fh,TRUE);
		ShowWindow(hFileWnd,SW_SHOW);
	} else {
		fh=0;
		ShowWindow(hFileWnd,SW_HIDE);
	}

	if (Option.bShowClipWnd) {
		MoveWindow(hClipWnd,0,t+f+fh+GAP,w,ch,TRUE);
		ShowWindow(hClipWnd,SW_SHOW);
	} else {
		ShowWindow(hClipWnd,SW_HIDE);
	}

	MoveWindow(g_hMDIClient,w+(w ? GAP:0),t+f,crt.right-w-(w ? GAP:0),
		crt.bottom-(t+s+f+o+(o ? GAP:0)),TRUE);

	HWND hChild;
	hChild=GetWindow(g_hMDIClient,GW_CHILD);
	if (hChild && !IsZoomed(hChild)) {
		while (hChild) {
			RedrawWindow(hChild,NULL,NULL,RDW_FRAME | RDW_UPDATENOW | RDW_INVALIDATE);
			hChild=GetWindow(hChild,GW_HWNDNEXT);
		}
	}
}

void GetChildSize(int &t,int &s,int &f,int &o,int &w,int &ch)
{
	RECT trt;
	RECT crt;
	int c;		// MDI Ŭ���̾�Ʈ ������ ����
	t=0;		// ���� ����
	s=0;		// ���¶� ����
	f=0;		// �������� ����
	o=0;		// ���â�� ����
	w=0;		// ����â�� ��
	ch=0;		// ��뱸 â�� ����

	if (Option.bShowToolBar) {
		GetWindowRect(hToolBar,&trt);
		t=trt.bottom-trt.top;
	}

	if (Option.bShowStatus) {
		GetWindowRect(hStatus,&trt);
		s=trt.bottom-trt.top;
	}

	if (Option.bShowFileTab) {
		GetWindowRect(hTabFrame,&trt);
		f=trt.bottom-trt.top;
	}

	if (bShowOutput) {
		o=Option.OutputHeight;
	}

	// ����â�� ��뱸â �� �� �ϳ��� ���̸� ����â�� ���� ������.
	if (Option.bShowFileWnd || Option.bShowClipWnd) {
		w=Option.FileWndWidth;
	}

	// MDI Ŭ���̾�Ʈ ����(=����â�� ��뱸â ����)�� ����
	GetClientRect(g_hFrameWnd,&crt);
	c=crt.bottom-(t+s+f+o+(o ? GAP:0));

	// Ŭ���̾�Ʈ ������ �����ε� ���â�� ���̴� ���¸� ���â�� ���̸� �ٿ�
	// ������ ������, ���� ��� ���� �ʰ� �Ѵ�.
	if (c < 0 && o) {
		o=crt.bottom-(t+s+f+GAP);
		c=0;
	}

	// ��뱸â�� ����
	if (Option.bShowClipWnd) {
		if (Option.bShowFileWnd) {
			ch=c*Option.ClipWndRatio/10000;
		} else {
			ch=c;
		}
	}
}

HWND NewChild()
{
	HWND hChild;
	SInfo *pSi;

	g_NewNo++;
	hChild=New();
	pSi=(SInfo *)GetWindowLongPtr(hChild,0);
	wsprintf(pSi->NowFile,"�̸����� %d",g_NewNo);
	SetWindowText(hChild,pSi->NowFile);
	AddFileTab(pSi->NowFile);
	return hChild;
}

void PrintInfo(CApiEdit &Ae)
{
	TCHAR Mes[512];
	TCHAR str[256];
	int byte,byte2;
	HWND hActive;
	SInfo *pSi;

	hActive=(HWND)SendMessage(g_hMDIClient,WM_MDIGETACTIVE,0,NULL);
	pSi=(SInfo *)GetWindowLongPtr(hActive,0);
	byte=Ae.GetTextLength();
	wsprintf(Mes,"ũ�� : %d ����Ʈ(%d K����Ʈ)\r\n",byte, byte/1024);
	wsprintf(str,"�� �� : %d ��\r\n",Ae.GetDocInfo(AE_DOC_TOTALPARA)+1);
	lstrcat(Mes,str);
	wsprintf(str,"����ִ� �� �� : %d ��\r\n",Ae.GetDocInfo(AE_DOC_EMPTYLINE));
	lstrcat(Mes,str);
	wsprintf(str,"�ܾ� �� : %d �ܾ�\r\n",Ae.GetDocInfo(AE_DOC_WORD));
	lstrcat(Mes,str);
	byte2=Ae.GetDocInfo(AE_DOC_EXSPACESIZE);
	wsprintf(str,"������ ������ ũ�� : %d ����Ʈ(����� %d%%)\r\n",
		byte2, byte==0 ? 0:100-byte2*100/byte);
	lstrcat(Mes,str);
	byte=Ae.GetDocInfo(AE_DOC_CHAR);
	byte2=Ae.GetDocInfo(AE_DOC_HAN);
	wsprintf(str,"�� ���� �� : %d ���� (�ѱ� %d, ���� %d)",
		byte, byte2, byte-byte2);
	lstrcat(Mes,str);
	if (pSi->Ae.GetActiveView()->ViewType == HEXA_VIEW) {
		lstrcat(Mes,"\r\n\r\n��� �信���� ���� ���� ��� �� ������ �����ݴϴ�.");
	}

	MessageBox(g_hFrameWnd,Mes,"���� ����",MB_OK);
}

LRESULT CALLBACK DgCbtHookProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	CBT_CREATEWND *pCbt;
	HWND hWnd;
	TCHAR szClassName[128];
	WNDPROC OldProc;

	if (nCode == HCBT_CREATEWND) {
		hWnd=(HWND)wParam;
		pCbt=(CBT_CREATEWND *)lParam;

		if (HIWORD(pCbt->lpcs->lpszClass)) {
			lstrcpy(szClassName,pCbt->lpcs->lpszClass);
		} else {
			GlobalGetAtomName((ATOM)pCbt->lpcs->lpszClass,szClassName,32);
		}

		if (lstrcmpi(szClassName,"#32770")==0 && ((pCbt->lpcs->style & WS_CHILD)==0)) {
			OldProc=(WNDPROC)GetWindowLongPtr(hWnd,GWL_WNDPROC);
			SetProp(hWnd,"OldProp",OldProc);
			SetWindowLongPtr(hWnd,GWLP_WNDPROC,(DWORD)DgNewDlgProc);
		}
	}
	return CallNextHookEx(hCbtHook,nCode,wParam,lParam);
}

LRESULT CALLBACK DgNewDlgProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	WNDPROC OldProc;
	OldProc=(WNDPROC)GetProp(hWnd,"OldProp");
	TCHAR szCap[128];
	LRESULT ret;

	switch(iMessage) {
	case WM_INITDIALOG:
		MoveToParentCenter(hWnd);
		ret=CallWindowProc(OldProc,hWnd,iMessage,wParam,lParam);
		GetWindowText(hWnd,szCap,128);
		if (lstrcmp(szCap,"����")==0) {
			MoveToParentCenter(hWnd);
		}
		return ret;
	case WM_NCDESTROY:
		SetWindowLongPtr(hWnd,GWLP_WNDPROC,(DWORD_PTR)OldProc);
		RemoveProp(hWnd,"OldProp");
		break;
	}
	return CallWindowProc(OldProc,hWnd,iMessage,wParam,lParam);
}

void MakeBackupName(TCHAR *Folder,TCHAR *fname,TCHAR *ext,int level,TCHAR *szName)
{
	TCHAR szNum[12];

	// ù ��° ��� ���Ͽ��� ���ڸ� ������ �ʴ´�.
	if (level == 1) {
		szNum[0]=0;
	} else {
		wsprintf(szNum,"%d",level);
	}

	// �̸� ���̱� �ɼǿ� ���� ���ڿ��� �����Ѵ�.
	switch (Option.BackupNaming) {
	case 0:
		wsprintf(szName,"%s%s.bak%s",Folder,fname,szNum);
		break;
	case 1:
		wsprintf(szName,"%s%s_bak%s%s",Folder,fname,szNum,ext);
		break;
	case 2:
		wsprintf(szName,"%s%s_���%s%s",Folder,fname,szNum,ext);
		break;
	case 3:
		wsprintf(szName,"%s%s%s.bak%s",Folder,fname,ext,szNum);
		break;
	}
}

void MakeBackup(TCHAR *Path)
{
	TCHAR Folder[MAX_PATH];
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	int i;
	TCHAR Src[MAX_PATH];
	TCHAR Dest[MAX_PATH];

	// ��� �ɼ��� ���õ��� �ʾҰų� FTP �����̸� ������� �ʴ´�.
	if (Option.BackupLevel == 0 || strnicmp(Path,"ftp",3)==0) {
		return;
	}

	// ��� ������ �����Ǿ� ������ �� ������ ����Ѵ�.
	_splitpath(Path,drive,dir,fname,ext);
	if (lstrlen(Option.BackupFolder)==0 || _access(Option.BackupFolder,0)!=0) {
		wsprintf(Folder,"%s%s",drive,dir);
	} else {
		lstrcpy(Folder,Option.BackupFolder);
		if (Folder[lstrlen(Folder)-1]!='\\') {
			lstrcat(Folder,"\\");
		}
	}

	// �ٴܰ�� ����Ѵ�. ���� �ܰ� ��� ������ ���� �ܰ�� �����Ѵ�.
	for (i=Option.BackupLevel;i>0;i--) {
		if (i==1) {
			lstrcpy(Src,Path);
		} else {
			MakeBackupName(Folder,fname,ext,i-1,Src);
		}
		if (_access(Src,0)==0) {
			MakeBackupName(Folder,fname,ext,i,Dest);
			CopyFile(Src,Dest,FALSE);
		}
	}
}

void AutoSave()
{
	HWND hChild;
	SInfo *pSi;

	// ���� ��� ���ϵ��� ��� �����Ѵ�.
	hChild=GetWindow(g_hMDIClient,GW_CHILD);
	while (hChild) {
		pSi=(SInfo *)GetWindowLongPtr(hChild,0);
		if (strnicmp(pSi->NowFile,"http",4) != 0 &&
			strncmp(pSi->NowFile,"�̸�����",8) != 0 &&
			pSi->Ae.GetReadOnly() == FALSE &&
			pSi->Ae.GetModified() == TRUE) {
			SaveToFile(hChild,pSi->NowFile);
		}
		hChild=GetWindow(hChild,GW_HWNDNEXT);
	}

	// ���� ��Ȱ��ȭ�� �����̸� ���� ���� ������ �ٽ� �ۼ��Ѵ�.
	if (GetForegroundWindow() != g_hFrameWnd) {
		SaveEditings();
	}
}

// �������� ������ ������ �����Ѵ�.
void SaveEditings()
{
	HWND hChild;
	SInfo *pSi;
	int idx=0;
	HANDLE hFile;

	if (Option.bWatchChange && g_ChildNum) {
		if (pEditings) {
			free(pEditings);
		}
		pEditings=(FileInfo *)malloc(sizeof(FileInfo)*(g_ChildNum+1));
		hChild=GetWindow(g_hMDIClient,GW_CHILD);
		while (hChild) {
			pSi=(SInfo *)GetWindowLongPtr(hChild,0);
			if (strncmp(pSi->NowFile,"�̸�����",8) && strnicmp(pSi->NowFile,"http",4)
				&& strnicmp(pSi->NowFile,"ftp",3)) {
				pEditings[idx].hWnd=hChild;
				hFile=CreateFile(pSi->NowFile,0,0,NULL,
					OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
				if (hFile!=INVALID_HANDLE_VALUE) {
					pEditings[idx].size=GetFileSize(hFile,NULL);
					GetFileTime(hFile,NULL,NULL,&pEditings[idx].st);
					CloseHandle(hFile);
					idx++;
				}
			}
			hChild=GetWindow(hChild,GW_HWNDNEXT);
		}
		pEditings[idx].hWnd=NULL;
	}
}

void MakeAssociate(TCHAR *ext, TCHAR *desc)
{
	TCHAR OldProgID[64];
	TCHAR OldDesc[64];
	TCHAR NewProgID[64];
	TCHAR NewDesc[64];
	TCHAR szKey[MAX_PATH];
	TCHAR Path[MAX_PATH];

	SHRegReadString(SHCR,ext,NULL,"",OldProgID,64);
	SHRegReadString(SHCR,OldProgID,NULL,"",OldDesc,64);

	wsprintf(NewProgID,"Dangeun%s",ext);
	if (desc == NULL) {
		lstrcpy(NewDesc,OldDesc);
	} else {
		lstrcpy(NewDesc,desc);
	}

	SHRegWriteString(SHCR,NewProgID,NULL,NewDesc);
	wsprintf(szKey,"%s\\shell\\open\\command",NewProgID);
	GetModuleFileName(g_hInst,Path,MAX_PATH);
	lstrcat(Path, " \"%1\"");
	SHRegWriteString(SHCR,szKey,NULL,Path);

	SHRegWriteString(SHCR,ext,NULL,NewProgID);

	if (lstrcmp(OldProgID, NewProgID)!=0) {
		SHRegWriteString(SHCR,NewProgID,"OldProgID",OldProgID);
	}

	SHChangeNotify(SHCNE_ASSOCCHANGED,SHCNF_IDLIST,NULL,NULL);
}

void UnAssociate(TCHAR *ext)
{
	TCHAR OldProgID[64];
	TCHAR NewProgID[64];

	wsprintf(NewProgID,"Dangeun%s",ext);
	SHRegReadString(SHCR,ext,NULL,"",OldProgID,64);
	if (lstrcmp(OldProgID, NewProgID)==0) {
		SHRegReadString(SHCR,NewProgID,"OldProgID","",OldProgID,64);
		SHRegWriteString(SHCR,ext,NULL,OldProgID);
		SHDeleteKey(SHCR,NewProgID);
	}

	SHChangeNotify(SHCNE_ASSOCCHANGED,SHCNF_IDLIST,NULL,NULL);
}

void RearrangePath()
{
	TCHAR InstPath[MAX_PATH];
	TCHAR ExePath[MAX_PATH];
	TCHAR lpSubKey[MAX_PATH];
	FILETIME FileTime;
	DWORD i;
	LONG Result;
	HKEY key;
	DWORD Size;

	// ������Ʈ���� ��ο� ���� ������ ��� ������ �ƹ� �͵� �� �ʿ䰡 ����.
	SHRegReadString(SHCU,KEY"Setting","InstPath","",InstPath,MAX_PATH);
	GetModuleFileName(g_hInst,ExePath,MAX_PATH);
	if (lstrcmp(ExePath,InstPath)==0) {
		return;
	}

	// �� ��θ� ����Ѵ�.
	SHRegWriteString(SHCU,KEY"Setting","InstPath",ExePath);

	// ����� ������ ��θ� �����Ѵ�.
	RegOpenKeyEx(SHCR,NULL,0,KEY_READ,&key);
	Result=ERROR_SUCCESS;
	for (i=0;Result==ERROR_SUCCESS;i++) {
		Size=MAX_PATH;
		Result=RegEnumKeyEx(key,i,lpSubKey,&Size,NULL,NULL,NULL,&FileTime);
		if (Result==ERROR_SUCCESS) {
			if (strncmp(lpSubKey,"Dangeun.",8)==0) {
				MakeAssociate(lpSubKey+7,NULL);
			}
		}
	}
	RegCloseKey(key);
}

// ���� ���� ������ ��ġ�� ���Ѵ�.
BOOL GetEiPath(TCHAR *Path, TCHAR *EiPath)
{
	TCHAR Drive[_MAX_DRIVE];
	TCHAR Dir[_MAX_DIR];
	TCHAR Name[MAX_PATH];
	TCHAR Ext[_MAX_EXT];

	_splitpath(Path,Drive,Dir,Name,Ext);
	if (lstrcmpi(Ext,".dgx")==0)
		return FALSE;
	if (lstrlen(Option.EditInfoFolder)==0 || _access(Option.EditInfoFolder,0)!=0) {
		wsprintf(EiPath,"%s%s%s.dgx",Drive,Dir,Name);
	} else {
		wsprintf(EiPath,"%s\\%s.dgx",Option.EditInfoFolder,Name);
	}
	return TRUE;
}

// ��� ���ڵ带 ������ ���� ������ ����ü�� �ۼ��Ѵ�.
BOOL MakeEditInfo(CApiEdit &Ae, TCHAR *Path, EditInfo &ei)
{
	HANDLE hFile;
	int i;

	if (Option.EditInfoMask == 0) {
		return FALSE;
	}

	// ����� �ۼ��Ѵ�. ���, ũ��, �ð� ���� ����Ѵ�.
	memset(&ei,0,sizeof(EditInfo));
	lstrcpy(ei.Text,"��� ���� ���� �����Դϴ�");
	ei.Ver=120;
	lstrcpy(ei.FilePath,Path);
	hFile=CreateFile(Path,0,0,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile==INVALID_HANDLE_VALUE) {
		return FALSE;
	}
	ei.FileSize=GetFileSize(hFile,NULL);
	GetFileTime(hFile,NULL,NULL,&ei.FileTime);
	CloseHandle(hFile);
	ei.EditInfoMask=Option.EditInfoMask;

	// ���� ���¸� �����ϰ� �� ���� ��ġ, ���� ����, �ϸ�ũ, ������ ����Ѵ�.
	ei.VertRatio=Ae.VertRatio;
	ei.HorzRatio=Ae.HorzRatio;
	ei.ActiveView=Ae.ActiveView;
	for (i=0;i<4 && Ae.arView[i];i++) {
		ei.arViewType[i]=Ae.arView[i]->ViewType;

		if (ei.EditInfoMask & EI_OFFSET) {
			ei.off[i]=Ae.arView[i]->GetOffset();
		}

		if (ei.EditInfoMask & EI_SELECTION) {
			Ae.arView[i]->GetSelect(ei.SelStart[i], ei.SelEnd[i], FALSE);
		}

		if (ei.EditInfoMask & EI_BOOKMARK) {
			memcpy(ei.BookMark[i],Ae.arView[i]->GetBookmark(),sizeof(int)*MAXBOOKMARK);
		}

		if (ei.EditInfoMask & EI_SETTING) {
			ei.bWrap[i]=Ae.GetWrap()==0 ? FALSE:TRUE;
			ei.LineRatio[i]=Ae.GetLineRatio();
			Ae.GetFont(&ei.logfont[i]);
		}
	}
	return TRUE;
}

// ���� �������� ��� ���ڵ带 ������ �κи� �ۼ��Ѵ�.
void UpdateEditInfo(CApiEdit &Ae, TCHAR *Path)
{
	EditInfo ei;
	HANDLE hFile;
	TCHAR EiPath[MAX_PATH];
	DWORD dwWritten;
	DWORD dwCreate;

	if (MakeEditInfo(Ae,Path,ei) == FALSE) {
		return;
	}

	if (GetEiPath(Path,EiPath)==FALSE) {
		return;
	}
	if (_access(EiPath,0)==0) {
		dwCreate=OPEN_EXISTING;
	} else {
		dwCreate=CREATE_ALWAYS;
	}
	hFile=CreateFile(EiPath,GENERIC_WRITE,0,NULL,
		dwCreate,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile==INVALID_HANDLE_VALUE) {
		return;
	}

	// ���� ������ �ٽ� ����Ѵ�.
	WriteFile(hFile,&ei,sizeof(EditInfo),&dwWritten,NULL);
	CloseHandle(hFile);
}

// ���� ������ ��� ���ڵ���� �ۼ��Ͽ� ���Ϸ� ����Ѵ�.
void SaveEditInfo(CApiEdit &Ae, TCHAR *Path)
{
	EditInfo ei;
	HANDLE hFile;
	TCHAR EiPath[MAX_PATH];
	DWORD dwWritten;
	UndoRecord *p;
	int datasize;

	if (MakeEditInfo(Ae,Path,ei) == FALSE) {
		return;
	}

	if (GetEiPath(Path,EiPath)==FALSE) {
		return;
	}
	hFile=CreateFile(EiPath,GENERIC_WRITE,0,NULL,
		CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile==INVALID_HANDLE_VALUE) {
		return;
	}

	// ���� ���� ���
	WriteFile(hFile,&ei,sizeof(EditInfo),&dwWritten,NULL);

	// ��� ���ڵ� ���� �ɼ��� ���õǾ� ������ ��� ���ڵ嵵 ���
	if (ei.EditInfoMask & EI_UNDORECORD) {
		WriteFile(hFile,&Ae.nowur,sizeof(int),&dwWritten,NULL);
		p=Ae.pUR;
		// ��� ���ڵ带 ��ȸ�Ѵ�. ���� ���� UR_NONE�� ���Եȴ�.
		for (;;p++) {
			// ���ڵ� ���� ���
			WriteFile(hFile,p,sizeof(UndoRecord),&dwWritten,NULL);
			if (p->action == UR_NONE) {
				datasize=0;
			} else {
				datasize=_msize(p->data);
			}
			// ���ڵ��� ������ ���
			WriteFile(hFile,&datasize,sizeof(int),&dwWritten,NULL);
			WriteFile(hFile,p->data,datasize,&dwWritten,NULL);
			if (p->action == UR_NONE) {
				break;
			}
		}
	}

	CloseHandle(hFile);
}

// ���� ������ �о� �����Ѵ�.
void LoadEditInfo(CApiEdit &Ae, TCHAR *Path)
{
	EditInfo ei;
	HANDLE hFile=INVALID_HANDLE_VALUE;
	HANDLE hDoc=INVALID_HANDLE_VALUE;
	TCHAR EiPath[MAX_PATH];
	TCHAR Drive[_MAX_DRIVE];
	TCHAR Dir[_MAX_DIR];
	TCHAR Name[MAX_PATH];
	DWORD dwRead;
	FILETIME FileTime;
	int i;
	int urnum;
	int datasize;
	UndoRecord UR;

	// ���� �������� ���� ã�� ������ ������ ã�� ����.
	_splitpath(Path,Drive,Dir,Name,NULL);
	wsprintf(EiPath,"%s%s%s.dgx",Drive,Dir,Name);
	if (_access(EiPath,0)!=0) {
		if (lstrlen(Option.EditInfoFolder)!=0) {
			wsprintf(EiPath,"%s\\%s.dgx",Option.EditInfoFolder,Name);
		}
	}

	// ���� ���� ������ ������ ������ �� ����.
	hFile=CreateFile(EiPath,GENERIC_READ,0,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile==INVALID_HANDLE_VALUE) {
		goto end;
	}

	// ������ Ʋ���ų� ��� ������ Ʋ���� ������ �� ����.
	ReadFile(hFile,&ei,sizeof(EditInfo),&dwRead,NULL);
	if (ei.Ver != 120 || lstrcmp(ei.FilePath,Path) != 0 || ei.EditInfoMask == 0) {
		goto end;
	}

	// ���� ������ ��¥�� ũ�⵵ ��ġ�ؾ� �Ѵ�.
	hDoc=CreateFile(Path,0,0,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hDoc==INVALID_HANDLE_VALUE) {
		goto end;
	}
	GetFileTime(hDoc,NULL,NULL,&FileTime);
	if ((ei.FileSize != GetFileSize(hDoc,NULL) || 
		(CompareFileTime(&FileTime,&ei.FileTime) != 0))) {
		goto end;
	}

	// ���ҵǾ� ������ ���� ���¸� �����ϰ� �� ���� Ÿ�Ե� �״�� �����Ѵ�.
	if (ei.VertRatio) {
		Ae.SplitView(AESPLIT_VERT,ei.VertRatio);
	}
	if (ei.HorzRatio) {
		Ae.SplitView(AESPLIT_HORZ,ei.HorzRatio);
	}
	for (i=0;i<4 && Ae.arView[i];i++) {
		Ae.ChangeViewType(i,ei.arViewType[i]);
	}

	// �� ���� ���� ���¸� �о� �����Ѵ�.
	for (i=0;i<4 && Ae.arView[i];i++) {
		if (ei.EditInfoMask & EI_OFFSET) {
			Ae.arView[i]->SetOffset(ei.off[i]);
		}

		if (ei.EditInfoMask & EI_SELECTION) {
			if (ei.SelStart[i] != ei.SelEnd[i]) {
				Ae.arView[i]->SetSelect(ei.SelStart[i], ei.SelEnd[i], FALSE);
			}
		}

		if (ei.EditInfoMask & EI_BOOKMARK) {
			Ae.arView[i]->SetBookmark(ei.BookMark[i]);
		}

		if (ei.EditInfoMask & EI_SETTING) {
			Ae.arView[i]->SetWrap(ei.bWrap[i] ? Option.nWrap:0);
			Ae.arView[i]->SetLineRatio(ei.LineRatio[i]);
			Ae.arView[i]->SetFont(&ei.logfont[i]);
		}
	}
	Ae.ActiveView=ei.ActiveView;
	SetFocus(Ae.hWnd);

	// ��� ���ڵ尡 ��ϵǾ� ������ �����Ѵ�.
	if (ei.EditInfoMask & EI_UNDORECORD) {
		ReadFile(hFile,&Ae.nowur,sizeof(int),&dwRead,NULL);
		// �� �̻� ���� ������ ���ų� UR_NONE�� ���� ������ ��� �д´�.
		for (urnum=0;;urnum++) {
			ReadFile(hFile,&UR,sizeof(UndoRecord),&dwRead,NULL);
			if (dwRead==0) {
				break;
			}
			// �ʿ�� ��� ���ڵ带 ���Ҵ��Ѵ�.
			if (urnum==Ae.URSize-1) {
				Ae.URSize+=640;
				Ae.pUR=(UndoRecord *)realloc(Ae.pUR,sizeof(UndoRecord)*Ae.URSize);
				memset(Ae.pUR+Ae.URSize-640,0,sizeof(UndoRecord)*640);
			}
			// ���ڵ��� �����͸� �д´�.
			ReadFile(hFile,&datasize,sizeof(int),&dwRead,NULL);
			if (datasize) {
				UR.data=(TCHAR *)malloc(datasize);
				ReadFile(hFile,UR.data,datasize,&dwRead,NULL);
			}
			Ae.pUR[urnum]=UR;
			if (UR.action == UR_NONE) {
				break;
			}
		}

		Ae.SaveModified();
	}

end:
	if (hDoc!=INVALID_HANDLE_VALUE) CloseHandle(hDoc);
	if (hFile!=INVALID_HANDLE_VALUE) CloseHandle(hFile);
}

BOOL CALLBACK EditRecordProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	static BOOL bUndo;
	static HWND hList;
	RECT rt;
	LVCOLUMN COL;
	SInfo *pSi;
	HWND hActive;
	int idx, count, Inc, idx2;
	UndoRecord *pUR;
	LVITEM LI;
	TCHAR szAct[12], szCon[1024];

	switch(iMessage) {
	case WM_INITDIALOG:
		if (lParam == IDM_EDIT_UNDO) {
			bUndo=TRUE;
			Inc=-1;
			SetDlgItemText(hDlg,IDC_MULTIUR,"���� ���۱��� ���");
		} else {
			bUndo=FALSE;
			Inc=1;
			SetDlgItemText(hDlg,IDC_MULTIUR,"���� ���۱��� �����");
		}

		SendMessage(hToolBar,TB_GETITEMRECT,bUndo ? 8:9,(LPARAM)&rt);
		rt.top=rt.bottom+2;
		ClientToScreen(g_hFrameWnd,(LPPOINT)&rt);
		SetWindowPos(hDlg,HWND_NOTOPMOST,rt.left,
			rt.top,0,0,SWP_NOSIZE);

		hList=GetDlgItem(hDlg,IDC_UNDOLIST);
		COL.mask=LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		COL.fmt=LVCFMT_LEFT;
		COL.cx=40;
		COL.pszText="����";
		COL.iSubItem=0;
		ListView_InsertColumn(hList,0,&COL);

		COL.cx=145;
		COL.pszText="����";
		COL.iSubItem=1;
		ListView_InsertColumn(hList,1,&COL);

		ListView_SetExtendedListViewStyle(hList, LVS_EX_FULLROWSELECT);
		hActive=(HWND)SendMessage(g_hMDIClient,WM_MDIGETACTIVE,0,NULL);
		pSi=(SInfo *)GetWindowLong(hActive,0);

		for (idx=pSi->Ae.nowur, count=0;;idx+=Inc) {
			pUR=&pSi->Ae.pUR[idx];
			if (bUndo) {
				if (pUR->action == UR_NONE || pUR->status == UR_CANCELED)
					continue;
			} else {
				if (pUR->action == UR_NONE)
					break;
			}

			if (bUndo && pUR->action != UR_GROUP && pUR->tick) {
				for (;idx >= 0;idx--) {
					if (pSi->Ae.pUR[idx].action==UR_GROUP && pSi->Ae.pUR[idx].tick==pUR->tick) {
						break;
					}
				}
				if (idx < 0) {
					break;
				}
				switch (pSi->Ae.pUR[idx].pos) {
				case 8:
				case 9:
					for (idx2=idx;;idx2++) {
						if (pSi->Ae.pUR[idx2].action==UR_INSERT)
							break;
					}
					lstrcpy(szAct,"����");
					ReplaceCtrlString(pSi->Ae.pUR[idx2].data,pSi->Ae.pUR[idx2].len,szCon,1024);
					break;
				default:
					lstrcpy(szAct,"�׷�");
					lstrcpy(szCon,arGroupDesc[pSi->Ae.pUR[idx].pos]);
					if (pSi->Ae.pUR[idx].data) {
						lstrcat(szCon,pSi->Ae.pUR[idx].data);
					}
					break;
				}
			} else if (bUndo==FALSE && pUR->action == UR_GROUP) {
				switch (pSi->Ae.pUR[idx].pos) {
				case 8:
				case 9:
					for (idx2=idx;;idx2++) {
						if (pSi->Ae.pUR[idx2].action==UR_INSERT)
							break;
					}
					lstrcpy(szAct,"����");
					ReplaceCtrlString(pSi->Ae.pUR[idx2].data,pSi->Ae.pUR[idx2].len,szCon,1024);
					break;
				default:
					lstrcpy(szAct,"�׷�");
					lstrcpy(szCon,arGroupDesc[pUR->pos]);
					if (pUR->data) {
						lstrcat(szCon,pUR->data);
					}
					break;
				}
				idx=pSi->Ae.FindGroupEnd(idx);
			} else {
				switch (pUR->action) {
				case UR_INSERT:
					lstrcpy(szAct,"����");
					ReplaceCtrlString(pUR->data,pUR->len,szCon,1024);
					break;
				case UR_DELETE:
					lstrcpy(szAct,"����");
					ReplaceCtrlString(pUR->data,pUR->len,szCon,1024);
					break;
				case UR_MOVE:
					lstrcpy(szAct,"�̵�");
					wsprintf(szCon,"%d -> %d(%d����Ʈ)",pUR->pos,pUR->dest[0],pUR->dest[1]);
					break;
				}
			}

			LI.mask=LVIF_TEXT | LVIF_PARAM;
			LI.iItem=count;
			LI.iSubItem=0;
			LI.lParam=idx;
			LI.pszText=szAct;
			ListView_InsertItem(hList,&LI);
			ListView_SetItemText(hList,count,1,szCon);
			if ((bUndo && idx == 0) || count == 200) {
				break;
			}
			count++;
		}

		SetFocus(hList);
		return FALSE;
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE) {
			PostMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDCANCEL,0),0);
			return TRUE;
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))	{
		case IDC_MULTIUR:
			idx=ListView_GetNextItem(hList,-1,LVNI_ALL | LVNI_SELECTED);
			if (idx != -1) {
				LI.mask=LVIF_PARAM;
				LI.iItem=idx;
				ListView_GetItem(hList,&LI);
				PostMessage(g_hFrameWnd,WM_USER+3,(WPARAM)bUndo,(LPARAM)LI.lParam);
			}
		case IDCANCEL:
			DestroyWindow(hDlg);
			return FALSE;
		}
		return FALSE;
	case WM_NOTIFY:
		LPNMHDR hdr;
		hdr=(LPNMHDR)lParam;
		if (hdr->hwndFrom == hList) {
			switch (hdr->code) {
			case NM_DBLCLK:
				PostMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_MULTIUR,0),0);
				return TRUE;
			}
		}
		break;
	}
	return FALSE;
}

// ��ϵ� ����Ű���� ���� ����Ű�̸� ���� �� TRUE�� �����Ѵ�.
BOOL TestShortCut(HWND hWnd,LPMSG msg)
{
	int i;
	BOOL bFind=FALSE;
	BOOL bShift,bControl,bAlt;
	DWORD Flag=0;
	SShortCut *psc;
	HWND hActive;
	SInfo *pSi;
	UINT vk;

	if (msg->message != WM_KEYDOWN && msg->message != WM_SYSKEYDOWN) {
		return FALSE;
	}

	bShift=((GetKeyState(VK_SHIFT) & 0x8000) != 0); 
	bControl=((GetKeyState(VK_CONTROL) & 0x8000) != 0);
	bAlt=((GetKeyState(VK_MENU) & 0x8000) != 0);

	if (bControl) Flag |= SC_CTRL;
	if (bAlt) Flag |= SC_ALT;
	if (bShift) Flag |= SC_SHIFT;

	// �ѱ� �������̸� ����Ű�� ���� ���Ѵ�.
	if (msg->wParam==VK_PROCESSKEY) {
		vk=ImmGetVirtualKey(hWnd);
	} else {
		vk=msg->wParam;
	}

	for (i=0;DgShortCut.arShortCut[i].vk;i++) {
		psc=&DgShortCut.arShortCut[i];
		// ��� ��尡 �ƴ� ���� ��� ��忡���� �� �� �ִ� ����Ű ����
		// Ctrl, Alt�� �����Ǿ� ���� ���� ����Ű�� ��� ��� �����̴�.
		if (!bComMode && (psc->Flag & (SC_CTRL | SC_ALT)) == 0 && 
				(psc->vk >= '0' && psc->vk <= 'Z')) {
			continue;
		}
		if ((psc->vk == vk) &&	(psc->Flag == Flag)) {
			bFind=TRUE;
			break;
		}
	}

	// ��� ����
	if (bFind) {
		hActive=(HWND)SendMessage(g_hMDIClient,WM_MDIGETACTIVE,0,(LPARAM)NULL);
		if (hActive) {
			pSi=(SInfo *)GetWindowLong(hActive,0);
			pSi->Ae.GetActiveView()->EndComp();
		}
		if (psc->id) {
			SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(psc->id,0),0);
		} else {
			if (hActive==NULL) {
				return FALSE;
			}
			pSi->Ae.ExecMacro(psc->Com);
		}
		return TRUE;
	}

	// ����Ű�� �ƴ� ���� ��� ��忡�� ���ǵ��� ���� ����Ű�� �����Ѵ�.
	if (bComMode) {
		if ((Flag & (SC_CTRL | SC_ALT)) == 0 && 
			(msg->wParam >= '0' && msg->wParam <= 'Z')) {
			return TRUE;
		}
	}

	return FALSE;
}

BOOL TestNoSaved()
{
	HWND hCloseChild;
	SInfo *pSi;

	hCloseChild=GetWindow(g_hMDIClient,GW_CHILD);
	while (hCloseChild) {
		pSi=(SInfo *)GetWindowLong(hCloseChild,0);
		if (pSi->Ae.GetModified()) {
			return TRUE;
		}
		hCloseChild=GetWindow(hCloseChild,GW_HWNDNEXT);
	}
	return FALSE;
}

BOOL CALLBACK ConfirmSaveProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HWND hList;
	LVCOLUMN COL;
	SInfo *pSi;
	HWND hChild;
	LVITEM LI;
	int count,i,idx;
	TCHAR TextBuf[512],Content[1024];
	TCHAR Path[MAX_PATH];
	TCITEM tie;

	switch(iMessage) {
	case WM_INITDIALOG:
		MoveToParentCenter(hDlg);
		hList=GetDlgItem(hDlg,IDC_LISTNOSAVED);
		COL.mask=LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		COL.fmt=LVCFMT_LEFT;
		COL.cx=250;
		COL.pszText="���ϸ�";
		COL.iSubItem=0;
		ListView_InsertColumn(hList,0,&COL);

		COL.cx=250;
		COL.pszText="����";
		COL.iSubItem=1;
		ListView_InsertColumn(hList,1,&COL);
		ListView_SetExtendedListViewStyle(hList, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

		// ���� ���� ������� �����ش�.
		count=TabCtrl_GetItemCount(hFileTab);
		for (i=0,idx=0;i<count;i++) {
			tie.mask=TCIF_PARAM;
			TabCtrl_GetItem(hFileTab,i,&tie);
			hChild=FindChildWithFile((TCHAR *)tie.lParam);
			pSi=(SInfo *)GetWindowLong(hChild,0);
			if (pSi->Ae.GetModified()) {
				LI.mask=LVIF_TEXT | LVIF_PARAM;
				LI.iItem=idx;
				LI.iSubItem=0;
				LI.pszText=pSi->NowFile;
				LI.lParam=(LPARAM)hChild;
				ListView_InsertItem(hList,&LI);

				memset(TextBuf,0,512);
				pSi->Ae.GetText(TextBuf,500);
				ReplaceCtrlString(TextBuf,512,Content,1024);
				ListView_SetItemText(hList,idx,1,Content);
				idx++;
			}
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BTNSAVEALL:
		case IDC_BTNSAVECHECKED:
			hList=GetDlgItem(hDlg,IDC_LISTNOSAVED);
			for (idx=0;idx<ListView_GetItemCount(hList);idx++) {
				if (LOWORD(wParam)==IDC_BTNSAVECHECKED) {
					if (ListView_GetCheckState(hList,idx)==FALSE) {
						continue;
					}
				}
				LI.mask=LVIF_PARAM | LVIF_TEXT;
				LI.iItem=idx;
				LI.iSubItem=0;
				LI.pszText=Path;
				LI.cchTextMax=MAX_PATH;
				ListView_GetItem(hList,&LI);
				// ��� ������ ������ �ش�.
				ListView_SetItemState(hList,-1,0,LVIS_SELECTED | LVIS_FOCUSED);
				ListView_SetItemState(hList,idx,
					LVIS_SELECTED | LVIS_FOCUSED,LVIS_SELECTED | LVIS_FOCUSED);
				SendMessage(g_hMDIClient,WM_MDIACTIVATE,(WPARAM)LI.lParam,0);
				TabCtrl_SetCurSel(hFileTab,FindFileTab(Path));
				// ���� �Ǵ� ��ҽ� ���� ��ü�� ����Ѵ�.
				if (Save((HWND)LI.lParam)==FALSE) {
					EndDialog(hDlg,TRUE);
					return TRUE;
				}
			}
			EndDialog(hDlg,FALSE);
			return TRUE;
		case IDC_BTNDISCARDALL:
			EndDialog(hDlg,FALSE);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg,TRUE);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CALLBACK ConfirmReloadProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HWND hList;
	LVCOLUMN COL;
	SInfo *pSi;
	LVITEM LI;
	int i,idx;
	TCHAR TextBuf[512],Content[1024];
	TCHAR Path[MAX_PATH];
	HANDLE hFile;
	DWORD size;
	FILETIME st;

	switch(iMessage) {
	case WM_INITDIALOG:
		MoveToParentCenter(hDlg);
		hList=GetDlgItem(hDlg,IDC_LISTNOSAVED);
		COL.mask=LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		COL.fmt=LVCFMT_LEFT;
		COL.cx=250;
		COL.pszText="���ϸ�";
		COL.iSubItem=0;
		ListView_InsertColumn(hList,0,&COL);

		COL.cx=250;
		COL.pszText="����";
		COL.iSubItem=1;
		ListView_InsertColumn(hList,1,&COL);
		ListView_SetExtendedListViewStyle(hList, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

		// ����� ���� ����� ���� �ش�.
		for (i=0,idx=0;pEditings[i].hWnd!=NULL;i++) {
			pSi=(SInfo *)GetWindowLong(pEditings[i].hWnd,0);
			hFile=CreateFile(pSi->NowFile,0,0,NULL,
				OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
			if (hFile!=INVALID_HANDLE_VALUE) {
				size=GetFileSize(hFile,NULL);
				GetFileTime(hFile,NULL,NULL,&st);
				if (pEditings[i].size!=size || 
					CompareFileTime(&pEditings[i].st,&st)!=0) {
					LI.mask=LVIF_TEXT | LVIF_PARAM;
					LI.iItem=idx;
					LI.iSubItem=0;
					LI.pszText=pSi->NowFile;
					LI.lParam=(LPARAM)pEditings[i].hWnd;
					ListView_InsertItem(hList,&LI);

					memset(TextBuf,0,512);
					pSi->Ae.GetText(TextBuf,500);
					ReplaceCtrlString(TextBuf,512,Content,1024);
					ListView_SetItemText(hList,idx,1,Content);
					idx++;
				}
			}
			CloseHandle(hFile);
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BTNSAVEALL:
		case IDC_BTNSAVECHECKED:
			hList=GetDlgItem(hDlg,IDC_LISTNOSAVED);
			for (idx=0;idx<ListView_GetItemCount(hList);idx++) {
				if (LOWORD(wParam)==IDC_BTNSAVECHECKED) {
					if (ListView_GetCheckState(hList,idx)==FALSE) {
						continue;
					}
				}
				LI.mask=LVIF_PARAM | LVIF_TEXT;
				LI.iItem=idx;
				LI.iSubItem=0;
				LI.pszText=Path;
				LI.cchTextMax=MAX_PATH;
				ListView_GetItem(hList,&LI);
				RevertFile((HWND)LI.lParam);
			}
			// no break;
		case IDCANCEL:
			free(pEditings);
			pEditings=NULL;
			EndDialog(hDlg,TRUE);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

void InsertFile(CApiEditText *pView)
{
	OPENFILENAME OFN;
	TCHAR lpstrFile[MAX_PATH]={0,};
	HANDLE hFile;
	DWORD dwRead,dwSize;
	TCHAR *TextBuf;

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner=g_hFrameWnd;
	OFN.lpstrFilter="��� ����(*.*)\0*.*\0�ؽ�Ʈ ����\0*.txt\0";
	OFN.lpstrFile=lpstrFile;
	OFN.nMaxFile=MAX_PATH;
	OFN.Flags=OFN_EXPLORER;

	if (GetOpenFileName(&OFN)) {
		hFile=CreateFile(lpstrFile,GENERIC_READ,0,NULL,
			OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		if (hFile==INVALID_HANDLE_VALUE) {
			MessageBox(g_hFrameWnd, "������ �� �� �����ϴ�","�˸�",MB_OK);
			return;
		}
		dwSize=GetFileSize(hFile,NULL);
		if (dwSize > 30*1048576) {
			MessageBox(g_hFrameWnd, "�ִ� 30M������ ������ �� �ֽ��ϴ�.","�˸�",MB_OK);
		} else {
			TextBuf=(TCHAR *)malloc(dwSize+2);
			ReadFile(hFile,TextBuf,dwSize,&dwRead,NULL);
			TextBuf[dwRead]=0;
			TextBuf[dwRead+1]=0;
			pView->InsertText(TextBuf);
			free(TextBuf);
		}
		CloseHandle(hFile);
	}
}

static int g_cat, g_nx, g_ny;
enum {GRID_PGUP, GRID_PGDN, GRID_DBLCLK, GRID_CHG};

BOOL CALLBACK SpecialDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	static CApiEditText *pView;
	static TCHAR *szCategory[]={"����1","����2","���� ����","�ѱ� ������","�׸��� ����",
		"������","������","������","��ȣ����","���󰡳�","��Ÿī��","���þƾ�"};
	static HWND hGrid;
	RECT brt;
	int i,code;
	TCHAR szTemp[16];

	switch(iMessage) {
	case WM_INITDIALOG:
		MoveToParentCenter(hDlg);
		pView=(CApiEditText *)lParam;
		for (i=0;i<ARSIZE(szCategory);i++) {
			SendDlgItemMessage(hDlg,IDC_LISTCATEGORY,LB_ADDSTRING,0,(LPARAM)szCategory[i]);
		}
		SendDlgItemMessage(hDlg,IDC_LISTCATEGORY,LB_SETCURSEL,g_cat,0);
		GetWindowRect(GetDlgItem(hDlg,IDC_BTNCHARMAP),&brt);
		ScreenToClient(hDlg,(LPPOINT)&brt);
		hGrid=CreateWindowEx(WS_EX_CLIENTEDGE,"DGStringGrid",NULL,
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			brt.left,brt.top,26*16+2,26*6+2,hDlg,(HMENU)0,g_hInst,NULL);

		MoveWindow(GetDlgItem(hDlg,IDCANCEL),brt.left+26*16+2-90,brt.top+26*6+2+15,90,22,TRUE);
		MoveWindow(GetDlgItem(hDlg,IDOK),brt.left+26*16+2-190,brt.top+26*6+2+15,90,22,TRUE);
		MoveWindow(GetDlgItem(hDlg,IDC_STCODE),brt.left,brt.top+26*6+2+15,40,22,TRUE);
		MoveWindow(GetDlgItem(hDlg,IDC_EDCODE),brt.left+45,brt.top+26*6+2+15,90,22,TRUE);
		SetWindowPos(hDlg,HWND_NOTOPMOST,0,0,brt.left+26*16+2+12,
			brt.top+26*6+2+15+22+25+GetSystemMetrics(SM_CYCAPTION),SWP_NOMOVE);
		SetFocus(hGrid);
		return FALSE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 0:
			switch (HIWORD(wParam)) {
			case GRID_PGUP:
				if (g_cat > 0) {
					g_cat--;
					SendDlgItemMessage(hDlg,IDC_LISTCATEGORY,LB_SETCURSEL,g_cat,0);
					InvalidateRect(hGrid,NULL,TRUE);
				}
				break;
			case GRID_PGDN:
				if (g_cat < ARSIZE(szCategory)-1) {
					g_cat++;
					SendDlgItemMessage(hDlg,IDC_LISTCATEGORY,LB_SETCURSEL,g_cat,0);
					InvalidateRect(hGrid,NULL,TRUE);
				}
				break;
			case GRID_DBLCLK:
				SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDOK,0),0);
				break;
			case GRID_CHG:
				code=0xa1a1+g_cat*0x100+g_ny*16+g_nx;
				wsprintf(szTemp,"%d(%X)",code,code);
				SetDlgItemText(hDlg,IDC_EDCODE,szTemp);
				break;
			}
			break;
		case IDC_LISTCATEGORY:
			switch (HIWORD(wParam)) {
			case LBN_SELCHANGE:
				g_cat=SendDlgItemMessage(hDlg,IDC_LISTCATEGORY,LB_GETCURSEL,0,0);
				InvalidateRect(hGrid,NULL,TRUE);
				break;
			}
			break;
		case IDOK:
			code=0xa1a1+g_cat*0x100+g_ny*16+g_nx;
			szTemp[0]=code >> 8;
			szTemp[1]=code & 0xff;
			szTemp[2]=0;
			pView->InsertText(szTemp);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg,IDCANCEL);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

LRESULT CALLBACK DGStringGridProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	TEXTMETRIC tm;
	POINT pt;
	int i,j;
	RECT crt,nrt;
	TCHAR Text[3]={0,};
	HPEN hPen,OldPen;

	switch(iMessage) {
	case WM_GETDLGCODE:
		return DLGC_WANTARROWS;
	case WM_SETFOCUS:
	case WM_KILLFOCUS:
		InvalidateRect(hWnd,NULL,TRUE);
		return 0;
	case WM_PAINT:
		hdc=BeginPaint(hWnd, &ps);
		GetClientRect(hWnd,&crt);
		hPen=CreatePen(PS_SOLID,1,RGB(128,128,128));
		OldPen=(HPEN)SelectObject(hdc,hPen);
		for (i=1;i<16;i++) {
			MoveToEx(hdc,i*26,0,NULL);
			LineTo(hdc,i*26,crt.bottom);
		}
		for (i=1;i<6;i++) {
			MoveToEx(hdc,0,i*26,NULL);
			LineTo(hdc,crt.right,i*26);
		}

		SetRect(&nrt,g_nx*26,g_ny*26,g_nx*26+26,g_ny*26+26);
		FillRect(hdc,&nrt,GetSysColorBrush(COLOR_HIGHLIGHT));
		if (GetFocus() == hWnd) {
			DrawFocusRect(hdc,&nrt);
		}
		SetBkMode(hdc,TRANSPARENT);
		SetTextAlign(hdc,TA_CENTER);
		GetTextMetrics(hdc,&tm);
		for (i=0;i<16;i++) {
			for (j=0;j<6;j++) {
				Text[0]=0xa1+g_cat;
				Text[1]=0xa1+j*16+i;
				if (i==g_nx && j==g_ny) {
					SetTextColor(hdc,RGB(255,255,255));
				} else {
					SetTextColor(hdc,RGB(0,0,0));
				}
				TextOut(hdc,i*26+13,j*26+(26-tm.tmHeight)/2,Text,2);
			}
		}
		DeleteObject(SelectObject(hdc,OldPen));
		EndPaint(hWnd, &ps);
		return 0;
	case WM_LBUTTONDOWN:
		SetFocus(hWnd);
		pt.x=LOWORD(lParam)/26;
		pt.y=HIWORD(lParam)/26;
		if (pt.x!=g_nx || pt.y!=g_ny) {
			g_nx=pt.x;
			g_ny=pt.y;
			InvalidateRect(hWnd,NULL,TRUE);
			SendMessage(GetParent(hWnd),WM_COMMAND,MAKEWPARAM(0,GRID_CHG),(LPARAM)hWnd);
		}
		return 0;
	case WM_LBUTTONDBLCLK:
		SendMessage(GetParent(hWnd),WM_COMMAND,MAKEWPARAM(0,2),(LPARAM)hWnd);
		return 0;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_LEFT:
			if (g_nx > 0) {
				g_nx--;
			} else if (g_ny > 0) {
				g_nx=15;
				g_ny--;
			}
			SendMessage(GetParent(hWnd),WM_COMMAND,MAKEWPARAM(0,GRID_CHG),(LPARAM)hWnd);
			InvalidateRect(hWnd,NULL,TRUE);
			break;
		case VK_RIGHT:
			if (g_nx < 15) {
				g_nx++;
			} else if (g_ny < 5) {
				g_nx=0;
				g_ny++;
			}
			SendMessage(GetParent(hWnd),WM_COMMAND,MAKEWPARAM(0,GRID_CHG),(LPARAM)hWnd);
			InvalidateRect(hWnd,NULL,TRUE);
			break;
		case VK_UP:
			if (g_ny > 0) {
				g_ny--;
				SendMessage(GetParent(hWnd),WM_COMMAND,MAKEWPARAM(0,GRID_CHG),(LPARAM)hWnd);
				InvalidateRect(hWnd,NULL,TRUE);
			}
			break;
		case VK_DOWN:
			if (g_ny < 5) {
				g_ny++;
				SendMessage(GetParent(hWnd),WM_COMMAND,MAKEWPARAM(0,GRID_CHG),(LPARAM)hWnd);
				InvalidateRect(hWnd,NULL,TRUE);
			}
			break;
		case VK_PRIOR:
			SendMessage(GetParent(hWnd),WM_COMMAND,MAKEWPARAM(0,GRID_PGUP),(LPARAM)hWnd);
			SendMessage(GetParent(hWnd),WM_COMMAND,MAKEWPARAM(0,GRID_CHG),(LPARAM)hWnd);
			break;
		case VK_NEXT:
			SendMessage(GetParent(hWnd),WM_COMMAND,MAKEWPARAM(0,GRID_PGDN),(LPARAM)hWnd);
			SendMessage(GetParent(hWnd),WM_COMMAND,MAKEWPARAM(0,GRID_CHG),(LPARAM)hWnd);
			break;
		}
		return 0;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}

BOOL ResolveShortCut(TCHAR *ShortCut, TCHAR *Path)
{
	HRESULT hr;
	IShellLink *pSl=NULL;
	IPersistFile *pPf=NULL;
	WCHAR wszLnkFile[MAX_PATH]={0,};

	hr=CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
		IID_IShellLink, (void **)&pSl);
	if (FAILED(hr))
		return E_FAIL;

	hr=pSl->QueryInterface(IID_IPersistFile, (void **)&pPf);
	if (SUCCEEDED(hr)) {
		MultiByteToWideChar(CP_ACP, 0, ShortCut, -1, wszLnkFile, MAX_PATH);
		hr=pPf->Load(wszLnkFile,0);
		if (SUCCEEDED(hr)) {
			pSl->GetPath(Path,MAX_PATH,NULL,SLGP_UNCPRIORITY);
		}
	}

	if (pPf) pPf->Release();
	if (pSl) pSl->Release();
	if (SUCCEEDED(hr)) {
		return TRUE;
	} else {
		return FALSE;
	}
}

BOOL CALLBACK ReadModeProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HWND hActive;
	SInfo *pSi;
	static CApiEdit *Ae;
	TCHAR szCap[64];
	int i,idx,Value;
	UDACCEL Accel;
	LOGFONT tFont;
	HDC hdc;
	RECT wrt,drt;
	int dx,dy;

	switch(iMessage) {
	case WM_INITDIALOG:
		hActive=(HWND)SendMessage(g_hMDIClient,WM_MDIGETACTIVE,0,NULL);
		pSi=(SInfo *)GetWindowLong(hActive,0);
		Ae=&pSi->Ae;
		// ��Ʈ���� �� ����
		if (arFont==NULL) {
			DgEnumFonts();
		}
		SendDlgItemMessage(hDlg,IDC_RMFONT,CB_ADDSTRING,0,(LPARAM)"==�ý���==");
		for (i=0;i<FontNum;i++) {
			SendDlgItemMessage(hDlg,IDC_RMFONT,CB_ADDSTRING,0,(LPARAM)arFont[i].lfFaceName);
		}
		if (Option.Rmlogfont.lfHeight == 0) {
			idx=0;
		} else {
			idx=SendDlgItemMessage(hDlg,IDC_RMFONT,CB_FINDSTRING,-1,
				(LPARAM)Option.Rmlogfont.lfFaceName);
			SetDlgItemInt(hDlg,IDC_RMFONTSIZE,Option.Rmlogfont.lfHeight,FALSE);
		}
		SendDlgItemMessage(hDlg,IDC_RMFONT,CB_SETCURSEL,idx,0);
		SetDlgItemInt(hDlg,IDC_RMLINERATIO,Option.RmLineRatio,FALSE);
		sprintf(szCap,"�ֱ�:%.1f��",(double)Option.RmFreq/10);
		SetDlgItemText(hDlg,IDC_RMFREQ,szCap);
		wsprintf(szCap,"�ټ�:%d",Option.RmLineNum);
		SetDlgItemText(hDlg,IDC_RMLINENUM,szCap);
		SetDlgItemInt(hDlg,IDC_RMMANUAL,Option.RmManual,FALSE);
		CheckDlgButton(hDlg,IDC_RMWRAP,Option.RmWrap ? BST_CHECKED:BST_UNCHECKED);
		SendDlgItemMessage(hDlg,IDC_RMSPINFONTSIZE,UDM_SETRANGE,0,MAKELPARAM(72,8));
		SendDlgItemMessage(hDlg,IDC_RMSPINLINERATIO,UDM_SETRANGE,0,MAKELPARAM(500,100));
		SendDlgItemMessage(hDlg,IDC_RMSPINMANUAL,UDM_SETRANGE,0,MAKELPARAM(100,1));
		Accel.nSec=1;
		Accel.nInc=10;
		SendDlgItemMessage(hDlg,IDC_RMSPINLINERATIO,UDM_SETACCEL,1,(LPARAM)&Accel);
		SendDlgItemMessage(hDlg,IDC_RMSLFREQ,TBM_SETRANGE,FALSE,MAKELPARAM(1,300));
		SendDlgItemMessage(hDlg,IDC_RMSLFREQ,TBM_SETPOS,TRUE,Option.RmFreq);
		SendDlgItemMessage(hDlg,IDC_RMSLLINENUM,TBM_SETRANGE,FALSE,MAKELPARAM(1,10));
		SendDlgItemMessage(hDlg,IDC_RMSLLINENUM,TBM_SETPOS,TRUE,Option.RmLineNum);

		// ApiEdit�� �ɼ� ����
		SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_RMWRAP,0),0);
		SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_RMLINERATIO,EN_CHANGE),0);
		SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_RMFONTSIZE,EN_CHANGE),0);

		// ��ȭ������ ��ġ ����
		GetWindowRect(GetParent(hDlg),&wrt);
		GetWindowRect(hDlg,&drt);
		dx=wrt.right;
		dy=wrt.bottom-(drt.bottom-drt.top);
		if (dx+(drt.right-drt.left) > GetSystemMetrics(SM_CXSCREEN)) {
			dx=GetSystemMetrics(SM_CXSCREEN)-(drt.right-drt.left);
		}
		if (dy+(drt.bottom-drt.top) > GetSystemMetrics(SM_CYSCREEN)) {
			dy=GetSystemMetrics(SM_CYSCREEN)-(drt.bottom-drt.top);
		}
		SetWindowPos(hDlg,HWND_NOTOPMOST,dx,dy,0,0,SWP_NOSIZE);

		SetFocus(GetDlgItem(hDlg,IDC_RMAUTOSCROLL));
		return FALSE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_RMAUTOSCROLL:
			if (IsDlgButtonChecked(hDlg,IDC_RMAUTOSCROLL)==BST_CHECKED) {
				Value=SendMessage(GetDlgItem(hDlg,IDC_RMSLFREQ),TBM_GETPOS,0,0);
				SetTimer(hDlg,1,Value*100,NULL);
			} else {
				KillTimer(hDlg,1);
			}
			return TRUE;
		case IDC_RMWRAP:
			if (IsDlgButtonChecked(hDlg,IDC_RMWRAP)==BST_CHECKED) {
				Ae->SetWrap(Option.nWrap);
				Option.RmWrap=TRUE;
			} else {
				Ae->SetWrap(0);
				Option.RmWrap=FALSE;
			}
			return TRUE;
		case IDC_RMFONT:
			if (HIWORD(wParam)==CBN_SELCHANGE) {
				idx=SendDlgItemMessage(hDlg,IDC_RMFONT,CB_GETCURSEL,0,0);
				if (idx==0) {
					Option.Rmlogfont.lfHeight=0;
				} else {
					SendDlgItemMessage(hDlg,IDC_RMFONT,CB_GETLBTEXT,idx,(LPARAM)szCap);
					for (i=0;i<FontNum;i++) {
						if (lstrcmp(szCap,arFont[i].lfFaceName) == 0)
							break;
					}
					Option.Rmlogfont=arFont[i];
					Option.Rmlogfont.lfHeight=GetDlgItemInt(hDlg,IDC_RMFONTSIZE,NULL,FALSE);
				}
				tFont=Option.Rmlogfont;
				hdc=GetDC(NULL);
				tFont.lfHeight=tFont.lfHeight*GetDeviceCaps(hdc,LOGPIXELSY)/72;
				tFont.lfWidth=0;
				ReleaseDC(NULL,hdc);
				Ae->SetFont(&tFont);
			}
			return TRUE;
		case IDC_RMFONTSIZE:
			if (HIWORD(wParam)==EN_CHANGE) {
				Value=GetDlgItemInt(hDlg,IDC_RMFONTSIZE,NULL,FALSE);
				if (Value) {
					Option.Rmlogfont.lfHeight=Value;
					tFont=Option.Rmlogfont;
					hdc=GetDC(NULL);
					tFont.lfHeight=tFont.lfHeight*GetDeviceCaps(hdc,LOGPIXELSY)/72;
					tFont.lfWidth=0;
					ReleaseDC(NULL,hdc);
					Ae->SetFont(&tFont);
				}
			}
			return TRUE;
		case IDC_RMLINERATIO:
			if (HIWORD(wParam)==EN_CHANGE) {
				Value=GetDlgItemInt(hDlg,IDC_RMLINERATIO,NULL,FALSE);
				if (Value) {
					Option.RmLineRatio=Value;
					Ae->SetLineRatio(Value);
				}
			}
			return TRUE;
		case IDC_RMMANUAL:
			if (HIWORD(wParam)==EN_CHANGE) {
				Value=GetDlgItemInt(hDlg,IDC_RMMANUAL,NULL,FALSE);
				if (Value) {
					Option.RmManual=Value;
				}
			}
			return TRUE;
		case IDC_RMBTNMANUAL:
			Value=GetDlgItemInt(hDlg,IDC_RMMANUAL,NULL,FALSE);
			Ae->GetActiveView()->Scroll(Value);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg,IDCANCEL);
			return TRUE;
		}
		return FALSE;
	case WM_TIMER:
		Ae->GetActiveView()->Scroll(Option.RmLineNum);
		return TRUE;
	case WM_HSCROLL:
		if ((HWND)lParam == GetDlgItem(hDlg,IDC_RMSLFREQ)) {
			Option.RmFreq=SendMessage(GetDlgItem(hDlg,IDC_RMSLFREQ),TBM_GETPOS,0,0);
			sprintf(szCap,"�ֱ�:%.1f��",(double)Option.RmFreq/10);
			SetDlgItemText(hDlg,IDC_RMFREQ,szCap);
			SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_RMAUTOSCROLL,0),0);
		}
		if ((HWND)lParam == GetDlgItem(hDlg,IDC_RMSLLINENUM)) {
			Option.RmLineNum=SendMessage(GetDlgItem(hDlg,IDC_RMSLLINENUM),TBM_GETPOS,0,0);
			wsprintf(szCap,"�ټ�:%d",Option.RmLineNum);
			SetDlgItemText(hDlg,IDC_RMLINENUM,szCap);
		}
		return TRUE;
	}
	return FALSE;
}

SParaOption::SParaOption()
{
	startpage=0;
	p1_Type=1;
	lstrcpy(p1_szRep,"");
	p1_MultiEnter=0;
	p2_Type=0;
	p2_nChar=80;
	p2_nPixel=640;
	p2_bIndent=FALSE;
	p2_nIndent=4;
	p2_bRemoveSpace=TRUE;
	p3_Orient=0;
	p3_Type=1;
	p3_nChar=80;
	p3_nPixel=640;
	p4_Type=0;
	p4_Case=FALSE;
	p4_Unique=FALSE;
	p5_Type=1;
	p5_Num=1;
}

BOOL CALLBACK ParaDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	TCITEM tie;
	static HWND hTabDlg;
	HWND hActive;
	SInfo *pSi;
	TCHAR szTemp[256];
	int iTemp;
	RECT art,drt,irt;

	switch(iMessage) {
	case WM_INITDIALOG:
		MoveToParentCenter(hDlg);
		hActive=(HWND)SendMessage(g_hMDIClient,WM_MDIGETACTIVE,0,NULL);
		pSi=(SInfo *)GetWindowLong(hActive,0);
		GetWindowRect(pSi->Ae.GetActiveView()->hWnd,&art);
		GetWindowRect(hDlg,&drt);
		IntersectRect(&irt,&art,&drt);
		if (!IsRectEmpty(&irt)) {
			// ���ʿ� ������ ������ ���ʿ� ���� �ٿ� ����.
			if (art.left > (drt.right-drt.left)) {
				drt.left=art.left-(drt.right-drt.left);
			// �ƴϸ� �����ʿ� ���̵� ȭ���� ������� �ʵ��� �Ѵ�.
			} else {
				drt.left=min(art.right,GetSystemMetrics(SM_CXSCREEN)-(drt.right-drt.left));
			}
			SetWindowPos(hDlg,HWND_NOTOPMOST,drt.left,drt.top,0,0,SWP_NOSIZE);
		}

		hParaTab=GetDlgItem(hDlg,IDC_PARATAB);
		tie.mask=TCIF_TEXT;
		tie.pszText="����";
		TabCtrl_InsertItem(hParaTab,0,&tie);
		tie.pszText="����";
		TabCtrl_InsertItem(hParaTab,1,&tie);
		tie.pszText="���� ����";
		TabCtrl_InsertItem(hParaTab,2,&tie);
		tie.pszText="ũ��� ����";
		TabCtrl_InsertItem(hParaTab,3,&tie);
		tie.pszText="���� ����";
		TabCtrl_InsertItem(hParaTab,4,&tie);

		// ���� �ٲ㵵 ���� �޽����� ������ �����Ƿ� ���� ������ �Ѵ�.
		TabCtrl_SetCurSel(hParaTab,ParaOption.startpage);
		SendMessage(hDlg,WM_USER+1,ParaOption.startpage,0);
		return TRUE;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code) {
		case TCN_SELCHANGE:
			SendMessage(hDlg,WM_USER+1,TabCtrl_GetCurSel(hParaTab),0);
			break;
		}
		return TRUE;
	case WM_USER+1:
		if (hTabDlg) {
			DestroyWindow(hTabDlg);
		}
		switch (wParam) {
		case 0:
			hTabDlg=CreateDialog(g_hInst,MAKEINTRESOURCE(IDD_PARA1),hDlg,Para1Proc);
			break;
		case 1:
			hTabDlg=CreateDialog(g_hInst,MAKEINTRESOURCE(IDD_PARA2),hDlg,Para2Proc);
			break;
		case 2:
			hTabDlg=CreateDialog(g_hInst,MAKEINTRESOURCE(IDD_PARA3),hDlg,Para3Proc);
			break;
		case 3:
			hTabDlg=CreateDialog(g_hInst,MAKEINTRESOURCE(IDD_PARA4),hDlg,Para4Proc);
			break;
		case 4:
			hTabDlg=CreateDialog(g_hInst,MAKEINTRESOURCE(IDD_PARA5),hDlg,Para5Proc);
			break;
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
		case IDC_BTNAPPLY:
			hActive=(HWND)SendMessage(g_hMDIClient,WM_MDIGETACTIVE,0,NULL);
			pSi=(SInfo *)GetWindowLong(hActive,0);
			switch (TabCtrl_GetCurSel(hParaTab)) {
			case 0:
				ParaOption.p1_MultiEnter=SendDlgItemMessage(hTabDlg,IDC_P1_CB1,CB_GETCURSEL,0,0);
				if (IsDlgButtonChecked(hTabDlg,IDC_P1_RA1)==BST_CHECKED) {
					ParaOption.p1_Type=0;
					lstrcpy(szTemp,"");
				}
				if (IsDlgButtonChecked(hTabDlg,IDC_P1_RA2)==BST_CHECKED) {
					ParaOption.p1_Type=1;
					lstrcpy(szTemp," ");
				}
				if (IsDlgButtonChecked(hTabDlg,IDC_P1_RA3)==BST_CHECKED) {
					ParaOption.p1_Type=2;
					lstrcpy(szTemp,"\t");
				}
				if (IsDlgButtonChecked(hTabDlg,IDC_P1_RA4)==BST_CHECKED) {
					ParaOption.p1_Type=3;
					GetDlgItemText(hTabDlg,IDC_P1_ED1,ParaOption.p1_szRep,32);
					lstrcpy(szTemp,ParaOption.p1_szRep);
				}

				pSi->Ae.GetActiveView()->MergePara(ParaOption.p1_MultiEnter,szTemp);
				break;
			case 1:
				if (IsDlgButtonChecked(hTabDlg,IDC_P2_RA1)==BST_CHECKED) {
					ParaOption.p2_Type=0;
				}
				if (IsDlgButtonChecked(hTabDlg,IDC_P2_RA2)==BST_CHECKED) {
					ParaOption.p2_Type=1;
					ParaOption.p2_nChar=iTemp=GetDlgItemInt(hTabDlg,IDC_P2_ED1,NULL,FALSE);
					if (ParaOption.p2_nChar < 2) {
						MessageBox(hDlg,"���ڼ��� �ּ��� 2�� �̻��̾�� �մϴ�","�˸�",MB_OK);
						return TRUE;
					}
				}
				if (IsDlgButtonChecked(hTabDlg,IDC_P2_RA3)==BST_CHECKED) {
					ParaOption.p2_Type=2;
					ParaOption.p2_nPixel=iTemp=GetDlgItemInt(hTabDlg,IDC_P2_ED1,NULL,FALSE);
					if (ParaOption.p2_nPixel < 100) {
						MessageBox(hDlg,"ȭ������ �ּ��� 100�ȼ� �̻��̾�� �մϴ�","�˸�",MB_OK);
						return TRUE;
					}
				}
				if (IsDlgButtonChecked(hTabDlg,IDC_P2_RA4)==BST_CHECKED) {
					ParaOption.p2_Type=3;
				}
				if (IsDlgButtonChecked(hTabDlg,IDC_P2_CHK1)==BST_CHECKED) {
					ParaOption.p2_bIndent=TRUE;
					ParaOption.p2_nIndent=GetDlgItemInt(hTabDlg,IDC_P2_ED2,NULL,TRUE);
					if (ParaOption.p2_nIndent > 32 || ParaOption.p2_nIndent < -32) {
						MessageBox(hDlg,"�鿩����(�����)�� -32~32���̿��� �մϴ�.","�˸�",MB_OK);
						return TRUE;
					}
				} else {
					ParaOption.p2_bIndent=FALSE;
				}
				ParaOption.p2_bRemoveSpace=!(IsDlgButtonChecked(hTabDlg,IDC_P2_CHK2)==BST_CHECKED);
				pSi->Ae.GetActiveView()->SplitPara(ParaOption.p2_Type,iTemp,ParaOption.p2_bRemoveSpace,
					ParaOption.p2_bIndent ? ParaOption.p2_nIndent:0);
				break;
			case 2:
				if (IsDlgButtonChecked(hTabDlg,IDC_P3_RA1)==BST_CHECKED) {
					ParaOption.p3_Orient=0;
				}
				if (IsDlgButtonChecked(hTabDlg,IDC_P3_RA2)==BST_CHECKED) {
					ParaOption.p3_Orient=1;
				}
				if (IsDlgButtonChecked(hTabDlg,IDC_P3_RA3)==BST_CHECKED) {
					ParaOption.p3_Orient=2;
				}
				if (IsDlgButtonChecked(hTabDlg,IDC_P3_RA4)==BST_CHECKED) {
					ParaOption.p3_Type=1;
					ParaOption.p3_nChar=iTemp=GetDlgItemInt(hTabDlg,IDC_P3_ED1,NULL,FALSE);
					if (iTemp < 2) {
						MessageBox(hDlg,"���ڼ��� �ּ��� 2�� �̻��̾�� �մϴ�","�˸�",MB_OK);
						return TRUE;
					}
				}
				if (IsDlgButtonChecked(hTabDlg,IDC_P3_RA5)==BST_CHECKED) {
					ParaOption.p3_Type=2;
					ParaOption.p3_nPixel=iTemp=GetDlgItemInt(hTabDlg,IDC_P3_ED1,NULL,FALSE);
					if (iTemp < 100) {
						MessageBox(hDlg,"ȭ������ �ּ��� 100�ȼ� �̻��̾�� �մϴ�","�˸�",MB_OK);
						return TRUE;
					}
				}
				pSi->Ae.GetActiveView()->AlignPara(ParaOption.p3_Orient,ParaOption.p3_Type,iTemp);
				break;
			case 3:
				if (IsDlgButtonChecked(hTabDlg,IDC_P4_RA1)==BST_CHECKED) {
					ParaOption.p4_Type=0;
				}
				if (IsDlgButtonChecked(hTabDlg,IDC_P4_RA2)==BST_CHECKED) {
					ParaOption.p4_Type=1;
				}
				ParaOption.p4_Case=IsDlgButtonChecked(hTabDlg,IDC_P4_CHK1);
				ParaOption.p4_Unique=IsDlgButtonChecked(hTabDlg,IDC_P4_CHK2);
				pSi->Ae.GetActiveView()->SortPara(ParaOption.p4_Type,ParaOption.p4_Case,ParaOption.p4_Unique);
				break;
			case 4:
				if (IsDlgButtonChecked(hTabDlg,IDC_P5_RA1)==BST_CHECKED) {
					ParaOption.p5_Type=0;
				}
				if (IsDlgButtonChecked(hTabDlg,IDC_P5_RA2)==BST_CHECKED) {
					ParaOption.p5_Type=1;
				}
				if (IsDlgButtonChecked(hTabDlg,IDC_P5_RA3)==BST_CHECKED) {
					ParaOption.p5_Type=2;
				}
				ParaOption.p5_Num=GetDlgItemInt(hTabDlg,IDC_P5_ED1,NULL,FALSE);
				if (ParaOption.p5_Num < 0 || ParaOption.p5_Num > 16) {
					MessageBox(hDlg,"�� �� ���� 0~16���̿��� �մϴ�","�˸�",MB_OK);
					return TRUE;
				}
				pSi->Ae.GetActiveView()->ArrangeEmptyPara(ParaOption.p5_Type,ParaOption.p5_Num);
				break;
			}
			if (LOWORD(wParam)==IDOK) {
				EndDialog(hDlg,IDOK);
			}
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg,IDCANCEL);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CALLBACK Para1Proc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	RECT prt;

	switch(iMessage) {
	case WM_INITDIALOG:
		ParaOption.startpage=0;
		GetWindowRect(hParaTab,&prt);
		TabCtrl_AdjustRect(hParaTab,FALSE,&prt);
		ScreenToClient(GetParent(hDlg),(LPPOINT)&prt);
		SetWindowPos(hDlg,HWND_TOP,prt.left,prt.top,0,0,SWP_NOSIZE);

		SendDlgItemMessage(hDlg,IDC_P1_CB1,CB_ADDSTRING,0,(LPARAM)"������ ��ü");
		SendDlgItemMessage(hDlg,IDC_P1_CB1,CB_ADDSTRING,0,(LPARAM)"������ ����");
		SendDlgItemMessage(hDlg,IDC_P1_CB1,CB_ADDSTRING,0,(LPARAM)"������� ����");
		SendDlgItemMessage(hDlg,IDC_P1_CB1,CB_SETCURSEL,ParaOption.p1_MultiEnter,0);
		CheckRadioButton(hDlg,IDC_P1_RA1,IDC_P1_RA4,IDC_P1_RA1+ParaOption.p1_Type);
		SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_P1_RA1+ParaOption.p1_Type,BN_CLICKED),0);
		SetDlgItemText(hDlg,IDC_P1_ED1,ParaOption.p1_szRep);
		SendDlgItemMessage(hDlg,IDC_P1_ED1,EM_LIMITTEXT,31,0);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_P1_RA1:
		case IDC_P1_RA2:
		case IDC_P1_RA3:
			EnableWindow(GetDlgItem(hDlg,IDC_P1_ED1),FALSE);
			break;
		case IDC_P1_RA4:
			EnableWindow(GetDlgItem(hDlg,IDC_P1_ED1),TRUE);
			break;
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CALLBACK Para2Proc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	RECT prt;

	switch(iMessage) {
	case WM_INITDIALOG:
		ParaOption.startpage=1;
		GetWindowRect(hParaTab,&prt);
		TabCtrl_AdjustRect(hParaTab,FALSE,&prt);
		ScreenToClient(GetParent(hDlg),(LPPOINT)&prt);
		SetWindowPos(hDlg,HWND_TOP,prt.left,prt.top,0,0,SWP_NOSIZE);

		CheckRadioButton(hDlg,IDC_P2_RA1,IDC_P2_RA4,IDC_P2_RA1+ParaOption.p2_Type);
		SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_P2_RA1+ParaOption.p2_Type,BN_CLICKED),0);
		SendDlgItemMessage(hDlg,IDC_P2_SPIN2,UDM_SETRANGE,0,MAKELPARAM(32,-32));
		SetDlgItemInt(hDlg,IDC_P2_ED2,ParaOption.p2_nIndent,TRUE);
		CheckDlgButton(hDlg,IDC_P2_CHK1,(ParaOption.p2_bIndent ? BST_CHECKED:BST_UNCHECKED));
		CheckDlgButton(hDlg,IDC_P2_CHK2,(ParaOption.p2_bRemoveSpace ? BST_UNCHECKED:BST_CHECKED));
		SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_P2_CHK1,BN_CLICKED),0);
		return TRUE;
	case WM_COMMAND:
		// ��ħǥ, ȭ���� �� ���� �鿩���� �� �� ����. ���ڼ��� �ȼ����� �Է��� �ʿ䵵 ����
		if (LOWORD(wParam)==IDC_P2_RA1 || LOWORD(wParam)==IDC_P2_RA4) {
			CheckDlgButton(hDlg,IDC_P2_CHK1,BST_UNCHECKED);
			EnableWindow(GetDlgItem(hDlg,IDC_P2_CHK1),FALSE);
			SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_P2_CHK1,BN_CLICKED),0);
			EnableWindow(GetDlgItem(hDlg,IDC_P2_ED1),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_P2_SPIN1),FALSE);
		} else if (LOWORD(wParam)==IDC_P2_RA2 || LOWORD(wParam)==IDC_P2_RA3) {
			EnableWindow(GetDlgItem(hDlg,IDC_P2_CHK1),TRUE);
			SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_P2_CHK1,BN_CLICKED),0);
			EnableWindow(GetDlgItem(hDlg,IDC_P2_ED1),TRUE);
			EnableWindow(GetDlgItem(hDlg,IDC_P2_SPIN1),TRUE);
		}

		switch (LOWORD(wParam)) {
		case IDC_P2_RA2:
			SetDlgItemInt(hDlg,IDC_P2_ED1,ParaOption.p2_nChar,FALSE);
			SetDlgItemText(hDlg,IDC_P2_ST1,"����");
			SendDlgItemMessage(hDlg,IDC_P2_SPIN1,UDM_SETRANGE,0,MAKELPARAM(1024,2));
			break;
		case IDC_P2_RA3:
			SetDlgItemInt(hDlg,IDC_P2_ED1,ParaOption.p2_nPixel,FALSE);
			SetDlgItemText(hDlg,IDC_P2_ST1,"�ȼ�");
			SendDlgItemMessage(hDlg,IDC_P2_SPIN1,UDM_SETRANGE,0,MAKELPARAM(4096,100));
			break;
		case IDC_P2_CHK1:
			if (IsDlgButtonChecked(hDlg,IDC_P2_CHK1)==BST_CHECKED) {
				EnableWindow(GetDlgItem(hDlg,IDC_P2_ED2),TRUE);
				EnableWindow(GetDlgItem(hDlg,IDC_P2_SPIN2),TRUE);
				CheckDlgButton(hDlg,IDC_P2_CHK2,BST_UNCHECKED);
				EnableWindow(GetDlgItem(hDlg,IDC_P2_CHK2),FALSE);
			} else {
				EnableWindow(GetDlgItem(hDlg,IDC_P2_ED2),FALSE);
				EnableWindow(GetDlgItem(hDlg,IDC_P2_SPIN2),FALSE);
				EnableWindow(GetDlgItem(hDlg,IDC_P2_CHK2),TRUE);
			}
			break;
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CALLBACK Para3Proc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	RECT prt;

	switch(iMessage) {
	case WM_INITDIALOG:
		ParaOption.startpage=2;
		GetWindowRect(hParaTab,&prt);
		TabCtrl_AdjustRect(hParaTab,FALSE,&prt);
		ScreenToClient(GetParent(hDlg),(LPPOINT)&prt);
		SetWindowPos(hDlg,HWND_TOP,prt.left,prt.top,0,0,SWP_NOSIZE);

		CheckRadioButton(hDlg,IDC_P3_RA1,IDC_P3_RA3,IDC_P3_RA1+ParaOption.p3_Orient);
		CheckRadioButton(hDlg,IDC_P3_RA4,IDC_P3_RA5,IDC_P3_RA4+ParaOption.p3_Type-1);
		SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_P3_RA4+ParaOption.p3_Type-1,BN_CLICKED),0);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_P3_RA4:
			SetDlgItemInt(hDlg,IDC_P3_ED1,ParaOption.p3_nChar,FALSE);
			SetDlgItemText(hDlg,IDC_P3_ST1,"����");
			SendDlgItemMessage(hDlg,IDC_P3_SPIN1,UDM_SETRANGE,0,MAKELPARAM(1024,2));
			return TRUE;
		case IDC_P3_RA5:
			SetDlgItemInt(hDlg,IDC_P3_ED1,ParaOption.p3_nPixel,FALSE);
			SetDlgItemText(hDlg,IDC_P3_ST1,"�ȼ�");
			SendDlgItemMessage(hDlg,IDC_P3_SPIN1,UDM_SETRANGE,0,MAKELPARAM(4096,100));
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CALLBACK Para4Proc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	RECT prt;

	switch(iMessage) {
	case WM_INITDIALOG:
		ParaOption.startpage=3;
		GetWindowRect(hParaTab,&prt);
		TabCtrl_AdjustRect(hParaTab,FALSE,&prt);
		ScreenToClient(GetParent(hDlg),(LPPOINT)&prt);
		SetWindowPos(hDlg,HWND_TOP,prt.left,prt.top,0,0,SWP_NOSIZE);

		CheckRadioButton(hDlg,IDC_P4_RA1,IDC_P4_RA2,IDC_P4_RA1+ParaOption.p4_Type);
		CheckDlgButton(hDlg,IDC_P4_CHK1,ParaOption.p4_Case);
		CheckDlgButton(hDlg,IDC_P4_CHK2,ParaOption.p4_Unique);
		return TRUE;
	}
	return FALSE;
}

BOOL CALLBACK Para5Proc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	RECT prt;

	switch(iMessage) {
	case WM_INITDIALOG:
		ParaOption.startpage=4;
		GetWindowRect(hParaTab,&prt);
		TabCtrl_AdjustRect(hParaTab,FALSE,&prt);
		ScreenToClient(GetParent(hDlg),(LPPOINT)&prt);
		SetWindowPos(hDlg,HWND_TOP,prt.left,prt.top,0,0,SWP_NOSIZE);

		CheckRadioButton(hDlg,IDC_P5_RA1,IDC_P5_RA3,IDC_P5_RA1+ParaOption.p5_Type);
		SetDlgItemInt(hDlg,IDC_P5_ED1,ParaOption.p5_Num,FALSE);
		SendDlgItemMessage(hDlg,IDC_P5_SPIN1,UDM_SETRANGE,0,MAKELPARAM(16,0));
		return TRUE;
	}
	return FALSE;
}

void InstantSearch(BOOL bStart)
{
	RECT prt;
	HWND hActive;
	TCHAR szTemp[256];

	hActive=(HWND)SendMessage(g_hMDIClient,WM_MDIGETACTIVE,0,NULL);
	if (hActive==NULL) {
		return;
	}

	if (bStart) {
		SetStatusText(1,"�ٷ� ã��");
		SendMessage(hStatus,SB_GETRECT,0,(LPARAM)&prt);
		hInstEdit=CreateWindowEx(WS_EX_STATICEDGE,"Edit",NULL,
			WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
			prt.left+60,prt.top+1,130,prt.bottom-prt.top-2,
			hStatus,(HMENU)0,g_hInst,NULL);
		OldInstEditProc=(WNDPROC)SetWindowLongPtr(hInstEdit,GWLP_WNDPROC,(LONG_PTR)InstEditProc);
		SendMessage(hInstEdit,WM_SETFONT,(WPARAM)hGul9,MAKELPARAM(FALSE,0));
		SendMessage(hInstEdit,EM_LIMITTEXT,255,0);
		SetFocus(hInstEdit);
	} else {
		GetWindowText(hInstEdit,szTemp,256);
		if (lstrlen(szTemp)) {
			arFind[0].Add(szTemp);
		}
		SetStatusText(1,NULL);
		SetWindowLongPtr(hInstEdit,GWLP_WNDPROC,(LONG_PTR)OldInstEditProc);
		DestroyWindow(hInstEdit);
		hInstEdit=NULL;
		SetFocus(hActive);
	}
}

LRESULT CALLBACK InstEditProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	LRESULT Result;
	DWORD tFlag;

	switch (iMessage) {
	case WM_CHAR:
		if (wParam=='\t')
			return TRUE;
		Result=CallWindowProc(OldInstEditProc,hWnd,iMessage,wParam,lParam);
		InstSrchNow(hWnd,0,FindFlag);
		return Result;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_RETURN:
		case VK_ESCAPE:
			InstantSearch(FALSE);
			return TRUE;
		case VK_TAB:
			SetWindowText(hWnd,"");
			return TRUE;
		case VK_DOWN:
			tFlag=FindFlag;
			tFlag &= ~AE_FIND_UP;
			InstSrchNow(hWnd,-1,tFlag);
			return TRUE;
		case VK_UP:
			tFlag=FindFlag;
			tFlag |= AE_FIND_UP;
			InstSrchNow(hWnd,-1,tFlag);
			return TRUE;
		}
		break;
	case WM_KILLFOCUS:
		InstantSearch(FALSE);
		return TRUE;
	}
	return CallWindowProc(OldInstEditProc,hWnd,iMessage,wParam,lParam);
}

void InstSrchNow(HWND hEdit,int Start,DWORD Flag)
{
	TCHAR szTemp[256];
	HWND hActive;
	SInfo *pSi;

	hActive=(HWND)SendMessage(g_hMDIClient,WM_MDIGETACTIVE,0,NULL);
	if (hActive!=NULL) {
		pSi=(SInfo *)GetWindowLong(hActive,0);

		GetWindowText(hEdit,szTemp,256);
		if (pSi->Ae.FindText(Start,szTemp,Flag) == FALSE) {
			if (Option.bSoundFindFail) {
				MessageBeep(MB_ICONASTERISK);
			}
		}
	}
}

BOOL CALLBACK ShareNoticeProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage) {
	case WM_INITDIALOG:
		MoveToParentCenter(hDlg);
		SetTimer(hDlg,1,3000,NULL);
		return TRUE;
	case WM_TIMER:
		EnableWindow(GetDlgItem(hDlg,IDOK),TRUE);
		EnableWindow(GetDlgItem(hDlg,IDCANCEL),TRUE);
		SetFocus(GetDlgItem(hDlg,IDOK));
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
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


