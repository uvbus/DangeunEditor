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

CApiEditHexa::CApiEditHexa()
{
	ViewType=HEXA_VIEW; 
}

BOOL CApiEditHexa::Create(int x,int y,int w,int h,DWORD style,UINT id,HWND hParent)
{
	CreateWindow("ApiEditText",NULL, style | WS_CLIPCHILDREN,
		x,y,w,h,hParent,(HMENU)id,GetModuleHandle(NULL),this);
	return TRUE;
}

LRESULT CApiEditHexa::OnMessage(UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage) {
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_CHAR, OnChar);
		HANDLE_MSG(hWnd, WM_KEYDOWN, OnKey);
		HANDLE_MSG(hWnd, WM_KEYUP, OnKey);
		HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
		HANDLE_MSG(hWnd, WM_SETFOCUS, OnSetFocus);
		HANDLE_MSG(hWnd, WM_KILLFOCUS, OnKillFocus);
		HANDLE_MSG(hWnd, WM_LBUTTONDOWN, OnLButtonDown);
		HANDLE_MSG(hWnd, WM_LBUTTONDBLCLK, OnLButtonDown);
		HANDLE_MSG(hWnd, WM_MOUSEMOVE, OnMouseMove);
		HANDLE_MSG(hWnd, WM_LBUTTONUP, OnLButtonUp);
		HANDLE_MSG(hWnd, WM_TIMER, OnTimer);
		HANDLE_MSG(hWnd, WM_SIZE, OnSize);
		HANDLE_MSG(hWnd, WM_HSCROLL, OnHScroll);
		HANDLE_MSG(hWnd, WM_VSCROLL, OnVScroll);
		HANDLE_MSG(hWnd, WM_CONTEXTMENU, OnContextMenu);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hWnd, WM_SETCURSOR, OnSetCursor);
		HANDLE_MSG(hWnd, WM_GETDLGCODE, OnGetDlgCode);
	case WM_MOUSEWHEEL:
		OnMouseWheel(hWnd,wParam,lParam);
		return 0;
	case WM_CANCELMODE:
		bCapture=FALSE;
		break;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}

///////////////////////////////////////////////////////////////////////////////////
// 메시지 핸들러

BOOL CApiEditHexa::OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	bpl=16;
	bCapital=TRUE;
	nHexSep=4;
	xMax=0;
	return CApiEditText::OnCreate(hWnd,lpCreateStruct);
}

void CApiEditHexa::OnDestroy(HWND hWnd)
{
	CApiEditText::OnDestroy(hWnd);
}

void CApiEditHexa::OnSize(HWND hWnd, UINT state, int cx, int cy)
{
	CApiEditText::OnSize(hWnd,state,cx,cy);
}

void CApiEditHexa::OnKey(HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	BOOL bShift, bControl;

	bShift=((GetKeyState(VK_SHIFT) & 0x8000) != 0); 
	bControl=((GetKeyState(VK_CONTROL) & 0x8000) != 0);

	if (fDown==FALSE)
		return;

	// 북마크 단축키 처리
	if (vk>='0' && vk<='9' && bControl) {
		if (bShift) {
			ToggleBookmark(-1,vk-'0');
		} else {
			GotoBookmark(vk-'0');
		}
	}

	// 조립을 끝내는 키 입력
	if (vk==VK_LEFT || vk==VK_RIGHT || vk==VK_UP || vk==VK_DOWN ||
		vk==VK_PRIOR || vk==VK_NEXT || vk==VK_HOME || vk==VK_END ||
		vk==VK_INSERT || vk==VK_DELETE) {
		EndComp();
	}

	bFindSelect=FALSE;

	ProcessKey(vk,bControl,bShift);
}

void CApiEditHexa::OnPaint(HWND hWnd)
{
	CApiEditText::OnPaint(hWnd);
}

void CApiEditHexa::OnSetFocus(HWND hWnd, HWND hwndOldFocus)
{
	CApiEditText::OnSetFocus(hWnd,hwndOldFocus);
}

void CApiEditHexa::OnKillFocus(HWND hWnd, HWND hwndNewFocus)
{
	EndComp();
	CApiEditText::OnKillFocus(hWnd,hwndNewFocus);
}

