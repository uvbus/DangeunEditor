//#define _WIN32_WINNT 0x400
//#define _WIN32_WINDOWS 0x401
//#include <windows.h>
//#include <windowsx.h>
//#include <imm.h>
//#include <stdio.h>
//#include <io.h>
//#include <malloc.h>
//#include <commctrl.h>
//#include <Shlwapi.h>
//#include <Shlobj.h>
//#include <wininet.h>
//#include <stdio.h>
//#include "Parse.h"
//#include "AeUtil.h"
//#include "ApiEdit.h"
//#include "ApiEditText.h"
//#include "ApiEditHexa.h"
#include "stdafx.h"

LRESULT CALLBACK ApiEditProc(HWND,UINT,WPARAM,LPARAM);

HCURSOR CApiEdit::hCSel;
HCURSOR CApiEdit::hCCopy;
HCURSOR CApiEdit::hCMove;
HCURSOR CApiEdit::hCMargin;
HCURSOR CApiEdit::hCNoDrop;
HFONT CApiEdit::hLineNumFont;
UINT CApiEdit::AeColFormat;

class CRegisterHelper
{
public:
	CRegisterHelper();
	~CRegisterHelper();

	struct _arObj
	{
		CWindow *pObj;
		HWND hWnd;
	} *arObj;
	int arSize;
	int nReg;

	CWindow *FindObject(HWND hWnd);
	void AddObject(HWND hWnd, CWindow *pObj);
	void RemoveObject(HWND hWnd);
};

CRegisterHelper::CRegisterHelper()
{
	WNDCLASS WndClass;

	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hbrBackground=NULL;
	WndClass.hCursor=LoadCursor(NULL,IDC_IBEAM);
	WndClass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	WndClass.hInstance=GetModuleHandle(NULL);
	WndClass.lpfnWndProc=(WNDPROC)ApiEditProc;
	WndClass.lpszClassName="ApiEdit";
	WndClass.lpszMenuName=NULL;
	WndClass.style=CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	RegisterClass(&WndClass);

	WndClass.lpszClassName="ApiEditText";
	RegisterClass(&WndClass);

	nReg=0;
	arSize=10;
	arObj=(_arObj *)malloc(arSize*sizeof(_arObj));
	memset(arObj,0,arSize*sizeof(_arObj));
	CApiEdit::LoadAeResource();
	CApiEdit::RegisterAeColFormat();
}

CRegisterHelper::~CRegisterHelper()
{
	free(arObj);
	arObj=NULL;
	CApiEdit::UnLoadAeResource();
}

CWindow *CRegisterHelper::FindObject(HWND hWnd)
{
	int i;

	for (i=0;i<nReg;i++) {
		if (arObj[i].hWnd == hWnd)
			return arObj[i].pObj;
	}
	return NULL;
}

void CRegisterHelper::AddObject(HWND hWnd, CWindow *pObj)
{
	int i;

	if (nReg == arSize-1) {
		arSize++;
		arObj=(_arObj *)realloc(arObj,arSize*sizeof(_arObj));
		arObj[arSize-1].hWnd=NULL;
		arObj[arSize-1].pObj=NULL;
	}

	for (i=0;;i++) {
		if (arObj[i].hWnd == NULL)
			break;
	}
	arObj[i].hWnd=hWnd;
	arObj[i].pObj=pObj;
	pObj->hWnd=hWnd;
	nReg++;
}

void CRegisterHelper::RemoveObject(HWND hWnd)
{
	int i,j;

	if (IsWindow(hWnd)) {
		DestroyWindow(hWnd);
	}
	for (i=0;i<nReg;i++) {
		if (arObj[i].hWnd == hWnd)
			break;
	}
	for (j=i+1;j<arSize;j++) {
		arObj[j-1].hWnd=arObj[j].hWnd;
		arObj[j-1].pObj=arObj[j].pObj;
	}
	nReg--;
}

CRegisterHelper _RegisterHelper;

CWindow::~CWindow()
{
	if (_RegisterHelper.arObj)
		_RegisterHelper.RemoveObject(hWnd);
}

LRESULT CALLBACK ApiEditProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	CWindow *pAE;

	pAE=_RegisterHelper.FindObject(hWnd);
	if (pAE == NULL) {
		pAE=(CWindow *)((LPCREATESTRUCT)lParam)->lpCreateParams;
		_RegisterHelper.AddObject(hWnd,pAE);
	}

	return pAE->OnMessage(iMessage, wParam, lParam);
}

CApiEdit::CApiEdit()
{
	int i;

	buf=NULL;
	pUR=NULL;
	Style=AES_WANTTAB | AES_ALLOWDRAG | AES_AUTOINDENT | AES_BLOCKINDENTWITHTAB | 
		AES_HOMETOFIRSTCHAR | AES_HIDECURLINE | AES_CALCTABWITHAVG | AES_HORZSPLITABLE | 
		AES_VERTSPLITABLE |	AES_AUTOCONVERT | AES_USEAUTOCLIP | AES_AUTOCLIPBYTAB | 
		AES_SHOWMARGIN;

	UndoSize=100*1024;
	UndoMin=100;
	nShowCurLine=0;
	HideSelType=1;
	ActiveView=0;
	for (i=0;i<4;i++) {
		arView[i]=NULL;
	}
	SetColor(AECOLOR_MAR1,RGB(192,192,192));
	SetColor(AECOLOR_MAR2,RGB(160,160,160));
	SetColor(AECOLOR_NUM,RGB(0,0,0));
	SetColor(AECOLOR_MARK,RGB(255,255,0));
	SetColor(AECOLOR_CODE,RGB(128,128,128));
	SetColor(AECOLOR_CUR,RGB(255,255,0));
	SetColor(AECOLOR_FORE,-1);
	SetColor(AECOLOR_BACK,-1);
	SetColor(AECOLOR_SELFORE,-1);
	SetColor(AECOLOR_SELBACK,-1);
	SetColor(AECOLOR_INNERRECT,-1);
	SetColor(AECOLOR_RULER,RGB(220,220,220));
	SetColor(AECOLOR_SELFRAME,RGB(0,0,0));
	CaretWidth=2;
	VertRatio=0;
	HorzRatio=0;
	iDrag=0;
	UserBase=WM_USER+629;
	FixedFont.lfHeight=0; 
	SetRect(&InnerRect,1,2,0,0);
	VAlign=1;
	SelBlockType=1;
	hBackBit=NULL;
	BackBitPos=0;
	nWheel=1;
	nCtrlWheel=6;
	nShiftWheel=5;
	nCtrlShiftWheel=7;
	CharExtra=100;
	WordExtra=100;
	CharRatio=100;
	CursorEdit=0;
	ShowTabType=1;
	ShowEnterType=1;
	ShowSpaceType=0;
}

BOOL CApiEdit::Create(int x,int y,int w,int h,DWORD style,UINT id,HWND hParent)
{
	CreateWindow("ApiEdit",NULL, style | WS_CLIPCHILDREN,
		x,y,w,h,hParent,(HMENU)id,GetModuleHandle(NULL),this);
	return TRUE;
}

LRESULT CApiEdit::OnMessage(UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage) {
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
		HANDLE_MSG(hWnd, WM_SETFOCUS, OnSetFocus);
		HANDLE_MSG(hWnd, WM_LBUTTONDOWN, OnLButtonDown);
		HANDLE_MSG(hWnd, WM_LBUTTONDBLCLK, OnLButtonDown);
		HANDLE_MSG(hWnd, WM_MOUSEMOVE, OnMouseMove);
		HANDLE_MSG(hWnd, WM_LBUTTONUP, OnLButtonUp);
		HANDLE_MSG(hWnd, WM_SIZE, OnSize);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hWnd, WM_SETCURSOR, OnSetCursor);
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}

///////////////////////////////////////////////////////////////////////////////////
// �޽��� �ڵ鷯

BOOL CApiEdit::OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	InitDoc();
	arView[0]=CreateView(TEXT_VIEW);

	return TRUE;
}

void CApiEdit::OnDestroy(HWND hWnd)
{
	int i;

	for (i=0;i<4 && arView[i];i++) {
		DeleteView(i);
	}
	free(buf);
	if (pUR) {
		for (i=0;;i++) {
			if (pUR[i].action == UR_NONE)
				break;
			if (pUR[i].data)
				free(pUR[i].data);
		}
		free(pUR);
	}
}

void CApiEdit::OnSize(HWND hWnd, UINT state, int cx, int cy)
{
	RECT crt;
	int width,height;

	if (state == SIZE_MINIMIZED) {
		return;
	}

	GetClientRect(hWnd,&crt);
	// ���ҵ��� ���� ���
	if (arView[1]==NULL) {
		MoveWindow(arView[0]->hWnd,0,0,crt.right,crt.bottom,TRUE);
	// 2���ҵ� ���
	} else if (arView[2]==NULL) {
		if (VertRatio) {
			height=(crt.bottom-VGAP)*VertRatio/10000;
			MoveWindow(arView[0]->hWnd,0,0,crt.right,height,TRUE);
			MoveWindow(arView[1]->hWnd,0,height+VGAP,crt.right,crt.bottom-height-VGAP,TRUE);
		} else {
			width=(crt.right-VGAP)*HorzRatio/10000;
			MoveWindow(arView[0]->hWnd,0,0,width,crt.bottom,TRUE);
			MoveWindow(arView[1]->hWnd,width+VGAP,0,crt.right-width-VGAP,crt.bottom,TRUE);
		}
	// 4���ҵ� ���
	} else {
		height=(crt.bottom-VGAP)*VertRatio/10000;
		width=(crt.right-VGAP)*HorzRatio/10000;
		MoveWindow(arView[0]->hWnd,0,0,width,height,TRUE);
		MoveWindow(arView[1]->hWnd,width+VGAP,0,crt.right-width-VGAP,height,TRUE);
		MoveWindow(arView[2]->hWnd,0,height+VGAP,width,crt.bottom-height-VGAP,TRUE);
		MoveWindow(arView[3]->hWnd,width+VGAP,height+VGAP,crt.right-width-VGAP,
			crt.bottom-height-VGAP,TRUE);
	}
}

void CApiEdit::OnPaint(HWND hWnd)
{
	HDC hdc;
	PAINTSTRUCT ps;
	HPEN hHilight,hShadow,OldPen;
	HBRUSH hFace;
	RECT crt,rt;
	int width,height;

	hdc=BeginPaint(hWnd,&ps);
	if (VertRatio || HorzRatio) {
		hHilight=CreatePen(PS_SOLID,1,GetSysColor(COLOR_3DHILIGHT));
		hShadow=CreatePen(PS_SOLID,1,GetSysColor(COLOR_3DSHADOW));
		hFace=CreateSolidBrush(GetSysColor(COLOR_3DFACE));
		GetClientRect(hWnd,&crt);

		// ���� ���ø� �ٸ� �׸���.
		if (VertRatio) {
			height=(crt.bottom-VGAP)*VertRatio/10000;
			SetRect(&rt,crt.left,height,crt.right,height+VGAP);
			FillRect(hdc,&rt,hFace);
			OldPen=(HPEN)SelectObject(hdc,hHilight);
			MoveToEx(hdc,rt.left,rt.top+1,NULL);
			LineTo(hdc,rt.right,rt.top+1);
			SelectObject(hdc,hShadow);
			MoveToEx(hdc,rt.left,rt.bottom-1,NULL);
			LineTo(hdc,rt.right,rt.bottom-1);
		}

		// ���� ���ø� �ٸ� �׸���.
		if (HorzRatio) {
			width=(crt.right-VGAP)*HorzRatio/10000;
			SetRect(&rt,width,crt.top,width+VGAP,crt.bottom);
			FillRect(hdc,&rt,hFace);
			OldPen=(HPEN)SelectObject(hdc,hHilight);
			MoveToEx(hdc,rt.left+1,rt.top,NULL);
			LineTo(hdc,rt.left+1,rt.bottom);
			SelectObject(hdc,hShadow);
			MoveToEx(hdc,rt.right-1,rt.top,NULL);
			LineTo(hdc,rt.right-1,rt.bottom);
		}

		if (VertRatio && HorzRatio) {
			SetRect(&rt,crt.left,height+2,crt.right,height+VGAP-1);
			FillRect(hdc,&rt,hFace);
		}

		SelectObject(hdc,OldPen);
		DeleteObject(hHilight);
		DeleteObject(hShadow);
		DeleteObject(hFace);
	}
	EndPaint(hWnd,&ps);
}

