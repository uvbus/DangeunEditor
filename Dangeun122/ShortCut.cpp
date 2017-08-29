#include "stdafx.h"

// 가상키 목록
SVirtKey arVirtKey[]={
	{"A",'A'},{"B",'B'},{"C",'C'},{"D",'D'},{"E",'E'},{"F",'F'},{"G",'G'},{"H",'H'},
	{"I",'I'},{"J",'J'},{"K",'K'},{"L",'L'},{"M",'M'},{"N",'N'},{"O",'O'},{"P",'P'},
	{"Q",'Q'},{"R",'R'},{"S",'S'},{"T",'T'},{"U",'U'},{"V",'V'},{"W",'W'},{"X",'X'},
	{"Y",'Y'},{"Z",'Z'},{"1",'1'},{"2",'2'},{"3",'3'},{"4",'4'},{"5",'5'},{"6",'6'},
	{"7",'7'},{"8",'8'},{"9",'9'},{"0",'0'},{"F1",VK_F1},{"F2",VK_F2},{"F3",VK_F3},
	{"F4",VK_F4},{"F5",VK_F5},{"F6",VK_F6},{"F7",VK_F7},{"F8",VK_F8},{"F9",VK_F9},
	{"F10",VK_F10},{"F11",VK_F11},{"F12",VK_F12},
	{"~(`)",192},{"-(_)",189},{"=(+)",187},{"[(})",219},{"](})",221},{";(:)",186},
	{"'(\")",222},{",(<)",188},{".(>)",190},{"/(?)",191},{"\\(|)",220}
};

int GetVirtKeyIndex(UINT vk)
{
	int i;

	for (i=0;i<ARSIZE(arVirtKey);i++) {
		if (arVirtKey[i].vk == vk) {
			return i;
		}
	}
	return -1;
}

