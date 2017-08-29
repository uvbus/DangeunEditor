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
#include "stdafx.h"

CApiEditText::CApiEditText()
{
	arHanWidth=NULL;
	pLine=NULL;
	hBit=NULL;
	Parser=NULL;
	bFindSelect=FALSE;
	ViewType=TEXT_VIEW; 
	Bklogfont.lfHeight=-1;
	bShowCursor=TRUE;
}

BOOL CApiEditText::Create(int x,int y,int w,int h,DWORD style,UINT id,HWND hParent)
{
	CreateWindow("ApiEditText",NULL, style | WS_CLIPCHILDREN,
		x,y,w,h,hParent,(HMENU)id,GetModuleHandle(NULL),this);
	return TRUE;
}

LRESULT CApiEditText::OnMessage(UINT iMessage,WPARAM wParam,LPARAM lParam)
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
	case WM_IME_COMPOSITION:
		return OnImeComposition(hWnd, wParam, lParam);
	case WM_IME_CHAR:
		return OnImeChar(hWnd, wParam, lParam);
	case WM_IME_STARTCOMPOSITION:
		return 0;
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

BOOL CApiEditText::OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	bComp=FALSE;
	bCapture=FALSE;
	bSelLine=FALSE;
	SelStartLine=0;
	bDragSel=FALSE;
	SumDelta=0;
	bHideCaret=FALSE;

	hVertScroll=CreateWindow("scrollbar",NULL,WS_CHILD | WS_VISIBLE | SBS_VERT,
		0,0,0,0,hWnd,(HMENU)AEID_VERTSCROLL,GetModuleHandle(NULL),NULL);
	hHorzScroll=CreateWindow("scrollbar",NULL,WS_CHILD | WS_VISIBLE | SBS_HORZ,
		0,0,0,0,hWnd,(HMENU)AEID_HORZSCROLL,GetModuleHandle(NULL),NULL);
	hBtnSetting=CreateWindow("button","",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		0,0,0,0,hWnd,(HMENU)AEID_BTNSETTING,GetModuleHandle(NULL),NULL);

	SetDefaultSetting();
	bVertSplit=TRUE;
	bHorzSplit=TRUE;
	InitView();

	lpDx=(int *)malloc(1025*sizeof(int));
	return TRUE;
}

void CApiEditText::OnDestroy(HWND hWnd)
{
	if (arHanWidth) {
		free(arHanWidth);
		arHanWidth=NULL;
	}
	if (pLine) {
		free(pLine);
		pLine=NULL;
	}
	if (hBit) {
		DeleteObject(hBit);
		hBit=NULL;
	}
	SetParser(-1);
	if (hMainFont) {
		DeleteObject(hMainFont);
	}
	free(lpDx);
}

void CApiEditText::OnSize(HWND hWnd, UINT state, int cx, int cy)
{
	int nPara;
	int nLine;
	int toff;
	int r,c;
	RECT crt;

	if (state != SIZE_MINIMIZED) {
		GetClientRect(hWnd,&crt);
		frt=crt;
		// 스크롤 바 영역과 안쪽 여백은 편집 영역에서 제외된다.
		frt.right-=(SBW+pFrame->InnerRect.right);
		frt.bottom-=(SBW+pFrame->InnerRect.bottom);
		frt.left+=(MarginWidth+pFrame->InnerRect.left);
		frt.top+=(pFrame->InnerRect.top+(pFrame->TestStyle(AES_SHOWRULER) ? RULERHEIGHT:0));
		// 포맷팅 영역의 폭과 높이를 미리 구해 놓는다.
		Fwidth=frt.right-frt.left;
		Fheight=frt.bottom-frt.top;
		// 분할 가능하면 스크롤 바 위, 왼쪽에 스플릿 박스를 위한 여백을 남긴다.
		if (bVertSplit) {
			MoveWindow(hVertScroll,crt.right-SBW,SPLITWIDTH,SBW,
				crt.bottom-SBW-SPLITWIDTH,TRUE);
		} else {
			MoveWindow(hVertScroll,crt.right-SBW,0,SBW,crt.bottom-SBW,TRUE);
		}
		if (bHorzSplit) {
			MoveWindow(hHorzScroll,SPLITWIDTH,crt.bottom-SBW,
				crt.right-SBW-SPLITWIDTH,SBW,TRUE);
		} else {
			MoveWindow(hHorzScroll,0,crt.bottom-SBW,crt.right-SBW,SBW,TRUE);
		}
		MoveWindow(hBtnSetting,crt.right-SBW,crt.bottom-SBW,SBW,SBW,TRUE);

		if (nWrap && ViewType == TEXT_VIEW) {
			nPara=pLine[yPos/LineHeight].nPara;
			nLine=pLine[yPos/LineHeight].nLine;
			toff=GetOffFromPara(nPara,0);
			UpdateLineInfo();
		}
		UpdateScrollInfo();
		if (nWrap && ViewType == TEXT_VIEW) {
			GetRCFromOff(toff,r,c);
			yPos=(r+nLine)*LineHeight;
			yPos=max(0,min(yPos,yMax-(Fheight/LineHeight)*LineHeight));
		}
		if (GetFocus()==hWnd) {
			SetCaret(TRUE,FALSE);
		}
		if (hBit) {
			DeleteObject(hBit);
			hBit=NULL;
		}
	}
}

void CApiEditText::OnKey(HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	BOOL bShift, bControl;
	BOOL bCopy;

	bShift=((GetKeyState(VK_SHIFT) & 0x8000) != 0); 
	bControl=((GetKeyState(VK_CONTROL) & 0x8000) != 0);

	if (bDragSel) {
		if (vk==VK_CONTROL) {
			bCopy=fDown;
			if (pFrame->TestStyle(AES_SWAPDRAGBUTTON)) {
				bCopy=!bCopy;
			}
			if (bCopy) {
				SetCursor(CApiEdit::hCCopy);
			} else {
				SetCursor(CApiEdit::hCMove);
			}
		}
		return;
	}

	if (fDown==FALSE)
		return;

	if (bColumnMode && SelStart != SelEnd && !bShift) {
		if (vk==VK_LEFT || vk==VK_RIGHT || vk==VK_UP || vk==VK_DOWN ||
			vk==VK_PRIOR || vk==VK_NEXT || vk==VK_HOME || vk==VK_END) {
			ClearSelection();
			ExtraX=0;
			SelExtraX=0;
		}
	}

	if (bShift && !bColumnMode) {
		ignoreVirt++;
	}
	bFindSelect=FALSE;

	if (vk>='0' && vk<='9' && bControl) {
		if (bShift) {
			ToggleBookmark(-1,vk-'0');
		} else {
			GotoBookmark(vk-'0');
		}
	}

	ProcessKey(vk,bControl,bShift);

	if (bShift && !bColumnMode) {
		ignoreVirt--;
	}
	if (bShift && bColumnMode && SelStart != SelEnd) {
		SelExtraX=ExtraX;
	}
}

void CApiEditText::OnPaint(HWND hWnd)
{
	HDC hdc, hMemDC;
	PAINTSTRUCT ps;
	int l,l2;
	int s,e,t,e2;
	HBRUSH hBrush;
	HBITMAP OldBitmap;
	RECT lrt;
	HBRUSH hMarBrush;
	RECT mrt;
	HPEN hMarPen,OldPen;
	HFONT hOldFont;
	RECT crt,srt;
	HBRUSH hInnerBrush;
	int RulerHeight;

	hdc=BeginPaint(hWnd,&ps);
	SetBkMode(hdc,TRANSPARENT);
	GetClientRect(hWnd,&crt);

	if (pFrame->TestStyle(AES_SHOWRULER)) {
		DrawRuler(hWnd,hdc);
		RulerHeight=RULERHEIGHT;
	} else {
		RulerHeight=0;
	}

	// 안쪽 브러시 생성
	hInnerBrush=CreateSolidBrush(pFrame->GetColor(AECOLOR_INNERRECT));
	//위, 오른쪽 여백 그림
	if (pFrame->InnerRect.top) {
		SetRect(&mrt,MarginWidth,RulerHeight,crt.right-SBW,frt.top);
		FillRect(hdc,&mrt,hInnerBrush);
	}
	if (pFrame->InnerRect.right) {
		SetRect(&mrt,frt.right,RulerHeight,crt.right-SBW,crt.bottom-SBW);
		FillRect(hdc,&mrt,hInnerBrush);
	}

	t=yPos/LineHeight;
	s=max((yPos+PureY(ps.rcPaint.top))/LineHeight,t);
	e=max((yPos+PureY(ps.rcPaint.bottom)-1)/LineHeight,t);
	e2=e=max((yPos+PureY(ps.rcPaint.bottom)-1)/LineHeight,t);
	if (ViewType == TEXT_VIEW) {
		e=min(e,TotalLine-1);
		Parser->ParseLines(*this,e);
	} else {
		e=min(e,GetRowCount()-1);
	}
	
	hMemDC=CreateCompatibleDC(hdc);
	SetBkMode(hMemDC,TRANSPARENT);
	if (hBit == NULL) {
		hBit=CreateCompatibleBitmap(hdc,frt.right,LineHeight);
	}
	OldBitmap=(HBITMAP)SelectObject(hMemDC,hBit);

	if (hMainFont) {
		hOldFont=(HFONT)SelectObject(hMemDC,hMainFont);
	}

	hBrush=CreateSolidBrush(pFrame->GetColor(AECOLOR_BACK));
	SetRect(&lrt,MarginWidth,0,frt.right,LineHeight);

	for (l=s;l<=e;l++) {
		FillRect(hMemDC,&lrt,hBrush);
		DrawLine(hMemDC,l);
		BitBlt(hdc,0,DocY((l-t)*LineHeight),frt.right,LineHeight,
			hMemDC,0,0,SRCCOPY);
	}

	if (pFrame->TestStyle(AES_SHOWMARGIN)) {
		hMarBrush=CreateSolidBrush(pFrame->GetColor(AECOLOR_MAR1));
		// 마진 여백 1픽셀 감소
		SetRect(&mrt,0,(l-t)*LineHeight+frt.top,MarginWidth,crt.bottom-SBW);
		FillRect(hdc,&mrt,hMarBrush);
		// 위쪽 마진도 그린다.
		SetRect(&mrt,0,0,MarginWidth,frt.top);
		FillRect(hdc,&mrt,hMarBrush);

		hMarPen=CreatePen(PS_SOLID,1, pFrame->GetColor(AECOLOR_MAR2));
		OldPen=(HPEN)SelectObject(hdc,hMarPen);
		// 마진 구분선은 처음부터 끝까지 그음
		MoveToEx(hdc,MarginWidth-1,0,NULL);
		LineTo(hdc,MarginWidth-1,crt.bottom-SBW);

		SelectObject(hdc,OldPen);
		DeleteObject(hMarPen);
		DeleteObject(hMarBrush);
	}
	SetRect(&lrt,frt.left,DocY((l-t)*LineHeight),frt.right,frt.bottom);	
	FillRect(hdc,&lrt,hBrush);

	
	// 문서 뒷부분의 배경 비트맵을 그린다.
	if (pFrame->hBackBit) {
		SetRect(&lrt,MarginWidth,0,frt.right,LineHeight);
		for (l2=l;l2<=e2;l2++) {
			FillRect(hMemDC,&lrt,hBrush);
			DrawBackBit(hMemDC,l2);
			BitBlt(hdc,frt.left,(l2-t)*LineHeight+frt.top,frt.right,
				(l2-t)*LineHeight+LineHeight+frt.top,hMemDC,frt.left,0,SRCCOPY);
		}
	} else {
		SetRect(&lrt,frt.left,(l-t)*LineHeight+frt.top,frt.right,frt.bottom);
		FillRect(hdc,&lrt,hBrush);
	}

	// 문서 뒷부분의 왼쪽 여백 그림. 더블 버퍼링되지 않으므로 직접 그려야 한다.
	if (pFrame->InnerRect.left) {
		SetRect(&mrt,MarginWidth,DocY((l-t)*LineHeight),frt.left,frt.bottom);
		FillRect(hdc,&mrt,hInnerBrush);
	}
	// 아래 여백 그림
	if (pFrame->InnerRect.bottom) {
		SetRect(&mrt,MarginWidth,frt.bottom,frt.right,crt.bottom-SBW);
		FillRect(hdc,&mrt,hInnerBrush);
	}
	// 안쪽 브러시 파괴
	DeleteObject(hInnerBrush);

	if (hMainFont) {
		SelectObject(hMemDC,hOldFont);
	}
	DeleteObject(hBrush);
	SelectObject(hMemDC,OldBitmap);
	DeleteDC(hMemDC);
	// 스플릿 박스를 그린다.
	if (bVertSplit) {
		SetRect(&srt,crt.right-SBW,crt.top,crt.right,crt.top+SPLITWIDTH);
		DrawFrameControl(hdc,&srt,DFC_BUTTON,DFCS_BUTTONPUSH);
	}
	if (bHorzSplit) {
		SetRect(&srt,crt.left,crt.bottom-SBW,crt.left+SPLITWIDTH,crt.bottom);
		DrawFrameControl(hdc,&srt,DFC_BUTTON,DFCS_BUTTONPUSH);
	}
	if (SelStart!=SelEnd && pFrame->SelBlockType==2) {
		DrawSelFrame(hdc);
	}
	EndPaint(hWnd,&ps);
}

void CApiEditText::OnSetFocus(HWND hWnd, HWND hwndOldFocus)
{
	// 차일드가 포커스를 가진 상태에서 파괴될 때 부모가 정리되는 중에  
	// WM_SEFOCUS가 날라오는데 이때는 무시한다.
	if (pLine == NULL) return;
	SetCaret(FALSE,FALSE);

	if ((pFrame->HideSelType != 2 && SelStart != SelEnd) ||
		(pFrame->TestStyle(AES_HIDECURLINE) == TRUE && pFrame->nShowCurLine != 0)) {
		Invalidate(-1);
	}
	pFrame->ChangeActiveView(this,0);
}

void CApiEditText::OnKillFocus(HWND hWnd, HWND hwndNewFocus)
{
	EndComp();
	DestroyCaret();

	if ((pFrame->HideSelType != 2 && SelStart != SelEnd) ||
		(pFrame->TestStyle(AES_HIDECURLINE) == TRUE && pFrame->nShowCurLine != 0)) {
		Invalidate(-1);
	}
}

void CApiEditText::OnChar(HWND hWnd, TCHAR ch, int cRepeat)
{
	TCHAR szChar[256];
	int i;
	BOOL bPrevSel;
	TCHAR szCom[256];

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

	if ((ch < ' ' && ch != '\r' && ch != '\t') || ch==127)
		return;
	
	if (pFrame->bReadOnly || bCapture)
		return;

	if (ch == '\r') {
		szChar[0]='\r';
		szChar[1]='\n';
		szChar[2]=0;
	} else {
		szChar[0]=ch;
		szChar[1]=0;
	}

	if (ch == '\r') {
		lstrcpy(szCom,"insert \"\\n\"");
	} else if (ch == '\t') {
		lstrcpy(szCom,"insert \"\\t\"");
	} else if (ch == '\"') {
		lstrcpy(szCom,"insert \"\\\"\"");
	} else if (ch == '\\') {
		lstrcpy(szCom,"insert \"\\\\\"");
	} else {
		wsprintf(szCom,"insert \"%s\"",szChar);
	}
	AeMc.RecordCommand(szCom);
	ProcessCursorInEditing();

	if (ch=='\t') {
		if (pFrame->TestStyle(AES_SPACEFORTAB)) {
			for (i=0;i<TabWidth;i++) {
				szChar[i]=' ';
			}
			szChar[i]=0;
		}
		if (pFrame->TestStyle(AES_BLOCKINDENTWITHTAB) && SelStart != SelEnd) {
			BlockIndent((GetKeyState(VK_SHIFT) & 0x8000) != 0);
			return;
		}
	}

	InputExtraSpace(off);
	
	if (SelStart != SelEnd) {
		pFrame->StartUndoGroup(1);
	}
	bPrevSel=DeleteSelection();
	for (i=0;i<cRepeat;i++) {
		if (bOvr && bPrevSel==FALSE && ch!='\r') {
			if (pFrame->IsDBCS(off)) {
				if (!pFrame->IsEnterCode(off)) {
					Delete(off,2);
				}
			} else {
				Delete(off,1);
			}
		}
		Insert(off,szChar,lstrlen(szChar));
		off+=lstrlen(szChar);
	}
	if (bPrevSel) {
		pFrame->EndUndoGroup();
	}
	bComp=FALSE;
	Invalidate(pFrame->FindParaStart(off-lstrlen(szChar)));

	if (pFrame->TestStyle(AES_AUTOINDENT)) {
		ProcessIndent(ch);
	}

	SetCaret();
	TestAutoClip(ch);
}

LRESULT CApiEditText::OnImeChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	TCHAR szChar[3];
	TCHAR szCom[256];

	if (IsDBCSLeadByte((BYTE)(wParam >> 8))) {
		szChar[0]=HIBYTE(LOWORD(wParam));
		szChar[1]=LOBYTE(LOWORD(wParam));
		szChar[2]=0;
	} else {
		szChar[0]=(BYTE)wParam;
		szChar[1]=0;
	}

	if (bComp) {
		pFrame->URInsert(off-lstrlen(szChar),szChar,lstrlen(szChar));
		pFrame->buf[off-2]=szChar[0];
		pFrame->buf[off-1]=szChar[1];
		if (bPrevCompoGroup) {
			pFrame->EndUndoGroup();
			bPrevCompoGroup=FALSE;
		}
	} else {
		if (bOvr) {
			if (pFrame->IsDBCS(off)) {
				if (!pFrame->IsEnterCode(off)) {
					Delete(off,2);
				}
			} else {
				Delete(off,1);
			}
		}
		Insert(off,szChar,lstrlen(szChar));
		off+=lstrlen(szChar);
	}

	wsprintf(szCom,"insert \"%s\"",szChar);
	AeMc.RecordCommand(szCom);

	bComp=FALSE;
	Invalidate(pFrame->FindParaStart(off-lstrlen(szChar)));
	SetCaret();
	TestAutoClip(wParam);
	return 0;
}

LRESULT CApiEditText::OnImeComposition(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HIMC hImc;
	TCHAR *szComp;
	int len;
	BOOL bPrevSel;
	BOOL bNewIns=TRUE;

	if (pFrame->bReadOnly || bCapture)
		return 0;

	InputExtraSpace(off);
	if (SelStart != SelEnd) {
		pFrame->StartUndoGroup(1);
		bPrevCompoGroup=TRUE;
	}
	bPrevSel=DeleteSelection();
	hImc=ImmGetContext(hWnd);
	if (lParam & GCS_COMPSTR) {
		ProcessCursorInEditing();
		len=ImmGetCompositionString(hImc,GCS_COMPSTR,NULL,0);
		szComp=(TCHAR *)malloc(len+1);
		ImmGetCompositionString(hImc,GCS_COMPSTR,szComp,len);
		szComp[len]=0;
		if (bComp && len!=0) {
			pFrame->buf[off-2]=szComp[0];
			pFrame->buf[off-1]=szComp[1];
			pFrame->UpdateViews(this,off-2,0);
		} else {
			if (bComp) {
				off-=2;
				Delete(off,2,FALSE);
				bNewIns=FALSE;
			}
			if (len == 0) {
				bComp=FALSE;
			} else {
				bComp=TRUE;
			}

			if (bOvr && bNewIns && bPrevSel==FALSE) {
				if (pFrame->IsDBCS(off)) {
					if (!pFrame->IsEnterCode(off)) {
						Delete(off,2);
					}
				} else {
					Delete(off,1);
				}
			}

			Insert(off,szComp,lstrlen(szComp),FALSE);
			off+=len;
		}

		ImmReleaseContext(hWnd,hImc);
		free(szComp);
		Invalidate(pFrame->FindParaStart(off-len));
		SetCaret();
	}

	return DefWindowProc(hWnd,WM_IME_COMPOSITION,wParam,lParam);
}

void CApiEditText::OnHScroll(HWND hWnd, HWND hwndCtl, UINT code, int pos)
{
	int xInc;
	SCROLLINFO si;

	if (!IsWindowEnabled(hHorzScroll)) {
		return;
	}
	xInc=0;
	switch (code) {
	case SB_LINEUP:
		xInc=-FontHeight;
		break;
	case SB_LINEDOWN:
		xInc=FontHeight;
		break;
	case SB_PAGEUP:
		xInc=-(frt.right-frt.left);
		break;
	case SB_PAGEDOWN:
		xInc=frt.right-frt.left;
		break;
	case SB_THUMBTRACK:
		si.cbSize=sizeof(SCROLLINFO);
		si.fMask=SIF_TRACKPOS;
		GetScrollInfo(hHorzScroll,SB_CTL,&si);
		xInc=si.nTrackPos-xPos;
		break;
	default:
		break;
	}

	xInc=max(-xPos, min(xInc, xMax-xPos));
	xPos=xPos+xInc;
	if (pFrame->hBackBit && pFrame->BackBitPos!=AEBB_TILESCROLL) {
		Invalidate(-1);
	} else {
		ScrollWindow(hWnd, -xInc, 0, &frt, &frt);
	}
	SetScrollPos(hHorzScroll, SB_CTL, xPos, TRUE);
	ManageCaret();
}

void CApiEditText::OnVScroll(HWND hWnd, HWND hwndCtl, UINT code, int pos)
{
	int yInc;
	SCROLLINFO si;
	int LinePerPage;
	RECT srt;

	if (!IsWindowEnabled(hVertScroll)) {
		return;
	}
	LinePerPage=(Fheight/LineHeight)*LineHeight;
	yInc=0;
	switch (code) {
	case SB_LINEUP:
		yInc=-LineHeight;
		break;
	case SB_LINEDOWN:
		yInc=LineHeight;
		break;
	case SB_PAGEUP:
		yInc=-LinePerPage;
		break;
	case SB_PAGEDOWN:
		yInc=LinePerPage;
		break;
	case SB_THUMBTRACK:
		si.cbSize=sizeof(SCROLLINFO);
		si.fMask=SIF_TRACKPOS;
		GetScrollInfo(hVertScroll,SB_CTL,&si);
		yInc=si.nTrackPos-yPos;
		break;
	default:
		break;
	}

	yInc=max(-yPos, min(yInc, yMax-yPos-LinePerPage));
	yInc=yInc-(yInc % LineHeight);
	yPos=yPos+yInc;
	// 포맷팅 영역과 마진 영역만 스크롤한다.
	srt=frt;
	srt.left=0;
	if (pFrame->hBackBit && pFrame->BackBitPos!=AEBB_TILESCROLL) {
		Invalidate(-1);
	} else {
		ScrollWindow(hWnd, 0, -yInc, &srt,&srt);
	}
	SetScrollPos(hVertScroll, SB_CTL, yPos, TRUE);
	ManageCaret();
}

