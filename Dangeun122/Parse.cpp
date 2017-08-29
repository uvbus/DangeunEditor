//#include <windows.h>
//#include "Parse.h"
//#include "ApiEdit.h"
//#include "ApiEditText.h"
#include "stdafx.h"

SParseStyle::SParseStyle()
{
	name[0]=0;
	fore=(DWORD)-2;
	back=(DWORD)-1;
}

CParse::CParse()
{
	pInfo=NULL;
	InitInfo(TRUE);
}

CParse::~CParse()
{
	InitInfo(FALSE);
}

void CParse::InitInfo(BOOL bAlloc)
{
	int i;

	if (pInfo) {
		for (i=0;i<ParseSize;i++) {
			if (pInfo[i].pUnit) {
				free(pInfo[i].pUnit);
			}
		}
		free(pInfo);
	}

	if (bAlloc) {
		ParseSize=100;
		pInfo=(ParseInfo *)malloc(sizeof(ParseInfo)*ParseSize);
		for (i=0;i<ParseSize;i++) {
			pInfo[i].UnitSize=5;
			pInfo[i].pUnit=(ParseUnit *)malloc(sizeof(ParseUnit)*pInfo[i].UnitSize);
			memset(pInfo[i].pUnit,-1,sizeof(ParseUnit)*pInfo[i].UnitSize);
			pInfo[i].Context=0;
		}
	}
}

void CParse::MakeParseInfo(int nLine, int &nUnit, int pos, int style)
{
	if (nUnit > 0) {
		if (pInfo[nLine].pUnit[nUnit-1].pos == pos) {
			nUnit--;
		} else 
		if (pInfo[nLine].pUnit[nUnit-1].style == style) {
			return;
		}
	}

	if (pInfo[nLine].UnitSize == nUnit) {
		pInfo[nLine].UnitSize++;
		pInfo[nLine].pUnit=(ParseUnit *)realloc(pInfo[nLine].pUnit,
			sizeof(ParseUnit)*pInfo[nLine].UnitSize);
	}

	pInfo[nLine].pUnit[nUnit].pos=pos;
	pInfo[nLine].pUnit[nUnit].style=style;
	nUnit++;
}

void CParse::DeleteParseInfo(int nLine)
{
	int l;

	for (l=nLine;;l++) {
		if (l>=ParseSize || pInfo[l].pUnit[0].pos == -1)
			break;

		memset(pInfo[l].pUnit,-1,sizeof(ParseUnit)*pInfo[l].UnitSize);
	}
}

void CParse::ParseLines(CApiEditText &ae,int nLine,Ae_PrintInfo *pi)
{
	int l,i;
	int OldSize;

	if (nLine >= ParseSize) {
		OldSize=ParseSize;
		ParseSize=nLine+100;
		pInfo=(ParseInfo *)realloc(pInfo,sizeof(ParseInfo)*ParseSize);
		for (i=OldSize;i<ParseSize;i++) {
			pInfo[i].UnitSize=5;
			pInfo[i].pUnit=(ParseUnit *)malloc(sizeof(ParseUnit)*pInfo[i].UnitSize);
			memset(pInfo[i].pUnit,-1,sizeof(ParseUnit)*pInfo[i].UnitSize);
			pInfo[i].Context=0;
		}
	}

	if (pInfo[nLine].pUnit[0].pos != -1)
		return;

	for (l=nLine;l>=0;l--) {
		if (pInfo[l].pUnit[0].pos != -1)
			break;
	}
	l++;

	for (i=l;i<=nLine;i++) {
		ParseLine(ae,i,pi);
	}
}

BOOL CParse::IsStringExist(TCHAR *list,TCHAR *str,int len,BOOL bCase)
{
	TCHAR *tbuf;
	BOOL ret;

	tbuf=(TCHAR *)malloc(len+3);
	tbuf[0]=' ';
	lstrcpyn(tbuf+1,str,len+1);
	tbuf[len+1]=' ';
	tbuf[len+2]=0;
	if (bCase==FALSE) {
		CharLower(tbuf);
	}
	ret=(strstr(list,tbuf)!=NULL);
	free(tbuf);
	return ret;
}

BOOL CParse::IsNumber(CApiEditText &ae,int s, int e)
{
	int i;
	TCHAR ch;

	if (ae.pFrame->buf[s]=='0' && (ae.pFrame->buf[s+1]=='x' || ae.pFrame->buf[s+1]=='X')) {
		for (i=s+2;i<=e;i++) {
			ch=ae.pFrame->buf[i];
			if (!((ch >= '0' && ch <= '9') || 
				  (ch >= 'a' && ch <= 'f') || 
				  (ch >= 'A' && ch <= 'F')))
				return FALSE;
		}
	} else {
		for (i=s;i<=e;i++) {
			ch=ae.pFrame->buf[i];
			if (!(ch >= '0' && ch <= '9'))
				return FALSE;
		}
	}

	return TRUE;
}

void CParse::GetStyleColor(int style, COLORREF &fore, COLORREF &back)
{
	assert(style >= 0 && style < 32);
	
	fore=arStyle[style].fore;
	back=arStyle[style].back;
}

void CParse::SetStyleColor(int style, COLORREF fore, COLORREF back)
{
	arStyle[style].fore=fore;
	arStyle[style].back=back;
}

void CParse::GetStyleName(int style, TCHAR *szName)
{
	assert(style >= 0 && style < 32);
	
	lstrcpy(szName,arStyle[style].name);
}

CParseNull::CParseNull()
{
	lstrcpy(arStyle[0].name,"보통");
	arStyle[0].fore=(DWORD)-1;
	arStyle[0].back=(DWORD)-1;
}

TCHAR *CParseNull::GetInfo(int iIndex)
{
	switch (iIndex) {
	case 0:
		return (TCHAR *)0;
	case 1:
		return TEXT(" \t\r\n\"\'\\.,<>:;/()[]{}~!@#%^&*-+=?|$");
	default:
		return TEXT("");
	}
	return 0;
}