// 메뉴 항목 목록
SMenuCom arMenuCom[]={
	{"파일/새 파일",IDM_FILE_NEW},
	{"파일/열기",IDM_FILE_OPEN},
	{"파일/저장",IDM_FILE_SAVE},
	{"파일/새 이름으로",IDM_FILE_SAVEAS},
	{"파일/닫기",IDM_FILE_CLOSE},
	{"파일/모두 저장",IDM_FILE_SAVEALL},
	{"파일/모두 닫기",IDM_FILE_CLOSEALL},
	{"파일/FTP 열기",IDM_FILE_FTPOPEN},
	{"파일/HTTP 열기",IDM_FILE_HTTPOPEN},
	{"파일/포맷 변환/ANSI",IDM_FILE_ANSI},
	{"파일/포맷 변환/유니코드",IDM_FILE_UNI},
	{"파일/포맷 변환/유니코드 빅",IDM_FILE_UNIBIG},
	{"파일/포맷 변환/유니코드(NB)",IDM_FILE_UNINOBOM},
	{"파일/포맷 변환/유니코드 빅(NB)",IDM_FILE_UNIBIGNOBOM},
	{"파일/포맷 변환/UTF-8",IDM_FILE_UTF8},
	{"파일/포맷 변환/UTF-8(NB)",IDM_FILE_UTF8NOBOM},
	{"파일/읽기 전용",IDM_FILE_READONLY},
	{"파일/최후 저장 상태로 되돌리기",IDM_FILE_REVERT},
	{"파일/인쇄",IDM_FILE_PRINT},
	{"파일/문서 정보",IDM_FILE_INFO},
	{"파일/프로젝트 관리",IDM_FILE_PROJECT},
	{"파일/최근 파일1",45000},
	{"파일/최근 파일2",45001},
	{"파일/최근 파일3",45002},
	{"파일/최근 파일4",45003},
	{"파일/최근 파일5",45004},
	{"파일/최근 파일6",45005},
	{"파일/최근 파일7",45006},
	{"파일/최근 파일8",45007},
	{"파일/최근 파일9",45008},
	{"파일/최근 파일10",45009},
	{"파일/종료",IDM_FILE_EXIT},

	{"편집/취소",IDM_EDIT_UNDO},
	{"편집/재실행",IDM_EDIT_REDO},
	{"편집/복사",IDM_EDIT_COPY},
	{"편집/잘라내기",IDM_EDIT_CUT},
	{"편집/붙여넣기",IDM_EDIT_PASTE},
	{"편집/전체선택",IDM_EDIT_SELALL},
	{"편집/복사해서 추가",IDM_EDIT_COPYAPPEND},
	{"편집/잘라서 추가",IDM_EDIT_CUTAPPEND},
	{"편집/변환/대문자로",IDM_EDIT_UPPER},
	{"편집/변환/소문자로",IDM_EDIT_LOWER},
	{"편집/변환/대소문자 반대로",IDM_EDIT_REVERSE},
	{"편집/변환/첫글자만 대문자로",IDM_EDIT_UPPERFIRST},
	{"편집/변환/문장 처음만 대문자로",IDM_EDIT_UPPERSENT},
	{"편집/변환/공백을 탭으로",IDM_EDIT_TOTAB},
	{"편집/변환/탭을 공백으로",IDM_EDIT_TOSPACE},
	{"편집/변환/불필요한 공백 제거",IDM_EDIT_REMOVESPACE},
	{"편집/변환/들여쓰기",IDM_EDIT_INDENT},
	{"편집/변환/내어쓰기",IDM_EDIT_UNINDENT},
	{"편집/변환/주석으로",IDM_EDIT_COMMENT},
	{"편집/변환/주석제거",IDM_EDIT_UNCOMMENT},
	{"편집/삽입/날짜",IDM_EDIT_DATE},
	{"편집/삽입/시간",IDM_EDIT_TIME},
	{"편집/삽입/윗줄 문자 반복",IDM_EDIT_COPYUP},
	{"편집/삽입/아래줄 문자 반복",IDM_EDIT_COPYDOWN},
	{"편집/삽입/파일",IDM_EDIT_INSERTFILE},
	{"편집/삽입/특수문자",IDM_EDIT_INSERTSPECIAL},
	{"편집/가상 공백",IDM_EDIT_VIRT},
	{"편집/열 모드",IDM_EDIT_COLUMN},
	{"편집/명령 모드",IDM_EDIT_COMMODE},

	{"보기/자동 개행",IDM_VIEW_WORDWRAP},
	{"보기/제어 코드 보기",IDM_VIEW_CODE},
	{"보기/활성창의 글꼴/시스템",IDM_FONT_1},
	{"보기/활성창의 글꼴/굴림 10",IDM_FONT_2},
	{"보기/활성창의 글꼴/굴림 12",IDM_FONT_3},
	{"보기/활성창의 글꼴/굴림체 10",IDM_FONT_4},
	{"보기/활성창의 글꼴/굴림체 12",IDM_FONT_5},
	{"보기/활성창의 글꼴/궁서 12",IDM_FONT_6},
	{"보기/활성창의 글꼴/궁서 15",IDM_FONT_7},
	{"보기/활성창의 글꼴/궁서 20",IDM_FONT_8},
	{"보기/활성창의 글꼴/바탕 12",IDM_FONT_9},
	{"보기/활성창의 글꼴/바탕 15",IDM_FONT_10},
	{"보기/활성창의 글꼴/글꼴 선택",IDM_FONT_11},
	{"보기/활성창의 줄간/100",IDM_LINE_1},
	{"보기/활성창의 줄간/120",IDM_LINE_2},
	{"보기/활성창의 줄간/150",IDM_LINE_3},
	{"보기/활성창의 줄간/200",IDM_LINE_4},
	{"보기/활성창의 줄간/300",IDM_LINE_5},
	{"보기/문법 강조/없음",IDM_SYN_NONE},
	{"보기/문법 강조/C/C++C",IDM_SYN_CPP},
	{"보기/문법 강조/HTML",IDM_SYN_HTML},
	{"보기/문법 강조/SQL",IDM_SYN_SQL},
	{"보기/문법 강조/SQL",IDM_SYN_SQL},
	{"보기/문법 강조/Java",IDM_SYN_JAVA},
	{"보기/16진 보기",IDM_VIEW_HEXA},
	{"보기/툴바",IDM_VIEW_TOOLBAR},
	{"보기/상태란",IDM_VIEW_STATUS},
	{"보기/파일창",IDM_VIEW_FILE},
	{"보기/파일탭",IDM_VIEW_FILETAB},
	{"보기/검색 결과",IDM_VIEW_OUTPUT},
	{"보기/상용구 창",IDM_VIEW_CLIP},

	{"검색/찾기",IDM_SEARCH_FIND},
	{"검색/다음 찾기",IDM_SEARCH_NEXT},
	{"검색/이전 찾기",IDM_SEARCH_PREV},
	{"검색/현재 단어 찾기",IDM_SEARCH_WORD},
	{"검색/현재 단어 위로",IDM_SEARCH_WORDUP},
	{"검색/바꾸기",IDM_SEARCH_REPLACE},
	{"검색/책갈피 설정/해제",IDM_SEARCH_TOGGLEMARK},
	{"검색/다음 책갈피로",IDM_SEARCH_NEXTMARK},
	{"검색/이전 책갈피로",IDM_SEARCH_PREVMARK},
	{"검색/책갈피 모두 해제",IDM_SEARCH_CLEARMARK},
	{"검색/줄 찾기",IDM_SEARCH_GOTOLINE},
	{"검색/괄호 짝 찾기",IDM_SEARCH_PAREN},
	{"검색/괄호 짝 선택",IDM_SEARCH_PARENSEL},
	{"검색/마지막 편집 위치",IDM_SEARCH_LASTEDIT},
	{"검색/파일에서 찾기",IDM_SEARCH_FILES},
	{"검색/파일에서 바꾸기",IDM_SEARCH_RFILES},
	{"검색/바로 찾기",IDM_SEARCH_INST},

	{"도구/기본 설정",IDM_TOOL_CONFIG},
	{"도구/설치 정보 삭제",IDM_TOOL_UNINSTALL},
	{"도구/매크로",IDM_TOOL_MACRO},
	{"도구/단축키 정의",IDM_TOOL_SHORTCUT},
	{"도구/상용구 관리",IDM_TOOL_CLIP},
	{"도구/읽기 모드",IDM_TOOL_READMODE},
	{"도구/문단 재구성",IDM_TOOL_PARA},

	{"창/계단식 정렬",IDM_WIN_CASCADE},
	{"창/수평 정렬",IDM_WIN_HORZ},
	{"창/수직 정렬",IDM_WIN_VERT},
	{"창/아이콘 정렬",IDM_WIN_ARRANGE},
	{"창/다음 뷰",IDM_WIN_NEXTVIEW},
	{"창/이전 뷰",IDM_WIN_PREVVIEW},

	{"도움말/프로그램 소개",IDM_HELP_ABOUT},
	{"도움말/도움말",IDM_HELP_INDEX},
	{"도움말/홈페이지 방문",IDM_HELP_HOME}
};

int GetMenuComIndex(UINT id)
{
	int i;

	for (i=0;i<ARSIZE(arMenuCom);i++) {
		if (arMenuCom[i].id == id) {
			return i;
		}
	}
	return -1;
}

CShortCut::CShortCut()
{
	arShortCut=NULL;
	bModified=FALSE;
}

CShortCut::~CShortCut()
{
	Exit();
}

void CShortCut::Exit()
{
	int i;

	// 단축키 배열과 매크로 문자열을 해제한다.
	if (arShortCut) {
		for (i=0;arShortCut[i].vk;i++) {
			if (arShortCut[i].Com) {
				free(arShortCut[i].Com);
			}
		}
		free(arShortCut);
		arShortCut=NULL;
	}
}

