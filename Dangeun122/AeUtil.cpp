//#include <windows.h>
#include "stdafx.h"

// ���� �迭
// Flag�� b0=�ݺ� ���ɼ�, b1=������ ����(���ڿ� ������� ����)
SMacVerb arComVerb[]={
	{"move","�̵�",0x03},
	{"movesel","�����̵�",0x03},
	{"insert","����",0x00},
	{"delete","����",0x01},
	{"bs","�չ��� ����",0x03},
	{"select","����",0x02},
	{"copy","����",0x00},
	{"copyapp","�����ؼ� �߰�",0x00},
	{"cut","�߶󳻱�",0x00},
	{"cutapp","�߶� �߰�",0x00},
	{"paste","�ٿ��ֱ�",0x01},
	{"convert","��ȯ",0x03},
	{"copychar","���Ʒ� ���� �ݺ�",0x03},
	{"scroll","��ũ��",0x03},
	{"setmark","å���� ����",0x02},
	{"gomark","å���� �̵�",0x02},
	{"find","ã��",0x00},
	{"replace","�ٲٱ�",0x00},
	{"focus","�� ��ȯ",0x03},
	{"","",0x00}
};

// ������ �迭
SMacObject arComObject[]={
	{"left","����"},						// move:0
	{"right","������"},
	{"up","����"},
	{"down","�Ʒ���"},
	{"home","�� ó������"},
	{"end","�� ������"},
	{"pgup","������ ����"},
	{"pgdn","������ �Ʒ���"},
	{"wleft","���� �ܾ��"},
	{"wright","���� �ܾ�"},
	{"wordhome","�ܾ� ó������"},
	{"wordend","�ܾ� ������"},
	{"dochome","���� ó��"},
	{"docend","���� ��"},
	{"paren","�����Ǵ� ��ȣ"},
	{"last","������ ���� ��ġ"},
	{"nextpara","���� ����"},
	{"prevpara","���� ����"},
	{"parahome","���� ó��"},
	{"paraend","���� ��"},
	{NULL,NULL},
	{"char","����"},						// bs:21
	{"word","�ܾ�"},
	{NULL,NULL},
	{"word","�ܾ�"},						// select:24
	{"line","��"},
	{"para","����"},
	{"doc","����"},
	{NULL,NULL},
	{"upper","�빮�ڷ�"},					// convert:29
	{"lower","�ҹ��ڷ�"},
	{"reversecase","��� �ݴ��"},
	{"upperfirst","ù���ڸ� �빮�ڷ�"},
	{"uppersent","���� ó���� �빮�ڷ�"},
	{"totab","������ ������"},
	{"tospace","���� ��������"},
	{"removespace","���ʿ��� ���� ����"},
	{"indent","�鿩����"},
	{"unindent","�����"},
	{"comment","�ּ�����"},
	{"uncomment","�ּ�����"},
	{NULL,NULL},
	{"up","���� ����"},						// copychar:42
	{"down","�Ʒ��� ����"},
	{NULL,NULL},
	{"up","��"},							// scroll:45
	{"down","�Ʒ�"},
	{"pgup","������ ��"},
	{"pgdn","������ �Ʒ�"},
	{"halfup","�� ������ ��"},
	{"halfdown","�� ������ �Ʒ�"},
	{"left","����"},
	{"right","������"},
	{"pageleft","������ ����"},
	{"pageright","������ ������"},
	{NULL,NULL},
	{"unname","�̸����� �ϸ�ũ"},			// setmark:56
	{"0","0�� �ϸ�ũ"},
	{"1","1�� �ϸ�ũ"},
	{"2","2�� �ϸ�ũ"},
	{"3","3�� �ϸ�ũ"},
	{"4","4�� �ϸ�ũ"},
	{"5","5�� �ϸ�ũ"},
	{"6","6�� �ϸ�ũ"},
	{"7","7�� �ϸ�ũ"},
	{"8","8�� �ϸ�ũ"},
	{"9","9�� �ϸ�ũ"},
	{NULL,NULL},
	{"next","���� �ϸ�ũ"},					// gomark:68
	{"prev","���� �ϸ�ũ"},
	{"0","0�� �ϸ�ũ"},
	{"1","1�� �ϸ�ũ"},
	{"2","2�� �ϸ�ũ"},
	{"3","3�� �ϸ�ũ"},
	{"4","4�� �ϸ�ũ"},
	{"5","5�� �ϸ�ũ"},
	{"6","6�� �ϸ�ũ"},
	{"7","7�� �ϸ�ũ"},
	{"8","8�� �ϸ�ũ"},
	{"9","9�� �ϸ�ũ"},
	{"next","���� ����"},					// focus:80
	{"prev","���� ����"},
	{NULL,NULL}
};

