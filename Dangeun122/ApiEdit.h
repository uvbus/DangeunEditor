#ifndef __APIEDIT_H
#define __APIEDIT_H

#define IDM_AE_CUT 40001
#define IDM_AE_COPY 40002
#define IDM_AE_PASTE 40003
#define IDM_AE_SELALL 40004
#define IDM_AE_COPYAPPEND 40005
#define IDM_AE_CUTAPPEND 40006
#define IDM_AE_COPYDUMP 40007
#define MAXBOOKMARK 266

#define AE_FIND_UP 0x1
#define AE_FIND_WHOLEWORD 0x2
#define AE_FIND_MATCHCASE 0x4
#define AE_FIND_WRAP 0x8
#define AE_FIND_CLOSE 0x10
#define AE_FIND_RECURSIVE 0x20
#define AE_FIND_SHORTPATH 0x40

enum { AE_INFO_ROW, AE_INFO_COL, AE_INFO_PARA, AE_INFO_LINE, AE_INFO_PARACOL };

#define AEN_CHANGE 1
#define AEN_MOVE 2
#define AEN_INSMODE 3
#define AEN_CHGMODI 4
#define AEN_CHGVIEW 5

#define AeIsWhiteSpace(c) ((c)==' ' || (c) == '\t')
#define AeIsDocEnd(p) ((p)-pFrame->buf == pFrame->doclen)
#define AeIsLineEnd(p) (pFrame->IsEnterCode(p) || AeIsDocEnd(p))

// 인코딩 방식 - 상위 워드
#define AE_ENC_UNKNOWN 0x0
#define AE_ENC_ANSI 0x1
#define AE_ENC_UNI 0x2
#define AE_ENC_UNIBIG 0x4
#define AE_ENC_UNINOBOM 0x8
#define AE_ENC_UNIBIGNOBOM 0x10
#define AE_ENC_UTF8 0x20
#define AE_ENC_UTF8NOBOM 0x40
#define AE_ENC_BINARY 0x80

// 개행 방식 - 하위 워드
#define AE_ENTER_UNKNOWN 0x00000
#define AE_ENTER_CRLF 0x10000
#define AE_ENTER_CR 0x20000
#define AE_ENTER_LF 0x40000

enum {AE_DOC_TOTALPARA, AE_DOC_WORD, AE_DOC_EXSPACESIZE, AE_DOC_CHAR, 
	AE_DOC_HAN, AE_DOC_EMPTYLINE};

struct tagLine
{
	int Start;
	int End;
	int nPara;
	int nLine;
};

struct UndoRecord
{
	BYTE action;
	BYTE status;
	int pos;
	int len;
	union {
		TCHAR *data;
		int *dest;
	};
	int tick;
};
enum { UR_NONE, UR_INSERT, UR_DELETE, UR_MOVE, UR_GROUP };
enum { UR_MAKING, UR_CANCELED };

struct Ae_PrintInfo
{
	HDC pdc;
	int dpiY;
	RECT prt;
	int ParseID;
	int nWrap;
	BOOL bLineNum;

	int Origin;
	int TabSize;
	int LineHeight;
	int LinePerPage;
	int TotalPage;
	tagLine *pLine;
	CParse *Parser;
};

#define Mili(n,r) int((n)*(r)/25.4)
#define AEID_VERTSCROLL 1
#define AEID_HORZSCROLL 2
#define AEID_BTNSETTING 3
#define SPLITWIDTH 8
#define SBW GetSystemMetrics(SM_CXHSCROLL)
enum { AESPLIT_VERT=1, AESPLIT_HORZ };
#define VGAP 6
#define RULERHEIGHT 20

struct Ae_SelTable
{
	int Line;
	int Start;
	int End;
};

// 문서 좌표와 순수 문서 좌표를 변환한다.
#define PureX(x) ((x)-frt.left)
#define PureY(y) ((y)-frt.top)
#define DocX(x) ((x)+frt.left)
#define DocY(y) ((y)+frt.top)

enum { AEBB_LEFTTOP, AEBB_RIGHTTOP, AEBB_LEFTBOTTOM, AEBB_RIGHTBOTTOM,
	AEBB_CENTER, AEBB_TILE, AEBB_TILESCROLL };

