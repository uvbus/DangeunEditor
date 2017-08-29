#ifndef __PRINT_H
#define __PRINT_H

void Print(HWND hChild);
void PrintDirect(HWND hChild);
BOOL PrintDoc(HWND hChild, HDC pdc, int nPages, int nCopies, int nFirst, int nFinal);
LRESULT CALLBACK DgPrintingProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK AbortProc(HDC hPrtdc, int iError);
void PrintFormatText(TCHAR *szFormat,Ae_PrintInfo *pi,TCHAR *Path,int nPage,int y);
void TextReplace(TCHAR *Text,TCHAR *what,TCHAR *to);

#endif // __PRINT_H