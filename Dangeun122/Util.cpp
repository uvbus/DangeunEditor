#include "stdafx.h"

#ifdef _DEBUG
#define DEBUGLOGFILE "c:\\DgDebug.txt"
void WriteLogFile(TCHAR *strLog,...)
{
	HANDLE hLog;
	static int count=0;
	DWORD dwWritten;
	TCHAR szLog[1024];
	TCHAR strLog2[1024];
	va_list marker;
	SYSTEMTIME st;

	va_start( marker, strLog );
	vsprintf(szLog,strLog,marker);

	if (count == 0) {
		hLog=CreateFile(DEBUGLOGFILE,GENERIC_WRITE,0,NULL,
		CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	} else {
		hLog=CreateFile(DEBUGLOGFILE,GENERIC_WRITE,0,NULL,
		OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	}

	GetLocalTime(&st);
	wsprintf(strLog2,"카운터=%06d(%d:%d:%d:%d) %s\r\n",count++, 
		st.wHour,st.wMinute,st.wSecond,st.wMilliseconds,szLog);
	SetFilePointer(hLog,0,NULL,FILE_END);
	WriteFile(hLog,strLog2,strlen(strLog2),&dwWritten,NULL);
	CloseHandle(hLog);
}

void CustomTrace(char *format, ...)
{
	TCHAR buf[1024];
	va_list marker;

	va_start(marker, format);
	vsprintf(buf,format,marker);
	OutputDebugString(buf);
}
#endif

void MySetImeMode(HWND hEdit, BOOL bHan)
{
	HIMC hImc;
	hImc=ImmGetContext(hEdit);

	if (bHan == TRUE) {
		ImmSetConversionStatus(hImc,IME_CMODE_NATIVE,IME_SMODE_NONE);
	} else {
		ImmSetConversionStatus(hImc,0,IME_SMODE_NONE);
	}

	ImmReleaseContext(hEdit,hImc );
}

UINT SHRegReadInt(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, INT nDefault)
{
	HKEY key;
	DWORD dwDisp;
	UINT Result;
	DWORD Size;
	if (RegCreateKeyEx(hKey, lpKey,0,NULL,
		REG_OPTION_NON_VOLATILE, KEY_READ,NULL,&key,&dwDisp)
		!=ERROR_SUCCESS) 
		return 0;
	Size=sizeof(LONG);
	if (RegQueryValueEx(key, lpValue, 0, NULL,(LPBYTE)&Result, &Size)
		!=ERROR_SUCCESS)
		Result=nDefault;
	RegCloseKey(key);
	return Result;
}

BOOL SHRegReadString(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, LPCTSTR lpDefault, 
   LPTSTR lpRet, DWORD nSize) 
{
	HKEY key;
	DWORD dwDisp;
	DWORD Size;
	if (RegCreateKeyEx(hKey, lpKey,0,NULL,
		REG_OPTION_NON_VOLATILE, KEY_READ,NULL,&key,&dwDisp)
		!=ERROR_SUCCESS) 
		return FALSE;
	Size=nSize;
	if (RegQueryValueEx(key, lpValue, 0, NULL,(LPBYTE)lpRet, &Size)
		!=ERROR_SUCCESS) {
		lstrcpy(lpRet, lpDefault);
		return FALSE;
	}
	RegCloseKey(key);
	return TRUE;
}

BOOL SHRegWriteInt(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, UINT nData)
{
	HKEY key;
	DWORD dwDisp;
	if (RegCreateKeyEx(hKey, lpKey,0,NULL,
		REG_OPTION_NON_VOLATILE, KEY_WRITE,NULL,&key,&dwDisp)
		!=ERROR_SUCCESS) {
		return FALSE;
	}
	if (RegSetValueEx(key, lpValue,0,REG_DWORD,(LPBYTE)&nData,sizeof(UINT))
		!=ERROR_SUCCESS) 
		return FALSE;
	RegCloseKey(key);
	return TRUE;
}

BOOL SHRegWriteString(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, LPCTSTR lpData)
{
	HKEY key;
	DWORD dwDisp;
	if (RegCreateKeyEx(hKey, lpKey,0,NULL,
		REG_OPTION_NON_VOLATILE, KEY_WRITE,NULL,&key,&dwDisp)
		!=ERROR_SUCCESS) 
		return FALSE;
	if (RegSetValueEx(key, lpValue,0,REG_SZ,(LPBYTE)lpData,lstrlen(lpData)+1)
		!=ERROR_SUCCESS) 
		return FALSE;
	RegCloseKey(key);
	return TRUE;
}

void SavePosition(HWND hWnd, TCHAR *Key)
{
	WINDOWPLACEMENT wndpl;

	wndpl.length=sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(hWnd,&wndpl);

	SHRegWriteInt(SHCU,Key,"showCmd", wndpl.showCmd);
	SHRegWriteInt(SHCU,Key,"left", wndpl.rcNormalPosition.left);
	SHRegWriteInt(SHCU,Key,"top", wndpl.rcNormalPosition.top);
	SHRegWriteInt(SHCU,Key,"right", wndpl.rcNormalPosition.right);
	SHRegWriteInt(SHCU,Key,"bottom",wndpl.rcNormalPosition.bottom);
}

void LoadPosition(HWND hWnd, TCHAR *Key, RECT *Def/*=NULL*/)
{
	WINDOWPLACEMENT wndpl;
	RECT drt;

	if (Def==NULL) {
		SetRect(&drt,10,10,600,400);
	} else {
		CopyRect(&drt,Def);
	}
	wndpl.length=sizeof(WINDOWPLACEMENT);
	wndpl.flags=0;
	wndpl.rcNormalPosition.left=SHRegReadInt(SHCU,Key,"left",drt.left);
	wndpl.rcNormalPosition.top=SHRegReadInt(SHCU,Key,"top",drt.top);
	wndpl.rcNormalPosition.right=SHRegReadInt(SHCU,Key,"right",drt.right);
	wndpl.rcNormalPosition.bottom=SHRegReadInt(SHCU,Key,"bottom",drt.bottom);
	wndpl.showCmd=SHRegReadInt(SHCU,Key,"showCmd",drt.bottom);

	if (wndpl.showCmd == SW_SHOWMINIMIZED) {
		wndpl.showCmd=SW_RESTORE;
	}

	wndpl.ptMinPosition.x=wndpl.ptMinPosition.y=0;
	wndpl.ptMaxPosition.x=wndpl.ptMaxPosition.y=0;
	SetWindowPlacement(hWnd,&wndpl);
}

HRESULT MyCreateShortCut(LPCSTR szSrcFile, LPSTR szLnkFile, LPSTR szArgument, LPSTR szDesc)
{
	HRESULT hr;
	IShellLink *pSl;
	IPersistFile *pPf;
	WCHAR wszLnkFile[MAX_PATH]={0,};

	hr=CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
		IID_IShellLink, (void **)&pSl);
	if (FAILED(hr))
		return E_FAIL;

	pSl->SetPath(szSrcFile);
	pSl->SetDescription(szDesc);
	pSl->SetArguments(szArgument);

	hr=pSl->QueryInterface(IID_IPersistFile, (void **)&pPf);
	if (FAILED(hr)) {
		pSl->Release();
		return E_FAIL;
	}

	MultiByteToWideChar(CP_ACP, 0, szLnkFile, -1, wszLnkFile, MAX_PATH);
	hr=pPf->Save(wszLnkFile, TRUE);

	pPf->Release();
	pSl->Release();
	return hr;
}

void MyGetSpecialFolderPath(HWND hWnd, int nFolder, TCHAR *szPath)
{
	LPITEMIDLIST pidl;
	LPMALLOC pMalloc;

	SHGetSpecialFolderLocation(hWnd,nFolder,&pidl);
	SHGetPathFromIDList(pidl, szPath);
	SHGetMalloc(&pMalloc);
	pMalloc->Free(pidl);
	pMalloc->Release();
}

void MoveToParentCenter(HWND hWnd)
{
	RECT wrt,crt;
	HWND hParent;
	RECT rt,irt;
	HMONITOR hMon;
	MONITORINFOEX mi;

	hParent=GetParent(hWnd);
	if (hParent == NULL) {
		return;
	}
	if (IsIconic(hParent)) {
		ShowWindow(hParent,SW_RESTORE);
	}

	GetWindowRect(hParent,&wrt);
	GetWindowRect(hWnd,&crt);
	rt.left=wrt.left+(wrt.right-wrt.left)/2-(crt.right-crt.left)/2;
	rt.top=wrt.top+(wrt.bottom-wrt.top)/2-(crt.bottom-crt.top)/2;
	rt.right=rt.left+(crt.right-crt.left);
	rt.bottom=rt.top+(crt.bottom-crt.top);

	// 많이 걸쳐진 모니터의 워크 에리어에 완전히 포함되어 있지 않으면 좌표를 조정한다.
	hMon=MonitorFromWindow(hWnd,MONITOR_DEFAULTTOPRIMARY);
	mi.cbSize=sizeof(MONITORINFOEX);
	GetMonitorInfo(hMon,&mi);
	IntersectRect(&irt,&rt,&mi.rcWork);
	if (EqualRect(&irt,&rt)==FALSE) {
		// 모니터 안으로 수직 이동
		if (rt.top < mi.rcWork.top) {
			rt.top=mi.rcWork.top;
		} else if (rt.bottom > mi.rcWork.bottom) {
			rt.top=mi.rcWork.bottom-(rt.bottom-rt.top);
		}
		// 모니터 안으로 수평 이동
		if (rt.left < mi.rcWork.left) {
			rt.left=mi.rcWork.left;
		} else if (rt.right > mi.rcWork.right) {
			rt.left=mi.rcWork.right-(rt.right-rt.left);
		}
	}

	SetWindowPos(hWnd,HWND_NOTOPMOST,rt.left,rt.top,0,0,SWP_NOSIZE);
}

int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData)
{
	switch (uMsg) {
	case BFFM_INITIALIZED:
		if (lpData != NULL) {
			SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)lpData);
		}
		break;
	}
	return 0;
}

