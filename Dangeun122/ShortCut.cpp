#include "stdafx.h"

// ����Ű ���
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

// �޴� �׸� ���
SMenuCom arMenuCom[]={
	{"����/�� ����",IDM_FILE_NEW},
	{"����/����",IDM_FILE_OPEN},
	{"����/����",IDM_FILE_SAVE},
	{"����/�� �̸�����",IDM_FILE_SAVEAS},
	{"����/�ݱ�",IDM_FILE_CLOSE},
	{"����/��� ����",IDM_FILE_SAVEALL},
	{"����/��� �ݱ�",IDM_FILE_CLOSEALL},
	{"����/FTP ����",IDM_FILE_FTPOPEN},
	{"����/HTTP ����",IDM_FILE_HTTPOPEN},
	{"����/���� ��ȯ/ANSI",IDM_FILE_ANSI},
	{"����/���� ��ȯ/�����ڵ�",IDM_FILE_UNI},
	{"����/���� ��ȯ/�����ڵ� ��",IDM_FILE_UNIBIG},
	{"����/���� ��ȯ/�����ڵ�(NB)",IDM_FILE_UNINOBOM},
	{"����/���� ��ȯ/�����ڵ� ��(NB)",IDM_FILE_UNIBIGNOBOM},
	{"����/���� ��ȯ/UTF-8",IDM_FILE_UTF8},
	{"����/���� ��ȯ/UTF-8(NB)",IDM_FILE_UTF8NOBOM},
	{"����/�б� ����",IDM_FILE_READONLY},
	{"����/���� ���� ���·� �ǵ�����",IDM_FILE_REVERT},
	{"����/�μ�",IDM_FILE_PRINT},
	{"����/���� ����",IDM_FILE_INFO},
	{"����/������Ʈ ����",IDM_FILE_PROJECT},
	{"����/�ֱ� ����1",45000},
	{"����/�ֱ� ����2",45001},
	{"����/�ֱ� ����3",45002},
	{"����/�ֱ� ����4",45003},
	{"����/�ֱ� ����5",45004},
	{"����/�ֱ� ����6",45005},
	{"����/�ֱ� ����7",45006},
	{"����/�ֱ� ����8",45007},
	{"����/�ֱ� ����9",45008},
	{"����/�ֱ� ����10",45009},
	{"����/����",IDM_FILE_EXIT},

	{"����/���",IDM_EDIT_UNDO},
	{"����/�����",IDM_EDIT_REDO},
	{"����/����",IDM_EDIT_COPY},
	{"����/�߶󳻱�",IDM_EDIT_CUT},
	{"����/�ٿ��ֱ�",IDM_EDIT_PASTE},
	{"����/��ü����",IDM_EDIT_SELALL},
	{"����/�����ؼ� �߰�",IDM_EDIT_COPYAPPEND},
	{"����/�߶� �߰�",IDM_EDIT_CUTAPPEND},
	{"����/��ȯ/�빮�ڷ�",IDM_EDIT_UPPER},
	{"����/��ȯ/�ҹ��ڷ�",IDM_EDIT_LOWER},
	{"����/��ȯ/��ҹ��� �ݴ��",IDM_EDIT_REVERSE},
	{"����/��ȯ/ù���ڸ� �빮�ڷ�",IDM_EDIT_UPPERFIRST},
	{"����/��ȯ/���� ó���� �빮�ڷ�",IDM_EDIT_UPPERSENT},
	{"����/��ȯ/������ ������",IDM_EDIT_TOTAB},
	{"����/��ȯ/���� ��������",IDM_EDIT_TOSPACE},
	{"����/��ȯ/���ʿ��� ���� ����",IDM_EDIT_REMOVESPACE},
	{"����/��ȯ/�鿩����",IDM_EDIT_INDENT},
	{"����/��ȯ/�����",IDM_EDIT_UNINDENT},
	{"����/��ȯ/�ּ�����",IDM_EDIT_COMMENT},
	{"����/��ȯ/�ּ�����",IDM_EDIT_UNCOMMENT},
	{"����/����/��¥",IDM_EDIT_DATE},
	{"����/����/�ð�",IDM_EDIT_TIME},
	{"����/����/���� ���� �ݺ�",IDM_EDIT_COPYUP},
	{"����/����/�Ʒ��� ���� �ݺ�",IDM_EDIT_COPYDOWN},
	{"����/����/����",IDM_EDIT_INSERTFILE},
	{"����/����/Ư������",IDM_EDIT_INSERTSPECIAL},
	{"����/���� ����",IDM_EDIT_VIRT},
	{"����/�� ���",IDM_EDIT_COLUMN},
	{"����/��� ���",IDM_EDIT_COMMODE},

	{"����/�ڵ� ����",IDM_VIEW_WORDWRAP},
	{"����/���� �ڵ� ����",IDM_VIEW_CODE},
	{"����/Ȱ��â�� �۲�/�ý���",IDM_FONT_1},
	{"����/Ȱ��â�� �۲�/���� 10",IDM_FONT_2},
	{"����/Ȱ��â�� �۲�/���� 12",IDM_FONT_3},
	{"����/Ȱ��â�� �۲�/����ü 10",IDM_FONT_4},
	{"����/Ȱ��â�� �۲�/����ü 12",IDM_FONT_5},
	{"����/Ȱ��â�� �۲�/�ü� 12",IDM_FONT_6},
	{"����/Ȱ��â�� �۲�/�ü� 15",IDM_FONT_7},
	{"����/Ȱ��â�� �۲�/�ü� 20",IDM_FONT_8},
	{"����/Ȱ��â�� �۲�/���� 12",IDM_FONT_9},
	{"����/Ȱ��â�� �۲�/���� 15",IDM_FONT_10},
	{"����/Ȱ��â�� �۲�/�۲� ����",IDM_FONT_11},
	{"����/Ȱ��â�� �ٰ�/100",IDM_LINE_1},
	{"����/Ȱ��â�� �ٰ�/120",IDM_LINE_2},
	{"����/Ȱ��â�� �ٰ�/150",IDM_LINE_3},
	{"����/Ȱ��â�� �ٰ�/200",IDM_LINE_4},
	{"����/Ȱ��â�� �ٰ�/300",IDM_LINE_5},
	{"����/���� ����/����",IDM_SYN_NONE},
	{"����/���� ����/C/C++C",IDM_SYN_CPP},
	{"����/���� ����/HTML",IDM_SYN_HTML},
	{"����/���� ����/SQL",IDM_SYN_SQL},
	{"����/���� ����/SQL",IDM_SYN_SQL},
	{"����/���� ����/Java",IDM_SYN_JAVA},
	{"����/16�� ����",IDM_VIEW_HEXA},
	{"����/����",IDM_VIEW_TOOLBAR},
	{"����/���¶�",IDM_VIEW_STATUS},
	{"����/����â",IDM_VIEW_FILE},
	{"����/������",IDM_VIEW_FILETAB},
	{"����/�˻� ���",IDM_VIEW_OUTPUT},
	{"����/��뱸 â",IDM_VIEW_CLIP},

	{"�˻�/ã��",IDM_SEARCH_FIND},
	{"�˻�/���� ã��",IDM_SEARCH_NEXT},
	{"�˻�/���� ã��",IDM_SEARCH_PREV},
	{"�˻�/���� �ܾ� ã��",IDM_SEARCH_WORD},
	{"�˻�/���� �ܾ� ����",IDM_SEARCH_WORDUP},
	{"�˻�/�ٲٱ�",IDM_SEARCH_REPLACE},
	{"�˻�/å���� ����/����",IDM_SEARCH_TOGGLEMARK},
	{"�˻�/���� å���Ƿ�",IDM_SEARCH_NEXTMARK},
	{"�˻�/���� å���Ƿ�",IDM_SEARCH_PREVMARK},
	{"�˻�/å���� ��� ����",IDM_SEARCH_CLEARMARK},
	{"�˻�/�� ã��",IDM_SEARCH_GOTOLINE},
	{"�˻�/��ȣ ¦ ã��",IDM_SEARCH_PAREN},
	{"�˻�/��ȣ ¦ ����",IDM_SEARCH_PARENSEL},
	{"�˻�/������ ���� ��ġ",IDM_SEARCH_LASTEDIT},
	{"�˻�/���Ͽ��� ã��",IDM_SEARCH_FILES},
	{"�˻�/���Ͽ��� �ٲٱ�",IDM_SEARCH_RFILES},
	{"�˻�/�ٷ� ã��",IDM_SEARCH_INST},

	{"����/�⺻ ����",IDM_TOOL_CONFIG},
	{"����/��ġ ���� ����",IDM_TOOL_UNINSTALL},
	{"����/��ũ��",IDM_TOOL_MACRO},
	{"����/����Ű ����",IDM_TOOL_SHORTCUT},
	{"����/��뱸 ����",IDM_TOOL_CLIP},
	{"����/�б� ���",IDM_TOOL_READMODE},
	{"����/���� �籸��",IDM_TOOL_PARA},

	{"â/��ܽ� ����",IDM_WIN_CASCADE},
	{"â/���� ����",IDM_WIN_HORZ},
	{"â/���� ����",IDM_WIN_VERT},
	{"â/������ ����",IDM_WIN_ARRANGE},
	{"â/���� ��",IDM_WIN_NEXTVIEW},
	{"â/���� ��",IDM_WIN_PREVVIEW},

	{"����/���α׷� �Ұ�",IDM_HELP_ABOUT},
	{"����/����",IDM_HELP_INDEX},
	{"����/Ȩ������ �湮",IDM_HELP_HOME}
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

	// ����Ű �迭�� ��ũ�� ���ڿ��� �����Ѵ�.
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

// ����Ʈ ����Ű ���
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

	// �迭�� �� �ε��� ã��
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
		// �迭 ũ�� ����
		size=_msize(arShortCut)/sizeof(arShortCut[0]);
		if (last == size-1) {
			size+=10;
			arShortCut=(SShortCut *)realloc(arShortCut,size*sizeof(SShortCut));
			memset(arShortCut+size-10,0,10*sizeof(SShortCut));
		}
	}

	// ��ũ�� ����̸� ���� �Ҵ��� �� ��ũ�� ���ڿ��� �����Ѵ�.
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

	// ��ũ�� ���ڿ� ����
	if (arShortCut[idx].Com) {
		free(arShortCut[idx].Com);
	}

	// ���� ����Ű�� ��ĭ�� ������ �ű��.
	for (i=idx+1;;i++) {
		arShortCut[i-1]=arShortCut[i];
		if (arShortCut[i].vk==0) {
			break;
		}
	}
}

