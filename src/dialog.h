enum dlgChildEnum{
	DLG_COMBO_Tuning=1,
	DLG_EDIT_Frequency,
	DLG_EDIT_Transpose,
	DLG_EDIT_Tempo,
	DLG_EDIT_Speed,
	DLG_EDIT_Highlight,
	DLG_EDIT_Offset,
	DLG_EDIT_Amplify,
	DLG_BTN_Analyze,
};

const struct _DLGTEMPLATE{
	DLGTEMPLATE dlgt;
	int dummy;	//DialogBoxIndirectParamW need extra 4 bytes of 0 to read
}_dlgt={
	{WS_CAPTION|WS_SYSMENU|DS_CENTER,WS_EX_DLGMODALFRAME,0,0,0,157,145},	//314*290, real size should multiply 2
	0
};

int dialogopened=0;

wchar_t *notetext(int x){
	static wchar_t result[4];

	switch (x%NoteCount){
	case 0:
	case 1:
		result[0]='C';
		break;
	case 2:
	case 3:
		result[0]='D';
		break;
	case 4:
		result[0]='E';
		break;
	case 5:
	case 6:
		result[0]='F';
		break;
	case 7:
	case 8:
		result[0]='G';
		break;
	case 9:
	case 10:
		result[0]='A';
		break;
	case 11:
		result[0]='B';
		break;
	default:
		result[0]='?';
		break;
	}
	switch (x%NoteCount){
	case 1:
	case 3:
	case 6:
	case 8:
	case 10:
		result[1]='#';
		break;
	default:
		result[1]='-';
		break;
	}
	result[2]=x/NoteCount+'0';
	result[3]=0;
	return result;
}

int textnote(wchar_t *s){
	int result;

	switch (s[0]){
	case 'C':
		result=0;
		break;
	case 'D':
		result=2;
		break;
	case 'E':
		result=4;
		break;
	case 'F':
		result=5;
		break;
	case 'G':
		result=7;
		break;
	case 'A':
		result=9;
		break;
	case 'B':
		result=11;
		break;
	default:
		return -1;
	}
	switch (s[1]){
	case '-':
		break;
	case '#':
		result+=1;
		break;
	default:
		return -1;
	}
	if (s[2]>='0'&&s[2]<='9'){
		result+=(s[2]-'0')*NoteCount;
	}else{
		return -1;
	}
	return result;
}

