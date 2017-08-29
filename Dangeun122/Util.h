#ifndef __UTIL_H
#define __UTIL_H

#define STARTQ TCHAR qMes[256];\
	static qcount=0;\
	LARGE_INTEGER qs,s1,s2,s3,qe,qfreq;\
	double g1,g2,g3,ge;\
	QueryPerformanceFrequency(&qfreq);\
	QueryPerformanceCounter(&qs);\
	s1=s2=s3=qs;
#define SPOT1 QueryPerformanceCounter(&s1);
#define SPOT2 QueryPerformanceCounter(&s2);
#define SPOT3 QueryPerformanceCounter(&s3);
#define ENDQ QueryPerformanceCounter(&qe);\
	g1=((double)(s1.QuadPart-qs.QuadPart))/((double)qfreq.QuadPart);\
	g2=((double)(s2.QuadPart-qs.QuadPart))/((double)qfreq.QuadPart);\
	g3=((double)(s3.QuadPart-qs.QuadPart))/((double)qfreq.QuadPart);\
	ge=((double)(qe.QuadPart-qs.QuadPart))/((double)qfreq.QuadPart);\
	sprintf(qMes, "%04d : S1=%.6f,S2=%.6f,S3=%.6f,QE=%.6f\r\n",++qcount,g1,g2,g3,ge);\
	OutputDebugString(qMes);

#define SHLM HKEY_LOCAL_MACHINE
#define SHCU HKEY_CURRENT_USER
#define SHCR HKEY_CLASSES_ROOT

void WriteLogFile(TCHAR *strLog,...);
void CustomTrace(char *format, ...);
void MySetImeMode(HWND hEdit, BOOL bHan);
UINT SHRegReadInt(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, INT nDefault);
BOOL SHRegReadString(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, LPCTSTR lpDefault, 
   LPTSTR lpRet, DWORD nSize);
BOOL SHRegWriteInt(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, UINT nData);
BOOL SHRegWriteString(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, LPCTSTR lpData);
void SavePosition(HWND hWnd, TCHAR *Key);
void LoadPosition(HWND hWnd, TCHAR *Key, RECT *Def=NULL);
HRESULT MyCreateShortCut(LPCSTR szSrcFile, LPSTR szLnkFile, LPSTR szArgument, LPSTR szDesc);
void MyGetSpecialFolderPath(HWND hWnd, int nFolder, TCHAR *szPath);
void MoveToParentCenter(HWND hWnd);
BOOL BrowseFolder(HWND hParent, LPCTSTR szTitle, LPCTSTR StartPath, TCHAR *szFolder);
void ParseFtpInfo(TCHAR *Path, TCHAR *Server, TCHAR *User, TCHAR *Pass, TCHAR *& File, int &Port);
void EncodePath(TCHAR *Path, TCHAR *EncPath);
void EncodeBinary(PBYTE buf, int Length, BOOL bEncode);
int BinaryToTextEnccode(PBYTE Src, int length, PBYTE Dest, BOOL bEncode);
int DecodeText(TCHAR *EncText, TCHAR *Text);
void EncodeText(TCHAR *Text, TCHAR *EncText);

#define FIF_DEEP 1
#define FIF_DIRFILTER 2
#define FIF_INCHID 4
typedef int (*FIFCALLBACK)(TCHAR *, DWORD, LPVOID);
void FindInFiles(TCHAR *Path, TCHAR *Pattern, DWORD Flags, FIFCALLBACK pCallBack, LPVOID pCustom);
BOOL IsMatch(TCHAR *Path, TCHAR *Pattern);

void DrawBitmapById(HDC hdc,int x,int y,UINT bitid);
void ReplaceCtrlString(TCHAR *src, int slen, TCHAR *dest, int dlen);
BOOL IsStrEmpty(TCHAR *str);
void StringNormalize(TCHAR *src, TCHAR *dest);
HBITMAP MakeDDBFromDIB(TCHAR *Path);
void MakeAbsolutePath(TCHAR *Path);
BOOL MakeRelativePath(TCHAR *Path);
BOOL RegisterSvr(TCHAR *DllName, BOOL bReg);
BOOL MyGetImeMode(HWND hEdit);
DWORD MyGetAbsDay(SYSTEMTIME st);
void MyAbsToSystem(DWORD Abs, SYSTEMTIME &st);

#endif // __UTIL_H