void CApiEdit::OnSetFocus(HWND hWnd, HWND hwndOldFocus)
{
	if (ActiveView != -1)
		SetFocus(GetActiveView()->hWnd);
}

void CApiEdit::OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	RECT crt,vrt,hrt;
	POINT pt;
	int width,height;

	GetClientRect(hWnd,&crt);
	pt.x=x;
	pt.y=y;
	SplitPt=pt;

	height=(crt.bottom-VGAP)*VertRatio/10000;
	SetRect(&vrt,crt.left,height,crt.right,height+VGAP);
	width=(crt.right-VGAP)*HorzRatio/10000;
	SetRect(&hrt,width,crt.top,width+VGAP,crt.bottom);

	// ���� Ŭ���ϸ� ������ ����Ѵ�.
	if (fDoubleClick) {
		if (VertRatio && PtInRect(&vrt,pt)) {
			if (ActiveView==0 || (arView[2] && ActiveView==1)) {
				VertRatio=9500;
			} else {
				VertRatio=500;
			}
			UnSplitView(AESPLIT_VERT);
		}
		if (HorzRatio && PtInRect(&hrt,pt)) {
			if (ActiveView==0 || (arView[2] && ActiveView==2)) {
				HorzRatio=9500;
			} else {
				HorzRatio=500;
			}
			UnSplitView(AESPLIT_HORZ);
		}
		SendMessage(hWnd,WM_SIZE,0,0);
		InvalidateRect(hWnd,NULL,TRUE);
		return;
	}

	// ���� ���ø� �� �巡�� ����
	if (VertRatio && PtInRect(&vrt,pt)) {
		iDrag=AESPLIT_VERT;
		OldSplitY=height;
		DrawSplitBar(AESPLIT_VERT,OldSplitY);
	}

	// ���� ���ø� �� �巡�� ����
	if (HorzRatio && PtInRect(&hrt,pt)) {
		iDrag |= AESPLIT_HORZ;
		OldSplitX=width;
		DrawSplitBar(AESPLIT_HORZ,OldSplitX);
	}

	if (iDrag) {
		SetCapture(hWnd);
	}
}

void CApiEdit::OnMouseMove(HWND hWnd, int x, int y, UINT keyFlags)
{
	RECT crt;
	int tx,ty;

	if (iDrag==0) {
		return;
	}

	GetClientRect(hWnd,&crt);
	// ���� ���ø� �� �̵�
	if (iDrag & AESPLIT_VERT) {
		ty=min(max(y,crt.top),crt.bottom-VGAP);
		if (OldSplitY!=ty) {
			DrawSplitBar(AESPLIT_VERT,OldSplitY);
			OldSplitY=ty;
			DrawSplitBar(AESPLIT_VERT,OldSplitY);
		}
	}

	// ���� ���ø� �� �̵�
	if (iDrag & AESPLIT_HORZ) {
		tx=min(max(x,crt.left),crt.right-VGAP);
		if (OldSplitX!=tx) {
			DrawSplitBar(AESPLIT_HORZ,OldSplitX);
			OldSplitX=tx;
			DrawSplitBar(AESPLIT_HORZ,OldSplitX);
		}
	}
}

void CApiEdit::OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags)
{
	RECT crt;

	if (iDrag==0) {
		return;
	}

	GetClientRect(hWnd,&crt);
	// ���� ���ø� ���� ����
	if (iDrag & AESPLIT_VERT) {
		DrawSplitBar(AESPLIT_VERT,OldSplitY);
		if (y != SplitPt.y) {
			VertRatio=OldSplitY*10000/(crt.bottom-VGAP);
			VertRatio=max(min(VertRatio,9500),500);
			UnSplitView(AESPLIT_VERT);
		}
	}

	// ���� ���ø� ���� ����
	if (iDrag & AESPLIT_HORZ) {
		DrawSplitBar(AESPLIT_HORZ,OldSplitX);
		if (x != SplitPt.x) {
			HorzRatio=OldSplitX*10000/(crt.right-VGAP);
			HorzRatio=max(min(HorzRatio,9500),500);
			UnSplitView(AESPLIT_HORZ);
		}
	}

	iDrag=0;
	ReleaseCapture();
	SendMessage(hWnd,WM_SIZE,0,0);
	InvalidateRect(hWnd,NULL,TRUE);
}

void CApiEdit::OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify)
{
	GetActiveView()->OnCommand(hWnd,id,hwndCtl,codeNotify);
}

BOOL CApiEdit::OnSetCursor(HWND hWnd, HWND hwndCursor, UINT codeHitTest, UINT msg)
{
	RECT crt,rt;
	POINT pt;
	int width,height;
	BOOL bVert=FALSE,bHorz=FALSE;

	if ((VertRatio || HorzRatio) && codeHitTest==HTCLIENT) {
		GetClientRect(hWnd,&crt);
		GetCursorPos(&pt);
		ScreenToClient(hWnd,&pt);

		// ���� ���ø� �� ��
		if (VertRatio) {
			height=(crt.bottom-VGAP)*VertRatio/10000;
			SetRect(&rt,crt.left,height,crt.right,height+VGAP);
			if (PtInRect(&rt,pt)) {
				bVert=TRUE;
			}
		}

		// ���� ��Ŭ�� �� ��
		if (HorzRatio) {
			width=(crt.right-VGAP)*HorzRatio/10000;
			SetRect(&rt,width,crt.top,width+VGAP,crt.bottom);
			if (PtInRect(&rt,pt)) {
				bHorz=TRUE;
			}
		}

		if (bHorz && bVert) {
			SetCursor(LoadCursor(NULL,IDC_SIZEALL));
			return TRUE;
		}
		if (bHorz) {
			SetCursor(LoadCursor(NULL,IDC_SIZEWE));
			return TRUE;
		}
		if (bVert) {
			SetCursor(LoadCursor(NULL,IDC_SIZENS));
			return TRUE;
		}
	}

	return FORWARD_WM_SETCURSOR(hWnd,hwndCursor,codeHitTest,msg,DefWindowProc);
}

///////////////////////////////////////////////////////////////////////////////////
// �Ϲ� �Լ���

CApiEditText *CApiEdit::GetActiveView()
{
	return arView[ActiveView];
}

CApiEditText *CApiEdit::CreateView(int Type)
{
	CApiEditText *pView;
	switch (Type) {
	case TEXT_VIEW:
		pView=new CApiEditText;
		break;
	case HEXA_VIEW:
		pView=new CApiEditHexa;
		break;
	}
	pView->pFrame=this;
	pView->Create(0,0,0,0,WS_CHILD | WS_VISIBLE,0,hWnd);
	SendMessage(GetParent(hWnd),UserBase+1,0,(LPARAM)pView);
	return pView;
}

void CApiEdit::DeleteView(int nView)
{
	if (ActiveView==nView) {
		ActiveView=-1;
	}
	DestroyWindow(arView[nView]->hWnd);
	delete arView[nView];
	arView[nView]=NULL;
}

void CApiEdit::Insert(int nPos, TCHAR *str, int len, BOOL bRec/*=TRUE*/)
{
	int movelen;
	int needlen;

	if (bReadOnly)
		return;

	if (bRec) {
		URInsert(nPos,str,len);
	}

	needlen=doclen+len;
	if (needlen > buflen) {
		buflen = needlen+1024;
		buf=(TCHAR *)realloc(buf,buflen);
		if (buf == NULL) {
		}
	}

	if (len==0) return;
	movelen=doclen-nPos;
	memmove(buf+nPos+len,buf+nPos,movelen);
	memcpy(buf+nPos,str,len);
	SetModified(TRUE);
	LastEditOff=nPos+len;

	doclen+=len;
	SendNotify(AEN_CHANGE);
}

void CApiEdit::Delete(int nPos, int nCount, BOOL bRec/*=TRUE*/)
{
	int movelen;
	TCHAR *p=buf+nPos;
	TCHAR *t;

	if (bReadOnly)
		return;

	if (nCount == 0) return;
	if (doclen < nPos+nCount) return;
	
	if (bRec) {
		t=(TCHAR *)malloc(nCount);
		memcpy(t,p,nCount);
		URDelete(nPos,t,nCount);
		free(t);
	}

	movelen=doclen-nPos-nCount;
	memmove(buf+nPos, buf+nPos+nCount, movelen);
	SetModified(TRUE);
	doclen-=nCount;
	LastEditOff=nPos;
	SendNotify(AEN_CHANGE);
}

void CApiEdit::Undo()
{
	int from, to, len, dest;
	int tick;
	CApiEditText *pView;

	pView=GetActiveView();
	if (pView->bComp) {
		return;
	}
	pView->ExtraX=0;
	if (pUR[nowur].action == UR_NONE || pUR[nowur].status==UR_CANCELED) {
		nowur--;
	}

	pView->ClearSelection();
	tick=pUR[nowur].tick;

	for (;;) {
		switch (pUR[nowur].action) {
		case UR_INSERT:
			pView->Delete(pUR[nowur].pos,pUR[nowur].len,FALSE);
			pView->SetOffset(pUR[nowur].pos);
			break;
		case UR_DELETE:
			pView->Insert(pUR[nowur].pos,pUR[nowur].data,pUR[nowur].len,FALSE);
			pView->SetOffset(pUR[nowur].pos+pUR[nowur].len);
			break;
		case UR_MOVE:
			from=pUR[nowur].pos;
			to=pUR[nowur].dest[0];
			len=pUR[nowur].dest[1];
			if (from > to) {
				dest=from+len;
				pView->CopyString(FALSE,to,dest,len,FALSE);
			} else {
				pView->CopyString(FALSE,to-len,from,len,FALSE);
			}
			pView->SetOffset(from+len);
			break;
		}
		pUR[nowur].status=UR_CANCELED;
		if (tick==0 || nowur==0)
			break;
		if (tick!=pUR[nowur-1].tick)
			break;
		nowur--;
	}

	Invalidate(-1);
	ArrangeModified();
}

void CApiEdit::Redo()
{
	int from, to, len;
	int tick;
	CApiEditText *pView;

	pView=GetActiveView();
	pView->ExtraX=0;
	pView->ClearSelection();
	tick=pUR[nowur].tick;

	for (;;) {
		switch (pUR[nowur].action) {
		case UR_INSERT:
			pView->Insert(pUR[nowur].pos,pUR[nowur].data,pUR[nowur].len,FALSE);
			pView->SetOffset(pUR[nowur].pos+pUR[nowur].len);
			break;
		case UR_DELETE:
			pView->Delete(pUR[nowur].pos,pUR[nowur].len,FALSE);
			pView->SetOffset(pUR[nowur].pos);
			break;
		case UR_MOVE:
			from=pUR[nowur].pos;
			to=pUR[nowur].dest[0];
			len=pUR[nowur].dest[1];
			pView->CopyString(FALSE,from,to,len,FALSE);
			pView->SetOffset(to+len);
			break;
		}
		pUR[nowur].status=UR_MAKING;
		NextRecord();
		if (tick==0)
			break;
		if (tick!=pUR[nowur].tick)
			break;
	}

	Invalidate(-1);
	ArrangeModified();
}

void CApiEdit::SetText(TCHAR *TextBuf, int dwSize)
{
	int lenEnc;
	TCHAR *bufEnc;
	int lenEnter;
	TCHAR *bufEnter;
	int lenFix;
	TCHAR *bufFix;

	InitDoc();
	if (TestStyle(AES_AUTOCONVERT)==FALSE) {
		dwFormat=AE_ENC_ANSI;
		GetActiveView()->Insert(0,TextBuf,dwSize,FALSE);
		return;
	}

	dwFormat=AnalyzeFormat(TextBuf, dwSize);

	// ���̳ʸ� �����̸� �ٷ� �����ϰ� ����
	if ((dwFormat & 0xffff) == AE_ENC_BINARY) {
		GetActiveView()->Insert(0,TextBuf,dwSize,FALSE);
		ChangeViewType(0,HEXA_VIEW);
		return;
	}

	// ���ڵ� ��ȯ
	if ((dwFormat & 0xffff) != AE_ENC_ANSI) {
		lenEnc=ConvertEncode(dwFormat & 0xffff,AE_ENC_ANSI,TextBuf,dwSize,bufEnc);
	} else {
		lenEnc=dwSize;
		bufEnc = (TCHAR *)malloc(lenEnc);
		memcpy(bufEnc,TextBuf,lenEnc);
	}

	// ���� ��� ��ȯ
	if ((dwFormat & 0xffff0000) != AE_ENTER_CRLF) {
		lenEnter=ConvertEnter(dwFormat & 0xffff0000,AE_ENTER_CRLF,bufEnc,lenEnc,bufEnter);
	} else {
		lenEnter=lenEnc;
		bufEnter = (TCHAR *)malloc(lenEnter);
		memcpy(bufEnter,bufEnc,lenEnter);
	}
	free(bufEnc);

	// ���� �ڵ� �ϰ��� �˻�
	if (TestEnterCode(bufEnter,lenEnter)==FALSE) {
		lenFix=FixEnterCode(bufEnter,lenEnter,bufFix);
		GetActiveView()->Insert(0,bufFix,lenFix,FALSE);
		free(bufFix);
	} else {
		GetActiveView()->Insert(0,bufEnter,lenEnter,FALSE);
	}
	free(bufEnter);
}

