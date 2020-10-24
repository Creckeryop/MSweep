#pragma once
#include <vita2d.h>

class particle
{
public:
	int size;
	float x, y, v_x, v_y, g_x, g_y, rot, r_s;
	vita2d_texture *texture = NULL;

	particle(vita2d_texture *_texture, float _x, float _y, float _g_x, float _g_y, float _v_x, float _v_y, float _r_s, int _size);
	void Update(double dt);
	void Draw(float _x, float _y);
	void Draw(float _x, float _y, unsigned int color);
};