void CParseNull::ParseLine(CApiEditText &ae,int nLine,Ae_PrintInfo *pi)
{
	int nUnit=0;
	tagLine *tLine;

	if (pi) {
		tLine=pi->pLine;
	} else {
		tLine=ae.pLine;
	}

	MakeParseInfo(nLine,nUnit,tLine[nLine].Start,0);
}

enum {
	CPP_CON_NORMAL,
	CPP_CON_BLOCKCOMMENT,
	CPP_CON_LINECOMMENT,
	CPP_CON_STRING,
	CPP_CON_CHAR
};

enum {
	CPP_STYLE_NORMAL,
	CPP_STYLE_COMMENT,
	CPP_STYLE_STRING,
	CPP_STYLE_CHAR,
	CPP_STYLE_NUMBER,
	CPP_STYLE_KEYWORD,
	CPP_STYLE_PREPROCESSOR
};

CParseCpp::CParseCpp()
{
	lstrcpy(arStyle[0].name,"보통");
	arStyle[0].fore=(DWORD)-1;
	lstrcpy(arStyle[1].name,"주석");
	arStyle[1].fore=RGB(0,0,0);
	arStyle[1].back=RGB(192,192,192);
	lstrcpy(arStyle[2].name,"문자열");
	arStyle[2].fore=RGB(0,128,0);
	lstrcpy(arStyle[3].name,"문자");
	arStyle[3].fore=RGB(0,128,0);;
	lstrcpy(arStyle[4].name,"숫자");
	arStyle[4].fore=RGB(255,0,0);
	lstrcpy(arStyle[5].name,"키워드");
	arStyle[5].fore=RGB(0,0,255);
	lstrcpy(arStyle[6].name,"전처리기");
	arStyle[6].fore=RGB(255,0,255);
}

TCHAR *CParseCpp::GetInfo(int iIndex)
{
	switch (iIndex) {
	case 0:
		return (TCHAR *)1;
	case 1:
		return TEXT(" \t\r\n\"\'\\.,<>:;/()[]{}~!@#%^&*-+=?|");
	case 2:
		return TEXT("//");
	case 3:
		return TEXT("/*");
	case 4:
		return TEXT("*/");
	case 5:
		return TEXT("{");
	case 6:
		return TEXT("}");
	}
	return 0;
}

void CParseCpp::ParseLine(CApiEditText &ae,int nLine,Ae_PrintInfo *pi)
{
	DWORD Context;
	int s,e,i;
	int nUnit=0;
	int idpos,idend;
	tagLine *tLine;

	if (pi) {
		tLine=pi->pLine;
	} else {
		tLine=ae.pLine;
	}

	if (pInfo[nLine].pUnit[0].pos != -1)
		return;

	if (nLine == 0) {
		Context=CPP_CON_NORMAL;
	} else {
		Context=pInfo[nLine-1].Context;
	}

	s=tLine[nLine].Start;
	e=tLine[nLine].End;
	idpos=s;

	switch(Context) {
	case CPP_CON_NORMAL:
		MakeParseInfo(nLine,nUnit,s,CPP_STYLE_NORMAL);
		break;
	case CPP_CON_STRING:
		MakeParseInfo(nLine,nUnit,s,CPP_STYLE_STRING);
		break;
	case CPP_CON_CHAR:
		MakeParseInfo(nLine,nUnit,s,CPP_STYLE_CHAR);
		break;
	case CPP_CON_BLOCKCOMMENT:
		MakeParseInfo(nLine,nUnit,s,CPP_STYLE_COMMENT);
		break;
	case CPP_CON_LINECOMMENT:
		if (tLine[nLine].nLine != 0) {
			MakeParseInfo(nLine,nUnit,s,CPP_STYLE_COMMENT);
			goto EndParse;
		} else {
			MakeParseInfo(nLine,nUnit,s,CPP_STYLE_NORMAL);
			Context=CPP_CON_NORMAL;
			break;
		}
	}

	for (i=s;i<e;i++) {
		switch(Context) {
		case CPP_CON_NORMAL:
			if (ae.pFrame->buf[i]=='/' && ae.pFrame->buf[i+1]=='*') {
				Context=CPP_CON_BLOCKCOMMENT;
			}
			if (ae.pFrame->buf[i]=='/' && ae.pFrame->buf[i+1]=='/') {
				Context=CPP_CON_LINECOMMENT;
			}
			if (ae.pFrame->buf[i]=='\"') {
				Context=CPP_CON_STRING;
			}
			if (ae.pFrame->buf[i]=='\'') {
				Context=CPP_CON_CHAR;
			}

			if (Context!=CPP_CON_NORMAL || strchr(GetInfo(1),ae.pFrame->buf[i]) || i==e-1) {
				if (i==e-1 && !strchr(GetInfo(1),ae.pFrame->buf[i])) {
					idend=i;
				} else {
					idend=i-1;
				}

				if (idend-idpos+1 >= 1) {
					if (IsKeyword(ae,idpos,idend)) {
						MakeParseInfo(nLine,nUnit,idpos,CPP_STYLE_KEYWORD);
						MakeParseInfo(nLine,nUnit,idend+1,CPP_STYLE_NORMAL);
					} else
					if (IsNumber(ae,idpos,idend)) {
						MakeParseInfo(nLine,nUnit,idpos,CPP_STYLE_NUMBER);
						MakeParseInfo(nLine,nUnit,idend+1,CPP_STYLE_NORMAL);
					} else
					if (idpos!=s && ae.pFrame->buf[idpos-1]=='#') {
						if (IsPreProcessor(ae,idpos-1,idend)) {
							MakeParseInfo(nLine,nUnit,idpos-1,CPP_STYLE_PREPROCESSOR);
							MakeParseInfo(nLine,nUnit,idend+1,CPP_STYLE_NORMAL);
						}
					}
				}

				idpos=i+1;
			}

			switch (Context) {
			case CPP_CON_NORMAL:
				break;
			case CPP_CON_BLOCKCOMMENT:
				MakeParseInfo(nLine,nUnit,i,CPP_STYLE_COMMENT);
				i++;
				break;
			case CPP_CON_LINECOMMENT:
				MakeParseInfo(nLine,nUnit,i,CPP_STYLE_COMMENT);
				goto EndParse;
			case CPP_CON_STRING:
				MakeParseInfo(nLine,nUnit,i,CPP_STYLE_STRING);
				break;
			case CPP_CON_CHAR:
				MakeParseInfo(nLine,nUnit,i,CPP_STYLE_CHAR);
				break;
			}
			continue;
		case CPP_CON_BLOCKCOMMENT:
			if (ae.pFrame->buf[i]=='*' && ae.pFrame->buf[i+1]=='/') {
				i++;
				MakeParseInfo(nLine,nUnit,i+1,CPP_STYLE_NORMAL);
				Context=CPP_CON_NORMAL;
				idpos=i+1;
			}
			continue;
		case CPP_CON_STRING:
			if (ae.pFrame->buf[i]=='\\') {
				i++;
				continue;
			}
			if (ae.pFrame->buf[i]=='\"') {
				MakeParseInfo(nLine,nUnit,i+1,CPP_STYLE_NORMAL);
				Context=CPP_CON_NORMAL;
				idpos=i+1;
			}
			continue;
		case CPP_CON_CHAR:
			if (ae.pFrame->buf[i]=='\\') {
				i++;
				continue;
			}
			if (ae.pFrame->buf[i]=='\'') {
				MakeParseInfo(nLine,nUnit,i+1,CPP_STYLE_NORMAL);
				Context=CPP_CON_NORMAL;
				idpos=i+1;
			}
			continue;
		}
	}

	if (Context==CPP_CON_STRING || Context==CPP_CON_CHAR) {
		if (ae.pFrame->buf[e]=='\r' && ae.pFrame->buf[e-1]!='\\') {
			Context=CPP_CON_NORMAL;
		}
	}

EndParse:
	pInfo[nLine].Context=Context;
}

