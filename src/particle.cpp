#include "particle.h"
particle::particle(vita2d_texture *_texture, float _x, float _y, float _g_x, float _g_y, float _v_x, float _v_y, float _r_s, int _size) {
	x = _x;
	y = _y;
	g_x = _g_x;
	g_y = _g_y;
	v_x = _v_x;
	v_y = _v_y;
	r_s = _r_s;
	size = _size;
	texture = _texture;
	rot = 0;
}
void particle::Draw(float _x, float _y) {
	if (texture != NULL) {
		vita2d_draw_texture_scale_rotate(texture, x + _x, y + _y, size, size, rot);
	}
}
void particle::Draw(float _x, float _y, unsigned int color) {
	if (texture != NULL) {
		vita2d_draw_texture_tint_scale_rotate(texture, x + _x, y + _y, size, size, rot, color);
	}
}
void particle::Update(double dt) {
	x += v_x*dt;
	y += v_y*dt;
	v_x += g_x*dt;
	v_y += g_y*dt;
	rot += r_s*dt;
}