void CApiEdit::InitDoc()
{
	int i;

	if (pUR) {
		for (i=0;;i++) {
			if (pUR[i].action == UR_NONE)
				break;
			if (pUR[i].data)
				free(pUR[i].data);
		}
		free(pUR);
	}
	URSize=100;
	pUR=(UndoRecord *)malloc(sizeof(UndoRecord)*URSize);
	memset(pUR,0,sizeof(UndoRecord)*URSize);
	nowur=0;
	if (buf) {
		free(buf);
	}
	buflen=1024;
	buf=(TCHAR *)malloc(buflen);
	memset(buf,0,buflen);
	doclen=0;
	bReadOnly=FALSE;
	LastEditOff=0;
	bModified=FALSE;
	SaveModified();
	URtick=0;
	URnest=0;
	dwFormat=AE_ENC_ANSI | AE_ENTER_CRLF;
	ForAllView(InitView())
}

BOOL CApiEdit::IsDBCS(int nPos)
{
	return (IsDBCSLeadByte(buf[nPos]) && (nPos+1 < doclen)
		|| IsEnterCode(nPos));
}

int CApiEdit::GetPrevOff(int nPos)
{
	int n, size;

	if (nPos==0) {
		return 0;
	}

	for (n=nPos-1;n!=0;n--) {
		if (IsEnterCode(n)) {
			break;
		}
	}

	for (;;) {
		if (IsDBCS(n)) {
			size=2;
		} else {
			size=1;
		}
		n+=size;
		if (n >= nPos)
			break;
	}
	return n-size;
}

int CApiEdit::GetNextOff(int nPos)
{
	if (IsDBCS(nPos)) {
		return nPos+2;
	} else {
		return nPos+1;
	}
}

int CApiEdit::FindParaStart(int nPos)
{
	int n;

	assert(nPos >= 0 && nPos <= doclen);
	if (nPos==0) {
		return 0;
	}

	for (n=nPos-1;n!=0;n--) {
		if (IsEnterCode(n)) {
			n+=2;
			break;
		}
	}

	return n;
}

int CApiEdit::IncludeEnter(int nPos)
{
	if (IsEnterCode(nPos)) {
		return nPos+2;
	} else {
		return nPos;
	}
}

void CApiEdit::LoadAeResource()
{
	static BYTE CopyAnd[]={
		0XFF,0XFF,0XFF,0XFF,0X9F,0XFF,0XFF,0XFF, 0X8F,0XFF,0XFF,0XFF,0X87,0XFF,0XFF,0XFF,
		0X83,0XFF,0XFF,0XFF,0X81,0XFF,0XFF,0XFF, 0X80,0XFF,0XFF,0XFF,0X80,0X7F,0XFF,0XFF,
		0X80,0X3F,0XFF,0XFF,0X80,0X1F,0XFF,0XFF, 0X80,0X0F,0XFF,0XFF,0X80,0XFF,0XFF,0XFF,
		0X88,0X7F,0XFF,0XFF,0X98,0X7F,0XFF,0XFF, 0XBC,0X3F,0XFF,0XFF,0XFC,0X3F,0XFF,0XFF,
		0XFE,0X1F,0XFF,0XFF,0XF0,0X01,0X00,0X7F, 0XF0,0X01,0X00,0X7F,0XF3,0XD9,0X00,0X7F,
		0XF3,0XF9,0X00,0X7F,0XF3,0XF9,0X00,0X7F, 0XF3,0XF9,0X00,0X7F,0XF0,0X01,0X00,0X7F,
		0XF0,0X01,0X00,0X7F,0XFF,0XFF,0X00,0X7F, 0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
		0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF, 0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	};
	static BYTE CopyXor[]={
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 0X20,0X00,0X00,0X00,0X30,0X00,0X00,0X00,
		0X38,0X00,0X00,0X00,0X3C,0X00,0X00,0X00, 0X3E,0X00,0X00,0X00,0X3F,0X00,0X00,0X00,
		0X3F,0X80,0X00,0X00,0X3F,0XC0,0X00,0X00, 0X3E,0X00,0X00,0X00,0X36,0X00,0X00,0X00,
		0X23,0X00,0X00,0X00,0X03,0X00,0X00,0X00, 0X01,0X80,0X00,0X00,0X01,0X80,0X00,0X00,
		0X00,0XC0,0X00,0X00,0X04,0XD4,0X00,0X00, 0X0A,0X2A,0X7F,0X00,0X04,0X04,0X77,0X00,
		0X08,0X02,0X77,0X00,0X04,0X04,0X41,0X00, 0X08,0X02,0X77,0X00,0X05,0X54,0X77,0X00,
		0X0A,0XAA,0X7F,0X00,0X00,0X00,0X00,0X00, 0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	};
	static BYTE MoveAnd[]={
		0XFF,0XFF,0XFF,0XFF,0X9F,0XFF,0XFF,0XFF, 0X8F,0XFF,0XFF,0XFF,0X87,0XFF,0XFF,0XFF,
		0X83,0XFF,0XFF,0XFF,0X81,0XFF,0XFF,0XFF, 0X80,0XFF,0XFF,0XFF,0X80,0X7F,0XFF,0XFF,
		0X80,0X3F,0XFF,0XFF,0X80,0X1F,0XFF,0XFF, 0X80,0X0F,0XFF,0XFF,0X80,0XFF,0XFF,0XFF,
		0X88,0X7F,0XFF,0XFF,0X98,0X7F,0XFF,0XFF, 0XBC,0X3F,0XFF,0XFF,0XFC,0X3F,0XFF,0XFF,
		0XFE,0X1F,0XFF,0XFF,0XF0,0X01,0XFF,0XFF, 0XF0,0X01,0XFF,0XFF,0XF3,0XD9,0XFF,0XFF,
		0XF3,0XF9,0XFF,0XFF,0XF3,0XF9,0XFF,0XFF, 0XF3,0XF9,0XFF,0XFF,0XF0,0X01,0XFF,0XFF,
		0XF0,0X01,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF, 0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
		0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF, 0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	};
	static BYTE MoveXor[]={
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 0X20,0X00,0X00,0X00,0X30,0X00,0X00,0X00,
		0X38,0X00,0X00,0X00,0X3C,0X00,0X00,0X00, 0X3E,0X00,0X00,0X00,0X3F,0X00,0X00,0X00,
		0X3F,0X80,0X00,0X00,0X3F,0XC0,0X00,0X00, 0X3E,0X00,0X00,0X00,0X36,0X00,0X00,0X00,
		0X23,0X00,0X00,0X00,0X03,0X00,0X00,0X00, 0X01,0X80,0X00,0X00,0X01,0X80,0X00,0X00,
		0X00,0XC0,0X00,0X00,0X04,0XD4,0X00,0X00, 0X0A,0X2A,0X00,0X00,0X04,0X04,0X00,0X00,
		0X08,0X02,0X00,0X00,0X04,0X04,0X00,0X00, 0X08,0X02,0X00,0X00,0X05,0X54,0X00,0X00,
		0X0A,0XAA,0X00,0X00,0X00,0X00,0X00,0X00, 0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	};
	static BYTE MarginAnd[]={
		0XFF,0XFF,0X7F,0XFF,0XFF,0XFF,0X3F,0XFF, 0XFF,0XFF,0X1F,0XFF,0XFF,0X00,0X0F,0XFF,
		0XFF,0X00,0X07,0XFF,0XFF,0X00,0X03,0XFF, 0XFF,0X00,0X07,0XFF,0XFF,0X00,0X0F,0XFF,
		0XFF,0XFF,0X1F,0XFF,0XFF,0XFF,0X3F,0XFF, 0XFF,0XFF,0X7F,0XFF,0XFF,0XFF,0XFF,0XFF,
		0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF, 0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
		0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF, 0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
		0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF, 0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
		0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF, 0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
		0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF, 0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	};
	static BYTE MarginXor[]={
		0X00,0X00,0X80,0X00,0X00,0X00,0XC0,0X00, 0X00,0X00,0XA0,0X00,0X00,0XFF,0X90,0X00,
		0X00,0X80,0X08,0X00,0X00,0X80,0X04,0X00, 0X00,0X80,0X08,0X00,0X00,0XFF,0X90,0X00,
		0X00,0X00,0XA0,0X00,0X00,0X00,0XC0,0X00, 0X00,0X00,0X80,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	};

	hCSel=LoadCursor(NULL,IDC_ARROW);
	hCNoDrop=LoadCursor(NULL,IDC_NO);
	hCCopy=CreateCursor(g_hInst,1,1,32,32,CopyAnd,CopyXor);
	hCMove=CreateCursor(g_hInst,1,1,32,32,MoveAnd,MoveXor);
	hCMargin=CreateCursor(g_hInst,21,5,32,32,MarginAnd,MarginXor);

	hLineNumFont=CreateFont(12,0,0,0,0,0,0,0,HANGEUL_CHARSET,3,2,1,
		VARIABLE_PITCH | FF_MODERN,"����");
}

void CApiEdit::UnLoadAeResource()
{
	DestroyCursor(hCCopy);
	DestroyCursor(hCMove);
	DestroyCursor(hCMargin);
	DeleteObject(hLineNumFont);
}

void CApiEdit::SetModified(BOOL aModi)
{
	if (bModified != aModi) {
		bModified=aModi;
		SendNotify(AEN_CHGMODI);
	}
	if (aModi==FALSE) {
		SaveModified();
	}
}

// TextBuf�� ���� ���˴�� ��ȯ�ؼ� �ؽ�Ʈ ����. TextBuf�� ����� ���̸� ������ �Ѵ�.
void CApiEdit::GetText(TCHAR *TextBuf, int BufSize , int s/*=-1*/, int e/*=-1*/)
{
	int lenEnter;
	TCHAR *bufEnter;
	int lenEnc;
	TCHAR *bufEnc;

	// �Ϻθ� ��û������ ��û�Ѹ�ŭ�� ����
	if (s != -1) {
		memcpy(TextBuf,buf+s,min(BufSize,e-s));
		return;
	}

	// ���̳ʸ��̰ų� ��ȯ�� ���ʿ��ϸ� ��ü ����
	if ((dwFormat & 0xffff) == AE_ENC_BINARY || dwFormat == (AE_ENC_ANSI | AE_ENTER_CRLF)) {
		memcpy(TextBuf,buf,min(doclen,BufSize));
		return;
	}

	// ���� ��� ������� ��ȯ
	if ((dwFormat & 0xffff0000) != AE_ENTER_CRLF) {
		lenEnter=ConvertEnter(AE_ENTER_CRLF,dwFormat & 0xffff0000,buf,doclen,bufEnter);
	} else {
		lenEnter=doclen;
		bufEnter = (TCHAR *)malloc(lenEnter);
		memcpy(bufEnter,buf,lenEnter);
	}

	// ���ڵ� ������� ��ȯ
	if ((dwFormat & 0xffff) != AE_ENC_ANSI) {
		lenEnc=ConvertEncode(AE_ENC_ANSI,dwFormat & 0xffff,bufEnter,lenEnter,bufEnc);
	} else {
		lenEnc=lenEnter;
		bufEnc = (TCHAR *)malloc(lenEnc);
		memcpy(bufEnc,bufEnter,lenEnc);
	}
	free(bufEnter);

	memcpy(TextBuf,bufEnc,min(lenEnc,BufSize));
	free(bufEnc);
}