BOOL CParseCpp::IsKeyword(CApiEditText &ae,int s, int e)
{
	static TCHAR *keyword=" auto break bool case char class const "
		"continue default delete do double else explicit enum "
		"extern float for friend goto if inline int long "
		"new namespace operator private protected public register "
		"return short signed sizeof static struct switch "
		"template this try typedef union unsigned using "
		"virtual void volatile while __asm __fastcall __based "
		"__cdecl __pascal __inline ";

	return IsStringExist(keyword,ae.pFrame->buf+s,e-s+1,TRUE);
}

BOOL CParseCpp::IsPreProcessor(CApiEditText &ae,int s, int e)
{
	static TCHAR *preproc=" #define #elif #else #endif #error #if #ifdef "
		"#ifndef #include #line #pragma #undef ";

	return IsStringExist(preproc,ae.pFrame->buf+s,e-s+1,TRUE);
} 

CParseJava::CParseJava()
{
	arStyle[1].back=RGB(255,249,157);
	lstrcpy(arStyle[2].name,"문자열");
	arStyle[2].fore=RGB(255,0,255);
	lstrcpy(arStyle[3].name,"문자");
	arStyle[3].fore=RGB(255,0,255);
	lstrcpy(arStyle[4].name,"숫자");
	arStyle[4].fore=RGB(0,128,0);
}

TCHAR *CParseJava::GetInfo(int iIndex)
{
	switch (iIndex) {
	case 0:
		return (TCHAR *)4;
	case 1:
		return TEXT(" \t\r\n\"\'\\.,<>:;/()[]{}~!@#%^&*-+=?|");
	case 2:
		return TEXT("//");
	case 3:
		return TEXT("/*");
	case 4:
		return TEXT("*/");
	case 5:
		return TEXT("{");
	case 6:
		return TEXT("}");
	}
	return 0;
}

BOOL CParseJava::IsKeyword(CApiEditText &ae,int s, int e)
{
	static TCHAR *keyword=" abstract continue for new switch assert "
		"default if package synchronized boolean do goto private this "
		"break double implements protected int byte else import public "
		"throw throws case enum instanceof return transient catch extends "
		"short try char final interface static void class finally long "
		"strictfp volatile const float native super while "
		"true false null ";

	return IsStringExist(keyword,ae.pFrame->buf+s,e-s+1,TRUE);
}

BOOL CParseJava::IsPreProcessor(CApiEditText &ae,int s, int e)
{
	static TCHAR *preproc="";

	return IsStringExist(preproc,ae.pFrame->buf+s,e-s+1,TRUE);
}

CParseCS::CParseCS()
{
	lstrcpy(arStyle[1].name,"주석");
	arStyle[1].fore=RGB(128,128,128);
	arStyle[1].back=DWORD(-1);
}

TCHAR *CParseCS::GetInfo(int iIndex)
{
	switch (iIndex) {
	case 0:
		return (TCHAR *)5;
	case 1:
		return TEXT(" \t\r\n\"\'\\.,<>:;/()[]{}~!@#%^&*-+=?|");
	case 2:
		return TEXT("//");
	case 3:
		return TEXT("/*");
	case 4:
		return TEXT("*/");
	case 5:
		return TEXT("{");
	case 6:
		return TEXT("}");
	}
	return 0;
}

BOOL CParseCS::IsKeyword(CApiEditText &ae,int s, int e)
{
	static TCHAR *keyword=" abstract as base bool break byte case catch char "
		"checked class const continue decimal default delegate do double else "
		"enum event explicit extern false finally fixed float for foreach goto "
		"if implicit in int interface internal is lock long namespace new null "
		"object operator out override params private protected public readonly "
		"ref return sbyte sealed short sizeof stackalloc static string struct "
		"switch this throw true try typeof uint ulong unchecked unsafe ushort "
		"using virtual void volatile while ";

	return IsStringExist(keyword,ae.pFrame->buf+s,e-s+1,TRUE);
}

