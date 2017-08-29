#include "stdafx.h"

SOption::SOption()
{
	int i;
	for (i=0;i<sizeof(arFtp)/sizeof(arFtp[0]);i++) {
		lstrcpy(arFtp[i].Name,"");
	}
	Init();
	bModified=FALSE;
} 

SStMask StMaskInit[STNUM]={
	{true,0,100,false,0},
	{true,1,120,false,0},
	{true,2,80,false,0},
	{true,3,80,false,0},
	{true,4,80,false,0},
	{false,5,50,false,0},
	{false,6,40,false,0},
	{false,7,80,false,0},
	{false,8,80,false,0},
	{false,9,120,false,0},
	{false,10,65,false,0},
	{false,11,70,false,0},
	{false,12,90,false,0}
};

void SOption::Init()
{
	int i;
	
	Style=AES_ALLOWDRAG | AES_AUTOINDENT | AES_BLOCKINDENTWITHTAB | AES_HOMETOFIRSTCHAR |
		AES_HIDECURLINE | AES_CALCTABWITHAVG | AES_HORZSPLITABLE | AES_VERTSPLITABLE |
		AES_AUTOCONVERT | AES_USEAUTOCLIP | AES_AUTOCLIPBYTAB | AES_SHOWMARGIN;
	StartAction=2;
	bExplorerPopup=TRUE;
	bAllowMulti=FALSE;
	bMaxFirstChild=TRUE;
	MaxMru=3;
	lstrcpy(DefExt,"txt");
	Hangul=FALSE;
	OutputHeight=100;
	bInitFind=FALSE;
	bInitNextFind=FALSE;
	bShowToolBar=TRUE;
	bShowStatus=TRUE;
	bShowFileWnd=TRUE;
	FileWndWidth=200;
	bShowFileTab=TRUE;
	bShowHidden=FALSE;
	FilterIndex=0;
	bToolBarBig=FALSE;
	bToolBarText=FALSE;
	bBrowseMode=FALSE;
	bSoundFindFail=TRUE;
	bWatchChange=TRUE;
	bReloadNoAsk=FALSE;
	bReloadProject=TRUE;

	LineRatio=120;
	bWrap=TRUE;
	nWrap=2;
	RightWrap=0;
	ColMark=0;
	HideSelType=1;
	nShowCurLine=0;
	memset(&logfont,0,sizeof(LOGFONT));
	lstrcpy(logfont.lfFaceName,"굴림");
	logfont.lfHeight=10;
	logfont.lfCharSet=HANGEUL_CHARSET;
	logfont.lfPitchAndFamily=VARIABLE_PITCH | FF_MODERN;
	TabWidth=4;
	ShowTabType=1;
	ShowEnterType=1;
	ShowSpaceType=0;
	CaretWidth=2;
	UndoLimit=2;
	FindDlgPos=1;

	lstrcpy(arExt[0],"");
	lstrcpy(arExt[1],"c;cpp;h;hpp");
	lstrcpy(arExt[2],"html;htm;asp;php");
	lstrcpy(arExt[3],"sql");
	lstrcpy(arExt[4],"java");
	lstrcpy(arExt[5],"cs");
	arStyle[0][0].fore=-1;

	prtFont.lfHeight=-1;
	SetRect(&prtMargin,15,25,15,25);
	prtbParse=TRUE;
	prtnWrap=2;
	prtbLineNum=FALSE;
	prtbSysHeader=TRUE;
	prtbDirect=FALSE;
	lstrcpy(prtHeader,"");
	lstrcpy(prtFooter,"[파일][가운데][쪽]/[전체쪽][오른쪽]당근으로 인쇄");
	prtnHeader=0;
	prtnFooter=4;
	lstrcpy(prtBkText,"");

	bpl=2;
	bCapital=TRUE;
	nHexSep=1;
	BackupLevel=0;
	lstrcpy(BackupFolder,"");
	BackupNaming=1;
	AutoSaveInterval=0;
	bMakeEditInfo=FALSE;
	bUpdateEditInfo=TRUE;
	lstrcpy(EditInfoFolder,"");
	EditInfoMask=0x1f;
	bUseEditInfo=FALSE;
	FixedFont.lfHeight=0;
	bShowClipWnd=TRUE;
	ClipWndRatio=5000;
	bReloadClip=TRUE;
	bPreviewClip=FALSE;
	SetRect(&InnerRect,1,2,0,0);
	VAlign=1;
	SelBlockType=1;
	BackBit[0]=0;
	BackBitPos=0;
	RmWrap=TRUE;
	memset(&Rmlogfont,0,sizeof(LOGFONT));
	Rmlogfont.lfHeight=20;
	lstrcpy(Rmlogfont.lfFaceName,"굴림");
	Rmlogfont.lfPitchAndFamily=VARIABLE_PITCH | FF_ROMAN;
	Rmlogfont.lfCharSet=HANGEUL_CHARSET;
	RmLineRatio=200;
	RmFreq=35;
	RmLineNum=1;
	RmManual=1;
	for (i=0;i<STNUM;i++) {
		StMask[i]=StMaskInit[i];
	}
	nWheel=1;
	nCtrlWheel=6;
	nShiftWheel=5;
	nCtrlShiftWheel=7;
	CharExtra=100;
	WordExtra=100;
	CharRatio=100;
	CursorEdit=0;
	for (i=0;i<AECOLORS;i++) {
		Color[i]=0;
	}
}

