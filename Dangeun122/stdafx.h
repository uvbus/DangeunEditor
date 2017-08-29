#define _WIN32_WINNT 0x400
#define _WIN32_WINDOWS 0x401
#define WINVER 0x500

#pragma warning (disable:4996)            // deprecated 경고 무시

#include <windows.h>
#include <windowsx.h>
#include <imm.h>
#include <stdio.h>
#include <io.h>
#include <malloc.h>
#include <commctrl.h>
#include <Shlwapi.h>
#include <Shlobj.h>
#include <wininet.h>
#include <assert.h>
#include "Parse.h"
#include "Util.h"
#include "resource.h"
#include "AeUtil.h"
#include "ApiEdit.h"
#include "ApiEditText.h"
#include "ApiEditHexa.h"
#include "Project.h"
#include "Config.h"
#include "FileWnd.h"
#include "Internet.h"
#include "Option.h"
#include "Mru.h"
#include "Print.h"
#include "ToolBar.h"
#include "Find.h"
#include "Dangeun.h"
#include "Macro.h"
#include "ShortCut.h"
#include "Clip.h"
#include "global.h"