BOOL CParseCS::IsPreProcessor(CApiEditText &ae,int s, int e)
{
	static TCHAR *preproc=" #define #if #elif #else #endif #error #warning "
		" #line #undef #region #endregion ";

	return IsStringExist(preproc,ae.pFrame->buf+s,e-s+1,TRUE);
}

enum {
	HTML_CON_NORMAL,
	HTML_CON_BRACKET,
	HTML_CON_COMMENT,
	HTML_CON_VALUE1,
	HTML_CON_VALUE2,
	HTML_CON_ASP,
	HTML_CON_ASPLINECOM,
	HTML_CON_ASPSTRING,
	HTML_CON_PHP,
	HTML_CON_PHPSTRING,
	HTML_CON_PHPLINECOM,
	HTML_CON_PHPBLOCKCOM
};

enum {
	HTML_STYLE_NORMAL,
	HTML_STYLE_TAG,
	HTML_STYLE_ATTR,
	HTML_STYLE_VALUE,
	HTML_STYLE_COMMENT,
	HTML_STYLE_ASP,
	HTML_STYLE_ASPKEY,
	HTML_STYLE_ASPSTRING,
	HTML_STYLE_ASPCOMMENT,
	HTML_STYLE_PHP,
	HTML_STYLE_PHPKEY,
	HTML_STYLE_PHPSTRING,
	HTML_STYLE_PHPCOMMENT,
	HTML_STYLE_PHPFUNCTION
};

CParseHtml::CParseHtml()
{
	lstrcpy(arStyle[0].name,"보통");
	arStyle[0].fore=(DWORD)-1;
	lstrcpy(arStyle[1].name,"태그");
	arStyle[1].fore=RGB(0,0,255);
	lstrcpy(arStyle[2].name,"속성");
	arStyle[2].fore=RGB(255,0,0);
	lstrcpy(arStyle[3].name,"속성값");
	arStyle[3].fore=RGB(0,128,0);
	lstrcpy(arStyle[4].name,"주석");
	arStyle[4].fore=RGB(0,0,0);
	arStyle[4].back=RGB(192,192,192);
	lstrcpy(arStyle[5].name,"ASP 스크립트");
	arStyle[5].fore=(DWORD)-1;
	arStyle[5].back=RGB(141,207,244);
	lstrcpy(arStyle[6].name,"ASP 키워드");
	arStyle[6].fore=(DWORD)RGB(0,0,255);
	arStyle[6].back=RGB(141,207,244);
	lstrcpy(arStyle[7].name,"ASP 문자열");
	arStyle[7].fore=RGB(0,128,0);
	arStyle[7].back=RGB(141,207,244);
	lstrcpy(arStyle[8].name,"ASP 주석");
	arStyle[8].fore=RGB(128,128,128);
	arStyle[8].back=RGB(141,207,244);
	lstrcpy(arStyle[9].name,"PHP 스크립트");
	arStyle[9].fore=(DWORD)-1;
	arStyle[9].back=RGB(255,249,157);
	lstrcpy(arStyle[10].name,"PHP 키워드");
	arStyle[10].fore=RGB(0,0,255);
	arStyle[10].back=RGB(255,249,157);
	lstrcpy(arStyle[11].name,"PHP 문자열");
	arStyle[11].fore=RGB(0,128,0);
	arStyle[11].back=RGB(255,249,157);
	lstrcpy(arStyle[12].name,"PHP 주석");
	arStyle[12].fore=RGB(128,128,128);
	arStyle[12].back=RGB(255,249,157);
	lstrcpy(arStyle[14].name,"PHP 함수");
	arStyle[13].fore=RGB(255,0,0);
	arStyle[13].back=RGB(255,249,157);
}

TCHAR *CParseHtml::GetInfo(int iIndex)
{
	switch (iIndex) {
	case 0:
		return (TCHAR *)2;
	case 1:
		return TEXT(" \t\r\n\"\'\\.,<>:;/()[]{}~!@#%^&*-+=?|$");
	case 2:
		return TEXT("");
	case 3:
		return TEXT("<!--");
	case 4:
		return TEXT("-->");
	case 5:
		return TEXT("");
	case 6:
		return TEXT("");
	}
	return 0;
}