int SOption::GetMaxMru()
{
	int arMaxMru[]={5,10,15,20,25,30};
	return arMaxMru[MaxMru];
}

int SOption::GetUndoLimit()
{
	int arUndoLimit[]={1024*10,1024*100,1048576,1048576*5,1048576*10};
	return arUndoLimit[UndoLimit];
}

void SOption::GetStyleColor()
{
	CParse *Parser;
	int id,i;

	for (id=1;id<PARSENUM;id++) {
		switch(id) {
		case 1:
			Parser=new CParseCpp;
			break;
		case 2:
			Parser=new CParseHtml;
			break;
		case 3:
			Parser=new CParseSql;
			break;
		case 4:
			Parser=new CParseJava;
			break;
		case 5:
			Parser=new CParseCS;
			break;
		}
		for (i=0;i<32;i++) {
			Parser->GetStyleName(i,arStyle[id][i].name);
			Parser->GetStyleColor(i,arStyle[id][i].fore,arStyle[id][i].back);
			if (arStyle[id][i].fore==-2)
				break;
		}

		delete Parser;
	}
}

void SOption::SetStyleColor(CParse *Parser)
{
	int i;
	int id;

	id=int(Parser->GetInfo(0));

	for (i=0;i<32;i++) {
		Parser->SetStyleColor(i,arStyle[id][i].fore,arStyle[id][i].back);
		if (arStyle[id][i].fore==-2)
			break;
	}
}

int SOption::GetBpl()
{
	static int arBpl[]={4,8,16,24,32,48,64};
	return arBpl[bpl];
}

int SOption::GetHexSep()
{
	static int arHexSep[]={2,4,8,16};
	return arHexSep[nHexSep];
}

int SOption::GetStNumByOrder(int Order)
{
	int i;
	for (i=0;i<STNUM;i++) {
		if (StMask[i].Order==Order) {
			return i;
		}
	}
	return -1;
}