BOOL BrowseFolder(HWND hParent, LPCTSTR szTitle, LPCTSTR StartPath, TCHAR *szFolder)
{
	LPMALLOC pMalloc;
	LPITEMIDLIST pidl;
	BROWSEINFO bi;

	bi.hwndOwner = hParent;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = NULL;
	bi.lpszTitle = szTitle ;
	bi.ulFlags = 0;
	bi.lpfn = BrowseCallbackProc;;
	bi.lParam = (LPARAM)StartPath;
						
	pidl = SHBrowseForFolder(&bi);

	if (pidl == NULL) {
		return FALSE;
	}
	SHGetPathFromIDList(pidl, szFolder);

	if (SHGetMalloc(&pMalloc) != NOERROR) {
		return FALSE;
	}
	pMalloc->Free(pidl);
	pMalloc->Release();
	return TRUE;
}

BOOL IsMatch(TCHAR *Path, TCHAR *Pattern)
{
	TCHAR Pat[MAX_PATH];
	TCHAR *t,*p;
	TCHAR Ext[_MAX_EXT];
	TCHAR Name[MAX_PATH];
	TCHAR *p1,*p2;
	BOOL bOther;

	_splitpath(Path,NULL,NULL,Name,Ext);
	lstrcat(Name,Ext);
	CharUpper(Name);

	t=Pattern;
	for (;;) {
		p=Pat;
		while (*t!=';' && *t!=0) {
			*p=*t;
			p++;
			t++;
		}
		*p=0;

		CharUpper(Pat);
		if (Pat[0]=='*' && Pat[1]=='.') {
			if (lstrcmpi(Pat+1,Ext)==0) {
				return TRUE;
			}
		}
		p1=Name;
		p2=Pat;
		for (;;) {
			if (*p2=='?') {
				p1++;
				p2++;
			} else if (*p2=='*') {
				p2++;
				while (*p1!=*p2 && *p1!=0) 
					p1++;
			} else {
				if (*p1!=*p2) {
					break;
				}
				p1++;
				p2++;
			}

			if (*p1==0 && *p2==0) {
				return TRUE;
			}
			if (*p1!=0 && *p2==0) {
				break;
			}
			if (*p1==0 && *p2!=0) {
				bOther=FALSE;
				while (*p2) {
					if (*p2!='.' && *p2!='*') {
						bOther=TRUE;
					}
					p2++;
				}
				if (bOther==FALSE) {
					return TRUE;
				}
				break;
			}
		}

		if (*t==0) {
			break;
		}
		t++;
	}
	return FALSE;
}