void CParseHtml::ParseLine(CApiEditText &ae,int nLine,Ae_PrintInfo *pi)
{
	DWORD Context;
	int s,e,i;
	int nUnit=0;
	int idpos,idend;
	tagLine *tLine;

	if (pi) {
		tLine=pi->pLine;
	} else {
		tLine=ae.pLine;
	}

	if (pInfo[nLine].pUnit[0].pos != -1)
		return;

	if (nLine == 0) {
		Context=HTML_CON_NORMAL;
	} else {
		Context=pInfo[nLine-1].Context;
	}

	s=tLine[nLine].Start;
	e=tLine[nLine].End;
	idpos=s;

	switch(Context) {
	case HTML_CON_NORMAL:
	case HTML_CON_BRACKET:
		MakeParseInfo(nLine,nUnit,s,HTML_STYLE_NORMAL);
		break;
	case HTML_CON_COMMENT:
		MakeParseInfo(nLine,nUnit,s,HTML_STYLE_COMMENT);
		break;
	case HTML_CON_VALUE1:
	case HTML_CON_VALUE2:
		MakeParseInfo(nLine,nUnit,s,HTML_STYLE_VALUE);
		break;
	case HTML_CON_ASP:
		MakeParseInfo(nLine,nUnit,s,HTML_STYLE_ASP);
		break;
	case HTML_CON_ASPSTRING:
		MakeParseInfo(nLine,nUnit,s,HTML_STYLE_ASPSTRING);
		break;
	case HTML_CON_PHP:
		MakeParseInfo(nLine,nUnit,s,HTML_STYLE_PHP);
		break;
	case HTML_CON_PHPSTRING:
		MakeParseInfo(nLine,nUnit,s,HTML_STYLE_PHPSTRING);
		break;
	case HTML_CON_PHPLINECOM:
		if (tLine[nLine].nLine != 0) {
			MakeParseInfo(nLine,nUnit,s,HTML_STYLE_PHPCOMMENT);
			goto EndParse;
		} else {
			MakeParseInfo(nLine,nUnit,s,HTML_STYLE_PHP);
			Context=HTML_CON_PHP;
			break;
		}
		break;
	case HTML_CON_ASPLINECOM:
		if (tLine[nLine].nLine != 0) {
			MakeParseInfo(nLine,nUnit,s,HTML_STYLE_ASPCOMMENT);
			goto EndParse;
		} else {
			MakeParseInfo(nLine,nUnit,s,HTML_STYLE_ASP);
			Context=HTML_CON_ASP;
			break;
		}
		break;
	case HTML_CON_PHPBLOCKCOM:
		MakeParseInfo(nLine,nUnit,s,HTML_STYLE_COMMENT);
		break;
	}

	for (i=s;i<e;i++) {
		switch(Context) {
		case HTML_CON_NORMAL:
			if (ae.pFrame->buf[i]=='<' && ae.pFrame->buf[i+1]=='!' && ae.pFrame->buf[i+2]=='-' && ae.pFrame->buf[i+3]=='-') {
				MakeParseInfo(nLine,nUnit,i,HTML_STYLE_COMMENT);
				i+=3;
				Context=HTML_CON_COMMENT;
				continue;
			}
			if (ae.pFrame->buf[i]=='<' && ae.pFrame->buf[i+1]=='%') {
				MakeParseInfo(nLine,nUnit,i,HTML_STYLE_ASP);
				Context=HTML_CON_ASP;
				continue;
			}
			if (ae.pFrame->buf[i]=='<' && ae.pFrame->buf[i+1]=='?') {
				MakeParseInfo(nLine,nUnit,i,HTML_STYLE_PHP);
				Context=HTML_CON_PHP;
				continue;
			}
			if (ae.pFrame->buf[i]=='<') {
				MakeParseInfo(nLine,nUnit,i,HTML_STYLE_TAG);
				MakeParseInfo(nLine,nUnit,i+1,HTML_STYLE_NORMAL);
				Context=HTML_CON_BRACKET;
				idpos=i+1;
				continue;
			}
			continue;
		case HTML_CON_BRACKET:
			if (ae.pFrame->buf[i]=='\"') {
				MakeParseInfo(nLine,nUnit,i,HTML_STYLE_VALUE);
				Context=HTML_CON_VALUE1;
				continue;
			}
			if (ae.pFrame->buf[i]=='\'') {
				MakeParseInfo(nLine,nUnit,i,HTML_STYLE_VALUE);
				Context=HTML_CON_VALUE2;
				continue;
			}
			if (strchr(GetInfo(1),ae.pFrame->buf[i]) || i==e-1) {
				if (i==e-1 && !strchr(GetInfo(1),ae.pFrame->buf[i])) {
					idend=i;
				} else {
					idend=i-1;
				}
				if (idend-idpos+1 >= 1) {
					if ((idpos > 0 && ae.pFrame->buf[idpos-1] == '<') || (idpos > 1 && ae.pFrame->buf[idpos-2]=='<' && ae.pFrame->buf[idpos-1]=='/')) {
						if (IsTag(ae,idpos,idend)) {
							if (idpos > s && ae.pFrame->buf[idpos-1] == '/' )
								idpos--;
							MakeParseInfo(nLine,nUnit,idpos,HTML_STYLE_TAG);
							MakeParseInfo(nLine,nUnit,idend+1,HTML_STYLE_NORMAL);
						}
					} else
					if (IsAttr(ae,idpos,idend)) {
						MakeParseInfo(nLine,nUnit,idpos,HTML_STYLE_ATTR);
						MakeParseInfo(nLine,nUnit,idend+1,HTML_STYLE_NORMAL);
					}
				}

				idpos=i+1;
			}

			if (ae.pFrame->buf[i]=='>') {
				MakeParseInfo(nLine,nUnit,i,HTML_STYLE_TAG);
				MakeParseInfo(nLine,nUnit,i+1,HTML_STYLE_NORMAL);
				Context=HTML_CON_NORMAL;
				continue;
			}
			continue;
		case HTML_CON_COMMENT:
			if (ae.pFrame->buf[i]=='-' && ae.pFrame->buf[i+1]=='-' && ae.pFrame->buf[i+2]=='>') {
				i+=2;
				MakeParseInfo(nLine,nUnit,i+1,HTML_STYLE_NORMAL);
				Context=HTML_CON_NORMAL;
				idpos=i+1;
			}
			continue;
		case HTML_CON_VALUE1:
			if (ae.pFrame->buf[i]=='\\') {
				i++;
				continue;
			}
			if (ae.pFrame->buf[i]=='\"') {
				MakeParseInfo(nLine,nUnit,i+1,HTML_STYLE_NORMAL);
				Context=HTML_CON_BRACKET;
				idpos=i+1;
			}
			continue;
		case HTML_CON_VALUE2:
			if (ae.pFrame->buf[i]=='\\') {
				i++;
				continue;
			}
			if (ae.pFrame->buf[i]=='\'') {
				MakeParseInfo(nLine,nUnit,i+1,HTML_STYLE_NORMAL);
				Context=HTML_CON_BRACKET;
				idpos=i+1;
			}
			continue;
		case HTML_CON_ASP:
			if (ae.pFrame->buf[i]=='%' && ae.pFrame->buf[i+1]=='>') {
				i++;
				MakeParseInfo(nLine,nUnit,i+1,HTML_STYLE_NORMAL);
				Context=HTML_CON_NORMAL;
				idpos=i+1;
			}
			if (ae.pFrame->buf[i]=='\"') {
				MakeParseInfo(nLine,nUnit,i,HTML_STYLE_ASPSTRING);
				Context=HTML_CON_ASPSTRING;
				continue;
			}
			if (ae.pFrame->buf[i]=='\'') {
				MakeParseInfo(nLine,nUnit,i,HTML_STYLE_ASPCOMMENT);
				Context=HTML_CON_ASPLINECOM;
				goto EndParse;
			}
			if (strchr(GetInfo(1),ae.pFrame->buf[i]) || i==e-1) {
				if (i==e-1 && !strchr(GetInfo(1),ae.pFrame->buf[i])) {
					idend=i;
				} else {
					idend=i-1;
				}
				if (idend-idpos+1 >= 1) {
					if (IsAspKeyword(ae,idpos,idend)) {
						MakeParseInfo(nLine,nUnit,idpos,HTML_STYLE_ASPKEY);
						MakeParseInfo(nLine,nUnit,idend+1,HTML_STYLE_ASP);
					}
				}

				idpos=i+1;
			}
			continue;
		case HTML_CON_ASPSTRING:
			if (ae.pFrame->buf[i]=='%' && ae.pFrame->buf[i+1]=='>') {
				i++;
				MakeParseInfo(nLine,nUnit,i+1,HTML_STYLE_NORMAL);
				Context=HTML_CON_NORMAL;
				idpos=i+1;
			}
			if (ae.pFrame->buf[i]=='\"') {
				MakeParseInfo(nLine,nUnit,i+1,HTML_STYLE_ASP);
				Context=HTML_CON_ASP;
				idpos=i+1;
			}
			continue;
		case HTML_CON_PHP:
			if (ae.pFrame->buf[i]=='?' && ae.pFrame->buf[i+1]=='>') {
				i++;
				MakeParseInfo(nLine,nUnit,i+1,HTML_STYLE_NORMAL);
				Context=HTML_CON_NORMAL;
				idpos=i+1;
				continue;
			}
			if (ae.pFrame->buf[i]=='\"') {
				MakeParseInfo(nLine,nUnit,i,HTML_STYLE_PHPSTRING);
				Context=HTML_CON_PHPSTRING;
				continue;
			}
			if (ae.pFrame->buf[i]=='/' && ae.pFrame->buf[i+1]=='*') {
				MakeParseInfo(nLine,nUnit,i,HTML_STYLE_PHPCOMMENT);
				i++;
				Context=HTML_CON_PHPBLOCKCOM;
				continue;
			}
			if ((ae.pFrame->buf[i]=='/' && ae.pFrame->buf[i+1]=='/') || (ae.pFrame->buf[i]=='#')) {
				MakeParseInfo(nLine,nUnit,i,HTML_STYLE_PHPCOMMENT);
				Context=HTML_CON_PHPLINECOM;
				goto EndParse;
			}
			if (strchr(GetInfo(1),ae.pFrame->buf[i]) || i==e-1) {
				if (i==e-1 && !strchr(GetInfo(1),ae.pFrame->buf[i])) {
					idend=i;
				} else {
					idend=i-1;
				}
				if (idend-idpos+1 >= 1) {
					if (IsPhpKeyword(ae,idpos,idend)) {
						MakeParseInfo(nLine,nUnit,idpos,HTML_STYLE_PHPKEY);
						MakeParseInfo(nLine,nUnit,idend+1,HTML_STYLE_PHP);
					} else if (IsPhpFunction(ae,idpos,idend)) {
						MakeParseInfo(nLine,nUnit,idpos,HTML_STYLE_PHPFUNCTION);
						MakeParseInfo(nLine,nUnit,idend+1,HTML_STYLE_PHP);
					}
				}

				idpos=i+1;
			}
			continue;
		case HTML_CON_PHPSTRING:
			if (ae.pFrame->buf[i]=='?' && ae.pFrame->buf[i+1]=='>') {
				i++;
				MakeParseInfo(nLine,nUnit,i+1,HTML_STYLE_NORMAL);
				Context=HTML_CON_NORMAL;
				idpos=i+1;
				continue;
			}
			if (ae.pFrame->buf[i]=='\"') {
				MakeParseInfo(nLine,nUnit,i+1,HTML_STYLE_PHP);
				Context=HTML_CON_PHP;
				idpos=i+1;
			}
			continue;
		case HTML_CON_PHPBLOCKCOM:
			if (ae.pFrame->buf[i]=='*' && ae.pFrame->buf[i+1]=='/') {
				i++;
				MakeParseInfo(nLine,nUnit,i+1,HTML_STYLE_PHP);
				Context=HTML_CON_PHP;
				idpos=i+1;
			}
			continue;
		}
	}

EndParse:
	pInfo[nLine].Context=Context;
}

