//Device.h
//

#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "math3d/Manager3D.h"

struct Device {
	INT width;
	INT height;

	FLOAT *depth;//Depth test buffer
	DWORD *image;//Image buffer
	DWORD *tango;//Target buffer
	FLOAT *shade;//Shade buffer

	Graphics * graphics;
	Gdiplus::Pen pen;

	INT draw_line = 1;
	INT render_linear = 1;
	INT render_proj = -1;
	INT render_light = -1;

	EPoint e, s, v, c;
	Vert3D p;
	ege_colpoint cps[3];
	Vert3D n, n_1, n_2, n0, n1, n2, r;
	DWORD * _tango, *_image, *_trans, *_mirror;
	DWORD * __image, *__tango, *__trans, *__mirror;
	FLOAT *__depth;
	DWORD ___image, ___last = 0;

	Device() :
		width(100),
		height(100),
		depth(NULL),
		image(NULL),
		tango(NULL),
		shade(NULL),
		pen((Color(0, 0, 0))){

	}
	Device(INT w, INT h) :
		pen((Color(0, 0, 0))){
		Resize(w, h);
	}
	~Device() {
		Destory();
	}

	void Destory() {
		if (depth) {
			delete[] depth;
			depth = NULL;
		}
		if (image) {
			delete[] image;
			image = NULL;
		}
		if (tango) {
			delete[] tango;
			tango = NULL;
		}
		if (shade) {
			delete[] shade;
			shade = NULL;
		}
	}

	void Resize(INT w, INT h)  {
		Destory();
		width = w;
		height = h;

		depth = new FLOAT[width * height];
		image = new DWORD[width * height];
		tango = new DWORD[width * height];
		shade = new FLOAT[width * height];
		_image = image;
		_tango = tango;
	}

	void Render(Manager3D& man, VObj * range, VObj * range0, VObj * range1) {

		memset(depth, 0, width * height * sizeof(FLOAT));
		memset(tango, 0, width * height * sizeof(DWORD));
		memset(image, 0, width * height * sizeof(DWORD));
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
			do {
				v = obj->verts_r.link;
				if (v && obj->verts_r.linkcount > 0) {
					v0 = NULL; v1 = NULL;
					do {
						//there must be three points
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
											//pen.SetColor(Color(255, 255, 255));
											//graphics->DrawLine(&pen, v0->x0, v0->y0, v1->x0, v1->y0);
											//graphics->DrawLine(&pen, v1->x0, v1->y0, v->x0, v->y0);
											//graphics->DrawLine(&pen, v->x0, v->y0, v0->x0, v0->y0);
											Draw_Line(_image, width, height, v0->x0, v0->y0, v1->x0, v1->y0, WHITE);
											Draw_Line(_image, width, height, v1->x0, v1->y0, v->x0, v->y0, WHITE);
											Draw_Line(_image, width, height, v->x0, v->y0, v0->x0, v0->y0, WHITE);
											//ege::setcolor(WHITE);
											//ege::line(v0->x0, v0->y0, v1->x0, v1->y0, image);
											//ege::line(v1->x0, v1->y0, v->x0, v->y0, image);
											//ege::line(v->x0, v->y0, v0->x0, v0->y0, image);
										}
									}
									else {
										__image = &___image;
									}

									//step1: render the triangle
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
											//step2: depth test
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
														// setteled value 16579836
														// when painted in ege::fillpoly_gradient
														if (*__image == 11053224) {
															*__image = obj->line;
															//*__image = obj->color;
														}
														else {
															*__image = obj->color;
														}
													}
													else {
														//nothing todo
													}

													//step3: render light
													cam = obj->cam;
													if (cam) {
														n0.set((j - cam->offset_w) / cam->scale_w, (i - cam->offset_h) / cam->scale_h, z, 1);
														// get position
														//(-n_1.x * (n0.x - v->x) - n_1.y * (n0.y - v->y)) / n_1.z + v->zz;
														zz = Vert3D::getZ(v->n_z, v->x, v->y, v->zz, n0.x, n0.y);
														man.cams.link->anti_normalize(n0, zz);

														lgt = man.lgts.link;
														f = 0;
														if (lgt) {
															do {
																f += lgt->getFactor(v->n_r, n0);

																if (render_light < 0) {
																	break;
																}

																lgt = man.lgts.next(lgt);
															} while (lgt && lgt != man.lgts.link);
														}

														//step4: render transparent
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
														}

														//step5: get shadow map
														cam = man.cams.link;
														lgt = man.lgts.link;
														n1.set(n0) *cam->M_1 * lgt->M_1;
														man.cams.link->project(n1);
														_j = (int)(n1.x * cam->scale_w + cam->offset_w), _i = (int)(n1.y * cam->scale_h + cam->offset_h);

														if (!(_i < 0 || _i > height - 1 || _j < 0 || _j > width - 1)) {
															_index = _i * width + _j;
															if (render_proj > 0) {
																_tango[_index] = RED;// obj->color;
															}

															if (EP_GTZERO(shade[_index] - z - 1e-1)) {
																*__tango = Light3D::multi(*__image, f / 5);
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
								}
							}
						}

