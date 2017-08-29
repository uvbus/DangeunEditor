#ifndef __PARSE_H
#define __PARSE_H

#define PARSENUM 6

struct ParseUnit
{
	int pos;
	int style;
};

struct ParseInfo
{
	ParseUnit *pUnit;
	int UnitSize;
	DWORD Context;
};

struct SParseStyle
{
	SParseStyle();
	TCHAR name[32];
	COLORREF fore;
	COLORREF back;
};

class CApiEditText;
struct Ae_PrintInfo;
class CParse
{
protected:
	int ParseSize;
	SParseStyle arStyle[32];

public:
	ParseInfo *pInfo;
	CParse();
	~CParse();
	void InitInfo(BOOL bAlloc);
	void DeleteParseInfo(int nLine);
	void MakeParseInfo(int nLine, int &nUnit, int pos, int style);
	BOOL IsStringExist(TCHAR *list,TCHAR *str,int len,BOOL bCase);
	BOOL IsNumber(CApiEditText &ae,int s, int e);
	void ParseLines(CApiEditText &ae,int nLine,Ae_PrintInfo *pi=NULL);
	void GetStyleColor(int style, COLORREF &fore, COLORREF &back);
	void SetStyleColor(int style, COLORREF fore, COLORREF back);
	void GetStyleName(int style, TCHAR *szName);

	virtual void ParseLine(CApiEditText &ae,int nLine,Ae_PrintInfo *pi)=0;
	virtual TCHAR *GetInfo(int iIndex)=0;
};

class CParseNull : public CParse
{
public:
	CParseNull();
	~CParseNull() {};
	TCHAR *GetInfo(int iIndex);
private:
	void ParseLine(CApiEditText &ae,int nLine,Ae_PrintInfo *pi);
};

class CParseCpp : public CParse
{
public:
	CParseCpp();
	~CParseCpp() {};
	virtual TCHAR *GetInfo(int iIndex);

protected:
	virtual void ParseLine(CApiEditText &ae,int nLine,Ae_PrintInfo *pi);
	virtual BOOL IsKeyword(CApiEditText &ae,int s, int e);
	virtual BOOL IsPreProcessor(CApiEditText &ae,int s, int e);
};

class CParseJava : public CParseCpp
{
public:
	CParseJava();
	~CParseJava() {};
	virtual TCHAR *GetInfo(int iIndex);

protected:
	virtual BOOL IsKeyword(CApiEditText &ae,int s, int e);
	virtual BOOL IsPreProcessor(CApiEditText &ae,int s, int e);
};

class CParseCS : public CParseCpp
{
public:
	CParseCS();
	~CParseCS() {};
	virtual TCHAR *GetInfo(int iIndex);

protected:
	virtual BOOL IsKeyword(CApiEditText &ae,int s, int e);
	virtual BOOL IsPreProcessor(CApiEditText &ae,int s, int e);
};

class CParseHtml : public CParse
{
public:
	CParseHtml();
	~CParseHtml() {};
	TCHAR *GetInfo(int iIndex);

private:
	void ParseLine(CApiEditText &ae,int nLine,Ae_PrintInfo *pi);
	BOOL IsTag(CApiEditText &ae,int s, int e);
	BOOL IsAttr(CApiEditText &ae,int s, int e);
	BOOL IsPhpKeyword(CApiEditText &ae,int s, int e);
	BOOL IsPhpFunction(CApiEditText &ae,int s, int e);
	BOOL IsAspKeyword(CApiEditText &ae,int s, int e);
};

class CParseSql : public CParse
{
public:
	CParseSql();
	~CParseSql() {};
	TCHAR *GetInfo(int iIndex);

private:
	void ParseLine(CApiEditText &ae,int nLine,Ae_PrintInfo *pi);
	BOOL IsKeyword(CApiEditText &ae,int s, int e);
	BOOL IsType(CApiEditText &ae,int s, int e);
};

#endif // __PARSE_H