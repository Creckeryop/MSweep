#pragma once
#include <vita2d.h>
#include "timer.h"

class animation
{
public:
	bool loop;
	int images, width, height, delta, size, dx, dy;
	timer time;
	vita2d_texture *sheet = NULL;

	animation(vita2d_texture *_sheet, int _width, int w, int h, int dt, int _size, int _dx, int _dy, bool LOOP);
	void Reset();
	void Play(float x, float y, float rot);
	~animation();
};