void CApiEditHexa::OnChar(HWND hWnd, TCHAR ch, int cRepeat)
{
	int Value=-1;
	TCHAR szDigit[1];

	if (ch == 1) {
		SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_AE_SELALL,0),0);
		return;
	}
	if (ch == 3) {
		SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_AE_COPY,0),0);
		return;
	}
	if (ch == 22) {
		SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_AE_PASTE,0),0);
		return;
	}
	if (ch == 24) {
		SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_AE_CUT,0),0);
		return;
	}

	if (ch == 25) {
		if (pFrame->CanRedo())
			pFrame->Redo();
		return;
	}
	if (ch == 26) {
		if (pFrame->CanUndo())
			pFrame->Undo();
		return;
	}

	// 입력된 문자를 숫자로 변환
	if (ch >= '0' && ch <= '9')
		Value=ch-'0';
	if (ch >= 'A' && ch <= 'F')
		Value=ch-'A'+10;
	if (ch >= 'a' && ch <= 'f')
		Value=ch-'a'+10;

	// 16진 숫자가 아니면 입력을 거부한다.
	if (Value==-1) {
		return;
	}
	ProcessCursorInEditing();

	// 조립중이 아니면 입력된 숫자 기록
	if (bComp==FALSE) {
		pFrame->StartUndoGroup(7);
		DeleteSelection();
		szDigit[0]=Value;
		if (bOvr) {
			Delete(off,1);
		}
		Insert(off,szDigit,1);
		bComp=TRUE;
	// 상위 니블이 이미 입력되어 있으면 합쳐서 기록
	} else {
		szDigit[0]=pFrame->buf[off] << 4 | Value;
		Delete(off,1);
		Insert(off,szDigit,1);
		EndComp();
		if (off < pFrame->doclen) {
			off++;
		}
	}

	Invalidate(off-1);
	SetCaret();
}

void CApiEditHexa::OnHScroll(HWND hWnd, HWND hwndCtl, UINT code, int pos)
{
	EndComp();
	CApiEditText::OnHScroll(hWnd,hwndCtl,code,pos);
}

void CApiEditHexa::OnVScroll(HWND hWnd, HWND hwndCtl, UINT code, int pos)
{
	EndComp();
	CApiEditText::OnVScroll(hWnd,hwndCtl,code,pos);
}

void CApiEditHexa::OnContextMenu(HWND hWnd, HWND hwndContext, UINT xPos, UINT yPos)
{
	EndComp();
	CApiEditText::OnContextMenu(hWnd,hwndContext,xPos,yPos);
}

void CApiEditHexa::OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify)
{
	EndComp();
	switch (id) {
	case IDM_AE_COPYDUMP:
		CopyDump();
		return;
	}
	CApiEditText::OnCommand(hWnd,id,hwndCtl,codeNotify);
}

void CApiEditHexa::OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	RECT crt;
	BOOL bShift;
	int toff;

	EndComp();
	ShowCursorEdit(TRUE);
	GetClientRect(hWnd,&crt);
	if (StartSplit(hWnd,fDoubleClick,x,y)) {
		return;
	}
	if (y > crt.bottom-SBW || x > crt.right-SBW) {
		return;
	}

	// 눈금자 위에서의 클릭은 무시한다.
	if (pFrame->TestStyle(AES_SHOWRULER) && y < RULERHEIGHT) {
		return;
	}

	bFindSelect=FALSE;
	SetFocus(hWnd);

	bShift=((GetKeyState(VK_SHIFT) & 0x8000) != 0); 
	if (bShift) {
		toff=GetOffFromXY(x+xPos,y+yPos);
		ExpandSelection(off,toff);
		off=toff;
	} else {
		ClearSelection();
		off=GetOffFromXY(x+xPos,y+yPos);
		SelStart=SelEnd=off;
	}

	SetCapture(hWnd);
	bCapture=TRUE;
	SetCaret();
}

void CApiEditHexa::OnMouseMove(HWND hWnd, int x, int y, UINT keyFlags)
{
	BOOL bInstTimer;
	int OldOff;

	ShowCursorEdit(TRUE);
	if (bCapture == FALSE) {
		return;
	}
	if (Spliting(hWnd,x,y)) {
		return;
	}

	OldOff=off;
	off=SelEnd=GetOffFromXY(x+xPos,y+yPos);
	SetCaret();
	Invalidate(min(OldOff,off),max(OldOff,off));
	if (pFrame->SelBlockType==2) {
		Invalidate(min(SelStart,SelEnd),max(SelStart,SelEnd));
	}

	bInstTimer=FALSE;
	if (y>frt.bottom) {
		SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, 0L);
		bInstTimer=TRUE;
	}
	if (y < frt.top) {
		SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, 0L);
		bInstTimer=TRUE;
	}
	if (bInstTimer==TRUE) {
		SetTimer(hWnd, 1, 100, NULL);
	} else {
		KillTimer(hWnd, 1);
	}
}

void CApiEditHexa::OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags)
{
	bCapture=FALSE;
	ReleaseCapture();
	KillTimer(hWnd,1);

	if (EndSplit(hWnd,x,y)) {
		return;
	}
}

void CApiEditHexa::OnTimer(HWND hWnd, UINT id)
{
	CApiEditText::OnTimer(hWnd,id);
}