BOOL bContFIF=TRUE;
void FindInFiles(TCHAR *Path, TCHAR *Pattern, DWORD Flags, FIFCALLBACK pCallBack, LPVOID pCustom)
{
	TCHAR SrchPath[MAX_PATH];
	TCHAR szFinded[MAX_PATH];
	WIN32_FIND_DATA wfd;
	HANDLE hSrch;
	BOOL nResult=TRUE;

	lstrcpy(SrchPath, Path);
	if (SrchPath[lstrlen(SrchPath)-1] == '\\') {
		SrchPath[lstrlen(SrchPath)-1]=0;
	}
	lstrcat(SrchPath, "\\*.*");
	hSrch=FindFirstFile(SrchPath,&wfd);
	if (hSrch == INVALID_HANDLE_VALUE) {
		return;
	}

	while (nResult) {
		wsprintf(szFinded,"%s\\%s",Path,wfd.cFileName);
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (lstrcmp(wfd.cFileName,".") && lstrcmp(wfd.cFileName,"..")) {
				if (
					((wfd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)==0 || (Flags & FIF_INCHID))
					&& ((Flags & FIF_DIRFILTER)==0 || IsMatch(szFinded, Pattern))
					) {
					pCallBack(szFinded,wfd.dwFileAttributes,pCustom);
				}
				if (Flags & FIF_DEEP) {
					FindInFiles(szFinded,Pattern,Flags,pCallBack,pCustom);
				}
			}
		} else {
			if (((wfd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)==0 || (Flags & FIF_INCHID)) 
				&& IsMatch(szFinded, Pattern)) {
				pCallBack(szFinded,wfd.dwFileAttributes,pCustom);
			}
		}
		if (bContFIF==FALSE)
			break;
		nResult=FindNextFile(hSrch,&wfd);
	}
	FindClose(hSrch);
}