// ��ȯ ���� �ؽ�Ʈ ���� ���� ����.
int CApiEdit::GetTextLength()
{
	int lenEnter;
	TCHAR *bufEnter;
	int lenEnc;
	TCHAR *bufEnc;

	// ���̳ʸ��̰ų� ��ȯ�� ���ʿ��ϸ� ���� ���� ���� ����
	if ((dwFormat & 0xffff) == AE_ENC_BINARY || dwFormat == (AE_ENC_ANSI | AE_ENTER_CRLF)) {
		return doclen;
	}

	// ���� ��� ������� ��ȯ�� ���� ���� ���̸� �� �� �ִ�.
	if ((dwFormat & 0xffff0000) != AE_ENTER_CRLF) {
		lenEnter=ConvertEnter(AE_ENTER_CRLF,dwFormat & 0xffff0000,buf,doclen,bufEnter);
	} else {
		lenEnter=doclen;
		bufEnter = (TCHAR *)malloc(lenEnter);
		memcpy(bufEnter,buf,lenEnter);
	}

	// ���ڵ� ������� ��ȯ�ؼ� lenEnc�� ���� ����
	if ((dwFormat & 0xffff) != AE_ENC_ANSI) {
		lenEnc=ConvertEncode(AE_ENC_ANSI,dwFormat & 0xffff,bufEnter,lenEnter,bufEnc);
		free(bufEnc);
	} else {
		lenEnc=lenEnter;
	}
	free(bufEnter);

	// ����, ���ڵ����� �� ��ȯ�� ���� ����
	return lenEnc;
}

void CApiEdit::SetReadOnly(BOOL aReadOnly)
{
	bReadOnly=aReadOnly;
}

BOOL CApiEdit::CanUndo()
{
	if (nowur == 0 && (pUR[0].action==UR_NONE || pUR[0].status==UR_CANCELED))
		return FALSE;

	return TRUE;
} 

BOOL CApiEdit::CanRedo()
{
	if (pUR[nowur].status == UR_CANCELED) {
		return TRUE;
	} else {
		return FALSE;
	}
} 

void CApiEdit::AllocURData(int idx,int need,int extra)
{
	if (pUR[idx].data == NULL) {
		pUR[idx].data=(TCHAR *)malloc(need);
	} else if (need > (int)_msize(pUR[idx].data)) {
		pUR[idx].data=(TCHAR *)realloc(pUR[idx].data,need+extra);
	}
}

void CApiEdit::NextRecord()
{
	int i,j,size, size2,len;

	nowur++;

	if (nowur==URSize-1) {
		URSize+=64;
		pUR=(UndoRecord *)realloc(pUR,sizeof(UndoRecord)*URSize);
		memset(pUR+URSize-64,0,sizeof(UndoRecord)*64);
	}

	for (size=0,i=0;;i++) {
		if (pUR[i].action == UR_NONE)
			break;
		size += sizeof(UndoRecord)+_msize(pUR[i].data);
	}

	if (i > UndoMin && size > UndoSize) {
		for (size2=0,i=0;;i++) {
			size2 += sizeof(UndoRecord)+_msize(pUR[i].data);
			if (size2 > UndoSize/2)
				break;
		}

		for (j=0;j<=i;j++) {
			if (pUR[j].data)
				free(pUR[j].data);
		}

		len=nowur-i;
		memmove(pUR,&pUR[i+1],sizeof(UndoRecord)*len);
		memset(pUR+len,0,sizeof(UndoRecord)*(nowur-len));
		nowur=len-1;
		saveur=saveur-i-1;
	}
}

void CApiEdit::ClearRedo()
{
	int i;

	for (i=nowur;;i++) {
		if (i<saveur) {
			saveur=-1;
		}
		if (pUR[i].action==UR_NONE) {
			break;
		}
		if (pUR[i].data != NULL)
			free(pUR[i].data);
		memset(&pUR[i],0,sizeof(UndoRecord));
	}
}

void CApiEdit::URInsert(int nPos, TCHAR *str, int len)
{
	if (CanRedo()) {
		ClearRedo();
	} else {
		if (pUR[nowur].action==UR_INSERT) {
			// ���� ��ġ�� �����̰� ���̰� 64 �̸��� ���� �����Ѵ�.
			if (pUR[nowur].pos + pUR[nowur].len==nPos && pUR[nowur].len < 64) {
				AllocURData(nowur,pUR[nowur].len+len,5);
				memcpy(pUR[nowur].data+pUR[nowur].len,str,len);
				pUR[nowur].len+=len;

				// �Էµ� ���ڿ��� �����̳� ���� �ڵ��̸� �����Ѵ�.
				if (str[0]==' ' || str[0]=='\r') {
					NextRecord();
				}
				return;
			} else {
				NextRecord();
			}
		} else {
			if (pUR[nowur].action != UR_NONE) {
				NextRecord();
			}
		}
	}

	AllocURData(nowur,len+8,0);
	pUR[nowur].action=UR_INSERT;
	pUR[nowur].pos=nPos;
	pUR[nowur].tick=URtick;
	pUR[nowur].len=len;
	memcpy(pUR[nowur].data,str,len);
}

void CApiEdit::URDelete(int nPos, TCHAR *str, int len)
{
	TCHAR *t;

	if (CanRedo()) {
		ClearRedo();
	} else {
		if (pUR[nowur].action==UR_DELETE) {
			if (str[0]==' ' || str[0]=='\r') {
				NextRecord();
			} else {
				if (pUR[nowur].pos - len==nPos) {
					AllocURData(nowur,pUR[nowur].len+len,5);

					t=(TCHAR *)malloc(_msize(pUR[nowur].data));
					memcpy(t,str,len);
					memcpy(t+len,pUR[nowur].data,pUR[nowur].len);
					pUR[nowur].len+=len;
					memcpy(pUR[nowur].data,t,pUR[nowur].len);
					free(t);

					pUR[nowur].pos = nPos;
					return;
				} else if (pUR[nowur].pos == nPos) {
					AllocURData(nowur,pUR[nowur].len+len,5);
					memcpy(pUR[nowur].data+pUR[nowur].len,str,len);
					pUR[nowur].len+=len;
					return;
				} else {
					NextRecord();
				}
			}
		} else {
			if (pUR[nowur].action != UR_NONE) {
				NextRecord();
			}
		}
	}

	AllocURData(nowur,len+8,0);
	pUR[nowur].action=UR_DELETE;
	pUR[nowur].pos=nPos;
	pUR[nowur].tick=URtick;
	pUR[nowur].len=len;
	memcpy(pUR[nowur].data,str,len);
}

void CApiEdit::URMove(int nPos, int Dest,TCHAR *str, int len)
{
	if (CanRedo()) {
		ClearRedo();
	} else {
		if (pUR[nowur].action != UR_NONE) {
			NextRecord();
		}
	}

	AllocURData(nowur,sizeof(int)*2,0);
	pUR[nowur].action=UR_MOVE;
	pUR[nowur].pos=nPos;
	pUR[nowur].tick=URtick;
	pUR[nowur].dest[0]=Dest;
	pUR[nowur].dest[1]=len;
}

void CApiEdit::SetUndoSize(int aSize)
{
	UndoSize=max(aSize,1024);
}

void CApiEdit::SetUndoMin(int aMin)
{
	UndoMin=max(aMin,10);
}

void CApiEdit::SaveModified()
{
	if (pUR[nowur].status!=UR_CANCELED && pUR[nowur].action != UR_NONE) {
		NextRecord();
	}

	saveur=nowur;
}

void CApiEdit::ArrangeModified()
{
	if (bModified == TRUE) {
		if (nowur==saveur && (pUR[nowur].status==UR_CANCELED || pUR[nowur].action==UR_NONE)) {
			SetModified(FALSE);
		}
	} else {
		SetModified(TRUE);
	}
}

void CApiEdit::StartUndoGroup(int id, TCHAR *desc/*=NULL*/)
{
	// �׻� ���ο� ���ڵ忡�� �����ϸ� �������� �ʴ´�.
	if (CanRedo()) {
		ClearRedo();
	} else {
		if (pUR[nowur].action != UR_NONE) {
			NextRecord();
		}
	}

	// �� �׷��� ���۵Ǹ� UR_GROUP ���ڵ带 �ۼ��Ѵ�. ��ø �׷��� �����Ѵ�.
	if (URnest==0) {
		URtick=((nowur << 16) | (GetTickCount() & 0xffff));

		// ������ desc�� �ۼ��ϰ� �׷� id�� pos�� ����Ѵ�.
		if (desc) {
			AllocURData(nowur,lstrlen(desc)+1,0);
			lstrcpy(pUR[nowur].data,desc);
		} else {
			AllocURData(nowur,sizeof(int)*2,0);
			lstrcpy(pUR[nowur].data,"");
		}
		pUR[nowur].action=UR_GROUP;
		pUR[nowur].pos=id;
		pUR[nowur].tick=URtick;
	}
	URnest++;
}

void CApiEdit::EndUndoGroup()
{
	URnest--;
	if (URnest==0) {
		// �� �׷��� �����Ѵ�.
		if (pUR[nowur].action == UR_GROUP) {
			pUR[nowur].action=UR_NONE;
		}
		URtick=0;
	}
}

void CApiEdit::SendNotify(int code)
{
	SendMessage(GetParent(hWnd),WM_COMMAND,
		MAKEWPARAM(GetWindowLongPtr(hWnd,GWLP_ID),code),(LPARAM)hWnd);
}

void CApiEdit::SetFormat(DWORD aFormat)
{
	DWORD OldEnc;

	OldEnc = dwFormat & 0xffff;
	if (aFormat != OldEnc) {
		SetModified(TRUE);
		dwFormat=(dwFormat & 0xffff0000) | aFormat;
	}
}

DWORD CApiEdit::AnalyzeFormat(TCHAR *Text, int dwSize)
{
	TCHAR *p,*p2;
	DWORD Enc = AE_ENC_ANSI;
	DWORD Enter = AE_ENTER_UNKNOWN;

	// EF, BB, BF�� ���۵Ǹ� UTF-8
	if (dwSize >= 3 && *Text==(TCHAR)0xef && *(Text+1)==(TCHAR)0xbb && 
		*(Text+2)==(TCHAR)0xbf) {
		Enc = AE_ENC_UTF8;
	} else

	// FF, FE�� ���۵Ǹ� �����ڵ�
	if (dwSize >= 2 && *Text==(TCHAR)0xff && *(Text+1)==(TCHAR)0xfe) {
		Enc = AE_ENC_UNI;
	} else

	// FE, FF�� ���۵Ǹ� �����ڵ� �� �����
	if (dwSize >= 2 && *Text==(TCHAR)0xfe && *(Text+1)==(TCHAR)0xff) {
		Enc = AE_ENC_UNIBIG;
	} else

	// BOM���� UTF-8�������� ����. 
	if (AnalyzeFormatUtf8(Text, dwSize)) {
		Enc = AE_ENC_UTF8NOBOM;
	} else

	// todo : AE_ENC_UNINOBOM, AE_ENC_UNIBIGNOBOM�� ���� �Ǻ��� ���� ��ȸ��

	// �߰��� 0�� �ϳ��� ������ ���̳ʸ�. �� ��� ���� ����� �ǹ̰� ����.
	if (dwSize != -1) {
		for (p=Text;p-Text < dwSize;p++) {
			if (*p==0) {
				Enc = AE_ENC_BINARY;
			}
		}
	}

	// ���� ��� ������ ���� ������ ANSI ���ڵ��̴�.

	// CR, LF�� �˻��غ��� ���� ����� �����Ѵ�.
	if (Enc == AE_ENC_ANSI || Enc == AE_ENC_UTF8 || Enc == AE_ENC_UTF8NOBOM) {
		p=strchr(Text,'\r');
		if (p==NULL || p-Text >= dwSize) {
			p2=strchr(Text,'\n');
			if (p2==NULL  || p2-Text >= dwSize) {
				Enter = AE_ENTER_CRLF;
			} else {
				Enter = AE_ENTER_LF;
			}
		} else {
			if (p+1-Text < dwSize && *(p+1)=='\n') {
				Enter = AE_ENTER_CRLF;
			} else {
				Enter = AE_ENTER_CR;
			}
		}
	}

	// �����ڵ�� 16��Ʈ �ڵ�� �˻�
	if (Enc == AE_ENC_UNI || Enc == AE_ENC_UNIBIG) {
		wchar_t *wText = (wchar_t *)Text;
		int wSize = dwSize/sizeof(wchar_t);
		wchar_t CR, LF;
		if (Enc == AE_ENC_UNI) {
			CR = 0x0d;
			LF = 0x0a;
		} else {
			CR = 0x0d00;
			LF = 0x0a00;
		}

		wchar_t *wp, *wp2;

		wp=wcschr(wText,CR);
		if (wp==NULL || wp-wText >= wSize) {
			wp2=wcschr(wText,LF);
			if (wp2==NULL  || wp2-wText >= wSize) {
				Enter = AE_ENTER_CRLF;
			} else {
				Enter = AE_ENTER_LF;
			}
		} else {
			if (wp+1-wText < wSize && *(wp+1)==LF) {
				Enter = AE_ENTER_CRLF;
			} else {
				Enter = AE_ENTER_CR;
			}
		}
	}

	return (Enc | Enter);
}

