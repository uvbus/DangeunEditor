#ifndef __SHORTCUT_H
#define __SHORTCUT_H

// ����Ű�� �̸��� Ű
struct SVirtKey
{
	TCHAR *Name;
	UINT vk;
};

int GetVirtKeyIndex(UINT vk);
extern SVirtKey arVirtKey[];

// �޴��� �׸��� �̸��� ID
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

// ����Ű �ϳ��� ����. ����Ű+����Ű�� ������ ����Ű �Է½� ����� 
// �޴��׸�, �Ǵ� ��ũ�� ����� ����Ѵ�.
struct SShortCut
{
	UINT vk;
	DWORD Flag;
	UINT id;
	TCHAR *Com;
};

// ����Ű ���� Ŭ����
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