void ParseFtpInfo(TCHAR *Path, TCHAR *Server, TCHAR *User, TCHAR *Pass, TCHAR *& File, int &Port)
{
	TCHAR *p,*p2;

	p=Path+6;
	p2=strchr(p,':');
	lstrcpyn(User,p,p2-p+1);

	p=p2+1;
	p2=strchr(p,':');
	lstrcpyn(Pass,p,p2-p+1);

	p=p2+1;
	p2=strchr(p,':');
	lstrcpyn(Server,p,p2-p+1);

	Port=atoi(p2+1);

	File=strchr(p2+1,'/')+1;
}

void EncodePath(TCHAR *Path, TCHAR *EncPath)
{
	TCHAR Server[256];
	TCHAR User[32];
	TCHAR Pass[32];
	TCHAR *File;
	int Port;

	if (strnicmp(Path,"ftp",3) == 0) {
		ParseFtpInfo(Path,Server,User,Pass,File,Port);
		wsprintf(EncPath,"ftp://%s/%s",Server,File);
	} else {
		lstrcpy(EncPath,Path);
	}
}

void EncodeBinary(PBYTE buf, int Length, BOOL bEncode)
{
	int i;
	BYTE b;
	int Carry;

	for (i=0;i<Length;i++) {
		b=buf[i];
		if (bEncode == TRUE) {
			if (b >= 128) Carry = 1; else Carry = 0;
			b = b * 2 + Carry;
			b = b ^ 0xdb;
			if (b % 2 == 1) Carry = 128; else Carry = 0;
			b = b / 2 + Carry;
			b = b ^ 0xbd;
			buf[i] = b;
		} else {
			b = b ^ 0xbd;
			if (b >= 128) Carry = 1; else Carry = 0;
			b = b * 2 + Carry;
			b = b ^ 0xdb;
			if (b % 2 == 1) Carry = 128; else Carry = 0;
			b = b / 2 + Carry;
			buf[i] = b;
		}
	}
}