BOOL CApiEditHexa::OnSetCursor(HWND hWnd, HWND hwndCursor, UINT codeHitTest, UINT msg)
{
	return CApiEditText::OnSetCursor(hWnd,hwndCursor,codeHitTest,msg);
}

void CApiEditHexa::OnMouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	EndComp();
	CApiEditText::OnMouseWheel(hWnd,wParam,lParam);
}

///////////////////////////////////////////////////////////////////////////////////
// 일반 함수들

void CApiEditHexa::SetCaret(BOOL bUpdatePrevX/*=TRUE*/, BOOL bScrollToCaret/*=TRUE*/)
{
	int x,y;
	int caretwidth;
	RECT crt;
	BOOL bScroll=FALSE;
	int ty;
	POINT OldPos;
	RECT InvRt;

	GetCaretPos(&OldPos);

	// 덮어쓰기 모드일 때는 글자 전체를 감싼다.
	if (bOvr==FALSE || bComp) {
		caretwidth=pFrame->CaretWidth;
	} else {
		caretwidth=GetDigitWidth(off);
	}
	if (GetFocus()==hWnd) {
		CreateCaret(hWnd,NULL,caretwidth,FontHeight);
		ShowCaret(hWnd);
	}
	GetXYFromOff(off,x,y);
	if (bComp) {
		x+=GetDigitWidth(off);
	}
	GetClientRect(hWnd,&crt);

	// 자동 스크롤 처리.
	if (bScrollToCaret && crt.right) {
		pFrame->SendNotify(AEN_MOVE);
		if ((x+caretwidth > xPos+frt.right) || (x < xPos+frt.left)) {
			xPos=max(0,PureX(x)-(frt.right-frt.left)/2);
			bScroll=TRUE;
		}

		if (PureY(y) < yPos) {
			yPos=PureY(y);
			bScroll=TRUE;
		}

		if (y+FontHeight > yPos+frt.bottom) {
			ty=(frt.bottom-frt.top-FontHeight)/LineHeight*LineHeight;
			yPos=PureY(y)-ty;
			bScroll=TRUE;
		}

		if (bScroll == TRUE) {
			if (IsWindowEnabled(hHorzScroll)) {
				SetScrollPos(hHorzScroll, SB_CTL, xPos, TRUE);
			}
			if (IsWindowEnabled(hVertScroll)) {
				SetScrollPos(hVertScroll, SB_CTL, yPos, TRUE);
			}
			Invalidate(-1);
		}
	}

	if (GetFocus()==hWnd) {
		SetCaretPos(x-xPos,y-yPos+GetCharYPos());
	}
	if (pFrame->nShowCurLine != 0 && OldPos.y != y-yPos) {
		InvRt.left=0;
		InvRt.right=frt.right;
		InvRt.top=min(OldPos.y,y-yPos)/LineHeight*LineHeight;
		InvRt.bottom=max(OldPos.y,y-yPos)+LineHeight;
		InvalidateRect(hWnd,&InvRt,FALSE);
	}
}

void CApiEditHexa::Insert(int nPos, TCHAR *str, int len, BOOL bRec/*=TRUE*/)
{
	if (pFrame->bReadOnly)
		return;

	pFrame->Insert(nPos,str,len,bRec);
	UpdateScrollInfo();
	pFrame->UpdateViews(this,nPos,len);
}

void CApiEditHexa::Delete(int nPos, int nCount, BOOL bRec/*=TRUE*/)
{
	if (pFrame->bReadOnly)
		return;

	if (nCount == 0) return;
	if (pFrame->doclen < nPos+nCount) return;

	pFrame->Delete(nPos,nCount,bRec);
	pFrame->UpdateViews(this,nPos,-nCount);
}

int CApiEditHexa::GetRowCount()
{
	return (pFrame->doclen)/bpl+1;
}

void CApiEditHexa::GetRCFromOff(int nPos,int &r,int &c)
{
	assert(nPos >= 0 && nPos <= pFrame->doclen);
	r=nPos / bpl;
	c=nPos % bpl;
}

int CApiEditHexa::GetOffFromRC(int r,int c)
{
	int toff;

	assert(r >= 0 && r <= GetRowCount());
	toff=r*bpl+c;
	toff=min(toff,pFrame->doclen);
	return toff;
}

void CApiEditHexa::GetXYFromOff(int nPos,int &x,int &y)
{
	int r,c;

	assert(nPos >= 0 && nPos <= pFrame->doclen);
	GetRCFromOff(nPos,r,c);
	y=DocY(r*LineHeight);
	x=DocX(c*bGap+bCol);
}

int CApiEditHexa::GetOffFromXY(int x,int y)
{
	int r,c;

	x-=frt.left;
	x=max(x,bCol);
	y=max(y,frt.top);

	r=PureY(y)/LineHeight;
	r=min(r,GetRowCount()-1);
	c=(x-bCol+FontWidth)/bGap;
	c=min(c,bpl-1);

	return GetOffFromRC(r,c);
}