BOOL CApiEdit::AnalyzeFormatUtf8(TCHAR *Text, int dwSize)
{
	TCHAR *p;
	BOOL bFind;

	bFind=FALSE;
	for (p=Text;p-Text < dwSize;p++) {
		if ((*p & 0x80) == 0x80) {
			bFind=TRUE;

			// ���� ��Ʈ�� 110�̰� ���� ������ ���� ��Ʈ�� 10�̸� UTF8����
			// p�� ���� ���� �Ѱų� �߰��� �ϳ��� ��Ģ�� ���� ������ UTF8�� �ƴ�
			if ((*p & 0xe0) == 0xc0) {
				p++;if (p-Text >= dwSize) return FALSE;
				if ((*p & 0xc0) != 0x80) return FALSE;
				continue;
			}

			// ���� ��Ʈ�� 1110�� ���� ���� �� ������ ���� ��Ʈ�� 10�̾�� �Ѵ�.
			if ((*p & 0xf0) == 0xe0) {
				p++;if (p-Text >= dwSize) return FALSE;
				if ((*p & 0xc0) != 0x80) return FALSE;
				p++;if (p-Text >= dwSize) return FALSE;
				if ((*p & 0xc0) != 0x80) return FALSE;
				continue;
			}

			// ���� ��Ʈ�� 11110�� ���� ���� �� ������ ���� ��Ʈ�� 10�̾�� �Ѵ�.
			if ((*p & 0xf8) == 0xf0) {
				p++;if (p-Text >= dwSize) return FALSE;
				if ((*p & 0xc0) != 0x80) return FALSE;
				p++;if (p-Text >= dwSize) return FALSE;
				if ((*p & 0xc0) != 0x80) return FALSE;
				p++;if (p-Text >= dwSize) return FALSE;
				if ((*p & 0xc0) != 0x80) return FALSE;
				continue;
			}

			// 0x80�� �Ѿ��µ� ���� ��Ʈ�� 110, 1110, 11110 �� �ϳ��� �ƴϸ�
			// UTF-8 ������ �ƴϴ�.
			return FALSE;
		}
	}
	
	// 0x80 �Ѵ� ���� �ϳ��� ������ ANSI�� ����Ѵ�.
	if (bFind == FALSE) {
		return FALSE;
	}

	// 0x80�� �Ѵ� ��� ���� UTF-8�� ������ �����ϸ� UTF-8�����̴�.
	return TRUE;
}

int CApiEdit::ConvertEncode(DWORD from, DWORD to, TCHAR *src, int srclen, TCHAR *&dest)
{
	int need,i,num;
	LPWSTR t;
	int bomoff=0;

	switch (from) {
	case AE_ENC_UNI:
	case AE_ENC_UNIBIG:
		// �� ������̸� ����Ʈ ������ ������ �� ��ȯ�Ѵ�.
		if (from == AE_ENC_UNIBIG) {
			for (t=(LPWSTR)src,i=0;i<srclen/2;t++,i++) {
				*t=((*t << 8) | (*t >> 8));
			}
		}
		need=WideCharToMultiByte(CP_ACP,0,(LPCWSTR)src+1,srclen/2-1,NULL,0,0,0);
		dest=(TCHAR *)malloc(need);
		WideCharToMultiByte(CP_ACP,0,(LPCWSTR)src+1,srclen/2-1,dest,need,0,0);
		return need;
	case AE_ENC_UTF8:
		bomoff=3;
	case AE_ENC_UTF8NOBOM:
		// �ӽ� ���ۿ� �����ڵ�� ���� ��ȯ�Ѵ�. 
		need=MultiByteToWideChar(CP_UTF8,0,src+bomoff,srclen-bomoff,NULL,0);
		t=(LPWSTR)malloc(need*sizeof(wchar_t));
		num=MultiByteToWideChar(CP_UTF8,0,src+bomoff,srclen-bomoff,t,need);
		// �����ڵ带 �Ƚ÷� ��ȯ�Ѵ�.
		need=WideCharToMultiByte(CP_ACP,0,(LPCWSTR)t,num,NULL,0,0,0);
		dest=(TCHAR *)malloc(need);
		WideCharToMultiByte(CP_ACP,0,(LPCWSTR)t,num,dest,need,0,0);
		free(t);
		return need;
	}

	switch (to) {
	case AE_ENC_UNI:
	case AE_ENC_UNIBIG:
		need=MultiByteToWideChar(CP_ACP,0,src,srclen,NULL,0);
		need++;								// ���ο� BOM �߰�
		dest=(TCHAR *)malloc(need*sizeof(wchar_t));
		dest[0]=(TCHAR)0xFF;
		dest[1]=(TCHAR)0xFE;
		MultiByteToWideChar(CP_ACP,0,src,srclen,(LPWSTR)dest+1,need);
		if (to == AE_ENC_UNIBIG) {
			for (t=(LPWSTR)dest,i=0;i<need;t++,i++) {
				*t=((*t << 8) | (*t >> 8));
			}
		}
		return need*2;
	case AE_ENC_UTF8:
		bomoff=3;
	case AE_ENC_UTF8NOBOM:
		// �Ƚø� �����ڵ�� ���� ��ȯ�Ѵ�.
		need=MultiByteToWideChar(CP_ACP,0,src,srclen,NULL,0);
		t=(LPWSTR)malloc(need*sizeof(wchar_t));
		num=MultiByteToWideChar(CP_ACP,0,src,srclen,t,need);
		// �����ڵ带 UTF-8�� ��ȯ�Ѵ�.
		need=WideCharToMultiByte(CP_UTF8,0,t,num,NULL,0,0,0);
		if (bomoff != 0) {
			need+=bomoff;
		}
		dest=(TCHAR *)malloc(need);
		if (bomoff != 0) {
			dest[0]=(TCHAR)0xEF;
			dest[1]=(TCHAR)0xBB;
			dest[2]=(TCHAR)0xBF;
		}
		WideCharToMultiByte(CP_UTF8,0,t,num,dest+bomoff,need,0,0);
		free(t);
		return need;
	}
	return 0;
}

int CApiEdit::ConvertEnter(DWORD from, DWORD to, TCHAR *src, int srclen, TCHAR *&dest)
{
	TCHAR *s,*d;
	TCHAR Find, Other;

	dest=(TCHAR *)malloc(lstrlen(src)*2);
	s=src;
	d=dest;

	if (to==AE_ENTER_CRLF) {
		switch (from) {
		case AE_ENTER_LF:
			Find='\n';
			Other='\r';
			break;
		case AE_ENTER_CR:
			Find='\r';
			Other='\n';
			break;
		}
		while (s-src < srclen) {
			if (*s == Other) {
				s++;
				continue;
			}
			if (*s==Find) {
				*d++='\r';
				*d++='\n';
				s++;
			} else {
				*d++=*s++;
			}
		}
		*d=0;
	} else {
		switch (to) {
		case AE_ENTER_LF:
			Find='\n';
			break;
		case AE_ENTER_CR:
			Find='\r';
			break;
		}
		while (s-src < srclen) {
			if (*s=='\r') {
				*d++=Find;
				s++;
				s++;
			} else {
				*d++=*s++;
			}
		}
		*d=0;
	}
	return lstrlen(dest);
}

int CApiEdit::GetWrap()
{
	return GetActiveView()->GetWrap();
}

void CApiEdit::SetWrap(int aWrap)
{
	ForAllView(SetWrap(aWrap))
}

void CApiEdit::Invalidate(int Pos1, int Pos2/*=-1*/)
{
	ForAllView(Invalidate(Pos1,Pos2))
}

void CApiEdit::ClearBookmark()
{
	ForAllView(ClearBookmark())
}

void CApiEdit::ToggleBookmark(int Para, int Mark)
{
	GetActiveView()->ToggleBookmark(Para,Mark);
}

void CApiEdit::GotoBookmark(int Mark)
{
	GetActiveView()->GotoBookmark(Mark);
}

void CApiEdit::GotoLine(int Line, int Col/*=0*/, int Type/*=0*/)
{
	GetActiveView()->GotoLine(Line,Col,Type);
}

void CApiEdit::GetSelect(int &s, int &e, BOOL bRegular/*=TRUE*/)
{
	GetActiveView()->GetSelect(s,e,bRegular);
}

void CApiEdit::SetSelect(int s, int e, BOOL bFind)
{
	GetActiveView()->SetSelect(s,e,bFind);
}

int CApiEdit::GetLineRatio()
{
	return GetActiveView()->GetLineRatio();
}

void CApiEdit::SetLineRatio(int aLineHeight)
{
	ForAllView(SetLineRatio(aLineHeight))
}

void CApiEdit::SetShowCurLine(int aShowCurLine)
{
	nShowCurLine=aShowCurLine;
	ForAllView(Invalidate(-1))
}

int CApiEdit::GetTabWidth()
{
	return GetActiveView()->GetTabWidth();
}

void CApiEdit::SetTabWidth(int aTabWidth)
{
	ForAllView(SetTabWidth(aTabWidth))
}

void CApiEdit::SetHideSelType(int aHideSelType)
{
	HideSelType=aHideSelType;
	ForAllView(Invalidate(-1))
}

void CApiEdit::GetFont(LOGFONT *alogfont)
{
	GetActiveView()->GetFont(alogfont);
}

void CApiEdit::SetFont(LOGFONT *alogfont)
{
	ForAllView(SetFont(alogfont))
}

void CApiEdit::SetShowTabType(int aShowTabType)
{
	ShowTabType=aShowTabType;
	ForAllView(Invalidate(-1))
}

void CApiEdit::SetShowEnterType(int aShowEnterType)
{
	ShowEnterType=aShowEnterType;
	ForAllView(Invalidate(-1))
}

void CApiEdit::SetShowSpaceType(int aShowSpaceType)
{
	ShowSpaceType=aShowSpaceType;
	ForAllView(Invalidate(-1))
}

void CApiEdit::SetCaretWidth(int aCaretWidth)
{
	CaretWidth=aCaretWidth;
	ForAllView(SetCaret())
}

int CApiEdit::GetRightWrap()
{
	return GetActiveView()->GetRightWrap();
}

void CApiEdit::SetRightWrap(int aRightWrap)
{
	ForAllView(SetRightWrap(aRightWrap))
}

int CApiEdit::GetColMark()
{
	return GetActiveView()->GetColMark();
}

void CApiEdit::SetColMark(int aColMark)
{
	ForAllView(SetColMark(aColMark))
}

CParse* CApiEdit::GetParser()
{
	return GetActiveView()->GetParser();
}

void CApiEdit::SetParser(int ParseType)
{
	ForAllView(SetParser(ParseType))
}

BOOL CApiEdit::FindText(int nPos, TCHAR *what, DWORD dwFlag)
{
	return GetActiveView()->FindText(nPos,what,dwFlag);
}

int CApiEdit::ReplaceText(int nPos, TCHAR *what, DWORD dwFlag, TCHAR *to)
{
	return GetActiveView()->ReplaceText(nPos,what,dwFlag,to);
}

int CApiEdit::GetOffset()
{
	return GetActiveView()->GetOffset();
}

void CApiEdit::GetSelRect(RECT &srt)
{
	GetActiveView()->GetSelRect(srt);
}

int CApiEdit::GetInfoFromOff(int Type, int nPos/*=-1*/)
{
	return GetActiveView()->GetInfoFromOff(Type,nPos);
}

void CApiEdit::GotoLastEditOff()
{
	GetActiveView()->GotoLastEditOff();
}

int CApiEdit::GetDocInfo(int type)
{
	return GetActiveView()->GetDocInfo(type);
}

BOOL CApiEdit::GetInsMode()
{
	return GetActiveView()->GetInsMode();
}

int CApiEdit::GetOffFromPara(int pr, int pc)
{
	return GetActiveView()->GetOffFromPara(pr,pc);
}

