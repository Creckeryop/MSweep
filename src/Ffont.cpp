#include "Ffont.h"
Ffont::Ffont() {
	texture = vita2d_load_PNG_file("app0:assets/fixedsys.png");
}
int Ffont::portLetter(char a){
	return a - 20;
}
void Ffont::write(float x, float y, std::string str, unsigned int color) {
	for (std::string::iterator i = str.begin(); i < str.end(); i++) {
		int shift = (*i) - 32;
		vita2d_draw_texture_tint_part(texture, x, y, FONT_WIDTH * (shift % 32), FONT_HEIGHT * (shift / 32), FONT_WIDTH, FONT_HEIGHT, color);
		x += FONT_WIDTH;
	}
}
void Ffont::write(float x, float y, float size, std::string str, unsigned int color) {
	for (std::string::iterator i = str.begin(); i < str.end(); i++) {
		int shift = (*i) - 32;
		vita2d_draw_texture_tint_part_scale(texture, x, y, FONT_WIDTH * (shift % 32), FONT_HEIGHT * (shift / 32), FONT_WIDTH, FONT_HEIGHT, size, size, color);
		x += FONT_WIDTH * size;
	}
}
void Ffont::write(float x, float y, float size, std::string str, unsigned int color, int mode) {
	float width = str.length() * FONT_WIDTH * size;
	switch (mode)
	{
	case FONT_CENTERED:
		write(x - width / 2 , y - FONT_HEIGHT * size / 2.0f, size, str, color);
		break;
	case FONT_LEFTED:
		write(x, y, size, str, color);
		break;
	case FONT_RIGHTED: 
		write(x - width, y, size, str, color);
		break;
	default:
		break;
	}
	
}
Ffont::~Ffont() {
	vita2d_free_texture(texture);
}