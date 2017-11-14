//Drawable.cpp
//
#include "stdafx.h"
#include "Drawable.h"

////////////////////////////////////////
HDC cliHDC;
int isresize = -1;
int isrefresh = -1;
HDC memHDC;
HBITMAP hbmp;
Graphics * graphics;
int nWidth;
int nHeight;
RECT rect;
INT width;
INT height;

////////////////////////////////////////
ULONG_PTR m_gdiplusToken;
Gdiplus::Pen pen(Color(0, 0, 0));
////////////////////////////////////////

////////////////////////////////////////
Manager3D man;

////////////////////////////////////////
INT draw_line = 1;
INT move_light = -1;
INT move_trans = -1;

////////////////////////////////////////
EPoint org;
EFTYPE scalex, scaley;
////////////////////////////////////////
Device device;
void onPaint(HWND hWnd) {
	if (isresize) {
		if (isresize != -1) {
			DeleteDC(memHDC);
			DeleteObject(hbmp);
		}
		isresize = 0;

		HDC hdc = GetDC(hWnd);
		memHDC = CreateCompatibleDC(hdc);
		::GetClientRect(hWnd, &rect);
		/*
		nWidth = GetDeviceCaps(hdc, HORZRES);
		nHeight = GetDeviceCaps(hdc, VERTRES);*/
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
		nWidth = width;
		nHeight = height;
		hbmp = CreateCompatibleBitmap(hdc, nWidth, nHeight);
		if (!SelectObject(memHDC, hbmp)) {
			isresize = 1;
			return;
		}
		graphics = Graphics::FromHDC(memHDC);
		if (!graphics) {
			isresize = 1;
			return;
		}
		cliHDC = hdc;

		device.Resize(width, height);
		device.graphics = graphics;

		org.Set(rect.right - rect.left, rect.bottom - rect.top);
		org /= 2;
		scalex = org.x / 4, scaley = org.x / 4;

		man.setCameraRange(org.x, org.y, scalex, scaley);
	}
	if (!graphics) {
		return;
	}
	if (isrefresh <= 0) {
		return;
	}
	isrefresh = 0;

	graphics->Clear(Color(0, 0, 0));
	//Add paint code here
	//Render in device buffer
	if (move_light > 0) {
		device.RenderShade(man);
	}
	device.ClearBeforeRender();
	device.Render(man, NULL, NULL, NULL);
	device.RenderMirror(man);
	//Blt buffer to window buffer
	int i, j, index;
	for (i = 0; i < device.width; i++) {
		for (j = 0; j < device.height; j++){
			index = j *  device.width + i;
			if (device.tango[index] != BLACK)
			{
				::SetPixel(memHDC, i, j, device.tango[index]);
			}
		}
	}

	//end paint code
	BitBlt(cliHDC, 0, 0, nWidth, nHeight, memHDC, 0, 0, SRCCOPY);

}
void onUpdate(HWND hWnd) {
	InvalidateRect(hWnd, 0, 0);
}
void onTimer(HWND hWnd) {
}

void onStartup(HWND hWnd) {
	//GDI Initialize
	Gdiplus::GdiplusStartupInput StartupInput;
	GdiplusStartup(&m_gdiplusToken, &StartupInput, NULL);

	Initialize();
}
void onClose(HWND hWnd) {
	//GDI destroy
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
}
void onResize(HWND hWnd) {
	isresize = 1;
	isrefresh = 1;
}

Object3D * mirror = NULL;

