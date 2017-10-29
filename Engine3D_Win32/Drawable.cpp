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
INT render_linear = 1;
INT render_proj = -1;
INT render_light = -1;

////////////////////////////////////////
EPoint org;
EFTYPE scalex, scaley;
EPoint e, s, v, c;
Vert3D p;
ege_colpoint cps[3];
Vert3D n, n_1, n_2, n0, n1, n2, r;
DWORD * _tango, *_image, *_trans, *_mirror;
DWORD * __image, *__tango, *__trans, *__mirror;
FLOAT *__depth;
DWORD ___image, ___last = 0;
DWORD * image;
DWORD * tango;
FLOAT * depth;
FLOAT * depth_r;
FLOAT * shade;
////////////////////////////////////////
void render(VObj * range, VObj * range0, VObj * range1) {

	VObj * _range;

	Mat3D mm;

	Obj3D * obj = man.objs.link;
	if (obj) {
		int render_trans = 0;
		int trans_w0 = EP_MAX, trans_h0 = EP_MAX;
		int trans_w1 = -EP_MAX, trans_h1 = -EP_MAX;
		VObj * v, *v0, *v1, *vtemp;

		EFTYPE z;
		INT index = 0, _index = 0;
		INT xs, xe, ys, ye;
		int i, j;
		int res;
		Camera3D* cam = NULL;
		Lgt3D * lgt;
		FLOAT zz;
		EFTYPE f, t, trans, _i, _j;
		DWORD * __trans, *__tango;
		do {
			v = obj->verts_r.link;
			if (v && obj->verts_r.linkcount > 0) {
				v0 = NULL; v1 = NULL;
				do {
					if (v0 && v1) {
						// back face cull
						if (v->backface > 0) {

							_range = NULL;
							if (range) {
								//if (Vert3D::CrossRect(range->xs, range->xe, range->ys, range->ye,v->xs, v->xe, v->ys, v->ye))
								_range = range;
							}
							else {
								_range = v;
							}
							// in range
							if (_range) {
								if (render_linear < 0) {
									cps[0].x = v0->x0; cps[0].y = v0->y0; cps[0].color = RED;
									cps[1].x = v1->x0; cps[1].y = v1->y0; cps[1].color = BLUE;
									cps[2].x = v->x0; cps[2].y = v->y0; cps[2].color = GREEN;

									// draw poly
									//ege::fillpoly_gradient(3, cps, image);
									// draw outline
									if (draw_line > 0) {
										pen.SetColor(Color(255, 2555, 2555));
										graphics->DrawLine(&pen, v0->x0, v0->y0, v1->x0, v1->y0);
										graphics->DrawLine(&pen, v1->x0, v1->y0, v->x0, v->y0);
										graphics->DrawLine(&pen, v->x0, v->y0, v0->x0, v0->y0);
										//ege::setcolor(WHITE);
										//ege::line(v0->x0, v0->y0, v1->x0, v1->y0, image);
										//ege::line(v1->x0, v1->y0, v->x0, v->y0, image);
										//ege::line(v->x0, v->y0, v0->x0, v0->y0, image);
									}
								}
								else {
									__image = &___image;
								}

								index = 0;
								xs = _range == v ? v->xs : max(_range->xs, v->xs); ys = _range == v ? v->ys : max(_range->ys, v->ys);
								xe = _range == v ? v->xe : min(_range->xe, v->xe); ye = _range == v ? v->ye : min(_range->ye, v->ye);
								for (i = ys; i <= ye && i < height; i++) {
									for (j = xs; j <= xe && j < width; j++) {
										index = i * width + j;
										if (render_linear < 0) {
											__image = &_image[index];
										}
										else {
											// linear interpolation
											res = Vert3D::IsInTriangle(v0->v_s, v1->v_s, v->v_s, p.set((FLOAT)j, (FLOAT)i, 0));
											if (res > 0) {
												*__image = obj->color;
												//*__image = obj->line;
											}
											else if (res < 0) {
												*__image = obj->color;
											}
											else{
												*__image = BLACK;
											}
										}
										if (*__image != BLACK) {
											__depth = &depth[index];
											// get depth
											//(-n.x * ((FLOAT)j - v.x) - n.y * ((FLOAT)i - v.y)) / n.z + v->z
											z = Vert3D::getZ(v->n_d, v->x0, v->y0, v->z, (EFTYPE)j, (EFTYPE)i);
											if (EP_ISZERO(*__depth)) {
												*__depth = z;
											}
											if (*__depth <= z) {
												__tango = &_tango[index];

												if (render_linear < 0) {
													// replace gradient color to object's color
													// 11053224 is white, which changed from the 
													// setteled value 16579836 when painted
													if (*__image == 11053224) {
														*__image = obj->line;
														//*__image = obj->color;
													}
													else {
														*__image = obj->color;
													}
												}

												// render light
												cam = obj->cam;
												if (cam) {
													n0.set((j - cam->offset_w) / cam->scale_w, (i - cam->offset_h) / cam->scale_h, z, 1);
													//n0.set(j, i, z, 1);
													// get position
													//(-n_1.x * (n0.x - v->x) - n_1.y * (n0.y - v->y)) / n_1.z + v->zz;
													zz = Vert3D::getZ(v->n_z, v->x, v->y, v->zz, n0.x, n0.y);
													man.cams.link->anti_normalize(n0, zz);

													lgt = man.lgts.link;
													f = 0;
													if (lgt) {
														do {
															f += lgt->getFactor(v->n_r, n0);

															lgt = man.lgts.next(lgt);
														} while (lgt && lgt != man.lgts.link);
													}

													if (!EP_ISZERO(obj->transparent)) {
														r.set(n0);
														t = r.negative() & v->n_r;
														if (t < 0) t = -t;
														trans = obj->transparent;
														if (trans < 0) trans = -trans;

														_i = (i - obj->center_r.y) * (trans / t) + obj->center_r.y;
														_j = (j - obj->center_r.x) * (trans / t) + obj->center_r.x;

														if (obj->transparent < 0) {
															_i = 2 * obj->center_r.y - _i;
															_j = 2 * obj->center_r.x - _j;
														}
														if (!(_i < 0 || _i > height - 1 || _j < 0 || _j > width - 1)) {
															_index = (INT)_i * width + (INT)_j;
															__trans = &_trans[index];
															//if (depth[_index] < z) 
															if (1)
															{
																*__trans = Light3D::add(*__image, _tango[_index], f);
															}
															else {
																*__trans = Light3D::multi(*__image, f);
															}
															if (*__trans == BLACK) {
																*__trans++;
															}
															if (trans_w1 < j) trans_w1 = j;
															if (trans_h1 < i) trans_h1 = i;
															if (trans_w0 > j) trans_w0 = j;
															if (trans_h0 > i) trans_h0 = i;
														}
													}
													else {
														*__image = Light3D::multi(*__image, f);
														*__tango = *__image;

														//::SetPixel(memHDC, j, i, *__tango);

													}
													cam = man.cams.link;
													lgt = man.lgts.link;
													n1.set(n0) *cam->M_1 * lgt->M_1;
													man.cams.link->project(n1);
													_j = (int)(n1.x * cam->scale_w + cam->offset_w), _i = (int)(n1.y * cam->scale_h + cam->offset_h);

													if (!(_i < 0 || _i > height - 1 || _j < 0 || _j > width - 1)) {
														_index = _i * width + _j;
														if (render_proj > 0) {
															_tango[_index] = RED;// obj->color;

															//::SetPixel(memHDC, _j, _i, RED);
														}

														if (EP_GTZERO(shade[_index] - z - 1e-1)) {
															*__tango = Light3D::multi(*__image, f / 5);

															//::SetPixel(memHDC, _j, _i, *__tango);
														}

													}
													if (render_proj > 0) {
														cam = man.cams.link;
														n1.set(n0) *cam->M_1 * cam->M;
														man.cams.link->project(n1);
														_j = (int)(n1.x * cam->scale_w + cam->offset_w), _i = (int)(n1.y * cam->scale_h + cam->offset_h);

														if (!(_i < 0 || _i > height - 1 || _j < 0 || _j > width - 1)) {
															_index = _i * width + _j;
															_tango[_index] = BLUE;// obj->color;

															//::SetPixel(memHDC, _j, _i, BLUE);

														}
													}
												}


												*__depth = z;
											}
											if (render_linear < 0) {
												*__image = BLACK;
											}
										}
									}
								}
								if (render_linear < 0) {
									for (i = v->ys; i <= v->ye && i < height; i++) {
										for (j = v->xs; j <= v->xe && j < width; j++) {
											_image[i * width + j] = BLACK;
										}
									}
								}
							}
						}

						v0 = v1;
						v1 = v;
					}
					else if (v0 == NULL) {
						v0 = v;
					}
					else if (v1 == NULL) {
						v1 = v;
					}

					v = obj->verts_r.next(v);
				} while (v && v != obj->verts_r.link);
			}

			if (render_trans == 0) {
				obj = man.objs.next(obj);
				if (!(obj && obj != man.objs.link)) {
					obj = man.tras.link;
					render_trans = 1;
				}
			}
			else {
				obj = man.tras.next(obj);
				render_trans++;
				if (!(obj && obj != man.tras.link)) {

					index = 0;
					for (i = trans_h0; i <= trans_h1 && i < height; i++) {
						for (j = trans_w0; j <= trans_w1 && j < width; j++) {
							index = i * width + j;
							__trans = &_trans[index];
							if (*__trans != BLACK) {
								__tango = &_tango[index];
								*__tango = *__trans;
								*__trans = BLACK;

								//::SetPixel(memHDC, j, i, *__tango);
							}
						}
					}

					break;
				}
			}
		} while (obj && obj != man.objs.link);
	}

	int i, j, index;
	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++){
			index = j * width + i;
			if (tango[index] != BLACK) {
				::SetPixel(memHDC, i, j, _tango[index]);
			}
		}
	}
}
void onPaint(HWND hWnd) {
	if (isresize) {
		if (isresize != -1) {
			DeleteDC(memHDC);
			DeleteObject(hbmp);
			if (depth) {
				delete[] depth;
			}
			if (image) {
				delete[] image;
			}
			if (tango) {
				delete[] tango;
			}
			if (shade) {
				delete[] shade;
			}
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
		depth = new FLOAT[width * height];
		image = new DWORD[width * height];
		_image = image;
		tango = new DWORD[width * height];
		_tango = tango;
		shade = new FLOAT[width * height];
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
	memset(depth, 0, width * height * sizeof(FLOAT));
	memset(tango, 0, width * height * sizeof(DWORD));
	render(NULL, NULL, NULL);

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

	if (depth) {
		delete[] depth;
	}
	if (image) {
		delete[] image;
	}
	if (tango) {
		delete[] tango;
	}
	if (shade) {
		delete[] shade;
	}
}
void onResize(HWND hWnd) {
	isresize = 1;
	isrefresh = 1;
}

void Initialize() {

	man.addCamera(50, 50, 50, 1000, 90, 90).move(0, 0, -200);
	//man.addCamera(30, 30, 60, 600, 30, 30).move(0, 0, -100);

	//man.addLight(9, -51, -60);
	man.addLight(5, 8, 220);
	//man.addLight(-1000, 100, 100);

	int count = 2;
	int c = 30;
	int i, j, k;
	EFTYPE r = 10;
	EFTYPE x_1, x_2, r_1, r_2, p_1 = PI / ((EFTYPE)c), p_2 = 2 * PI / ((EFTYPE)c);


	man.addObject().addVert(-10, -10, 10).addVert(10, -10, 10).addVert(-10, 10, 10).addVert(10, 10, 10, -1)
		.scale(10, 10, 10).move(0, -30, -300).setColor(GREEN);
	man.addObject().renderAABB().addVert(-10, -10, 10).addVert(10, -10, 10).addVert(-10, 10, 10).addVert(10, 10, 10, -1)
		.addVert(10, 10, -10).addVert(10, -10, 10, -1).addVert(10, -10, -10).addVert(-10, -10, 10, -1).addVert(-10, -10, -10)
		.addVert(-10, 10, 10, -1).addVert(-10, 10, -10).addVert(10, 10, -10, -1).addVert(-10, -10, -10).addVert(10, -10, -10, -1)
		.scale(0.5, 0.5, 0.5).move(0, -15, -50).setColor(RED).setLineColor(BLUE);

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
	man.addObject().renderAABB().addVert(-10, -10, 10).addVert(10, -10, 10).addVert(-10, 10, 10).addVert(10, 10, 10, -1)
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