void CShortCut::Modify(int idx,SShortCut *sc)
{
	// ���� ����Ű�� �Ҵ�� ����� ������ �ϴ� ����
	if (arShortCut[idx].Com) {
		free(arShortCut[idx].Com);
		arShortCut[idx].Com=NULL;
	}
	arShortCut[idx]=*sc;
	// ��ũ�� ����̸� �޸𸮸� �ٽ� �Ҵ��� �� �����Ѵ�.
	if (sc->id == 0) {
		arShortCut[idx].Com=(TCHAR *)malloc(lstrlen(sc->Com)+1);
		lstrcpy(arShortCut[idx].Com,sc->Com);
	} 
}

// ���� Ű�� �÷��װ� ���� ����Ű ��ȣ ã��
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

// ��� �Ǵ� ��ũ�� ����� ���� ����Ű ��ȣ ã��
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
		COL.pszText="Ű";
		COL.iSubItem=0;
		ListView_InsertColumn(hList,0,&COL);

		COL.cx=80;
		COL.pszText="����Ű";
		COL.iSubItem=1;
		ListView_InsertColumn(hList,1,&COL);

		COL.cx=60;
		COL.pszText="����";
		COL.iSubItem=2;
		ListView_InsertColumn(hList,2,&COL);

		COL.cx=200;
		COL.pszText="���";
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
			if (MessageBox(hDlg,"��� ����Ű ������ ��ġ �ʱ� ���·� �ٲٽðڽ��ϱ�?",
				"����",MB_YESNO)==IDYES) {
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

// ���� ����Ű ���� ���� ���. Ű�� �����Ѵ�.
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

		// ������ ��ġ ����
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
			ListView_SetItemText(hList,idx,2,"�޴�");
			j=GetMenuComIndex(DgShortCut.arShortCut[i].id);
			ListView_SetItemText(hList,idx,3,arMenuCom[j].Name);
		} else {
			ListView_SetItemText(hList,idx,2,"��ũ��");
			ListView_SetItemText(hList,idx,3,DgShortCut.arShortCut[i].Com);
		}
	}
}