void SOption::Load(TCHAR *Key)
{
	TCHAR szKey[MAX_PATH];
	TCHAR szSub[MAX_PATH];
	int i,j;
	TCHAR EncText[400];

	lstrcpy(szKey,Key);
	lstrcat(szKey,"Setting");

	Style=SHRegReadInt(SHCU,szKey,"Style",0);
	StartAction=SHRegReadInt(SHCU,szKey,"StartAction",0);
	bExplorerPopup=SHRegReadInt(SHCU,szKey,"bExplorerPopup",0);
	bMaxFirstChild=SHRegReadInt(SHCU,szKey,"bMaxFirstChild",0);
	MaxMru=SHRegReadInt(SHCU,szKey,"MaxMru",0);
	SHRegReadString(SHCU,szKey,"DefExt","",DefExt,12);
	Hangul=SHRegReadInt(SHCU,szKey,"Hangul",0);
	bInitFind=SHRegReadInt(SHCU,szKey,"InitFind",0);
	bInitNextFind=SHRegReadInt(SHCU,szKey,"InitNextFind",0);
	OutputHeight=SHRegReadInt(SHCU,szKey,"OutputHeight",0);
	bShowToolBar=SHRegReadInt(SHCU,szKey,"bShowToolBar",0);
	bShowStatus=SHRegReadInt(SHCU,szKey,"bShowStatus",0);
	bShowFileWnd=SHRegReadInt(SHCU,szKey,"bShowFileWnd",0);
	bShowFileTab=SHRegReadInt(SHCU,szKey,"bShowFileTab",0);
	FileWndWidth=SHRegReadInt(SHCU,szKey,"FileWndWidth",0);
	bShowHidden=SHRegReadInt(SHCU,szKey,"bShowHidden",0);
	FilterIndex=SHRegReadInt(SHCU,szKey,"FilterIndex",0);	
	bToolBarBig=SHRegReadInt(SHCU,szKey,"bToolBarBig",0);
	bToolBarText=SHRegReadInt(SHCU,szKey,"bToolBarText",0);
	bBrowseMode=SHRegReadInt(SHCU,szKey,"bBrowseMode",0);
	bSoundFindFail=SHRegReadInt(SHCU,szKey,"bSoundFindFail",0);
	bWatchChange=SHRegReadInt(SHCU,szKey,"bWatchChange",0);
	bReloadNoAsk=SHRegReadInt(SHCU,szKey,"bReloadNoAsk",0);
	bReloadProject=SHRegReadInt(SHCU,szKey,"bReloadProject",0);

	LineRatio=SHRegReadInt(SHCU,szKey,"LineRatio",0);
	bWrap=SHRegReadInt(SHCU,szKey,"bWrap",0);
	nWrap=SHRegReadInt(SHCU,szKey,"nWrap",0);
	HideSelType=SHRegReadInt(SHCU,szKey,"HideSelType",0);
	nShowCurLine=SHRegReadInt(SHCU,szKey,"nShowCurLine",0);
	TabWidth=SHRegReadInt(SHCU,szKey,"TabWidth",0);
	RightWrap=SHRegReadInt(SHCU,szKey,"RightWrap",0);
	ColMark=SHRegReadInt(SHCU,szKey,"ColMark",0);	
	ShowTabType=SHRegReadInt(SHCU,szKey,"ShowTabType",0);
	ShowEnterType=SHRegReadInt(SHCU,szKey,"ShowEnterType",0);
	ShowSpaceType=SHRegReadInt(SHCU,szKey,"ShowSpaceType",0);
	CaretWidth=SHRegReadInt(SHCU,szKey,"CaretWidth",0);
	UndoLimit=SHRegReadInt(SHCU,szKey,"UndoLimit",0);
	SHRegReadString(SHCU,szKey,"FaceName","",logfont.lfFaceName,32);
	logfont.lfPitchAndFamily=SHRegReadInt(SHCU,szKey,"PitchAndFamily",0);
	logfont.lfCharSet=SHRegReadInt(SHCU,szKey,"CharSet",0);
	logfont.lfHeight=SHRegReadInt(SHCU,szKey,"FontHeight",0);
	logfont.lfWeight=SHRegReadInt(SHCU,szKey,"FontWeight",0);
	logfont.lfItalic=SHRegReadInt(SHCU,szKey,"FontItalic",0);
	FindDlgPos=SHRegReadInt(SHCU,szKey,"FindDlgPos",0);

	SHRegReadString(SHCU,szKey,"PrtFaceName","",prtFont.lfFaceName,32);
	prtFont.lfPitchAndFamily=SHRegReadInt(SHCU,szKey,"PrtPitchAndFamily",0);
	prtFont.lfCharSet=SHRegReadInt(SHCU,szKey,"PrtCharSet",0);
	prtFont.lfHeight=SHRegReadInt(SHCU,szKey,"PrtFontHeight",0);
	prtMargin.left=SHRegReadInt(SHCU,szKey,"PrtMarginLeft",0);
	prtMargin.top=SHRegReadInt(SHCU,szKey,"PrtMarginTop",0);
	prtMargin.right=SHRegReadInt(SHCU,szKey,"PrtMarginRight",0);
	prtMargin.bottom=SHRegReadInt(SHCU,szKey,"PrtMarginBottom",0);
	prtbParse=SHRegReadInt(SHCU,szKey,"prtbParse",0);
	prtnWrap=SHRegReadInt(SHCU,szKey,"prtnWrap",0);
	prtbLineNum=SHRegReadInt(SHCU,szKey,"prtbLineNum",0);
	prtbSysHeader=SHRegReadInt(SHCU,szKey,"prtbSysHeader",0);
	prtbDirect=SHRegReadInt(SHCU,szKey,"prtbDirect",0);
	prtbParse=SHRegReadInt(SHCU,szKey,"prtbParse",0);
	prtnHeader=SHRegReadInt(SHCU,szKey,"prtnHeader",0);
	prtnFooter=SHRegReadInt(SHCU,szKey,"prtnFooter",0);
	SHRegReadString(SHCU,szKey,"prtHeader","",prtHeader,256);
	SHRegReadString(SHCU,szKey,"prtFooter","",prtFooter,256);
	SHRegReadString(SHCU,szKey,"prtBkText","",prtBkText,64);

	// 1.2에서 추가된 옵션들
	bpl=SHRegReadInt(SHCU,szKey,"bpl",0);
	bCapital=SHRegReadInt(SHCU,szKey,"bCapital",0);
	nHexSep=SHRegReadInt(SHCU,szKey,"nHexSep",1);
	BackupLevel=SHRegReadInt(SHCU,szKey,"BackupLevel",0);
	SHRegReadString(SHCU,szKey,"BackupFolder","",BackupFolder,256);
	BackupNaming=SHRegReadInt(SHCU,szKey,"BackupNaming",0);
	AutoSaveInterval=SHRegReadInt(SHCU,szKey,"AutoSaveInterval",0);
	bMakeEditInfo=SHRegReadInt(SHCU,szKey,"bMakeEditInfo",0);
	bUpdateEditInfo=SHRegReadInt(SHCU,szKey,"bUpdateEditInfo",0);
	SHRegReadString(SHCU,szKey,"EditInfoFolder","",EditInfoFolder,256);
	EditInfoMask=SHRegReadInt(SHCU,szKey,"EditInfoMask",0);
	bUseEditInfo=SHRegReadInt(SHCU,szKey,"bUseEditInfo",0);
	SHRegReadString(SHCU,szKey,"FixedFaceName","",FixedFont.lfFaceName,32);
	FixedFont.lfPitchAndFamily=SHRegReadInt(SHCU,szKey,"FixedPitchAndFamily",0);
	FixedFont.lfCharSet=SHRegReadInt(SHCU,szKey,"FixedCharSet",0);
	FixedFont.lfHeight=SHRegReadInt(SHCU,szKey,"FixedFontHeight",0);
	bShowClipWnd=SHRegReadInt(SHCU,szKey,"bShowClipWnd",0);
	ClipWndRatio=SHRegReadInt(SHCU,szKey,"ClipWndRatio",0);
	bReloadClip=SHRegReadInt(SHCU,szKey,"bReloadClip",0);
	bPreviewClip=SHRegReadInt(SHCU,szKey,"bPreviewClip",0);
	InnerRect.left=SHRegReadInt(SHCU,szKey,"InnerRectLeft",0);
	InnerRect.top=SHRegReadInt(SHCU,szKey,"InnerRectTop",0);
	InnerRect.right=SHRegReadInt(SHCU,szKey,"InnerRectRight",0);
	InnerRect.bottom=SHRegReadInt(SHCU,szKey,"InnerRectBottom",0);
	VAlign=SHRegReadInt(SHCU,szKey,"VAlign",0);
	SelBlockType=SHRegReadInt(SHCU,szKey,"SelBlockType",0);
	SHRegReadString(SHCU,szKey,"BackBit","",BackBit,MAX_PATH);
	BackBitPos=SHRegReadInt(SHCU,szKey,"BackBitPos",0);
	RmWrap=SHRegReadInt(SHCU,szKey,"RmWrap",0);
	SHRegReadString(SHCU,szKey,"RmFaceName","",Rmlogfont.lfFaceName,32);
	Rmlogfont.lfPitchAndFamily=SHRegReadInt(SHCU,szKey,"RmPitchAndFamily",0);
	Rmlogfont.lfCharSet=SHRegReadInt(SHCU,szKey,"RmCharSet",0);
	Rmlogfont.lfHeight=SHRegReadInt(SHCU,szKey,"RmFontHeight",0);
	RmLineRatio=SHRegReadInt(SHCU,szKey,"RmLineRatio",0);
	RmFreq=SHRegReadInt(SHCU,szKey,"RmFreq",0);
	RmLineNum=SHRegReadInt(SHCU,szKey,"RmLineNum",0);
	RmManual=SHRegReadInt(SHCU,szKey,"RmManual",0);
	nWheel=SHRegReadInt(SHCU,szKey,"nWheel",0);
	nCtrlWheel=SHRegReadInt(SHCU,szKey,"nCtrlWheel",0);
	nShiftWheel=SHRegReadInt(SHCU,szKey,"nShiftWheel",0);
	nCtrlShiftWheel=SHRegReadInt(SHCU,szKey,"nCtrlShiftWheel",0);
	CharExtra=SHRegReadInt(SHCU,szKey,"CharExtra",0);
	WordExtra=SHRegReadInt(SHCU,szKey,"WordExtra",0);
	CharRatio=SHRegReadInt(SHCU,szKey,"CharRatio",0);
	CursorEdit=SHRegReadInt(SHCU,szKey,"CursorEdit",0);

	lstrcpy(szKey,Key);
	lstrcat(szKey,"Style");
	for (i=1;i<PARSENUM;i++) {
		wsprintf(szSub,"%d-Ext",i);
		SHRegReadString(SHCU,szKey,szSub,"",arExt[i],256);
		for (j=0;j<32;j++) {
			wsprintf(szSub,"%d-%d-fore",i,j);
			arStyle[i][j].fore=SHRegReadInt(SHCU,szKey,szSub,-2);
			if (arStyle[i][j].fore == -2) {
				break;
			}
			wsprintf(szSub,"%d-%d-name",i,j);
			SHRegReadString(SHCU,szKey,szSub,"",arStyle[i][j].name,32);
			wsprintf(szSub,"%d-%d-back",i,j);
			arStyle[i][j].back=SHRegReadInt(SHCU,szKey,szSub,0);
		}
	}

	lstrcpy(szKey,Key);
	lstrcat(szKey,"Ftp");
	j=SHRegReadInt(SHCU,szKey,"Num",0);
	for (i=0;i<j;i++) {
		wsprintf(szSub,"Name-%d",i);
		SHRegReadString(SHCU,szKey,szSub,"",EncText,32);
		DecodeText(EncText,arFtp[i].Name);
		wsprintf(szSub,"Addr-%d",i);
		SHRegReadString(SHCU,szKey,szSub,"",EncText,256);
		DecodeText(EncText,arFtp[i].Addr);
		wsprintf(szSub,"User-%d",i);
		SHRegReadString(SHCU,szKey,szSub,"",EncText,32);
		DecodeText(EncText,arFtp[i].User);
		wsprintf(szSub,"Pass-%d",i);
		SHRegReadString(SHCU,szKey,szSub,"",EncText,32);
		DecodeText(EncText,arFtp[i].Pass);
		wsprintf(szSub,"Port-%d",i);
		arFtp[i].Port=SHRegReadInt(SHCU,szKey,szSub,0);
	}

	lstrcpy(szKey,Key);
	lstrcat(szKey,"Status");
	for (i=0;i<STNUM;i++) {
		wsprintf(szSub,"Show%d",i);
		StMask[i].bShow=SHRegReadInt(SHCU,szKey,szSub,0)==1 ? true:false;
		wsprintf(szSub,"Order%d",i);
		StMask[i].Order=SHRegReadInt(SHCU,szKey,szSub,0);
		wsprintf(szSub,"Width%d",i);
		StMask[i].Width=SHRegReadInt(SHCU,szKey,szSub,0);
		wsprintf(szSub,"Pop%d",i);
		StMask[i].bPop=SHRegReadInt(SHCU,szKey,szSub,0)==1 ? true:false;
		wsprintf(szSub,"Align%d",i);
		StMask[i].Align=SHRegReadInt(SHCU,szKey,szSub,0);
	}

	// 색상 옵션 읽음
	lstrcpy(szKey,Key);
	lstrcat(szKey,"Color");
	for (i=0;i<AECOLORS;i++) {
		wsprintf(szSub,"Color%d",i);
		Color[i]=SHRegReadInt(SHCU,szKey,szSub,0);
	}
}