void CApiEditHexa::UpdateScrollInfo()
{
	SCROLLINFO si;

	yMax=(GetRowCount()+frt.bottom/LineHeight/2)*LineHeight;

	si.cbSize=sizeof(SCROLLINFO);
	si.fMask=SIF_ALL | SIF_DISABLENOSCROLL;
	si.nMin=0;
	si.nMax=yMax;
	si.nPage=(frt.bottom/LineHeight)*LineHeight;
	si.nPos=yPos;
	SetScrollInfo(hVertScroll, SB_CTL, &si, TRUE);

	xMax=(bCol+bGap*bpl)*2;
	si.nMax=xMax;
	si.nPage=frt.right;
	si.nPos=xPos;
	SetScrollInfo(hHorzScroll, SB_CTL, &si, TRUE);
}

int CApiEditHexa::DrawLine(HDC hdc, int Line)
{
	int x,y;
	int nPos;
	TCHAR szText[1024];
	int i;
	SIZE sz;
	HPEN hPen,OldPen;
	int SelFirst, SelSecond;
	COLORREF OldFore;
	RECT srt;
	COLORREF fore,back;
	HBRUSH hSelBack;
	int r,c;
	BOOL bCurLine=FALSE;
	RECT lrt;
	HBRUSH hCurLine;

	// 출력 위치 계산. 문서 끝을 초과하면 리턴
	nPos=Line*bpl;
	if (nPos > pFrame->doclen) {
		return 0;
	}

	// 현재줄 표시 옵션이 음영이면 배경에 미리 출력한다.
	GetRCFromOff(off,r,c);
	if (r==Line && (pFrame->TestStyle(AES_HIDECURLINE)==FALSE || GetFocus()==hWnd)) {
		bCurLine=TRUE;
	}

	// 배경 비트맵 출력
	if (pFrame->hBackBit) {
		DrawBackBit(hdc,Line);
	}

	y=GetCharYPos();
	if (bCurLine && (pFrame->nShowCurLine == 3 || pFrame->nShowCurLine == 4)) {
		if (pFrame->SelBlockType==0) {
			SetRect(&lrt,frt.left,y,frt.right,y+FontHeight);
		} else {
			SetRect(&lrt,frt.left,0,frt.right,LineHeight);
		}
		hCurLine=CreateSolidBrush(pFrame->GetColor(AECOLOR_CUR));
		FillRect(hdc,&lrt,hCurLine);
		DeleteObject(hCurLine);
	}

	// 선택 블록 표시에 사용할 브러시 생성
	SelFirst=min(SelStart,SelEnd);
	SelSecond=max(SelStart,SelEnd);
	if (GetFocus()==hWnd || pFrame->HideSelType==2 || bFindSelect) {
		fore=pFrame->GetColor(AECOLOR_SELFORE);
		back=pFrame->GetColor(AECOLOR_SELBACK);
	} else {
		fore=RGB(0,0,0);
		back=RGB(192,192,192);
	}
	hSelBack=CreateSolidBrush(back);

	// 구분선 출력에 사용할 브러시 생성 및 텍스트 색상 지정
	hPen=CreatePen(PS_SOLID,FontWidth/8+1,RGB(128,128,128));
	OldPen=(HPEN)SelectObject(hdc,hPen);
	SetTextColor(hdc,pFrame->GetColor(AECOLOR_FORE));
	SetBkColor(hdc,pFrame->GetColor(AECOLOR_BACK));

	x=frt.left-xPos;

	// 오프셋 출력
	wsprintf(szText,"%.8X:",nPos);
	if (bCapital==FALSE) _strlwr(szText);
	TextOut(hdc,x,y,szText,lstrlen(szText));
	x+=bCol;

	// 바이트 출력. 문서 끝을 만나더라도 x값 갱신을 위해 루프는 끝까지 돌아야 한다.
	for (i=nPos;i<nPos+bpl;i++) {
		if (i < pFrame->doclen) {
			wsprintf(szText,"%.2X",(BYTE)pFrame->buf[i]);
			if (bCapital==FALSE) _strlwr(szText);

			// 바이트 텍스트 출력. 선택 영역이면 블록안에 텍스트 출력
			if (i>=SelFirst && i<SelSecond && 
				(GetFocus()==hWnd || pFrame->HideSelType!=0 || bFindSelect)) {
				OldFore=SetTextColor(hdc,fore);
				if (pFrame->SelBlockType==0) {
					SetRect(&srt,x,y,x+bGap,y+FontHeight);
				} else {
					SetRect(&srt,x,0,x+bGap,LineHeight);
				}
				FillRect(hdc,&srt,hSelBack);
				TextOut(hdc,x,y,szText,lstrlen(szText));
				SetTextColor(hdc,OldFore);
			} else {
				TextOut(hdc,x,y,szText,lstrlen(szText));
			}
			
			// 구분선 출력. 단 첫 바이트는 제외
			if (i != nPos && nHexSep && (i-nPos) % nHexSep == 0) {
				MoveToEx(hdc,x-FontWidth,0,NULL);
				LineTo(hdc,x-FontWidth,LineHeight);
			}
		}
		x+=bGap;
	}

	// 문자 출력
	x+=FontWidth;
	for (i=nPos;i<nPos+bpl;i++) {
		memset(szText,0,3);
		if (i==pFrame->doclen) {
			break;
		}
		// DBCS 뒷 바이트는 .으로 대체
		if (i==nPos && pFrame->IsInDBCS(i)) {
			szText[0]='.';
		} else {
			// 한글 출력
			if (pFrame->IsDBCS(i) && !pFrame->IsEnterCode(i)) {
				szText[0]=pFrame->buf[i++];
				szText[1]=pFrame->buf[i];
			} else {
				// 영문은 인쇄 가능한 문자만 출력
				if (pFrame->buf[i] >= 0x20 && pFrame->buf[i] <= 0x7e) {
					szText[0]=pFrame->buf[i];
				} else {
					szText[0]='.';
				}
			}
		}
		// 문자 출력
		GetTextExtentPoint32(hdc,szText,lstrlen(szText),&sz);
		if (i>=SelFirst && i<SelSecond && 
			(GetFocus()==hWnd || pFrame->HideSelType!=0 || bFindSelect)) {
			OldFore=SetTextColor(hdc,fore);
			if (pFrame->SelBlockType==0) {
				SetRect(&srt,x,y,x+sz.cx,y+FontHeight);
			} else {
				SetRect(&srt,x,0,x+sz.cx,LineHeight);
			}
			FillRect(hdc,&srt,hSelBack);
			TextOut(hdc,x,y,szText,lstrlen(szText));
			SetTextColor(hdc,OldFore);
		} else {
			TextOut(hdc,x,y,szText,lstrlen(szText));
		}
		x+=sz.cx;
	}

	DeleteObject(hSelBack);
	DeleteObject(SelectObject(hdc,OldPen));
	DrawLineSub(hdc,Line,bCurLine);
	return 1;
}

