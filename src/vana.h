#define UNICODE
#define _UNICODE

#include <windows.h>
#include <CommCtrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <math.h>
#ifdef LANG_EN
#include "lang_en/vana_ui.h"
#include "lang_en/vana_help.h"
#endif
#ifdef LANG_CH
#include "lang_ch/vana_ui.h"
#include "lang_ch/vana_help.h"
#endif
#ifdef LANG_JP
#include "lang_jp/vana_ui.h"
#include "lang_jp/vana_help.h"
#endif
#include "iniFormat.h"
#include "wavFile.h"

#define tbuflen		1024
#define clwidth		721
#define clheight	494

enum menuEnum{
	MENU_Load=0x8001,
	MENU_Play,
	MENU_Pause,
	MENU_Stop,
	MENU_Help,
};

#include "settings.h"

HINSTANCE hInstance;
wchar_t wbuf[tbuflen],fbuf[tbuflen];
int cxScreen,cyScreen,width,height,selectRow=0,currentRow=0;
HWAV hwav=NULL,hwavRead;
HDC hdcMem,hdcGraph;
HBITMAP hBitmapGraph=NULL;
HPEN hPenGray,hPenLTGray,hPenRed;
HBRUSH hBrushBg[NoteCount],hBrushBg0,hBrushRed;
RECT bgRect={0,0,clwidth,clheight};
SCROLLINFO vsi={sizeof(SCROLLINFO),SIF_DISABLENOSCROLL|SIF_PAGE|SIF_POS|SIF_RANGE,0,0,16,0,0};

#include "dialog.h"
#include "graph.h"
#include "sound.h"
#include "defproc.h"
#include "drawicon.h"