void SOption::Save(TCHAR *Key)
{
	TCHAR szKey[MAX_PATH];
	TCHAR szSub[MAX_PATH];
	int i,j;
	TCHAR EncText[400];

	lstrcpy(szKey,Key);
	lstrcat(szKey,"Setting");

	SHRegWriteInt(SHCU,szKey,"Style",Style);
	SHRegWriteInt(SHCU,szKey,"StartAction",StartAction);
	SHRegWriteInt(SHCU,szKey,"bExplorerPopup",bExplorerPopup);
	SHRegWriteInt(SHCU,szKey,"bMaxFirstChild",bMaxFirstChild);
	SHRegWriteInt(SHCU,szKey,"MaxMru",MaxMru);
	SHRegWriteString(SHCU,szKey,"DefExt",DefExt);
	SHRegWriteInt(SHCU,szKey,"Hangul",Hangul);
	SHRegWriteInt(SHCU,szKey,"InitFind",bInitFind);
	SHRegWriteInt(SHCU,szKey,"InitNextFind",bInitNextFind);
	SHRegWriteInt(SHCU,szKey,"OutputHeight",OutputHeight);
	SHRegWriteInt(SHCU,szKey,"bShowToolBar",bShowToolBar);
	SHRegWriteInt(SHCU,szKey,"bShowStatus",bShowStatus);
	SHRegWriteInt(SHCU,szKey,"bShowFileWnd",bShowFileWnd);
	SHRegWriteInt(SHCU,szKey,"bShowFileTab",bShowFileTab);
	SHRegWriteInt(SHCU,szKey,"FileWndWidth",FileWndWidth);
	SHRegWriteInt(SHCU,szKey,"bShowHidden",bShowHidden);
	SHRegWriteInt(SHCU,szKey,"FilterIndex",FilterIndex);	
	SHRegWriteInt(SHCU,szKey,"bToolBarBig",bToolBarBig);
	SHRegWriteInt(SHCU,szKey,"bToolBarText",bToolBarText);
	SHRegWriteInt(SHCU,szKey,"bBrowseMode",bBrowseMode);
	SHRegWriteInt(SHCU,szKey,"bSoundFindFail",bSoundFindFail);
	SHRegWriteInt(SHCU,szKey,"bWatchChange",bWatchChange);
	SHRegWriteInt(SHCU,szKey,"bReloadNoAsk",bReloadNoAsk);
	SHRegWriteInt(SHCU,szKey,"bReloadProject",bReloadProject);

	SHRegWriteInt(SHCU,szKey,"LineRatio",LineRatio);
	SHRegWriteInt(SHCU,szKey,"bWrap",bWrap);
	SHRegWriteInt(SHCU,szKey,"nWrap",nWrap);
	SHRegWriteInt(SHCU,szKey,"HideSelType",HideSelType);
	SHRegWriteInt(SHCU,szKey,"nShowCurLine",nShowCurLine);
	SHRegWriteInt(SHCU,szKey,"TabWidth",TabWidth);
	SHRegWriteInt(SHCU,szKey,"RightWrap",RightWrap); 	
	SHRegWriteInt(SHCU,szKey,"ColMark",ColMark);
	SHRegWriteInt(SHCU,szKey,"ShowTabType",ShowTabType);
	SHRegWriteInt(SHCU,szKey,"ShowEnterType",ShowEnterType);
	SHRegWriteInt(SHCU,szKey,"ShowSpaceType",ShowSpaceType);
	SHRegWriteInt(SHCU,szKey,"CaretWidth",CaretWidth);
	SHRegWriteInt(SHCU,szKey,"UndoLimit",UndoLimit);
	SHRegWriteString(SHCU,szKey,"FaceName",logfont.lfFaceName);
	SHRegWriteInt(SHCU,szKey,"PitchAndFamily",logfont.lfPitchAndFamily);
	SHRegWriteInt(SHCU,szKey,"CharSet",logfont.lfCharSet);
	SHRegWriteInt(SHCU,szKey,"FontHeight",logfont.lfHeight);
	SHRegWriteInt(SHCU,szKey,"FontWeight",logfont.lfWeight);
	SHRegWriteInt(SHCU,szKey,"FontItalic",logfont.lfItalic);
	SHRegWriteInt(SHCU,szKey,"FindDlgPos",FindDlgPos);

	SHRegWriteString(SHCU,szKey,"PrtFaceName",prtFont.lfFaceName);
	SHRegWriteInt(SHCU,szKey,"PrtPitchAndFamily",prtFont.lfPitchAndFamily);
	SHRegWriteInt(SHCU,szKey,"PrtCharSet",prtFont.lfCharSet);
	SHRegWriteInt(SHCU,szKey,"PrtFontHeight",prtFont.lfHeight);
	SHRegWriteInt(SHCU,szKey,"PrtMarginLeft",prtMargin.left);
	SHRegWriteInt(SHCU,szKey,"PrtMarginTop",prtMargin.top);
	SHRegWriteInt(SHCU,szKey,"PrtMarginRight",prtMargin.right);
	SHRegWriteInt(SHCU,szKey,"PrtMarginBottom",prtMargin.bottom);
	SHRegWriteInt(SHCU,szKey,"prtbParse",prtbParse);
	SHRegWriteInt(SHCU,szKey,"prtnWrap",prtnWrap);
	SHRegWriteInt(SHCU,szKey,"prtbLineNum",prtbLineNum);
	SHRegWriteInt(SHCU,szKey,"prtbSysHeader",prtbSysHeader);
	SHRegWriteInt(SHCU,szKey,"prtbDirect",prtbDirect);
	SHRegWriteInt(SHCU,szKey,"prtbParse",prtbParse);
	SHRegWriteInt(SHCU,szKey,"prtnHeader",prtnHeader);
	SHRegWriteInt(SHCU,szKey,"prtnFooter",prtnFooter);
	SHRegWriteString(SHCU,szKey,"prtHeader",prtHeader);
	SHRegWriteString(SHCU,szKey,"prtFooter",prtFooter);
	SHRegWriteString(SHCU,szKey,"prtBkText",prtBkText);

	// 1.2에서 추가된 옵션들
	SHRegWriteInt(SHCU,szKey,"bpl",bpl);
	SHRegWriteInt(SHCU,szKey,"bCapital",bCapital);
	SHRegWriteInt(SHCU,szKey,"nHexSep",nHexSep);
	SHRegWriteInt(SHCU,szKey,"BackupLevel",BackupLevel);
	SHRegWriteString(SHCU,szKey,"BackupFolder",BackupFolder);
	SHRegWriteInt(SHCU,szKey,"BackupNaming",BackupNaming);
	SHRegWriteInt(SHCU,szKey,"AutoSaveInterval",AutoSaveInterval);
	SHRegWriteInt(SHCU,szKey,"bMakeEditInfo",bMakeEditInfo);
	SHRegWriteInt(SHCU,szKey,"bUpdateEditInfo",bUpdateEditInfo);
	SHRegWriteString(SHCU,szKey,"EditInfoFolder",EditInfoFolder);
	SHRegWriteInt(SHCU,szKey,"EditInfoMask",EditInfoMask);
	SHRegWriteInt(SHCU,szKey,"bUseEditInfo",bUseEditInfo);
	SHRegWriteString(SHCU,szKey,"FixedFaceName",FixedFont.lfFaceName);
	SHRegWriteInt(SHCU,szKey,"FixedPitchAndFamily",FixedFont.lfPitchAndFamily);
	SHRegWriteInt(SHCU,szKey,"FixedCharSet",FixedFont.lfCharSet);
	SHRegWriteInt(SHCU,szKey,"FixedFontHeight",FixedFont.lfHeight);
	SHRegWriteInt(SHCU,szKey,"bShowClipWnd",bShowClipWnd);
	SHRegWriteInt(SHCU,szKey,"ClipWndRatio",ClipWndRatio);
	SHRegWriteInt(SHCU,szKey,"bReloadClip",bReloadClip);
	SHRegWriteInt(SHCU,szKey,"bPreviewClip",bPreviewClip);
	SHRegWriteInt(SHCU,szKey,"InnerRectLeft",InnerRect.left);
	SHRegWriteInt(SHCU,szKey,"InnerRectTop",InnerRect.top);
	SHRegWriteInt(SHCU,szKey,"InnerRectRight",InnerRect.right);
	SHRegWriteInt(SHCU,szKey,"InnerRectBottom",InnerRect.bottom);
	SHRegWriteInt(SHCU,szKey,"VAlign",VAlign);
	SHRegWriteInt(SHCU,szKey,"SelBlockType",SelBlockType);
	SHRegWriteString(SHCU,szKey,"BackBit",BackBit);
	SHRegWriteInt(SHCU,szKey,"BackBitPos",BackBitPos);
	SHRegWriteInt(SHCU,szKey,"RmWrap",RmWrap);
	SHRegWriteString(SHCU,szKey,"RmFaceName",Rmlogfont.lfFaceName);
	SHRegWriteInt(SHCU,szKey,"RmPitchAndFamily",Rmlogfont.lfPitchAndFamily);
	SHRegWriteInt(SHCU,szKey,"RmCharSet",Rmlogfont.lfCharSet);
	SHRegWriteInt(SHCU,szKey,"RmFontHeight",Rmlogfont.lfHeight);
	SHRegWriteInt(SHCU,szKey,"RmLineRatio",RmLineRatio);
	SHRegWriteInt(SHCU,szKey,"RmFreq",RmFreq);
	SHRegWriteInt(SHCU,szKey,"RmLineNum",RmLineNum);
	SHRegWriteInt(SHCU,szKey,"RmManual",RmManual);
	SHRegWriteInt(SHCU,szKey,"nWheel",nWheel);
	SHRegWriteInt(SHCU,szKey,"nCtrlWheel",nCtrlWheel);
	SHRegWriteInt(SHCU,szKey,"nShiftWheel",nShiftWheel);
	SHRegWriteInt(SHCU,szKey,"nCtrlShiftWheel",nCtrlShiftWheel);
	SHRegWriteInt(SHCU,szKey,"CharExtra",CharExtra);
	SHRegWriteInt(SHCU,szKey,"WordExtra",WordExtra);
	SHRegWriteInt(SHCU,szKey,"CharRatio",CharRatio);
	SHRegWriteInt(SHCU,szKey,"CursorEdit",CursorEdit);

	lstrcpy(szKey,Key);
	lstrcat(szKey,"Style");
	for (i=1;i<PARSENUM;i++) {
		wsprintf(szSub,"%d-Ext",i);
		SHRegWriteString(SHCU,szKey,szSub,arExt[i]);
		for (j=0;j<32;j++) {
			if (arStyle[i][j].fore == -2) {
				break;
			}
			wsprintf(szSub,"%d-%d-name",i,j);
			SHRegWriteString(SHCU,szKey,szSub,arStyle[i][j].name);
			wsprintf(szSub,"%d-%d-fore",i,j);
			SHRegWriteInt(SHCU,szKey,szSub,arStyle[i][j].fore);
			wsprintf(szSub,"%d-%d-back",i,j);
			SHRegWriteInt(SHCU,szKey,szSub,arStyle[i][j].back);
		}
	}

	lstrcpy(szKey,Key);
	lstrcat(szKey,"Ftp");
	for (i=0;;i++) {
		if (lstrlen(arFtp[i].Name)==0) {
			SHRegWriteInt(SHCU,szKey,"Num",i);
			break;
		}
		wsprintf(szSub,"Name-%d",i);
		EncodeText(arFtp[i].Name,EncText);
		SHRegWriteString(SHCU,szKey,szSub,EncText);
		wsprintf(szSub,"Addr-%d",i);
		EncodeText(arFtp[i].Addr,EncText);
		SHRegWriteString(SHCU,szKey,szSub,EncText);
		wsprintf(szSub,"User-%d",i);
		EncodeText(arFtp[i].User,EncText);
		SHRegWriteString(SHCU,szKey,szSub,EncText);
		wsprintf(szSub,"Pass-%d",i);
		EncodeText(arFtp[i].Pass,EncText);
		SHRegWriteString(SHCU,szKey,szSub,EncText);
		wsprintf(szSub,"Port-%d",i);
		SHRegWriteInt(SHCU,szKey,szSub,arFtp[i].Port);
	}

	lstrcpy(szKey,Key);
	lstrcat(szKey,"Status");
	for (i=0;i<STNUM;i++) {
		wsprintf(szSub,"Show%d",i);
		SHRegWriteInt(SHCU,szKey,szSub,StMask[i].bShow);
		wsprintf(szSub,"Order%d",i);
		SHRegWriteInt(SHCU,szKey,szSub,StMask[i].Order);
		wsprintf(szSub,"Width%d",i);
		SHRegWriteInt(SHCU,szKey,szSub,StMask[i].Width);
		wsprintf(szSub,"Pop%d",i);
		SHRegWriteInt(SHCU,szKey,szSub,StMask[i].bPop);
		wsprintf(szSub,"Align%d",i);
		SHRegWriteInt(SHCU,szKey,szSub,StMask[i].Align);
	}

	// 색상 옵션 저장
	lstrcpy(szKey,Key);
	lstrcat(szKey,"Color");
	for (i=0;i<AECOLORS;i++) {
		wsprintf(szSub,"Color%d",i);
		SHRegWriteInt(SHCU,szKey,szSub,Color[i]);
	}
}

void ReadMode()
{
	HWND hActive;
	SInfo *pSi;
	LOGFONT OldFont;
	int OldLineRatio;
	int OldWrap;

	hActive=(HWND)SendMessage(g_hMDIClient,WM_MDIGETACTIVE,0,NULL);
	if (hActive == NULL) {
		return;
	}
	pSi=(SInfo *)GetWindowLong(hActive,0);

	// 이전 설정 저장
	pSi->Ae.GetFont(&OldFont);
	OldLineRatio=pSi->Ae.GetLineRatio();
	OldWrap=pSi->Ae.GetWrap();

	DialogBox(g_hInst,MAKEINTRESOURCE(IDD_READMODE),g_hFrameWnd,ReadModeProc);

	// 설정 복구
	pSi->Ae.SetFont(&OldFont);
	pSi->Ae.SetLineRatio(OldLineRatio);
	pSi->Ae.SetWrap(OldWrap);
}