int BinaryToTextEnccode(PBYTE Src, int length, PBYTE Dest, BOOL bEncode)
{
	BYTE s1, s2, s3, s4, d1, d2, d3, d4;
	BYTE *buf;
	int si,di;
	int OriginalLen;

	if ((bEncode == FALSE) && (length < 12))
		return -3;

	if (bEncode) {
		buf=(PBYTE)malloc(length+8);
		memcpy(buf+6, Src, length);
		buf[0]=0x57;
		buf[1]=0xEC;
		buf[2]=0xFF;
		buf[3]=(length & 0xff0000) >> 16;
		buf[4]=(length & 0xff00) >> 8;
		buf[5]=(length & 0xff);
		buf[length+6]=0;	
		buf[length+7]=0;

		for (si=0,di=0;si<length+6;si+=3,di+=4) {
			s1=buf[si];
			s2=buf[si+1];
			s3=buf[si+2];
			d1=(s1 >> 2) + 48;
			d2=(((s1 & 0x3) << 4) | (s2 >> 4)) + 48;
			d3=(((s2 & 0xf) << 2) | (s3 >> 6)) + 48;
			d4=(s3 & 0x3f) + 48;
			Dest[di]=d1;
			Dest[di+1]=d2;
			Dest[di+2]=d3;
			Dest[di+3]=d4;
		}
		Dest[di]=0;
		free(buf);
		return 0;
	} else {
		if ((Src[0] != 'E') || (Src[1] != 'n') || (Src[2] != 'c') || (Src[3] != 'o')) {
			return -1;
		}
		for (si=4,di=-3;si<length;si+=4,di+=3) {
			s1=Src[si]-48;
			s2=Src[si+1]-48;
			s3=Src[si+2]-48;
			s4=Src[si+3]-48;
			d1=(s1 << 2) | ((s2 & 0x30) >> 4);
			d2=(s2 << 4) | ((s3 & 0x3c) >> 2);
			d3=(s3 << 6) | (s4 & 0x3f);

			if (si==4) {
				OriginalLen = (d1 << 16) + (d2 << 8) + d3;
				if ((OriginalLen+2)/3*4+8 != length) {
					return -2;
				}
			} else {
				Dest[di]=d1;
				Dest[di+1]=d2;
				Dest[di+2]=d3;
			}
		}
		return OriginalLen;
	}
}

void EncodeText(TCHAR *Text, TCHAR *EncText)
{
	int len;
	TCHAR *temp;

	len=lstrlen(Text)+1;
	temp=(TCHAR *)malloc(len);
	lstrcpy(temp,Text);
	EncodeBinary((PBYTE)temp,len,TRUE);
	BinaryToTextEnccode((PBYTE)temp,len,(PBYTE)EncText,TRUE);
	free(temp);
}

int DecodeText(TCHAR *EncText, TCHAR *Text)
{
	int len;
	len=BinaryToTextEnccode((PBYTE)EncText,lstrlen(EncText),(PBYTE)Text,FALSE);
	if (len < 0)
		return len;
	EncodeBinary((PBYTE)Text,len,FALSE);
	return 0;
}

