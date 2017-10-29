//EPaint.cpp
//Paint functions
//Varies with GDI tools
//Author: Stophin
//2014.01.08
//Ver: 0.01
//


#include "EPaint.h"

void EP_RenderStart()
{
	//setrendermode(RENDER_MANUAL);
}

void EP_RenderFlush(INT fps)
{
	//delay_fps(fps);
}

void EP_RenderEnd()
{
	return;
}

void EP_Refresh()
{
	//cleardevice();
}

void EP_Refresh(ECOLOR c)
{
	///setbkcolor(c);
}

EPoint EP_Init(EPTYPE wd,EPTYPE ht)
{
	EPoint gm(wd,ht);
	return gm;
}

void EP_Rectangle(const EPoint& p,const EPoint& w)
{
	//rectangle(p.x,p.y-w.y,p.x+w.x,p.y);
}

void EP_Rectangle(const RectF& p)
{
	//rectangle(p.X, p.Y, p.X + p.Width, p.Y + p.Height);
}
void EP_Rectangle(EPTYPE sx,EPTYPE sy,EPTYPE ex,EPTYPE ey)
{
	//rectangle(sx,sy,ex,ey);
}

void EP_FillRectangle(EPTYPE sx,EPTYPE sy,EPTYPE ex,EPTYPE ey)
{
	//bar(sx,sy,ex,ey);
}

void EP_InverseRect(const EPoint& lt,const EPoint& gm)
{
}

void EP_Line(const EPoint& s,const EPoint& e)
{
}

void EP_Line(EPTYPE sx,EPTYPE sy,EPTYPE ex,EPTYPE ey)
{
}

void EP_Lines(PointF * points, int count)
{
}

void EP_SetColor(ECOLOR c)
{
}

void EP_SetFillColor(ECOLOR c)
{
}

void EP_SetBKColor(ECOLOR c)
{
}

//Load image width file name, and resize
//image				: the image buffer
//fname				: file name
//pwidth&pheight	: Resize to pwidth*pheight, 
//					if pwidth or pheight is less than or equals to 0
//					image will not be resized, and be kept the same
//					with the origin size when loaded.
//					reference symbol '&' means  that this function
//					can change these parameters, so that it can
//					get image size when loaded.
void EP_LoadImage(EIMAGE& image,LPCTSTR fname,EPTYPE& pwidth,EPTYPE& pheight)
{
}


void EP_ResizeImage(EIMAGE& image,EPTYPE& pwidth,EPTYPE& pheight)
{
}

//Truncate picture by color c in itself
//mode: 1: only when current != c then paint
//		2: only when current == c then paint
//		0: all color will be painted
//	other: no response
void EP_TruncImage(EPTYPE x,EPTYPE y,EIMAGE image,ECOLOR c,int mode)
{
}

void EP_TruncateImage(EPTYPE x, EPTYPE y, EPTYPE w, EPTYPE h, EIMAGE image, EPTYPE tx, EPTYPE ty, EPTYPE tw, EPTYPE th, EIMAGE image_bk, INT bk, ECOLOR c)
{
}

//Truncate image with mask image
//mask image is divided by color c ( default to BLACK )
//image will be painted at (x,y)
//(vx,vy) is the offset of destination point from the start point
//(sx,sy) and (ex,ey) is the start and end point of the image
//bk determines using image color (true) or using mask image color (false) to paint
//if cp is passed, then judge whether cp is in drawing point
EPBOOL EP_TruncImage(EPTYPE x, EPTYPE y, EIMAGE image, EIMAGE image_bk, ECOLOR c, EPBOOL bk, const PEPoint cp, int imgtype, EFTYPE rotation, EPTYPE vx, EPTYPE vy, EPTYPE sx, EPTYPE sy, EPTYPE ex, EPTYPE ey)
{
	return 0;
}

void EP_SetPixel(EPTYPE x,EPTYPE y,ECOLOR c)
{
}

ECOLOR EP_GetPixel(EPTYPE x,EPTYPE y,EIMAGE image)
{
	return 0;
}

void EP_CopyImage(EIMAGE& image,EIMAGE source)
{
}

//get image buffer
DWORD* EP_GetImageBuffer(EIMAGE image)
{
	return NULL;
}

void EP_PutImage(EPTYPE x,EPTYPE y,EIMAGE image)
{
}

EIMAGE EP_NewImage()
{
	return NULL;
}

void EP_DeleteImage(EIMAGE& image)
{
}

int EP_GetImageWidth(EIMAGE image)
{
	return 0;
}

int EP_GetImageHeight(EIMAGE image)
{
	return 0;
}


int EP_GetTextWidth(EPCHAR& c)
{
	return 0;
}

void EP_Text(EPTYPE x,EPTYPE y,EPCHAR * c)
{
}

//end of file