void Initialize() {

	man.addCamera(50, 50, 50, 1000, 90, 90).move(0, 0, -200);
	man.addCamera(30, 30, 60, 600, 30, 30).move(0, 0, -100);
	man.addShadowCamera(100, 100, 500, 1000, 180, 180);

	//man.addLight(9, -51, -60);
	man.addLight(5, 8, 220);
	man.addLight(-1000, 100, 100);

	int count = 2;
	int c = 30;
	int i, j, k;
	EFTYPE r = 10;
	EFTYPE x_1, x_2, r_1, r_2, p_1 = PI / ((EFTYPE)c), p_2 = 2 * PI / ((EFTYPE)c);


	man.addObject().addVert(-10, -10, 10).addVert(10, -10, 10).addVert(-10, 10, 10).addVert(10, 10, 10, -1)
		.scale(10, 10, 10)./*rotate(50, 0, 0).*/move(0, -30, -300).setColor(GREEN);
	man.addObject().renderAABB().addVert(-10, -10, 10).addVert(10, -10, 10).addVert(-10, 10, 10).addVert(10, 10, 10, -1)
		.addVert(10, 10, -10).addVert(10, -10, 10, -1).addVert(10, -10, -10).addVert(-10, -10, 10, -1).addVert(-10, -10, -10)
		.addVert(-10, 10, 10, -1).addVert(-10, 10, -10).addVert(10, 10, -10, -1).addVert(-10, -10, -10).addVert(10, -10, -10, -1)
		.scale(0.5, 0.5, 0.5).move(100, -15, -50).setColor(RED).setLineColor(BLUE);
	man.addObject().renderAABB().addVert(-10, -10, 10).addVert(10, -10, 10).addVert(-10, 10, 10).addVert(10, 10, 10, -1)
		.addVert(10, 10, -10).addVert(10, -10, 10, -1).addVert(10, -10, -10).addVert(-10, -10, 10, -1).addVert(-10, -10, -10)
		.addVert(-10, 10, 10, -1).addVert(-10, 10, -10).addVert(10, 10, -10, -1).addVert(-10, -10, -10).addVert(10, -10, -10, -1)
		.scale(0.5, 0.5, 0.5).move(0, -15, -50).setColor(RED).setLineColor(BLUE);
	mirror = &man.addReflectionObject(1000).addVert(-10, 0, -10).addVert(10, 0, -10).addVert(-10, 0, 10).addVert(10, 0, 10, -1)
		.scale(10, 10, 10).rotate(90, 90, 0).move(100, -20, 0).setColor(LIGHTGRAY).setLineColor(RED);


	for (k = 0; k < count; k++) {
		EFTYPE x = rand() % 300 - 150;
		EFTYPE z = rand() % 300 - 150;
		EFTYPE y = rand() % 100;
		for (i = 0; i < c; i++) {
			x_1 = r * cos(i * p_1);
			r_1 = r * sin(i * p_1);
			x_2 = r * cos((i + 1) * p_1);
			r_2 = r * sin((i + 1) * p_1);
			Object3D& obj = man.addObject().renderAABB().addVert(x_1, 0, -r_1).addVert(x_2, 0, -r_2);
			for (j = 1; j < c; j++) {
				obj.addVert(x_1, r_1 * sin(j * p_2), -r_1 * cos(j * p_2))
					.addVert(x_2, r_2 * sin(j * p_2), -r_2 * cos(j * p_2), -1);
			}
			obj.addVert(x_1, 0, -r_1).addVert(x_2, 0, -r_2, -1).setCenter(0, 0, 0).move(x, y, z).rotate(0, 0, 0).setColor(GREEN).setLineColor(RED);
			if (k == 0) {
				obj.transparent = 1.01;
			}
		}
	}


	man.addObject().renderAABB().addVert(-10, -10, 10).addVert(10, -10, 10).addVert(-10, 10, 10).addVert(10, 10, 10, -1)
		.addVert(10, 10, -10).addVert(10, -10, 10, -1).addVert(10, -10, -10).addVert(-10, -10, 10, -1).addVert(-10, -10, -10)
		.addVert(-10, 10, 10, -1).addVert(-10, 10, -10).addVert(10, 10, -10, -1).addVert(-10, -10, -10).addVert(10, -10, -10, -1)
		.scale(0.5, 0.5, 0.5).move(0, -15, -50).setColor(RED).setLineColor(BLUE);
	man.addObject().renderAABB().addVert(-10, -10, 10).addVert(10, -10, 10).addVert(-10, 10, 10).addVert(10, 10, 10, -1)
		.addVert(10, 10, -10).addVert(10, -10, 10, -1).addVert(10, -10, -10).addVert(-10, -10, 10, -1).addVert(-10, -10, -10)
		.addVert(-10, 10, 10, -1).addVert(-10, 10, -10).addVert(10, 10, -10, -1).addVert(-10, -10, -10).addVert(10, -10, -10, -1)
		.scale(0.5, 0.5, 0.5).move(0, 15, -50).setColor(RED).setLineColor(BLUE);
	man.addObject().renderAABB().addVert(-10, -10, 10).addVert(10, -10, 10).addVert(-10, 10, 10).addVert(10, 10, 10, -1)
		.addVert(10, 10, -10).addVert(10, -10, 10, -1).addVert(10, -10, -10).addVert(-10, -10, 10, -1).addVert(-10, -10, -10)
		.addVert(-10, 10, 10, -1).addVert(-10, 10, -10).addVert(10, 10, -10, -1).addVert(-10, -10, -10).addVert(10, -10, -10, -1)
		.scale(0.5, 0.5, 0.5).move(15, 0, -50).setColor(RED).setLineColor(BLUE);
	man.addTransparentObject(1.01).renderAABB().addVert(-10, -10, 10).addVert(10, -10, 10).addVert(-10, 10, 10).addVert(10, 10, 10, -1)
		.addVert(10, 10, -10).addVert(10, -10, 10, -1).addVert(10, -10, -10).addVert(-10, -10, 10, -1).addVert(-10, -10, -10)
		.addVert(-10, 10, 10, -1).addVert(-10, 10, -10).addVert(10, 10, -10, -1).addVert(-10, -10, -10).addVert(10, -10, -10, -1)
		.scale(0.5, 0.5, 0.5).move(0, 0, -50).setColor(BLUE).setLineColor(BLUE);
}

