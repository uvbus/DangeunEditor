#ifndef __APIEDITUTIL_H
#define __APIEDITUTIL_H

enum {AEC_MOVE, AEC_MOVESEL, AEC_INSERT, AEC_DELETE, AEC_BS, AEC_SELECT, 
	AEC_COPY, AEC_COPYAPP, AEC_CUT, AEC_CUTAPP, AEC_PASTE, AEC_CONVERT, AEC_COPYCHAR, 
	AEC_SCROLL, AEC_SETMARK, AEC_GOMARK, AEC_FIND,AEC_REPLACE,AEC_FOCUS};
enum {AECM_LEFT, AECM_RIGHT, AECM_UP, AECM_DOWN, AECM_HOME, AECM_END, AECM_PGUP, 
	AECM_PGDN, AECM_WLEFT, AECM_WRIGHT, AECM_WORDHOME, AECM_WORDEND, AECM_DOCHOME, 
	AECM_DOCEND, AECM_PAREN,AECM_LAST, AECM_NEXTPARA, AECM_PREVPARA, AECM_PARAHOME, 
	AECM_PARAEND}; 
enum {AECS_WORD, AECS_LINE, AECS_PARA, AECS_DOC};
enum {AECC_UPPER, AECC_LOWER, AECC_REVERSECASE, AECC_UPPERFIRST, AECC_UPPSERSENT, 
	AECC_TOTAB, AECC_TOSAPCE, AECC_REMOVESPACE, AECC_INDENT, AECC_UNINDENT, 
	AECC_COMMENT, AECC_UNCOMMENT};
enum {AECS_UP, AECS_DOWN, AECS_PGUP, AECS_PGDN, AECS_HALFUP, AECS_HALFDOWN,
	AECS_LEFT, AECS_RIGHT, AECS_PAGELEFT, AECS_PAGERIGHT};

struct SMacVerb
{
	TCHAR *Name;
	TCHAR *Desc;
	DWORD Flag;
};

struct SMacObject
{
	TCHAR *Name;
	TCHAR *Desc;
};

extern SMacVerb arComVerb[];
extern SMacObject arComObject[];
extern int arObjOfVerb[];

TCHAR *FindString(TCHAR *buf, int buflen, int nPos, TCHAR* what,int len,BOOL bDown, BOOL bCase);
void SkipSpace(const TCHAR *&pc);
BOOL SkipString(const TCHAR *&pc);
void AeEscapeChar(const TCHAR *&s,TCHAR *&d);
int GetComVerb(const TCHAR *&szCom,SMacVerb *arVerb);
int GetComObject(int Verb,const TCHAR *&szCom,SMacObject *arObject,int *arObj);
void GetComString(const TCHAR *&szCom,TCHAR *&str,BOOL bEscape=TRUE);
int GetComNumber(const TCHAR *szCom,int Min);

#define MAXMC 64
#define DEFMCSIZE 32
#define AEVERBNUM 19

class CMcRecord
{
public:
	TCHAR *arMc[MAXMC];
	int nowmc;
	int runlevel;

	CMcRecord();
	~CMcRecord();
	void RecordCommand(const TCHAR *Com);
};

void ReallocBuf(TCHAR *&mem,TCHAR *&now,int Require,int Extra);
int AeEscapeString(TCHAR *src,TCHAR *dest);

#endif // __APIEDITUTIL_H

