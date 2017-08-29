#ifndef __PROJECT_H
#define __PROJECT_H

enum {DGP_SUB=1, DGP_FILE, DGP_FOLDER};

struct ProjectHeader
{
	TCHAR Text[64];
	int Ver;
	TCHAR Name[32];
	TCHAR Desc[128];
	BOOL bAbsolute;
};

struct ProjectRecord
{
	int Parent;
	BYTE Type;
	TCHAR Path[400];
};

BOOL CALLBACK DgProjectProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK DgNewProjectProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL OpenProject(HWND hDlg,TCHAR *Project,BOOL &bAbsolute);
void InsertProjectRecord(HWND hTree,ProjectRecord *data,HTREEITEM pNode,int pid);
void AddFileToProject(HWND hDlg,TCHAR *Path,BOOL bAbsolute);
void SaveProject(HWND hDlg, TCHAR *Project);
void SaveProjectRecord(HWND hDlg, HANDLE hFile,HTREEITEM hParent,int pid);
BOOL LoadProject(TCHAR *Project,BOOL bUpdate);
HTREEITEM FindTreeItem(HWND hTree, HTREEITEM hParent, TCHAR *Text);
int GetProjectNodePath(HWND hDlg,HWND hPrjTree,HTREEITEM hItem,TCHAR *Path);
BOOL TestProjectValid(HWND hDlg, HTREEITEM hParent);
HTREEITEM CopyTreeItem(HWND hTree, HTREEITEM hSrc, HTREEITEM hDest, BOOL bNext);
HTREEITEM CopyTreeItems(HWND hTree, HTREEITEM hSrc, HTREEITEM hDest,BOOL bNext);
void RePathProject(HANDLE hFile,TCHAR *Project,BOOL bAbsolute);

#endif // __PROJECT_H