// �� ���纰 ������ �迭���� ���� ÷��
int arObjOfVerb[]={0,0,-1,-1,21,24,-1,-1,-1,-1,-1,29,42,45,56,68,-1,-1,80};

TCHAR *FindString(TCHAR *buf, int buflen, int nPos, TCHAR* what,int len,BOOL bDown, BOOL bCase)
{
	TCHAR *pWhat,*p;
	int nOff;
	TCHAR *pResult;
	BOOL bFind;
	int Inc;
	TCHAR ch;

	pWhat=(TCHAR *)malloc(len);
	memcpy(pWhat,what,len);
	if (bCase==FALSE) {
		for (p=pWhat;p-pWhat<len;p++) {
			*p=toupper(*p);
		}
	}

	if (bDown) {
		Inc=1;
	} else {
		Inc=-1;
	}

	for (;;nPos+=Inc) {
		if (nPos>buflen-len || nPos<0) {
			pResult=NULL;
			break;
		}

		bFind=TRUE;
		for (nOff=0;nOff<len;nOff++) {
			if (bCase) {
				ch=buf[nPos+nOff];
			} else {
				ch=toupper(buf[nPos+nOff]);
			}

			if (ch!=pWhat[nOff]) {
				bFind=FALSE;
				break;
			}
		}

		if (bFind==TRUE) {
			pResult=buf+nPos;
			break;
		}
	}

	free(pWhat);
	return pResult;
}

// ������ �ǳʶڴ�.
void SkipSpace(const TCHAR *&pc)
{
	for (;*pc;pc++) {
		if (*pc!=' ' && *pc!='\t')
			break;
	}
}

// ���ڿ��� ������ ���ڿ� ������ �ǳʶ�. pc�� �ݴ� ����ǥ�� ��ġ��.
// �ݴ� ����ǥ�� ������ FALSE�� �����Ѵ�.
BOOL SkipString(const TCHAR *&pc)
{
	if (*pc=='\"') {
		for (pc++;;pc++) {
			if (*pc==NULL) {
				return FALSE;
			}
			if (*pc=='\"' && *(pc-1)!='\\') {
				break;
			}
		}
	}
	return TRUE;
}

// ���� �ϳ��� �̽��������Ѵ�. ��ġ �̵����� �� �ش�.
void AeEscapeChar(const TCHAR *&s,TCHAR *&d)
{
	TCHAR high,low,third;
	int code;

	if (*s=='\\') {
		s++;
		switch (*s) {
		case '\\':
			*d++='\\';
			break;
		// ���� ���� \�ϳ��� �ִ� ���. \���ڷ� �����ϰ� s�� ������Ű�� �ʰ� ����
		case NULL:
			*d++='\\';
			return;
		case 'n':
		case 'N':
			*d++='\r';
			*d++='\n';
			break;
		case 't':
		case 'T':
			*d++='\t';
			break;
		// ã�⿡���� �� �� �ʿ䰡 ����. �ٷ� "�� �� ���� �ִ�.
		case '\"':
			*d++='\"';
			break;
		// 16�� �ڵ� ó��
		case 'x':
		case 'X':
			s++;
			if (*s==NULL) return;
			high=*s;
			s++;
			if (*s==NULL) return;
			if (IsCharLower(high)) high-=0x20;
			if (high >= '0' && high <= '9') {
				high=high-'0';
			} else if (high >= 'A' && high <= 'F') {
				high=high-'A'+10;
			} else {
				break;
			}
			low=*s;
			if (IsCharLower(low)) low-=0x20;
			if (low >= '0' && low <= '9') {
				low=low-'0';
			} else if (low >= 'A' && low <= 'F') {
				low=low-'A'+10;
			} else {
				break;
			}
			*d++=(high << 4 | low);
			break;
		// 10�� �ڵ� ó��
		case 'i':
		case 'I':
			s++;
			if (*s==NULL) return;
			high=*s;
			s++;
			if (*s==NULL) return;
			if (high >= '0' && high <= '9') {
				high=high-'0';
			} else {
				s++;
				break;
			}
			low=*s;
			s++;
			if (*s==NULL) return;
			if (low >= '0' && low <= '9') {
				low=low-'0';
			} else {
				break;
			}
			third=*s;
			if (third >= '0' && third <= '9') {
				third=third-'0';
			} else {
				break;
			}
			code=high*100+low*10+third;
			if (code < 256) {
				*d++=code;
			}
			break;
		// Ȯ�忭�� �ƴ� ���ڴ� ������. ���� ��� ȥ�� ���� \r��.
		default:
			break;
		}
	} else {
		*d++=*s;
	}
	s++;
}