void CApiEditText::OnContextMenu(HWND hWnd, HWND hwndContext, UINT xPos, UINT yPos)
{
	HMENU hPopup;
	POINT pt;

	hPopup=CreatePopupMenu();
	AppendMenu(hPopup,MF_STRING,IDM_AE_CUT,"잘라내기(&T)");
	AppendMenu(hPopup,MF_STRING,IDM_AE_COPY,"복사(&C)");
	if (ViewType == HEXA_VIEW) {
		AppendMenu(hPopup,MF_STRING,IDM_AE_COPYDUMP,"덤프복사(&D)");
	}
	AppendMenu(hPopup,MF_STRING,IDM_AE_PASTE,"붙여넣기(&P)");
	AppendMenu(hPopup,MF_STRING,IDM_AE_SELALL,"모두선택(&A)");
	SendMessage(GetParent(pFrame->hWnd),pFrame->UserBase+4,(WPARAM)hPopup,0);

	if (IsClipboardFormatAvailable(CF_TEXT)==FALSE) {
		EnableMenuItem(hPopup, IDM_AE_PASTE, MF_BYCOMMAND | MF_GRAYED);
	}

	if (SelStart == SelEnd) {
		EnableMenuItem(hPopup, IDM_AE_CUT, MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(hPopup, IDM_AE_COPY, MF_BYCOMMAND | MF_GRAYED);
	}

	if (xPos==65535) {
		GetCaretPos(&pt);
		ClientToScreen(hWnd,&pt);
		xPos=pt.x;
		yPos=pt.y;
	}
	TrackPopupMenu(hPopup,TPM_LEFTALIGN | TPM_RIGHTBUTTON,(short)xPos,(short)yPos,0,hWnd,NULL);
	DestroyMenu(hPopup);
}

void CApiEditText::OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify)
{
	HGLOBAL hmem;
	TCHAR *ptr;
	int SelFirst, SelSecond;
	TCHAR *ptr2;
	HGLOBAL hmem2;
	int len,idx,linelen,len2;

	switch (id) {
	case IDM_AE_CUT:
		if (SelStart != SelEnd && pFrame->bReadOnly==FALSE && bCapture==FALSE) {
			AeMc.RecordCommand("cut");
			AeMc.runlevel++;
			SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_AE_COPY,0),0);
			AeMc.runlevel--;
			DeleteSelection();
			Invalidate(pFrame->FindParaStart(off));
			SetCaret();
		}
		return;
	case IDM_AE_COPY:
		if (SelStart != SelEnd) {
			AeMc.RecordCommand("copy");
			if (!bColumnMode) {
				SelFirst=min(SelStart,SelEnd);
				SelSecond=max(SelStart,SelEnd);
				hmem=GlobalAlloc(GHND, SelSecond-SelFirst+1);
				ptr=(TCHAR *)GlobalLock(hmem);
				memcpy(ptr,pFrame->buf+SelFirst,SelSecond-SelFirst);
				GlobalUnlock(hmem);
				if (OpenClipboard(hWnd)) {
					EmptyClipboard();
					SetClipboardData(CF_TEXT,hmem);
					CloseClipboard();
				}
			} else {
				len=MakeSelTable();
				hmem=GlobalAlloc(GHND, len+1);
				ptr=(TCHAR *)GlobalLock(hmem);
				for (idx=0;SelTable[idx].Line!=-1;idx++) {
					linelen=SelTable[idx].End-SelTable[idx].Start;
					memcpy(ptr,pFrame->buf+SelTable[idx].Start,linelen);
					ptr+=linelen;
					*ptr++='\r';
					*ptr++='\n';
				}
				*ptr=0;
				GlobalUnlock(hmem);

				hmem2=GlobalAlloc(GHND, len+1);
				ptr2=(TCHAR *)GlobalLock(hmem2);
				for (idx=0;SelTable[idx].Line!=-1;idx++) {
					linelen=SelTable[idx].End-SelTable[idx].Start;
					memcpy(ptr2,pFrame->buf+SelTable[idx].Start,linelen);
					ptr2+=linelen;
					*ptr2++=0;
				}

				*ptr2=(TCHAR)0xff;
				GlobalUnlock(hmem2);
				FreeSelTable();
				if (OpenClipboard(hWnd)) {
					EmptyClipboard();
					SetClipboardData(CF_TEXT,hmem);
					SetClipboardData(CApiEdit::AeColFormat,hmem2);
					CloseClipboard();
				}
			}
		}
		return;
	case IDM_AE_PASTE:
		BOOL bPrevSel;
		if (IsClipboardFormatAvailable(CF_TEXT) && pFrame->bReadOnly==FALSE && 
			bCapture==FALSE) {
			AeMc.RecordCommand("paste");
			if (IsClipboardFormatAvailable(CApiEdit::AeColFormat)) {
				OpenClipboard(hWnd);
				hmem=GetClipboardData(CApiEdit::AeColFormat);
				ptr=(TCHAR *)GlobalLock(hmem);
				pFrame->StartUndoGroup(17);
				DeleteSelection();
				PasteColumn(off,ExtraX,ptr);
				pFrame->EndUndoGroup();
				GlobalUnlock(hmem);
				CloseClipboard();
				Invalidate(pFrame->FindParaStart(off));
			} else {
				InputExtraSpace(off);
				if (pFrame->TestStyle(AES_SMARTSPACE) && ViewType!=HEXA_VIEW) {
					pFrame->StartUndoGroup(25);
				}
				if (SelStart != SelEnd) {
					pFrame->StartUndoGroup(2);
				}
				bPrevSel=DeleteSelection();
				OpenClipboard(hWnd);
				hmem=GetClipboardData(CF_TEXT);
				ptr=(TCHAR *)GlobalLock(hmem);
				Insert(off,ptr,lstrlen(ptr));

				if (bPrevSel) {
					pFrame->EndUndoGroup();
				}
				GlobalUnlock(hmem);
				CloseClipboard();
				Invalidate(pFrame->FindParaStart(off));
				SelFirst=off;
				off += lstrlen(ptr);
				SelSecond=off;
				if (pFrame->TestStyle(AES_SMARTSPACE) && ViewType!=HEXA_VIEW) {
					AdjustSpaceInsert(SelFirst,SelSecond);
					off=SelSecond;
					pFrame->EndUndoGroup();
				}
			}
			SetCaret();
		}
		return;
	case IDM_AE_SELALL:
		AeMc.RecordCommand("select doc");
		if (bColumnMode) {
			SetColumnMode(FALSE);
		}
		SelStart=0;
		SelEnd=pFrame->doclen;
		off=SelEnd;
		Invalidate(-1);
		SetCaret();
		return;
	case IDM_AE_CUTAPPEND:
		AeMc.RecordCommand("cutapp");
		if (!IsClipboardFormatAvailable(CF_TEXT)) {
			SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_AE_CUT,0),0);
			return;
		}
		if (SelStart != SelEnd && pFrame->bReadOnly==FALSE && bCapture==FALSE) {
			AeMc.runlevel++;
			SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_AE_COPYAPPEND,0),0);
			AeMc.runlevel--;
			DeleteSelection();
			Invalidate(pFrame->FindParaStart(off));
			SetCaret();
		}
		return;
	case IDM_AE_COPYAPPEND:
		AeMc.RecordCommand("copyapp");
		if (!IsClipboardFormatAvailable(CF_TEXT)) {
			SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_AE_COPY,0),0);
			return;
		}
		if (SelStart != SelEnd) {
			OpenClipboard(hWnd);
			hmem=GetClipboardData(CF_TEXT);
			ptr=(TCHAR *)GlobalLock(hmem);
			GlobalUnlock(hmem);
			CloseClipboard();

			if (!bColumnMode) {
				SelFirst=min(SelStart,SelEnd);
				SelSecond=max(SelStart,SelEnd);
				hmem=GlobalAlloc(GHND, lstrlen(ptr)+SelSecond-SelFirst+1);
				ptr2=(TCHAR *)GlobalLock(hmem);
				lstrcpy(ptr2,ptr);
				memcpy(ptr2+lstrlen(ptr2),pFrame->buf+SelFirst,SelSecond-SelFirst);
				GlobalUnlock(hmem);
				if (OpenClipboard(hWnd)) {
					EmptyClipboard();
					SetClipboardData(CF_TEXT,hmem);
					CloseClipboard();
				}
			} else {
				len2=lstrlen(ptr);
				len=MakeSelTable();

				hmem=GlobalAlloc(GHND,len2+len+1);
				ptr2=(TCHAR *)GlobalLock(hmem);
				lstrcpy(ptr2,ptr);
				ptr2+=len2;
				for (idx=0;SelTable[idx].Line!=-1;idx++) {
					linelen=SelTable[idx].End-SelTable[idx].Start;
					memcpy(ptr2,pFrame->buf+SelTable[idx].Start,linelen);
					ptr2+=linelen;
					*ptr2++='\r';
					*ptr2++='\n';
				}
				*ptr2=0;
				GlobalUnlock(hmem);

				if (IsClipboardFormatAvailable(CApiEdit::AeColFormat)) {
					OpenClipboard(hWnd);
					hmem2=GetClipboardData(CApiEdit::AeColFormat);
					ptr=(TCHAR *)GlobalLock(hmem2);
					GlobalUnlock(hmem2);
					CloseClipboard();
					len2=0;
					ptr2=ptr;
					while (*ptr2!=(TCHAR)0xff) {
						len2++;
						ptr2++;
					}

					hmem2=GlobalAlloc(GHND,len2+len+1);
					ptr2=(TCHAR *)GlobalLock(hmem2);
					memcpy(ptr2,ptr,len2);
					ptr2+=len2;
					for (idx=0;SelTable[idx].Line!=-1;idx++) {
						linelen=SelTable[idx].End-SelTable[idx].Start;
						memcpy(ptr2,pFrame->buf+SelTable[idx].Start,linelen);
						ptr2+=linelen;
						*ptr2++=0;
					}
					*ptr2=(TCHAR)0xff;
					GlobalUnlock(hmem2);
				} else {
					hmem2=NULL;
				}
				FreeSelTable();
				if (OpenClipboard(hWnd)) {
					EmptyClipboard();
					SetClipboardData(CF_TEXT,hmem);
					if (hmem2) {
						SetClipboardData(CApiEdit::AeColFormat,hmem2);
					}
					CloseClipboard();
				}
			}
		}
	default:
		SendMessage(GetParent(pFrame->hWnd),pFrame->UserBase+5,id,0);
		return;
	}
}

void CApiEditText::OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	int toff;
	BOOL bShift, bControl;
	int r,c;
	int nr;
	int SelFirst, SelSecond;
	RECT crt;

	ShowCursorEdit(TRUE);
	GetClientRect(hWnd,&crt);
	// 스플릿 박스 위면 창 분할을 시작한다.
	if (StartSplit(hWnd,fDoubleClick,x,y)) {
		return;
	}
	// 사용금지된 스크롤 바 위에서의 클릭은 무시한다.
	if (y > crt.bottom-SBW || x > crt.right-SBW) {
		return;
	}

	// 눈금자 위에서의 클릭은 무시한다.
	if (pFrame->TestStyle(AES_SHOWRULER) && y < RULERHEIGHT) {
		return;
	}

	bFindSelect=FALSE;
	bShift=((GetKeyState(VK_SHIFT) & 0x8000) != 0); 
	bControl=((GetKeyState(VK_CONTROL) & 0x8000) != 0);

	SetFocus(hWnd);

	if (IsInSelection(x+xPos, y+yPos) && pFrame->bReadOnly==FALSE && pFrame->TestStyle(AES_ALLOWDRAG)) {
		bDragSel=TRUE;
		SetCapture(hWnd);
		bCapture=TRUE;
		DestroyCaret();
		CreateCaret(hWnd,(HBITMAP)1,2,FontHeight);
		ShowCaret(hWnd);
		return;
	}

	if (x < MarginWidth) {
		toff=GetOffFromXY(x+xPos,y+yPos);
		GetRCFromOff(toff,r,c);
		if (fDoubleClick) {
			while (pLine[r].nLine!=0) r--;
			for (nr=r+1;;nr++) {
				if (nr == TotalLine || pLine[nr].nLine == 0) {
					break;
				}
			}
			nr--;

			SelStart=pLine[r].Start;
			off=SelEnd=pFrame->IncludeEnter(pLine[nr].End);
			SetCaret();
			Invalidate(-1);
			return;
		}

		if (bControl) {
			SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_AE_SELALL,0),0);
			return;
		}

		if (bShift) {
			SelFirst=min(SelStart,SelEnd);
			SelSecond=max(SelStart,SelEnd);
			if (SelStart == SelEnd) {
				GetRCFromOff(off,nr,c);
			} else {
				if (toff > SelFirst) {
					GetRCFromOff(SelFirst,nr,c);
				} else {
					GetRCFromOff(SelSecond,nr,c);
				}
			}
			SelStartLine=nr;

			if (r >= SelStartLine) {
				SelStart=pLine[SelStartLine].Start;
				off=SelEnd=pFrame->IncludeEnter(pLine[r].End);
			} else {
				SelStart=pLine[SelStartLine].End;
				off=SelEnd=pLine[r].Start;
			}
		} else {
			SelStart=pLine[r].Start;;
			off=SelEnd=pFrame->IncludeEnter(pLine[r].End);
			SelStartLine=r;
		}
		SetCaret();
		Invalidate(-1);
		bSelLine=TRUE;
		SetCapture(hWnd);
		bCapture=TRUE;
		return;
	}

	if (fDoubleClick) {
		toff=GetOffFromXY(x+xPos,y+yPos);
		GetNowWord(toff,SelStart,SelEnd);
		if (SelStart != SelEnd) {
			off=SelEnd;
			SetCaret();
			Invalidate(-1);
		}
		return;
	}

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

void CApiEditText::OnMouseMove(HWND hWnd, int x, int y, UINT keyFlags)
{
	BOOL bInstTimer;
	int r,c;
	int OldOff;
	int toff;
	BOOL bControl;
	int sx,sy;

	ShowCursorEdit(TRUE);
	if (bCapture == FALSE) {
		return;
	}
	// 창 분할중이면 분할을 처리한다.
	if (Spliting(hWnd,x,y)) {
		return;
	}

	bControl=((GetKeyState(VK_CONTROL) & 0x8000) != 0);
	if (pFrame->TestStyle(AES_SWAPDRAGBUTTON)) {
		bControl=!bControl;
	}

	if (bDragSel) {
		if (x > frt.left && x < frt.right && y > frt.top && y < frt.bottom) {
			toff=GetOffFromXY(x+xPos,y+yPos);
			GetXYFromOff(toff,sx,sy);
			SetCaretPos(sx-xPos,sy-yPos+GetCharYPos());
			if (bControl) {
				SetCursor(CApiEdit::hCCopy);
			} else {
				SetCursor(CApiEdit::hCMove);
			}
		} else {
			SetCursor(CApiEdit::hCNoDrop);
		}
	} else {
		OldOff=off;
		if (bSelLine) {
			toff=GetOffFromXY(x+xPos,y+yPos);
			GetRCFromOff(toff,r,c);

			if (r >= SelStartLine) {
				SelStart=pLine[SelStartLine].Start;
				off=SelEnd=pFrame->IncludeEnter(pLine[r].End);
			} else {
				SelStart=pLine[SelStartLine].End;
				off=SelEnd=pLine[r].Start;
			}
		} else {
			// 선택 중에는 가상 위치로 벗어날 수 없음. 단, 컬럼 모드일 때는 예외
			if (!bColumnMode) {
				ignoreVirt++;
			}
			off=SelEnd=GetOffFromXY(x+xPos,y+yPos);
			if (!bColumnMode) {
				ignoreVirt--;
			} else {
				SelExtraX=ExtraX;
			}
		}
		SetCaret();
		Invalidate(min(OldOff,off),max(OldOff,off));
		if (bColumnMode || pFrame->SelBlockType==2) {
			Invalidate(min(SelStart,SelEnd),max(SelStart,SelEnd));
		}
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
	if (nWrap == 0) {
		GetRCFromOff(SelEnd,r,c);
		if (x>frt.right && SelEnd != pLine[r].End) {
			SendMessage(hWnd, WM_HSCROLL, SB_LINERIGHT, 0L);
			bInstTimer=TRUE;
		}
		if (x<frt.left && SelEnd != pLine[r].Start) {
			SendMessage(hWnd, WM_HSCROLL, SB_LINELEFT, 0L);
			bInstTimer=TRUE;
		}
	}
	if (bInstTimer==TRUE) {
		SetTimer(hWnd, 1, 100, NULL);
	} else {
		KillTimer(hWnd, 1);
	}
}

void CApiEditText::OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags)
{
	BOOL bControl;
	int SelLen;
	int toff;
	HIMC hImc;

	bCapture=FALSE;
	bSelLine=FALSE;
	ReleaseCapture();
	KillTimer(hWnd,1);
	hImc=ImmGetContext(hWnd);
	ImmNotifyIME(hImc,NI_COMPOSITIONSTR,CPS_CANCEL,0);
	ImmReleaseContext(hWnd,hImc );

	// 창 분할중이었으면 분할을 종료한다.
	if (EndSplit(hWnd,x,y)) {
		return;
	}
	bControl=((GetKeyState(VK_CONTROL) & 0x8000) != 0);
	if (pFrame->TestStyle(AES_SWAPDRAGBUTTON)) {
		bControl=!bControl;
	}

	if (bDragSel) {
		bDragSel=FALSE;
		if (IsInSelection(x+xPos, y+yPos)==TRUE) {
			off=GetOffFromXY(x+xPos,y+yPos);
			SetCaret();
			SelStart=SelEnd=0;
			Invalidate(-1);
		} else {
			if (x > frt.left && x < frt.right && y > frt.top && y < frt.bottom) {
				SelLen=abs(SelStart-SelEnd);
				toff=GetOffFromXY(x+xPos,y+yPos);
				if (bColumnMode) {
					ColumnCopyString(bControl,toff,ExtraX);
					return;
				} else {
					CopyString(bControl,min(SelStart,SelEnd),toff,SelLen);
				}

				SelStart=toff;
				SelEnd=SelStart+SelLen;
				off=SelEnd;
				SetCaret();
				Invalidate(-1);
			} else {
				SetCaret();
			}
		}
	}
}