void CApiEditHexa::SetFont(LOGFONT *alogfont)
{
	CApiEditText::SetFont(alogfont);
	bCol=FontWidth*12;
	bGap=FontWidth*4;
	if (xMax!=0) {
		UpdateScrollInfo();
		SetCaret();
		Invalidate(-1);
	}
}

int CApiEditHexa::GetInfoFromOff(int Type, int nPos/*=-1*/)
{
	int r,c;
	int ret;

	if (nPos==-1) {
		nPos=off;
	}

	GetRCFromOff(nPos,r,c);

	switch (Type) {
	case AE_INFO_ROW:
	case AE_INFO_PARA:
		ret=r;
		break;
	case AE_INFO_COL:
	case AE_INFO_PARACOL:
		ret=c;
		break;
	case AE_INFO_LINE:
		ret=0;
		break;
	}

	return ret;
}

int CApiEditHexa::GetDocInfo(int type)
{
	int count=0;
	int han=0;
	TCHAR *p=pFrame->buf;

	switch (type) {
	case AE_DOC_TOTALPARA:
		count=GetRowCount();
		break;
	case AE_DOC_WORD:
	case AE_DOC_EXSPACESIZE:
	case AE_DOC_CHAR:
	case AE_DOC_HAN:
		count=CApiEditText::GetDocInfo(type);
		break;
	case AE_DOC_EMPTYLINE:
		break;
	}
	return count;
}

void CApiEditHexa::BeginPrint(Ae_PrintInfo *pi)
{
	TEXTMETRIC tm;
	SIZE sz;

	pi->Origin=pi->prt.left;
	if (pi->bLineNum) {
		GetTextExtentPoint32(pi->pdc,"999999",6,&sz);
		pi->Origin+=sz.cx;
	}

	GetTextMetrics(pi->pdc,&tm);
	pi->LineHeight=int(tm.tmHeight*LineRatio/100);
	pi->LinePerPage=(pi->prt.bottom-pi->prt.top)/pi->LineHeight;
	pi->TotalPage=(GetRowCount()-1)/pi->LinePerPage+1;
	pi->TabSize=tm.tmAveCharWidth;
}