void CApiEdit::BeginPrint(Ae_PrintInfo *pi)
{
	GetActiveView()->BeginPrint(pi);
}

BOOL CApiEdit::PrintPage(Ae_PrintInfo *pi,int nPage)
{
	return GetActiveView()->PrintPage(pi,nPage);
}

void CApiEdit::EndPrint(Ae_PrintInfo *pi)
{
	GetActiveView()->EndPrint(pi);
} 

// SplitType ������ pos��ġ�� ���� ���� ���븦 �׸���.
void CApiEdit::DrawSplitBar(int SplitType,int pos)
{
	HDC hdc;
	RECT crt;
	DWORD dwStyle;
	HBRUSH hSplitBrush;
	HBRUSH OldBrush;
	static WORD SplitPattern[]={0x5555,0xaaaa,0x5555,0xaaaa,
		0x5555,0xaaaa,0x5555,0xaaaa};
	HBITMAP hPattern;

	// ���� ���븦 �׸��� ���� ȸ�� ���� �귯��
	hPattern=CreateBitmap(8,8,1,1,SplitPattern);
	hSplitBrush=CreatePatternBrush(hPattern);
	DeleteObject(hPattern);

	// WS_CLIPCHILDREN ��Ÿ���� ��� ���ְ� ���� ���븦 �׸���.
	GetClientRect(hWnd,&crt);
	dwStyle=GetWindowLongPtr(hWnd,GWL_STYLE);
	SetWindowLongPtr(hWnd,GWL_STYLE,dwStyle & ~WS_CLIPCHILDREN);
	hdc=GetDC(hWnd);
	OldBrush=(HBRUSH)SelectObject(hdc,hSplitBrush);
	if (SplitType==AESPLIT_VERT) {
		PatBlt(hdc,crt.left,crt.top+pos,crt.right,4,PATINVERT);
	} else {
		PatBlt(hdc,crt.left+pos,crt.top,4,crt.bottom,PATINVERT);
	}

	SelectObject(hdc,OldBrush);
	DeleteObject(hSplitBrush);
	ReleaseDC(hWnd,hdc);
	SetWindowLongPtr(hWnd,GWL_STYLE,dwStyle);
}

// SplitType �������� Ratio ������ŭ â�� �����Ѵ�.
void CApiEdit::SplitView(int SplitType, int Ratio)
{
	// ���� ����
	if (SplitType == AESPLIT_VERT) {
		// ���ҵ��� �ʾ����� 2�����Ѵ�.
		if (arView[1]==NULL) {
			arView[1]=CreateView(arView[0]->ViewType);
			CloneView(arView[1],arView[0],SplitType);
		// 2�����̸� 4�����Ѵ�.
		} else {
			arView[2]=CreateView(arView[0]->ViewType);
			CloneView(arView[2],arView[0],SplitType);
			arView[3]=CreateView(arView[1]->ViewType);
			CloneView(arView[3],arView[1],SplitType);
		}
		VertRatio=Ratio;
	// ���� ����
	} else {
		if (arView[1]==NULL) {
			arView[1]=CreateView(arView[0]->ViewType);
			CloneView(arView[1],arView[0],SplitType);
		} else {
			arView[2]=arView[1];
			arView[1]=CreateView(arView[0]->ViewType);
			CloneView(arView[1],arView[0],SplitType);
			arView[3]=CreateView(arView[2]->ViewType);
			CloneView(arView[3],arView[2],SplitType);
		}
		HorzRatio=Ratio;
	}
	SendMessage(hWnd,WM_SIZE,0,0);
	InvalidateRect(hWnd,NULL,TRUE);
	AdjustSplitable();
	SendNotify(AEN_CHGVIEW);
}

// ���ҵ� ���� ���ø� �ڽ��� �����Ѵ�.
void CApiEdit::AdjustSplitable()
{
	int i;

	// ���ҵǾ� ���� ������ ����� ���� ����
	if (arView[1]==NULL) {
		arView[0]->SetHorzSplit(TestStyle(AES_HORZSPLITABLE));
		arView[0]->SetVertSplit(TestStyle(AES_VERTSPLITABLE));
	// 2���ҵǾ� ������ ���ҵ��� �����ʸ� ���� ����
	} else if (arView[2]==NULL) {
		if (VertRatio) {
			arView[0]->SetHorzSplit(TestStyle(AES_HORZSPLITABLE));
			arView[0]->SetVertSplit(FALSE);
			arView[1]->SetHorzSplit(TestStyle(AES_HORZSPLITABLE));
			arView[1]->SetVertSplit(FALSE);
		} else {
			arView[0]->SetHorzSplit(FALSE);
			arView[0]->SetVertSplit(TestStyle(AES_VERTSPLITABLE));
			arView[1]->SetHorzSplit(FALSE);
			arView[1]->SetVertSplit(TestStyle(AES_VERTSPLITABLE));
		}
	// 4���ҵǾ� ������ �� �̻� ������ �� ����.
	} else {
		for (i=0;i<4;i++) {
			arView[i]->SetHorzSplit(FALSE);
			arView[i]->SetVertSplit(FALSE);
		}
	}
}

// nPos�� DBCS ������ �޹���Ʈ������ �����Ѵ�.
BOOL CApiEdit::IsInDBCS(int nPos)
{
	int toff;

	toff=max(0,nPos-32);
	for (;toff < nPos;) {
		if (IsDBCS(toff)) {
			toff+=2;
		} else {
			toff++;
		}
	}
	return (toff!=nPos);
}

// �並 �����Ѵ�.
void CApiEdit::CloneView(CApiEditText *pDest, CApiEditText *pSrc,int SplitType)
{
	LOGFONT logfont;
	int ViewFirstLine, ViewFirstOff,pr,pc;

	// �۲� ���� �Ӽ� ����
	pDest->LineRatio=pSrc->LineRatio;
	pDest->SetWrap(pSrc->nWrap);
	pDest->TabWidth=pSrc->TabWidth;
	pDest->RightWrap=pSrc->RightWrap;
	pSrc->GetFont(&logfont);
	pDest->SetFont(&logfont);

	// ���̱� ���� �Ӽ� ����
	pDest->ColMark=pSrc->ColMark;

	// �м��� ���� 
	pDest->SetParser((int)pSrc->GetParser()->GetInfo(0));
	SendMessage(GetParent(hWnd),UserBase+6,0,(LPARAM)pDest->GetParser());

	// ���� ��ġ ����. ��� �信�� �ؽ�Ʈ ��� ������ ���� DBCS ��迡 ��ġ�� �ʵ��� �����Ѵ�.
	if (pSrc->ViewType==HEXA_VIEW && pDest->ViewType==TEXT_VIEW && IsInDBCS(pSrc->off)) {
		pDest->off=GetPrevOff(pSrc->off);
	} else {
		pDest->off=pSrc->off;
	}

	// �ϸ�ũ�� ���� Ÿ���� �䳢���� �����Ѵ�. SplitType�� 3�� ���� ���� Ÿ���� ����� ���̴�.
	if (SplitType != 3) {
		memcpy(pDest->arMark,pSrc->arMark,sizeof(int)*MAXBOOKMARK);
	}

	// ���� ��ũ�� ��ġ ����
	if (SplitType==AESPLIT_VERT) {
		pDest->yPos=pSrc->yPos;
	} else if (SplitType==AESPLIT_HORZ) {
		if (pDest->nWrap==0 || pSrc->ViewType!=TEXT_VIEW) {
			pDest->yPos=pSrc->yPos;
		} else {
			ViewFirstLine=pSrc->yPos/pSrc->LineHeight;
			ViewFirstOff=pSrc->pLine[ViewFirstLine].Start;
			pSrc->GetParaFromOff(ViewFirstOff,pr,pc);
			ViewFirstLine=pDest->GetParaFirstLine(pr);
			pDest->yPos=ViewFirstLine*pDest->LineHeight;
		}
	}
}

// ������ ����Ѵ�.
void CApiEdit::UnSplitView(int SplitType)
{
	CApiEditText *OldActive;
	int i;

	OldActive=arView[ActiveView];

	if (SplitType==AESPLIT_VERT) {
		// ���� ������ 5%�϶� ���� �並 �����Ѵ�.
		if (VertRatio==500) {
			if (arView[2]==NULL) {
				DeleteView(0);
				arView[0]=arView[1];
				arView[1]=NULL;
			} else {
				DeleteView(0);
				arView[0]=arView[2];
				arView[2]=NULL;
				DeleteView(1);
				arView[1]=arView[3];
				arView[3]=NULL;
			}
			VertRatio=0;
		}
		// ���� ������ 95%�� �� �Ʒ��� �並 �����Ѵ�.
		if (VertRatio==9500) {
			if (arView[2]==NULL) {
				DeleteView(1);
			} else {
				DeleteView(2);
				DeleteView(3);
			}
			VertRatio=0;
		}
	} else {
		if (HorzRatio==500) {
			if (arView[2]==NULL) {
				DeleteView(0);
				arView[0]=arView[1];
				arView[1]=NULL;
			} else {
				DeleteView(0);
				arView[0]=arView[1];
				DeleteView(2);
				arView[1]=arView[3];
				arView[3]=NULL;
			}
			HorzRatio=0;
		}
		if (HorzRatio==9500) {
			if (arView[2]==NULL) {
				DeleteView(1);
			} else {
				DeleteView(1);
				arView[1]=arView[2];
				DeleteView(3);
				arView[2]=NULL;
			}
			HorzRatio=0;
		}
	}

	AdjustSplitable();

	// ���� ������� Ȱ�� �並 ã�� �ٽ� Ȱ��ȭ�Ѵ�.
	for (i=0;i<4;i++) {
		if (arView[i] && arView[i]==OldActive) {
			break;
		}
	}

	if (i != 4) {
		ActiveView=i;
	} else {
		ActiveView=0;
		SetFocus(arView[ActiveView]->hWnd);
	}
}

// Ȱ�� �並 ��ȯ�Ѵ�. Who = 0:�ڱ� �ڽ�, 1:���� ��, 2:���� ��
void CApiEdit::ChangeActiveView(CApiEditText *pAe,int Who)
{
	int i,Next;

	// pAe�� Ȱ�� ��� �����.
	for (i=0;i<4;i++) {
		if (arView[i]==pAe) {
			ActiveView=i;
		}
	}

	// ���ҵǾ� ���� �ʰų� �ڽ��� Ȱ��ȭ�� ���� �ƹ� �͵� �� �ʿ䰡 ����.
	if (arView[1]!=NULL && Who!=0) {
		// ��ȿ�� ���� �� �˻�
		if (Who==1) {
			for (Next=ActiveView;;) {
				Next = (Next == 3 ? 0:Next+1);
				if (arView[Next]) {
					ActiveView=Next;
					break;
				}
			}
		// ��ȿ�� ���� �� �˻�
		} else {
			for (Next=ActiveView;;) {
				Next = (Next == 0 ? 3:Next-1);
				if (arView[Next]) {
					ActiveView=Next;
					break;
				}
			}
		}
		// �˻��� �信�� ��Ŀ���� �ѱ��.
		SetFocus(arView[Next]->hWnd);
	}
	SendNotify(AEN_CHGVIEW);
}

// ���� ����� ��� �����Ѵ�. nPos�� nCount�� ����� ��ġ�� ���� �����̴�.
void CApiEdit::UpdateViews(CApiEditText *pView,int nPos,int nCount)
{
	int i;

	// �����ϴ� ��� ��(pView ����)�� �����Ѵ�.
	for (i=0;i<4;i++) {
		if (arView[i] && arView[i]!=pView) {
			if (nCount) {
				if (arView[i]->off >= nPos) {
					arView[i]->off=max(arView[i]->off+nCount,nPos);
				}
				if (arView[i]->SelStart >= nPos) {
					arView[i]->SelStart=max(arView[i]->SelStart+nCount,nPos);
				}
				if (arView[i]->SelEnd >= nPos) {
					arView[i]->SelEnd=max(arView[i]->SelEnd+nCount,nPos);
				}
				if (arView[i]->ViewType != HEXA_VIEW) {
					// CR, LF�� ���� �Էµ� ��� ��ü �����Ѵ�.
					if (buf[nPos]=='\n' && nPos > 0 && buf[nPos-1]=='\r') {
						arView[i]->UpdateLineInfo();
					} else {
						arView[i]->UpdateLineInfo(nPos,nCount);
					}
				}
				arView[i]->UpdateScrollInfo();
			}
			if (arView[i]->ViewType != HEXA_VIEW) {
				arView[i]->Invalidate(FindParaStart(nPos));
			} else {
				arView[i]->Invalidate(nPos);
			}
		}
	}
}