// 디폴트 단축키 목록
SShortCut DefaultShortCut[]={
	{'N',	SC_CTRL,				IDM_FILE_NEW,			NULL},
	{'O',	SC_CTRL,				IDM_FILE_OPEN,			NULL},
	{VK_F4,	SC_CTRL,				IDM_FILE_CLOSE,			NULL},
	{'S',	SC_CTRL,				IDM_FILE_SAVE,			NULL},
	{'S',	SC_CTRL | SC_SHIFT,		IDM_FILE_SAVEALL,		NULL},
	{'C',	SC_CTRL | SC_ALT,		IDM_FILE_CLOSEALL,		NULL},
	{'O',	SC_CTRL | SC_ALT,		IDM_FILE_FTPOPEN,		NULL},
	{'P',	SC_CTRL,				IDM_FILE_PRINT,			NULL},
	{VK_F4,	SC_ALT,					IDM_FILE_EXIT,			NULL},
	{'Z',	SC_CTRL,				IDM_EDIT_UNDO,			NULL},
	{'Y',	SC_CTRL,				IDM_EDIT_REDO,			NULL},
	{'C',	SC_CTRL,				IDM_EDIT_COPY,			NULL},
	{'X',	SC_CTRL,				IDM_EDIT_CUT,			NULL},
	{'V',	SC_CTRL,				IDM_EDIT_PASTE,			NULL},
	{'A',	SC_CTRL,				IDM_EDIT_SELALL,		NULL},
	{'C',	SC_CTRL | SC_SHIFT,		IDM_EDIT_COPYAPPEND,	NULL},
	{'X',	SC_CTRL | SC_SHIFT,		IDM_EDIT_CUTAPPEND,		NULL},
	{'U',	SC_CTRL,				IDM_EDIT_UPPER,			NULL},
	{'L',	SC_CTRL,				IDM_EDIT_LOWER,			NULL},
	{'K',	SC_CTRL,				IDM_EDIT_INSERTSPECIAL,	NULL},
	{'M',	SC_CTRL,				IDM_EDIT_COMMENT,		NULL},
	{'M',	SC_CTRL | SC_SHIFT,		IDM_EDIT_UNCOMMENT,		NULL},
	{'I',	SC_CTRL,				IDM_EDIT_COPYUP,		NULL},
	{'I',	SC_CTRL | SC_SHIFT,		IDM_EDIT_COPYDOWN,		NULL},
	{'C',	SC_ALT,					IDM_EDIT_COLUMN,		NULL},
	{'X',	SC_ALT,					IDM_EDIT_COMMODE,		NULL},
	{'W',	SC_CTRL,				IDM_VIEW_WORDWRAP,		NULL},
	{'E',	SC_CTRL,				IDM_VIEW_CODE,			NULL},
	{'H',	SC_CTRL,				IDM_VIEW_HEXA,			NULL},
	{'F',	SC_CTRL,				IDM_SEARCH_FIND,		NULL},
	{VK_F3,	0,						IDM_SEARCH_NEXT,		NULL},
	{VK_F3,	SC_SHIFT,				IDM_SEARCH_PREV,		NULL},
	{VK_F3,	SC_CTRL,				IDM_SEARCH_WORD,		NULL},
	{VK_F3,	SC_CTRL | SC_SHIFT,		IDM_SEARCH_WORDUP,		NULL},
	{'R',	SC_CTRL,				IDM_SEARCH_REPLACE,		NULL},
	{VK_F2,	0,						IDM_SEARCH_NEXTMARK,	NULL},
	{VK_F2,	SC_SHIFT,				IDM_SEARCH_PREVMARK,	NULL},
	{VK_F2,	SC_CTRL,				IDM_SEARCH_TOGGLEMARK,	NULL},
	{'G',	SC_CTRL,				IDM_SEARCH_GOTOLINE,	NULL},
	{'B',	SC_CTRL,				IDM_SEARCH_PAREN,		NULL},
	{'B',	SC_CTRL | SC_SHIFT,		IDM_SEARCH_PARENSEL,	NULL},
	{'Q',	SC_CTRL,				IDM_SEARCH_LASTEDIT,	NULL},
	{'F',	SC_CTRL | SC_SHIFT,		IDM_SEARCH_FILES,		NULL},
	{VK_F4,	0,						IDM_SEARCH_INST,		NULL},
	{'T',	SC_CTRL,				IDM_TOOL_CONFIG,		NULL},
	{'M',	SC_CTRL | SC_ALT,		IDM_TOOL_MACRO,			NULL},
	{'R',	SC_CTRL | SC_ALT,		IDM_TOOL_READMODE,		NULL},
	{'P',	SC_CTRL | SC_ALT,		IDM_TOOL_PARA,			NULL},
	{VK_F1,	0,						IDM_HELP_INDEX,			NULL},
	{VK_F6,	0,						IDM_WIN_NEXTVIEW,		NULL},
	{VK_F6,	SC_SHIFT,				IDM_WIN_PREVVIEW,		NULL},

	{'1',	SC_ALT,					IDM_FONT_1,				NULL},
	{'2',	SC_ALT,					IDM_FONT_2,				NULL},
	{'3',	SC_ALT,					IDM_FONT_3,				NULL},
	{'4',	SC_ALT,					IDM_FONT_4,				NULL},
	{'5',	SC_ALT,					IDM_FONT_5,				NULL},
	{'6',	SC_ALT,					IDM_FONT_6,				NULL},
	{'7',	SC_ALT,					IDM_FONT_7,				NULL},
	{'8',	SC_ALT,					IDM_FONT_8,				NULL},
	{'9',	SC_ALT,					IDM_FONT_9,				NULL},
	{'0',	SC_ALT,					IDM_FONT_10,			NULL},
	{'1',	SC_CTRL | SC_ALT,		45000,					NULL},
	{'2',	SC_CTRL | SC_ALT,		45001,					NULL},
	{'3',	SC_CTRL | SC_ALT,		45002,					NULL},
	{'4',	SC_CTRL | SC_ALT,		45003,					NULL},
	{'5',	SC_CTRL | SC_ALT,		45004,					NULL},
	{'6',	SC_CTRL | SC_ALT,		45005,					NULL},
	{'7',	SC_CTRL | SC_ALT,		45006,					NULL},
	{'8',	SC_CTRL | SC_ALT,		45007,					NULL},
	{'9',	SC_CTRL | SC_ALT,		45008,					NULL},
	{'0',	SC_CTRL | SC_ALT,		45009,					NULL},
	{'1',	SC_ALT | SC_SHIFT,		IDM_SYN_CPP,			NULL},
	{'2',	SC_ALT | SC_SHIFT,		IDM_SYN_HTML,			NULL},
	{'3',	SC_ALT | SC_SHIFT,		IDM_SYN_SQL,			NULL},
	{'4',	SC_ALT | SC_SHIFT,		IDM_SYN_JAVA,			NULL},
	{'5',	SC_ALT | SC_SHIFT,		IDM_SYN_CS,				NULL},
	{'0',	SC_ALT | SC_SHIFT,		IDM_SYN_NONE,			NULL},
	{188,	SC_CTRL | SC_SHIFT,		IDM_LINE_1,				NULL},
	{190,	SC_CTRL | SC_SHIFT,		IDM_LINE_2,				NULL},
	{191,	SC_CTRL | SC_SHIFT,		IDM_LINE_3,				NULL},
	{186,	SC_CTRL | SC_SHIFT,		IDM_LINE_4,				NULL},
	{222,	SC_CTRL | SC_SHIFT,		IDM_LINE_5,				NULL},
	{'1',	SC_CTRL,				0,		"gomark 1"},
	{'2',	SC_CTRL,				0,		"gomark 2"},
	{'3',	SC_CTRL,				0,		"gomark 3"},
	{'4',	SC_CTRL,				0,		"gomark 4"},
	{'5',	SC_CTRL,				0,		"gomark 5"},
	{'6',	SC_CTRL,				0,		"gomark 6"},
	{'7',	SC_CTRL,				0,		"gomark 7"},
	{'8',	SC_CTRL,				0,		"gomark 8"},
	{'9',	SC_CTRL,				0,		"gomark 9"},
	{'0',	SC_CTRL,				0,		"gomark 0"},
	{'1',	SC_CTRL | SC_SHIFT,		0,		"setmark 1"},
	{'2',	SC_CTRL | SC_SHIFT,		0,		"setmark 2"},
	{'3',	SC_CTRL | SC_SHIFT,		0,		"setmark 3"},
	{'4',	SC_CTRL | SC_SHIFT,		0,		"setmark 4"},
	{'5',	SC_CTRL | SC_SHIFT,		0,		"setmark 5"},
	{'6',	SC_CTRL | SC_SHIFT,		0,		"setmark 6"},
	{'7',	SC_CTRL | SC_SHIFT,		0,		"setmark 7"},
	{'8',	SC_CTRL | SC_SHIFT,		0,		"setmark 8"},
	{'9',	SC_CTRL | SC_SHIFT,		0,		"setmark 9"},
	{'0',	SC_CTRL | SC_SHIFT,		0,		"setmark 0"},

	{'A',	0,						0,		"select word;copy"},
	{'A',	SC_SHIFT,				0,		"select word"},
	{'B',	0,						IDM_SEARCH_PAREN,		NULL},
	{'B',	SC_SHIFT,				IDM_SEARCH_PARENSEL,	NULL},
	{'C',	0,						IDM_EDIT_COPY,			NULL},
	{'C',	SC_SHIFT,				IDM_EDIT_COPYAPPEND,	NULL},
	{'D',	0,						0,		"movesel end;delete"},
	{'D',	SC_SHIFT,				0,		"movesel paraend;delete"},
	{'E',	0,						0,		"select line;delete"},
	{'E',	SC_SHIFT,				0,		"select para;delete"},
	{'F',	0,						IDM_SEARCH_WORD,		NULL},
	{'F',	SC_SHIFT,				IDM_SEARCH_WORDUP,		NULL},
	{'G',	0,						0,		"move down;move end"},
	{'H',	0,						0,		"movesel home;delete"},
	{'H',	SC_SHIFT,				0,		"movesel parahome;delete"},
	{'I',	0,						IDM_EDIT_COPYUP,		NULL},
	{'I',	SC_SHIFT,				IDM_EDIT_COPYDOWN,		NULL},
	{'J',	0,						0,		"select para;copy;move parahome;paste;move prevpara"},
	{'J',	SC_SHIFT,				0,		"move nextpara;select para;copy;move parahome;paste;move prevpara 2;"},
	{'L',	0,						IDM_EDIT_LOWER,			NULL},
	{'L',	SC_SHIFT,				0,		"select word;convert lower"},
	{'O',	0,						0,		"select line;copy"},
	{'O',	SC_SHIFT,				0,		"select line"},
	{'P',	0,						0,		"select para;copy"},
	{'P',	SC_SHIFT,				0,		"select para"},
	{'Q',	0,						IDM_SEARCH_LASTEDIT,	NULL},
	{'U',	0,						IDM_EDIT_UPPER,			NULL},
	{'U',	SC_SHIFT,				0,		"select word;convert upper"},
	{'V',	0,						IDM_EDIT_PASTE,			NULL},
	{'W',	0,						IDM_VIEW_WORDWRAP,			NULL},
	{'X',	0,						IDM_EDIT_CUT,			NULL},
	{'X',	SC_SHIFT,				IDM_EDIT_CUTAPPEND,		NULL},
	{'Y',	0,						IDM_EDIT_REDO,			NULL},
	{'Z',	0,						IDM_EDIT_UNDO,			NULL}
};