void DrawBitmapById(HDC hdc,int x,int y,UINT bitid)
{
	HDC MemDC;
	HBITMAP hBit,OldBitmap;
	int bx,by;
	BITMAP bit;

	hBit=LoadBitmap(g_hInst,MAKEINTRESOURCE(bitid));
	MemDC=CreateCompatibleDC(hdc);
	OldBitmap=(HBITMAP)SelectObject(MemDC, hBit);

	GetObject(hBit,sizeof(BITMAP),&bit);
	bx=bit.bmWidth;
	by=bit.bmHeight;

	BitBlt(hdc,x,y,bx,by,MemDC,0,0,SRCCOPY);

	SelectObject(MemDC,OldBitmap);
	DeleteDC(MemDC);
	DeleteObject(hBit);
}

// 제어코드를 읽을 수 있는 문자로 변환한다.
void ReplaceCtrlString(TCHAR *src, int slen, TCHAR *dest, int dlen)
{
	TCHAR *s,*d;
	TCHAR *dest2;

	s=src;
	if (dest==NULL) {
		d=dest2=(TCHAR *)malloc(1024);
		dlen=1024;
	} else {
		d=dest;
	}

	memset(d,0,dlen);
	for (;*s;d++, s++) {
		if (s-src >= slen) {
			break;
		}
		if ((dest && d-dest >= dlen-1) || (dest==NULL && d-dest2 >= dlen-1)) {
			break;
		}

		if (*s == '\r' && s-src+1 < slen && s[1] == '\n') {
			lstrcat(d,"[개행]");
			d+=5;
			s++;
		} else if (*s == '\t') {
			lstrcat(d,"[탭]");
			d+=3;
		} else {
			if (IsDBCSLeadByte(*s)) {
				*d++=*s++;
				*d=*s;
			} else {
				if (*s >= 0x20 && *s <= 0x7e) {
					*d=*s;
				} else {
					*d='.';
				}
			}
		}
	}
	if (dest==NULL) {
		lstrcpy(src,dest2);
		free(dest2);
	}
}

BOOL IsStrEmpty(TCHAR *str)
{
	int len,i;
	BOOL bEmpty=TRUE;

	len=lstrlen(str);
	// 길이가 0이면 비어있다.
	if (len == 0) {
		return TRUE;
	}

	// 공백이나 탭 문자 외에 다른 문자가 있으면 비어있지 않다.
	for (i=0;i<len;i++) {
		if ((str[i]!=' ') && (str[i]!='\t')) {
			bEmpty=FALSE;
			break;
		}
	}
	return bEmpty;
}

void StringNormalize(TCHAR *src, TCHAR *dest)
{
	TCHAR *s,*d;
	TCHAR *dest2;

	s=src;
	if (dest==NULL) {
		d=dest2=(TCHAR *)malloc(lstrlen(src)+1);
	} else {
		d=dest;
	}
	SkipSpace(const_cast<const TCHAR *&>(s));

	// 중간의 공백 정리
	for (;*s;) {
		if (*s==' ' || *s=='\t') {
			*d++=' ';
			SkipSpace(const_cast<const TCHAR *&>(s));
		} else {
			*d++=*s++;
		}
	}
	*d=0;

	// 끝 공백 하나가 있으면 제거
	if (d > dest && d[-1]==' ') {
		d[-1]=0;
	}

	if (dest==NULL) {
		lstrcpy(src,dest2);
		free(dest2);
	}
}