						break;
					}
				}
			} while (obj && obj != man.objs.link);
		}
	}

	int Draw_Line(DWORD* vb_start, int lpitch, int height,
		int x1, int y1, // ��ʼ��
		int x2, int y2, // �յ�
		unsigned char color // ��ɫ����
		) // video buffer and memory pitch
	{
		// this function draws a line from xo,yo to x1,y1 using differential error
		// terms (based on Bresenahams work)

		int dx, // difference in x's
			dy, // difference in y's
			dx2, // dx,dy * 2
			dy2,
			x_inc, // amount in pixel space to move during drawing
			y_inc, // amount in pixel space to move during drawing
			error, // the discriminant i.e. error i.e. decision variable
			index; // used for looping

		// pre-compute first pixel address in video buffer
		vb_start = vb_start + x1 + y1 * lpitch;

		// compute horizontal and vertical deltas
		dx = x2 - x1;
		dy = y2 - y1;

		// test which direction the line is going in i.e. slope angle
		if (dx >= 0)
		{
			x_inc = 1;

		} // end if line is moving right
		else
		{
			x_inc = -1;
			dx = -dx; // need absolute value

		} // end else moving left

		// test y component of slope

		if (dy >= 0)
		{
			y_inc = lpitch;
		} // end if line is moving down
		else
		{
			y_inc = -lpitch;
			dy = -dy; // need absolute value

		} // end else moving up

		// compute (dx,dy) * 2
		dx2 = dx << 1;
		dy2 = dy << 1;

		// now based on which delta is greater we can draw the line
		if (dx > dy)     //б��С��1�����
		{
			// initialize error term
			error = dy2 - dx;

			// draw the line
			for (index = 0; index <= dx; index++)
			{
				// set the pixel
				*vb_start = color;

				// test if error has overflowed
				if (error >= 0)
				{
					error -= dx2;
					// move to next line
					vb_start += y_inc;

				}
				// adjust the error term
				error += dy2;
				// move to the next pixel
				vb_start += x_inc;
			}
		}
		else   //б�ʴ��ڵ���1�����
		{
			// initialize error term
			error = dx2 - dy;

			for (index = 0; index <= dy; index++)
			{
				// set the pixel
				*vb_start = color;

				// test if error overflowed
				if (error >= 0)
				{
					error -= dy2;//���ﰴ�����Ƶ������Ϊerror+=dx2-dy2;

					// move to next line
					vb_start += x_inc;

				} // end if error overflowed

				// adjust the error term
				error += dx2;

				// move to the next pixel
				vb_start += y_inc;
			}
		}
		return(1);

	} // end Draw_Line

};



#endif//_DEVICE_H_