class CWindow
{
public:
	HWND hWnd;
	~CWindow();
	virtual LRESULT OnMessage(UINT iMessage,WPARAM wParam,LPARAM lParam)=0;
};

enum {TEXT_VIEW, HEXA_VIEW};
#define ForAllView(fn) for (int aet=0;aet<4 && arView[aet];aet++) {\
	arView[aet]->fn;\
}

// 컨트롤의 스타일
#define AES_WANTTAB					0x00000001
#define AES_USELINEEND				0x00000002
#define AES_ALLOWDRAG				0x00000004
#define AES_SPACEFORTAB				0x00000008
#define AES_AUTOINDENT				0x00000010
#define AES_BLOCKINDENTWITHTAB		0x00000020
#define AES_HOMETOFIRSTCHAR			0x00000040
#define AES_HIDECURLINE				0x00000080
#define AES_CALCTABWITHAVG			0x00000100
#define AES_HORZSPLITABLE			0x00000200
#define AES_VERTSPLITABLE			0x00000400
#define AES_AUTOCONVERT				0x00000800
#define AES_FIXEDCOLUMN				0x00001000
#define AES_NOWRAPCOLUMN			0x00002000
#define AES_USEAUTOCLIP				0x00004000
#define AES_AUTOCLIPBYTAB			0x00008000
#define AES_SHOWMARGIN				0x00010000
#define AES_SHOWENTER				0x00020000
#define AES_SHOWTAB					0x00040000
#define AES_SHOWSPACE				0x00080000
#define AES_SHOWLINENUM				0x00100000
#define AES_SHOWRULER				0x00200000
#define AES_SWAPDRAGBUTTON			0x00400000
#define AES_SMARTSPACE				0x00800000
#define AES_NOFIRSTSPACE			0x01000000

// 색상 번호
enum { AECOLOR_MAR1, AECOLOR_MAR2, AECOLOR_NUM, AECOLOR_MARK, AECOLOR_CODE, 
	AECOLOR_CUR, AECOLOR_FORE, AECOLOR_BACK, AECOLOR_SELFORE, AECOLOR_SELBACK, 
	AECOLOR_INNERRECT, AECOLOR_RULER, AECOLOR_SELFRAME };
#define AECOLORS 13

class CApiEditText;
class CApiEdit : public CWindow
{
	friend class CApiEditText;
	friend class CApiEditHexa;
	friend class CParse;
	friend class CParseCpp;
	friend class CParseNull;
	friend class CParseHtml;
	friend class CParseSql;
	friend class CParseJava;
	friend class CParseCS;
private:
	TCHAR *buf;
	static HFONT hLineNumFont;
	static HCURSOR hCSel,hCCopy,hCMove,hCMargin,hCNoDrop;
	BOOL bReadOnly;
	int buflen;
	int doclen;
	BOOL bModified;
	int UndoSize;
	int UndoMin;
	int saveur;
	int URtick;
	int URnest;
	int LastEditOff;
	DWORD dwFormat;
	int HideSelType;
	int nShowCurLine;
	int CaretWidth;
	int iDrag;
	POINT SplitPt;
	int OldSplitX,OldSplitY;
	UINT UserBase;
	LOGFONT FixedFont;
	static UINT AeColFormat;
	RECT InnerRect;
	int VAlign;
	int SelBlockType;
	HBITMAP hBackBit;
	int BackBitPos;
	int nWheel;
	int nCtrlWheel;
	int nShiftWheel;
	int nCtrlShiftWheel;
	int CharExtra;
	int WordExtra;
	int CharRatio;
	int CursorEdit;
	DWORD Style;
	int ShowTabType;
	int ShowEnterType;
	int ShowSpaceType;
	COLORREF Color[AECOLORS];

public:
	CApiEdit();
	UndoRecord *pUR;
	int URSize;
	int nowur;
	CApiEditText *arView[4];
	int ActiveView;
	int VertRatio;
	int HorzRatio;
	BOOL Create(int x,int y,int w,int h,DWORD style,UINT id,HWND hParent);
	LRESULT OnMessage(UINT iMessage,WPARAM wParam,LPARAM lParam);

	BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
	void OnPaint(HWND hWnd);
	void OnSetFocus(HWND hWnd, HWND hwndOldFocus);
	void OnDestroy(HWND hWnd);
	void OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	void OnMouseMove(HWND hWnd, int x, int y, UINT keyFlags);
	void OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags);
	void OnSize(HWND hWnd, UINT state, int cx, int cy);
	void OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify);
	BOOL OnSetCursor(HWND hWnd, HWND hwndCursor, UINT codeHitTest, UINT msg);

	BOOL IsDBCS(int nPos);
	int GetPrevOff(int nPos);
	int GetNextOff(int nPos);
	void Insert(int nPos, TCHAR *str, int len, BOOL bRec=TRUE);
	void Delete(int nPos, int nCount, BOOL bRec=TRUE);
	void SetWrap(int aWrap);
	void Invalidate(int Pos1, int Pos2=-1);
	int FindParaStart(int nPos);
	int IncludeEnter(int nPos);
	void ClearBookmark();
	void ToggleBookmark(int Para, int Mark);
	void GotoBookmark(int Mark);
	void GotoLine(int Line, int Col=0, int Type=0);
	static void LoadAeResource();
	static void UnLoadAeResource();
	BOOL GetModified() { return bModified; }
	void SetModified(BOOL aModi);
	void InitDoc();
	void SetText(TCHAR *TextBuf, int dwSize);
	void GetText(TCHAR *TextBuf, int BufSize, int s=-1, int e=-1);
	int GetTextLength();
	BOOL GetReadOnly() { return bReadOnly; }
	void SetReadOnly(BOOL aReadOnly);
	void GetSelect(int &s, int &e, BOOL bRegular=TRUE);
	void SetSelect(int s, int e, BOOL bFind);
	BOOL GetShowState(int nIndex);
	void SetShowState(int nIndex, BOOL aValue);
	int GetLineRatio();
	void SetLineRatio(int aLineHeight);
	int GetWrap();
	int GetShowCurLine() { return nShowCurLine; }
	void SetShowCurLine(int aShowCurLine);
	int GetTabWidth();
	void SetTabWidth(int aTabWidth);
	int GetHideSelType() { return HideSelType; }
	void SetHideSelType(int aHideSelType);
	void GetFont(LOGFONT *alogfont);
	void SetFont(LOGFONT *alogfont);
	int GetShowTabType() { return ShowTabType; }
	void SetShowTabType(int aShowTabType);
	int GetShowEnterType() { return ShowEnterType; }
	void SetShowEnterType(int aShowEnterType);
	int GetShowSpaceType() { return ShowSpaceType; }
	void SetShowSpaceType(int aShowSpaceType);
	int GetCaretWidth() { return CaretWidth; }
	void SetCaretWidth(int aCaretWidth);
	int GetRightWrap();
	void SetRightWrap(int aRightWrap);
	int GetColMark();
	void SetColMark(int aColMark);
	void URInsert(int nPos, TCHAR *str, int len);
	void URDelete(int nPos, TCHAR *str, int len);
	void URMove(int nPos, int Dest, TCHAR *str, int len);
	void Undo();
	void Redo();
	BOOL CanUndo();
	BOOL CanRedo();
	void ClearRedo();
	void NextRecord();
	void AllocURData(int idx,int need,int extra);
	int GetUndoSize() { return UndoSize; }
	void SetUndoSize(int aSize);
	int GetUndoMin() { return UndoMin; }
	void SetUndoMin(int aMin);
	void SaveModified();
	void ArrangeModified();
	void StartUndoGroup(int id, TCHAR *desc=NULL);
	void EndUndoGroup();
	CParse* GetParser();
	void SetParser(int ParseType);
	BOOL FindText(int nPos, TCHAR *what, DWORD dwFlag);
	int ReplaceText(int nPos, TCHAR *what, DWORD dwFlag, TCHAR *to);
	int GetOffset();
	void GetSelRect(RECT &srt);
	BOOL bFindSelect;
	int GetInfoFromOff(int Type, int nPos=-1);
	void GotoLastEditOff();
	void SendNotify(int code);
	int GetDocInfo(int type);
	BOOL GetInsMode();
	int GetOffFromPara(int pr, int pc);
	DWORD GetFormat() { return dwFormat; }
	void SetFormat(DWORD aFormat);
	DWORD AnalyzeFormat(TCHAR *Text, int dwSize);
	BOOL AnalyzeFormatUtf8(TCHAR *Text, int dwSize);
	int ConvertEncode(DWORD from, DWORD to, TCHAR *src, int srclen, TCHAR *&dest);
	int ConvertEnter(DWORD from, DWORD to, TCHAR *src, int srclen, TCHAR *&dest);
	void BeginPrint(Ae_PrintInfo *pi);
	BOOL PrintPage(Ae_PrintInfo *pi,int nPage);
	void EndPrint(Ae_PrintInfo *pi);
	CApiEditText *GetActiveView();
	CApiEditText *CreateView(int Type);
	void DeleteView(int nView);
	void DrawSplitBar(int SplitType,int pos);
	void SplitView(int SplitType, int Ratio);
	BOOL IsInDBCS(int nPos);
	void CloneView(CApiEditText *pDest, CApiEditText *pSrc,int SplitType);
	void AdjustSplitable();
	void UnSplitView(int SplitType);
	void ChangeActiveView(CApiEditText *pAe,int Who);
	void UpdateViews(CApiEditText *pView,int nPos,int nCount);
	BOOL IsEnterCode(int nPos);
	BOOL IsEnterCode(TCHAR *p);
	BOOL IsEnterCode(TCHAR *tbuf,int len,TCHAR *p);
	BOOL TestEnterCode(TCHAR *TextBuf, int dwSize);
	int FixEnterCode(TCHAR *TextBuf, int dwSize, TCHAR *&dest);
	void ChangeViewType(int nView,int Type);
	void SetBpl(int abpl);
	void SetCapital(int aCapital);
	void SetHexSep(int aHexSep);
	UINT GetUserBase() { return UserBase; }
	void SetUserBase(UINT aValue) { UserBase=aValue; }
	int FindGroupEnd(int idx);
	void MultiUndo(int idx);
	void MultiRedo(int idx);
	void GetFixedFont(LOGFONT *alogfont);
	void SetFixedFont(LOGFONT *alogfont);
	static void RegisterAeColFormat();
	int DoCommand(const TCHAR *szCom);
	void ExecMacro(const TCHAR *szMacro);
	RECT GetInnerRect() { return InnerRect; }
	void SetInnerRect(RECT aRect);
	int GetVAlign() { return VAlign; }
	void SetVAlign(int aValue);
	int GetSelBlockType() { return SelBlockType; }
	void SetSelBlockType(int aValue);
	HBITMAP GetBackBit() { return hBackBit; }
	void SetBackBit(HBITMAP hBit);
	int GetBackBitPos() { return BackBitPos; }
	void SetBackBitPos(int aValue);
	int GetWheel() { return nWheel; }
	int GetCtrlWheel() { return nCtrlWheel; }
	int GetShiftWheel() { return nShiftWheel; }
	int GetCtrlShiftWheel() { return nCtrlShiftWheel; }
	void SetWheel(int aWheel) { nWheel=aWheel; }
	void SetCtrlWheel(int aWheel) { nCtrlWheel=aWheel; }
	void SetShiftWheel(int aWheel) { nShiftWheel=aWheel; }
	void SetCtrlShiftWheel(int aWheel) { nCtrlShiftWheel=aWheel; }
	int GetCharExtra() { return CharExtra; }
	void SetCharExtra(int aValue);
	int GetWordExtra() { return WordExtra; }
	void SetWordExtra(int aValue);
	int GetCharRatio() { return CharRatio; }
	void SetCharRatio(int aValue);
	int GetCursorEdit() { return CursorEdit; }
	void SetCursorEdit(int aValue) { CursorEdit=aValue; }
	BOOL FindTextSub(int nPos, TCHAR *what, DWORD dwFlag,int &s, int &e);
	DWORD GetStyle() { return Style; }
	void SetStyle(DWORD aStyle);
	BOOL TestStyle(DWORD aStyle) { return (Style & aStyle) != 0; }
	void ChangeStyle(DWORD aStyle, BOOL bCheck) { 
		if (bCheck) SetStyle(Style | aStyle); else SetStyle(Style & ~aStyle); 
	}
	COLORREF GetColor(int index);
	void SetColor(int index, COLORREF aColor);
};

#endif // __APIEDIT_H
