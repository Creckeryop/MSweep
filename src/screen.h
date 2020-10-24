#pragma once
#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <string.h>
#include <iostream>
#include <vector>
#include "minesweeper.h"
#include "timer.h"
#include "animation.h"
#include "particle.h"

class game;

class screen
{
public:
	float x, y;
	game *owner = NULL;
	timer t, state_change, pause, win;
	int key_pause = 300, pause_1 = 300, pause_2 = 200, pause_3 = 60, state = 0;
	vita2d_texture *screenshot = NULL;
	bool created = false;

	screen(game *owner) : owner(owner){};
	virtual void draw(){};
	virtual void update(double dt){};
	virtual void control(SceCtrlData &pad, SceCtrlData &oldpad){};
	virtual ~screen(){};
};

class game_screen : public screen
{
private:
	std::string records;
	bool first = true, is_record = false;
	int hold_x = -1, hold_y = -1;

	void open();
	bool around(int x, int y);
public:
	int Colors[10] = {
		RGBA8(0, 0, 0, 255),
		RGBA8(57, 90, 211, 255),
		RGBA8(0, 135, 81, 255),
		RGBA8(219, 0, 69, 255),
		RGBA8(126, 37, 83, 255),
		RGBA8(131, 118, 156, 255),
		RGBA8(0, 135, 81, 255),
		RGBA8(211, 99, 140, 255),
		RGBA8(69, 69, 69, 255),
		RGBA8(25, 25, 25, 255)
	};
	int BWColors[10] = {
		RGBA8(0, 0, 0, 255),
		RGBA8(94, 94, 94, 255),
		RGBA8(132, 132, 132, 255),
		RGBA8(162, 162, 162, 255),
		RGBA8(124, 124, 124, 255),
		RGBA8(155, 155, 155, 255),
		RGBA8(132, 132, 132, 255),
		RGBA8(209, 209, 209, 255),
		RGBA8(69, 69, 69, 255),
		RGBA8(25, 25, 25, 255)
	};
	float p_x = 0, p_y = 0;
	vita2d_texture *tile1 = NULL, *tile2 = NULL, *frame = NULL, *font = NULL, *wind = NULL, *mine = NULL, *buttons = NULL, *tiles = NULL, *flags = NULL;
	animation bomb = animation(vita2d_load_PNG_file("app0:assets/bomb.png"), 176, 16, 16, 50, 2, 0, 0, false);
	animation flag = animation(NULL, 64, 16, 16, 100, 2, 0, 0, true);
	animation mark = animation(vita2d_load_PNG_file("app0:assets/quest.png"), 80, 16, 16, 120, 2, 0, 0, true);
	std::vector<particle> particles;
	short frame_x, frame_y, size = 2;
	timer time, record, record_e, record_m, record_h, record_c;
	minesweeper *scene = NULL;
	bool **pole = NULL;

	game_screen(game *owner);
	void draw();
	void update(double dt);
	void control(SceCtrlData &pad, SceCtrlData &oldpad);
	void load(int width, int height, int mines);
	void load(int mode);
	void save();
	void change_theme(int theme_id);
	~game_screen();
};

class pause_screen : public screen
{
public:
	std::string states[4] = {"Continue", "New Game", "Settings", "Exit"};

	pause_screen(game *owner);
	void draw();
	void update(double dt);
	void control(SceCtrlData &pad, SceCtrlData &oldpad);
};

class settings_screen : public screen
{
public:
	int pause_3 = 0;
	std::string states[6] = {"", "Width: ", "Height: ", "Mines: ", "Style: ", "Reset Saves"};
	std::string modes[5] = {"Easy", "Medium", "Expert", "Custom"};

	settings_screen(game *owner);
	void draw();
	void update(double dt);
	void control(SceCtrlData &pad, SceCtrlData &oldpad);
};