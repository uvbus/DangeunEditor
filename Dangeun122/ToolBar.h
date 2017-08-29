#ifndef __TOOLBAR_H
#define __TOOLBAR_H

void CreateToolBar(BOOL bText, BOOL bBig);
void SetStatusPart();
void SetStatusText(int mask,LPCTSTR Mes=NULL);
LRESULT CALLBACK DGTabProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);
void AddFileTab(TCHAR *Path);
int FindFileTab(TCHAR *Path);
void ChangeFileTab(int idx, TCHAR *Path);
void DeleteFileTab(TCHAR *Path);

#endif // __TOOLBAR_H