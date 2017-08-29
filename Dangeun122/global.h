#ifndef __GLOBAL_H
#define __GLOBAL_H

#define KEY "Software\\Miyoungsoft\\Dangeun\\1.2\\"
#define GAP 3
#define ARSIZE(ar) (sizeof(ar)/sizeof(ar[0]))

extern HINSTANCE g_hInst;
extern HWND g_hFrameWnd;
extern HWND g_hMDIClient;
extern int g_NewNo;
extern int g_ChildNum;
extern SOption Option, NewOption;
extern LOGFONT *arFont;
extern int FontNum;
extern int g_StartPage;
extern CMru Mru;
extern BOOL bUninstall;
extern HWND g_FindDlg;
extern CHistory arFind[4];
extern DWORD FindFlag;
extern HWND hOutput;
extern HWND hList;
extern int TotalFind;
extern TCHAR LastFIF[256];
extern HANDLE hFIFThread;
extern HWND hToolBar;
extern HWND hStatus;
extern HWND hFileWnd;
extern HWND hTabFrame;
extern HWND hFileTab;
extern HCURSOR hCurHorz, hCurVert;
extern HWND hFileTree, hFilter;
extern HIMAGELIST hFileImg;
extern HIMAGELIST hTabImg;
extern HFONT hGul9;
extern BOOL bContDown;
extern HINTERNET hInternet, hFtp;
extern BOOL bPrinting;
extern HWND hDlgCancel;
extern BOOL bShowOutput;
extern BOOL bContFIF;
extern CMacro DgMacro;
extern CShortCut DgShortCut;
extern HWND hClipWnd;
extern HWND hClipTree;
extern HBITMAP hBackBit;

#endif // __GLOBAL_H