BOOL CALLBACK DlgProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam){
	static int _tuning,_transpose,_speed,_highlight,_offset;
	static double _frequency,_tempo,_amplify;

	int temp;
	wchar_t *s;
	HINSTANCE hInstanceDlg;
	HWND hwndChild;

	switch (message){
	case WM_INITDIALOG:
		if (!dialogopened){
			dialogopened=1;
			_tuning=DefBaseNote;
			_frequency=DefBaseFreq;
			_transpose=DefTranspose;
			_speed=DefSpeed;
			_highlight=DefHighlight;
			_offset=DefOffset;
			_amplify=DefAmplifyDB;
		}
		_tempo=wav_analysetempo(hwavRead,AutoTempoMin,AutoTempoMax);

		SetWindowTextW(hwnd,szDialog);
		hInstanceDlg=(HINSTANCE)GetWindowLongW(hwnd,GWL_HINSTANCE);
		hwndChild=CreateWindowW(L"static",szLbl_File,WS_CHILD|WS_VISIBLE|SS_LEFT,12,15,84,12,hwnd,NULL,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
		hwndChild=CreateWindowW(L"edit",fbuf,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|ES_READONLY,100,12,202,21,hwnd,NULL,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);

		hwndChild=CreateWindowW(L"static",szLbl_SampleRate,WS_CHILD|WS_VISIBLE|SS_LEFT,12,42,84,12,hwnd,NULL,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
		swprintf(wbuf,L"%.0lf",wav_samplerate(hwavRead));
		hwndChild=CreateWindowW(L"edit",wbuf,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|ES_READONLY,100,39,202,21,hwnd,NULL,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);

		hwndChild=CreateWindowW(L"static",szLbl_Length,WS_CHILD|WS_VISIBLE|SS_LEFT,12,69,84,12,hwnd,NULL,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
		temp=wav_length(hwavRead)/wav_samplerate(hwavRead);
		swprintf(wbuf,L"%d (%d'%02d\")",wav_length(hwavRead),temp/60,temp%60);
		hwndChild=CreateWindowW(L"edit",wbuf,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|ES_READONLY,100,66,202,21,hwnd,NULL,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);

		hwndChild=CreateWindowW(L"static",szLbl_Tuning,WS_CHILD|WS_VISIBLE|SS_LEFT,12,96,84,12,hwnd,NULL,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
		hwndChild=CreateWindowW(L"combobox",L"",WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_TABSTOP|CBS_DROPDOWN|CBS_HASSTRINGS,100,93,50,200,hwnd,(HMENU)DLG_COMBO_Tuning,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"C-4");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"C#4");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"D-4");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"D#4");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"E-4");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"F-4");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"F#4");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"G-4");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"G#4");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"A-4");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"A#4");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"B-4");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"C-5");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"C#5");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"D-5");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"D#5");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"E-5");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"F-5");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"F#5");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"G-5");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"G#5");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"A-5");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"A#5");
		SendMessage(hwndChild,CB_ADDSTRING,0,(LPARAM)L"B-5");
		SendMessage(hwndChild,WM_SETTEXT,0,(LPARAM)notetext(_tuning));
		hwndChild=CreateWindowW(L"static",L"=",WS_CHILD|WS_VISIBLE|SS_LEFT,156,96,11,12,hwnd,NULL,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
		swprintf(wbuf,L"%.1lf",_frequency);
		hwndChild=CreateWindowW(L"edit",wbuf,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP,173,93,100,21,hwnd,(HMENU)DLG_EDIT_Frequency,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
		hwndChild=CreateWindowW(L"static",L"Hz",WS_CHILD|WS_VISIBLE|SS_LEFT,279,96,23,12,hwnd,NULL,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);

		hwndChild=CreateWindowW(L"static",szLbl_Transpose,WS_CHILD|WS_VISIBLE|SS_LEFT,12,123,84,12,hwnd,NULL,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
		swprintf(wbuf,L"%d",_transpose);
		hwndChild=CreateWindowW(L"edit",wbuf,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP,100,120,173,21,hwnd,(HMENU)DLG_EDIT_Transpose,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);

		hwndChild=CreateWindowW(L"static",szLbl_Tempo,WS_CHILD|WS_VISIBLE|SS_LEFT,12,150,84,12,hwnd,NULL,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
		swprintf(wbuf,L"%.2lf",_tempo);
		hwndChild=CreateWindowW(L"edit",wbuf,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP,100,147,173,21,hwnd,(HMENU)DLG_EDIT_Tempo,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
		hwndChild=CreateWindowW(L"static",L"bpm",WS_CHILD|WS_VISIBLE|SS_LEFT,279,150,23,12,hwnd,NULL,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);

		hwndChild=CreateWindowW(L"static",szLbl_Speed,WS_CHILD|WS_VISIBLE|SS_LEFT,12,177,84,12,hwnd,NULL,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
		swprintf(wbuf,L"%d",_speed);
		hwndChild=CreateWindowW(L"edit",wbuf,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP,100,174,173,21,hwnd,(HMENU)DLG_EDIT_Speed,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
		hwndChild=CreateWindowW(L"static",L"x",WS_CHILD|WS_VISIBLE|SS_LEFT,279,177,23,12,hwnd,NULL,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);

		hwndChild=CreateWindowW(L"static",szLbl_Highlight,WS_CHILD|WS_VISIBLE|SS_LEFT,12,204,84,12,hwnd,NULL,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
		swprintf(wbuf,L"%d",_highlight);
		hwndChild=CreateWindowW(L"edit",wbuf,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP,100,201,50,21,hwnd,(HMENU)DLG_EDIT_Highlight,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
		hwndChild=CreateWindowW(L"static",L"+",WS_CHILD|WS_VISIBLE|SS_LEFT,156,204,11,12,hwnd,NULL,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
		swprintf(wbuf,L"%d",_offset);
		hwndChild=CreateWindowW(L"edit",wbuf,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP,173,201,100,21,hwnd,(HMENU)DLG_EDIT_Offset,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);

		hwndChild=CreateWindowW(L"static",szLbl_Amplify,WS_CHILD|WS_VISIBLE|SS_LEFT,12,231,84,12,hwnd,NULL,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
		swprintf(wbuf,L"%.1lf",_amplify);
		hwndChild=CreateWindowW(L"edit",wbuf,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP,100,228,173,21,hwnd,(HMENU)DLG_EDIT_Amplify,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
		hwndChild=CreateWindowW(L"static",L"dB",WS_CHILD|WS_VISIBLE|SS_LEFT,279,231,23,12,hwnd,NULL,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);

		hwndChild=CreateWindowW(L"button",szBtn_Analyze,WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_DEFPUSHBUTTON,227,255,75,23,hwnd,(HMENU)DLG_BTN_Analyze,hInstanceDlg,NULL);
		SendMessage(hwndChild,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),0);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)){
		case DLG_BTN_Analyze:
			GetDlgItemTextW(hwnd,DLG_COMBO_Tuning,wbuf,tbuflen);
			_tuning=textnote(wbuf);
			if (_tuning<0){
				MessageBoxW(hwnd,szErr_Tuning,szDialog,MB_ICONERROR);
				return TRUE;
			}
			GetDlgItemTextW(hwnd,DLG_EDIT_Frequency,wbuf,tbuflen);
			_frequency=wcstod(wbuf,&s);
			if (s[0]||_frequency<=0.0){
				MessageBoxW(hwnd,szErr_Frequency,szDialog,MB_ICONERROR);
				return TRUE;
			}
			GetDlgItemTextW(hwnd,DLG_EDIT_Transpose,wbuf,tbuflen);
			_transpose=wcstol(wbuf,&s,0);
			if (s[0]){
				MessageBoxW(hwnd,szErr_Transpose,szDialog,MB_ICONERROR);
				return TRUE;
			}
			GetDlgItemTextW(hwnd,DLG_EDIT_Tempo,wbuf,tbuflen);
			_tempo=wcstod(wbuf,&s);
			if (s[0]||_tempo<=0.0){
				MessageBoxW(hwnd,szErr_Tempo,szDialog,MB_ICONERROR);
				return TRUE;
			}
			GetDlgItemTextW(hwnd,DLG_EDIT_Speed,wbuf,tbuflen);
			_speed=wcstol(wbuf,&s,0);
			if (s[0]||_speed<=0){
				MessageBoxW(hwnd,szErr_Speed,szDialog,MB_ICONERROR);
				return TRUE;
			}
			GetDlgItemTextW(hwnd,DLG_EDIT_Highlight,wbuf,tbuflen);
			_highlight=wcstol(wbuf,&s,0);
			if (s[0]||_highlight<=0){
				MessageBoxW(hwnd,szErr_Highlight,szDialog,MB_ICONERROR);
				return TRUE;
			}
			GetDlgItemTextW(hwnd,DLG_EDIT_Offset,wbuf,tbuflen);
			_offset=wcstol(wbuf,&s,0);
			if (s[0]||_offset<0){
				MessageBoxW(hwnd,szErr_Offset,szDialog,MB_ICONERROR);
				return TRUE;
			}
			GetDlgItemTextW(hwnd,DLG_EDIT_Amplify,wbuf,tbuflen);
			_amplify=wcstod(wbuf,&s);
			if (s[0]){
				MessageBoxW(hwnd,szErr_Amplify,szDialog,MB_ICONERROR);
				return TRUE;
			}
			BaseNote=(double)_tuning/NoteCount;
			BaseFreq=_frequency;
			Transpose=_transpose;
			if (Transpose<0){
				Transpose=NoteCount-(-Transpose)%NoteCount;
			}else{
				Transpose%=NoteCount;
			}
			Tempo=_tempo;
			Speed=_speed;
			Highlight=_highlight;
			Offset=_offset;
			AmplifyDB=_amplify;
			EndDialog(hwnd,1);
			return TRUE;
		default:
			;
		}
		break;
	case WM_CLOSE:
		EndDialog(hwnd,0);
		return TRUE;
	}
	return FALSE;
}