void CShortCut::Init()
{
	int i;

	Exit();
	arShortCut=(SShortCut *)malloc(256*sizeof(SShortCut));
	memset(arShortCut,0,256*sizeof(SShortCut));

	for (i=0;i<ARSIZE(DefaultShortCut);i++) {
		Add(&DefaultShortCut[i]);
	}
}

void CShortCut::Add(SShortCut *sc)
{
	int last,size;

	// 배열의 끝 인덱스 찾음
	if (arShortCut==NULL) {
		arShortCut=(SShortCut *)malloc(256*sizeof(SShortCut));
		memset(arShortCut,0,256*sizeof(SShortCut));
		last=0;
	} else {
		for (last=0;;last++) {
			if (arShortCut[last].vk == 0) {
				break;
			}
		}
		// 배열 크기 관리
		size=_msize(arShortCut)/sizeof(arShortCut[0]);
		if (last == size-1) {
			size+=10;
			arShortCut=(SShortCut *)realloc(arShortCut,size*sizeof(SShortCut));
			memset(arShortCut+size-10,0,10*sizeof(SShortCut));
		}
	}

	// 매크로 명령이면 동적 할당한 후 매크로 문자열을 복사한다.
	arShortCut[last]=*sc;
	if (sc->id == 0) {
		arShortCut[last].Com=(TCHAR *)malloc(lstrlen(sc->Com)+1);
		lstrcpy(arShortCut[last].Com,sc->Com);
	} else {
		arShortCut[last].Com=NULL;
	}
}