BOOL CParseHtml::IsTag(CApiEditText &ae,int s, int e)
{
	static TCHAR *tag="a abbr above acronym address applet array "
		"area b base basefont bdo bgsound big blink blockquote "
		"body box br button caption center cite code col "
		"colgroup comment dd del dfn dir div dl dt em embed "
		"fieldset fig font form frame frameset h1 h2 h3 h4 "
		"h5 h6 head hr html i iframe ilayer img input ins "
		"isindex kbd label layer legend li link listring map "
		"marquee menu meta multicol nextid nobr noframes nolayer "
		"note noscript object ol option optgroup p param pre "
		"q quote range root s samp script select small sound "
		"spacer span sqrt strike strong style sub sup table "
		"tbody td text textarea tfoot th thead title tr tt "
		"u ul var wbr xmp ";

	return IsStringExist(tag,ae.pFrame->buf+s,e-s+1,FALSE);
}

BOOL CParseHtml::IsAttr(CApiEditText &ae,int s, int e)
{
	static TCHAR *attr="abbr accetp-charset accept accesskey action "
		"align alink alt archive axis background behavior below "
		"bgcolor border cellpadding cellspacing char charoff "
		"charset checked cite class classid clear code codebase "
		"codetype color cols colspan compact content coords data "
		"datetime dir enctype face for frame frameborder "
		"framespacing headers height hidden href hreflang hspace "
		"http-equiv id ismap label lang language link loop "
		"longdesc mailto marginheight marginwidth maxlength media "
		"method multiple name nohref noresize noshade object "
		"onblur onchange onfocus onkeydown onkeypress onkeyup "
		"onload onreset onselect onsubmit onunload onclick "
		"ondblclick onmousemdown onmousemove onmouseout onmouseover "
		"onmouseup profile prompt readonly rel rev rows rowspan "
		"rules scheme scope scrolling selected shape size span "
		"src standby start style summary tabindex target text "
		"tile topmargin type url usemap valign value valuetype "
		"version vlink vspace width ";

	return IsStringExist(attr,ae.pFrame->buf+s,e-s+1,FALSE);
}

