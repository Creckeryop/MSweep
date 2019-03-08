#pragma once
#include <vita2d.h>
#include <iostream>
#include "timer.h"
class animation {
public:
	int images, width, height, delta, size;
	bool loop;
	vita2d_texture *sheet = NULL;
	timer time;
	animation(vita2d_texture *_sheet, int w, int h, int dt, int _size, bool LOOP) {
		sheet = _sheet;
		width = w;
		height = h;
		delta = dt;
		size = _size;
		loop = LOOP;
		images = vita2d_texture_get_width(sheet) / w;
		time.Reset();
	}
	void Reset() {
		time.Reset();
	}
	void Play(float x, float y, float rot) {
		int card = time.GetTime() / delta;
		if (card >= images || card < 0) {
			if (loop) {
				card = 0;
				Reset();
			}
			else {
				card = images - 1;
			}
		}
		vita2d_draw_texture_part_scale_rotate(sheet, x, y, card * width, 0, width, height, size, size, rot);
	}
	~animation() {
		delete sheet;
	}
};