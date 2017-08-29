#ifndef __DANGEUN_H
#define __DANGEUN_H

LRESULT CALLBACK DGWndProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK DGChildProc(HWND,UINT,WPARAM,LPARAM);
int OnCreate(HWND hWnd,WPARAM wParam,LPARAM lParam);
void OnDestroy(HWND hWnd,WPARAM wParam,LPARAM lParam);
void OnTimer(HWND hWnd,WPARAM wParam,LPARAM lParam);
void OnCommand(HWND hWnd,WPARAM wParam,LPARAM lParam);
BOOL OnClose(HWND hWnd,WPARAM wParam,LPARAM lParam);
BOOL OnQueryEndSession(HWND hWnd,WPARAM wParam,LPARAM lParam);
void OnInitMenu(HWND hWnd,WPARAM wParam,LPARAM lParam);
void OnDropFiles(HWND hWnd,WPARAM wParam,LPARAM lParam);
void OnFontChange(HWND hWnd,WPARAM wParam,LPARAM lParam);
void OnActivateApp(HWND hWnd,WPARAM wParam,LPARAM lParam);
void OnUser1(HWND hWnd,WPARAM wParam,LPARAM lParam);
void OnUser2(HWND hWnd,WPARAM wParam,LPARAM lParam);
BOOL OnSize(HWND hWnd,WPARAM wParam,LPARAM lParam);
BOOL OnSetCursor(HWND hWnd,WPARAM wParam,LPARAM lParam);
void OnLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
void OnMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam);
void OnLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam);
void OnNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
void OnIdle();
void OnSysColorChange(HWND hWnd,WPARAM wParam,LPARAM lParam);
void OnUser3(HWND hWnd,WPARAM wParam,LPARAM lParam);
void OnUser4(HWND hWnd,WPARAM wParam,LPARAM lParam);

BOOL CALLBACK AboutProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
HWND New();
void Open();
BOOL OpenFromFile(TCHAR *Path, BOOL bReadOnly=FALSE, BOOL bBrowse=FALSE);
BOOL OpenFileToChild(HWND hChild, TCHAR *Path);
BOOL Save(HWND hChild);
BOOL SaveAs(HWND hChild);
BOOL SaveToFile(HWND hChild,TCHAR *Path);
int ConfirmSave(HWND hChild);
HWND FindChildWithFile(TCHAR *path);
BOOL TestNeedActive(WORD ID);
void RevertFile(HWND hChild);
void SaveAll();
void CloseAll();
void RegisterPopup(BOOL bReg,BOOL bDelete=FALSE);
void ActivateBrood();
BOOL CALLBACK SetupDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK UninstallDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
void Uninstall();
void SelectParser(CApiEdit &Ae,TCHAR *path);
void Relayout();
void GetChildSize(int &t,int &s,int &f,int &o,int &w,int &ch);
HWND NewChild();
void PrintInfo(CApiEdit &Ae);
LRESULT CALLBACK DgCbtHookProc(int nCode,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK DgNewDlgProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);

struct SInfo
{
	CApiEdit Ae;
	TCHAR NowFile[MAX_PATH];
};

struct FileInfo
{
	HWND hWnd;
	DWORD size;
	FILETIME st;
};

struct EditInfo
{
	TCHAR Text[64];
	int Ver;
	TCHAR FilePath[MAX_PATH];
	DWORD FileSize;
	FILETIME FileTime;
	int EditInfoMask;

	int HorzRatio,VertRatio;
	int arViewType[4];
	int ActiveView;
	int BookMark[4][MAXBOOKMARK];
	int off[4];
	int SelStart[4];
	int SelEnd[4];
	BOOL bWrap[4];
	int LineRatio[4];
	LOGFONT logfont[4];
};

void MakeBackupName(TCHAR *Folder,TCHAR *fname,TCHAR *ext,int level,TCHAR *szName);
void MakeBackup(TCHAR *Path);
void AutoSave();
void SaveEditings();
void MakeAssociate(TCHAR *ext, TCHAR *desc);
void UnAssociate(TCHAR *ext);
void RearrangePath();
BOOL GetEiPath(TCHAR *Path, TCHAR *EiPath);
BOOL MakeEditInfo(CApiEdit &Ae, TCHAR *Path, EditInfo &ei);
void UpdateEditInfo(CApiEdit &Ae, TCHAR *Path);
void SaveEditInfo(CApiEdit &Ae, TCHAR *Path);
void LoadEditInfo(CApiEdit &Ae, TCHAR *Path);
BOOL CALLBACK EditRecordProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL TestShortCut(HWND hWnd,LPMSG msg);
BOOL TestNoSaved();
BOOL CALLBACK ConfirmSaveProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK ConfirmReloadProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL ResolveShortCut(TCHAR *ShortCut, TCHAR *Path);
void InsertFile(CApiEditText *pView);
BOOL CALLBACK SpecialDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK DGStringGridProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);
void ReadMode();
BOOL CALLBACK ReadModeProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);

struct SParaOption
{
	SParaOption();

	int startpage;
	int p1_Type;
	TCHAR p1_szRep[32];
	int p1_MultiEnter;
	int p2_Type;
	int p2_nChar;
	int p2_nPixel;
	BOOL p2_bIndent;
	int p2_nIndent;
	BOOL p2_bRemoveSpace;
	int p3_Orient;
	int p3_Type;
	int p3_nChar;
	int p3_nPixel;
	int p4_Type;
	BOOL p4_Case;
	BOOL p4_Unique;
	int p5_Type;
	int p5_Num;
};

BOOL CALLBACK ParaDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK Para1Proc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK Para2Proc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK Para3Proc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK Para4Proc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK Para5Proc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
void InstantSearch(BOOL bStart);
LRESULT CALLBACK InstEditProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);
void InstSrchNow(HWND hEdit,int Start,DWORD Flag);
BOOL CALLBACK ShareNoticeProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);

#endif // __DANGEUN_H