#ifndef __OPTION_H
#define __OPTION_H

#define EI_OFFSET 1
#define EI_SELECTION 2
#define EI_BOOKMARK 4
#define EI_UNDORECORD 8
#define EI_SETTING 0x10

struct SStMask
{
	bool bShow;
	BYTE Order;
	short Width;
	bool bPop;
	BYTE Align;
};
#define STNUM 13
#define STMAX 32

struct SOption
{
	BOOL bModified;
	SOption();
	void Init();
	int GetMaxMru();
	int GetUndoLimit();
	void Load(TCHAR *Key);
	void Save(TCHAR *Key);
	void GetStyleColor();
	void SetStyleColor(CParse *Parser);
	int GetBpl();
	int GetHexSep();
	int GetStNumByOrder(int Order);
	BOOL TestStyle(DWORD aStyle) { return (Style & aStyle) != 0; }
	DWORD GetStyle() { return Style; }
	void SetStyle(DWORD aStyle) { Style=aStyle; }
	void ChangeStyle(DWORD aStyle, BOOL bCheck) { 
		if (bCheck) SetStyle(Style | aStyle); else SetStyle(Style & ~aStyle);
	}

	DWORD Style;
	int StartAction;
	BOOL bExplorerPopup;
	BOOL bAllowMulti;
	BOOL bMaxFirstChild;
	int MaxMru;
	TCHAR DefExt[12];
	BOOL Hangul;
	int OutputHeight;
	BOOL bInitFind;
	BOOL bInitNextFind;
	BOOL bShowToolBar;
	BOOL bShowStatus;
	BOOL bShowFileWnd;
	int FileWndWidth;
	BOOL bShowFileTab;
	BOOL bShowHidden;
	int FilterIndex;
	BOOL bToolBarBig;
	BOOL bToolBarText;
	BOOL bBrowseMode;
	BOOL bSoundFindFail;
	BOOL bWatchChange;
	BOOL bReloadNoAsk;
	BOOL bReloadProject;

	int LineRatio;
	BOOL bWrap;
	int nWrap;
	int RightWrap;
	int ColMark;
	int HideSelType;
	int nShowCurLine;
	LOGFONT logfont;
	int TabWidth;
	int ShowTabType;
	int ShowEnterType;
	int ShowSpaceType;
	int CaretWidth;
	int UndoLimit;
	int FindDlgPos;

	TCHAR arExt[PARSENUM][250];
	SParseStyle arStyle[PARSENUM][32];
	FtpInfo arFtp[32];

	LOGFONT prtFont;
	RECT prtMargin;
	BOOL prtbParse;
	BOOL prtnWrap;
	BOOL prtbLineNum;
	BOOL prtbSysHeader;
	BOOL prtbDirect;
	TCHAR prtHeader[256];
	TCHAR prtFooter[256];
	int prtnHeader;
	int prtnFooter;
	TCHAR prtBkText[64];

	int bpl;
	BOOL bCapital;
	int nHexSep;
	int BackupLevel;
	TCHAR BackupFolder[MAX_PATH];
	int BackupNaming;
	int AutoSaveInterval;
	BOOL bMakeEditInfo;
	BOOL bUpdateEditInfo;
	TCHAR EditInfoFolder[MAX_PATH];
	int EditInfoMask;
	BOOL bUseEditInfo;
	LOGFONT FixedFont;
	BOOL bShowClipWnd;
	int ClipWndRatio;
	BOOL bReloadClip;
	BOOL bPreviewClip;
	RECT InnerRect;
	int VAlign;
	int SelBlockType;
	TCHAR BackBit[MAX_PATH];
	int BackBitPos;
	BOOL RmWrap;
	LOGFONT Rmlogfont;
	int RmLineRatio;
	int RmFreq;
	int RmLineNum;
	int RmManual;
	SStMask StMask[STMAX];
	int nWheel;
	int nCtrlWheel;
	int nShiftWheel;
	int nCtrlShiftWheel;
	int CharExtra;
	int WordExtra;
	int CharRatio;
	int CursorEdit;
	COLORREF Color[AECOLORS];
};

#endif // __OPTION_H