// ����Ű �߰� �� ���� ��ȭ����. �Ķ���ʹ� ���� ��� ����Ű�̵� -1�̸� �߰�
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
		// �����̸� ������ ����Ű�� �����ش�.
		if (objidx != -1) {
			SetDlgShortCut(hDlg,objidx);

			// ���� ��� ����Ű�� ������ �����ش�.
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
		// �߰��̸� �޴� ��� ���� ���·� ����� �ش�. CtrlŰ�� �⺻ ������ �ش�.
		} else {
			CheckDlgButton(hDlg,IDC_CHKCTRL,BST_CHECKED);
			CheckRadioButton(hDlg,IDC_RADIOID,IDC_RADIOCOMMAND,IDC_RADIOID);
			SendMessage(hDlg,WM_COMMAND,MAKEWPARAM(IDC_RADIOID,0),0);
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		// �޴� ��� ���� ���
		case IDC_RADIOID:
			EnableWindow(GetDlgItem(hDlg,IDC_CBMENUCOM),TRUE);
			EnableWindow(GetDlgItem(hDlg,IDC_EDCOMMAND),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_BTNADDCOMMAND),FALSE);
			return TRUE;
		// ��ũ�� �Է� ���
		case IDC_RADIOCOMMAND:
			EnableWindow(GetDlgItem(hDlg,IDC_CBMENUCOM),FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_EDCOMMAND),TRUE);
			EnableWindow(GetDlgItem(hDlg,IDC_BTNADDCOMMAND),TRUE);
			return TRUE;
		// �� ��� �߰�
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
			// �ϴ� �ۼ��� ����Ű ������ ������ ���´�.
			vk=SendDlgItemMessage(hDlg,IDC_CBKEY,CB_GETCURSEL,0,0);
			if (vk==-1) {
				MessageBox(hDlg,"Ű�� ������ �ֽʽÿ�","�˸�",MB_OK);
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
					MessageBox(hDlg,"�޴� ����� ������ �ֽʽÿ�","�˸�",MB_OK);
					return TRUE;
				}
				sc.id=arMenuCom[id].id;
				sc.Com=NULL;
			} else {
				sc.id=0;
				GetDlgItemText(hDlg,IDC_EDCOMMAND,szCom,1024);
				StringNormalize(szCom,NULL);
				if (IsStrEmpty(szCom)) {
					MessageBox(hDlg,"����� �ۼ��� �ֽʽÿ�","�˸�",MB_OK);
					return TRUE;
				}
				sc.Com=szCom;
			}

			// �߰��� �̹� ������� ����Ű�� �����߰ų� ������ ���� ���� �ٸ� ����Ű�� ����������
			// ������ ���� ���� ����Ű�� �����Ѵ�. �� ������ �Ҵ���� ���� ����Ű�� ���������� ���� ���ʿ�
			oriidx=GetDlgShortCut(hDlg);
			if ((objidx == -1 && oriidx != -1) || (objidx != -1 && oriidx != objidx && oriidx!=-1)) {
				if (DgShortCut.arShortCut[oriidx].id != 0) {
					int tid=DgShortCut.arShortCut[oriidx].id;
					Com=arMenuCom[GetMenuComIndex(tid)].Name;
				} else {
					Com=DgShortCut.arShortCut[oriidx].Com;
				}
				wsprintf(Mes,"�� ����Ű�� �̹� [%s] ������� ����ϰ� �ֽ��ϴ�. "
					"���� ����Ű�� �����ϰ� ���� ����ðڽ��ϱ�?",Com);
				if (MessageBox(hDlg,Mes,"����",MB_YESNO)==IDNO) {
					return TRUE;
				} else {
					DgShortCut.Delete(oriidx);
				}
			}

			// ���������� ���Ÿ� �Ѵ�. �߰��ô� �����Ѵ�.
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
		// ����Ű ���� ��Ʈ���� ����Ǹ� ����ƽ�� �� Ű�� ���� ���¸� ����Ѵ�.
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

