#ifndef __FILEWND_H
#define __FILEWND_H

enum {DGI_DRIVE, DGI_DRIVESEL, DGI_FOLDER, DGI_FOLDERSEL, DGI_FILE, DGI_FILESEL,
	DGI_PROJECT, DGI_PROJECTSEL, DGI_SUB, DGI_SUBSEL, 
	DGI_CLIPROOT, DGI_CLIPROOTSEL, DGI_CLIPFOLDER, DGI_CLIPFOLDERSEL, 
	DGI_CLIP, DGI_CLIPSEL};

LRESULT CALLBACK DGFileProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL IsDirEmpty(TCHAR *Path);
HTREEITEM FindInsAfter(HTREEITEM hParent, int iImage, TCHAR *Name);
int OnFindNode(TCHAR *Path,DWORD Attr,LPVOID pCustom);
void GetNodePath(HTREEITEM pNode, TCHAR *Path);
void InitTree();
void GetCurrentFolder(TCHAR *Path);
void GotoFolderInTree(TCHAR *Path);
void RefreshTree();

#endif // __FILEWND_H