// ���縦 ã���ش�. ������ ��� -1�� �����Ѵ�. ���� �������� �����͸� �Ű� �ش�.
int GetComVerb(const TCHAR *&szCom,SMacVerb *arVerb)
{
	TCHAR szVerb[32];
	TCHAR *p;
	int idx;

	// ���� �κи� ���� �и��Ѵ�.
	lstrcpyn(szVerb,szCom,32);
	for (p=szVerb;*p!=NULL;p++) {
		if (*p==' ') {
			*p=NULL;
			break;
		}
	}

	for (idx=0;;idx++) {
		if (lstrlen(arVerb[idx].Name) == 0) {
			idx=-1;
			break;
		}
		if (strcmpi(arVerb[idx].Name,szVerb)==0) {
			break;
		}
	}

	szCom += lstrlen(szVerb);
	SkipSpace(szCom);
	return idx;
}

// ����� ã���ش�. ������ ��� -1�� �����Ѵ�. ����� �ִ� ���縸 �� �Լ��� ȣ���ϹǷ� -1�� �����ص� �� �������.
// ������ �������� �����͸� �Ű� �ش�.
int GetComObject(int Verb,const TCHAR *&szCom,SMacObject *arObject,int *arObj)
{
	TCHAR szObject[32];
	TCHAR *p;
	int stidx;
	int idx;

	// ������ �迭���� ���� �ε��� ã��. ������ -1�̸� ����� ���� ������
	stidx=arObj[Verb];
	if (stidx==-1) {
		return -1;
	}

	// ������ �κи� ���� �и��Ѵ�. �����̳� ���� ������ �������� ��
	lstrcpyn(szObject,szCom,32);
	for (p=szObject;*p!=NULL;p++) {
		if (*p==' ' || *p=='\t') {
			*p=NULL;
			break;
		}
	}

	for (idx=stidx;arObject[idx].Name!=NULL;idx++) {
		if (strcmpi(arObject[idx].Name,szObject)==0) {
			break;
		}
	}
	if (arObject[idx].Name==NULL) {
		idx=-1;
	} else {
		idx-=stidx;
	}

	// ������ �������� �����͸� �Ű��ش�.
	szCom += lstrlen(szObject);
	SkipSpace(szCom);
	return idx;
}

// ����ǥ ������ �ִ� ���ڿ��� ������ �ش�. �޸𸮸� ���⼭ �Ҵ��ϰ� ȣ������� �����Ѵ�.
// ���ڿ� �������� �����͸� �Ű��ش�.
void GetComString(const TCHAR *&szCom,TCHAR *&str,BOOL bEscape/*=TRUE*/)
{
	int len;
	const TCHAR *pc;
	TCHAR *p;

	// ����� �ִ� ���ڿ��� ���̸�ŭ �Ҵ�
	len=lstrlen(szCom);
	str=(TCHAR *)malloc(len);

	// ����ǥ ���� ���ں��� ���縦 �����Ѵ�. Ȯ�忭 ó��
	for (pc=szCom+1,p=str;;) {
		// �ݴ� ����ǥ�� ���ٴ� ����. ������ ó���ȴ�.
		if (*pc==NULL) {
			free(str);
			str=NULL;
			break;
		}
		// �ݴ� ����ǥ. ���ڿ��� ��
		if (*pc=='\"') {
			*p=0;
			break;
		}

		if (bEscape) {
			AeEscapeChar(pc,p);
		// Ȯ�忭�� Ȯ������ ���� ��. ��� ���ڸ� �״�� �����ϵ� ��, �� �������� \"�� �ν����� �ʵ���
		// �� Ȯ�忭�� ���� �������ش�.
		} else {
			if (*pc=='\\' && *(pc+1)=='\"') {
				*p++=*pc++;
				*p++=*pc++;
			} else {
				*p++=*pc++;
			}
		}
	}

	// ���ڿ� �������� �����͸� ������.
	szCom = pc+1;
	SkipSpace(szCom);
}

