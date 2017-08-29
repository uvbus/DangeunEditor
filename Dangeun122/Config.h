#ifndef __CONFIG_H
#define __CONFIG_H

void Config();
void DgEnumFonts();
int CALLBACK EnumFamCallBack(ENUMLOGFONT *, NEWTEXTMETRIC *, int, LPARAM);
int CALLBACK ConfigProc(HWND hwndDlg, UINT uMsg, LPARAM lParam);
BOOL CALLBACK GeneralDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK ViewDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK ColorDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK EditDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK SyntaxDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK PrintDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK ShapeDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK ToolBarDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK FileDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK AssociateDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK OperationDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
void ApplyNow();
void SetSetting(CApiEdit &Ae);
void AdjustDlgUI(int Page,HWND hDlg);
void InitSysColor();
COLORREF& GetColorFromID(int ID);
void SetSettingView(CApiEditText *pView);

struct arColor
{
	TCHAR Name[16];
	COLORREF Color;
};

#endif // __CONFIG_H