#ifndef __INTERNET_H
#define __INTERNET_H

struct FtpInfo
{
	TCHAR Name[32];
	TCHAR Addr[256];
	TCHAR User[32];
	TCHAR Pass[32];
	int Port;
};

BOOL CALLBACK DGHttpProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
int DgHttpDown(TCHAR *URL, TCHAR *&Text);
BOOL CALLBACK DGDownProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK DGFtpConfigProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK DGFtpProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
BOOL DgFtpConnect(TCHAR *Server, TCHAR *User, TCHAR *Pass, int Port);
void DgFtpUnConnect();
void DgFtpAddFile(HWND hDlg,WIN32_FIND_DATA wfd);
void DgFtpEnumFiles(HWND hDlg);
void LoadFtpServerList(HWND hDlg);
int DgFtpDown(TCHAR *Path, TCHAR *&Text);
BOOL DgFtpUp(TCHAR *Path, TCHAR *Text);
void EnableFtpButton(HWND hDlg,BOOL bEnable);
void FtpNewFile(HWND hDlg,HWND hList,BOOL bFolder);
void FtpDelete(HWND hDlg,HWND hList);
BOOL FtpDeleteFolder(TCHAR *Path);
void FtpDownLoad(HWND hDlg,HWND hList);
BOOL FtpDownLoadFile(TCHAR *Path,TCHAR *DownPath,unsigned FileSize);
BOOL FtpDownLoadFolder(TCHAR *Path,TCHAR *DownPath);
void FtpUpLoad(HWND hDlg,HWND hList,HDROP hDrop);
BOOL FtpUpLoadFile(TCHAR *Path);
BOOL FtpUpLoadFolder(TCHAR *Path);

#endif // __INTERNET_H