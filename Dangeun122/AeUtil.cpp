//#include <windows.h>
#include "stdafx.h"

// 동사 배열
// Flag의 b0=반복 가능성, b1=목적어 유무(문자열 목적어는 제외)
SMacVerb arComVerb[]={
	{"move","이동",0x03},
	{"movesel","선택이동",0x03},
	{"insert","삽입",0x00},
	{"delete","삭제",0x01},
	{"bs","앞문자 삭제",0x03},
	{"select","선택",0x02},
	{"copy","복사",0x00},
	{"copyapp","복사해서 추가",0x00},
	{"cut","잘라내기",0x00},
	{"cutapp","잘라서 추가",0x00},
	{"paste","붙여넣기",0x01},
	{"convert","변환",0x03},
	{"copychar","위아래 문자 반복",0x03},
	{"scroll","스크롤",0x03},
	{"setmark","책갈피 설정",0x02},
	{"gomark","책갈피 이동",0x02},
	{"find","찾기",0x00},
	{"replace","바꾸기",0x00},
	{"focus","뷰 전환",0x03},
	{"","",0x00}
};

// 목적어 배열
SMacObject arComObject[]={
	{"left","왼쪽"},						// move:0
	{"right","오른쪽"},
	{"up","위로"},
	{"down","아래로"},
	{"home","줄 처음으로"},
	{"end","줄 끝으로"},
	{"pgup","페이지 위로"},
	{"pgdn","페이지 아래로"},
	{"wleft","이전 단어로"},
	{"wright","이후 단어"},
	{"wordhome","단어 처음으로"},
	{"wordend","단어 끝으로"},
	{"dochome","문서 처음"},
	{"docend","문서 끝"},
	{"paren","대응되는 괄호"},
	{"last","마지막 편집 위치"},
	{"nextpara","다음 문단"},
	{"prevpara","이전 문단"},
	{"parahome","문단 처음"},
	{"paraend","문단 끝"},
	{NULL,NULL},
	{"char","문자"},						// bs:21
	{"word","단어"},
	{NULL,NULL},
	{"word","단어"},						// select:24
	{"line","줄"},
	{"para","문단"},
	{"doc","문서"},
	{NULL,NULL},
	{"upper","대문자로"},					// convert:29
	{"lower","소문자로"},
	{"reversecase","대소 반대로"},
	{"upperfirst","첫글자만 대문자로"},
	{"uppersent","문장 처음만 대문자로"},
	{"totab","공백을 탭으로"},
	{"tospace","탭을 공백으로"},
	{"removespace","불필요한 공백 제거"},
	{"indent","들여쓰기"},
	{"unindent","내어쓰기"},
	{"comment","주석으로"},
	{"uncomment","주석제거"},
	{NULL,NULL},
	{"up","윗줄 문자"},						// copychar:42
	{"down","아래줄 문자"},
	{NULL,NULL},
	{"up","위"},							// scroll:45
	{"down","아래"},
	{"pgup","페이지 위"},
	{"pgdn","페이지 아래"},
	{"halfup","반 페이지 위"},
	{"halfdown","반 페이지 아래"},
	{"left","왼쪽"},
	{"right","오른쪽"},
	{"pageleft","페이지 왼쪽"},
	{"pageright","페이지 오른쪽"},
	{NULL,NULL},
	{"unname","이름없는 북마크"},			// setmark:56
	{"0","0번 북마크"},
	{"1","1번 북마크"},
	{"2","2번 북마크"},
	{"3","3번 북마크"},
	{"4","4번 북마크"},
	{"5","5번 북마크"},
	{"6","6번 북마크"},
	{"7","7번 북마크"},
	{"8","8번 북마크"},
	{"9","9번 북마크"},
	{NULL,NULL},
	{"next","다음 북마크"},					// gomark:68
	{"prev","이전 북마크"},
	{"0","0번 북마크"},
	{"1","1번 북마크"},
	{"2","2번 북마크"},
	{"3","3번 북마크"},
	{"4","4번 북마크"},
	{"5","5번 북마크"},
	{"6","6번 북마크"},
	{"7","7번 북마크"},
	{"8","8번 북마크"},
	{"9","9번 북마크"},
	{"next","다음 보기"},					// focus:80
	{"prev","이전 보기"},
	{NULL,NULL}
};

// 각 동사별 목적어 배열상의 시작 첨자
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

// 공백을 건너뛴다.
void SkipSpace(const TCHAR *&pc)
{
	for (;*pc;pc++) {
		if (*pc!=' ' && *pc!='\t')
			break;
	}
}

// 문자열을 만나면 문자열 끝까지 건너뜀. pc는 닫는 따옴표에 위치함.
// 닫는 따옴표가 없으면 FALSE를 리턴한다.
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

