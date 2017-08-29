#ifndef __APIEDITTEXT_H
#define __APIEDITTEXT_H

class CApiEditText : public CWindow
{
	friend class CApiEdit;
	friend class CParse;
	friend class CParseCpp;
	friend class CParseNull;
	friend class CParseHtml;
	friend class CParseSql;
protected:
	BOOL bComp;
	int off;
	int FontHeight;
	int LineRatio;
	int LineHeight;
	int nWrap;
	int PrevX;
	BOOL bLineEnd;
	int yPos,xPos;
	int yMax,xMax;
	int FontWidth;
	int TabWidth;
	int TabSize;
	int SelStart, SelEnd;
	BOOL bCapture;
	int arChWidth[256];
	BYTE *arHanWidth;
	tagLine *pLine;
	int TotalLine;
	HBITMAP hBit;
	int MarginWidth;
	RECT frt;
	int arMark[MAXBOOKMARK];
	BOOL bSelLine;
	int SelStartLine;
	BOOL bDragSel;
	BOOL bOvr;
	int Linelen;
	int SumDelta;
	BOOL bHideCaret;
	LOGFONT logfont;
	int RightWrap;
	int ColMark;
	BOOL bPrevCompoGroup;
	CParse *Parser;
	BOOL bFindSelect;
	HFONT hMainFont;
	BOOL bVertSplit;
	BOOL bHorzSplit;
	HWND hVertScroll;
	HWND hHorzScroll;
	HWND hBtnSetting;
	int iDrag;
	int OldSplit;
	BOOL bVirtualPos;
	int ExtraX;
	int ignoreVirt;
	BOOL bColumnMode;
	int SelExtraX;
	LOGFONT Bklogfont;
	Ae_SelTable *SelTable;
	int Fwidth, Fheight;
	int SpaceWidth;
	BOOL bShowCursor;
	int *lpDx;

public:
	CApiEditText();
	CApiEdit *pFrame;
	int ViewType;
	CMcRecord AeMc;

	BOOL Create(int x,int y,int w,int h,DWORD style,UINT id,HWND hParent);
	LRESULT OnMessage(UINT iMessage,WPARAM wParam,LPARAM lParam);

	BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
	void OnChar(HWND hWnd, TCHAR ch, int cRepeat);
	void OnKey(HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
	void OnPaint(HWND hWnd);
	void OnSetFocus(HWND hWnd, HWND hwndOldFocus);
	void OnKillFocus(HWND hWnd, HWND hwndNewFocus);
	void OnDestroy(HWND hWnd);
	LRESULT OnImeComposition(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnImeChar(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	void OnMouseMove(HWND hWnd, int x, int y, UINT keyFlags);
	void OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags);
	void OnTimer(HWND hWnd, UINT id);
	void OnSize(HWND hWnd, UINT state, int cx, int cy);
	void OnHScroll(HWND hWnd, HWND hwndCtl, UINT code, int pos);
	void OnVScroll(HWND hWnd, HWND hwndCtl, UINT code, int pos);
	void OnContextMenu(HWND hWnd, HWND hwndContext, UINT xPos, UINT yPos);
	virtual void OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify);
	BOOL OnSetCursor(HWND hWnd, HWND hwndCursor, UINT codeHitTest, UINT msg);
	UINT OnGetDlgCode(HWND hWnd, LPMSG lpmsg);
	void OnMouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam);