void CShortCut::Delete(int idx)
{
	int i;

	// 매크로 문자열 해제
	if (arShortCut[idx].Com) {
		free(arShortCut[idx].Com);
	}

	// 뒤쪽 단축키를 한칸씩 앞으로 옮긴다.
	for (i=idx+1;;i++) {
		arShortCut[i-1]=arShortCut[i];
		if (arShortCut[i].vk==0) {
			break;
		}
	}
}

void CShortCut::Modify(int idx,SShortCut *sc)
{
	// 이전 단축키에 할당된 명령이 있으면 일단 해제
	if (arShortCut[idx].Com) {
		free(arShortCut[idx].Com);
		arShortCut[idx].Com=NULL;
	}
	arShortCut[idx]=*sc;
	// 매크로 명령이면 메모리를 다시 할당한 후 복사한다.
	if (sc->id == 0) {
		arShortCut[idx].Com=(TCHAR *)malloc(lstrlen(sc->Com)+1);
		lstrcpy(arShortCut[idx].Com,sc->Com);
	} 
}

// 가상 키와 플래그가 같은 단축키 번호 찾음
int CShortCut::Find(UINT vk,DWORD Flag)
{
	int i;

	for (i=0;arShortCut[i].vk;i++) {
		if (arShortCut[i].vk == vk && (arShortCut[i].Flag & 0x7) == Flag) {
			return i;
		}
	}

	return -1;
}

// 명령 또는 매크로 명령이 같은 단축키 번호 찾음
int CShortCut::Find(UINT id,TCHAR *Com)
{
	int i;

	for (i=0;arShortCut[i].vk;i++) {
		if (id == 0) {
			if (arShortCut[i].id==0 && lstrcmp(arShortCut[i].Com,Com)==0) {
				return i;
			}
		} else {
			if (arShortCut[i].id==id) {
				return i;
			}
		}
	}

	return -1;
}

void CShortCut::Load(TCHAR *Key)
{
	TCHAR szKey[MAX_PATH];
	TCHAR szSub[MAX_PATH];
	TCHAR Com[1024];
	SShortCut sc;
	int i,Num;

	lstrcpy(szKey,Key);
	lstrcat(szKey,"ShortCut");

	Num=SHRegReadInt(SHCU,szKey,"Num",0);
	for (i=0;i<Num;i++) {
		wsprintf(szSub,"Key%d",i);
		sc.vk=SHRegReadInt(SHCU,szKey,szSub,0);
		wsprintf(szSub,"Flag%d",i);
		sc.Flag=SHRegReadInt(SHCU,szKey,szSub,0);
		wsprintf(szSub,"id%d",i);
		sc.id=SHRegReadInt(SHCU,szKey,szSub,0);
		if (sc.id == 0) {
			wsprintf(szSub,"Com%d",i);
			SHRegReadString(SHCU,szKey,szSub,"",Com,1024);
			sc.Com=Com;
		}

		Add(&sc);
	}
}

void CShortCut::Save(TCHAR *Key)
{
	TCHAR szKey[MAX_PATH];
	TCHAR szSub[MAX_PATH];
	int i;

	lstrcpy(szKey,Key);
	lstrcat(szKey,"ShortCut");

	for (i=0;arShortCut[i].vk;i++) {
		wsprintf(szSub,"Key%d",i);
		SHRegWriteInt(SHCU,szKey,szSub,arShortCut[i].vk);
		wsprintf(szSub,"Flag%d",i);
		SHRegWriteInt(SHCU,szKey,szSub,arShortCut[i].Flag);
		wsprintf(szSub,"id%d",i);
		SHRegWriteInt(SHCU,szKey,szSub,arShortCut[i].id);
		if (arShortCut[i].id == 0) {
			wsprintf(szSub,"Com%d",i);
			SHRegWriteString(SHCU,szKey,szSub,arShortCut[i].Com);
		}
	}
	SHRegWriteInt(SHCU,szKey,"Num",i);
} 

