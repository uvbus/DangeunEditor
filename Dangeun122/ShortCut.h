#ifndef __SHORTCUT_H
#define __SHORTCUT_H

// 가상키의 이름과 키
struct SVirtKey
{
	TCHAR *Name;
	UINT vk;
};

int GetVirtKeyIndex(UINT vk);
extern SVirtKey arVirtKey[];

// 메뉴의 항목의 이름과 ID
struct SMenuCom
{
	TCHAR *Name;
	UINT id;
};

int GetMenuComIndex(UINT id);
extern SMenuCom arMenuCom[];

#define SC_SHIFT 0x01
#define SC_CTRL 0x02
#define SC_ALT 0x04

// 단축키 하나의 정보. 조합키+가상키로 구성된 단축키 입력시 실행될 
// 메뉴항목, 또는 매크로 명령을 기억한다.
struct SShortCut
{
	UINT vk;
	DWORD Flag;
	UINT id;
	TCHAR *Com;
};

// 단축키 관리 클래스
class CShortCut
{
public:
	SShortCut *arShortCut;
	BOOL bModified;

	CShortCut();
	~CShortCut();
	void Exit();

	void Init();
	void Add(SShortCut *sc);
	void Delete(int idx);
	void Modify(int idx,SShortCut *sc);
	int Find(UINT vk,DWORD Flag);
	int Find(UINT id,TCHAR *Com);
	void Load(TCHAR *Key);
	void Save(TCHAR *Key);
};

void DisplayShortCutList(HWND hDlg);
BOOL CALLBACK ShortCutListProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK ShortCutProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
int GetDlgShortCut(HWND hDlg);
void SetDlgShortCut(HWND hDlg,int idx);
void DisplayShortCutStatus(HWND hDlg);
void UpdateMenuCaption(HMENU hMenu);
void UpdateItemCaption(HMENU hMenu,int idx,UINT ID,TCHAR *Caption);

#endif // __SHORTCUT_H
