#pragma once
#include <vita2d.h>
#include <iostream>
#define FONT_WIDTH 8
#define FONT_HEIGHT 16
#define FONT_CENTERED 1
#define FONT_LEFTED 2
#define FONT_RIGHTED 3
class Ffont {
public:
	vita2d_texture *texture = NULL;
	Ffont();
	int portLetter(char a);
	void write(float x, float y, string str, unsigned int color);
	void write(float x, float y, float size, std::string str, unsigned int color);
	void write(float x, float y, float size, std::string str, unsigned int color, int mode);
	~Ffont();
};