BOOL CALLBACK ShortCutListProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	static HWND hList;
	LVCOLUMN COL;
	LVITEM LI;
	int idx;

	switch(iMessage) {
	case WM_INITDIALOG:
		MoveToParentCenter(hDlg);
		hList=GetDlgItem(hDlg,IDC_SHORTCUTLIST);
		COL.mask=LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		COL.fmt=LVCFMT_LEFT;
		COL.cx=50;
		COL.pszText="키";
		COL.iSubItem=0;
		ListView_InsertColumn(hList,0,&COL);

		COL.cx=80;
		COL.pszText="조합키";
		COL.iSubItem=1;
		ListView_InsertColumn(hList,1,&COL);

		COL.cx=60;
		COL.pszText="종류";
		COL.iSubItem=2;
		ListView_InsertColumn(hList,2,&COL);

		COL.cx=200;
		COL.pszText="기능";
		COL.iSubItem=3;
		ListView_InsertColumn(hList,3,&COL);

		ListView_SetExtendedListViewStyle(hList,LVS_EX_FULLROWSELECT);
		DisplayShortCutList(hDlg);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BTNADD:
			if (DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_SHORTCUT), 
				hDlg, ShortCutProc,(LPARAM)-1)==IDOK) {
				DisplayShortCutList(hDlg);
				ListView_SetItemState(hList,ListView_GetItemCount(hList),
					LVIS_SELECTED | LVIS_FOCUSED,LVIS_SELECTED | LVIS_FOCUSED);
			}
			return TRUE;
		case IDC_BTNEDIT:
			idx=ListView_GetNextItem(hList,-1,LVNI_ALL | LVNI_SELECTED);
			if (idx != -1) {
				LI.iItem=idx;
				LI.mask=LVIF_PARAM;
				ListView_GetItem(hList,&LI);
				if (DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_SHORTCUT), 
					hDlg, ShortCutProc,(LPARAM)LI.lParam)==IDOK) {
					DisplayShortCutList(hDlg);
				}
			}
			return TRUE;
		case IDC_BTNDELETE:
			idx=ListView_GetNextItem(hList,-1,LVNI_ALL | LVNI_SELECTED);
			if (idx != -1) {
				LI.iItem=idx;
				LI.mask=LVIF_PARAM;
				ListView_GetItem(hList,&LI);
				DgShortCut.Delete(LI.lParam);
				DisplayShortCutList(hDlg);
			}
			return TRUE;
		case IDC_BTNDEFAULT:
			if (MessageBox(hDlg,"모든 단축키 설정을 설치 초기 상태로 바꾸시겠습니까?",
				"질문",MB_YESNO)==IDYES) {
				DgShortCut.Init();
				DisplayShortCutList(hDlg);
			}
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg,IDCANCEL);
			return TRUE;
		}
		return FALSE;
	case WM_NOTIFY:
		LPNMHDR hdr;
		hdr=(LPNMHDR)lParam;
		if (hdr->hwndFrom == hList) {
			switch (hdr->code) {
			case LVN_ITEMCHANGED:
				idx=ListView_GetNextItem(hList,-1,LVNI_ALL | LVNI_SELECTED);
				EnableWindow(GetDlgItem(hDlg,IDC_BTNEDIT),(idx != -1));
				EnableWindow(GetDlgItem(hDlg,IDC_BTNDELETE),(idx != -1));
				break;
			case NM_DBLCLK:
				PostMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_BTNEDIT,0),0);
				return TRUE;
			}
		}
		break;
	}
	return FALSE;
}

// 현재 단축키 설정 상태 출력. 키로 정렬한다.
void DisplayShortCutList(HWND hDlg)
{
	HWND hList;
	int i,j;
	LVITEM LI,LI2;
	int idx;
	TCHAR szTemp[128];
	TCHAR Text[12];

	hList=GetDlgItem(hDlg,IDC_SHORTCUTLIST);
	ListView_DeleteAllItems(hList);
	for (i=0;DgShortCut.arShortCut[i].vk;i++) {
		LI.mask=LVIF_TEXT | LVIF_PARAM;
		LI.iSubItem=0;
		LI.lParam=i;
		j=GetVirtKeyIndex(DgShortCut.arShortCut[i].vk);
		LI.pszText=arVirtKey[j].Name;

		// 삽입할 위치 선정
		for (idx=0;;idx++) {
			LI2.mask=LVIF_TEXT;
			LI2.iItem=idx;
			LI2.iSubItem=0;
			LI2.pszText=Text;
			LI2.cchTextMax=12;
			if (ListView_GetItem(hList,&LI2)==FALSE) {
				break;
			}
			if (lstrcmpi(LI.pszText, Text) < 0) {
				break;
			}
		}

		LI.iItem=idx;
		ListView_InsertItem(hList,&LI);

		lstrcpy(szTemp,"");
		if (DgShortCut.arShortCut[i].Flag & SC_CTRL) {
			lstrcpy(szTemp,"Ctrl");
		}
		if (DgShortCut.arShortCut[i].Flag & SC_ALT) {
			if (lstrlen(szTemp)) {
				lstrcat(szTemp,"+");
			}
			lstrcat(szTemp,"Alt");
		}
		if (DgShortCut.arShortCut[i].Flag & SC_SHIFT) {
			if (lstrlen(szTemp)) {
				lstrcat(szTemp,"+");
			}
			lstrcat(szTemp,"Shift");
		}

		ListView_SetItemText(hList,idx,1,szTemp);
		
		if (DgShortCut.arShortCut[i].id) {
			ListView_SetItemText(hList,idx,2,"메뉴");
			j=GetMenuComIndex(DgShortCut.arShortCut[i].id);
			ListView_SetItemText(hList,idx,3,arMenuCom[j].Name);
		} else {
			ListView_SetItemText(hList,idx,2,"매크로");
			ListView_SetItemText(hList,idx,3,DgShortCut.arShortCut[i].Com);
		}
	}
}

