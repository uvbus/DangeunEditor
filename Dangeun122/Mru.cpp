#include "stdafx.h"

CHistory::CHistory()
{
	width=height=num=0;
	data=NULL;
}

CHistory::CHistory(int awidth, int aheight)
{
	Init(awidth,aheight);
}

CHistory::~CHistory()
{
	int i;
	for (i=0;i<height;i++) {
		free(data[i]);
	}
	free(data);
}

void CHistory::Init(int awidth, int aheight)
{
	int i;

	width=awidth;
	height=aheight;

	data=(TCHAR **)malloc(sizeof(TCHAR *)*height);
	for (i=0;i<height;i++) {
		data[i]=(TCHAR *)malloc(width);
		memset(data[i],0,width);
	}
	num=0;
}

void CHistory::ChangeHeight(int aheight)
{
	int i;

	if (aheight > height) {
		data=(TCHAR **)realloc(data,sizeof(TCHAR *)*aheight);
		for (i=height;i<aheight;i++) {
			data[i]=(TCHAR *)malloc(width);
			memset(data[i],0,width);
		}
	}

	if (aheight < height) {
		for (i=aheight;i<height;i++) {
			free(data[i]);
		}
	}

	height=aheight;
	num=min(num,height);
}

void CHistory::Add(TCHAR *str)
{
	int i,nFind=10000;

	for (i=0;i<num;i++) {
		if (lstrcmp(str,data[i])==0) {
			nFind=i;
			break;
		}
	}

	for (i=min(nFind,min(num,height-1));i>0;i--) {
		lstrcpy(data[i],data[i-1]);
	}
	lstrcpy(data[0],str);
	if (nFind == 10000 && num < height) {
		num++;
	}
}

void CHistory::Delete(int idx)
{
	int i;
	for (i=idx;i<height-1;i++) {
		lstrcpy(data[i],data[i+1]);
	}
	lstrcpy(data[height-1],"");
	num--;
}

TCHAR *CHistory::Get(int idx)
{
	return data[idx];
}

void CHistory::Set(int idx, TCHAR *str)
{
	lstrcpy(data[idx],str);
}

void CHistory::Empty()
{
	int i;

	for (i=0;i<height;i++) {
		lstrcpy(data[i],"");
	}
}

CMru::CMru()
{
}

CMru::~CMru()
{
}

BOOL CMru::InitMRU(TCHAR *aKey, HMENU aMenu, UINT aMenuID, int aMaxMru)
{
	MaxMru=aMaxMru;
	arMru.Init(MAX_PATH+64,MaxMru);
	lstrcpy(Key,aKey);
	MenuID=aMenuID;
	return ChangeMenu(aMenu);
}

BOOL CMru::ChangeMenu(HMENU aMenu)
{
	if (aMenu) {
		hMenu=aMenu;
		ArrangeMenu();
		return TRUE;
	}
	return FALSE;
}

void CMru::ChangeMaxMru(int aMaxMru)
{
	arMru.ChangeHeight(aMaxMru);
	ArrangeMenu();
}

void CMru::LoadMRU()
{
	int i;
	TCHAR SubKey[32];
	TCHAR Path[MAX_PATH];
	TCHAR Text[MAX_PATH];

	arMru.num=SHRegReadInt(SHCU,Key,"nFiles",0);
	arMru.num=min(arMru.num, MaxMru);
	for (i=0;i<arMru.num;i++) {
		wsprintf(SubKey,"File%d",i);
		SHRegReadString(SHCU,Key,SubKey,"",Path,MAX_PATH);
		if (strncmp(Path,"Enco",4)==0) {
			DecodeText(Path,Text);
		} else {
			lstrcpy(Text,Path);
		}
		arMru.Set(i,Text);
	}
	ArrangeMenu();
}

void CMru::SaveMRU()
{
	int i;
	TCHAR SubKey[32];
	TCHAR EncText[MAX_PATH];

	SHRegWriteInt(SHCU,Key,"nFiles",arMru.num);
	for (i=0;i<arMru.num;i++) {
		wsprintf(SubKey,"File%d",i);
		if (strnicmp(arMru.Get(i),"ftp",3)==0) {
			EncodeText(arMru.Get(i),EncText);
		} else {
			lstrcpy(EncText,arMru.Get(i));
		}
		SHRegWriteString(SHCU,Key,SubKey,EncText);
	}
}

void CMru::AddMRU(TCHAR *Path)
{
	arMru.Add(Path);
	ArrangeMenu();
}

void CMru::RemoveMRU(int nIndex)
{
	arMru.Delete(nIndex);
	ArrangeMenu();
}

void CMru::ArrangeMenu()
{
	int i;
	TCHAR szItem[MAX_PATH+12];
	TCHAR EncPath[MAX_PATH];

	if (arMru.num) {
		while (DeleteMenu(hMenu,0,MF_BYPOSITION));
	}

	for (i=0;i<arMru.num;i++) {
		EncodePath(arMru.Get(i),EncPath);
		if (i < 9) {
			wsprintf(szItem,"&%c %s",i+1+'0',EncPath);
		} else {
			wsprintf(szItem,"&%c %s",i-9+'A',EncPath);
		}
		AppendMenu(hMenu,MF_STRING,MenuID+i,szItem);
	}
}

BOOL CMru::GetFilePath(int nIndex,TCHAR *buf)
{
	if (nIndex >= arMru.num) {
		return FALSE;
	}
	lstrcpy(buf,arMru.Get(nIndex));
	return TRUE;
}