BOOL CApiEditHexa::PrintPage(Ae_PrintInfo *pi,int nPage)
{
	int l,Line;
	int nowoff;
	int i,x,y;
	TCHAR szLine[32];
	BOOL bEnd=FALSE;
	TCHAR szText[1024];

	for (l=pi->LinePerPage*(nPage-1),Line=0;l<pi->LinePerPage*nPage;l++,Line++) {
		if (l==GetRowCount()) {
			bEnd=TRUE;
			break;
		}

		x=pi->Origin;
		y=pi->prt.top+Line*pi->LineHeight;
		nowoff=l*bpl;
	
		// 오프셋 출력
		wsprintf(szText,"%.8X:",nowoff);
		if (bCapital == FALSE) _strlwr(szText);
		TextOut(pi->pdc,x,y,szText,lstrlen(szText));
		x+=pi->TabSize*12;

		// 바이트 출력
		for (i=nowoff;i<nowoff+bpl;i++) {
			if (i < pFrame->doclen) {
				wsprintf(szText,"%.2X",(BYTE)pFrame->buf[i]);
				if (bCapital == FALSE) _strlwr(szText);
				TextOut(pi->pdc,x,y,szText,lstrlen(szText));
			}

			// 구분선 출력. 단 첫 바이트는 제외
			if (i != nowoff && nHexSep && (i-nowoff) % nHexSep == 0) {
				MoveToEx(pi->pdc,x-TabSize/2,y,NULL);
				LineTo(pi->pdc,x-TabSize/2,y+pi->LineHeight);
			}
			x+=pi->TabSize*3;
		}

		// 문자 출력
		x+=pi->TabSize;
		memset(szText,0,bpl+1);
		for (i=nowoff;i<nowoff+bpl;i++) {
			if (i==pFrame->doclen) {
				break;
			}
			if ((pFrame->buf[i] & 0x80) !=0 && i+1<nowoff+bpl) {
				szText[lstrlen(szText)]=pFrame->buf[i++];
				szText[lstrlen(szText)]=pFrame->buf[i];
			} else {
				if (pFrame->buf[i] >= 0x20 && pFrame->buf[i] <= 0x7e) {
					szText[lstrlen(szText)]=pFrame->buf[i];
				} else {
					szText[lstrlen(szText)]='.';
				}
			}
		}
		TextOut(pi->pdc,x,y,szText,lstrlen(szText));

		// 줄번호 출력
		if (pi->bLineNum) {
			wsprintf(szLine,"%d",l);
			SetTextColor(pi->pdc,RGB(128,128,128));
			TextOut(pi->pdc,pi->prt.left,pi->prt.top+l*pi->LineHeight,
				szLine,lstrlen(szLine));
		}
	}

	return bEnd;
}

void CApiEditHexa::EndPrint(Ae_PrintInfo *pi)
{
}

void CApiEditHexa::InitView()
{
	bOvr=TRUE;
	off=0;
	bComp=FALSE;
	xMax=2048;
	xPos=0;
	yPos=0;
	SelStart=SelEnd=0;
	ClearBookmark();
	UpdateScrollInfo();
	if (GetFocus()==hWnd) {
		SetCaret();
	}
	bVertSplit=TRUE;
	bHorzSplit=TRUE;
	iDrag=0;
	OldSplit=0;
	Invalidate(-1);
}

int CApiEditHexa::GetDigitWidth(int nPos)
{
	TCHAR szDigit[3];

	wsprintf(szDigit,"%.2X",(BYTE)pFrame->buf[nPos]);
	if (bCapital == FALSE) _strlwr(szDigit);
	return GetCharWidth(szDigit,1)+GetCharWidth(szDigit+1,1);
}

void CApiEditHexa::EndComp()
{
	if (bComp) {
		bComp=FALSE;
		pFrame->EndUndoGroup();
	}
}

void CApiEditHexa::SetBpl(int abpl)
{
	if (bpl != abpl) {
		bpl=abpl;
		UpdateScrollInfo();
		SetCaret();
		Invalidate(-1);
	}
}

void CApiEditHexa::SetCapital(int aCapital)
{
	if (bCapital != aCapital) {
		bCapital=aCapital;
		Invalidate(-1);
	}
}

void CApiEditHexa::SetHexSep(int aHexSep)
{
	if (nHexSep != aHexSep) {
		nHexSep=aHexSep;
		Invalidate(-1);
	}
}