// 단축키 추가 및 수정 대화상자. 파라미터는 수정 대상 단축키이되 -1이면 추가
BOOL CALLBACK ShortCutProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	static int objidx;
	int oriidx;
	int vk,id;
	int i;
	TCHAR szCom[1024];
	TCHAR *szMacro, *Com;
	TCHAR Mes[256];
	SShortCut sc;

	switch(iMessage) {
	case WM_INITDIALOG:
		MoveToParentCenter(hDlg);
		objidx=(int)lParam;
		for (i=0;i<ARSIZE(arVirtKey);i++) {
			SendDlgItemMessage(hDlg,IDC_CBKEY,CB_ADDSTRING,0,(LPARAM)arVirtKey[i].Name);
		}
		for (i=0;i<ARSIZE(arMenuCom);i++) {
			SendDlgItemMessage(hDlg,IDC_CBMENUCOM,CB_ADDSTRING,0,(LPARAM)arMenuCom[i].Name);
		}
		// 편집이면 설정된 단축키를 보여준다.
		if (objidx != -1) {
			SetDlgShortCut(hDlg,objidx);

			// 편집 대상 단축키의 내용을 보여준다.
			oriidx=GetDlgShortCut(hDlg);
			if (DgShortCut.arShortCut[oriidx].id) {
				CheckRadioButton(hDlg,IDC_RADIOID,IDC_RADIOCOMMAND,IDC_RADIOID);
				SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_RADIOID,0),0);
				id=GetMenuComIndex(DgShortCut.arShortCut[oriidx].id);
				SendDlgItemMessage(hDlg,IDC_CBMENUCOM,CB_SELECTSTRING,-1,
					(LPARAM)arMenuCom[id].Name);
			} else {
				CheckRadioButton(hDlg,IDC_RADIOID,IDC_RADIOCOMMAND,IDC_RADIOCOMMAND);
				SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_RADIOCOMMAND,0),0);
				SetDlgItemText(hDlg,IDC_EDCOMMAND,DgShortCut.arShortCut[oriidx].Com);
			}
		// 추가이면 메뉴 명령 선택 상태로 만들어 준다. Ctrl키는 기본 선택해 준다.
		} else {
			CheckDlgButton(hDlg,IDC_CHKCTRL,BST_CHECKED);
			CheckRadioButton(hDlg,IDC_RADIOID,IDC_RADIOCOMMAND,IDC_RADIOID);
			SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_RADIOID,0),0);
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		// 메뉴 명령 선택 모드
		case IDC_RADIOID:
			EnableWindow(GetDlgItem(hDlg,IDC_CBMENUCOM),TRUE);
			EnableWindow(GetDlgItem(hDlg,IDC_EDCOMMAND),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_BTNADDCOMMAND),FALSE);
			return TRUE;
		// 매크로 입력 모드
		case IDC_RADIOCOMMAND:
			EnableWindow(GetDlgItem(hDlg,IDC_CBMENUCOM),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_EDCOMMAND),TRUE);
			EnableWindow(GetDlgItem(hDlg,IDC_BTNADDCOMMAND),TRUE);
			return TRUE;
		// 새 명령 추가
		case IDC_BTNADDCOMMAND:
			szMacro=(TCHAR *)malloc(256);
			if (DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_ADDCOMMAND),hDlg,
				AddCommandDlgProc,(LPARAM)szMacro)==IDOK) {
				GetDlgItemText(hDlg,IDC_EDCOMMAND,szCom,1024);
				if (lstrlen(szCom) && szCom[lstrlen(szCom)-1]!=';') {
					lstrcat(szCom,";");
				}
				lstrcat(szCom,szMacro);
				SetDlgItemText(hDlg,IDC_EDCOMMAND,szCom);
			}
			free(szMacro);
			return TRUE;
		case IDOK:
			// 일단 작성한 단축키 정보를 조사해 놓는다.
			vk=SendDlgItemMessage(hDlg,IDC_CBKEY,CB_GETCURSEL,0,0);
			if (vk==-1) {
				MessageBox(hDlg,"키를 선택해 주십시오","알림",MB_OK);
				return TRUE;
			}
			sc.vk=arVirtKey[vk].vk;
			sc.Flag=0;

			if (IsDlgButtonChecked(hDlg,IDC_CHKCTRL))
				sc.Flag |= SC_CTRL;
			if (IsDlgButtonChecked(hDlg,IDC_CHKALT))
				sc.Flag |= SC_ALT;
			if (IsDlgButtonChecked(hDlg,IDC_CHKSHIFT))
				sc.Flag |= SC_SHIFT;

			if (IsDlgButtonChecked(hDlg,IDC_RADIOID)) {
				id=SendDlgItemMessage(hDlg,IDC_CBMENUCOM,CB_GETCURSEL,0,0);
				if (id==-1) {
					MessageBox(hDlg,"메뉴 명령을 선택해 주십시오","알림",MB_OK);
					return TRUE;
				}
				sc.id=arMenuCom[id].id;
				sc.Com=NULL;
			} else {
				sc.id=0;
				GetDlgItemText(hDlg,IDC_EDCOMMAND,szCom,1024);
				StringNormalize(szCom,NULL);
				if (IsStrEmpty(szCom)) {
					MessageBox(hDlg,"명령을 작성해 주십시오","알림",MB_OK);
					return TRUE;
				}
				sc.Com=szCom;
			}

			// 추가시 이미 사용중인 단축키를 지정했거나 편집시 편집 대상과 다른 단축키를 지정했으면
			// 질문을 통해 기존 단축키를 삭제한다. 단 편집시 할당되지 않은 단축키를 지정했으면 질문 불필요
			oriidx=GetDlgShortCut(hDlg);
			if ((objidx == -1 && oriidx != -1) || (objidx != -1 && oriidx != objidx && oriidx!=-1)) {
				if (DgShortCut.arShortCut[oriidx].id != 0) {
					int tid=DgShortCut.arShortCut[oriidx].id;
					Com=arMenuCom[GetMenuComIndex(tid)].Name;
				} else {
					Com=DgShortCut.arShortCut[oriidx].Com;
				}
				wsprintf(Mes,"이 단축키는 이미 [%s] 명령으로 사용하고 있습니다. "
					"기존 단축키를 삭제하고 새로 만드시겠습니까?",Com);
				if (MessageBox(hDlg,Mes,"질문",MB_YESNO)==IDNO) {
					return TRUE;
				} else {
					DgShortCut.Delete(oriidx);
				}
			}

			// 편집했으면 갱신만 한다. 추가시는 삭제한다.
			if (objidx!=-1) {
				DgShortCut.Modify(objidx,&sc);
			} else {
				DgShortCut.Add(&sc);
			}
			EndDialog(hDlg,IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg,IDCANCEL);
			return TRUE;
		// 단축키 지정 컨트롤이 변경되면 스태틱에 이 키의 현재 상태를 출력한다.
		case IDC_CBKEY:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				DisplayShortCutStatus(hDlg);
			}
			return TRUE;
		case IDC_CHKCTRL:
		case IDC_CHKALT:
		case IDC_CHKSHIFT:
			DisplayShortCutStatus(hDlg);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

// 대화상자에 선택된 단축키의 인덱스를 찾아준다.
int GetDlgShortCut(HWND hDlg)
{
	DWORD Flag=0;
	UINT vk;

	if (IsDlgButtonChecked(hDlg,IDC_CHKCTRL))
		Flag |= SC_CTRL;
	if (IsDlgButtonChecked(hDlg,IDC_CHKALT))
		Flag |= SC_ALT;
	if (IsDlgButtonChecked(hDlg,IDC_CHKSHIFT))
		Flag |= SC_SHIFT;

	vk=arVirtKey[SendDlgItemMessage(hDlg,IDC_CBKEY,CB_GETCURSEL,0,0)].vk;
	return DgShortCut.Find(vk,Flag);
}

