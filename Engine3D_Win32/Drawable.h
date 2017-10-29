//Drawable.h
//
#define _AFXDLL
#include <afxcmn.h>     
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

#include "math3D/Manager3D.h"

void onStartup(HWND hWnd);
void onClose(HWND hWnd);
void onResize(HWND hWnd);
void onPaint(HWND hWnd);
void onUpdate(HWND hWnd);
void onTimer(HWND hWnd);

void Initialize();

void render(VObj * range, VObj * range0, VObj * range1);


VOID onScroll(FLOAT delta);
VOID onMenu(FLOAT x, FLOAT y, INT mode);
VOID onDrag(FLOAT x, FLOAT y, INT mode);
