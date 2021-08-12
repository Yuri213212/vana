/***

vana v1.1
Copyright (C) 2020-2021 Yuri213212
Site:https://github.com/Yuri213212/vana
Email: yuri213212@vip.qq.com
License: CC BY-NC-SA 4.0
https://creativecommons.org/licenses/by-nc-sa/4.0/

***/

#include "vana.h"

int translateKey(int x){
	x&=0x3fff;
	if (x>=16&&x<=27){
		return x-16;
	}
	if (x>=30&&x<=40){
		return x-30;
	}
	if (x==43||x==28){
		return 11;
	}
	if (x>=44&&x<=53){
		return x-44;
	}
	if (x==115){
		return 10;
	}
	return -1;
}

void drawGraph(){
	static RECT gRect={0,0,clwidth,0};

	int i,j;
	HDC hdc;
	HBRUSH hBrush;

	if (hBitmapGraph){
		DeleteObject(hBitmapGraph);
	}
	gRect.bottom=graphLength*6+1;
	hdc=GetDC(NULL);
	hBitmapGraph=CreateCompatibleBitmap(hdc,clwidth,gRect.bottom);
	ReleaseDC(NULL,hdc);
	SelectObject(hdcGraph,hBitmapGraph);
	FillRect(hdcGraph,&gRect,GetStockObject(BLACK_BRUSH));
	SelectObject(hdcGraph,hPenGray);
	for (j=Offset%Speed;j<=graphLength;j+=Speed){
		MoveToEx(hdcGraph,0,j*6,NULL);
		LineTo(hdcGraph,clwidth,j*6);
	}
	SelectObject(hdcGraph,hPenLTGray);
	for (j=Offset%Highlight;j<=graphLength;j+=Highlight){
		MoveToEx(hdcGraph,0,j*6,NULL);
		LineTo(hdcGraph,clwidth,j*6);
	}
	SelectObject(hdcGraph,GetStockObject(NULL_PEN));
	for (j=0;j<graphLength;++j){
		for (i=0;i<DisplayCount;++i){
			hBrush=CreateSolidBrush(graphColor[j][i]);
			SelectObject(hdcGraph,hBrush);
			Rectangle(hdcGraph,i*6+1,j*6+1,i*6+7,j*6+7);
			DeleteObject(hBrush);
		}
	}
	free(graphColor);
}