// szCom ���ڿ����� ���ڸ� �о��ش�. �ּ����� ���� ������ �� �ִ�.
int GetComNumber(const TCHAR *szCom,int Min)
{
	return max(Min,atoi(szCom));
}

CMcRecord::CMcRecord()
{
	int i;

	for (i=0;i<MAXMC;i++) {
		arMc[i]=(TCHAR *)malloc(DEFMCSIZE);
	}
	nowmc=0;
	runlevel=0;
}

CMcRecord::~CMcRecord()
{
	int i;

	for (i=0;i<MAXMC;i++) {
		free(arMc[i]);
	}
}

void CMcRecord::RecordCommand(const TCHAR *Com)
{
	TCHAR *pOld;
	int Count;
	const TCHAR *p;
	TCHAR szTemp[32];
	int MergeType=0;
	int Verb;
	TCHAR *pCount;
	TCHAR *s1,*s2;

	if (runlevel != 0) {
		return;
	}

	// �迭 ������ �Ѿ��� �� �迭�� ��ĭ ������ �����Ѵ�.
	if (nowmc == MAXMC) {
		pOld=arMc[0];
		memmove(&arMc[0],&arMc[1],sizeof(arMc[0])*(MAXMC-1));

		// ����ȭ ����. ������ ù ����� �޸𸮸� ��Ȱ���Ѵ�.
		// ũ�Ⱑ �ٸ��� ���� �� ���Ҵ��Ѵ�.
		if (_msize(pOld) == _msize(arMc[MAXMC-1])) {
			arMc[MAXMC-1]=pOld;
		} else {
			free(pOld);
			arMc[MAXMC-1]=(TCHAR *)malloc(DEFMCSIZE);
		}
		nowmc--;
	}

	// ������ ������ ���� ���� ���ɼ��� �Ǻ��Ѵ�. ù��° ����� ������ ���� �ȵ�
	p=Com;
	Verb=GetComVerb(p,arComVerb);
	if (nowmc != 0) {
		if ((arComVerb[Verb].Flag & 0x1) && (strncmp(Com,arMc[nowmc-1],lstrlen(Com))==0)) {
			MergeType=1;
		}
		if (Verb==AEC_INSERT && (strncmp(arMc[nowmc-1],"insert",6)==0)) {
			MergeType=2;
		}
	}

	switch (MergeType) {
	case 0:
		if (_msize(arMc[nowmc]) < (unsigned)lstrlen(Com)+1) {
			arMc[nowmc]=(TCHAR *)realloc(arMc[nowmc],lstrlen(Com)+1);
		}
		lstrcpy(arMc[nowmc],Com);
		nowmc++;
		break;
	case 1:
		pCount=arMc[nowmc-1]+lstrlen(Com);
		Count=GetComNumber(pCount,1);
		Count++;
		wsprintf(szTemp, " %d",Count);
		lstrcpy(pCount,szTemp);
		break;
	case 2:
		// ���ڿ� ���սô� Ȯ�忭�� Ȯ������ ���ƾ� �Ѵ�.
		GetComString(p,s1,FALSE);
		p=arMc[nowmc-1];
		GetComVerb(p,arComVerb);
		GetComString(p,s2,FALSE);

		// �޸� ����
		if (_msize(arMc[nowmc-1]) < (unsigned)10+lstrlen(s1)+lstrlen(s2)) {
			arMc[nowmc-1]=(TCHAR *)realloc(arMc[nowmc-1],_msize(arMc[nowmc-1])+10);
		}
		wsprintf(arMc[nowmc-1],"insert \"%s%s\"",s2,s1);
		free(s1);
		free(s2);
		// 
		break;
	}
}

void ReallocBuf(TCHAR *&mem,TCHAR *&now,int Require,int Extra)
{
	unsigned toff;
	toff=now-mem;
	if (_msize(mem) < toff+Require) {
		mem=(TCHAR *)realloc(mem,_msize(mem)+Extra);
		now=mem+toff;
	}
}

// Ȯ�忭�� ��ȯ�� �ش�. ��ȯ ���� ���̸� �����Ѵ�.
int AeEscapeString(TCHAR *src,TCHAR *dest)
{
	const TCHAR *s;
	TCHAR *d;
	s=src;
	d=dest;

	for (;*s;) {
		AeEscapeChar(s,d);
	}
	return d-dest;
}
