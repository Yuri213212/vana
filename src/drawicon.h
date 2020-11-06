HICON drawIcon(){
	const POINT aptLt[12]={
		{0,4},
		{2,2},
		{6,2},
		{8,4},
		{8,7},
		{7,8},
		{7,9},
		{3,13},
		{2,13},
		{1,14},
		{0,14},
		{0,4}
	};
	const POINT aptDk[11]={
		{1,6},
		{3,4},
		{5,4},
		{6,5},
		{6,7},
		{5,8},
		{5,9},
		{3,11},
		{2,11},
		{1,12},
		{1,6}
	};
	const BYTE ANDmaskIcon[]={
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00
	};
	const BITMAP bitmapmask={0,16,16,2,1,1,(void *)ANDmaskIcon};
	static ICONINFO iconinfo={TRUE};

	HDC hdc,hdcMem;
	HPEN hPenBlue,hPenCyan;
	HBRUSH hBrushLtPink,hBrushDkPink;
	HICON hIcon;

	hdc=GetDC(NULL);
	hdcMem=CreateCompatibleDC(hdc);
	iconinfo.hbmMask=CreateBitmapIndirect(&bitmapmask);
	iconinfo.hbmColor=CreateCompatibleBitmap(hdc,16,16);
	ReleaseDC(NULL,hdc);
	SelectObject(hdcMem,iconinfo.hbmColor);
	hPenBlue=CreatePen(PS_SOLID,1,RGB(0,102,204));
	hPenCyan=CreatePen(PS_SOLID,1,RGB(48,192,255));
	hBrushLtPink=CreateSolidBrush(RGB(255,128,192));
	hBrushDkPink=CreateSolidBrush(RGB(192,64,128));
	SelectObject(hdcMem,GetStockObject(NULL_PEN));
	SelectObject(hdcMem,GetStockObject(BLACK_BRUSH));
	Rectangle(hdcMem,0,0,17,17);
	SelectObject(hdcMem,hBrushLtPink);
	Polygon(hdcMem,aptLt,12);
	SelectObject(hdcMem,hBrushDkPink);
	Polygon(hdcMem,aptDk,11);
	SelectObject(hdcMem,hPenBlue);
	MoveToEx(hdcMem,3,15,NULL);
	LineTo(hdcMem,3,14);
	SelectObject(hdcMem,hPenCyan);
	LineTo(hdcMem,3,13);
	SelectObject(hdcMem,hPenBlue);
	MoveToEx(hdcMem,5,15,NULL);
	LineTo(hdcMem,5,11);
	SelectObject(hdcMem,hPenCyan);
	LineTo(hdcMem,5,10);
	SelectObject(hdcMem,hPenBlue);
	MoveToEx(hdcMem,7,15,NULL);
	LineTo(hdcMem,7,8);
	SelectObject(hdcMem,hPenCyan);
	LineTo(hdcMem,7,7);
	SelectObject(hdcMem,hPenBlue);
	MoveToEx(hdcMem,9,15,NULL);
	LineTo(hdcMem,9,10);
	SelectObject(hdcMem,hPenCyan);
	LineTo(hdcMem,9,11);
	SelectObject(hdcMem,hPenBlue);
	MoveToEx(hdcMem,11,15,NULL);
	LineTo(hdcMem,11,6);
	SelectObject(hdcMem,hPenCyan);
	LineTo(hdcMem,11,5);
	SelectObject(hdcMem,hPenBlue);
	MoveToEx(hdcMem,13,15,NULL);
	LineTo(hdcMem,13,13);
	SelectObject(hdcMem,hPenCyan);
	LineTo(hdcMem,13,12);
	SelectObject(hdcMem,hPenBlue);
	MoveToEx(hdcMem,15,15,NULL);
	LineTo(hdcMem,15,4);
	SelectObject(hdcMem,hPenCyan);
	LineTo(hdcMem,15,3);
	DeleteObject(hPenBlue);
	DeleteObject(hPenCyan);
	DeleteObject(hBrushLtPink);
	DeleteObject(hBrushDkPink);
	hIcon=CreateIconIndirect(&iconinfo);
	DeleteDC(hdcMem);
	DeleteObject(iconinfo.hbmMask);
	DeleteObject(iconinfo.hbmColor);
	return hIcon;
}