HBITMAP MakeDDBFromDIB(TCHAR *Path)
{
	HDC hdc;
	HANDLE hFile;
	DWORD FileSize, dwRead;
	BITMAPFILEHEADER *fh;
	HBITMAP hBit;
	PVOID ih;
	TCHAR AbsPath[MAX_PATH];

	lstrcpy(AbsPath,Path);
	MakeAbsolutePath(AbsPath);
	hFile=CreateFile(AbsPath,GENERIC_READ,0,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (hFile==INVALID_HANDLE_VALUE) {
		return NULL;
	}

	FileSize=GetFileSize(hFile,NULL);

	fh=(BITMAPFILEHEADER *)malloc(FileSize);
	ReadFile(hFile,fh,FileSize,&dwRead,NULL);
	CloseHandle(hFile);

	ih=((PBYTE)fh+sizeof(BITMAPFILEHEADER));
	hdc=GetDC(NULL);
	hBit=CreateDIBitmap(hdc,(BITMAPINFOHEADER *)ih,CBM_INIT,
		(PBYTE)fh+fh->bfOffBits,(BITMAPINFO *)ih,DIB_RGB_COLORS);
	ReleaseDC(NULL,hdc);

	free(fh);
	return hBit;
}

BOOL MakeRelativePath(TCHAR *Path)
{
	TCHAR ExePath[MAX_PATH];
	TCHAR Drive[_MAX_DRIVE];
	TCHAR Dir[_MAX_DIR];

	GetModuleFileName(g_hInst,ExePath,MAX_PATH);
	_splitpath(ExePath,Drive,Dir,NULL,NULL);
	if (strnicmp(ExePath,Path,lstrlen(Drive)+lstrlen(Dir))==0) {
		lstrcpy(ExePath,Path);
		lstrcpy(Path,ExePath+lstrlen(Drive)+lstrlen(Dir));
		return TRUE;
	} else {
		return FALSE;
	}
}

void MakeAbsolutePath(TCHAR *Path)
{
	TCHAR ExePath[MAX_PATH];
	TCHAR Drive[_MAX_DRIVE];
	TCHAR Dir[_MAX_DIR];

	if (strchr(Path,':')==NULL) {
		GetModuleFileName(g_hInst,ExePath,MAX_PATH);
		_splitpath(ExePath,Drive,Dir,NULL,NULL);
		lstrcpy(ExePath,Path);
		wsprintf(Path,"%s%s%s",Drive,Dir,ExePath);
	}
}

BOOL RegisterSvr(TCHAR *DllName, BOOL bReg)
{
	HRESULT (STDAPICALLTYPE * lpDllEntryPoint)(void);
	HINSTANCE hLib;
	BOOL Result=TRUE;

	// DLL 파일을 메모리로 읽어들인다.
	if (FAILED(OleInitialize(NULL))) 
		return FALSE;
	hLib=LoadLibraryEx(DllName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (hLib < (HINSTANCE)HINSTANCE_ERROR) 
		return FALSE;

	// 설치, 또는 해제 함수의 번지를 찾는다.
	if (bReg == TRUE) {
		(FARPROC&)lpDllEntryPoint = GetProcAddress(hLib, "DllRegisterServer");
	} else {
		(FARPROC&)lpDllEntryPoint = GetProcAddress(hLib, "DllUnregisterServer");
	}

	// 함수를 호출한다.
	if (lpDllEntryPoint) {
		if (FAILED((*lpDllEntryPoint)())) 
			Result=FALSE;
	} else {
		Result=FALSE;
	}
	FreeLibrary(hLib);
	OleUninitialize();
	return Result;
}

BOOL MyGetImeMode(HWND hEdit)
{
	HIMC hImc;
	DWORD Con, Sen;

	hImc=ImmGetContext(hEdit);
	ImmGetConversionStatus(hImc,&Con,&Sen);
	ImmReleaseContext(hEdit,hImc );
	if (Con & IME_CMODE_NATIVE) {
		return TRUE;
	} else {
		return FALSE;
	}
}

DWORD MyGetAbsDay(SYSTEMTIME st)
{
	INT64 i64;
	FILETIME fst;

	st.wHour=st.wMinute=st.wSecond=st.wMilliseconds=st.wDayOfWeek=0;
	SystemTimeToFileTime(&st,&fst);
	i64=(((INT64)fst.dwHighDateTime) << 32) + fst.dwLowDateTime;
	i64 = i64 / 864000000000;
	return (DWORD)i64;
}

void MyAbsToSystem(DWORD Abs, SYSTEMTIME &st)
{
	INT64 i64;
	FILETIME fst;

	i64=Abs * 864000000000;
	fst.dwHighDateTime = (DWORD)(i64 >> 32);
	fst.dwLowDateTime = (DWORD)(i64 & 0xffffffff);
	FileTimeToSystemTime(&fst, &st);
}