// ��ȭ���ڿ� ���õ� ����Ű�� �ε����� ã���ش�.
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

// idx�� ����Ű�� ��ȭ���ڿ� �����ش�.
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

// ��ȭ���ڿ��� ���õ� ����Ű�� ����ƽ�� ����Ѵ�.
void DisplayShortCutStatus(HWND hDlg)
{
	int idx;
	TCHAR *Name;
	TCHAR Status[256];

	idx=GetDlgShortCut(hDlg);
	if (idx == -1) {
		SetDlgItemText(hDlg,IDC_STSHORTCUT,"�� ����Ű�� ���� ��� : �Ҵ���� �ʾ���");
	} else {
		if (DgShortCut.arShortCut[idx].id != 0) {
			Name=arMenuCom[GetMenuComIndex(DgShortCut.arShortCut[idx].id)].Name;
		} else {
			Name=DgShortCut.arShortCut[idx].Com;
		}
		wsprintf(Status,"�� ����Ű�� ���� ��� : %s",Name);
		SetDlgItemText(hDlg,IDC_STSHORTCUT,Status);
	}
}

// hMenu �޴� �Ʒ��� ��� �׸� ĸ���� �ٽ� �ۼ��Ѵ�.
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

// �޴� �׸�ڿ� ĸ���� ���� ���δ�.
void UpdateItemCaption(HMENU hMenu,int idx,UINT ID,TCHAR *Caption)
{
	int sc,vkidx;
	TCHAR szName[256]={0,};
	TCHAR szShortCut[256]={0,};
	TCHAR NewShortCut[256];
	TCHAR NewCaption[256];
	TCHAR *s,*d;
	BOOL NewExist=FALSE, OldExist=FALSE;

	// ���� ĸ���� �޴���� ����Ű ���ڿ��� �и�. �� ������ ����Ű ���ڿ��̴�.
	for (s=Caption,d=szName;*s!=0 && *s!='\t';) {
		*d++=*s++;
	}
	if (*s=='\t') {
		OldExist=TRUE;
		for (d=szShortCut,s++;*s;) {
			*d++=*s++;
		}
	}

	// �޴� �׸��� ���� ����Ű ���ڿ��� ���Ѵ�.
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

	// ����Ű ����
	if (OldExist) {
		if (NewExist) {
			// ����Ű�� �ٲ� ���
			if (strcmp(szShortCut,NewShortCut)!=0) {
				wsprintf(NewCaption,"%s\t%s",szName,NewShortCut);
				ModifyMenu(hMenu,idx,MF_BYPOSITION | MF_STRING,ID,NewCaption);
			}
		} else {
			// ���� �־��µ� ������ ���-����Ű ����
			ModifyMenu(hMenu,idx,MF_BYPOSITION | MF_STRING,ID,szName);
		}
	} else {
		if (NewExist) {
			// ���� �����µ� ���� ���� ���
			wsprintf(NewCaption,"%s\t%s",szName,NewShortCut);
			ModifyMenu(hMenu,idx,MF_BYPOSITION | MF_STRING,ID,NewCaption);
		}
		// ���� �����µ� ���ݵ� ���� ���. �ƹ��͵� �� �ʿ����
	}
}
