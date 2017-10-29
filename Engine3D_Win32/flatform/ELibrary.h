//ELibrary.h
//now we're using ege library tool
//you may change this
//once changed, associated files should be changed
//Author: Stophin
//2014.01.08
//Ver: 0.01
//
#ifndef _ELIBRARY_H_
#define _ELIBRARY_H_

///start of ege define
#ifdef _EGE_LIBRARY_
#include <graphics.h>
#else
#define RGBTOBGR(color)         ((((color) & 0xFF) << 16) | (((color) & 0xFF0000) >> 16) | ((color) & 0xFF00FF00))
#define EGERGB(r, g, b)         ( ((r)<<16) | ((g)<<8) | (b))
#define EGERGBA(r, g, b, a)     ( ((r)<<16) | ((g)<<8) | (b) | ((a)<<24) )
#define EGEARGB(a, r, g, b)     ( ((r)<<16) | ((g)<<8) | (b) | ((a)<<24) )
#define EGEACOLOR(a, color)     ( ((color) & 0xFFFFFF) | ((a)<<24) )
#define EGECOLORA(color, a)     ( ((color) & 0xFFFFFF) | ((a)<<24) )
#define EGEGET_R(c)             ( ((c)>>16) & 0xFF )
#define EGEGET_G(c)             ( ((c)>> 8) & 0xFF )
#define EGEGET_B(c)             ( ((c)) & 0xFF )
#define EGEGET_A(c)             ( ((c)>>24) & 0xFF )
#define EGEGRAY(gray)           ( ((gray)<<16) | ((gray)<<8) | (gray))
#define EGEGRAYA(gray, a)       ( ((gray)<<16) | ((gray)<<8) | (gray) | ((a)<<24) )
#define EGEAGRAY(a, gray)       ( ((gray)<<16) | ((gray)<<8) | (gray) | ((a)<<24) )

// ÑÕÉ«
enum COLORS {
	BLACK = 0,
	BLUE = EGERGB(0, 0, 0xA8),
	GREEN = EGERGB(0, 0xA8, 0),
	CYAN = EGERGB(0, 0xA8, 0xA8),
	RED = EGERGB(0xA8, 0, 0),
	MAGENTA = EGERGB(0xA8, 0, 0xA8),
	BROWN = EGERGB(0xA8, 0xA8, 0),
	LIGHTGRAY = EGERGB(0xA8, 0xA8, 0xA8),
	DARKGRAY = EGERGB(0x54, 0x54, 0x54),
	LIGHTBLUE = EGERGB(0x54, 0x54, 0xFC),
	LIGHTGREEN = EGERGB(0x54, 0xFC, 0x54),
	LIGHTCYAN = EGERGB(0x54, 0xFC, 0xFC),
	LIGHTRED = EGERGB(0xFC, 0x54, 0x54),
	LIGHTMAGENTA = EGERGB(0xFC, 0x54, 0xFC),
	YELLOW = EGERGB(0xFC, 0xFC, 0x54),
	WHITE = EGERGB(0xFC, 0xFC, 0xFC),
};
typedef void * PIMAGE;
typedef unsigned int color_t;

typedef struct ege_colpoint {
	float   x;
	float   y;
	color_t color;
}ege_colpoint;
#endif
/// end of ege defines


#include <comdef.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

#endif	//end of _ELIBRARY_H_
//end of file