EFTYPE scale = 10.0;
INT is_control = 0;
VOID onScroll(FLOAT delta) {
	isrefresh = 1;
	if (is_control) {
		if (delta > 0) {
			scale += 1.0;
		}
		else {
			scale -= 1.0;
		}
		if (scale <= 0) {
			scale = 1.0;
		}
	}
	else {
		if (move_light > 0) {
			if (delta > 0) {
				man.moveLight(0, 0, scale);
			}
			else {
				man.moveLight(0, 0, -scale);
			}
		}
		else if (move_trans > 0) {

			if (delta > 0) {
				Obj3D * obj = man.tras.link;
				if (obj) {
					do {

						obj->move(0, 0, scale);


						obj = man.objs.next(obj);
					} while (obj && obj != man.tras.link);
				}
			}
			else {
				Obj3D * obj = man.tras.link;
				if (obj) {
					do {

						obj->move(0, 0, -scale);


						obj = man.objs.next(obj);
					} while (obj && obj != man.tras.link);
				}
			}
		}
		else {
			if (delta > 0) {
				man.moveCamera(0, 0, scale);
			}
			else {
				man.moveCamera(0, 0, -scale);
			}
		}
	}
}

PointF menu;
VOID onMenu(FLOAT x, FLOAT y, INT mode)
{
	if (mode == 1) // mouse down
	{
		menu.X = x;
		menu.Y = y;
	}
	else if (mode == 2) // mouse move
	{
		if (menu.X != 0 && menu.Y != 0)
		{
			if (move_light > 0) {
				man.moveLight(-(x - menu.X) / scale, -(y - menu.Y) / scale, 0);
				isrefresh = 1;
			}
			else {
				man.moveCamera(-(x - menu.X) / scale, -(y - menu.Y) / scale, 0);
				isrefresh = 1;
			}
			menu.X = x;
			menu.Y = y;
		}
	}
	else	// mouse up
	{
		menu.X = 0;
		menu.Y = 0;
	}
}

PointF drag;
VOID onDrag(FLOAT x, FLOAT y, INT mode)
{
	if (mode == 1) // mouse down
	{
		drag.X = x;
		drag.Y = y;
	}
	else if (mode == 2) // mouse move
	{
		if (drag.X != 0 && drag.Y != 0)
		{
			if (move_light > 0) {
				man.rotateLight(-(y - drag.Y) / scale, (x - drag.X) / scale, 0);
				isrefresh = 1;
			}
			else {
				man.rotateCamera(-(y - drag.Y) / scale, (x - drag.X) / scale, 0);
				isrefresh = 1;
			}
			drag.X = x;
			drag.Y = y;
		}
	}
	else	// mouse up
	{
		drag.X = 0;
		drag.Y = 0;
	}
}

VOID onKeyDown(WPARAM wParam, LPARAM lParam) {
	char c = (char)wParam;
	switch (c) {
	case 'L':
		move_light = -move_light;
		break;
	case 'T':
		device.render_linear = -device.render_linear;
		break;
	case 'R':
		device.render_proj = -device.render_proj;
		break;
	case 'G':
		device.render_light = -device.render_light;
		break;
	case 'P':
		man.nextCamera();
		break;
	case 'A':
		if (mirror) {
			mirror->rotate(10, 0, 0);
		}
		break;
	case 'S':
		if (mirror) {
			mirror->rotate(0, 10, 0);
		}
		break;
	case 'D':
		if (mirror) {
			mirror->rotate(0, 0, 10);
		}
		break;
	}
	isrefresh = 1;
}
VOID onKeyUp(WPARAM wParam, LPARAM lParam) {
	char c = (char)wParam;
}