// idx번 단축키를 대화상자에 보여준다.
void SetDlgShortCut(HWND hDlg,int idx)
{
	int i;

	for (i=0;i<ARSIZE(arVirtKey);i++) {
		if (arVirtKey[i].vk == DgShortCut.arShortCut[idx].vk)
			break;
	}
	SendDlgItemMessage(hDlg,IDC_CBKEY,CB_SELECTSTRING,-1,
		(LPARAM)arVirtKey[i].Name);

	if (DgShortCut.arShortCut[idx].Flag & SC_CTRL) {
		CheckDlgButton(hDlg,IDC_CHKCTRL,BST_CHECKED);
	}
	if (DgShortCut.arShortCut[idx].Flag & SC_ALT) {
		CheckDlgButton(hDlg,IDC_CHKALT,BST_CHECKED);
	}
	if (DgShortCut.arShortCut[idx].Flag & SC_SHIFT) {
		CheckDlgButton(hDlg,IDC_CHKSHIFT,BST_CHECKED);
	}
}

// 대화상자에서 선택된 단축키를 스태틱에 출력한다.
void DisplayShortCutStatus(HWND hDlg)
{
	int idx;
	TCHAR *Name;
	TCHAR Status[256];

	idx=GetDlgShortCut(hDlg);
	if (idx == -1) {
		SetDlgItemText(hDlg,IDC_STSHORTCUT,"이 단축키의 현재 기능 : 할당되지 않았음");
	} else {
		if (DgShortCut.arShortCut[idx].id != 0) {
			Name=arMenuCom[GetMenuComIndex(DgShortCut.arShortCut[idx].id)].Name;
		} else {
			Name=DgShortCut.arShortCut[idx].Com;
		}
		wsprintf(Status,"이 단축키의 현재 기능 : %s",Name);
		SetDlgItemText(hDlg,IDC_STSHORTCUT,Status);
	}
}

// hMenu 메뉴 아래의 모든 항목 캡션을 다시 작성한다.
void UpdateMenuCaption(HMENU hMenu)
{
	int i,count;
	MENUITEMINFO mi;
	TCHAR Caption[256];

	count=GetMenuItemCount(hMenu);
	for (i=0;i<count;i++) {
		mi.cbSize=sizeof(MENUITEMINFO);
		mi.fMask=MIIM_ID | MIIM_TYPE | MIIM_SUBMENU;
		mi.dwTypeData=Caption;
		mi.cch=256;
		GetMenuItemInfo(hMenu,i,TRUE,&mi);
		if (mi.fType==MFT_SEPARATOR) {
			continue;
		}
		if (mi.hSubMenu!=NULL) {
			UpdateMenuCaption(mi.hSubMenu);
		} else {
			if (mi.fType == MFT_STRING) {
				UpdateItemCaption(hMenu,i,mi.wID,Caption);
			}
		}
	}
}

// 메뉴 항목뒤에 캡션을 새로 붙인다.
void UpdateItemCaption(HMENU hMenu,int idx,UINT ID,TCHAR *Caption)
{
	int sc,vkidx;
	TCHAR szName[256]={0,};
	TCHAR szShortCut[256]={0,};
	TCHAR NewShortCut[256];
	TCHAR NewCaption[256];
	TCHAR *s,*d;
	BOOL NewExist=FALSE, OldExist=FALSE;

	// 기존 캡션을 메뉴명과 단축키 문자열로 분리. 탭 뒤쪽이 단축키 문자열이다.
	for (s=Caption,d=szName;*s!=0 && *s!='\t';) {
		*d++=*s++;
	}
	if (*s=='\t') {
		OldExist=TRUE;
		for (d=szShortCut,s++;*s;) {
			*d++=*s++;
		}
	}

	// 메뉴 항목의 현재 단축키 문자열을 구한다.
	sc=DgShortCut.Find(ID,(TCHAR *)NULL);
	if (sc != -1) {
		NewExist=TRUE;
		lstrcpy(NewShortCut,"");
		if (DgShortCut.arShortCut[sc].Flag & SC_CTRL) {
			lstrcpy(NewShortCut,"Ctrl");
		}
		if (DgShortCut.arShortCut[sc].Flag & SC_ALT) {
			if (lstrlen(NewShortCut)) {
				lstrcat(NewShortCut,"+");
			}
			lstrcat(NewShortCut,"Alt");
		}
		if (DgShortCut.arShortCut[sc].Flag & SC_SHIFT) {
			if (lstrlen(NewShortCut)) {
				lstrcat(NewShortCut,"+");
			}
			lstrcat(NewShortCut,"Shift");
		}
		if (lstrlen(NewShortCut)) {
			lstrcat(NewShortCut,"+");
		}
		vkidx=GetVirtKeyIndex(DgShortCut.arShortCut[sc].vk);
		lstrcat(NewShortCut,arVirtKey[vkidx].Name);
	}

	// 단축키 수정
	if (OldExist) {
		if (NewExist) {
			// 단축키가 바뀐 경우
			if (strcmp(szShortCut,NewShortCut)!=0) {
				wsprintf(NewCaption,"%s\t%s",szName,NewShortCut);
				ModifyMenu(hMenu,idx,MF_BYPOSITION | MF_STRING,ID,NewCaption);
			}
		} else {
			// 원래 있었는데 없어진 경우-단축키 제거
			ModifyMenu(hMenu,idx,MF_BYPOSITION | MF_STRING,ID,szName);
		}
	} else {
		if (NewExist) {
			// 원래 없었는데 새로 생긴 경우
			wsprintf(NewCaption,"%s\t%s",szName,NewShortCut);
			ModifyMenu(hMenu,idx,MF_BYPOSITION | MF_STRING,ID,NewCaption);
		}
		// 원래 없었는데 지금도 없는 경우. 아무것도 할 필요없음
	}
}
