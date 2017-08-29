#ifndef __FIND_H
#define __FIND_H

void RefillHistory(HWND hCombo, CHistory &Hist);
BOOL CALLBACK FindDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK GotoLineDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK DGOutputProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK FileFindDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
void FindOrReplaceInFiles(BOOL bReplace);
int OnFindFile(TCHAR *Path,DWORD Attr,LPVOID pCustom);
void GetLineNumAndText(TCHAR *szBuf, int buflen, TCHAR *ptr, TCHAR *Text, int &line, int &col);
BOOL IsDelimiter(TCHAR t);
int OnReplaceFile(TCHAR *Path, DWORD Attr, LPVOID pCustom);
void OpenFileFromOutput(int idx);
void CopyOutputToText();
void SendOutputToWindow();
void ExtractOutputToText(TCHAR *&buf);
DWORD WINAPI FIFThread(LPVOID pCallback);
BOOL TestFIFThread();

#endif // __FIND_H