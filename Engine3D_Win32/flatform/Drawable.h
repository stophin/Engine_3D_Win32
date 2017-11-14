//Drawable.h
//
#define _AFXDLL
#include <afxcmn.h>     
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

#include "Device.h"

void onStartup(HWND hWnd);
void onClose(HWND hWnd);
void onResize(HWND hWnd);
void onPaint(HWND hWnd);
void onUpdate(HWND hWnd);
void onTimer(HWND hWnd);

void Initialize();


VOID onScroll(FLOAT delta);
VOID onMenu(FLOAT x, FLOAT y, INT mode);
VOID onDrag(FLOAT x, FLOAT y, INT mode);
VOID onKeyDown(WPARAM wParam, LPARAM lParam);
VOID onKeyUp(WPARAM wParam, LPARAM lParam);