BOOL CParseHtml::IsPhpKeyword(CApiEditText &ae,int s, int e)
{
	static TCHAR *pkey="array bool break case char class "
		"continue default do double else elseif endfor endif "
		"endswitch endwhile extends false float for foreach "
		"function global if include int integer long mixed "
		"new object old_function php real require return static "
		"string switch true var void while ";

	return IsStringExist(pkey,ae.pFrame->buf+s,e-s+1,TRUE);
}

BOOL CParseHtml::IsPhpFunction(CApiEditText &ae,int s, int e)
{
	static TCHAR *pfun="abs acos addcslashes addslashes "
		"apache_lookup_uri apache_note asin atan atan2 base_convert "
		"bin2hex bindex ceil chdir checkdate chop chr "
		"chunk_split closedir convert_cyr_string cos count_chars "
		"crc32 crypt date decbin dechex decoct deg2rad dir "
		"echo exp explode floor flush get_html_translation_table "
		"get_meta_tags getallheaders getcwd getdate getrandmax "
		"gettimeofday gmdate gmmktime gmstrftime hebrev hebrevc "
		"hexdec htmlentities htmlspecialchars implode join lcg "
		"levenshtein localtime log log10 ltrim max md5 "
		"metaphone microtime min mktime ml2br mt_getrandmax "
		"mt_rand mt_srand mysql_affected_rows mysql_change_user "
		"mysql_close mysql_connect mysql_create_db mysql_data_seek "
		"mysql_db_name mysql_db_query mysql_drop_db mysql_errno "
		"mysql_error mysql_fetch_array mysql_fetch_field "
		"mysql_fetch_lengths mysql_fetch_object mysql_fetch_row "
		"mysql_field_flags mysql_field_len mysql_field_name "
		"mysql_field_seek mysql_field_table mysql_free_result "
		"mysql_insert_id mysql_list_dbs mysql_list_fields "
		"mysql_list_tables mysql_num_fields mysql_num_rows "
		"mysql_pconnect mysql_query mysql_result mysql_select_db "
		"mysql_tablename mysql_type number_fromat octdec opendir "
		"ord parse_str pi pow print printf "
		"quoted_printable_decode quotemeta rad2deg rand rawurldecode "
		"rawurlencode readdir rewinddir round rtrim setlocale "
		"similar_text sin soundex sprintf sqry srand sscanf "
		"str_pad str_repeat str_replace strcasecmp strchr strcmp "
		"strcspn strftime strip_tags stripcslashes stripslatshes "
		"stristr strlen strnatcasecmp strnatcmp strncmp strpos "
		"strrchr strrev strrpos strspn strstr strtolower "
		"strtotime strtoupper strtr strtrok substr substr_count "
		"substr_replace tan time trim ucfirst ucwords virtual "
		"wordwrap ";

	return IsStringExist(pfun,ae.pFrame->buf+s,e-s+1,TRUE);
}

BOOL CParseHtml::IsAspKeyword(CApiEditText &ae,int s, int e)
{
	static TCHAR *akey="and call case delete dim dowhile else "
		"elseif end false for foreach from function if in "
		"insert int into isdate isnull isnumeric isstring key "
		"left len like loop mid next or right select set "
		"split sub then to true update values wend where "
		"while with ";

	return IsStringExist(akey,ae.pFrame->buf+s,e-s+1,TRUE);
}

enum {
	SQL_CON_NORMAL,
	SQL_CON_BLOCKCOMMENT,
	SQL_CON_LINECOMMENT,
	SQL_CON_STRING
};

enum {
	SQL_STYLE_NORMAL,
	SQL_STYLE_COMMENT,
	SQL_STYLE_STRING,
	SQL_STYLE_NUMBER,
	SQL_STYLE_KEYWORD,
	SQL_STYLE_TYPE
};

CParseSql::CParseSql()
{
	lstrcpy(arStyle[0].name,"보통");
	arStyle[0].fore=(DWORD)-1;
	lstrcpy(arStyle[1].name,"주석");
	arStyle[1].fore=RGB(0,0,0);
	arStyle[1].back=RGB(192,192,192);
	lstrcpy(arStyle[2].name,"문자열");
	arStyle[2].fore=RGB(0,128,0);
	lstrcpy(arStyle[3].name,"숫자");
	arStyle[3].fore=RGB(255,0,0);
	lstrcpy(arStyle[4].name,"키워드");
	arStyle[4].fore=RGB(0,0,255);
	lstrcpy(arStyle[5].name,"타입");
	arStyle[5].fore=RGB(0,255,0);
}

TCHAR *CParseSql::GetInfo(int iIndex)
{
	switch (iIndex) {
	case 0:
		return (TCHAR *)3;
	case 1:
		return TEXT(" \t\r\n\"\'.,<>:;/()[]{}~!@#$%^&*-+?$");
	case 2:
		return TEXT("--");
	case 3:
		return TEXT("/*");
	case 4:
		return TEXT("*/");
	case 5:
		return TEXT("");
	case 6:
		return TEXT("");
	}
	return 0;
}