void refershWindow(HWND hwnd){
	if (currentRow>=graphLength){
		currentRow=graphLength-1;
	}
	if (currentRow<0){
		currentRow=0;
	}
	SetScrollPos(hwnd,SB_VERT,currentRow,TRUE);
	FillRect(hdcMem,&bgRect,hBrushBg[Transpose]);
	if (hBitmapGraph){
		BitBlt(hdcMem,0,7,clwidth,clheight-7,hdcGraph,0,(currentRow-CursorPos)*6,SRCPAINT);
	}
	InvalidateRect(hwnd,NULL,FALSE);
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam){
	static HWND hwndTooltip=NULL;
	static TOOLINFOW ti[DisplayCount]={};
	static SCROLLINFO hsi={sizeof(SCROLLINFO),SIF_DISABLENOSCROLL|SIF_PAGE|SIF_POS|SIF_RANGE,0,271,16,256,0};
	static HWAVEOUT hWaveOut=NULL;
	static OPENFILENAMEW openFileName={};
	static LARGE_INTEGER wallclock,buttonwait,buttonwaitto;
	static int play=0,waveclosing=0,windowclosing=0,buttonstate=0;

	HDC hdc;
	HBITMAP hBitmap;
	HDROP hdrop;
	FILE *fp;
	int i,j,temp;

	switch (message){
	case WM_CREATE:
		hwndTooltip=CreateWindowEx(WS_EX_TOPMOST,TOOLTIPS_CLASS,NULL,WS_POPUP|TTS_ALWAYSTIP|TTS_NOPREFIX,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,hwnd,NULL,hInstance,NULL);
		SetWindowPos(hwndTooltip,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
		for (i=0;i<DisplayCount;++i){
			ti[i].cbSize=sizeof(TOOLINFOW);
			ti[i].uFlags=TTF_SUBCLASS;
			ti[i].hwnd=hwnd;
			ti[i].uId=i;
			ti[i].rect.left=i*6+1;
			ti[i].rect.top=0;
			ti[i].rect.right=i*6+6;
			ti[i].rect.bottom=clheight;
			ti[i].hinst=hInstance;
			ti[i].lpszText=LPSTR_TEXTCALLBACK;
			SendMessageW(hwndTooltip,TTM_ADDTOOLW,0,(LPARAM)&ti[i]);
		}
		SendMessageW(hwndTooltip,TTM_SETDELAYTIME,TTDT_AUTOPOP,0x7FFF);
		SendMessageW(hwndTooltip,TTM_SETDELAYTIME,TTDT_INITIAL,0);
		SendMessageW(hwndTooltip,TTM_SETDELAYTIME,TTDT_RESHOW,0);

		QueryPerformanceCounter(&buttonwaitto);
		QueryPerformanceFrequency(&buttonwait);
		buttonwait.QuadPart/=10;//100ms
		openFileName.lStructSize=sizeof(OPENFILENAMEW);
		openFileName.hwndOwner=hwnd;
		openFileName.lpstrFilter=L"WAV Files (*.wav)\0*.wav\0\0";
		openFileName.nFilterIndex=1;
		openFileName.lpstrFile=fbuf;
		openFileName.nMaxFile=tbuflen;
		openFileName.lpstrDefExt=L"wav";
		hPenGray=CreatePen(PS_SOLID,1,RGB(63,63,63));
		hPenLTGray=CreatePen(PS_SOLID,1,RGB(191,191,191));
		hPenRed=CreatePen(PS_SOLID,1,RGB(255,0,0));
		hBrushRed=CreateSolidBrush(RGB(255,0,0));
		hdc=GetDC(NULL);
		hdcMem=CreateCompatibleDC(hdc);
		hdcGraph=CreateCompatibleDC(hdc);
		hBitmap=CreateCompatibleBitmap(hdc,clwidth,clheight);
		ReleaseDC(NULL,hdc);
		SelectObject(hdcMem,hBitmap);

		FillRect(hdcMem,&bgRect,GetStockObject(BLACK_BRUSH));
		SelectObject(hdcMem,hPenGray);
		for (i=0;i<2;++i){
			MoveToEx(hdcMem,0,i*6,NULL);
			LineTo(hdcMem,clwidth,i*6);
		}
		for (i=0;i<=DisplayCount;++i){
			MoveToEx(hdcMem,i*6,0,NULL);
			LineTo(hdcMem,i*6,6);
		}
		hBrushBg0=CreatePatternBrush(hBitmap);
		SelectObject(hdcMem,GetStockObject(WHITE_BRUSH));
		for (j=0;j<NoteCount;++j){
			FillRect(hdcMem,&bgRect,hBrushBg0);
			for (i=0;i<DisplayCount;++i){
				switch ((i+NoteCount+StartNote-j)%NoteCount){
				case 0:
				case 2:
				case 4:
				case 5:
				case 7:
				case 9:
				case 11:
					SelectObject(hdcMem,hPenGray);
					MoveToEx(hdcMem,i*6+3,7,NULL);
					LineTo(hdcMem,i*6+3,clheight);
					SelectObject(hdcMem,GetStockObject(NULL_PEN));
					Rectangle(hdcMem,i*6+1,1,i*6+7,7);
					break;
				default:
					;
				}
				if (i==RefNote-StartNote){
					SelectObject(hdcMem,hBrushRed);
					Rectangle(hdcMem,i*6+2,2,i*6+6,6);
					SelectObject(hdcMem,GetStockObject(WHITE_BRUSH));
				}
			}
			SelectObject(hdcMem,hPenRed);
			MoveToEx(hdcMem,0,CursorPos*6+10,NULL);
			LineTo(hdcMem,clwidth,CursorPos*6+10);
			hBrushBg[j]=CreatePatternBrush(hBitmap);
		}
		DeleteObject(hBitmap);
		SetScrollInfo(hwnd,SB_VERT,&vsi,TRUE);
		hsi.nPos=Volume;
		SetScrollInfo(hwnd,SB_HORZ,&hsi,TRUE);
		refershWindow(hwnd);
		if (fbuf[0]){
			PostMessage(hwnd,WM_APP,0,0);
		}
		SetPriorityClass(GetCurrentProcess(),HIGH_PRIORITY_CLASS);
		return 0;
	case WM_DROPFILES:
		if (play) break;
		hdrop=(HDROP)wParam;
		temp=DragQueryFileW(hdrop,0,fbuf,tbuflen);
		DragFinish(hdrop);
		SetWindowPos(hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		SetWindowPos(hwnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		if (temp){
			PostMessage(hwnd,WM_APP,0,0);
		}
		return 0;
	case WM_APP:
		fp=_wfopen(fbuf,L"rb");
		if (!fp){
			swprintf(wbuf,szErr_OpenFile,fbuf);
			MessageBoxW(hwnd,wbuf,szTitle,MB_ICONERROR);
			return 0;
		}
		hwavRead=wav_read(fp);
		fclose(fp);
		if (!hwavRead){
			MessageBoxW(hwnd,szErr_FileFormat,szTitle,MB_ICONERROR);
			return 0;
		}
		if (DialogBoxIndirectParamW(GetModuleHandleW(NULL),&_dlgt.dlgt,hwnd,DlgProc,0)){
			if (LengthLimit>0){
				temp=ceil(wav_length(hwavRead)*Tempo*Speed/(60.0*wav_samplerate(hwavRead)));
				if (temp>LengthLimit){
					swprintf(wbuf,szErr_LengthLimit,temp,LengthLimit);
					MessageBoxW(hwnd,wbuf,szTitle,MB_ICONERROR);
					wav_delete(hwavRead);
					return 0;
				}
			}
			if (hWaveOut){
				waveclosing=1;
				waveOutReset(hWaveOut);
			}else{
				PostMessage(hwnd,WM_APP+1,0,0);
			}
		}else{
			wav_delete(hwavRead);
		}
		return 0;
	case WM_APP+1:
		EnableWindow(hwnd,FALSE);
		SetCursor(LoadCursor(NULL,IDC_WAIT));
		selectRow=0;
		currentRow=0;
		wav_delete(hwav);
		hwav=hwavRead;
		graph_analyze(hwav,Tempo*Speed,pow(2.0,AmplifyDB/3.0));
		drawGraph();
		vsi.nMax=graphLength+Highlight-1;
		vsi.nPos=0;
		vsi.nPage=Highlight;
		SetScrollInfo(hwnd,SB_VERT,&vsi,TRUE);
		refershWindow(hwnd);
		SetCursor(LoadCursor(NULL,IDC_ARROW));
		EnableWindow(hwnd,TRUE);
		ShowWindow(hwnd,SW_MINIMIZE);	//refresh after not responding state
		ShowWindow(hwnd,SW_RESTORE);
		SetWindowPos(hwnd,NULL,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		hWaveOut=openSound(hwnd);
		if (!hWaveOut){
			wav_delete(hwav);
			hwav=NULL;
			MessageBoxW(hwnd,szErr_AudioDevice,szTitle,MB_ICONERROR);
		}
		return 0;
	case WM_ERASEBKGND:
		return TRUE;
	case WM_PAINT:
		hdc=GetDC(hwnd);
		BitBlt(hdc,0,0,clwidth,clheight,hdcMem,0,0,SRCCOPY);
		ReleaseDC(hwnd,hdc);
		ValidateRect(hwnd,NULL);
		return 0;
	case WM_NOTIFY:{
		NMHDR *pnmhdr;

		pnmhdr=(NMHDR *)lParam;
		switch (pnmhdr->code){
			case TTN_GETDISPINFOW:{
				static wchar_t buf0[4];

				NMTTDISPINFOW *pnmtdi;
				int id;

				id=pnmhdr->idFrom;
				pnmtdi=(NMTTDISPINFOW *)lParam;
				if (Transpose>6){
					notetext(buf0,pnmhdr->idFrom+StartNote-Transpose+12);
				}else{
					notetext(buf0,pnmhdr->idFrom+StartNote-Transpose);
				}
				swprintf(pnmtdi->szText,szTooltip,buf0,notetext(NULL,pnmhdr->idFrom+StartNote));
			break;}
		}
		break;}
	case WM_COMMAND:
		switch (LOWORD(wParam)){
		case MENU_Load:
			if (play) return 0;
			if (GetOpenFileNameW(&openFileName)){
				PostMessage(hwnd,WM_APP,0,0);
			}
			return 0;
		case MENU_Play:
			if (!hWaveOut) return 0;
			if (play){
				currentRow=selectRow;
			}else{
				selectRow=currentRow;
			}
			sampleCounter=graphStart[currentRow];
			play=1;
			return 0;
		case MENU_Pause:
			if (!play) return 0;
			play=0;
			refershWindow(hwnd);
			return 0;
		case MENU_Stop:
			if (!play) return 0;
			play=0;
			currentRow=selectRow;
			refershWindow(hwnd);
			return 0;
		case MENU_Help:
			MessageBoxW(hwnd,szHelp,szTitle,MB_ICONINFORMATION);
			return 0;
		default:
			;
		}
		break;
	case WM_MOUSEWHEEL:
		if (play) return 0;
		if (!graphLength) return 0;
		if ((short)HIWORD(wParam)<0){
			currentRow+=Highlight;
		}else{
			currentRow-=Highlight;
		}
		refershWindow(hwnd);
		return 0;
	case WM_VSCROLL:
		if (play) return 0;
		if (!graphLength) return 0;
		switch(LOWORD(wParam)){
		case SB_TOP:
			currentRow=0;
			break;
		case SB_BOTTOM:
			currentRow=graphLength-1;
			break;
		case SB_LINEUP:
			--currentRow;
			break;
		case SB_PAGEUP:
			currentRow-=Highlight;
			break;
		case SB_LINEDOWN:
			++currentRow;
			break;
		case SB_PAGEDOWN:
			currentRow+=Highlight;
			break;
		case SB_THUMBTRACK:
			currentRow=HIWORD(wParam);
			break;
		default:
			return 0;
		}
		refershWindow(hwnd);
		return 0;
	case WM_HSCROLL:
		switch(LOWORD(wParam)){
		case SB_LEFT:
			Volume=0;
			break;
		case SB_RIGHT:
			Volume=256;
			break;
		case SB_LINELEFT:
			Volume-=1;
			break;
		case SB_PAGELEFT:
			Volume-=16;
			break;
		case SB_LINERIGHT:
			Volume+=1;
			break;
		case SB_PAGERIGHT:
			Volume+=16;
			break;
		case SB_THUMBTRACK:
			Volume=HIWORD(wParam);
			break;
		default:
			return 0;
		}
		if (Volume>256){
			Volume=256;
		}
		if (Volume<0){
			Volume=0;
		}
		SetScrollPos(hwnd,SB_HORZ,Volume,TRUE);
		return 0;
	case WM_SETFOCUS:
		buttonstate=1;
		QueryPerformanceCounter(&wallclock);
		buttonwaitto.QuadPart=wallclock.QuadPart+buttonwait.QuadPart;
		break;
	case WM_LBUTTONDOWN:
		if (buttonstate){
			buttonstate=0;
			QueryPerformanceCounter(&wallclock);
			if (buttonwaitto.QuadPart-wallclock.QuadPart>0) return 0;
		}
		if (play) return 0;
		if (!graphLength) return 0;
		temp=HIWORD(lParam);
		if (temp<7) return 0;
		currentRow+=(temp-7)/6-CursorPos;
		refershWindow(hwnd);
		return 0;
	case WM_KEYDOWN:
		switch (wParam){
		case VK_LEFT:
			SendMessage(hwnd,WM_HSCROLL,SB_PAGELEFT,0);
			return 0;
		case VK_RIGHT:
			SendMessage(hwnd,WM_HSCROLL,SB_PAGERIGHT,0);
			return 0;
		case VK_UP:
			SendMessage(hwnd,WM_VSCROLL,SB_LINEUP,0);
			return 0;
		case VK_DOWN:
			SendMessage(hwnd,WM_VSCROLL,SB_LINEDOWN,0);
			return 0;
		case VK_PRIOR://Page Up
			SendMessage(hwnd,WM_VSCROLL,SB_PAGEUP,0);
			return 0;
		case VK_NEXT://Page Down
			SendMessage(hwnd,WM_VSCROLL,SB_PAGEDOWN,0);
			return 0;
		case VK_HOME:
			SendMessage(hwnd,WM_VSCROLL,SB_TOP,0);
			return 0;
		case VK_END:
			SendMessage(hwnd,WM_VSCROLL,SB_BOTTOM,0);
			return 0;
		case VK_RETURN://Enter
			SendMessage(hwnd,WM_COMMAND,MENU_Play,0);
			return 0;
		case VK_ESCAPE://Esc
			SendMessage(hwnd,WM_COMMAND,MENU_Stop,0);
			return 0;
		case ' ':
			if (!hWaveOut) return 0;
			if (play){
				SendMessage(hwnd,WM_COMMAND,MENU_Pause,0);
			}else{
				SendMessage(hwnd,WM_COMMAND,MENU_Play,0);
			}
			return 0;
		default:
			temp=translateKey(HIWORD(lParam));
			if (temp<0) return 0;
			Transpose=temp;
			if (!play){
				refershWindow(hwnd);
			}
			return 0;
		}
		return 0;
	case MM_WOM_OPEN:
		waveOutWrite(hWaveOut,&waveHdr[0],sizeof(WAVEHDR));
		waveOutWrite(hWaveOut,&waveHdr[1],sizeof(WAVEHDR));
		return 0;
	case MM_WOM_DONE:
		if (waveclosing){
			waveOutClose(hWaveOut);
			return 0;
		}
		if (play){
			fillBuffer((short *)((PWAVEHDR)lParam)->lpData);
			waveOutWrite(hWaveOut,(PWAVEHDR)lParam,sizeof(WAVEHDR));
			if (sampleCounter>=wav_length(hwav)){
				SendMessage(hwnd,WM_COMMAND,MENU_Stop,0);
			}else{
				refershWindow(hwnd);
			}
		}else{
			memset(((PWAVEHDR)lParam)->lpData,0,BufferLength*sizeof(short));
			waveOutWrite(hWaveOut,(PWAVEHDR)lParam,sizeof(WAVEHDR));
		}
		return 0;
	case MM_WOM_CLOSE:
		waveOutUnprepareHeader(hWaveOut,&waveHdr[0],sizeof(WAVEHDR));
		waveOutUnprepareHeader(hWaveOut,&waveHdr[1],sizeof(WAVEHDR));
		hWaveOut=NULL;
		deleteSound();
		if (windowclosing){
			DestroyWindow(hwnd);
		}else if (waveclosing){
			waveclosing=0;
			PostMessage(hwnd,WM_APP+1,0,0);
		}
		return 0;
	case WM_CLOSE:
		if (hWaveOut){
			play=0;
			waveclosing=1;
			windowclosing=1;
			waveOutReset(hWaveOut);
		}else{
			DestroyWindow(hwnd);
		}
		return 0;
	case WM_DESTROY:
		SetPriorityClass(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
		wav_delete(hwav);
		if (graphStart){
			free(graphStart);
		}
		if (hBitmapGraph){
			DeleteObject(hBitmapGraph);
		}
		DeleteObject(hPenGray);
		DeleteObject(hPenLTGray);
		DeleteObject(hPenRed);
		for (j=0;j<NoteCount;++j){
			DeleteObject(hBrushBg[j]);
		}
		DeleteObject(hBrushBg0);
		DeleteObject(hBrushRed);
		DeleteDC(hdcMem);
		DeleteDC(hdcGraph);
		PostQuitMessage(0);
		return 0;
	}
	return myDefWindowProc(hwnd,message,wParam,lParam);
}

int main(){
	int argc;
	wchar_t **argv;
	int iCmdShow;
	STARTUPINFOW si;

	HICON hIcon;
	WNDCLASSW wndclass;
	FILE *fp;
	HMENU hMenu;
	HWND hwnd;
	MSG msg;
	int i;

	argv=CommandLineToArgvW(GetCommandLineW(),&argc);
	hInstance=GetModuleHandleW(NULL);
	GetStartupInfoW(&si);
	iCmdShow=si.dwFlags&STARTF_USESHOWWINDOW?si.wShowWindow:SW_SHOWNORMAL;

	hIcon=drawIcon();
	wndclass.style=CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc=WndProc;
	wndclass.cbClsExtra=0;
	wndclass.cbWndExtra=0;
	wndclass.hInstance=hInstance;
	wndclass.hIcon=hIcon;
	wndclass.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground=NULL;
	wndclass.lpszMenuName=szAppName;
	wndclass.lpszClassName=szAppName;
	if (!RegisterClassW(&wndclass)){
		MessageBoxW(NULL,szErr_RegisterClass,argv[0],MB_ICONERROR);
		DestroyIcon(hIcon);
		return 0;
	}

	GetCurrentDirectoryW(tbuflen,fbuf);
	GetModuleFileNameW(NULL,wbuf,tbuflen);
	wbuf[tbuflen-1]=0;
	for (i=wcslen(wbuf)-1;wbuf[i]!='\\'&&i>=0;--i){
		wbuf[i]=0;
	}
	SetCurrentDirectoryW(wbuf);
	fp=_wfopen(szSettingFile,L"rb");
	if (fp){
		if (!readSettings(fp)){
			swprintf(wbuf,szWarn_SettingFile,szSettingFile);
			MessageBoxW(NULL,wbuf,argv[0],MB_ICONWARNING);
		}
		fclose(fp);
	}
	BaseNote=(double)DefBaseNote/NoteCount;
	BaseFreq=DefBaseFreq;
	Transpose=DefTranspose;
	if (Transpose<0){
		Transpose=NoteCount-(-Transpose)%NoteCount;
	}else{
		Transpose%=NoteCount;
	}
	Speed=DefSpeed;
	Highlight=DefHighlight;
	Offset=DefOffset;
	AmplifyDB=DefAmplifyDB;
	Volume=DefVolume;
	SetCurrentDirectoryW(fbuf);
	if (argc>1){
		wcscpy(fbuf,argv[1]);
	}else{
		fbuf[0]=0;
	}

	cxScreen=GetSystemMetrics(SM_CXSCREEN);
	cyScreen=GetSystemMetrics(SM_CYSCREEN);
	width=clwidth+GetSystemMetrics(SM_CXFIXEDFRAME)*2+GetSystemMetrics(SM_CXVSCROLL);
	height=clheight+GetSystemMetrics(SM_CYFIXEDFRAME)*2+GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYHSCROLL)+GetSystemMetrics(SM_CYMENU);
	hMenu=CreateMenu();
	AppendMenuW(hMenu,MF_STRING,MENU_Load,szMenu_Load);
	AppendMenuW(hMenu,MF_STRING,MENU_Play,szMenu_Play);
	AppendMenuW(hMenu,MF_STRING,MENU_Pause,szMenu_Pause);
	AppendMenuW(hMenu,MF_STRING,MENU_Stop,szMenu_Stop);
	AppendMenuW(hMenu,MF_STRING,MENU_Help,szMenu_Help);
	hwnd=CreateWindowExW(WS_EX_ACCEPTFILES,szAppName,szTitle,WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_VSCROLL|WS_HSCROLL,(cxScreen-width)/2,(cyScreen-height)/2,width,height,NULL,hMenu,hInstance,NULL);
	ShowWindow(hwnd,iCmdShow);
	UpdateWindow(hwnd);
	while (GetMessage(&msg,NULL,0,0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	DestroyIcon(hIcon);
	LocalFree(argv);
	return msg.wParam;
}
