#ifndef __APIEDITHEXA_H
#define __APIEDITHEXA_H

class CApiEditHexa : public CApiEditText
{
	friend class CApiEdit;
	friend class CParse;
	friend class CParseCpp;
	friend class CParseNull;
	friend class CParseHtml;
	friend class CParseSql;
protected:
	int bpl;
	BOOL bCapital;
	int nHexSep;
	int bCol,bGap;

public:
	CApiEditHexa();

	BOOL Create(int x,int y,int w,int h,DWORD style,UINT id,HWND hParent);
	LRESULT OnMessage(UINT iMessage,WPARAM wParam,LPARAM lParam);

	BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
	void OnChar(HWND hWnd, TCHAR ch, int cRepeat);
	void OnKey(HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
	void OnPaint(HWND hWnd);
	void OnSetFocus(HWND hWnd, HWND hwndOldFocus);
	void OnKillFocus(HWND hWnd, HWND hwndNewFocus);
	void OnDestroy(HWND hWnd);
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
	void OnMouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam);

	virtual void SetCaret(BOOL bUpdatePrevX=TRUE, BOOL bScrollToCaret=TRUE);
	virtual void Insert(int nPos, TCHAR *str, int len, BOOL bRec=TRUE);
	virtual void Delete(int nPos, int nCount, BOOL bRec=TRUE);
	virtual int GetRowCount();
	virtual void GetRCFromOff(int nPos, int &r, int &c);
	virtual int GetOffFromRC(int r, int c);
	virtual void GetXYFromOff(int nPos, int &x, int &y);
	virtual int GetOffFromXY(int x, int y);
	virtual void SetWrap(int aWrap) { nWrap=aWrap; }
	virtual void UpdateScrollInfo();
	virtual int DrawLine(HDC hdc, int Line);
	virtual BOOL IsInSelection(int x,int y) { return FALSE; }
	virtual void SetFont(LOGFONT *alogfont);
	virtual int GetInfoFromOff(int Type, int nPos=-1);
	virtual int GetDocInfo(int type);
	virtual void BeginPrint(Ae_PrintInfo *pi);
	virtual BOOL PrintPage(Ae_PrintInfo *pi,int nPage);
	virtual void EndPrint(Ae_PrintInfo *pi);
	virtual void InitView();
	int GetDigitWidth(int nPos);
	void EndComp();
	int GetBpl() { return bpl; }
	void SetBpl(int abpl);
	BOOL GetCapital() { return bCapital; }
	void SetCapital(int aCapital);
	int GetHexSep() { return nHexSep; }
	void SetHexSep(int aHexSep);
	void CopyDump();
	void ProcessKey(UINT vk,BOOL bControl,BOOL bShift);
	void Select(int n);
	void Move(int n, BOOL bSelect);
	virtual void DrawSelFrame(HDC hdc);
};

#endif // __APIEDITHEXA_H
