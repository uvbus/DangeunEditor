#ifndef __MRU_H
#define __MRU_H

class CHistory
{
private:
	TCHAR **data;
	int width;

public:
	int num;
	int height;

	CHistory();
	CHistory(int awidth,int aheight);
	~CHistory();

	void Init(int awidth, int aheight);
	void ChangeHeight(int aheight);
	void Add(TCHAR *str);
	void Delete(int idx);
	TCHAR *Get(int idx);
	void Set(int idx, TCHAR *str);
	void Empty();
};

class CMru
{
private:
	CHistory arMru;
	HMENU hMenu;
	TCHAR Key[MAX_PATH];

public:
	UINT MenuID;
	int MaxMru;

	CMru();
	~CMru();
	BOOL InitMRU(TCHAR *aKey, HMENU aMenu, UINT aMenuID, int aMaxMru);
	BOOL ChangeMenu(HMENU aMenu);
	void ChangeMaxMru(int aMaxMru);
	void LoadMRU();
	void SaveMRU();
	void AddMRU(TCHAR *Path);
	void RemoveMRU(int nIndex);
	void ArrangeMenu();
	BOOL GetFilePath(int nIndex,TCHAR *buf);
};

#endif // __MRU_H