void CApiEditHexa::CopyDump()
{
	HGLOBAL hmem;
	TCHAR *ptr;
	int SelFirst, SelSecond;
	int st,ed;
	TCHAR szText[1024];
	int i,Line,nPos;

	// 복사 대상은 항상 줄 단위로 선정한다.
	if (SelStart == SelEnd) {
		st=ed=off/bpl;
	} else {
		SelFirst=min(SelStart,SelEnd);
		SelSecond=max(SelStart,SelEnd);
		st=SelFirst/bpl;
		ed=SelSecond/bpl;
	}

	hmem=GlobalAlloc(GHND, (ed-st+1)*bpl*10+1024);
	ptr=(TCHAR *)GlobalLock(hmem);

	// 범위내의 줄 출력
	for (Line=st;Line<=ed;Line++) {
		nPos=Line*bpl;
		wsprintf(szText,"%.8X    ",nPos);
		if (bCapital) _strlwr(szText);
		lstrcat(ptr,szText);

		// 바이트 출력
		for (i=nPos;i<nPos+bpl;i++) {
			if (i < pFrame->doclen) {
				wsprintf(szText,"%.2X  ",(BYTE)pFrame->buf[i]);
				if (bCapital==FALSE) _strlwr(szText);
				lstrcat(ptr,szText);
			}
		}

		// 문자 출력
		lstrcat(ptr,"  ");
		for (i=nPos;i<nPos+bpl;i++) {
			if (i < pFrame->doclen) {
				memset(szText,0,3);
				if (i==nPos && pFrame->IsInDBCS(i)) {
					szText[0]='.';
				} else {
					if (pFrame->IsDBCS(i) && !pFrame->IsEnterCode(i)) {
						// 중간에 0이 나와서는 안된다.
						if (pFrame->buf[i+1] == 0) {
							lstrcpy(szText,"..");
						} else {
							szText[0]=pFrame->buf[i++];
							szText[1]=pFrame->buf[i];
						}
					} else {
						if (pFrame->buf[i] >= 0x20 && pFrame->buf[i] <= 0x7e) {
							szText[0]=pFrame->buf[i];
						} else {
							szText[0]='.';
						}
					}
				}
				lstrcat(ptr,szText);
			}
		}
		lstrcat(ptr,"\r\n");
	}
	
	GlobalUnlock(hmem);
	if (OpenClipboard(hWnd)) {
		EmptyClipboard();
		SetClipboardData(CF_TEXT,hmem);
		CloseClipboard();
	}
}

void CApiEditHexa::ProcessKey(UINT vk,BOOL bControl,BOOL bShift)
{
	int r,c;
	int oldr;
	int OldOff;

	ProcessKeyRecord(vk,bControl,bShift);

	switch (vk) {
	case VK_F2:
		if (bControl) {
			ToggleBookmark(-1,100);
		} else {
			if (bShift) {
				GotoBookmark(11);
			} else {
				GotoBookmark(10);
			}
		}
		return;
	case VK_F6:
		pFrame->ChangeActiveView(this,(bShift ? 2:1));
		return;
	case VK_LEFT:
		if (off > 0) {
			OldOff=off;
			off--;
			if (bShift) {
				ExpandSelection(OldOff,off);
			} else {
				if (SelStart != SelEnd) {
					off=min(SelStart, SelEnd);
				}
			}
			SetCaret();
		}
		if (!bShift) {
			ClearSelection();
		}
		return;
	case VK_RIGHT:
		if (off < pFrame->doclen) {
			OldOff=off;
			off++;
			if (bShift) {
				ExpandSelection(OldOff,off);
			} else {
				if (SelStart != SelEnd) {
					off=max(SelStart, SelEnd);
				}
			}
			SetCaret();
		}
		if (!bShift) {
			ClearSelection();
		}
		return;
	case VK_UP:
		if (bControl && bShift)
			return;
		GetRCFromOff(off,r,c);
		if (r > 0) {
			if (bControl) {
				SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, 0L);
				if (r != (Fheight+yPos)/LineHeight)
					return;
			}
			OldOff=off;
			r--;
			off=GetOffFromRC(r,c);
			if (bShift) {
				ExpandSelection(OldOff,off);
			} else {
				if (SelStart != SelEnd) {
					off=min(SelStart, SelEnd);
					ClearSelection();
					SetCaret();
					SendMessage(hWnd,WM_KEYDOWN,VK_UP,(LPARAM)0);
				}
			}
			SetCaret();
		}
		if (!bShift) {
			ClearSelection();
		}
		return;
	case VK_DOWN:
		if (bControl && bShift)
			return;
		GetRCFromOff(off,r,c);
		if (bControl) {
			SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, 0L);
			if (r != yPos/LineHeight-1) {
				return;
			}
		}
		if (r < GetRowCount()-1) {
			OldOff=off;
			r++;
			off=GetOffFromRC(r,c);
			if (bShift) {
				ExpandSelection(OldOff,off);
			} else {
				if (SelStart != SelEnd) {
					off=max(SelStart, SelEnd);
					ClearSelection();
					SetCaret();
					SendMessage(hWnd,WM_KEYDOWN,VK_DOWN,(LPARAM)0);
				}
			}
			SetCaret();
		}
		if (!bShift) {
			ClearSelection();
		}
		return;
	case VK_PRIOR:
		GetRCFromOff(off,r,c);
		oldr=r;
		r-=Fheight/LineHeight;
		r=max(r,0);
		yPos=yPos-(oldr-r)*LineHeight;
		yPos=max(yPos,0);
		Invalidate(-1);
		SetScrollPos(hVertScroll, SB_CTL, yPos, TRUE);

		OldOff=off;
		off=GetOffFromRC(r,c);
		if (bShift) {
			ExpandSelection(OldOff,off);
		} else {
			ClearSelection();
		}
		SetCaret(FALSE);
		return;
	case VK_NEXT:
		GetRCFromOff(off,r,c);
		oldr=r;
		r+=Fheight/LineHeight;
		r=min(r,GetRowCount()-1);
		yPos=yPos+(r-oldr)*LineHeight;
		yPos=max(0,min(yPos,yMax-(Fheight/LineHeight)*LineHeight));
		Invalidate(-1);
		SetScrollPos(hVertScroll, SB_CTL, yPos, TRUE);

		OldOff=off;
		off=GetOffFromRC(r,c);
		if (bShift) {
			ExpandSelection(OldOff,off);
		} else {
			ClearSelection();
		}
		SetCaret();
		return;
	case VK_HOME:
		GetRCFromOff(off,r,c);
		OldOff=off;
		if (bControl) {
			off=0;
		} else {
			off=GetOffFromRC(r,0);
		}
		if (bShift) {
			ExpandSelection(OldOff,off);
		} else {
			ClearSelection();
		}
		SetCaret();
		return;
	case VK_END:
		GetRCFromOff(off,r,c);
		OldOff=off;
		if (bControl) {
			off=pFrame->doclen;
		} else {
			off=GetOffFromRC(r,bpl-1);
		}
		if (bShift) {
			ExpandSelection(OldOff,off);
		} else {
			ClearSelection();
		}
		SetCaret();
		return;
	case VK_DELETE:
		if (pFrame->bReadOnly || bCapture)
			return;
		if (bShift) {
			SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_AE_CUT,0),0);
			return;
		}
		if (DeleteSelection()==FALSE) {
			Delete(off, 1);
		}
		Invalidate(off);
		SetCaret();
		return;
	case VK_INSERT:
		if (bShift) {
			SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_AE_PASTE,0),0);
		} else if (bControl) {
			SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_AE_COPY,0),0);
		} else {
			bOvr = !bOvr;
			pFrame->SendNotify(AEN_INSMODE);
			SetCaret();
		}
		return;
	}
}