// 문자 하나를 이스케이프한다. 위치 이동까지 해 준다.
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
		// 제일 끝에 \하나만 있는 경우. \문자로 인정하고 s를 증가시키지 않고 리턴
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
		// 찾기에서는 꼭 할 필요가 없다. 바로 "를 쓸 수도 있다.
		case '\"':
			*d++='\"';
			break;
		// 16진 코드 처리
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
		// 10진 코드 처리
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
		// 확장열이 아닌 문자는 무시함. 예를 들어 혼자 오는 \r등.
		default:
			break;
		}
	} else {
		*d++=*s;
	}
	s++;
}

// 동사를 찾아준다. 에러일 경우 -1을 리턴한다. 동사 다음으로 포인터를 옮겨 준다.
int GetComVerb(const TCHAR *&szCom,SMacVerb *arVerb)
{
	TCHAR szVerb[32];
	TCHAR *p;
	int idx;

	// 동사 부분만 따로 분리한다.
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

// 목적어를 찾아준다. 에러일 경우 -1을 리턴한다. 목적어가 있는 동사만 이 함수를 호출하므로 -1을 리턴해도 별 상관없다.
// 목적어 다음으로 포인터를 옮겨 준다.
int GetComObject(int Verb,const TCHAR *&szCom,SMacObject *arObject,int *arObj)
{
	TCHAR szObject[32];
	TCHAR *p;
	int stidx;
	int idx;

	// 목적어 배열에서 시작 인덱스 찾음. 시작이 -1이면 목적어가 없는 동사임
	stidx=arObj[Verb];
	if (stidx==-1) {
		return -1;
	}

	// 목적어 부분만 따로 분리한다. 공백이나 탭을 만나면 목적어의 끝
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

	// 목적어 다음으로 포인터를 옮겨준다.
	szCom += lstrlen(szObject);
	SkipSpace(szCom);
	return idx;
}

// 따옴표 다음에 있는 문자열을 추출해 준다. 메모리를 여기서 할당하고 호출원에서 해제한다.
// 문자열 다음으로 포인터를 옮겨준다.
void GetComString(const TCHAR *&szCom,TCHAR *&str,BOOL bEscape/*=TRUE*/)
{
	int len;
	const TCHAR *pc;
	TCHAR *p;

	// 추출될 최대 문자열의 길이만큼 할당
	len=lstrlen(szCom);
	str=(TCHAR *)malloc(len);

	// 따옴표 다음 문자부터 복사를 시작한다. 확장열 처리
	for (pc=szCom+1,p=str;;) {
		// 닫는 따옴표가 없다는 뜻임. 에러로 처리된다.
		if (*pc==NULL) {
			free(str);
			str=NULL;
			break;
		}
		// 닫는 따옴표. 문자열의 끝
		if (*pc=='\"') {
			*p=0;
			break;
		}

		if (bEscape) {
			AeEscapeChar(pc,p);
		// 확장열을 확장하지 않을 때. 모든 문자를 그대로 조사하되 단, 위 루프에서 \"를 인식하지 않도록
		// 이 확장열만 따로 복사해준다.
		} else {
			if (*pc=='\\' && *(pc+1)=='\"') {
				*p++=*pc++;
				*p++=*pc++;
			} else {
				*p++=*pc++;
			}
		}
	}

	// 문자열 다음으로 포인터를 보낸다.
	szCom = pc+1;
	SkipSpace(szCom);
}

// szCom 문자열에서 숫자를 읽어준다. 최소한의 값을 지정할 수 있다.
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

	// 배열 범위를 넘었을 때 배열을 한칸 앞으로 복사한다.
	if (nowmc == MAXMC) {
		pOld=arMc[0];
		memmove(&arMc[0],&arMc[1],sizeof(arMc[0])*(MAXMC-1));

		// 단편화 방지. 지워진 첫 기록의 메모리를 재활용한다.
		// 크기가 다르면 해제 후 재할당한다.
		if (_msize(pOld) == _msize(arMc[MAXMC-1])) {
			arMc[MAXMC-1]=pOld;
		} else {
			free(pOld);
			arMc[MAXMC-1]=(TCHAR *)malloc(DEFMCSIZE);
		}
		nowmc--;
	}

	// 동사의 종류를 보고 병합 가능성을 판별한다. 첫번째 기록은 무조건 병합 안됨
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
		// 문자열 병합시는 확장열을 확장하지 말아야 한다.
		GetComString(p,s1,FALSE);
		p=arMc[nowmc-1];
		GetComVerb(p,arComVerb);
		GetComString(p,s2,FALSE);

		// 메모리 관리
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

// 확장열을 변환해 준다. 변환 후의 길이를 리턴한다.
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
