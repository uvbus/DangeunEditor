#ifndef __CLIP_H
#define __CLIP_H

// 상용구 구조체
class CClipRecord
{
public:
	CClipRecord();

	int Parent;
	int Type;
	TCHAR Name[32];
	TCHAR AutoName[16];
	TCHAR *Content;
	int len;
	int DefAction;

	TCHAR FontFace[32];
	BYTE PitchAndFamily;
	BYTE CharSet;
	BYTE FontSize;
	COLORREF cFore,cBack;
	int nWrap;
	int nParser;
	int nView;
	int LineRatio;
	BYTE Reserved[16];
};
enum {DGCT_FOLDER=1, DGCT_CLIP };

// 상용구의 헤더
struct SClipHeader
{
	TCHAR Text[64];
	int Ver;
	TCHAR Name[64];
	TCHAR Desc[128];
	int Num;
	BYTE Reserved[16];
};

// 상용구 파일
class CClipFile
{
public:
	SClipHeader Header;
	CClipRecord *arRecord;

	CClipFile();
	~CClipFile();
	int Load(TCHAR *Path);
	BOOL Save(TCHAR *Path);
};

BOOL CALLBACK ClipDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK EditClipProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
void DisplayClipPreview(CApiEdit *pAe,CClipRecord *cr);
int ConfirmSaveClip(HWND hDlg);
BOOL LoadClipFileToDlg(HWND hDlg,TCHAR *Path);
void AddClipFileToTree(HWND hTree,HTREEITEM hPrev,TCHAR *Path,CClipFile *cf);
void AddClipRecordToTree(HWND hTree,HTREEITEM pNode,CClipFile *cf,int idx);
BOOL SaveClipFileFromDlg(HWND hDlg);
void AddClipRecordToFile(HWND hTree,HTREEITEM hParent,int pid,CClipFile *cf);
HTREEITEM CopyTreeItem2(HWND hTree, HTREEITEM hSrc, HTREEITEM hDest, BOOL bNext);
HTREEITEM CopyTreeItems2(HWND hTree, HTREEITEM hSrc, HTREEITEM hDest,BOOL bNext);
LRESULT CALLBACK DGClipProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL LoadClipFile(TCHAR *Path,BOOL bUpdate);
CClipRecord *FindClipRecord(TCHAR *Name,BOOL bAutoName);
CClipRecord *FindClipRecordSub(HTREEITEM hRoot,TCHAR *Name,BOOL bAutoName);

#endif // __CLIP_H