void CApiEditText::OnTimer(HWND hWnd, UINT id)
{
	POINT pt;

	switch (id) {
	case 1:
		GetCursorPos(&pt);
		ScreenToClient(hWnd, &pt);
		SendMessage(hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
		break;
	}
}

BOOL CApiEditText::OnSetCursor(HWND hWnd, HWND hwndCursor, UINT codeHitTest, UINT msg)
{
	POINT pt;
	RECT crt;

	GetClientRect(hWnd,&crt);
	GetCursorPos(&pt);
	ScreenToClient(hWnd,&pt);

	if (codeHitTest==HTCLIENT) {
		if (pt.x < MarginWidth && pt.y < crt.bottom-SBW) {
			SetCursor(CApiEdit::hCMargin);
			return TRUE;
		}

		// 수직 스플릿 박스의 커서
		if (bVertSplit && pt.x > crt.right-SBW && pt.y < SPLITWIDTH) {
			SetCursor(LoadCursor(NULL,IDC_SIZENS));
			return TRUE;
		}

		// 수평 스클릿 박스의 커서
		if (bHorzSplit && pt.x < SPLITWIDTH && pt.y > crt.bottom-SBW) {
			SetCursor(LoadCursor(NULL,IDC_SIZEWE));
			return TRUE;
		}

		// 사용금지된 스크롤 바 위에서의 화살표 커서를 표시한다.
		if (pt.y > crt.bottom-SBW || pt.x > crt.right-SBW) {
			SetCursor(LoadCursor(NULL,IDC_ARROW));
			return TRUE;
		}

		// 눈금자 위에서는 화살표 커서를 표시한다.
		if (pFrame->TestStyle(AES_SHOWRULER) && pt.y < RULERHEIGHT) {
			SetCursor(LoadCursor(NULL,IDC_ARROW));
			return TRUE;
		}

		pt.x += xPos;
		pt.y += yPos;
		if (IsInSelection(pt.x, pt.y)) {
			SetCursor(CApiEdit::hCSel);
			return TRUE;
		}
	}

	return FORWARD_WM_SETCURSOR(hWnd,hwndCursor,codeHitTest,msg,DefWindowProc);
}

UINT CApiEditText::OnGetDlgCode(HWND hWnd, LPMSG lpmsg)
{
	if (lpmsg) {
		if (lpmsg->message == WM_KEYDOWN && lpmsg->wParam=='\t' && (pFrame->TestStyle(AES_WANTTAB) == FALSE))
			return 0;
	}
	return DLGC_WANTARROWS | DLGC_WANTTAB | DLGC_WANTALLKEYS | DLGC_WANTCHARS;
}

void CApiEditText::OnMouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	short zDelta;
	UINT Lines;
	int Unit;
	int nScroll=0;
	int Value;
	LOGFONT tFont;
	BOOL bShift, bControl;
	int action;

	bShift=((GetKeyState(VK_SHIFT) & 0x8000) != 0); 
	bControl=((GetKeyState(VK_CONTROL) & 0x8000) != 0);

	SystemParametersInfo(SPI_GETWHEELSCROLLLINES,0,&Lines,0);
	Unit=WHEEL_DELTA/Lines;
	zDelta=(short)HIWORD(wParam);
	SumDelta += zDelta;
	while (abs(SumDelta) >= Unit) {
		if (SumDelta > 0) {
			nScroll--;
			SumDelta-=Unit;
		} else {
			nScroll++;
			SumDelta+=Unit;
		}
	}

	// 컨트롤 키와 쉬프트키의 상태로부터 동작을 결정한다.
	if (bControl) {
		if (bShift) {
			action=pFrame->nCtrlShiftWheel;
		} else {
			action=pFrame->nCtrlWheel;
		}
	} else {
		if (bShift) {
			action=pFrame->nShiftWheel;
		} else {
			action=pFrame->nWheel;
		}
	}

	if (action==1) {
		Scroll(nScroll);
	} else {
		while ((UINT)abs(nScroll) >= Lines) {
			switch (action) {
			case 0:
				break;
			case 2:
				if (nScroll > 0) {
					Scroll(1);
				} else {
					Scroll(-1);
				}
				break;
			case 3:
				if (nScroll > 0) {
					SendMessage(hWnd,WM_VSCROLL,MAKELONG(SB_PAGEDOWN,0),0);
				} else {
					SendMessage(hWnd,WM_VSCROLL,MAKELONG(SB_PAGEUP,0),0);
				}
				break;
			case 4:
				if (nScroll > 0) {
					Scroll(Fheight/LineHeight/2);
				} else {
					Scroll(-Fheight/LineHeight/2);
				}
				break;
			case 5:
				Value=GetLineRatio();
				if (nScroll > 0) {
					Value=max(Value-10,100);
				} else {
					Value=min(Value+10,1000);
				}
				SetLineRatio(Value);
				break;
			case 6:
				GetFont(&tFont);
				if (tFont.lfHeight != 0) {
					if (nScroll > 0) {
						tFont.lfHeight=max(tFont.lfHeight-3,12);
					} else {
						tFont.lfHeight=min(tFont.lfHeight+3,128);
					}
					SetFont(&tFont);
				}
				break;
			case 7:
				if (nScroll > 0) {
					if (pFrame->CanUndo()) {
						pFrame->Undo();
					}
				} else {
					if (pFrame->CanRedo()) {
						pFrame->Redo();
					}
				}
				break;
			}
			if (nScroll > 0) {
				nScroll -= Lines;
			} else {
				nScroll += Lines;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////
// 일반 함수들

int CApiEditText::GetCharWidth(TCHAR *ch, int len)
{
	SIZE sz;
	HDC hdc;
	HFONT hFont, hOldFont=NULL;
	int code;
	int idx;

	if (len==1) {
		idx=*(BYTE *)ch;
		if (idx==' ') {
			return SpaceWidth;
		} else {
			return arChWidth[idx]*pFrame->CharExtra/100;
		}
	} else {
		code=*(BYTE *)ch << 8 | *((BYTE *)ch+1);
		if (code == 0x0d0a) {
			return 0;
		}
		if (arHanWidth[code-0x8000]==0xff) {
			hdc=GetDC(hWnd);
			if (logfont.lfHeight != 0) {
				if (pFrame->CharRatio != 100) {
					logfont.lfWidth=logfont.lfHeight*pFrame->CharRatio/200;
				}
				hFont=CreateFontIndirect(&logfont);
				hOldFont=(HFONT)SelectObject(hdc,hFont);
			}
			GetTextExtentPoint32(hdc,ch,2,&sz);
			if (hOldFont) {
				DeleteObject(SelectObject(hdc,hOldFont));
			}
			ReleaseDC(hWnd,hdc);
			arHanWidth[code-0x8000]=(BYTE)sz.cx;
		}
		return arHanWidth[code-0x8000]*pFrame->CharExtra/100;
	}
}

void CApiEditText::SetCaret(BOOL bUpdatePrevX/*=TRUE*/, BOOL bScrollToCaret/*=TRUE*/)
{
	int toff;
	int caretwidth;
	int x,y;
	int ty;
	BOOL bScroll=FALSE;
	POINT OldPos;
	RECT InvRt;
	RECT crt;

	GetCaretPos(&OldPos);

	if (bComp) {
		toff=off-2;
		caretwidth=GetCharWidth(pFrame->buf+toff,2);
	} else {
		toff=off;
		caretwidth=pFrame->CaretWidth; 
	}

	if (bOvr) {
		if (pFrame->IsDBCS(toff)) {
			if (pFrame->IsEnterCode(toff)) {
				caretwidth=SpaceWidth;
			} else {
				caretwidth=GetCharWidth(pFrame->buf+toff,2);
			}
		} else {
			caretwidth=GetCharWidth(pFrame->buf+toff,1);
		}
	}
	if (GetFocus()==hWnd) {
		CreateCaret(hWnd,NULL,caretwidth,FontHeight);
		ShowCaret(hWnd);
	}

	GetXYFromOff(toff,x,y);
	GetClientRect(hWnd,&crt);
	if (bScrollToCaret && crt.right) {
		pFrame->SendNotify(AEN_MOVE);
		if (nWrap==0) {
			if ((x+caretwidth > xPos+frt.right) || (x < xPos+frt.left)) {
				// 순수 문서 좌표에서 폭의 절반을 뺀 위치를 새로 정함
				xPos=max(0,PureX(x)-(frt.right-frt.left)/2);
				bScroll=TRUE;
			}
		}

		// 순수 문서상의 좌표가 yPos보다 더 작으면 그 줄을 첫줄로 바꾼다.
		if (PureY(y) < yPos) {
			yPos=PureY(y);
			bScroll=TRUE;
		}

		if (y+FontHeight > yPos+frt.bottom) {
			// ty=화면 제일 아래쪽의 보이는 줄의 화면상의 좌표. 줄간 영역은 제외. 배수 조정
			ty=(frt.bottom-frt.top-FontHeight)/LineHeight*LineHeight;
			// 스크롤 위치 = 순서 문서상의 좌표에서 제일 아래 줄 좌표.
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
	if (bUpdatePrevX) {
		PrevX=x;
	}
	if (pFrame->nShowCurLine != 0 && OldPos.y != y-yPos) {
		InvRt.left=0;
		InvRt.right=frt.right;
		InvRt.top=min(OldPos.y,y-yPos)/LineHeight*LineHeight;
		InvRt.bottom=max(OldPos.y,y-yPos)+LineHeight;
		InvalidateRect(hWnd,&InvRt,FALSE);
	}
}

void CApiEditText::Insert(int nPos, TCHAR *str, int len, BOOL bRec/*=TRUE*/)
{
	int nCrLf=0,i;
	int r,c;
	int nowPara;
	TCHAR *p=str;

	if (pFrame->bReadOnly)
		return;

	pFrame->Insert(nPos,str,len,bRec);

	bLineEnd=FALSE;
	UpdateLineInfo(nPos,len);
	UpdateScrollInfo();

	for (i=0;i<len;i++) {
		if (pFrame->IsEnterCode(str,len,p))
			nCrLf++;
		p++;
	}

	if (nCrLf) {
		GetRCFromOff(nPos,r,c);
		nowPara=pLine[r].nPara;
		if (c != 0) {
			nowPara++;
		}

		for (i=0;i<MAXBOOKMARK;i++) {
			if (arMark[i] >= nowPara) {
				arMark[i]+=nCrLf;
			}
		}
	}
	pFrame->UpdateViews(this,nPos,len);
}

void CApiEditText::Delete(int nPos, int nCount, BOOL bRec/*=TRUE*/)
{
	int nCrLf=0,i;
	int sr,sc,er,ec;
	int sPara,ePara;
	TCHAR *p=pFrame->buf+nPos;

	if (pFrame->bReadOnly)
		return;

	if (nCount == 0) return;
	if (pFrame->doclen < nPos+nCount) return;
	
	for (i=0;i<nCount;i++) {
		if (pFrame->IsEnterCode(p+i))
			nCrLf++;
	}

	if (nCrLf) {
		GetRCFromOff(nPos,sr,sc);
		sPara=pLine[sr].nPara;
		GetRCFromOff(nPos+nCount,er,ec);
		ePara=pLine[er].nPara;

		for (i=0;i<MAXBOOKMARK;i++) {
			if (arMark[i] >= sPara && arMark[i] < ePara) {
				arMark[i]=-1;
			}
			if (arMark[i] >= ePara) {
				arMark[i]-=nCrLf;
			}
		}
	}

	pFrame->Delete(nPos,nCount,bRec);
	UpdateLineInfo(nPos,-nCount);
	UpdateScrollInfo();
	pFrame->UpdateViews(this,nPos,-nCount);
}

void CApiEditText::GetLine(int Line, int &s, int &e)
{
	TCHAR *p, *Head;
	int len, acwidth;
	TCHAR *EndPos=NULL;
	TCHAR *EndPosNoSpace=NULL;
	BOOL IsPrevDBCS=FALSE;
	int Right;

	if (RightWrap == 0) {
		Right=frt.right-2;
	} else {
		Right=frt.left+RightWrap*FontWidth;
	}

	if (Line == 0) {
		p=pFrame->buf;
		s=0;
	} else {
		p=pFrame->buf+pLine[Line-1].End;
		if (AeIsDocEnd(p)) {
			s=-1;
			e=-1;
			return;
		}

		if (pFrame->IsEnterCode(p)) {
			p+=2;
		}

		s=p-pFrame->buf;
	}

	if (nWrap == 0 && RightWrap == 0) {
		while (!AeIsLineEnd(p)) {
			if (pFrame->IsDBCS(p-pFrame->buf)) {
				p++;
			}
			p++;
		}
	} else {
		for (acwidth=frt.left,Head=p;;) {
			if (*p == '\t') {
				len=1;
				acwidth=DocX((PureX(acwidth)/TabSize+1)*TabSize);
				EndPos=p;
			} else {
				if (pFrame->IsDBCS(p-pFrame->buf)) {
					len=2;

					if (nWrap==2 || (nWrap==3 && p != Head && IsPrevDBCS==FALSE)) {
						EndPos=p;
						if (*EndPos != ' ') {
							EndPosNoSpace=EndPos;
						}
					}
					IsPrevDBCS=TRUE;
				} else {
					len=1;

					if (IsPrevDBCS==TRUE) {
						EndPos=p;
						if (*EndPos != ' ') {
							EndPosNoSpace=EndPos;
						}
					}
					IsPrevDBCS=FALSE;
				}
				acwidth+=GetCharWidth(p,len);
			}

			if (AeIsLineEnd(p)) {
				EndPos=p;
				break;
			}

			if (acwidth > max(Right, frt.left+FontHeight*4)) {
				break;
			}

			if (*p == ' ' || *p=='\t') {
				EndPos=p+1;
				if (*EndPos != ' ') {
					EndPosNoSpace=EndPos;
				}
			}

			p+=len;
		}
	}

	if (nWrap == 1 || EndPos == NULL) {
		p=p;
	} else {
		p=EndPos;
	}

	if (pFrame->TestStyle(AES_NOFIRSTSPACE) && p < pFrame->buf+pFrame->doclen && *p==' ' && EndPosNoSpace!=NULL) {
		p=EndPosNoSpace;
	}
	e=p-pFrame->buf;
}

void CApiEditText::GetRCFromOff(int nPos, int &r, int &c)
{
	int Upper,Lower;

	assert(nPos >= 0 && nPos <= pFrame->doclen);
	Lower=0;
	Upper=TotalLine-1;
	for (;;) {
		r=(Upper+Lower)/2;

		if (nPos > pLine[r].Start && nPos < pLine[r].End)
			break;

		if (nPos == pLine[r].Start) {
			if (pLine[r].nLine != 0 && bLineEnd==TRUE) {
				r--;
			}
			break;
		}

		if (nPos == pLine[r].End) {
			if (AeIsLineEnd(pFrame->buf+pLine[r].End) || bLineEnd == TRUE) {
				break;
			}
		}

		if (pLine[r].Start > nPos) {
			Upper=r-1;
		} else {
			Lower=r+1;
		}
	}

	c=nPos-pLine[r].Start;
	if (ExtraX > 0) {
		c+=ExtraX;
	}
}

int CApiEditText::GetOffFromRC(int r, int c)
{
	int len;

	assert(r >= 0 && r <= TotalLine-1);
	len=pLine[r].End-pLine[r].Start;
	if (IsVirtualPosable()) {
		ExtraX=max(c-len,0);
	} else {
		ExtraX=0;
	}
	return pLine[r].Start+min(c,len);
}

void CApiEditText::GetXYFromOff(int nPos, int &x, int &y)
{
	int r,c;
	TCHAR *p;

	assert(nPos >= 0 && nPos <= pFrame->doclen);
	GetRCFromOff(nPos, r, c);
	y=r*LineHeight+frt.top;

	x=frt.left;
	for (p=pFrame->buf+pLine[r].Start;p!=pFrame->buf+nPos;) {
		assert(p < pFrame->buf+nPos);
		if (*p == '\t') {
			x = DocX((PureX(x)/TabSize+1)*TabSize);
			p++;
		} else {
			if (pFrame->IsDBCS(p-pFrame->buf)) {
				x += GetCharWidth(p,2);
				p+=2;
			} else {
				x += GetCharWidth(p,1);
				p++;
			}
		}
	}
	if (ExtraX > 0) {
		x += SpaceWidth*ExtraX;
	}
}

int CApiEditText::GetRowCount()
{
	return TotalLine;
}

void CApiEditText::SetWrap(int aWrap)
{
	nWrap=aWrap;
	if (nWrap && RightWrap==0) {
		EnableScrollBar(hHorzScroll,SB_CTL,ESB_DISABLE_BOTH);
		xPos=0;
	} else {
		EnableScrollBar(hHorzScroll,SB_CTL,ESB_ENABLE_BOTH);
	}

	Invalidate(-1);
	UpdateLineInfo();
	UpdateScrollInfo();
	SetCaret();
}

int CApiEditText::GetXPosOnLine(int r,int DestX)
{
	TCHAR *p;
	int len, acwidth;

	assert(r >= 0 && r <= TotalLine-1);
	if (DestX == frt.left) {
		return pLine[r].Start;
	}
	for (p=pFrame->buf+pLine[r].Start, acwidth=frt.left;;) {
		if (p-pFrame->buf == pLine[r].End) {
			if (IsVirtualPosable()) {
				ExtraX=(DestX-acwidth)/SpaceWidth;
			}
			break;
		}
		assert(p < pFrame->buf+pLine[r].End);

		if (*p == '\t') {
			len=1;
			acwidth=DocX((PureX(acwidth)/TabSize+1)*TabSize);
		} else {
			if (pFrame->IsDBCS(p-pFrame->buf)) {
				len=2;
			} else {
				len=1;
			}
			acwidth+=GetCharWidth(p,len);;
		}

		p+=len;
		
		if (acwidth >= DestX) {
			ExtraX=0;
			break;
		}
	}

	bLineEnd=(p-pFrame->buf==pLine[r].End && !AeIsLineEnd(p));
	return p-pFrame->buf;
}

void CApiEditText::UpdateScrollInfo()
{
	SCROLLINFO si;
	int i, MaxLength;

	yMax=(GetRowCount()+Fheight/LineHeight/2)*LineHeight;

	si.cbSize=sizeof(SCROLLINFO);
	si.fMask=SIF_ALL | SIF_DISABLENOSCROLL;
	si.nMin=0;
	si.nMax=yMax;
	si.nPage=(Fheight/LineHeight)*LineHeight;

	if (si.nMax < (int)si.nPage) {
		yPos=0;
		Invalidate(-1);
	}
	si.nPos=yPos;
	SetScrollInfo(hVertScroll, SB_CTL, &si, TRUE);
	if (si.nPage >= (unsigned)si.nMax) {
		EnableWindow(hVertScroll,FALSE);
	}

	if (nWrap == 0 || RightWrap != 0) {
		for (i=0,MaxLength=0;;i++) {
			if (pLine[i].Start==-1) {
				break;
			}
			MaxLength=max(MaxLength,(pLine[i].End-pLine[i].Start));
		}

		xMax=int(MaxLength*FontWidth*1.5);
		si.nMax=xMax;
		si.nPage=Fwidth;
		si.nPos=xPos;
		SetScrollInfo(hHorzScroll, SB_CTL, &si, TRUE);
		if (si.nPage >= (unsigned)si.nMax) {
			EnableWindow(hHorzScroll,FALSE);
		}
	}
}

int CApiEditText::DrawLine(HDC hdc, int Line)
{
	int x,y;
	int len;
	int nowoff;
	BOOL bInSel;
	COLORREF fore, back;
	int SelFirst, SelSecond;
	int r,c;
	BOOL bCurLine=FALSE;
	RECT lrt;
	HBRUSH hCurLine;
	int toff;
	int nUnit;
	int style;
	BOOL bFindUnit;
	BOOL bSelEnter=FALSE;

	assert(Line >= 0 && Line <= TotalLine-1);
	if (pLine[Line].Start == -1)
		return 0;

	if (bComp) {
		toff=off-2;
	} else {
		toff=off;
	}
	GetRCFromOff(toff,r,c);
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

	if (bColumnMode == FALSE) {
		SelFirst=min(SelStart,SelEnd);
		SelSecond=max(SelStart,SelEnd);
	} else {
		GetSelInLine(Line,SelFirst,SelSecond);
	}

	x=frt.left-xPos;
	nowoff=pLine[Line].Start;
	for (;;) {
		for (len=0;;) {
			assert(nowoff+len <= pLine[Line].End);
			if (nowoff+len == pLine[Line].End || len >= 1024) {
				if (SelStart != SelEnd && nowoff >= SelFirst && nowoff < SelSecond) {
					bInSel=TRUE;
				} else {
					bInSel=FALSE;
				}
				break;
			}
			if (pFrame->buf[nowoff+len] == '\t') {
				if (len == 0)
					len=1;
				if (SelStart != SelEnd && nowoff >= SelFirst && nowoff < SelSecond) {
					bInSel=TRUE;
				} else {
					bInSel=FALSE;
				}
				break;
			}

			if (SelStart != SelEnd && len != 0 && nowoff+len == SelFirst) {
				bInSel=FALSE;
				break;
			}
			if (SelStart != SelEnd && len != 0 && nowoff+len == SelSecond) {
				bInSel=TRUE;
				break;
			}

			bFindUnit=FALSE;
			for (nUnit=0;nUnit<Parser->pInfo[Line].UnitSize;nUnit++) {
				if (Parser->pInfo[Line].pUnit[nUnit].pos == nowoff+len) {
					bFindUnit=TRUE;
					break;
				}
				if (Parser->pInfo[Line].pUnit[nUnit].pos == -1) {
					break;
				}
			}

			if (bFindUnit) {
				if (SelStart != SelEnd && nowoff >= SelFirst && nowoff < SelSecond) {
					bInSel=TRUE;
				} else {
					bInSel=FALSE;
				}

				if (len != 0)
					break;
			}

			if (pFrame->IsDBCS(nowoff+len)) {
				len+=2;
			} else {
				len++;
			}
		}

		if (bInSel && (GetFocus()==hWnd || pFrame->HideSelType!=0 || bFindSelect)) {
			if (GetFocus()==hWnd || pFrame->HideSelType==2 || bFindSelect) {
				fore=pFrame->GetColor(AECOLOR_SELFORE);
				back=pFrame->GetColor(AECOLOR_SELBACK);
			} else {
				fore=RGB(0,0,0);
				back=RGB(192,192,192);
			}
		} else {
			if (len==0) {
				fore=-1;
				back=-1;
			} else {
				for (nUnit=0;nUnit<Parser->pInfo[Line].UnitSize;nUnit++) {
					if (Parser->pInfo[Line].pUnit[nUnit].pos >= nowoff+len || 
						Parser->pInfo[Line].pUnit[nUnit].pos == -1) {
						break;
					}
				}
				style=Parser->pInfo[Line].pUnit[nUnit-1].style;
				Parser->GetStyleColor(style,fore,back);
			}

			if (fore==(DWORD)-1) {
				fore=pFrame->GetColor(AECOLOR_FORE);
			}
			if (back==(DWORD)-1) {
				back=pFrame->GetColor(AECOLOR_BACK);
			} 
			if ((pFrame->nShowCurLine==3 || pFrame->nShowCurLine==4) && bCurLine) {
				back=pFrame->GetColor(AECOLOR_CUR);
			}
		}

		// 줄끝의 선택된 개행 코드가 있는지 조사한다.
		if (nowoff+len == pLine[Line].End) {
			if (!pFrame->TestStyle(AES_SHOWENTER) && pFrame->IsEnterCode(pLine[Line].End) && 
				pLine[Line].End >= SelFirst && pLine[Line].End < SelSecond) {
				bSelEnter=TRUE;
			}
		}

		// 선택된 개행 코드가 있으면 x갱신
		DrawSegment(hdc,x,y,nowoff,len,
			(nowoff+len==pLine[Line].End && !bSelEnter),fore,back);

		nowoff+=len;
		// 선택된 개행 코드 출력
		if (nowoff == pLine[Line].End) {
			if (bSelEnter) {
				if (pFrame->SelBlockType==0) {
					SetRect(&lrt,x,y,x+SpaceWidth,y+FontHeight);
				} else {
					SetRect(&lrt,x,0,x+SpaceWidth,LineHeight);
				}
				if (GetFocus()==hWnd || pFrame->HideSelType==2 || bFindSelect) {
					hCurLine=CreateSolidBrush(pFrame->GetColor(AECOLOR_SELBACK));
				} else {
					hCurLine=CreateSolidBrush(RGB(192,192,192));
				}
				FillRect(hdc,&lrt,hCurLine);
				DeleteObject(hCurLine);
			}
			break;
		}
	}

	DrawLineSub(hdc,Line,bCurLine);
	return 1;
}

void CApiEditText::DrawLineSub(HDC hdc,int Line,BOOL bCurLine)
{
	HBRUSH hMarBrush;
	HPEN hMarPen,OldPen;
	RECT mrt;
	TCHAR szLine[10];
	UINT OldAlign;
	HFONT OldFont;
	int OldExtra;
	int Mark;
	HBRUSH hMark, OldBrush;
	RECT lrt;
	int r,c,len;
	POINT tri[]={{1,1},{1,0},{0,0},{1,1}};
	int vy;

	vy=GetCharYPos();
	// 박스 또는 밑줄로 현재행 표시하기
	if ((pFrame->nShowCurLine==1 || pFrame->nShowCurLine==2 || 
		pFrame->nShowCurLine==4) && bCurLine) {
		if (pFrame->nShowCurLine == 1 || pFrame->nShowCurLine == 4) {
			if (pFrame->SelBlockType==0) {
				SetRect(&lrt,frt.left-1,vy,frt.right+1,vy+FontHeight);
			} else {
				SetRect(&lrt,frt.left-1,0,frt.right+1,LineHeight);
			}
		} else {
			SetRect(&lrt,frt.left-1,vy+FontHeight,frt.right+1,vy+FontHeight);
		}
		SetTextColor(hdc,RGB(0,0,0));
		SetBkColor(hdc,GetSysColor(COLOR_WINDOW));
		DrawFocusRect(hdc,&lrt);
	}

	// 자동 개행열 출력
	if (ViewType==TEXT_VIEW && ColMark != 0) {
		hMarPen=CreatePen(PS_SOLID,1,RGB(128,128,128));
		OldPen=(HPEN)SelectObject(hdc,hMarPen);
		MoveToEx(hdc,frt.left+ColMark*FontWidth-xPos,0,NULL);
		LineTo(hdc,frt.left+ColMark*FontWidth-xPos,LineHeight);
		SelectObject(hdc,OldPen);
		DeletePen(hMarPen);
	}

	// 왼쪽 안쪽 여백 그림
	if (pFrame->InnerRect.left) {
		hMarBrush=CreateSolidBrush(pFrame->GetColor(AECOLOR_INNERRECT));
		SetRect(&mrt,MarginWidth,0,frt.left,LineHeight);
		FillRect(hdc,&mrt,hMarBrush);
		DeleteObject(hMarBrush);
	}

	// 마진 출력
	if (pFrame->TestStyle(AES_SHOWMARGIN)) {
		hMarBrush=CreateSolidBrush(pFrame->GetColor(AECOLOR_MAR1));
		SetRect(&mrt,0,0,MarginWidth,LineHeight);
		FillRect(hdc,&mrt,hMarBrush);
		hMarPen=CreatePen(PS_SOLID,1,pFrame->GetColor(AECOLOR_MAR2));
		OldPen=(HPEN)SelectObject(hdc,hMarPen);
		MoveToEx(hdc,MarginWidth-1,0,NULL);
		LineTo(hdc,MarginWidth-1,LineHeight);

		SelectObject(hdc,OldPen);
		DeleteObject(hMarPen);
		DeleteObject(hMarBrush);

		// 줄 번호 출력
		if (pFrame->TestStyle(AES_SHOWLINENUM) && (ViewType==HEXA_VIEW || pLine[Line].nLine==0)) {
			if (ViewType==TEXT_VIEW) {
				wsprintf(szLine,"%d",pLine[Line].nPara+1);
			} else {
				wsprintf(szLine,"%d",Line+1);
			}
			OldAlign=SetTextAlign(hdc,TA_RIGHT);
			SetTextColor(hdc,pFrame->GetColor(AECOLOR_NUM));
			OldFont=(HFONT)SelectObject(hdc,CApiEdit::hLineNumFont);
			OldExtra=SetTextCharacterExtra(hdc,-1);

			TextOut(hdc,MarginWidth-3,(FontHeight-10)/2+vy,szLine,lstrlen(szLine));
			
			SetTextCharacterExtra(hdc,OldExtra);
			SelectObject(hdc,OldFont);
			SetTextAlign(hdc,OldAlign);
		}

		// 북마크 출력
		if (ViewType==HEXA_VIEW || pLine[Line].nLine == 0) {
			if (ViewType==TEXT_VIEW) {
				Mark=FindBookmark(pLine[Line].nPara);
			} else {
				Mark=FindBookmark(Line);
			}
			if (Mark != -1) {
				hMark=CreateSolidBrush(pFrame->GetColor(AECOLOR_MARK));
				OldBrush=(HBRUSH)SelectObject(hdc,hMark);
				RoundRect(hdc,0,vy,10,vy+FontHeight,2,2);
				DeleteObject(SelectObject(hdc,OldBrush));
				if (Mark >= 0 && Mark <= 9) {
					wsprintf(szLine,"%d",Mark);
					OldAlign=SetTextAlign(hdc,TA_LEFT);
					SetTextColor(hdc,RGB(0,0,0));
					OldFont=(HFONT)SelectObject(hdc,CApiEdit::hLineNumFont);

					TextOut(hdc,2,vy+2,szLine,lstrlen(szLine));

					SelectObject(hdc,OldFont);
					SetTextAlign(hdc,OldAlign);
				}
			}
		}

		// 마진에 현재행 표시
		if (pFrame->nShowCurLine == 5 && bCurLine) {
			OldBrush=(HBRUSH)SelectObject(hdc,GetStockObject(BLACK_BRUSH));
			r=min(FontHeight,MarginWidth);
			tri[1].y=r-1-r % 2;
			tri[2].x=r/2;
			tri[2].y=r/2;
			len=FontHeight/2-tri[2].y;
			if (len!=0 || vy!=0) {
				for (c=0;c<4;c++) {
					tri[c].y+=len+vy;
				}
			}
			Polygon(hdc,tri,4);
			SelectObject(hdc,OldBrush);
		}
	}
}

void CApiEditText::DrawSegment(HDC hdc, int &x, int y, int SegOff, int len, BOOL ignoreX,
	COLORREF fore, COLORREF back)
{
	int docx;
	int oldx;
	RECT rt;
	HBRUSH Brush;

	if (pFrame->buf[SegOff] == '\t') {
		oldx=x;
		docx=x+xPos;
		docx=DocX((PureX(docx)/TabSize+1)*TabSize);
		x=docx-xPos;
		if (back!=pFrame->GetColor(AECOLOR_BACK)) {
			if (pFrame->SelBlockType==0) {
				SetRect(&rt,oldx,y,x+1,y+FontHeight);
			} else {
				SetRect(&rt,oldx,0,x+1,LineHeight);
			}
			Brush=CreateSolidBrush(back);
			FillRect(hdc,&rt,Brush);
			DeleteObject(Brush);
		}
		if (pFrame->TestStyle(AES_SHOWTAB)) {
			DisplayTab(hdc,oldx,x,y,back);
		}
	} else {
		SetTextColor(hdc,fore);
		oldx=x;
		if (pFrame->TestStyle(AES_SHOWENTER) || ignoreX == FALSE || back!=pFrame->GetColor(AECOLOR_BACK)) {
			x+=MyGetTextExtent(pFrame->buf+SegOff,len);
		}
		if (back!=pFrame->GetColor(AECOLOR_BACK)) {
			if (pFrame->SelBlockType==0) {
				SetRect(&rt,oldx,y,x,y+FontHeight);
			} else {
				SetRect(&rt,oldx,0,x,LineHeight);
			}
			Brush=CreateSolidBrush(back);
			FillRect(hdc,&rt,Brush);
			DeleteObject(Brush);
		}
		AeTextOut(hdc,oldx,y,pFrame->buf+SegOff,len);
		if (pFrame->TestStyle(AES_SHOWSPACE)) {
			DisplaySpace(hdc,oldx,y,pFrame->buf+SegOff,len);
		}
	}
	if (pFrame->TestStyle(AES_SHOWENTER)) {
		if (pFrame->IsEnterCode(SegOff+len)) {
			if (SegOff+len >= min(SelStart,SelEnd) && SegOff+len < max(SelStart,SelEnd)) {
				DisplayEnter(hdc,x,y,back);
			} else {
				DisplayEnter(hdc,x,y,pFrame->GetColor(AECOLOR_BACK));
			}
		}
	}
}

int CApiEditText::GetOffFromXY(int x, int y)
{
	int r,len;
	TCHAR *p;
	int chWidth;
	int acwidth;

	x=max(x,frt.left);
	y=max(y,frt.top);

	r=PureY(y)/LineHeight;
	r=min(r,GetRowCount()-1);

	for (p=pFrame->buf+pLine[r].Start,acwidth=frt.left;;) {
		if (p-pFrame->buf == pLine[r].End) {
			if (IsVirtualPosable()) {
				ExtraX=(x-acwidth)/SpaceWidth;
			} else {
				ExtraX=0;
			}
			break;
		}
		assert(p < pFrame->buf+pLine[r].End);

		if (*p == '\t') {
			len=1;
			chWidth=DocX((PureX(acwidth)/TabSize+1)*TabSize-acwidth);
		} else {
			if (pFrame->IsDBCS(p-pFrame->buf)) {
				len=2;
			} else {
				len=1;
			}
			chWidth=GetCharWidth(p,len);
		}

		acwidth += chWidth;
		if (acwidth-chWidth/2 >= x) {
			ExtraX=0;
			break;
		}

		p+=len;
	}

	bLineEnd=(p-pFrame->buf==pLine[r].End && !AeIsLineEnd(p));
	return p-pFrame->buf;
}

BOOL CApiEditText::IsDelimiter(int nPos)
{
	return (strchr(Parser->GetInfo(1),pFrame->buf[nPos]) || pFrame->buf[nPos]==0);
}

int CApiEditText::GetPrevWord(int nPos)
{
	if (nPos == 0)
		return nPos;

	for (nPos--;;nPos--) {
		if (IsDelimiter(nPos)==FALSE || nPos==0) {
			break;
		}
	}

	for (;;nPos--) {
		if (IsDelimiter(nPos)==TRUE || nPos==0) {
			break;
		}
	}

	if (nPos != 0)
		nPos++;
	return nPos;
}

int CApiEditText::GetNextWord(int nPos)
{
	for (;;nPos++) {
		if (IsDelimiter(nPos)==TRUE) {
			break;
		}
	}

	for (;;nPos++) {
		if (IsDelimiter(nPos)==FALSE || pFrame->buf[nPos]==0) {
			break;
		}
	}
	return nPos;
}

void CApiEditText::ClearSelection()
{
	if (SelStart != SelEnd) {
		Invalidate(min(SelStart,SelEnd),max(SelStart,SelEnd));
		SelStart=SelEnd=0;
	}
}

void CApiEditText::ExpandSelection(int Start, int End)
{
	int OldSelEnd;

	if (SelStart==SelEnd) {
		SelStart=Start;
		SelEnd=End;
		Invalidate(min(SelStart,SelEnd),max(SelStart,SelEnd));
	} else {
		OldSelEnd=SelEnd;
		SelEnd=End;
		Invalidate(min(OldSelEnd,SelEnd),max(OldSelEnd,SelEnd));
	}
	if (bColumnMode || pFrame->SelBlockType==2) {
		Invalidate(min(SelStart,SelEnd),max(SelStart,SelEnd));
	}
}

BOOL CApiEditText::DeleteSelection()
{
	int SelFirst, SelSecond;
	int idx,Acc,linelen;

	if (SelStart != SelEnd) {
		if (!bColumnMode) {
			SelFirst=min(SelStart,SelEnd);
			SelSecond=max(SelStart,SelEnd);
			if (pFrame->TestStyle(AES_SMARTSPACE) && ViewType!=HEXA_VIEW) {
				pFrame->StartUndoGroup(24);
			}
			Delete(SelFirst,SelSecond-SelFirst);
			off=SelFirst;
			if (pFrame->TestStyle(AES_SMARTSPACE) && ViewType!=HEXA_VIEW) {
				AdjustSpace(off,FALSE);
				pFrame->EndUndoGroup();
			}
		} else {
			MakeSelTable();
			pFrame->StartUndoGroup(16);
			for (idx=0,Acc=0;SelTable[idx].Line!=-1;idx++) {
				linelen=SelTable[idx].End-SelTable[idx].Start;
				Delete(SelTable[idx].Start-Acc,linelen);
				Acc+=linelen;
			}
			pFrame->EndUndoGroup();
			ExtraX=0;
			off=SelTable[0].Start;
			FreeSelTable();
		}
		SelStart=SelEnd=0;
		return TRUE;
	}
	return FALSE;
}

void CApiEditText::PrepareCharWidth(HDC hdc)
{
	int i;
	TCHAR Char[3]={0,};
	SIZE sz;
	int hi,low;
	BOOL Const=FALSE;

	for (i=0;i<=255;i++) {
		Char[0]=i;
		GetTextExtentPoint32(hdc, Char, 1, &sz);
		arChWidth[i]=(BYTE)sz.cx;
	}
	SpaceWidth=arChWidth[' ']*pFrame->CharExtra*pFrame->WordExtra/100/100;

	if (arHanWidth) {
		free(arHanWidth);
	}
	arHanWidth=(BYTE *)malloc(0x8000);
	for (i=0;i<0x8000;i++) {
		arHanWidth[i]=0xff;
	}

	for (hi=0xb000;hi<=0xc800;hi+=0x100) {
		for (low=0xa1;low<=0xfe;low++) {
			Char[0]=hi >> 8;
			Char[1]=low;
			if (Const==TRUE) {
				sz.cx=arHanWidth[0xb0a1-0x8000];
			} else {
				GetTextExtentPoint32(hdc,Char,2,&sz);
			}
			arHanWidth[hi+low-0x8000]=(BYTE)sz.cx;
			if (hi==0xb000 && low==0xfe) {
				for (i=0xb0a1,Const=TRUE;i<=0xb0fe;i++) {
					if (arHanWidth[i-0x8000] != arHanWidth[0xb0a1-0x8000]) {
						Const=FALSE;
					}
				}
			}
		}
	}
}

int CApiEditText::MyGetTextExtent(TCHAR *text, int len)
{
	TCHAR *p=text;
	int acwidth;

	for (acwidth=0;p-text!=len;) {
		if (pFrame->IsDBCS(p-pFrame->buf)) {
			acwidth+=GetCharWidth(p,2);
			p+=2;
		} else {
			acwidth+=GetCharWidth(p,1);
			p++;
		}
	}
	return acwidth;
}

void CApiEditText::UpdateLineInfo(int nPos/*=-1*/, int nCount/*=-1*/)
{
	int l,s,e;
	int nPara, nLine=0;
	int i;
	int ParaStart;
	int ps=-1,pe=-1,pp=0;
	int dPara;

	if (nPos==-1) {
		l=0;
		nPara=0;
	} else {
		ParaStart=pFrame->FindParaStart(nPos);
		if (ParaStart == 0) {
			l=0;
			nPara=0;
		} else {
			GetRCFromOff(ParaStart-2,l,e);
			nPara=pLine[l].nPara+1;
			l++;
		}
	}
	Parser->DeleteParseInfo(l);

	for (;;l++) {
		if (l >= Linelen) {
			Linelen += 1000;
			pLine=(tagLine *)realloc(pLine,sizeof(tagLine)*Linelen);
			if (pLine == NULL) {
			}
			for (i=Linelen-1000;i<Linelen;i++) {
				pLine[i].Start=-1;
			}
		}

		GetLine(l,s,e);
		if (nPos != -1 && e > nPos && pLine[l].Start != -1) {
			if (pLine[l].Start+nCount == s && pLine[l].End+nCount == e) {
				dPara=nPara-pLine[l].nPara;
				for (i=l;;i++) {
					if (pLine[i].Start==-1)
						break;
					pLine[i].Start+=nCount;
					pLine[i].End+=nCount;
					pLine[i].nPara+=dPara;
				}
				break;
			}

			if (ps+nCount == s && pe+nCount == e) {
				if (TotalLine+1 >= Linelen) {
					Linelen += 1000;
					pLine=(tagLine *)realloc(pLine,sizeof(tagLine)*Linelen);
					if (pLine == NULL) {
					}
					for (i=Linelen-1000;i<Linelen;i++) {
						pLine[i].Start=-1;
					}
				}
				dPara=nPara-pp;
				pLine[TotalLine+1].Start=-1;
				for (i=TotalLine;i>=l;i--) {
					pLine[i].Start=pLine[i-1].Start+nCount;
					pLine[i].End=pLine[i-1].End+nCount;
					pLine[i].nPara=pLine[i-1].nPara+dPara;
					pLine[i].nLine=pLine[i-1].nLine;
				}
				pLine[l].Start=s;
				pLine[l].End=e;
				pLine[l].nPara=nPara;
				pLine[l].nLine=nLine;
				TotalLine++;
				break;
			}
			
			if (pLine[l+1].Start+nCount == s && pLine[l+1].End+nCount == e) {
				dPara=nPara-pLine[l+1].nPara;
				for (i=l;;i++) {
					if (pLine[i+1].Start==-1) {
						pLine[i].Start=-1;
						break;
					}
					pLine[i].Start=pLine[i+1].Start+nCount;
					pLine[i].End=pLine[i+1].End+nCount;
					pLine[i].nPara=pLine[i+1].nPara+dPara;
					pLine[i].nLine=pLine[i+1].nLine;
				}
				TotalLine--;
				break;
			}
		}

		ps=pLine[l].Start;
		pe=pLine[l].End;
		pp=pLine[l].nPara;

		pLine[l].Start=s;
		pLine[l].End=e;
		pLine[l].nPara=nPara;
		pLine[l].nLine=nLine;
		if (s==-1) {
			TotalLine=l;
			if (pFrame->TestStyle(AES_SHOWMARGIN)) {
				if (nPara >= 9999 && MarginWidth==25) {
					MarginWidth=32;
					PostMessage(hWnd,WM_SIZE,0,0);
					Invalidate(-1);
				}
				if (nPara < 9999 && MarginWidth==32) {
					MarginWidth=25;
					PostMessage(hWnd,WM_SIZE,0,0);
					Invalidate(-1);
				}
			}
			break;
		}

		if (pFrame->IsEnterCode(e)) {
			nPara++;
			nLine=0;
		} else {
			nLine++;
		}
	}
}

void CApiEditText::Invalidate(int Pos1, int Pos2/*=-1*/)
{
	RECT rt;
	int x,y,y1,y2;

	if (Pos1 == -1) {
		InvalidateRect(hWnd,NULL,FALSE);
		return;
	}

	GetXYFromOff(Pos1,x,y);
	y1=y-yPos;

	if (Pos2 == -1) {
		y2=frt.bottom;
	} else {
		GetXYFromOff(Pos2,x,y);
		y2=y-yPos+LineHeight;
	}

	SetRect(&rt,0,y1,frt.right,y2);
	InvalidateRect(hWnd,&rt,FALSE);
}

void CApiEditText::ClearBookmark()
{
	int i;
	for (i=0;i<MAXBOOKMARK;i++) {
		arMark[i]=-1;
	}
	Invalidate(-1);
}

void CApiEditText::ToggleBookmark(int Para, int Mark)
{
	int r,c;
	int i;
	int state;
	TCHAR szCom[256];

	if (Para == -1) {
		GetRCFromOff(off,r,c);
		if (ViewType==HEXA_VIEW) {
			Para=r;
		} else {
			Para=pLine[r].nPara;
		}
	}
	
	if (Mark <= 9) {
		wsprintf(szCom,"setmark %d",Mark);
		AeMc.RecordCommand(szCom);
		if (arMark[Mark] == -1) {
			arMark[Mark]=Para;
		} else {
			if (arMark[Mark] == Para) {
				arMark[Mark]=-1;
			} else {
				arMark[Mark]=Para;
			}
		}
	} else {
		AeMc.RecordCommand("setmark unname");
		state=FindBookmark(Para);
		if (state == -1) {
			for (i=10;i<MAXBOOKMARK && arMark[i]!=-1;i++);
			if (i==MAXBOOKMARK) {
				return;
			}
			arMark[i]=Para;
		} else {
			for (i=10;i<MAXBOOKMARK && arMark[i]!=Para;i++);
			arMark[i]=-1;
		}
	}

	Invalidate(-1);
}

void CApiEditText::GotoBookmark(int Mark)
{
	int Para;
	int i;
	int r,c;
	int nowPara;
	int Min=1000000,Max=-1,Next=1000000,Prev=-1;
	TCHAR szCom[256];
	int OldyPos;

	if (Mark<=9) {
		wsprintf(szCom,"gomark %d",Mark);
		AeMc.RecordCommand(szCom);
		Para=arMark[Mark];
		if (Para == -1) {
			return;
		}
	} else {
		GetRCFromOff(off,r,c);
		if (ViewType==HEXA_VIEW) {
			nowPara=r;
		} else {
			nowPara=pLine[r].nPara;
		}
		for (i=10;i<MAXBOOKMARK;i++) {
			if (arMark[i] == -1)
				continue;
			Min=min(arMark[i],Min);
			Max=max(arMark[i],Max);
			if (arMark[i] > nowPara) {
				Next=min(arMark[i],Next);
			}
			if (arMark[i] < nowPara) {
				Prev=max(arMark[i],Prev);
			}
		}

		if (Mark == 10) {
			AeMc.RecordCommand("gomark next");
			if (Next == 1000000) {
				Para=Min;
				if (Para == 1000000) {
					return;
				}
			} else {
				Para=Next;
			}
		} else {
			AeMc.RecordCommand("gomark prev");
			if (Prev == -1) {
				Para=Max;
				if (Para == -1) {
					return;
				}
			} else {
				Para=Prev;
			}
		}
	}

	OldyPos=yPos;
	GotoLine(Para,0,(ViewType==HEXA_VIEW) ? 0:1);
	if (OldyPos!=yPos) {
		ArrangeVerticalCenter(off);
	}
}

void CApiEditText::GotoLine(int Line, int Col/*=0*/, int Type/*=0*/)
{
	int tLine;
	int toff;
	ClearSelection();

	switch (Type) {
	case 0:
		if (ViewType==TEXT_VIEW) {
			tLine=min(Line,TotalLine-1);
		} else {
			tLine=min(Line,GetRowCount()-1);
		}
		off=GetOffFromRC(Line,Col);
		break;
	case 1:
		toff=GetOffFromPara(Line,Col);
		if (toff!=-1) {
			off=toff;
		}
		break;
	}
	SetCaret();
}

int CApiEditText::FindBookmark(int Para)
{
	int i;

	for (i=0;i<MAXBOOKMARK;i++) {
		if (arMark[i] == Para) {
			return (i <= 9) ? i:100;
		}
	}
	return -1;
}

void CApiEditText::DisplayTab(HDC hdc, int x1, int x2, int y,COLORREF back)
{
	static BYTE arTab[]={0,0,1,0,2,0,1,1,1,2,1,3,1,4,
		4,0,3,1,3,2,3,3,3,4,5,1,5,2,5,3,5,4,4,2,
		6,0,6,1,6,2,6,3,6,4,7,0,7,2,7,4,8,1,8,3};
	HPEN hPen,OldPen;
	int i;

	hPen=CreatePen(PS_SOLID,1,pFrame->GetColor(AECOLOR_CODE));
	OldPen=(HPEN)SelectObject(hdc,hPen);

	switch (pFrame->GetShowTabType()) {
	case 0:
		MoveToEx(hdc,x1+1,y+FontHeight/2,NULL);
		LineTo(hdc,x2-1,y+FontHeight/2);
		LineTo(hdc,x2-3,y+FontHeight/2-2);
		LineTo(hdc,x2-3,y+FontHeight/2+2);
		LineTo(hdc,x2-1,y+FontHeight/2);
		break;
	case 1:
		MoveToEx(hdc,x1+1,y+FontHeight*3/4,NULL);
		LineTo(hdc,x2-1,y+FontHeight*3/4);
		LineTo(hdc,x2-3,y+FontHeight*3/4-2);
		LineTo(hdc,x2-3,y+FontHeight*3/4+2);
		LineTo(hdc,x2-1,y+FontHeight*3/4);
		for (i=0;i<sizeof(arTab)/sizeof(arTab[0]);i+=2) {
			SetPixel(hdc,arTab[i]+x1+2,y+arTab[i+1]+FontHeight*3/4-6,pFrame->GetColor(AECOLOR_CODE));
		}
		break;
	}

	SelectObject(hdc,OldPen);
	DeleteObject(hPen);
}

void CApiEditText::DisplayEnter(HDC hdc, int x, int y,COLORREF back)
{
	HPEN hPen,OldPen;
	HBRUSH OldBrush;
	RECT rt,brt;
	int width;
	HBRUSH Brush;

	hPen=CreatePen(PS_SOLID,1,pFrame->GetColor(AECOLOR_CODE));
	OldPen=(HPEN)SelectObject(hdc,hPen);

	switch (pFrame->GetShowEnterType()) {
	case 0:
		width=SpaceWidth*2;
		break;
	case 1:
		width=SpaceWidth*3;
		break;
	case 2:
		width=SpaceWidth*3;
		break;
	}

	SetRect(&rt,x,y,x+width,y+FontHeight);
	if (back!=pFrame->GetColor(AECOLOR_BACK)) {
		if (pFrame->SelBlockType==0) {
			brt=rt;
		} else {
			SetRect(&brt,x,0,x+width,LineHeight);
		}
		Brush=CreateSolidBrush(back);
		FillRect(hdc,&brt,Brush);
		DeleteObject(Brush);
	}

	if (pFrame->GetShowEnterType() <= 1) {
		MoveToEx(hdc,x+width-2,y+FontHeight/10,NULL);
		LineTo(hdc,x+width-2,y+FontHeight*3/4);
		LineTo(hdc,x+2,y+FontHeight*3/4);
		LineTo(hdc,x+4,y+FontHeight*3/4-2);
		LineTo(hdc,x+4,y+FontHeight*3/4+2);
		LineTo(hdc,x+2,y+FontHeight*3/4);
	} else {
		OldBrush=(HBRUSH)SelectObject(hdc,GetStockObject(NULL_BRUSH));
		Rectangle(hdc,rt.left+1,rt.top+1,rt.right-1,rt.bottom-1);
		SelectObject(hdc,OldBrush);
		MoveToEx(hdc,x+width-4,y+FontHeight/5,NULL);
		LineTo(hdc,x+width-4,y+FontHeight*5/8);
		LineTo(hdc,x+3,y+FontHeight*5/8);
		LineTo(hdc,x+5,y+FontHeight*5/8-2);
		LineTo(hdc,x+5,y+FontHeight*5/8+2);
		LineTo(hdc,x+3,y+FontHeight*5/8);
	}
	SelectObject(hdc,OldPen);
	DeleteObject(hPen);
}

void CApiEditText::DisplaySpace(HDC hdc, int x, int y, TCHAR *text, int len)
{
	int nPos;
	int nx=x;
	int width;
	HPEN hPen,OldPen;
	HBRUSH OldBrush;

	width=SpaceWidth;
	if (pFrame->GetShowSpaceType()!=0) {
		hPen=CreatePen(PS_SOLID,1,pFrame->GetColor(AECOLOR_CODE));
		OldPen=(HPEN)SelectObject(hdc,hPen);
		OldBrush=(HBRUSH)SelectObject(hdc,GetStockObject(NULL_BRUSH));
	}
	for (nPos=0;nPos!=len;nPos++) {
		if (text[nPos] == 0x20) {
			switch (pFrame->GetShowSpaceType()) {
			default:
			case 0:
				SetPixel(hdc,nx+width/2,y+FontHeight/2,pFrame->GetColor(AECOLOR_CODE));
				SetPixel(hdc,nx+width/2,y+FontHeight/2+1,pFrame->GetColor(AECOLOR_CODE));
				break;
			case 1:
				MoveToEx(hdc,nx,y+FontHeight*3/4+1,NULL);
				LineTo(hdc,nx,y+FontHeight-1);
				LineTo(hdc,nx+width-1,y+FontHeight-1);
				LineTo(hdc,nx+width-1,y+FontHeight*3/4);
				break;
			case 2:
				Rectangle(hdc,nx,0,nx+width,y+FontHeight);
				break;
			}
			nx+=width;
		} else {
			if (pFrame->IsDBCS(text-pFrame->buf+nPos)) {
				nx += GetCharWidth(text+nPos,2);
				nPos++;
			} else {
				nx += GetCharWidth(text+nPos,1);
			}
		}
	}
	if (pFrame->GetShowSpaceType()!=0) {
		SelectObject(hdc,OldPen);
		DeleteObject(hPen);
		SelectObject(hdc,OldBrush);
	}
}

void CApiEditText::GetNowWord(int nPos, int &s, int &e)
{
	for (;;nPos--) {
		if (IsDelimiter(nPos)==TRUE || nPos==0) {
			break;
		}
	}

	if ((nPos != 0 || IsDelimiter(nPos)) && 
		nPos != pFrame->doclen && IsDelimiter(nPos+1)==FALSE) {
		nPos++;
	}
	s=nPos;

	for (;;nPos++) {
		if (IsDelimiter(nPos)==TRUE) {
			break;
		}
	}
	e=nPos;
}

BOOL CApiEditText::IsInSelection(int x,int y)
{
	int toff;
	int r,c;
	int ss,se;
	int SelFirst, SelSecond;
	int x1,y1,x2,y2;
	RECT rt;
	POINT pt;
	BOOL bRet;
	int BkExtraX;

	if (SelStart == SelEnd) {
		return FALSE;
	}

	SelFirst=min(SelStart,SelEnd);
	SelSecond=max(SelStart,SelEnd);

	BkExtraX=ExtraX;
	ignoreVirt++;
	toff=GetOffFromXY(x,y);
	GetRCFromOff(toff,r,c);

	if (bColumnMode) {
		GetSelInLine(r,ss,se);
	} else {
		if (SelFirst > pLine[r].End || SelSecond <= pLine[r].Start) {
			bRet=FALSE;
			goto End;
		}

		if (SelFirst < pLine[r].Start) {
			ss=pLine[r].Start;
		} else {
			ss=SelFirst;
		}

		if (SelSecond > pLine[r].End) {
			se=pLine[r].End;
		} else {
			se=SelSecond;
		}
	}

	se=pFrame->GetPrevOff(se);

	GetXYFromOff(ss,x1,y1);
	GetXYFromOff(se,x2,y2);
	if (pFrame->IsDBCS(se)) {
		x2 += GetCharWidth(pFrame->buf+se,2);
	} else {
		x2 += GetCharWidth(pFrame->buf+se,1);
	}

	SetRect(&rt,x1,y1,x2,y1+LineHeight);
	pt.x=x;
	pt.y=y;
	bRet=PtInRect(&rt,pt);

End:
	ignoreVirt--;
	ExtraX=BkExtraX;
	return bRet;
}

void CApiEditText::CopyString(BOOL bCopy, int from, int &to, int len, BOOL bRec/*=TRUE*/)
{
	TCHAR *t;
	int orito=to;
	int BkExtraX;
	int ss,se;
	int nIns;

	BkExtraX=ExtraX;
	if (InputExtraSpace(to)) {
		if (from > to-BkExtraX) {
			from+=BkExtraX;
		}
	}

	t=(TCHAR *)malloc(len);
	memcpy(t,pFrame->buf+from,len);
	// 스마트 공백이고 기록 모드일 때만. 즉, 취소, 재실행시는 스마트 공백 동작 안함
	if (bRec==TRUE && pFrame->TestStyle(AES_SMARTSPACE) && ViewType!=HEXA_VIEW) {
		if (bCopy) {
			pFrame->StartUndoGroup(26);
		} else {
			pFrame->StartUndoGroup(27);
		}
		Insert(to,t,len,TRUE);
		se=to+len;
		nIns=AdjustSpaceInsert(to,se);

		if (bCopy==FALSE) {
			if (to > from) {
				Delete(from,len,TRUE);
				nIns=AdjustSpace(from,FALSE);
				to=to-len+nIns;
			} else {
				from+=nIns;
				Delete(from+len,len,TRUE);
				ss=from+len;
				AdjustSpace(ss,FALSE);
			}
		}
		pFrame->EndUndoGroup();
	} else {
		Insert(to,t,len,FALSE);

		if (bCopy==FALSE) {
			if (to > from) {
				Delete(from,len,FALSE);
				to-=len;
			} else {
				Delete(from+len,len,FALSE);
			}
		}
		if (bRec) {
			if (bCopy) {
				pFrame->URInsert(to,t,len);
			} else {
				pFrame->URMove(from,orito,t,len);
			}
		}
	}
	free(t); 
}

void CApiEditText::GetSelect(int &s, int &e,BOOL bRegular/*=TRUE*/)
{
	if (bRegular) {
		s=min(SelStart,SelEnd);
		e=max(SelStart,SelEnd);
	} else {
		s=SelStart;
		e=SelEnd;
	}
}

void CApiEditText::SetSelect(int s, int e, BOOL bFind)
{
	if (s <= pFrame->doclen && e <= pFrame->doclen) {
		SelStart=s;
		if (e == -1)
			e=pFrame->doclen;
		SelEnd=e;
	}
	off=SelEnd;
	SetCaret();
	if (bFind) {
		bFindSelect=TRUE;
	}
	Invalidate(-1);
}

void CApiEditText::SetLineRatio(int aLineHeight)
{
	int FirstRow;

	FirstRow=yPos/LineHeight;
	LineRatio=aLineHeight;
	LineHeight=int(FontHeight*LineRatio/100);
	yPos=FirstRow*LineHeight;
	if (hBit) {
		DeleteObject(hBit);
		hBit=NULL;
	}
	UpdateScrollInfo();
	Invalidate(-1);
	SetCaret();
}

void CApiEditText::SetTabWidth(int aTabWidth)
{
	TabWidth=aTabWidth;
	if (pFrame->TestStyle(AES_CALCTABWITHAVG)) {
		TabSize=FontWidth*TabWidth;
	} else {
		TabSize=SpaceWidth*TabWidth;
	}
	UpdateLineInfo();
	UpdateScrollInfo();
	Invalidate(-1);
	SetCaret();
}

void CApiEditText::GetFont(LOGFONT *alogfont)
{
	*alogfont=logfont;
}

void CApiEditText::SetFont(LOGFONT *alogfont)
{
	HDC hdc;
	TEXTMETRIC tm;
	HFONT hOldFont=NULL;

	if (hMainFont) {
		DeleteObject(hMainFont);
		hMainFont=NULL;
	}

	logfont=*alogfont;
	hdc=GetDC(hWnd);
	if (logfont.lfHeight != 0) {
		if (pFrame->CharRatio != 100) {
			logfont.lfWidth=logfont.lfHeight*pFrame->CharRatio/200;
		} else {
			logfont.lfWidth=0;
		}
		hMainFont=CreateFontIndirect(&logfont);
		hOldFont=(HFONT)SelectObject(hdc,hMainFont);
	}
	PrepareCharWidth(hdc);
	GetTextMetrics(hdc,&tm);
	FontHeight=tm.tmHeight;
	LineHeight=int(FontHeight*LineRatio/100);
	yPos=yPos-(yPos % LineHeight);
	FontWidth=tm.tmAveCharWidth;
	if (pFrame->TestStyle(AES_CALCTABWITHAVG)) {
		TabSize=FontWidth*TabWidth;
	} else {
		TabSize=SpaceWidth*TabWidth;
	}
	if (hOldFont) {
		SelectObject(hdc,hOldFont);
	}
	ReleaseDC(hWnd,hdc);

	if (hBit) {
		DeleteObject(hBit);
		hBit=NULL;
	}

	if (pLine) {
		UpdateLineInfo();
		UpdateScrollInfo();
		SetCaret();
		Invalidate(-1);
	}
}

void CApiEditText::SetDefaultSetting()
{
	if (pLine) {
		SetLineRatio(120);
		SetWrap(2);
		SetTabWidth(4);
	} else {
		LineRatio=120;
		nWrap=2;
		EnableScrollBar(hHorzScroll,SB_CTL,ESB_DISABLE_BOTH);
		TabWidth=4;
	}
	MarginWidth=25;
	logfont.lfHeight=0;
	hMainFont=NULL;
	SetFont(&logfont);
	RightWrap=0;
	ColMark=0;
	SetParser(0);
	bVirtualPos=FALSE;
	bColumnMode=FALSE;
}

void CApiEditText::SetRightWrap(int aRightWrap)
{
	RightWrap=aRightWrap;
	SetWrap(nWrap);
}

void CApiEditText::SetColMark(int aColMark)
{
	ColMark=aColMark;
	Invalidate(-1);
}

void CApiEditText::SetParser(int ParseType)
{
	if (Parser != NULL) {
		delete Parser;
	}

	switch(ParseType) {
	case -1:
		break;
	case 0:
		Parser=new CParseNull;
		break;
	case 1:
		Parser=new CParseCpp;
		break;
	case 2:
		Parser=new CParseHtml;
		break;
	case 3:
		Parser=new CParseSql;
		break;
	case 4:
		Parser=new CParseJava;
		break;
	case 5:
		Parser=new CParseCS;
		break;
	}

	Invalidate(-1);
}

BOOL CApiEditText::FindText(int nPos, TCHAR *what, DWORD dwFlag)
{
	TCHAR szCom[256];
	int s,e;
	int OldyPos;

	wsprintf(szCom,"find \"%s\"",what);
	if ((dwFlag & AE_FIND_WRAP)==0)
		lstrcat(szCom," nowrap");
	if (dwFlag & AE_FIND_UP)
		lstrcat(szCom," up");
	if (dwFlag & AE_FIND_WHOLEWORD)
		lstrcat(szCom," word");
	if (dwFlag & AE_FIND_MATCHCASE)
		lstrcat(szCom," case");
	AeMc.RecordCommand(szCom);

	if (FindTextSub(nPos,what,dwFlag,s,e)) {
		OldyPos=yPos;
		SetSelect(s,e,TRUE);
		if (OldyPos!=yPos) {	
			ArrangeVerticalCenter(SelStart);
		}
		return TRUE;
	}
	return FALSE;
}

int CApiEditText::ReplaceText(int nPos, TCHAR *what, DWORD dwFlag, TCHAR *to)
{
	int len,tolen;
	TCHAR *pSel;
	TCHAR *pWhat,*pTo;
	int Result;
	TCHAR szDesc[1024];
	TCHAR szCom[256];
	TCHAR *p;

	// 매크로 기록
	wsprintf(szCom,"replace \"%s\" \"%s\"",what,to);
	if ((dwFlag & AE_FIND_WRAP)==0)
		lstrcat(szCom," nowrap");
	if (dwFlag & AE_FIND_UP)
		lstrcat(szCom," up");
	if (dwFlag & AE_FIND_WHOLEWORD)
		lstrcat(szCom," word");
	if (dwFlag & AE_FIND_MATCHCASE)
		lstrcat(szCom," case");
	AeMc.RecordCommand(szCom);

	// 검색되지 않은 상태에서 바꾸기이면 검색부터 한다.
	if (SelStart == SelEnd) {
		if (FindText(nPos,what,dwFlag) == TRUE) {
			Result=1;
		} else {
			Result=0;
		}
	} else {
		// 확장열이면 먼저 변환한다.
		pWhat=(TCHAR *)malloc(lstrlen(what)+1);
		if (strchr(what,'\\')==NULL) {
			lstrcpy(pWhat,what);
			len=lstrlen(pWhat);
		} else {
			len=AeEscapeString(what,pWhat);
		}
		pTo=(TCHAR *)malloc(lstrlen(to)+1);
		if (strchr(to,'\\')==NULL) {
			lstrcpy(pTo,to);
			tolen=lstrlen(pTo);
		} else {
			tolen=AeEscapeString(to,pTo);
		}

		// 선택 영역의 텍스트를 읽는다.
		pSel=(TCHAR *)malloc(len);
		memcpy(pSel,pFrame->buf+min(SelStart,SelEnd),len);

		// 대소 구분없이 비교한다면 모두 대문자로 변경한다.
		if ((dwFlag & AE_FIND_MATCHCASE) == 0) {
			for (p=pWhat;p-pWhat<len;p++) *p=toupper(*p);
			for (p=pSel;p-pSel<len;p++) *p=toupper(*p);
		}

		// 선택영역이 검색할 내용이 아니면 검색 명령을 대신 실행한다.
		if (memcmp(pWhat,pSel,len) != 0) {
			if (FindText(nPos,what,dwFlag) == TRUE) {
				Result=1;
			} else {
				Result=0;
			}
		} else {
			// 선택영역을 지우고 대체 문자열을 삽입한다.
			wsprintf(szDesc," (%s -> %s)",what,to);
			pFrame->StartUndoGroup(11,szDesc);
			DeleteSelection();
			Insert(off,pTo,tolen);
			off+=tolen;
			SetCaret();
			Invalidate(-1);
			// 다음 검색 결과가 없으면 0을 리턴하여 바꾸기를 중지한다.
			if (FindText(off,what,dwFlag) == TRUE) {
				Result=2;
			} else {
				Result=0;
			}
			pFrame->EndUndoGroup();
		}
		free(pWhat);
		free(pTo);
		free(pSel);
	}
	return Result;
}

void CApiEditText::GetSelRect(RECT &srt)
{
	int x1,y1,x2,y2;

	if (SelStart==SelEnd) {
		srt.left=-1;
		return;
	}

	GetXYFromOff(min(SelStart,SelEnd),x1,y1);
	GetXYFromOff(max(SelStart,SelEnd),x2,y2);

	x1-=xPos;
	y1-=yPos;
	x2-=xPos;
	y2-=yPos;

	if (y1==y2) {
		srt.left=x1;
		srt.top=y1;
		srt.right=x2;
		srt.bottom=y1+FontHeight;
	} else {
		srt.left=frt.left;
		srt.top=y1;
		srt.right=frt.right;
		srt.bottom=y2+FontHeight;
	}
}

int CApiEditText::GetInfoFromOff(int Type, int nPos/*=-1*/)
{
	int r,c;
	int ret;

	if (nPos==-1) {
		nPos=off;
	}

	GetRCFromOff(nPos,r,c);

	switch (Type) {
	case AE_INFO_ROW:
		ret=r;
		break;
	case AE_INFO_COL:
		ret=c;
		break;
	case AE_INFO_PARA:
		ret=pLine[r].nPara;
		break;
	case AE_INFO_LINE:
		ret=pLine[r].nLine;
		break;
	case AE_INFO_PARACOL:
		while (pLine[r].nLine != 0) r--;
		return nPos-pLine[r].Start;
	}

	return ret;
}

void CApiEditText::FindParen(BOOL bSelect)
{
	TCHAR ch,ch2;
	BOOL bFind;
	int Inc;
	int toff;
	int level;
	int i;
	int OldOff;

	bFind=FALSE;
	for (i=0;i>=-1;i--) {
		if (i==-1 && off==0)
			break;
		ch=pFrame->buf[off+i];
		if (ch == '(' || ch == '{' || ch == '[' || ch == '<') {
			Inc=1;
			bFind=TRUE;
			break;
		}
		if (ch == ')' || ch == '}' || ch == ']' || ch == '>') {
			Inc=-1;
			bFind=TRUE;
			break;
		}
	}

	if (bFind==FALSE) {
		return;
	}
	OldOff=off+i;

	switch (ch) {
	case '(': ch2=')'; break;
	case '{': ch2='}'; break;
	case '[': ch2=']'; break;
	case '<': ch2='>'; break;
	case ')': ch2='('; break;
	case '}': ch2='{'; break;
	case ']': ch2='['; break;
	case '>': ch2='<'; break;
	}

	level=0;
	for (toff=OldOff;;) {
		toff+=Inc;

		if (toff == pFrame->doclen || toff == -1) {
			bFind=FALSE;
			break;
		}

		if (pFrame->buf[toff]==ch2) {
			if (level == 0) {
				bFind=TRUE;
				break;
			}
			level--;
		}
		if (pFrame->buf[toff]==ch)
			level++;
	}

	if (bFind) {
		off=toff;
		if (bSelect) {
			if (Inc==1) {
				off++;
			} else {
				OldOff++;
			}
			ExpandSelection(OldOff,off);
		} else {
			ClearSelection();
		}
		SetCaret();
	}
}

void CApiEditText::GotoLastEditOff()
{
	AeMc.RecordCommand("move last");
	Move(AECM_LAST,FALSE);
}

int CApiEditText::GetDocInfo(int type)
{
	int i;
	int count=0;
	int han=0;
	TCHAR *p=pFrame->buf;

	switch (type) {
	case AE_DOC_TOTALPARA:
		return pLine[TotalLine-1].nPara;
	case AE_DOC_WORD:
		for (;!AeIsDocEnd(p);) {
			if (AeIsWhiteSpace(*p) || pFrame->IsEnterCode(p)) {
				while ((AeIsWhiteSpace(*p) || *p=='\r' || *p=='\n') && !AeIsDocEnd(p))
					p++;
			} else {
				count++;
				while (!AeIsWhiteSpace(*p) && *p!='\r' && *p!='\n' && !AeIsDocEnd(p)) 
					p++;
			}
		}
		break;
	case AE_DOC_EXSPACESIZE:
		for (;!AeIsDocEnd(p);p++) {
			if (!(AeIsWhiteSpace(*p) || *p=='\r' || *p=='\n')) {
				count++;
			}
		}
		break;
	case AE_DOC_CHAR:
	case AE_DOC_HAN:
		for (;!AeIsDocEnd(p);p++) {
			if (!(AeIsWhiteSpace(*p) || *p=='\r' || *p=='\n')) {
				if (pFrame->IsDBCS(p-pFrame->buf)) {
					han++;
					p++;
				}
				count++;
			}
		}
		if (type == AE_DOC_HAN)
			return han;
		break;
	case AE_DOC_EMPTYLINE:
		for (i=0;i<=pLine[TotalLine-1].nPara;i++) {
			if (IsParaEmpty(pFrame->buf+GetOffFromPara(i,0))) {
				count++;
			}
		}
		break;
	}
	return count;
}

void CApiEditText::ChangeCase(int action)
{
	int SelFirst, SelSecond;
	int len;
	TCHAR *tbuf;
	TCHAR *p;
	int toff;

	if (pFrame->bReadOnly) {
		return;
	}

	if (bColumnMode && SelStart != SelEnd) {
		if (action < 3) {
			ChangeCaseColumn(action);
		}
		return;
	}

	if (SelStart == SelEnd) {
		switch (action) {
		case 0:
		case 1:
		case 2:
			if (AeIsDocEnd(pFrame->buf+off)) {
				return;
			}
			SelFirst=SelSecond=off;
			if (pFrame->IsDBCS(off)) {
				len=2;
			} else {
				len=1;
			}
			break;
		case 3:
			GetNowWord(off,SelFirst,SelSecond);
			len=SelSecond-SelFirst;
			break;
		case 4:
			return;
		}
	} else {
		SelFirst=min(SelStart,SelEnd);
		SelSecond=max(SelStart,SelEnd);
		len=SelSecond-SelFirst;
	}

	tbuf=(TCHAR *)malloc(len);
	memcpy(tbuf,pFrame->buf+SelFirst,len);

	switch (action) {
	case 0:
		AeMc.RecordCommand("convert upper");
		p=tbuf;
		while (p-tbuf < len) {
			if (IsCharLower(*p)) {
				*p-=0x20;
			}
			p++;
		}
		break;
	case 1:
		AeMc.RecordCommand("convert lower");
		p=tbuf;
		while (p-tbuf < len) {
			if (IsCharUpper(*p)) {
				*p+=0x20;
			}
			p++;
		}
		break;
	case 2:
		AeMc.RecordCommand("convert reversecase");
		p=tbuf;
		while (p-tbuf < len) {
			if (IsCharUpper(*p)) {
				*p+=0x20;
			} else if (IsCharLower(*p)) {
				*p-=0x20;
			}
			p++;
		}
		break;
	case 3:
		AeMc.RecordCommand("convert upperfirst");
		p=tbuf;
		if (IsCharLower(*p)) {
			*p-=0x20;
		}
		p++;
		while (p-tbuf < len) {
			if (IsDelimiter(SelFirst+(p-tbuf)-1)) {
				if (IsCharLower(*p)) {
					*p-=0x20;
				}
			} else {
				if (IsCharUpper(*p)) {
					*p+=0x20;
				}
			}
			p++;
		}
		break;
	case 4:
		AeMc.RecordCommand("convert uppersend");
		p=tbuf;

		toff=SelFirst;
		while (AeIsWhiteSpace(pFrame->buf[toff]) && toff > 0)
			toff--;
		if (toff==0 || pFrame->IsEnterCode(toff-1) || pFrame->buf[toff]=='.') {
			if (IsCharLower(*p)) {
				*p-=0x20;
			}
		} else {
			if (IsCharUpper(*p)) {
				*p+=0x20;
			}
		}
		p++;

		while (p-tbuf < len) {
			if (*p=='.' || pFrame->IsEnterCode(tbuf,len,p-1)) {
				p++;
				while (AeIsWhiteSpace(*p) && p-tbuf < len)
					p++;
				if (p-tbuf == len)
					break;
				if (IsCharLower(*p)) {
					*p-=0x20;
				}
			} else {
				if (IsCharUpper(*p)) {
					*p+=0x20;
				}
			}
			p++;
		}
		break;
	}

	pFrame->StartUndoGroup(3);
	Delete(SelFirst,len);
	Insert(SelFirst,tbuf,len);
	pFrame->EndUndoGroup();

	if (SelStart==SelEnd && action < 3) {
		off=pFrame->GetNextOff(off);
	}
	free(tbuf);
	Invalidate(SelFirst);
	SetCaret();
}

void CApiEditText::ConvertSpaceTab(BOOL bToSpace)
{
	int SelFirst, SelSecond;
	TCHAR *src,*dest;
	TCHAR *s,*d;
	int len;
	int nDiff;
	int i;
	BOOL bFind;

	if (pFrame->bReadOnly || ViewType==HEXA_VIEW) {
		return;
	}

	SelFirst=min(SelStart,SelEnd);
	SelSecond=max(SelStart,SelEnd);
	len=SelSecond-SelFirst;
	if (len==0) {
		return;
	}

	src=(TCHAR *)malloc(len);
	memcpy(src,pFrame->buf+SelFirst,len);
	for (s=src,i=0;s-src != len;s++) {
		if (*s=='\t')
			i++;
	}
	dest=(TCHAR *)malloc(len+i*(TabWidth-1));

	s=src;
	d=dest;
	nDiff=0;
	if (bToSpace) {
		AeMc.RecordCommand("convert tospace");
		while (s-src < len) {
			if (*s=='\t') {
				for (i=0;i<TabWidth;i++) {
					*d++=' ';
				}
				nDiff+=(TabWidth-1);
				s++;
			} else {
				*d++=*s++;
			}
		}
		pFrame->StartUndoGroup(12);
	} else {
		AeMc.RecordCommand("convert totab");
		while (s-src < len) {
			bFind=TRUE;
			for (i=0;i<TabWidth;i++) {
				if (s[i]==0 || s[i]!=' ') {
					bFind=FALSE;
					break;
				}
			}

			if (bFind) {
				*d++='\t';
				s+=TabWidth;
				nDiff+=(-TabWidth+1);
			} else {
				*d++=*s++;
			}
		}
		pFrame->StartUndoGroup(13);
	}
	*d=*s;
	SelSecond += nDiff;

	Delete(SelFirst,len);
	Insert(SelFirst,dest,d-dest);
	pFrame->EndUndoGroup();

	if (SelStart < SelEnd) {
		off=SelEnd=SelSecond;
	} else {
		SelStart=SelSecond;
	}

	Invalidate(SelFirst);
	SetCaret();
	free(src);
	free(dest);
}

void CApiEditText::RemoveExtraSpace()
{
	int SelFirst, SelSecond;
	TCHAR *src,*dest;
	TCHAR *s,*d;
	int len;
	int nDiff;

	if (pFrame->bReadOnly) {
		return;
	}
	AeMc.RecordCommand("convert removespace");
	if (SelStart==SelEnd) {
		SelFirst=0;
		SelSecond=pFrame->doclen;
	} else {
		SelFirst=min(SelStart,SelEnd);
		SelSecond=max(SelStart,SelEnd);
	}
	len=SelSecond-SelFirst;

	src=(TCHAR *)malloc(len);
	dest=(TCHAR *)malloc(len);
	memcpy(src,pFrame->buf+SelFirst,len);

	s=src+len-1;
	d=dest+len-1;
	nDiff=0;
	for (;;) {
		*d=*s;
		if (s==src)
			break;
		d--;
		s--;

		if (pFrame->IsEnterCode(src,len,s+1)) {
			while (AeIsWhiteSpace(*s) && s!=src) {
				s--;
				nDiff--;
			}
		}
	}

	if (nDiff) {
		pFrame->StartUndoGroup(4);
		Delete(SelFirst,len);
		Insert(SelFirst,d,len+nDiff);
		pFrame->EndUndoGroup();

		SelSecond += nDiff;

		if (SelStart==SelEnd) {
			off=SelStart=SelEnd=0;
		} else if (SelStart < SelEnd) {
			off=SelEnd=SelSecond;
		} else {
			SelStart=SelSecond;
		}

		Invalidate(SelFirst);
		SetCaret();
	}

	free(src);
	free(dest);
}

int CApiEditText::GetParaFirstLine(int nPara)
{
	int Upper,Lower;
	int r;

	Lower=0;
	Upper=TotalLine-1;
	if (nPara < 0 || nPara > pLine[Upper].nPara) {
		return -1;
	}

	for (;;) {
		r=(Upper+Lower)/2;

		if (pLine[r].nPara == nPara) {
			return r-pLine[r].nLine;
		}

		if (pLine[r].nPara > nPara) {
			Upper=r-1;
		} else {
			Lower=r+1;
		}
	}
}

int CApiEditText::GetParaLastLine(int nPara)
{
	int r;

	r=GetParaFirstLine(nPara+1);
	if (r==-1) {
		return TotalLine-1;
	} else {
		return r-1;
	}
}

void CApiEditText::GetParaFromOff(int nPos, int &pr, int &pc)
{
	int r,c;

	GetRCFromOff(nPos,r,c);
	r=r-pLine[r].nLine;

	pr=pLine[r].nPara;
	pc=nPos-pLine[r].Start;
}

int CApiEditText::GetOffFromPara(int pr, int pc)
{
	int r,re;

	r=GetParaFirstLine(pr);
	if (r==-1) {
		return -1;
	}
	re=GetParaLastLine(pr);

	if (pLine[r].Start+pc <= pLine[re].End) {
		return pLine[r].Start+pc;
	} else {
		return -1;
	}
}

BOOL CApiEditText::IsParaEmpty(TCHAR *p)
{
	for (;;p++) {
		if (AeIsLineEnd(p))
			return TRUE;
		if (!AeIsWhiteSpace(*p))
			return FALSE;
	}
}

int CApiEditText::GetPrevPara(int nPos)
{
	int pr,pc;

	GetParaFromOff(nPos,pr,pc);
	if (pr > 0)	pr--;
	while (IsParaEmpty(pFrame->buf+GetOffFromPara(pr,0)) && pr!=0) pr--;
	return pr;
}

int CApiEditText::GetIndentLevel(TCHAR *p)
{
	int level;

	for (level=0;;p++) {
		if (*p=='\t') {
			level+=TabWidth;
		} else if (*p==' '){
			level++;
		} else {
			break;
		}
	}
	return level;
}

void CApiEditText::GetIndentString(int nPara,TCHAR *szIndent,int len)
{
	TCHAR *p1,*p2;

	p1=pFrame->buf+GetOffFromPara(nPara,0);
	p2=szIndent;
	while (AeIsWhiteSpace(*p1)) {
		*p2++=*p1++;
		if (p2-szIndent == len-1)
			break;
	}
	*p2=0;
}

void CApiEditText::MakeIndentString(int nTab, TCHAR *str, int len)
{
	int i;

	if (pFrame->TestStyle(AES_SPACEFORTAB)) {
		for (i=0;i<nTab*TabWidth && i < len-1;i++) {
			str[i]=' ';
		}
	} else {
		for (i=0;i<nTab && i < len-1;i++) {
			str[i]='\t';
		}
	}
	str[i]=0;
}

BOOL CApiEditText::IsPrevParaBlockOpen(int nPos) 
{
	int tlen;
	int r;
	int toff;

	tlen=lstrlen(Parser->GetInfo(5));
	if (tlen==0) {
		return FALSE;
	}

	r=GetParaLastLine(GetPrevPara(nPos));

	toff=pLine[r].End;
	if (toff) 
		toff--;
	while (AeIsWhiteSpace(pFrame->buf[toff]) && toff > 0)
		toff--;

	if (toff >= tlen-1) {
		toff=toff-tlen+1;
		if (_strnicmp(pFrame->buf+toff,Parser->GetInfo(5),tlen)==0) {
			return TRUE;
		}
	}
	return FALSE;
}

void CApiEditText::ProcessIndent(TCHAR ch)
{
	TCHAR szIndent[4096];
	int toff;
	int tlen;
	BOOL bAllSpace;
	int nTab;

	if (ch == '\r') {
		if (IsPrevParaBlockOpen(off)) {
			MakeIndentString(1,szIndent,4096);
			pFrame->StartUndoGroup(9);
		} else {
			lstrcpy(szIndent,"");
			pFrame->StartUndoGroup(8);
		}

		GetIndentString(GetPrevPara(off),szIndent+lstrlen(szIndent),
			4096-lstrlen(szIndent));

		toff=off;
		while (AeIsWhiteSpace(pFrame->buf[toff]) && !AeIsLineEnd(pFrame->buf+toff)) toff++;
		if (toff != off) {
			Delete(off,toff-off);
		}

		if (lstrlen(szIndent)) {
			Insert(off,szIndent,lstrlen(szIndent));
			off+=lstrlen(szIndent);
		}
		pFrame->EndUndoGroup();
	}

	tlen=lstrlen(Parser->GetInfo(6));
	if (ch!='\r' && tlen && off >= tlen) {
		toff=off-tlen;
		if (_strnicmp(pFrame->buf+toff,Parser->GetInfo(6),tlen)==0) {
			for (;;) {
				toff--;
				if (pFrame->buf[toff] == '\n' || toff==0) {
					bAllSpace=TRUE;
					break;
				}
				if (!AeIsWhiteSpace(pFrame->buf[toff])) {
					bAllSpace=FALSE;
					break;
				}
			}

			if (bAllSpace) {
				GetIndentString(GetPrevPara(off),szIndent,4096);

				nTab=GetIndentLevel(szIndent)/TabWidth;

				if (!IsPrevParaBlockOpen(off)) {
					nTab=max(0,nTab-1);
				}

				MakeIndentString(nTab, szIndent,4096);

				toff=pFrame->FindParaStart(off);
				pFrame->StartUndoGroup(10);
				Delete(toff,off-toff-tlen);
				if (lstrlen(szIndent)) {
					Insert(toff,szIndent,lstrlen(szIndent));
				}
				pFrame->EndUndoGroup();

				off=off-(off-toff-tlen)+lstrlen(szIndent);
			}
		}
	}
}

void CApiEditText::BlockIndent(BOOL bUnindent)
{
	int SelFirst, SelSecond;
	int st,ed;
	int pr1,pr2,pc;
	int Line;
	TCHAR *src,*dest;
	TCHAR *s,*d;
	int i;
	int nTab;
	TCHAR szIndent[1024];
	int nDec, nDiff;
	BOOL bFirstPara=TRUE;

	if (pFrame->bReadOnly || ViewType==HEXA_VIEW) {
		return;
	}

	if (SelStart==SelEnd) {
		SelFirst=SelSecond=off;
	} else {
		SelFirst=min(SelStart,SelEnd);
		SelSecond=max(SelStart,SelEnd);
	}

	GetParaFromOff(SelFirst,pr1,pc);
	Line=GetParaFirstLine(pr1);
	st=pLine[Line].Start;

	GetParaFromOff(SelSecond,pr2,pc);
	Line=GetParaFirstLine(pr2);
	if (SelSecond==pLine[Line].Start && SelStart!=SelEnd) {
		pr2--;
	}
	Line=GetParaLastLine(pr2);
	ed=pLine[Line].End;

	src=(TCHAR *)malloc(ed-st);
	dest=(TCHAR *)malloc(ed-st+(pr2-pr1+1)*TabWidth);
	memcpy(src,pFrame->buf+st,ed-st);

	s=src;
	d=dest;
	nDiff=0;
	if (!bUnindent) {
		AeMc.RecordCommand("convert indent");
		for (;;) {
			if (!IsParaEmpty(s)) {
				MakeIndentString(1,d,4096);
				d=d+lstrlen(d);
				nDiff++;
			}

			while (!pFrame->IsEnterCode(src,ed-st,s) && s != src+ed-st) {
				*d++=*s++;
			}
			if (s==src+ed-st) {
				break;
			} else {
				*d++=*s++;
				*d++=*s++;
			}
		}

		if (SelFirst!=st) {
			SelFirst+=(pFrame->TestStyle(AES_SPACEFORTAB) ? TabWidth:1);
		}
		SelSecond+=(pFrame->TestStyle(AES_SPACEFORTAB) ? TabWidth:1)*nDiff;
	} else {
		AeMc.RecordCommand("convert unindent");
		for (;;) {
			nTab=GetIndentLevel(s)/TabWidth;

			nTab=max(0,nTab-1);

			MakeIndentString(nTab, szIndent,4096);

			nDec=0;
			while (AeIsWhiteSpace(*s)) {
				s++;
				nDec++;
			}

			for (i=0;i<lstrlen(szIndent);i++) {
				*d++=szIndent[i];
			}

			nDiff+=(nDec-lstrlen(szIndent));
			if (bFirstPara) {
				SelFirst-=nDiff;
				SelFirst=max(SelFirst,st);
				bFirstPara=FALSE;
			}

			while (!pFrame->IsEnterCode(src,ed-st,s) && s != src+ed-st) {
				*d++=*s++;
			}
			if (s==src+ed-st) {
				break;
			} else {
				*d++=*s++;
				*d++=*s++;
			}
		}
		SelSecond-=nDiff;
	}

	pFrame->StartUndoGroup(5);
	Delete(st,ed-st);
	Insert(st,dest,d-dest);
	pFrame->EndUndoGroup();

	if (SelStart == SelEnd) {
		off=SelStart=SelEnd=SelSecond;
	} else if (SelStart < SelEnd) {
		off=SelEnd=SelSecond;
		SelStart=SelFirst;
	} else {
		off=SelEnd=SelFirst;
		SelStart=SelSecond;
	}

	Invalidate(st);
	SetCaret();
	free(src);
	free(dest);
}

void CApiEditText::InsertComment(BOOL bUnComment)
{
	int SelFirst, SelSecond;
	int st,ed;
	int pr1,pr2,pc;
	int Line;
	TCHAR *src,*dest;
	TCHAR *s,*d;
	int i;
	int nDiff;
	BOOL bFirstPara=TRUE;
	TCHAR szComment[32];
	int clen;

	if (pFrame->bReadOnly || ViewType==HEXA_VIEW) {
		return;
	}

	lstrcpy(szComment,Parser->GetInfo(2));
	clen=lstrlen(szComment);
	if (clen==0) {
		return;
	}

	if (SelStart==SelEnd) {
		SelFirst=SelSecond=off;
	} else {
		SelFirst=min(SelStart,SelEnd);
		SelSecond=max(SelStart,SelEnd);
	}

	GetParaFromOff(SelFirst,pr1,pc);
	Line=GetParaFirstLine(pr1);
	st=pLine[Line].Start;

	GetParaFromOff(SelSecond,pr2,pc);
	Line=GetParaFirstLine(pr2);
	if (SelSecond==pLine[Line].Start && SelStart!=SelEnd) {
		pr2--;
	}
	Line=GetParaLastLine(pr2);
	ed=pLine[Line].End;

	src=(TCHAR *)malloc(ed-st);
	dest=(TCHAR *)malloc(ed-st+(pr2-pr1+1)*clen);
	memcpy(src,pFrame->buf+st,ed-st);

	s=src;
	d=dest;
	nDiff=0;
	if (!bUnComment) {
		AeMc.RecordCommand("convert comment");
		for (;;) {
			for (i=0;i<clen;i++) {
				*d++=szComment[i];
				nDiff++;
			}

			while (!pFrame->IsEnterCode(src,ed-st,s) && s != src+ed-st) {
				*d++=*s++;
			}
			if (s==src+ed-st) {
				break;
			} else {
				*d++=*s++;
				*d++=*s++;
			}
		}

		if (SelFirst!=st) {
			SelFirst+=clen;
		}
		SelSecond+=nDiff;
		pFrame->StartUndoGroup(14);
	} else {
		AeMc.RecordCommand("convert uncomment");
		for (;;) {
			if (_strnicmp(s,szComment,clen)==0) {
				s+=clen;
				nDiff-=clen;

				if (bFirstPara) {
					SelFirst+=nDiff;
					SelFirst=max(SelFirst,st);
					bFirstPara=FALSE;
				}
			}

			while (!pFrame->IsEnterCode(src,ed-st,s) && s != src+ed-st) {
				*d++=*s++;
			}
			if (s==src+ed-st) {
				break;
			} else {
				*d++=*s++;
				*d++=*s++;
			}
		}
		SelSecond+=nDiff;
		pFrame->StartUndoGroup(15);
	}

	Delete(st,ed-st);
	Insert(st,dest,d-dest);
	pFrame->EndUndoGroup();

	if (SelStart == SelEnd) {
		off=SelStart=SelEnd=SelSecond;
	} else if (SelStart < SelEnd) {
		off=SelEnd=SelSecond;
		SelStart=SelFirst;
	} else {
		off=SelEnd=SelFirst;
		SelStart=SelSecond;
	}

	Invalidate(st);
	SetCaret();
	free(src);
	free(dest);
}

void CApiEditText::InsertText(TCHAR *Text)
{
	if (pFrame->bReadOnly) {
		return;
	}

	InputExtraSpace(off);
	BOOL bPrevSel;
	if (SelStart != SelEnd) {
		pFrame->StartUndoGroup(1);
	}
	bPrevSel=DeleteSelection();

	Insert(off,Text,lstrlen(Text));
	Invalidate(pFrame->FindParaStart(off));
	off=off+lstrlen(Text);
	SetCaret();

	if (bPrevSel) {
		pFrame->EndUndoGroup();
	}
}

void CApiEditText::CopyChar(int dir)
{
	int pr,pc;
	int col;
	int toff;
	TCHAR szTemp[3]={0,};
	BOOL bSpace=FALSE;

	if (pFrame->bReadOnly || ViewType==HEXA_VIEW) {
		return;
	}

	GetParaFromOff(off,pr,pc);
	if (dir==0) {
		AeMc.RecordCommand("copychar up");
		if (pr > 0) {
			pr--;
		} else {
			return;
		}
	}
	if (dir==1) {
		AeMc.RecordCommand("copychar down");
		if (pr < pLine[TotalLine-1].nPara) {
			pr++;
		} else {
			return;
		}
	}

	ClearSelection();

	toff=GetOffFromPara(pr,0);
	for (col=0;;) {
		if (AeIsLineEnd(pFrame->buf+toff)) {
			return;
		}
		if (col==pc) {
			break;
		}
		if (col>pc) {
			bSpace=TRUE;
			break;
		}
		if (pFrame->IsDBCS(toff)) {
			toff+=2;
			col+=2;
		} else {
			toff++;
			col++;
		}
	}

	if (bSpace) {
		szTemp[0]=' ';
	} else {
		szTemp[0]=pFrame->buf[toff];
		if (pFrame->IsDBCS(toff)) {
			szTemp[1]=pFrame->buf[toff+1];
		}
	}
	Insert(off,szTemp,lstrlen(szTemp));
	Invalidate(pFrame->FindParaStart(off));
	off=off+lstrlen(szTemp);
	SetCaret();
}

void CApiEditText::BeginPrint(Ae_PrintInfo *pi)
{
	int l,s,e;
	int nPara=0, nLine=0;
	SIZE sz;
	TEXTMETRIC tm;
	int ParseID;
	int Alignlen;

	if (pi->ParseID == -1) {
		ParseID=(int)GetParser()->GetInfo(0);
	} else {
		ParseID=pi->ParseID;
	}

	switch (ParseID) {
	default:
		pi->Parser=NULL;
		break;
	case 1:
		pi->Parser=new CParseCpp;
		break;
	case 2:
		pi->Parser=new CParseHtml;
		break;
	case 3:
		pi->Parser=new CParseSql;
		break;
	}

	GetTextMetrics(pi->pdc,&tm);
	if (pFrame->TestStyle(AES_CALCTABWITHAVG)) {
		sz.cx=tm.tmAveCharWidth;
	} else {
		GetTextExtentPoint32(pi->pdc," ",1,&sz);
	}
	pi->TabSize=sz.cx*TabWidth;
	pi->LineHeight=int(tm.tmHeight*LineRatio/100);
	pi->LinePerPage=(pi->prt.bottom-pi->prt.top)/pi->LineHeight;

	Alignlen=TotalLine*2;
	pi->pLine=(tagLine *)malloc(sizeof(tagLine)*Alignlen);
	for (l=0;;l++) {
		if (l >= Alignlen) {
			Alignlen += 1000;
			pi->pLine=(tagLine *)realloc(pi->pLine,sizeof(tagLine)*Alignlen);
		}
		GetLine(l,s,e,pi);

		pi->pLine[l].Start=s;
		pi->pLine[l].End=e;
		pi->pLine[l].nPara=nPara;
		pi->pLine[l].nLine=nLine;
		if (s==-1) {
			break;
		}

		if (pFrame->IsEnterCode(e)) {
			nPara++;
			nLine=0;
		} else {
			nLine++;
		}
	}

	if (pi->Parser) {
		pi->Parser->ParseLines(*this,l,pi);
	}
	pi->TotalPage=(l-1)/pi->LinePerPage+1;
}

void CApiEditText::GetLine(int Line, int &s, int &e,Ae_PrintInfo *pi)
{
	TCHAR *p,*Head;
	int len, acwidth;
	TCHAR *EndPos=NULL;
	TCHAR *EndPosNoSpace=NULL;
	BOOL IsPrevDBCS=FALSE;
	SIZE sz;

	pi->Origin=pi->prt.left;
	if (pi->bLineNum) {
		GetTextExtentPoint32(pi->pdc,"999999",6,&sz);
		pi->Origin+=sz.cx;
	}

	if (Line == 0) {
		p=pFrame->buf;
		s=0;
	} else {
		p=pFrame->buf+pi->pLine[Line-1].End;
		if (AeIsDocEnd(p)) {
			s=-1;
			e=-1;
			return;
		}

		if (pFrame->IsEnterCode(p)) {
			p+=2;
		}

		s=p-pFrame->buf;
	}

	if (pi->nWrap == 0) {
		while (!AeIsLineEnd(p)) {
			if (pFrame->IsDBCS(p-pFrame->buf)) {
				p++;
			}
			p++;
		}
	} else {
		for (acwidth=pi->Origin,Head=p;;) {
			if (*p == '\t') {
				len=1;
				acwidth =((acwidth-pi->Origin)/pi->TabSize+1)*pi->TabSize+pi->Origin;
				EndPos=p;
			} else {
				if (pFrame->IsDBCS(p-pFrame->buf)) {
					len=2;

					if (pi->nWrap==2 || (pi->nWrap==3 && p != Head && IsPrevDBCS==FALSE)) {
						EndPos=p;
						if (*EndPos != ' ') {
							EndPosNoSpace=EndPos;
						}
					}
					IsPrevDBCS=TRUE;
				} else {
					len=1;

					if (IsPrevDBCS==TRUE) {
						EndPos=p;
						if (*EndPos != ' ') {
							EndPosNoSpace=EndPos;
						}
					}
					IsPrevDBCS=FALSE;
				}
				GetTextExtentPoint32(pi->pdc,p,len,&sz);
				acwidth+=sz.cx;
			}

			if (AeIsLineEnd(p)) {
				EndPos=p;
				break;
			}

			if (acwidth > pi->prt.right) {
				break;
			}

			if (*p == ' ' || *p=='\t') {
				EndPos=p+1;
				if (*EndPos != ' ') {
					EndPosNoSpace=EndPos;
				}
			}

			p+=len;
		}
	}

	if (pi->nWrap == 1 || EndPos == NULL) {
		p=p;
	} else {
		p=EndPos;
	}
	if (pFrame->TestStyle(AES_NOFIRSTSPACE) && *p==' ' && EndPosNoSpace!=NULL) {
		p=EndPosNoSpace;
	}
	e=p-pFrame->buf;
}

BOOL CApiEditText::PrintPage(Ae_PrintInfo *pi,int nPage)
{
	int l,Line;
	int len;
	int nowoff;
	int x;
	SIZE sz;
	int nUnit;
	int style;
	BOOL bFindUnit;
	COLORREF fore, back;
	TCHAR szLine[32];
	COLORREF OldTextColor, OldBkColor;
	int OldBkMode;
	BOOL bEnd=FALSE;

	OldTextColor=GetTextColor(pi->pdc);
	OldBkColor=GetBkColor(pi->pdc);
	OldBkMode=GetBkMode(pi->pdc);
	for (l=pi->LinePerPage*(nPage-1),Line=0;l<pi->LinePerPage*nPage;l++,Line++) {
		if (pi->pLine[l].Start == -1) {
			bEnd=TRUE;
			break;
		}

		x=pi->Origin;
		nowoff=pi->pLine[l].Start;
		for (;;) {
			for (len=0;;) {
				if (pFrame->buf[nowoff+len] == '\t') {
					if (len==0)
						len=1;
					break;
				}
				if (nowoff+len==pi->pLine[l].End) {
					break;
				}

				if (pi->Parser) {
					bFindUnit=FALSE;
					for (nUnit=0;nUnit<pi->Parser->pInfo[l].UnitSize;nUnit++) {
						if (pi->Parser->pInfo[l].pUnit[nUnit].pos == nowoff+len) {
							bFindUnit=TRUE;
							break;
						}
						if (pi->Parser->pInfo[l].pUnit[nUnit].pos == -1) {
							break;
						}
					}

					if (bFindUnit && len!=0) {
						break;
					}
				}
				len++;
			}

			if (pi->Parser && len!=0) {
				for (nUnit=0;nUnit<pi->Parser->pInfo[l].UnitSize;nUnit++) {
					if (pi->Parser->pInfo[l].pUnit[nUnit].pos >= nowoff+len || 
						pi->Parser->pInfo[l].pUnit[nUnit].pos == -1) {
						break;
					}
				}
				style=pi->Parser->pInfo[l].pUnit[nUnit-1].style;
				pi->Parser->GetStyleColor(style,fore,back);
				if (fore!=-1) {
					SetTextColor(pi->pdc,fore);
				} else {
					SetTextColor(pi->pdc,RGB(0,0,0));
				}
				if (back!=-1) {
					SetBkColor(pi->pdc,back);
					SetBkMode(pi->pdc,OPAQUE);
				} else {
					SetBkMode(pi->pdc,TRANSPARENT);
				}
			}else {
				SetTextColor(pi->pdc,RGB(0,0,0));
				SetBkMode(pi->pdc,TRANSPARENT);
			}

			if (pFrame->buf[nowoff] == '\t') {
				x =((x-pi->Origin)/pi->TabSize+1)*pi->TabSize+pi->Origin;
			} else {
				ExtTextOut(pi->pdc,x,pi->prt.top+Line*pi->LineHeight,
					ETO_CLIPPED,&pi->prt,pFrame->buf+nowoff,len,NULL);
				GetTextExtentPoint32(pi->pdc,pFrame->buf+nowoff,len,&sz);
				x+=sz.cx;
			}

			nowoff+=len;
			if (nowoff==pi->pLine[l].End) {
				break;
			}
		}

		if (pi->bLineNum) {
			if (pi->pLine[l].nLine==0) {
				wsprintf(szLine,"%d",pi->pLine[l].nPara);
				SetTextColor(pi->pdc,RGB(128,128,128));
				SetBkMode(pi->pdc,TRANSPARENT);
				TextOut(pi->pdc,pi->prt.left,pi->prt.top+Line*pi->LineHeight,
					szLine,lstrlen(szLine));
			}
		}
	}
	SetTextColor(pi->pdc,OldTextColor);
	SetBkColor(pi->pdc,OldBkColor);
	SetBkMode(pi->pdc,OldBkMode);

	return bEnd;
}

void CApiEditText::EndPrint(Ae_PrintInfo *pi)
{
	free(pi->pLine);
	if (pi->Parser) {
		delete pi->Parser;
	}
}

void CApiEditText::InitView()
{
	int i;

	Parser->InitInfo(TRUE);
	Linelen=100;
	if (pLine) {
		free(pLine);
	}
	pLine=(tagLine *)malloc(sizeof(tagLine)*Linelen);
	for (i=0;i<Linelen;i++) {
		pLine[i].Start=-1;
	}
	bOvr=FALSE;
	off=0;
	PrevX=0;
	bLineEnd=FALSE;
	xMax=1024;
	xPos=0;
	yPos=0;
	SelStart=SelEnd=0;
	ClearBookmark();
	UpdateLineInfo();
	UpdateScrollInfo();
	if (GetFocus()==hWnd) {
		SetCaret();
	}
	bPrevCompoGroup=FALSE;
	iDrag=0;
	OldSplit=0;
	ExtraX=0;
	ignoreVirt=0;
	SelExtraX=0;
	Invalidate(-1);
}

void CApiEditText::SetOffset(int aoff)
{
	off=aoff;
	SetCaret();
}

void CApiEditText::SetVertSplit(BOOL aValue)
{
	if (bVertSplit!=aValue) {
		bVertSplit=aValue;
		SendMessage(hWnd,WM_SIZE,0,0);
	}
}

void CApiEditText::SetHorzSplit(BOOL aValue)
{
	if (bHorzSplit!=aValue) {
		bHorzSplit=aValue;
		SendMessage(hWnd,WM_SIZE,0,0);
	}
}

BOOL CApiEditText::StartSplit(HWND hWnd,BOOL fDoubleClick,int x,int y)
{
	RECT crt;

	GetClientRect(hWnd,&crt);
	if (bVertSplit && x > crt.right-SBW && y < SPLITWIDTH) {
		if (fDoubleClick) {
			pFrame->SplitView(AESPLIT_VERT,5000);
			return TRUE;
		}
		iDrag=AESPLIT_VERT;
	}
	if (bHorzSplit && x < SPLITWIDTH && y > crt.bottom-SBW) {
		if (fDoubleClick) {
			pFrame->SplitView(AESPLIT_HORZ,5000);
			return TRUE;
		}
		iDrag=AESPLIT_HORZ;
	}
	if (iDrag) {
		SetCapture(hWnd);
		bCapture=TRUE;
		OldSplit=0;
		pFrame->DrawSplitBar(iDrag,OldSplit);
		return TRUE;
	}
	return FALSE;
}

BOOL CApiEditText::Spliting(HWND hWnd,int x,int y)
{
	int tx,ty;
	RECT FrameRect;

	GetClientRect(pFrame->hWnd,&FrameRect);
	if (iDrag == AESPLIT_VERT) {
		ty=min(max(y,FrameRect.top),FrameRect.bottom-VGAP);
		if (OldSplit != ty) {
			pFrame->DrawSplitBar(iDrag,OldSplit);
			OldSplit=ty;
			pFrame->DrawSplitBar(iDrag,OldSplit);
		}
		return TRUE;
	}
	if (iDrag ==AESPLIT_HORZ) {
		tx=min(max(x,FrameRect.left),FrameRect.right-VGAP);
		if (OldSplit != tx) {
			pFrame->DrawSplitBar(iDrag,OldSplit);
			OldSplit=tx;
			pFrame->DrawSplitBar(iDrag,OldSplit);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CApiEditText::EndSplit(HWND hWnd,int x,int y)
{
	int Ratio;
	RECT crt;

	GetClientRect(hWnd,&crt);
	if (iDrag) {
		pFrame->DrawSplitBar(iDrag,OldSplit);
		if (iDrag==AESPLIT_VERT) {
			Ratio=y*10000/(crt.bottom-crt.top);
		} else {
			Ratio=x*10000/(crt.right-crt.left);
		}
		if (Ratio > 500 && Ratio < 9500) {
			pFrame->SplitView(iDrag,Ratio);
		}
		iDrag=0;
		return TRUE;
	}
	return FALSE;
}

void CApiEditText::AeTextOut(HDC hdc,int x,int y,TCHAR *str,int len)
{
	int i;
	int boff;

	boff=str-pFrame->buf;
	for (i=0;i<len;) {
		if (pFrame->IsDBCS(boff+i)) {
			lpDx[i++]=GetCharWidth(str+i,2);
			lpDx[i++]=0;
		} else {
			lpDx[i++]=GetCharWidth(str+i,1);
		}
	}
	ExtTextOut(hdc,x,y,0,NULL,str,len,lpDx);
}

void CApiEditText::SetBookmark(int *aMark) 
{
	memcpy(arMark,aMark,sizeof(int)*MAXBOOKMARK);
	Invalidate(-1);
}

void CApiEditText::SetVirtualPos(int aValue)
{
	bVirtualPos=aValue;
	if (bVirtualPos==FALSE && ExtraX > 0) {
		ExtraX=0;
		SetCaret();
	}
}

BOOL CApiEditText::IsVirtualPosable()
{
	if (bVirtualPos || (bColumnMode && SelStart != SelEnd)) {
		if (ignoreVirt > 0) {
			return FALSE;
		} else {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CApiEditText::InputExtraSpace(int &nPos)
{
	TCHAR *pExSpace;

	if (ExtraX > 0) {
		pExSpace=(TCHAR *)malloc(ExtraX+1);
		memset(pExSpace,' ',ExtraX);
		pExSpace[ExtraX]=0;
		Insert(nPos,pExSpace,ExtraX);
		nPos+=ExtraX;
		free(pExSpace);
		ExtraX=0;
		return TRUE;
	}
	return FALSE;
}

void CApiEditText::SetColumnMode(int aMode)
{
	LOGFONT tFont;
	HFONT hFont;

	bColumnMode=aMode;
	if (bColumnMode==FALSE) {
		if (SelStart != SelEnd) {
			if (ExtraX != 0) {
				ExtraX=0;
				SetCaret();
			}
			SelExtraX=0;
		}
		if (pFrame->TestStyle(AES_FIXEDCOLUMN) && Bklogfont.lfHeight!=-1) {
			SetFont(&Bklogfont);
			Bklogfont.lfHeight=-1;
		}
	} else {
		if (pFrame->TestStyle(AES_NOWRAPCOLUMN) && GetWrap()!=0) {
			SetWrap(0);
		}
		if (pFrame->TestStyle(AES_FIXEDCOLUMN)) {
			if (pFrame->FixedFont.lfHeight==0) {
				hFont=(HFONT)GetStockObject(SYSTEM_FIXED_FONT);
				GetObject(hFont,sizeof(LOGFONT),&tFont);
			} else {
				tFont= pFrame->FixedFont;
			}
			GetFont(&Bklogfont);
			SetFont(&tFont);
		}
	}

	if (SelStart != SelEnd) {
		Invalidate(-1);
	}
}

void CApiEditText::GetSelInLine(int Line,int &s,int &e)
{
	int SelFirst, SelSecond;
	int sx,sy;
	int ex,ey;
	int left,right;
	int BkExtraX;

	s=e=0;

	if (SelStart == SelEnd) {
		return;
	}

	SelFirst=min(SelStart,SelEnd);
	SelSecond=max(SelStart,SelEnd);
	if (SelFirst > pLine[Line].End || SelSecond <= pLine[Line].Start) {
		return;
	}

	BkExtraX=ExtraX;
	ExtraX=0;
	ignoreVirt++;

	GetXYFromOff(SelStart,sx,sy);
	GetXYFromOff(SelEnd,ex,ey);
	ex+=SelExtraX*SpaceWidth;
	left=min(sx,ex);
	right=left+abs(sx-ex);

	s=GetOffFromXY(left,DocY(Line*LineHeight));
	e=GetOffFromXY(right,DocY(Line*LineHeight));
	ignoreVirt--;
	ExtraX=BkExtraX;
}

int CApiEditText::MakeSelTable()
{
	int Size;
	int SelFirst, SelSecond;
	int c,rs,re,Line;
	int idx;
	int len=0;

	if (!bColumnMode || SelStart == SelEnd) return 0;
	Size=100;
	SelTable=(Ae_SelTable *)malloc(Size*sizeof(Ae_SelTable));

	SelFirst=min(SelStart,SelEnd);
	SelSecond=max(SelStart,SelEnd);
	GetRCFromOff(SelFirst,rs,c);
	GetRCFromOff(SelSecond,re,c);
	for (Line=rs,idx=0;Line<=re;Line++,idx++) {
		if (idx == Size-1) {
			Size+=100;
			SelTable=(Ae_SelTable *)realloc(SelTable,Size*sizeof(Ae_SelTable));
		}
		SelTable[idx].Line=Line;
		GetSelInLine(Line,SelTable[idx].Start,SelTable[idx].End);
		len+=SelTable[idx].End-SelTable[idx].Start+2;
	}
	SelTable[idx].Line=-1;
	
	return len;
}

void CApiEditText::FreeSelTable()
{
	if (SelTable) free(SelTable);
	SelTable=NULL;
}

void CApiEditText::PasteColumn(int nPos,int aExtra,TCHAR *data)
{
	int PasteX,Line,dummy,y;
	int nLine;
	int toff;
	int BkVirtualPos;

	BkVirtualPos=bVirtualPos;
	bVirtualPos=TRUE;

	GetXYFromOff(nPos,PasteX,dummy);
	GetRCFromOff(nPos,Line,dummy);

	for (nLine=Line;;nLine++) {
		if (*data==(TCHAR)0xff) {
			break;
		}
		if (nLine == TotalLine) {
			Insert(pFrame->doclen,"\r\n",2);
		}
		GetXYFromOff(pLine[nLine].Start,dummy,y);
		toff=GetOffFromXY(PasteX,y);

		if (lstrlen(data)) {
			InputExtraSpace(toff);
			Insert(toff,data,lstrlen(data));
		}
		data+=lstrlen(data)+1;
	}
	bVirtualPos=BkVirtualPos;
}

void CApiEditText::ColumnCopyString(BOOL bCopy,int to,int aExtra)
{
	TCHAR *Src,*ptr;
	int len,idx,linelen;
	int Acc;

	len=MakeSelTable();
	Src=(TCHAR *)malloc(len+1);
	ptr=Src;
	for (idx=0;SelTable[idx].Line!=-1;idx++) {
		linelen=SelTable[idx].End-SelTable[idx].Start;
		memcpy(ptr,pFrame->buf+SelTable[idx].Start,linelen);
		ptr+=linelen;
		*ptr++=0;
	}
	*ptr=(TCHAR)0xff;

	if (bCopy) {
		pFrame->StartUndoGroup(18);
	} else {
		pFrame->StartUndoGroup(19);
		for (idx=0,Acc=0;SelTable[idx].Line!=-1;idx++) {
			linelen=SelTable[idx].End-SelTable[idx].Start;
			Delete(SelTable[idx].Start-Acc,linelen);
			if (SelTable[idx].Start < to) {
				to-=linelen;
			}
			Acc+=linelen;
		}
	}
	FreeSelTable();

	PasteColumn(to,aExtra,Src);
	pFrame->EndUndoGroup();
	free(Src);

	SelStart=SelEnd=off=to+aExtra;
	SetCaret();
	Invalidate(-1);
}

void CApiEditText::ChangeCaseColumn(int action)
{
	int idx;
	int bkSelStart,bkSelEnd;

	MakeSelTable();
	bColumnMode=FALSE;
	bkSelStart=SelStart;
	bkSelEnd=SelEnd;
	pFrame->StartUndoGroup(20);

	for (idx=0;SelTable[idx].Line!=-1;idx++) {
		SelStart=SelTable[idx].Start;
		SelEnd=SelTable[idx].End;
		if (SelStart!=SelEnd) {
			ChangeCase(action);
		}
	}
	
	pFrame->EndUndoGroup();
	bColumnMode=TRUE;
	SelStart=bkSelStart;
	SelEnd=bkSelEnd;
	FreeSelTable();
}

void CApiEditText::ProcessKey(UINT vk,BOOL bControl,BOOL bShift)
{
	int r,c;
	int oldr;
	int OldOff;
	int toff;

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
		break;
	case VK_F6:
		pFrame->ChangeActiveView(this,(bShift ? 2:1));
		break;
	case VK_LEFT:
		if (off > 0) {
			GetRCFromOff(off,r,c);

			OldOff=off;
			if (bControl) {
				off=GetPrevWord(off);
				ExtraX=0;
			} else {
				if (ExtraX > 0) {
					if (IsVirtualPosable()) {
						ExtraX--;
					} else {
						ExtraX=0;
					}
				} else {
					if (off==pLine[r].Start) {
						if (pFrame->IsEnterCode(pFrame->GetPrevOff(off)) || 
							pFrame->TestStyle(AES_USELINEEND)==FALSE) {
							off=pFrame->GetPrevOff(off);
							bLineEnd=FALSE;
						} else {
							bLineEnd=TRUE;
						}
					} else {
						off=pFrame->GetPrevOff(off);
						bLineEnd=FALSE;
					}
				}
			}

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
		break;
	case VK_RIGHT:
		if (off < pFrame->doclen || IsVirtualPosable()) {
			GetRCFromOff(off,r,c);

			OldOff=off;
			if (bControl) {
				off=GetNextWord(off);
				ExtraX=0;
			} else {
				if (ExtraX > 0) {
					if (IsVirtualPosable()) {
						ExtraX++;
					} else {
						ExtraX=0;
					}
				} else {
					if (off==pLine[r].End) {
						if (AeIsLineEnd(pFrame->buf+off)) {
							if (IsVirtualPosable()) {
								ExtraX++;
							} else {
								off=pFrame->GetNextOff(off);
							}
						}
						bLineEnd=FALSE;
					} else {
						off=pFrame->GetNextOff(off);
						if (off==pLine[r].End && !pFrame->IsEnterCode(off) && 
							pFrame->TestStyle(AES_USELINEEND)) {
							bLineEnd=TRUE;
						} else {
							bLineEnd=FALSE;
						}
					}
				}
			}

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
		break;
	case VK_UP:
		if (bControl && bShift)
			break;
		GetRCFromOff(off,r,c);
		
		OldOff=off;
		if (r > 0) {
			if (bControl) {
				SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, 0L);
				if (r != (Fheight+yPos)/LineHeight)
					break;
			}
			off=GetXPosOnLine(r-1,PrevX);
		// 첫 줄에서 위로 이동은 문서 처음으로 이동한다.
		} else {
			off=0;
		}

		if (bShift) {
			ExpandSelection(OldOff,off);
		} else {
			if (SelStart != SelEnd) {
				off=min(SelStart, SelEnd);
				ClearSelection();
				SetCaret(!bVirtualPos);
				SendMessage(hWnd,WM_KEYDOWN,VK_UP,(LPARAM)0);
			}
		}
		SetCaret(FALSE);

		if (!bShift) {
			ClearSelection();
		}
		break;
	case VK_DOWN:
		if (bControl && bShift)
			break;
		GetRCFromOff(off,r,c);
		if (bControl) {
			SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, 0L);
			if (r != yPos/LineHeight-1) {
				break;
			}
		}

		OldOff=off;
		if (r < GetRowCount()-1) {
			off=GetXPosOnLine(r+1,PrevX);
		// 마지막 줄에서 아래로 이동은 문서의 끝으로 이동이다.
		} else {
			off=pFrame->doclen;
		}

		if (bShift) {
			ExpandSelection(OldOff,off);
		} else {
			if (SelStart != SelEnd) {
				off=max(SelStart, SelEnd);
				ClearSelection();
				SetCaret(!bVirtualPos);
				SendMessage(hWnd,WM_KEYDOWN,VK_DOWN,(LPARAM)0);
			}
		}
		SetCaret(FALSE);

		if (!bShift) {
			ClearSelection();
		}
		break;
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
		// 첫줄에서는 문서 처음으로 이동한다.
		if (r == 0) {
			off=0;
		} else {
			off=GetXPosOnLine(r,PrevX);
		}
		if (bShift) {
			ExpandSelection(OldOff,off);
		} else {
			ClearSelection();
		}
		SetCaret(FALSE);
		break;
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
		// 마지막 줄이면 문서 끝으로 이동한다.
		if (r == GetRowCount()-1) {
			off=pFrame->doclen;
		} else {
			off=GetXPosOnLine(r,PrevX);
		}
		if (bShift) {
			ExpandSelection(OldOff,off);
		} else {
			ClearSelection();
		}
		SetCaret(FALSE);
		break;
	case VK_HOME:
		GetRCFromOff(off,r,c);
		OldOff=off;
		if (bControl) {
			off=0;
		} else {
			off=GetOffFromRC(r,0);
			if (pFrame->TestStyle(AES_HOMETOFIRSTCHAR)) {
				toff=off;
				while (AeIsWhiteSpace(pFrame->buf[toff])) 
					toff++;
				if (OldOff!=toff && !AeIsLineEnd(pFrame->buf+toff)) {
					off=toff;
				}
			}
		}
		bLineEnd=FALSE;
		if (bShift) {
			ExpandSelection(OldOff,off);
		} else {
			ClearSelection();
		}
		SetCaret();
		break;
	case VK_END:
		GetRCFromOff(off,r,c);
		OldOff=off;
		if (bControl) {
			off=pFrame->doclen;
		} else {
			ignoreVirt++;
			off=GetOffFromRC(r,1000000);
			ignoreVirt--;
		}
		if (!AeIsLineEnd(pFrame->buf+off)) {
			bLineEnd=TRUE;
		}
		if (bShift) {
			ExpandSelection(OldOff,off);
		} else {
			ClearSelection();
		}
		SetCaret();
		break;
	case VK_DELETE:
		if (pFrame->bReadOnly || bCapture)
			break;
		if (bShift) {
			SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_AE_CUT,0),0);
			break;
		}
		if (DeleteSelection()==FALSE) {
			if (pFrame->IsDBCS(off)) {
				Delete(off, 2);
			} else {
				Delete(off, 1);
			}
		}
		Invalidate(pFrame->FindParaStart(off));
		SetCaret();
		break;
	case VK_BACK:
		if (pFrame->bReadOnly || bCapture)
			break;
		if ((off==0 && SelStart==SelEnd) || (bShift && bControl))
			break;

		if (DeleteSelection()==FALSE) {
			if (bControl) {
				OldOff=off;
				off=GetPrevWord(OldOff);
				Delete(off,OldOff-off);
			} else {
				off=pFrame->GetPrevOff(off);
				if (pFrame->IsDBCS(off)) {
					Delete(off, 2);
				} else {
					Delete(off, 1);
				}
			}
		}
		Invalidate(pFrame->FindParaStart(off));
		SetCaret();
		break;
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
		break;
	}
}

void CApiEditText::Move(int n, BOOL bSelect)
{
	int pr, pc;
	int toff;

	GetParaFromOff(off,pr,pc);
	switch (n) {
	case AECM_LAST:
		toff=pFrame->LastEditOff;
		break;
	case AECM_WORDHOME:
		toff=off;
		if (toff == 0)
			break;
		// 단어 끝 구분자에 있는 경우를 위해 일단 한칸 앞으로 이동.
		for (toff--;;toff--) {
			if (IsDelimiter(toff) || toff==0)
				break;
		}
		if (toff !=0)
			toff++;
		break;
	case AECM_WORDEND:
		for (toff=off;;toff++) {
			if (IsDelimiter(toff))
				break;
		}
		break;
	case AECM_NEXTPARA:
		pr++;
		toff=GetOffFromPara(pr,0);
		if (toff==-1) {
			return;
		}
		break;
	case AECM_PREVPARA:
		pr--;
		toff=GetOffFromPara(pr,0);
		if (toff==-1) {
			return;
		}
		break;
	case AECM_PARAHOME:
		toff=pLine[GetParaFirstLine(pr)].Start;
		break;
	case AECM_PARAEND:
		GetParaFromOff(off,pr,pc);
		toff=pLine[GetParaLastLine(pr)].End;
		break;
	default:
		return;
	}

	if (bSelect) {
		ExpandSelection(off,toff);
		off=toff;
	} else {
		off=toff;
		ClearSelection();
	}
	SetCaret();
}

void CApiEditText::Select(int n)
{
	int r,c;
	int pr,pc;

	switch (n) {
	case AECS_WORD:
		GetNowWord(off,SelStart,SelEnd);
		SetSelect(SelStart,SelEnd,FALSE);
		break;
	case AECS_LINE:
		GetRCFromOff(off,r,c);
		SelStart=pLine[r].Start;
		SelEnd=pFrame->IncludeEnter(pLine[r].End);
		SetSelect(SelStart,SelEnd,FALSE);
		break;
	case AECS_PARA:
		GetParaFromOff(off,pr,pc);
		SelStart=pLine[GetParaFirstLine(pr)].Start;
		SelEnd=pFrame->IncludeEnter(pLine[GetParaLastLine(pr)].End);
		SetSelect(SelStart,SelEnd,FALSE);
		break;
	case AECS_DOC:
		SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_AE_SELALL,0),0);
		break;
	default:
		return;
	}
}

void CApiEditText::Scroll(int nLine, int nCode/*=0*/)
{
	int yInc;
	int LinePerPage;
	RECT srt;

	if (nCode) {
		switch (nCode) {
		case 1:
			SendMessage(hWnd,WM_VSCROLL,MAKELONG(SB_PAGEDOWN,0),0);
			return;
		case -1:
			SendMessage(hWnd,WM_VSCROLL,MAKELONG(SB_PAGEUP,0),0);
			return;
		case 2:
			nLine=Fheight/LineHeight/2;
			break;
		case -2:
			nLine=-Fheight/LineHeight/2;
			break;
		}
	}
	LinePerPage=(Fheight/LineHeight)*LineHeight;
	yInc=LineHeight*nLine;
	yInc=max(-yPos, min(yInc, yMax-yPos-LinePerPage));
	yInc=yInc-(yInc % LineHeight);
	yPos=yPos+yInc;
	srt=frt;
	srt.left=0;
	if (pFrame->hBackBit && pFrame->BackBitPos!=AEBB_TILESCROLL) {
		Invalidate(-1);
	} else {
		ScrollWindow(hWnd, 0, -yInc, &srt, &srt);
	}
	if (IsWindowEnabled(hVertScroll)) {
		SetScrollPos(hVertScroll, SB_CTL, yPos, TRUE);
	}
	ManageCaret();
}

void CApiEditText::ProcessKeyRecord(UINT vk,BOOL bControl,BOOL bShift)
{
	switch (vk) {
	case VK_LEFT:
		if (bShift) {
			if (bControl) {
				AeMc.RecordCommand("movesel wleft");
			} else {
				AeMc.RecordCommand("movesel left");
			}
		} else {
			if (bControl) {
				AeMc.RecordCommand("move wleft");
			} else {
				AeMc.RecordCommand("move left");
			}
		}
		break;
	case VK_RIGHT:
		if (bShift) {
			if (bControl) {
				AeMc.RecordCommand("movesel wright");
			} else {
				AeMc.RecordCommand("movesel right");
			}
		} else {
			if (bControl) {
				AeMc.RecordCommand("move wright");
			} else {
				AeMc.RecordCommand("move right");
			}
		}
		break;
	case VK_UP:
		if (bControl && bShift)
			break;
		if (bShift) {
			AeMc.RecordCommand("movesel up");
		} else {
			if (bControl) {
				AeMc.RecordCommand("scroll up");
			} else {
				AeMc.RecordCommand("move up");
			}
		}
		break;
	case VK_DOWN:
		if (bControl && bShift)
			break;
		if (bShift) {
			AeMc.RecordCommand("movesel down");
		} else {
			if (bControl) {
				AeMc.RecordCommand("scroll down");
			} else {
				AeMc.RecordCommand("move down");
			}
		}
		break;
	case VK_PRIOR:
		if (bShift) {
			AeMc.RecordCommand("movesel pgup");
		} else {
			AeMc.RecordCommand("move pgup");
		}
		break;
	case VK_NEXT:
		if (bShift) {
			AeMc.RecordCommand("movesel pgdn");
		} else {
			AeMc.RecordCommand("move pgdn");
		}
		break;
	case VK_HOME:
		if (bShift) {
			if (bControl) {
				AeMc.RecordCommand("movesel dochome");
			} else {
				AeMc.RecordCommand("movesel home");
			}
		} else {
			if (bControl) {
				AeMc.RecordCommand("move dochome");
			} else {
				AeMc.RecordCommand("move home");
			}
		}
		break;
	case VK_END:
		if (bShift) {
			if (bControl) {
				AeMc.RecordCommand("movesel docend");
			} else {
				AeMc.RecordCommand("movesel end");
			}
		} else {
			if (bControl) {
				AeMc.RecordCommand("move docend");
			} else {
				AeMc.RecordCommand("move end");
			}
		}
		break;
	case VK_DELETE:
		if (pFrame->bReadOnly || bCapture)
			break;
		if (!bShift) {
			AeMc.RecordCommand("delete");
		}
		break;
	case VK_BACK:
		if (pFrame->bReadOnly || bCapture || ViewType==HEXA_VIEW)
			break;
		if ((off==0 && SelStart==SelEnd) || (bShift && bControl))
			break;

		if (bControl) {
			AeMc.RecordCommand("bs word");
		} else {
			AeMc.RecordCommand("bs char");
		}
		break;
	}
}

void CApiEditText::EndComp()
{
	HIMC hImc;
	if (bComp) {
		hImc=ImmGetContext(hWnd);
		ImmNotifyIME(hImc,NI_COMPOSITIONSTR,CPS_COMPLETE,0);
		ImmReleaseContext(hWnd,hImc);
	}
}

// 상용구 삽입. 삽입 후 캐럿 이동, 선택까지 해야 한다.
void CApiEditText::InsertClipText(TCHAR *Text)
{
	TCHAR *pTemp;
	TCHAR *s,*d;
	int ts=-1,te=-1;
	int len;

	pFrame->StartUndoGroup(22);
	DeleteSelection();
	len=lstrlen(Text);
	pTemp=(TCHAR *)malloc(len+1);
	for (s=Text,d=pTemp;*s;) {
		// 뒤쪽에 최소한 3바이트 남았을 때 점검
		if (s-Text <= len-3 && s[0]=='^' && s[1]=='|' && s[2]=='^') {
			// 처음 발견되었을 때 오프셋 기록
			if (ts==-1) {
				ts=off+(s-Text);
			// 두번째 발견되었을 때 끝 오프셋 기록. 세번째 이후는 무시
			} else if (te==-1) {
				te=off+(s-Text)-3;
			}
			// 건너 뜀
			s+=3;
		} else {
			*d++=*s++;
		}
	}
	*d=0;

	InsertText(pTemp);
	if (te != -1) {
		SetSelect(ts,te,FALSE);
	} else if (ts != -1) {
		SetOffset(ts);
	} else {
		SetCaret();
	}
	free(pTemp);
	pFrame->EndUndoGroup();
}

void CApiEditText::TestAutoClip(int ch)
{
	TCHAR *pWord;
	int toff,len;

	// 최소한 2글자는 입력되어 있어야 검색의 의미가 있다.
	if (pFrame->TestStyle(AES_USEAUTOCLIP) && off >= 2) {
		if ((pFrame->TestStyle(AES_AUTOCLIPBYTAB) && ch=='\t') || 
			(pFrame->TestStyle(AES_AUTOCLIPBYTAB)==FALSE && ch==' ')) {
			toff=GetAutoClipName();
			len=off-1-toff;
			// len이 0인 경우 : 계속 공백만 이어지는 경우. 앞쪽 단어가 없음
			if (len) {
				pWord=(TCHAR *)calloc(len+1,1);
				memcpy(pWord,pFrame->buf+toff,len);
				if (SendMessage(GetParent(pFrame->hWnd),pFrame->UserBase+3,0,(LPARAM)pWord)) {
					Invalidate(pFrame->FindParaStart(toff));
				}
				free(pWord);
			}
		}
	}
}

int CApiEditText::GetAutoClipName()
{
	int toff;
	
	// 앞쪽의 공백 아닌 문자를 찾음. 구분자 개념이 없으므로 GetPrevWord 등을 쓸 수 없음
	for (toff=off-2;;toff--) {
		if (AeIsWhiteSpace(pFrame->buf[toff]) || pFrame->buf[toff] == '\n') {
			toff++;
			break;
		}
		if (toff==0) {
			break;
		}
	}
	return toff;
}

// 직전의 단어를 Text 상용구로 대체한다.
void CApiEditText::ReplaceClipText(TCHAR *Text)
{
	int toff;

	toff=GetAutoClipName();
	pFrame->StartUndoGroup(23);
	Delete(toff,off-toff);
	off=toff;
	InsertClipText(Text);
	pFrame->EndUndoGroup();
}

// 스크롤 상태를 고려하여 캐럿을 숨기거나 보인다.
void CApiEditText::ManageCaret()
{
	POINT pt;
	GetXYFromOff(off,(int &)pt.x,(int &)pt.y);
	pt.y-=yPos;
	pt.x-=xPos;

	// 포맷팅 영역을 벗어나면 숨긴다.
	if (PtInRect(&frt,pt)==FALSE) {
		if (bHideCaret==FALSE) {
			HideCaret(hWnd);
			bHideCaret=TRUE;
		}
	} else {
		ShowCaret(hWnd);
		bHideCaret=FALSE;
	}
	// 캐럿이 아직 보이는 상태면 이동한다.
	if (bHideCaret==FALSE) {
		SetCaret(FALSE,FALSE);
	}
}

void CApiEditText::DrawRuler(HWND hWnd,HDC hdc)
{
	TCHAR szTemp[10];
	RECT crt,rrt;
	HBRUSH hBrush;
	HPEN hPen,OldPen;
	HFONT hOldFont;
	int i,x;

	GetClientRect(hWnd,&crt);
	hBrush=CreateSolidBrush(pFrame->GetColor(AECOLOR_RULER));
	SetRect(&rrt,MarginWidth,0,crt.right-SBW,RULERHEIGHT);
	FillRect(hdc,&rrt,hBrush);
	hPen=CreatePen(PS_SOLID,1,RGB(128,128,128));
	OldPen=(HPEN)SelectObject(hdc,hPen);
	MoveToEx(hdc,MarginWidth,RULERHEIGHT-1,NULL);
	LineTo(hdc,crt.right-SBW,RULERHEIGHT-1);
	hOldFont=(HFONT)SelectObject(hdc,CApiEdit::hLineNumFont);
	SetTextAlign(hdc,TA_CENTER);
	for (i=0;;i++) {
		x=frt.left+FontWidth*i;
		MoveToEx(hdc,x,RULERHEIGHT-1,NULL);
		if (i%10==0) {
			wsprintf(szTemp,"%d",i);
			TextOut(hdc,x,2,szTemp,lstrlen(szTemp));
		}
		if (i%5==0) {
			LineTo(hdc,x,RULERHEIGHT-9);
		} else {
			LineTo(hdc,x,RULERHEIGHT-5);
		}
		// 출력한 후 좌표 점검해야 마지막 눈금까지 출력된다.
		if (x > crt.right-SBW) {
			break;
		}
	}
	SetTextAlign(hdc,TA_LEFT);
	SelectObject(hdc,hOldFont);
	DeleteObject(SelectObject(hdc,OldPen));
	DeleteObject(hBrush);
}

int CApiEditText::GetCharYPos()
{
	switch (pFrame->VAlign) {
	case 0:
		return 0;
	default:
	case 1:
		return (LineHeight-FontHeight)/2;
	case 2:
		return LineHeight-FontHeight;
	}
}

void CApiEditText::DrawSelFrame(HDC hdc)
{
	HRGN hRgn;
	int x1,y,x2;
	int SelFirst, SelSecond;
	HBRUSH hBrush;
	int rs,re,i;
	int ss,se;
	RECT rt;
	HRGN rgn;
	BOOL OldbLineEnd;
	int BkExtraX;

	BkExtraX=ExtraX;
	ExtraX=0;
	ignoreVirt++;

	OldbLineEnd=bLineEnd;
	SelFirst=min(SelStart,SelEnd);
	SelSecond=max(SelStart,SelEnd);
	bLineEnd=FALSE;
	GetRCFromOff(SelFirst,rs,i);
	bLineEnd=TRUE;
	GetRCFromOff(SelSecond,re,i);
	hRgn=CreateRectRgn(0,0,0,0);

	for (i=rs;i<=re;i++) {
		if (bColumnMode) {
			GetSelInLine(i,ss,se);
		} else {
			if (i==rs) {
				ss=SelFirst;
			} else {
				ss=pLine[i].Start;
			}
			if (i==re) {
				se=SelSecond;
			} else {
				se=pLine[i].End;
			}
		}
		bLineEnd=FALSE;
		GetXYFromOff(ss,x1,y);
		bLineEnd=TRUE;
		GetXYFromOff(se,x2,y);
		if (!bColumnMode && pFrame->IsEnterCode(se) && se >= SelFirst && se < SelSecond) {
			if (pFrame->TestStyle(AES_SHOWENTER)) {
				switch (pFrame->GetShowEnterType()) {
				case 0:
					x2+=SpaceWidth*2;
					break;
				case 1:
					x2+=SpaceWidth*3;
					break;
				case 2:
					x2+=SpaceWidth*3;
					break;
				}
			} else {
				x2+=SpaceWidth;
			}
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
	bLineEnd=OldbLineEnd;
	ignoreVirt--;
	ExtraX=BkExtraX;
}

void CApiEditText::DrawBackBit(HDC hdc,int nLine)
{
	int sLine;
	HDC hMemDC;
	HBITMAP OldBitmap;
	BITMAP bit;
	int bx,by,ys,xs;
	int ax,ay,bs;

	hMemDC=CreateCompatibleDC(hdc);
	OldBitmap=(HBITMAP)SelectObject(hMemDC,pFrame->hBackBit);
	GetObject(pFrame->hBackBit,sizeof(BITMAP),&bit);
	bx=bit.bmWidth;
	by=bit.bmHeight;

	// 6번외에는 화면상의 줄 수를 기준으로 한다.
	if (pFrame->BackBitPos==AEBB_TILESCROLL) {
		sLine=nLine;
	} else {
		sLine=nLine-yPos/LineHeight;
	}

	switch (pFrame->BackBitPos) {
	case AEBB_LEFTTOP:
		if (sLine*LineHeight < by) {
			BitBlt(hdc,frt.left,0,frt.right,LineHeight,hMemDC,0,
				sLine*LineHeight,SRCCOPY);
		}
		break;
	case AEBB_RIGHTTOP:
		if (sLine*LineHeight < by) {
			BitBlt(hdc,max(frt.right-bx,frt.left),0,frt.right,LineHeight,hMemDC,0,
				sLine*LineHeight,SRCCOPY);
		}
		break;
	case AEBB_LEFTBOTTOM:
		// 비트맵 출력 시작 위치 = 포맷팅 영역 높이 - 비트맵 높이
		ys=max(0,(frt.bottom-frt.top)-by);
		// 현재줄의 시작 좌표가 비트맵 출력 시작 좌표 이상이면. 대응되는 비트맵 부분 출력
		if (sLine*LineHeight >= ys) {
			BitBlt(hdc,frt.left,0,frt.right,LineHeight,hMemDC,0,
				sLine*LineHeight-ys,SRCCOPY);
		// 끝 좌표가 비트맵 출력 좌표 이상이면(비트맵이 출력되는 첫줄에만 해당된다)
		} else if (sLine*LineHeight+LineHeight-1 >= ys) {
			BitBlt(hdc,frt.left,ys % LineHeight,frt.right,LineHeight,hMemDC,0,
				0,SRCCOPY);
		}
		break;
	case AEBB_RIGHTBOTTOM:
		ys=max(0,(frt.bottom-frt.top)-by);
		if (sLine*LineHeight >= ys) {
			BitBlt(hdc,max(frt.right-bx,frt.left),0,frt.right,LineHeight,hMemDC,0,
				sLine*LineHeight-ys,SRCCOPY);
		} else if (sLine*LineHeight+LineHeight-1 >= ys) {
			BitBlt(hdc,max(frt.right-bx,frt.left),ys % LineHeight,frt.right,LineHeight,hMemDC,0,
				0,SRCCOPY);
		}
		break;
	case AEBB_CENTER:
		ys=max(0,(frt.bottom-frt.top)-by)/2;
		xs=max(0,(frt.right-frt.left)-bx)/2;
		if (sLine*LineHeight >= ys) {
			BitBlt(hdc,xs,0,frt.right,LineHeight,hMemDC,0,
				sLine*LineHeight-ys,SRCCOPY);
		} else if (sLine*LineHeight+LineHeight-1 >= ys) {
			BitBlt(hdc,xs,ys % LineHeight,frt.right,LineHeight,hMemDC,0,
				0,SRCCOPY);
		}
		break;
	case AEBB_TILE:
	case AEBB_TILESCROLL:
		// by-bs : 실제 출력된 높이. 비트맵 출력 시작점 이후의 남은 픽셀수.
		for (ay=0;ay < LineHeight;ay+=(by-bs)) {
			// 출력을 시작할 비트맵의 수직 좌표
			if (ay==0) {
				bs=sLine*LineHeight % by;
			} else {
				bs=0;
			}
			// 수평으로 반복 출력
			for (ax=frt.left;;ax+=bx) {
				if (ax >= frt.right) {
					break;
				}
				BitBlt(hdc,ax,ay,ax+bx,LineHeight,hMemDC,0,
					bs,SRCCOPY);
			}
		}
		break;
	}
	
	SelectObject(hMemDC,OldBitmap);
	DeleteDC(hMemDC);
}

void CApiEditText::ProcessCursorInEditing()
{
	RECT crt;
	POINT pt;

	switch (pFrame->CursorEdit) {
	default:
	case 0:
		break;
	case 1:
		ShowCursorEdit(FALSE);
		break;
	case 2:
	case 3:
		GetClientRect(hWnd,&crt);
		ClientToScreen(hWnd,(LPPOINT)&crt);
		ClientToScreen(hWnd,(LPPOINT)&crt.right);
		if (pFrame->CursorEdit==2) {
			SetCursorPos(crt.right-25,crt.top+8);
		} else {
			SetCursorPos(crt.right-25,crt.bottom-33);
		}
		break;
	case 4:
		GetCaretPos(&pt);
		ClientToScreen(hWnd,&pt);
		SetCursorPos(pt.x+FontWidth*5,pt.y+LineHeight*2);
		break;
	}
}

void CApiEditText::ShowCursorEdit(BOOL bShow)
{
	if (bShow) {
		if (!bShowCursor) {
			ShowCursor(TRUE);
			bShowCursor=TRUE;
		}
	} else {
		if (bShowCursor) {
			ShowCursor(FALSE);
			bShowCursor=FALSE;
		}
	}
}

// nPos 자리에 공백 조정. 리턴값은 삽입된 바이트 수
int CApiEditText::AdjustSpace(int &nPos,BOOL bAdd)
{
	int toff;
	int s,e;

	// 공백 시작 오프셋
	for (toff=nPos-1;;toff--) {
		if (toff==0) {
			s=toff;
			break;
		}
		if (pFrame->buf[toff]!= ' ') {
			s=toff+1;
			break;
		}
	}
	// 공백 끝 오프셋
	for (toff=s;;toff++) {
		if (toff==pFrame->doclen) {
			e=toff;
			break;
		}
		if (pFrame->buf[toff]!=' ') {
			e=toff;
			break;
		}
	}

	// 문단 처음이고 공백이 있을 경우. 모든 공백 삭제
	if (e-s > 0 && pFrame->FindParaStart(s)==s) {
		Delete(s,e-s);
		nPos=s;
		return -(e-s);
	}
	// 1보다 크면 하나만 남기고 삭제
	if (e-s > 1) {
		Delete(s+1,e-s-1);
		nPos=s+1;
		return -(e-s-1);
	}
	// 공백이 없고 bAdd이면 공백 하나 삽입
	if (bAdd && e-s==0) {
		Insert(nPos," ",1);
		nPos++;
		return 1;
	}
	// 원래 공백이 하나이거나 공백이 없었던 경우.
	return 0;
}

// 양쪽에 공백 정리.
int CApiEditText::AdjustSpaceInsert(int &nPos1,int &nPos2)
{
	int nIns;

	// 문단 처음은 뒤쪽만 조정하면 된다.
	if (nPos1==pFrame->FindParaStart(nPos1)) {
		nIns=AdjustSpace(nPos2,TRUE);
		return nIns;
	}

	// 양쪽에 공백이 없으면 조정할 필요가 없음.
	if ((nPos1==0 || pFrame->buf[nPos1-1]!=' ') && pFrame->buf[nPos2]!=' ')
		return 0;

	// 앞쪽 조정. nPos2 같이 이동
	nIns=AdjustSpace(nPos1,TRUE);
	nPos2+=nIns;

	// 뒤쪽 조정. nPos2는 조정 후의 공백 앞 위치
	nIns+=AdjustSpace(nPos2,TRUE);
	return nIns;
}

// 선택된 문단의 시작, 끝 오프셋을 구해준다.
void CApiEditText::SelectParas(int &SelFirst,int &SelSecond,int &st,int &ed)
{
	int pr1,pr2,pc;
	int Line;

	// 선택 영역이 없으면 문서 전체를 대상으로 한다.
	if (SelStart==SelEnd) {
		SelFirst=0;
		SelSecond=pFrame->doclen;
	} else {
		SelFirst=min(SelStart,SelEnd);
		SelSecond=max(SelStart,SelEnd);
	}

	// 시작 문단의 첫 오프셋
	GetParaFromOff(SelFirst,pr1,pc);
	Line=GetParaFirstLine(pr1);
	st=pLine[Line].Start;

	// 끝 문단의 끝 오프셋. 선택 끝이 문단 선두이면 이 문단은 제외된다.
	GetParaFromOff(SelSecond,pr2,pc);
	Line=GetParaFirstLine(pr2);
	if (SelSecond==pLine[Line].Start) {
		pr2--;
	}
	Line=GetParaLastLine(pr2);
	ed=pLine[Line].End;
}

#define BEFOREARRANGEPARA 	\
	int SelFirst, SelSecond;\
	int st,ed,nDiff=0;\
	TCHAR *src,*dest,*s,*d;\
	SelectParas(SelFirst,SelSecond,st,ed);\
	s=src=(TCHAR *)malloc(ed-st);\
	d=dest=(TCHAR *)malloc(ed-st);\
	memcpy(src,pFrame->buf+st,ed-st);

#define ENDARRANGEPARA(grpid) \
	SelSecond-=nDiff;\
	pFrame->StartUndoGroup(grpid);\
	Delete(st,ed-st);\
	Insert(st,dest,d-dest);\
	pFrame->EndUndoGroup();\
	if (SelStart == SelEnd) {\
		off=SelStart=SelEnd=st;\
	} else if (SelStart < SelEnd) {\
		off=SelEnd=SelSecond;\
		SelStart=SelFirst;\
	} else {\
		off=SelEnd=SelFirst;\
		SelStart=SelSecond;\
	}\
	if (pFrame->IsInDBCS(off)) off=pFrame->GetPrevOff(off);\
	if (pFrame->IsInDBCS(SelStart)) SelStart=pFrame->GetPrevOff(SelStart);\
	if (pFrame->IsInDBCS(SelEnd)) SelEnd=pFrame->GetPrevOff(SelEnd);\
	Invalidate(st);\
	SetCaret();\
	free(src);\
	free(dest);

void CApiEditText::MergePara(int MultiEnterType,TCHAR *szRep)
{
	BEFOREARRANGEPARA
	BOOL bPrevEnter=FALSE;

	for (;;) {
		ReallocBuf(dest,d,1024,2048);
		if (s[0]=='\r' && s+1 < src+ed-st && s[1]=='\n') {
			if (MultiEnterType!=0 && bPrevEnter) {
				if (MultiEnterType==1) {
					s+=2;
					nDiff+=2;
				} else {
					*d++=*s++;
					*d++=*s++;
				}
			} else {
				lstrcpy(d,szRep);
				d+=lstrlen(szRep);
				s+=2;
				nDiff+=2-lstrlen(szRep);
				bPrevEnter=TRUE;
			}
		} else {
			if (IsDBCSLeadByte(*s)) {
				*d++=*s++;
			}
			*d++=*s++;
			bPrevEnter=FALSE;
		}
		if (s==src+ed-st) {
			break;
		}
	}

	ENDARRANGEPARA(28)
}

void CApiEditText::SplitPara(int type, int num, BOOL bRemoveSpace, int iIndent)
{
	BEFOREARRANGEPARA
	int acwidth;
	int sl,el,l;
	int *arWrap,idx;
	int i;

	switch (type) {
	case 0:
		for (;;) {
			ReallocBuf(dest,d,1024,2048);
			if (IsDBCSLeadByte(*s)) {
				*d++=*s++;
			}
			*d++=*s++;
			if (s[-1]=='.') {
				*d++='\r';
				*d++='\n';
				nDiff-=2;
				// 마침표 다음의 공백은 건너 뜀
				if (bRemoveSpace) {
					while (AeIsWhiteSpace(*s)) {
						s++;
						nDiff++;
					}
				}
			}
			if (s==src+ed-st) {
				break;
			}
		}
		break;
	// 중간에 문단 구조가 바뀌기 때문에 pLine 배열은 참조할 수 없음
	case 1:
	case 2:
		ReallocBuf(dest,d,1024,2048);
		acwidth=0;
		// 문단 처음은 항상 들여쓰기 한다.
		if (iIndent > 0) {
			for (i=0;i<iIndent;i++) {
				*d++=' ';
				nDiff--;
				if (type==1) {
					acwidth++;
				} else {
					acwidth+=GetCharWidth(" ",1);
				}
			}
			// 공백은 건너 뛰어야 한다.
			while (AeIsWhiteSpace(*s)) {
				s++;
				nDiff++;
			}
		}
		for (;;) {
			ReallocBuf(dest,d,1024,2048);
			if (s[0]=='\r' && s+1 < src+ed-st && s[1]=='\n') {
				*d++=*s++;
				*d++=*s++;
				acwidth=0;
				// 들여쓰기 한다.
				if (iIndent > 0) {
					for (i=0;i<iIndent;i++) {
						*d++=' ';
						nDiff--;
						if (type==1) {
							acwidth++;
						} else {
							acwidth+=GetCharWidth(" ",1);
						}
					}
					// 공백은 건너 뛰어야 한다. 들여쓰기를 하지 않을 때는 그냥 둔다
					while (AeIsWhiteSpace(*s)) {
						s++;
						nDiff++;
					}
				}
			} else {
				// 현재 문자까지 더했을 때의 폭을 계산한다.
				if (type==1) {
					if (IsDBCSLeadByte(*s)) {
						acwidth+=2;
					} else {
						acwidth++;
					}
				} else {
					if (*s=='\t') {
						acwidth=(acwidth/TabSize+1)*TabSize;
					} else {
						if (IsDBCSLeadByte(*s)) {
							acwidth+=GetCharWidth(s,2);
						} else {
							acwidth+=GetCharWidth(s,1);
						}
					}
				}
				// 폭을 넘었을 때 분할. 같을 때는 분할하지 않음
				if (acwidth > num) {
					*d++='\r';
					*d++='\n';
					acwidth=0;
					nDiff-=2;
					// 삽입된 개행코드 다음의 공백은 건너 뜀
					if (bRemoveSpace) {
						while (AeIsWhiteSpace(*s)) {
							s++;
							nDiff++;
						}
					}
					// 내어 쓰기한다.
					if (iIndent < 0) {
						for (i=0;i>iIndent;i--) {
							*d++=' ';
							nDiff--;
							if (type==1) {
								acwidth++;
							} else {
								acwidth+=GetCharWidth(" ",1);
							}
						}
					}
				// 넘지 않았으면 문자 복사
				} else {
					if (IsDBCSLeadByte(*s)) {
						*d++=*s++;
					}
					*d++=*s++;
				}
			}
			if (s==src+ed-st) {
				break;
			}
		}
		break;
	case 3:
		GetRCFromOff(st,sl,l);
		GetRCFromOff(ed,el,l);
		arWrap=(int *)malloc((el-sl)*sizeof(int));
		// el줄은 문단 끝이므로 볼 필요없음. l+1은 항상 유효함
		for (l=sl,idx=0;l<el;l++) {
			if (pLine[l+1].nLine) {
				arWrap[idx]=pLine[l].End;
				idx++;
			}
		}

		for (idx=0;;) {
			ReallocBuf(dest,d,1024,2048);
			// 현재 오프셋이 자동 개행된 오프셋과 같으면
			if (s-src+st == arWrap[idx]) {
				*d++='\r';
				*d++='\n';
				nDiff-=2;
				idx++;
				// 마침표 다음의 공백은 건너 뜀
				if (bRemoveSpace) {
					while (AeIsWhiteSpace(*s)) {
						s++;
						nDiff++;
					}
				}
			} else {
				if (IsDBCSLeadByte(*s)) {
					*d++=*s++;
				}
				*d++=*s++;
			}
			if (s==src+ed-st) {
				break;
			}
		}
		free(arWrap);
		break;
	}

	ENDARRANGEPARA(29)
}

void CApiEditText::AlignPara(int orient, int type, int num)
{
	BEFOREARRANGEPARA
	int acwidth,extra;
	int i;
	TCHAR *s2;

	for (;;) {
		acwidth=0;
		// 앞쪽 공백은 건너뛴다.
		while (AeIsWhiteSpace(*s)) {
			s++;
			nDiff++;
		}
		s2=s;
		// 문단 끝까지 폭 계산
		for (;;) {
			if ((s[0]=='\r' && s+1 < src+ed-st && s[1]=='\n') || (s==src+ed-st)) {
				break;
			}
			if (type==1) {
				if (IsDBCSLeadByte(*s)) {
					acwidth+=2;
					s++;
				} else {
					acwidth++;
				}
			} else {
				if (*s=='\t') {
					acwidth=(acwidth/TabSize+1)*TabSize;
				} else {
					if (IsDBCSLeadByte(*s)) {
						acwidth+=GetCharWidth(s,2);
						s++;
					} else {
						acwidth+=GetCharWidth(s,1);
					}
				}
			}
			s++;
		}
		// 기준폭보다 더 길면 그대로 복사
		if (acwidth >= num || orient==0) {
			for (;s2!=s;) {
				*d++=*s2++;
			}
		} else {
			ReallocBuf(dest,d,1024,2048);
			extra=(num-acwidth);
			if (type==2) {
				extra=extra/GetCharWidth(" ",1);
			}
			switch (orient) {
			case 1:
				// 중앙:여분 공백의 절반만 삽입
				for (i=0;i<extra/2;i++) {
					*d++=' ';
					nDiff--;
				}
				break;
			case 2:
				// 오른쪽:여분 공백 삽입 후 복사
				for (i=0;i<extra;i++) {
					*d++=' ';
					nDiff--;
				}
				break;
			}
			for (;s2!=s;) {
				*d++=*s2++;
			}
		}
		if (s==src+ed-st) {
			break;
		}
		// 문단 끝의 개행 코드 복사. 마지막 문단에는 이 개행 코드가 없음
		*d++=*s++;
		*d++=*s++;
	}

	ENDARRANGEPARA(30)
}

// 문단 크기순 정렬
void CApiEditText::SortPara(int asc, BOOL Case, BOOL Unique)
{
	BEFOREARRANGEPARA
	struct stPara {
		TCHAR *ptr;			// 문단 시작 위치
		int len;			// 문단의 길이
		int order;			// 순서값
	};
	stPara *arPara;
	int nPara,i,j;
	TCHAR *p;
	int result,order;
	int same;

	// src에는 널 문자분, dest는 마지막 개행 코드분을 추가할당한다.
	s=src=(TCHAR *)realloc(src,ed-st+1);
	src[ed-st]=0;
	d=dest=(TCHAR *)realloc(dest,ed-st+2);

	// 문단 개수를 구하고 arPara를 할당한다.
	for (p=src,nPara=1;*p;p++) {
		if (*p=='\r' && *(p+1)=='\n') {
			nPara++;
			*p++=0;
			*p=0;
		}
	}
	arPara=(stPara *)malloc(nPara*sizeof(stPara));

	// 각 문단의 시작 위치와 길이를 구한다.
	for (i=0,p=src;i<nPara;i++) {
		arPara[i].ptr=p;
		arPara[i].len=lstrlen(p);
		p+=(arPara[i].len+2);
	}

	// 각 문단의 순서를 구한다.
	for (i=0;i<nPara;i++) {
		order=0;
		for (j=0;j<nPara;j++) {
			if (i == j) continue;
			if (Case) {
				result=strcmp(arPara[i].ptr,arPara[j].ptr);
			} else {
				result=strcmpi(arPara[i].ptr,arPara[j].ptr);
			}
			if (asc) {
				if (result < 0) order++;
			} else {
				if (result > 0) order++;
			}
		}
		arPara[i].order=order;
	}

	// 순서대로 dest로 복사한다. 같은 순위가 있을 수 있고 빠지는 순위도 있다.
	for (i=0;i<nPara;i++) {
		for (j=0,same=0;j<nPara;j++) {
			if (arPara[j].order == i) {
				same++;
				// 같은 순위가 둘 이상이고 중복 제거 옵션이 설정되어 있는 경우
				if (same > 1 && Unique) {
					nDiff+=(arPara[j].len+2);
				} else {
					memcpy(d,arPara[j].ptr,arPara[j].len);
					d+=arPara[j].len;
					*d++='\r';
					*d++='\n';
				}
			}
		}
	}
	// 마지막의 불필요한 개행 코드 제거
	d-=2;

	free(arPara);
	ENDARRANGEPARA(31)
}

void CApiEditText::ArrangeEmptyPara(int type, int num)
{
	BEFOREARRANGEPARA
	int nEmpty,i;

	for (;;) {
		ReallocBuf(dest,d,1024,2048);
		// 개행 코드. 일단 복사 후 빈줄 수를 맞춤
		if (s[0]=='\r' && s+1 < src+ed-st && s[1]=='\n') {
			*d++=*s++;
			*d++=*s++;
			// 빈 줄 수 계산
			for (nEmpty=0;;) {
				while (AeIsWhiteSpace(*s)) {
					s++;
					nDiff++;
				}
				if (s[0]=='\r' && s+1 < src+ed-st && s[1]=='\n') {
					nEmpty++;
					s+=2;
					nDiff+=2;
				} else {
					break;
				}
			}
			
			// 빈줄수 계산
			switch (type) {
			case 0:
				i=max(nEmpty,num);
				break;
			case 1:
				i=num;
				break;
			case 2:
				i=min(nEmpty,num);
				break;
			}

			// 계산된 빈줄 수 삽입
			for (;i>0;i--) {
				*d++='\r';
				*d++='\n';
				nDiff-=2;
			}
		} else {
			if (IsDBCSLeadByte(*s)) {
				*d++=*s++;
			}
			*d++=*s++;
		}
		if (s==src+ed-st) {
			break;
		}
	}

	ENDARRANGEPARA(32)
}

void CApiEditText::ArrangeVerticalCenter(int nPos)
{
	int x,y;

	GetXYFromOff(nPos,x,y);
	yPos=max(0,((y-frt.top)-(frt.bottom-frt.top)/2)/LineHeight*LineHeight);
	if (IsWindowEnabled(hVertScroll)) {
		SetScrollPos(hVertScroll, SB_CTL, yPos, TRUE);
	}
	Invalidate(-1);
	SetCaret();
}

BOOL CApiEditText::FindTextSub(int nPos, TCHAR *what, DWORD dwFlag,int &s, int &e)
{
	int nStart;
	TCHAR *pFound;
	BOOL bWrap;
	TCHAR *pWhat;
	int len;

	pWhat=(TCHAR *)malloc(lstrlen(what)+1);
	if (strchr(what,'\\')==NULL) {
		lstrcpy(pWhat,what);
		len=lstrlen(pWhat);
	} else {
		len=AeEscapeString(what,pWhat);
	}

	if (nPos == -1) {
		if (SelStart==SelEnd) {
			nStart=off;
		} else {
			if (dwFlag & AE_FIND_UP) {
				nStart=min(SelStart,SelEnd);
			} else {
				nStart=max(SelStart,SelEnd);
			}
		}
	} else {
		nStart=nPos;
	}

	bWrap=((dwFlag & AE_FIND_WRAP)!=0);
	if (dwFlag & AE_FIND_UP) {
		nStart=nStart-len;
		if (nStart<0) {
			if (bWrap) {
				bWrap=FALSE;
				nStart=max(0,pFrame->doclen-len);
			} else {
				free(pWhat);
				return FALSE;
			}
		}
	}

	for (;;) {
		pFound=FindString(pFrame->buf,pFrame->doclen,nStart,pWhat,len,(dwFlag & AE_FIND_UP)==0,
			(dwFlag & AE_FIND_MATCHCASE)!=0);
		if (pFound) {
			// 텍스트 뷰에서 DBCS 중간에 걸치면 검색 실패한 것으로 간주한다.
			if (ViewType != HEXA_VIEW && pFrame->IsInDBCS(pFound-pFrame->buf)) {
				if (dwFlag & AE_FIND_UP) {
					nStart=pFound-pFrame->buf-1;
				} else {
					nStart=pFound-pFrame->buf+len;
				}
				continue;
			}
			// 단어 단위로 검색할 경우 양쪽에 공백이 있어야 한다.
			if (dwFlag & AE_FIND_WHOLEWORD) {
				if ((pFound==pFrame->buf || IsDelimiter(pFound-pFrame->buf-1)) && 
					IsDelimiter(pFound-pFrame->buf+len)) {
					break;
				} else {
					if (dwFlag & AE_FIND_UP) {
						nStart=pFound-pFrame->buf-1;
					} else {
						nStart=pFound-pFrame->buf+len;
					}
					continue;
				}
			}
			break;
		}

		if (bWrap) {
			bWrap=FALSE;
			if ((dwFlag & AE_FIND_UP)==0) {
				nStart=0;
			} else {
				nStart=max(0,pFrame->doclen-len);
			}
			continue;
		}
		break;
	}

	free(pWhat);
	if (pFound) {
		s=pFound-pFrame->buf;
		e=s+len;
		return TRUE;
	} else {
		s=e=-1;
		return FALSE;
	}
}

void CApiEditText::GetParaText(int nPara,TCHAR *Text,int len)
{
	int ts,te;
	int i;
	TCHAR *p;

	ts=GetOffFromPara(nPara,0);
	te=pLine[GetParaLastLine(nPara)].End;

	for (p=Text,i=ts;i<te;i++) {
		if (pFrame->buf[i]=='\t') {
			*p++=' ';
			*p++=' ';
			*p++=' ';
			*p++=' ';
		} else {
			if (pFrame->IsDBCS(i)) {
				*p++=pFrame->buf[i++];
			}
			*p++=pFrame->buf[i];
		}
		if (p-Text >= len-2) {
			break;
		}
	}
	*p=0;
}
