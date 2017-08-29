#ifndef __MACRO_H
#define __MACRO_H

enum {DGC_CALL, DGC_CLIP, DGC_NAVI, DGC_SET, DGC_TOGGLE};
#define DGVERBNUM 5

int ExecMacro(const TCHAR *szMacro);
int DoCommand(const TCHAR *szCom);

struct SMacro
{
	TCHAR Name[65];
	TCHAR *Content;
};

class CMacro
{
public:
	CMacro();
	~CMacro();

	SMacro *arMacro;
	int Size;
	int Num;
	BOOL bModified;

	int Find(const TCHAR *Name);
	void Set(int idx,const TCHAR *szCon);
	void Add(const TCHAR *Name,const TCHAR *szCon);
	void Delete(int idx);
	void LoadFromReg(TCHAR *Key);
	void SaveToReg(TCHAR *Key);
	void LoadFromFile(TCHAR *Path);
	void SaveToFile(TCHAR *Path);
};

BOOL CALLBACK MacroDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK MacroEditDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
void DisplayMakingCommand(HWND hDlg);
BOOL CALLBACK AddCommandDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK RunMacroDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);

#endif // __MACRO_H