BOOL CApiEdit::IsEnterCode(int nPos)
{
	return IsEnterCode(buf,doclen,buf+nPos);
}

BOOL CApiEdit::IsEnterCode(TCHAR *p)
{
	return IsEnterCode(buf,doclen,p);
}

BOOL CApiEdit::IsEnterCode(TCHAR *tbuf,int len,TCHAR *p)
{
	return (*p=='\r' && p-tbuf+1 < len && p[1]=='\n');
}

// ���� �ڵ��� �ϰ��� �˻�
BOOL CApiEdit::TestEnterCode(TCHAR *TextBuf, int dwSize)
{
	TCHAR *s;

	for (s=TextBuf;s-TextBuf < dwSize;s++) {
		if ((*s == '\r' && (s-TextBuf == dwSize || *(s+1) != '\n')) ||
			(*s == '\n' && (s-TextBuf == 0 || *(s-1) != '\r'))) {
			return FALSE;
		}
	}
	return TRUE;
}

// �����ڵ带 \r\n���� ���� ����
int CApiEdit::FixEnterCode(TCHAR *TextBuf, int dwSize, TCHAR *&dest)
{
	TCHAR *s,*d;

	dest=(TCHAR *)malloc(dwSize*2);
	for (s=TextBuf,d=dest;s-TextBuf < dwSize;s++,d++) {
		if ((*s == '\r' && (s-TextBuf == dwSize || *(s+1) != '\n')) ||
			(*s == '\n' && (s-TextBuf == 0 || *(s-1) != '\r'))) {
			*d++='\r';
			*d='\n';
		} else {
			*d=*s;
		}
	}
	return d-dest;
}

void CApiEdit::ChangeViewType(int nView,int Type)
{
	CApiEditText *pView;
	int OldActive;

	// Ȱ�� �並 ��ȯ�Ѵ�.
	if (nView==-1) {
		nView=ActiveView;
	}

	// ���� Ÿ���̸� ��ȯ�� �ʿ䰡 ����.
	if (arView[nView]->ViewType == Type) {
		return;
	}

	// ���ο� �並 ���� �� �Ӽ��� �����Ѵ�.
	pView=CreateView(Type);
	CloneView(pView,arView[nView],3);
	OldActive=ActiveView;
	DeleteView(nView);

	// ���� ���� ��� ��ü�ϰ� Ȱ��ȭ�Ѵ�.
	arView[nView]=pView;
	ActiveView=OldActive;
	SetFocus(pView->hWnd);
	SendMessage(hWnd,WM_SIZE,0,0);
	AdjustSplitable();
	SendNotify(AEN_CHGVIEW);
}

void CApiEdit::SetBpl(int abpl)
{
	CApiEditHexa *pHexaView;
	for (int aet=0;aet<4 && arView[aet];aet++) {
		if (arView[aet]->ViewType == HEXA_VIEW) {
			pHexaView=(CApiEditHexa *)arView[aet];
			pHexaView->SetBpl(abpl);
		}
	}
}

void CApiEdit::SetCapital(int aCapital)
{
	CApiEditHexa *pHexaView;
	for (int aet=0;aet<4 && arView[aet];aet++) {
		if (arView[aet]->ViewType == HEXA_VIEW) {
			pHexaView=(CApiEditHexa *)arView[aet];
			pHexaView->SetCapital(aCapital);
		}
	}
}

void CApiEdit::SetHexSep(int aHexSep)
{
	CApiEditHexa *pHexaView;
	for (int aet=0;aet<4 && arView[aet];aet++) {
		if (arView[aet]->ViewType == HEXA_VIEW) {
			pHexaView=(CApiEditHexa *)arView[aet];
			pHexaView->SetHexSep(aHexSep);
		}
	}
}

int CApiEdit::FindGroupEnd(int idx)
{
	int i;

	for (i=idx+1;pUR[i].tick == pUR[idx].tick;i++);
	return i-1;
}

void CApiEdit::MultiUndo(int idx)
{
	for (;;) {
		if (CanUndo()) {
			Undo();
		} else {
			break;
		}
		if (nowur <= idx) {
			break;
		}
	}
}

void CApiEdit::MultiRedo(int idx)
{
	for (;;) {
		if (CanRedo()) {
			Redo();
		} else {
			break;
		}
		if (nowur > idx) {
			break;
		}
	}
}

void CApiEdit::GetFixedFont(LOGFONT *alogfont)
{
	*alogfont=FixedFont;
}

void CApiEdit::SetFixedFont(LOGFONT *alogfont)
{
	FixedFont=*alogfont;
}

void CApiEdit::RegisterAeColFormat()
{
	AeColFormat=RegisterClipboardFormat("ApiEdit_Column_Format");
}

// ��� ����. NULL �Ǵ� ������ ���� �������� �ϳ��� ������� ����.
// 0: ���� ����
// -1:���� ����
// -2:�������� �ʴ� ������
// -3:�ݴ� ����ǥ ����
int CApiEdit::DoCommand(const TCHAR *szCom)
{
	int comlen;
	const TCHAR *pc, *ps, *p;
	TCHAR *Com, *pCom;
	int Verb, Object;
	TCHAR *str,*str2;
	BOOL bShift;
	int Result=0;
	int Count, nCount;
	DWORD tFlag;
	CApiEditText *pView;

	// �׻� Ȱ���信 ���� �����ؾ� �Ѵ�.
	pView=GetActiveView();

	// ���� ������ �����Ѵ�.
	ps=szCom;
	SkipSpace(ps);

	// �ּ��̸� ��� ��ü�� �����Ѵ�.
	if (*ps=='/' && *(ps+1)=='/') {
		return 0;
	}

	// ��ɾ��� ���̸� ���Ѵ�. ��, ����, �����ݷ��� ������ ���̴�.
	for (pc=ps;;pc++) {
		if (*pc==NULL || *pc=='\r' || *pc==';') {
			break;
		}
		if (SkipString(pc)==FALSE) {
			return -3;
		}
	}
	comlen=pc-ps;

	// szCom�� ��� ���ڴ� �ҹ��ڷ�, �� ����ǥ ���� ����. �ݴ� ����ǥ�� �ִ����� �̹� üũ����
	Com=(TCHAR *)malloc(comlen+1);
	lstrcpyn(Com,ps,comlen+1);
	for (pCom=Com;*pCom;pCom++) {
		SkipString(const_cast<const TCHAR *&>(pCom));
		if (IsCharUpper(*pCom)) {
			*pCom+=0x20;
		}
	}

	// ���縦 ���� ã�´�.
	p=Com;
	Verb=GetComVerb(p,arComVerb);
	if (Verb==-1) {
		Result=-1;
		goto End;
	}

	switch (Verb) {
	case AEC_MOVE:
	case AEC_MOVESEL:
		Object=GetComObject(Verb,p,arComObject,arObjOfVerb);
		if (Verb==AEC_MOVE) {
			bShift=FALSE;
		} else {
			bShift=TRUE;
		}
		Count=GetComNumber(p,1);

		for (nCount=0;nCount<Count;nCount++) {
			switch (Object) {
			default:
			case -1:
				Result=-2;
				goto End;
			case AECM_LEFT:
				pView->ProcessKey(VK_LEFT,FALSE,bShift);
				break;
			case AECM_RIGHT:
				pView->ProcessKey(VK_RIGHT,FALSE,bShift);
				break;
			case AECM_UP:
				pView->ProcessKey(VK_UP,FALSE,bShift);
				break;
			case AECM_DOWN:
				pView->ProcessKey(VK_DOWN,FALSE,bShift);
				break;
			case AECM_HOME:
				pView->ProcessKey(VK_HOME,FALSE,bShift);
				break;
			case AECM_END:
				pView->ProcessKey(VK_END,FALSE,bShift);
				break;
			case AECM_PGUP:
				pView->ProcessKey(VK_PRIOR,FALSE,bShift);
				break;
			case AECM_PGDN:
				pView->ProcessKey(VK_NEXT,FALSE,bShift);
				break;
			case AECM_WLEFT:
				pView->ProcessKey(VK_LEFT,TRUE,bShift);
				break;
			case AECM_WRIGHT:
				pView->ProcessKey(VK_RIGHT,TRUE,bShift);
				break;
			case AECM_WORDHOME:
			case AECM_WORDEND:
			case AECM_LAST:
			case AECM_NEXTPARA:
			case AECM_PREVPARA:
			case AECM_PARAHOME:
			case AECM_PARAEND:
				pView->Move(Object,bShift);
				break;
				pView->Move(2,bShift);
				break;
			case AECM_DOCHOME:
				pView->ProcessKey(VK_HOME,TRUE,bShift);
				break;
			case AECM_DOCEND:
				pView->ProcessKey(VK_END,TRUE,bShift);
				break;
			case AECM_PAREN:
				pView->FindParen(bShift);
				break;
			}
		}
		break;
	case AEC_INSERT:
		GetComString(p,str);
		if (str) {
			Count=GetComNumber(p,1);
			for (nCount=0;nCount<Count;nCount++) {
				pView->InsertText(str);
			}
			free(str);
		} else {
			Result=-3;
			goto End;
		}
		break;
	case AEC_DELETE:
		Count=GetComNumber(p,1);
		for (nCount=0;nCount<Count;nCount++) {
			pView->ProcessKey(VK_DELETE,FALSE,FALSE);
		}
		break;
	case AEC_BS:
		Object=GetComObject(Verb,p,arComObject,arObjOfVerb);
		Count=GetComNumber(p,1);
		switch (Object) {
		default:
		case -1:
			Result=-2;
			goto End;
		case 0:
			for (nCount=0;nCount<Count;nCount++) {
				pView->ProcessKey(VK_BACK,FALSE,FALSE);
			}
			break;
		case 1:
			for (nCount=0;nCount<Count;nCount++) {
				pView->ProcessKey(VK_BACK,TRUE,FALSE);
			}
			break;
		}
		break;
	case AEC_SELECT:
		Object=GetComObject(Verb,p,arComObject,arObjOfVerb);
		switch (Object) {
		default:
		case -1:
			Result=-2;
			goto End;
		case AECS_WORD:
		case AECS_LINE:
		case AECS_PARA:
		case AECS_DOC:
			pView->Select(Object);
			break;
		}
		break;
	case AEC_COPY:
		SendMessage(pView->hWnd,WM_COMMAND,MAKEWPARAM(IDM_AE_COPY,0),0);
		break;
	case AEC_COPYAPP:
		SendMessage(pView->hWnd,WM_COMMAND,MAKEWPARAM(IDM_AE_COPYAPPEND,0),0);
		break;
	case AEC_CUT:
		SendMessage(pView->hWnd,WM_COMMAND,MAKEWPARAM(IDM_AE_CUT,0),0);
		break;
	case AEC_CUTAPP:
		SendMessage(pView->hWnd,WM_COMMAND,MAKEWPARAM(IDM_AE_CUTAPPEND,0),0);
		break;
	case AEC_PASTE:
		Count=GetComNumber(p,1);
		for (nCount=0;nCount<Count;nCount++) {
			SendMessage(pView->hWnd,WM_COMMAND,MAKEWPARAM(IDM_AE_PASTE,0),0);
		}
		break;
	case AEC_CONVERT:
		Object=GetComObject(Verb,p,arComObject,arObjOfVerb);
		switch (Object) {
		default:
		case -1:
			Result=-2;
			goto End;
		case AECC_UPPER:
			pView->ChangeCase(0);
			break;
		case AECC_LOWER:
			pView->ChangeCase(1);
			break;
		case AECC_REVERSECASE:
			pView->ChangeCase(2);
			break;
		case AECC_UPPERFIRST:
			pView->ChangeCase(3);
			break;
		case AECC_UPPSERSENT:
			pView->ChangeCase(4);
			break;
		case AECC_TOTAB:
			pView->ConvertSpaceTab(FALSE);
			break;
		case AECC_TOSAPCE:
			pView->ConvertSpaceTab(TRUE);
			break;
		case AECC_REMOVESPACE:
			pView->RemoveExtraSpace();
			break;
		case AECC_INDENT:
			pView->BlockIndent(FALSE);
			break;
		case AECC_UNINDENT:
			pView->BlockIndent(TRUE);
			break;
		case AECC_COMMENT:
			pView->InsertComment(FALSE);
			break;
		case AECC_UNCOMMENT:
			pView->InsertComment(TRUE);
			break;
		}
		break;
	case AEC_COPYCHAR:
		Object=GetComObject(Verb,p,arComObject,arObjOfVerb);
		Count=GetComNumber(p,1);
		for (nCount=0;nCount<Count;nCount++) {
			switch (Object) {
			default:
			case -1:
				Result=-2;
				goto End;
			case 0:
				pView->CopyChar(0);
				break;
			case 1:
				pView->CopyChar(1);
				break;
			}
		}
		break;
	case AEC_SCROLL:
		Object=GetComObject(Verb,p,arComObject,arObjOfVerb);
		Count=GetComNumber(p,1);
		for (nCount=0;nCount<Count;nCount++) {
			switch (Object) {
			default:
			case -1:
				Result=-2;
				goto End;
			case AECS_UP:
				pView->Scroll(-1);
				break;
			case AECS_DOWN:
				pView->Scroll(1);
				break;
			case AECS_PGUP:
				pView->Scroll(0,-1);
				break;
			case AECS_PGDN:
				pView->Scroll(0,1);
				break;
			case AECS_HALFUP:
				pView->Scroll(0,-2);
				break;
			case AECS_HALFDOWN:
				pView->Scroll(0,2);
				break;
			case AECS_LEFT:
				if (pView->GetWrap()==0) {
					SendMessage(pView->hWnd,WM_HSCROLL,MAKELONG(SB_LINEUP,0),0);
				}
				break;
			case AECS_RIGHT:
				if (pView->GetWrap()==0) {
					SendMessage(pView->hWnd,WM_HSCROLL,MAKELONG(SB_LINEDOWN,0),0);
				}
				break;
			case AECS_PAGELEFT:
				if (pView->GetWrap()==0) {
					SendMessage(pView->hWnd,WM_HSCROLL,MAKELONG(SB_PAGEUP,0),0);
				}
				break;
			case AECS_PAGERIGHT:
				if (pView->GetWrap()==0) {
					SendMessage(pView->hWnd,WM_HSCROLL,MAKELONG(SB_PAGEDOWN,0),0);
				}
				break;
			}
		}
		break;
	case AEC_SETMARK:
		Object=GetComObject(Verb,p,arComObject,arObjOfVerb);
		if (Object == 0) {
			pView->ToggleBookmark(-1,100);
		} else {
			pView->ToggleBookmark(-1,Object-1);
		}
		break;
	case AEC_GOMARK:
		Object=GetComObject(Verb,p,arComObject,arObjOfVerb);
		switch (Object) {
		case 0:
			pView->GotoBookmark(10);
			break;
		case 1:
			pView->GotoBookmark(11);
			break;
		default:
			pView->GotoBookmark(Object-2);
		}
		break;
	case AEC_FIND:
		GetComString(p,str);
		if (str) {
			tFlag=AE_FIND_WRAP;
			if (strstr(p,"nowrap")) {
				tFlag=tFlag & ~AE_FIND_WRAP;
			}
			if (strstr(p,"up")) {
				tFlag=tFlag | AE_FIND_UP;
			}
			if (strstr(p,"word")) {
				tFlag=tFlag | AE_FIND_WHOLEWORD;
			}
			if (strstr(p,"case")) {
				tFlag=tFlag | AE_FIND_MATCHCASE;
			}
			pView->FindText(-1,str,tFlag);
			free(str);
		} else {
			Result=-3;
			goto End;
		}
		break;
	case AEC_REPLACE:
		GetComString(p,str);
		if (str==NULL) {
			Result=-3;
			goto End;
		}
		GetComString(p,str2);
		if (str2==NULL) {
			free(str);
			Result=-3;
			goto End;
		}
		tFlag=AE_FIND_WRAP;
		if (strstr(p,"nowrap")) {
			tFlag=tFlag & ~AE_FIND_WRAP;
		}
		if (strstr(p,"up")) {
			tFlag=tFlag | AE_FIND_UP;
		}
		if (strstr(p,"word")) {
			tFlag=tFlag | AE_FIND_WHOLEWORD;
		}
		if (strstr(p,"case")) {
			tFlag=tFlag | AE_FIND_MATCHCASE;
		}
		// 0(����), 2(��ü �Ϸ�)�� ���� �ٽ� ȣ���� �ʿ䰡 ����. 1(�˻���)�� ���� �ٽ� �ѹ� �� ȣ���Ѵ�.
		if (pView->ReplaceText(-1,str,tFlag,str2)==1) {
			pView->ReplaceText(-1,str,tFlag,str2);
		}
		free(str);
		free(str2);
		break;
	case AEC_FOCUS:
		Object=GetComObject(Verb,p,arComObject,arObjOfVerb);
		switch (Object) {
		default:
		case 0:
			ChangeActiveView(GetActiveView(),1);
			goto End;
		case 1:
			ChangeActiveView(GetActiveView(),1);
			break;
		}
		break;
	}

End:
	free(Com);
	return Result;
}