void CApiEditHexa::Move(int n, BOOL bSelect)
{
	switch (n) {
	case AECM_LAST:
	case AECM_WORDHOME:
	case AECM_WORDEND:
		CApiEditText::Move(n,bSelect);
		break;
	case AECM_NEXTPARA:
	case AECM_PREVPARA:
	case AECM_PARAHOME:
	case AECM_PARAEND:
		break;
	}
}

void CApiEditHexa::Select(int n)
{
	switch (n) {
	case AECS_WORD:
	case AECS_DOC:
		CApiEditText::Select(n);
		break;
	case AECS_LINE:
	case AECS_PARA:
		break;
	}
}

void CApiEditHexa::DrawSelFrame(HDC hdc)
{
	HRGN hRgn;
	int x1,y,x2;
	int SelFirst, SelSecond;
	HBRUSH hBrush;
	int rs,re,i;
	int ss,se;
	RECT rt;
	HRGN rgn;

	SelFirst=min(SelStart,SelEnd);
	SelSecond=max(SelStart,SelEnd);
	GetRCFromOff(SelFirst,rs,i);
	GetRCFromOff(SelSecond,re,i);
	hRgn=CreateRectRgn(0,0,0,0);

	for (i=rs;i<=re;i++) {
		if (i==rs) {
			ss=SelFirst;
		} else {
			ss=i*bpl;
		}
		if (i==re) {
			se=SelSecond;
		} else {
			se=i*bpl+bpl-1;
		}
		GetXYFromOff(ss,x1,y);
		GetXYFromOff(se,x2,y);
		if (se != SelSecond) {
			x2+=bGap;
		}
		x1-=xPos;
		y-=yPos;
		x2-=xPos;
		SetRect(&rt,x1,y,x2,y+LineHeight);
		if (PtInRect(&frt,*(POINT *)&rt) || PtInRect(&frt,*(POINT *)&rt.right)) {
			rgn=CreateRectRgnIndirect(&rt);
			CombineRgn(hRgn,hRgn,rgn,RGN_OR);
			DeleteObject(rgn);
		}
	}

	hBrush=CreateSolidBrush(pFrame->GetColor(AECOLOR_SELFRAME));
	FrameRgn(hdc,hRgn,hBrush,2,2);
	DeleteObject(hBrush);
	DeleteObject(hRgn);
}