void CParseSql::ParseLine(CApiEditText &ae,int nLine,Ae_PrintInfo *pi)
{
	DWORD Context;
	int s,e,i;
	int nUnit=0;
	int idpos,idend;
	tagLine *tLine;

	if (pi) {
		tLine=pi->pLine;
	} else {
		tLine=ae.pLine;
	}

	if (pInfo[nLine].pUnit[0].pos != -1)
		return;

	if (nLine == 0) {
		Context=SQL_CON_NORMAL;
	} else {
		Context=pInfo[nLine-1].Context;
	}

	s=tLine[nLine].Start;
	e=tLine[nLine].End;
	idpos=s;

	switch(Context) {
	case SQL_CON_NORMAL:
		MakeParseInfo(nLine,nUnit,s,SQL_STYLE_NORMAL);
		break;
	case SQL_CON_STRING:
		MakeParseInfo(nLine,nUnit,s,SQL_STYLE_STRING);
		break;
	case SQL_CON_BLOCKCOMMENT:
		MakeParseInfo(nLine,nUnit,s,SQL_STYLE_COMMENT);
		break;
	case SQL_CON_LINECOMMENT:
		if (tLine[nLine].nLine != 0) {
			MakeParseInfo(nLine,nUnit,s,SQL_STYLE_COMMENT);
			goto EndParse;
		} else {
			MakeParseInfo(nLine,nUnit,s,SQL_STYLE_NORMAL);
			Context=SQL_CON_NORMAL;
			break;
		}
	}

	for (i=s;i<e;i++) {
		switch(Context) {
		case SQL_CON_NORMAL:
			if (ae.pFrame->buf[i]=='/' && ae.pFrame->buf[i+1]=='*') {
				MakeParseInfo(nLine,nUnit,i,SQL_STYLE_COMMENT);
				i++;
				Context=SQL_CON_BLOCKCOMMENT;
				continue;
			}
			if (ae.pFrame->buf[i]=='-' && ae.pFrame->buf[i+1]=='-') {
				MakeParseInfo(nLine,nUnit,i,SQL_STYLE_COMMENT);
				Context=SQL_CON_LINECOMMENT;
				goto EndParse;
			}
			if (ae.pFrame->buf[i]=='\'') {
				MakeParseInfo(nLine,nUnit,i,SQL_STYLE_STRING);
				Context=SQL_CON_STRING;
				continue;
			}

			if (strchr(GetInfo(1),ae.pFrame->buf[i]) || i==e-1) {
				if (i==e-1 && !strchr(GetInfo(1),ae.pFrame->buf[i])) {
					idend=i;
				} else {
					idend=i-1;
				}

				if (idend-idpos+1 >= 1) {
					if (IsKeyword(ae,idpos,idend)) {
						MakeParseInfo(nLine,nUnit,idpos,SQL_STYLE_KEYWORD);
						MakeParseInfo(nLine,nUnit,idend+1,SQL_STYLE_NORMAL);
					} else
					if (IsNumber(ae,idpos,idend)) {
						MakeParseInfo(nLine,nUnit,idpos,SQL_STYLE_NUMBER);
						MakeParseInfo(nLine,nUnit,idend+1,SQL_STYLE_NORMAL);
					} else
					if (IsType(ae,idpos,idend)) {
						MakeParseInfo(nLine,nUnit,idpos,SQL_STYLE_TYPE);
						MakeParseInfo(nLine,nUnit,idend+1,SQL_STYLE_NORMAL);
					}
				}

				idpos=i+1;
			}
			continue;
		case SQL_CON_BLOCKCOMMENT:
			if (ae.pFrame->buf[i]=='*' && ae.pFrame->buf[i+1]=='/') {
				i++;
				MakeParseInfo(nLine,nUnit,i+1,SQL_STYLE_NORMAL);
				Context=SQL_CON_NORMAL;
				idpos=i+1;
			}
			continue;
		case SQL_CON_STRING:
			if (ae.pFrame->buf[i]=='\'') {
				if (ae.pFrame->buf[i+1]=='\'') {
					i++;
					continue;
				} else {
					MakeParseInfo(nLine,nUnit,i+1,SQL_STYLE_NORMAL);
					Context=SQL_CON_NORMAL;
					idpos=i+1;
				}
			}
			continue;
		}
	}

EndParse:
	pInfo[nLine].Context=Context;
}

BOOL CParseSql::IsKeyword(CApiEditText &ae,int s, int e)
{
	static TCHAR *keyword="abs acos all alter and any ascii "
		"asin atan atn2 avg backup begin between break case "
		"cast ceiling charindex checkpoint checksum checksum_agg "
		"close coalesce collate collationproterty col_length "
		"col_name columnproperty commit constraint convert contains containstable "
		"continue cos cot count count_big create current_timestamp "
		"current_user cursor cursor_status database databaseproperty "
		"databasepropertyex datalength dateadd datediff datename "
		"datepart day db_id db_name dbcc deallocate declare "
		"default degrees delete deny difference drop dump else "
		"end execute exists exp fetch file_id file_name "
		"filegroup_id filegroup_name filegroupproperty fileproperty "
		"floor formatmessage freetext freetexttable from function "
		"getansinull getdate getutcdate go goto grant group "
		"grouping having if in index insert isdate isnull "
		"isnumeric key kill left len like load log log10 lower "
		"ltrim max min month nchar newid not null nullif numeric "
		"open or order parsename patindex permissions pi power primary "
		"print procedure quotename radians raiserror rand readtext "
		"reconfigure replace replicate restore return reverse revoke "
		"right rollback round rtrim rule save schema select set "
		"setuser shutdown sign sin some soundex space square sqrt "
		"statistics stdev stdevp str stuff substring sum table "
		"tan textptr textvalid transaction trigger trigger_nestlevel "
		"truncate unicode union update updatetext upper use user "
		"var varp view waitfor where while work writetext year "	;

	return IsStringExist(keyword,ae.pFrame->buf+s,e-s+1,FALSE);
}

BOOL CParseSql::IsType(CApiEditText &ae,int s, int e)
{
	static TCHAR *type="bigint binary bit char datetime "
		"decimal float image int nchar ntext numeric "
		"nvarchar money real smalldatetime smallint smallmoney "
		"sql_variant text timestamp tinyint uniqueidentifier "
		"varbinary varchar ";

	return IsStringExist(type,ae.pFrame->buf+s,e-s+1,FALSE);
}