void CApiEdit::ExecMacro(const TCHAR *szMacro)
{
	const TCHAR *pc;
	int Result,ParentResult;

	GetActiveView()->AeMc.runlevel++;
	StartUndoGroup(21);
	for (pc=szMacro;;) {
		if (*pc==NULL) {
			break;
		}
		// ��� ����.
		Result=DoCommand(pc);
		// �������� �����̸� ��ũ�� ����
		if (Result < -1) {
			break;
		}
		// ���簡 ������ �ϴ� �θ𿡰� ���� �����.
		if (Result==-1) {
			ParentResult=SendMessage(GetParent(hWnd),UserBase+2,0,(LPARAM)pc);
			// �θ� ó���� ���� ������ ��ũ�� ����.
			// 0�� ��� : ���� ����, �Ǵ� ���õ� ��Ȳ => ��� �����Ѵ�.
			if (ParentResult < 0) {
				break;
			}
		}

		// ���� ��� ã��
		for (;;pc++) {
			// ���� �ڵ带 ã������ ���� �ٷ�
			if (*pc=='\r') {
				pc+=2;
				break;
			}
			if (*pc==';') {
				pc++;
				break;
			}
			// NULL�� ã������ �׳� ����. �ٱ� ���� ���ο��� Ż����
			if (*pc==NULL) {
				break;
			}
			// ���ڿ� �ǳ� ��. �� ���������� �̹� ����� ����� �ǳʶٴ� ���̹Ƿ� �ݴ� ����ǥ�� �ݵ�� ����
			// ���� �����ٸ� ���� DoCommand���� �̹� ������ �����߰� ��ũ�� ����Ǿ���
			SkipString(pc);
		}
	}
	EndUndoGroup();
	GetActiveView()->AeMc.runlevel--;
}

void CApiEdit::SetInnerRect(RECT aRect)
{
	InnerRect=aRect;
	for (int aet=0;aet<4 && arView[aet];aet++) {
		SendMessage(arView[aet]->hWnd,WM_SIZE,0,0);
		arView[aet]->Invalidate(-1);
	}
}

void CApiEdit::SetSelBlockType(int aValue)
{
	SelBlockType=aValue;
	for (int aet=0;aet<4 && arView[aet];aet++) {
		if (arView[aet]->SelStart != arView[aet]->SelEnd) {
			arView[aet]->Invalidate(-1);
		}
	}
}

void CApiEdit::SetVAlign(int aValue)
{
	VAlign=aValue;
	ForAllView(Invalidate(-1))
}

void CApiEdit::SetBackBit(HBITMAP hBit)
{
	hBackBit=hBit;
	ForAllView(Invalidate(-1))
}

void CApiEdit::SetBackBitPos(int aValue)
{
	BackBitPos=aValue;
	ForAllView(Invalidate(-1))
}

void CApiEdit::SetCharExtra(int aValue)
{
	CharExtra=aValue;
	for (int aet=0;aet<4 && arView[aet];aet++) {
		arView[aet]->SetFont(&arView[aet]->logfont);
	}
}

void CApiEdit::SetWordExtra(int aValue)
{
	WordExtra=aValue;
	for (int aet=0;aet<4 && arView[aet];aet++) {
		arView[aet]->SetFont(&arView[aet]->logfont);
	}
}

void CApiEdit::SetCharRatio(int aValue)
{
	CharRatio=aValue;
	for (int aet=0;aet<4 && arView[aet];aet++) {
		arView[aet]->SetFont(&arView[aet]->logfont);
	}
}

BOOL CApiEdit::FindTextSub(int nPos, TCHAR *what, DWORD dwFlag,int &s, int &e)
{
	return GetActiveView()->FindTextSub(nPos,what,dwFlag,s,e);
}

// ����� ��Ÿ�Ͽ� ���� ȿ���� �����Ѵ�.
void CApiEdit::SetStyle(DWORD aStyle)
{
	DWORD Old=Style;

	Style=aStyle;

	if ((Old & AES_HIDECURLINE) != (Style & AES_HIDECURLINE)) {
		ForAllView(Invalidate(-1))
	}
	if ((Old & AES_CALCTABWITHAVG) != (Style & AES_CALCTABWITHAVG)) {
		ForAllView(SetTabWidth(arView[aet]->GetTabWidth()))
	}
	// ���� ���ɼ� ������ ���ҵ� ���¿����� ������ �� ����.
	if (arView[1]==NULL && (Old & AES_HORZSPLITABLE) != (Style & AES_HORZSPLITABLE)) {
		AdjustSplitable();
	}
	if (arView[1]==NULL && (Old & AES_VERTSPLITABLE) != (Style & AES_VERTSPLITABLE)) {
		AdjustSplitable();
	}

	if ((Old & AES_SHOWMARGIN) != (Style & AES_SHOWMARGIN)) {
		for (int aet=0;aet<4 && arView[aet];aet++) {
			arView[aet]->MarginWidth=(Style & AES_SHOWMARGIN) ? 25:0;
			arView[aet]->Invalidate(-1);
			SendMessage(arView[aet]->hWnd,WM_SIZE,SIZE_RESTORED,(LPARAM)0);
		}
	}
	if ((Old & AES_SHOWENTER) != (Style & AES_SHOWENTER)) {
		ForAllView(Invalidate(-1))
	}
	if ((Old & AES_SHOWTAB) != (Style & AES_SHOWTAB)) {
		ForAllView(Invalidate(-1))
	}
	if ((Old & AES_SHOWSPACE) != (Style & AES_SHOWSPACE)) {
		ForAllView(Invalidate(-1))
	}
	if ((Old & AES_SHOWLINENUM) != (Style & AES_SHOWLINENUM)) {
		ForAllView(Invalidate(-1))
	}
	if ((Old & AES_SHOWRULER) != (Style & AES_SHOWRULER)) {
		for (int aet=0;aet<4 && arView[aet];aet++) {
			arView[aet]->Invalidate(-1);
			SendMessage(arView[aet]->hWnd,WM_SIZE,SIZE_RESTORED,(LPARAM)0);
		}
	}
	if ((Old & AES_NOFIRSTSPACE) != (Style & AES_NOFIRSTSPACE)) {
		for (int aet=0;aet<4 && arView[aet];aet++) {
			arView[aet]->UpdateLineInfo();
			arView[aet]->UpdateScrollInfo();
			arView[aet]->SetCaret();
			arView[aet]->Invalidate(-1);
		}
	}
}

COLORREF CApiEdit::GetColor(int index)
{
	return Color[index];
}

void CApiEdit::SetColor(int index, COLORREF aColor)
{
	// �ϴ� �����ϰ� ����Ʈ�� �ִ� ������ ���� �����Ѵ�.
	Color[index]=aColor;
	switch (index) {
	case AECOLOR_FORE:
		if (aColor==-1) Color[AECOLOR_FORE]=GetSysColor(COLOR_WINDOWTEXT);
		break;
	case AECOLOR_BACK:
		if (aColor==-1) Color[AECOLOR_BACK]=GetSysColor(COLOR_WINDOW);
		break;
	case AECOLOR_SELFORE:
		if (aColor==-1) Color[AECOLOR_SELFORE]=GetSysColor(COLOR_HIGHLIGHTTEXT);
		break;
	case AECOLOR_SELBACK:
		if (aColor==-1) Color[AECOLOR_SELBACK]=GetSysColor(COLOR_HIGHLIGHT);
		break;
	case AECOLOR_INNERRECT:
		if (aColor==-1) Color[AECOLOR_INNERRECT]=GetSysColor(COLOR_WINDOW);
		break;
	}
	ForAllView(Invalidate(-1))
}