	virtual int GetCharWidth(TCHAR *ch, int len);
	virtual void SetCaret(BOOL bUpdatePrevX=TRUE, BOOL bScrollToCaret=TRUE);
	virtual void Insert(int nPos, TCHAR *str, int len, BOOL bRec=TRUE);
	virtual void Delete(int nPos, int nCount, BOOL bRec=TRUE);
	virtual void GetLine(int Line, int &s, int &e);
	virtual int GetRowCount();
	virtual void GetRCFromOff(int nPos, int &r, int &c);
	virtual int GetOffFromRC(int r, int c);
	virtual void GetXYFromOff(int nPos, int &x, int &y);
	virtual void SetWrap(int aWrap);
	virtual int GetXPosOnLine(int r,int DestX);
	virtual void UpdateScrollInfo();
	virtual int DrawLine(HDC hdc, int Line);
	virtual void DrawLineSub(HDC hdc,int Line,BOOL bCurLine);
	virtual void DrawSegment(HDC hdc, int &x, int y, int SegOff, int len, BOOL ignoreX, 
		COLORREF fore, COLORREF back);
	virtual int GetOffFromXY(int x, int y);
	virtual BOOL IsDelimiter(int nPos);
	virtual int GetPrevWord(int nPos);
	virtual int GetNextWord(int nPos);
	virtual void ClearSelection();
	virtual void ExpandSelection(int Start, int End);
	virtual BOOL DeleteSelection();
	virtual void PrepareCharWidth(HDC hdc);
	virtual int MyGetTextExtent(TCHAR *text, int len);
	virtual void UpdateLineInfo(int nPos=-1, int nCount=-1);
	virtual void Invalidate(int Pos1, int Pos2=-1);
	virtual void ClearBookmark();
	virtual void ToggleBookmark(int Para, int Mark);
	virtual void GotoBookmark(int Mark);
	virtual void GotoLine(int Line, int Col=0, int Type=0);
	virtual int FindBookmark(int Para);
	virtual void DisplayTab(HDC hdc, int x1, int x2, int y,COLORREF back);
	virtual void DisplayEnter(HDC hdc, int x, int y,COLORREF back);
	virtual void DisplaySpace(HDC hdc, int x, int y, TCHAR *text, int len);
	virtual void GetNowWord(int nPos, int &s, int &e);
	virtual BOOL IsInSelection(int x,int y);
	virtual void CopyString(BOOL bCopy, int from, int &to, int len, BOOL bRec=TRUE);
	virtual void GetSelect(int &s, int &e, BOOL bRegular=TRUE);
	virtual void SetSelect(int s, int e, BOOL bFind);
	virtual int GetLineRatio() { return LineRatio; }
	virtual void SetLineRatio(int aLineHeight);
	virtual int GetWrap() { return nWrap; }
	virtual int GetTabWidth() { return TabWidth; }
	virtual void SetTabWidth(int aTabWidth);
	virtual void GetFont(LOGFONT *alogfont);
	virtual void SetFont(LOGFONT *alogfont);
	virtual void SetDefaultSetting();
	virtual int GetRightWrap() { return RightWrap; }
	virtual void SetRightWrap(int aRightWrap);
	virtual int GetColMark() { return ColMark; }
	virtual void SetColMark(int aColMark);
	virtual CParse* GetParser() { return Parser; }
	virtual void SetParser(int ParseType);
	virtual BOOL FindText(int nPos, TCHAR *what, DWORD dwFlag);
	virtual int ReplaceText(int nPos, TCHAR *what, DWORD dwFlag, TCHAR *to);
	virtual int GetOffset() { return off; }
	virtual void GetSelRect(RECT &srt);
	virtual int GetInfoFromOff(int Type, int nPos=-1);
	virtual void FindParen(BOOL bSelect);
	virtual void GotoLastEditOff();
	virtual int GetDocInfo(int type);
	virtual BOOL GetInsMode() { return bOvr; }
	virtual void ChangeCase(int action);
	virtual void ConvertSpaceTab(BOOL bToSpace);
	virtual void RemoveExtraSpace();
	virtual void GetParaFromOff(int nPos, int &pr, int &pc);
	virtual int GetOffFromPara(int pr, int pc);
	virtual int GetParaFirstLine(int nPara);
	virtual int GetParaLastLine(int nPara);
	virtual void ProcessIndent(TCHAR ch);
	virtual BOOL IsParaEmpty(TCHAR *p);
	virtual int GetPrevPara(int nPos);
	virtual int GetIndentLevel(TCHAR *p);
	virtual void GetIndentString(int nPara,TCHAR *szIndent,int len);
	virtual void MakeIndentString(int nTab, TCHAR *str, int len);
	virtual BOOL IsPrevParaBlockOpen(int nPos);
	virtual void BlockIndent(BOOL bUnindent);
	virtual void InsertComment(BOOL bUnComment);
	virtual void InsertText(TCHAR *Text);
	virtual void CopyChar(int dir);
	virtual void BeginPrint(Ae_PrintInfo *pi);
	virtual BOOL PrintPage(Ae_PrintInfo *pi,int nPage);
	virtual void EndPrint(Ae_PrintInfo *pi);
	virtual void GetLine(int Line, int &s, int &e,Ae_PrintInfo *pi);
	virtual void InitView();
	virtual void SetOffset(int aoff);
	virtual BOOL GetVertSplit() { return bVertSplit; }
	virtual void SetVertSplit(BOOL aValue);
	virtual BOOL GetHorzSplit() { return bHorzSplit; }
	virtual void SetHorzSplit(BOOL aValue);
	virtual BOOL StartSplit(HWND hWnd,BOOL fDoubleClick,int x,int y);
	virtual BOOL Spliting(HWND hWnd,int x,int y);
	virtual BOOL EndSplit(HWND hWnd,int x,int y);
	virtual void AeTextOut(HDC hdc,int x,int y,TCHAR *str,int len);
	virtual int *GetBookmark() { return arMark; }
	virtual void SetBookmark(int *aMark);
	virtual BOOL GetVirtualPos() { return bVirtualPos; }
	virtual void SetVirtualPos(int aValue);
	virtual BOOL IsVirtualPosable();
	virtual BOOL InputExtraSpace(int &nPos);
	virtual BOOL GetColumnMode() { return bColumnMode; }
	virtual void SetColumnMode(int aMode);
	virtual void GetSelInLine(int Line,int &s,int &e);
	virtual int MakeSelTable();
	virtual void FreeSelTable();
	virtual void PasteColumn(int nPos,int aExtra,TCHAR *data);
	virtual void ColumnCopyString(BOOL bCopy,int to,int aExtra);
	virtual void ChangeCaseColumn(int action);
	virtual void ProcessKey(UINT vk,BOOL bControl,BOOL bShift);
	virtual void Select(int n);
	virtual void Move(int n, BOOL bSelect);
	virtual void Scroll(int nLine, int nCode=0);
	virtual void ProcessKeyRecord(UINT vk,BOOL bControl,BOOL bShift);
	virtual void EndComp();
	virtual void InsertClipText(TCHAR *Text);
	virtual void TestAutoClip(int ch);
	virtual int GetAutoClipName();
	virtual void ReplaceClipText(TCHAR *Text);
	virtual void ManageCaret();
	virtual void DrawRuler(HWND hWnd,HDC hdc);
	virtual int GetCharYPos();
	virtual void DrawSelFrame(HDC hdc);
	virtual void DrawBackBit(HDC hdc,int nLine);
	virtual void ProcessCursorInEditing();
	virtual void ShowCursorEdit(BOOL bShow);
	virtual int AdjustSpace(int &nPos,BOOL bAdd);
	virtual int AdjustSpaceInsert(int &nPos1,int &nPos2);
	virtual void SelectParas(int &SelFirst,int &SelSecond,int &st,int &ed);
	virtual void MergePara(int MultiEnterType,TCHAR *szRep);
	virtual void SplitPara(int type, int num, BOOL bRemoveSpace, int iIndent);
	virtual void AlignPara(int orient, int type, int num);
	virtual void SortPara(int asc, BOOL Case, BOOL Unique);
	virtual void ArrangeEmptyPara(int type, int num);
	virtual void ArrangeVerticalCenter(int nPos);
	int GetYPos() { return yPos; }
	virtual BOOL FindTextSub(int nPos, TCHAR *what, DWORD dwFlag,int &s, int &e);
	void GetParaText(int nPara,TCHAR *Text,int len);
};

#endif // __APIEDITTEXT_H
