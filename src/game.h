#pragma once
#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h>
#include <vita2d.h>
#include "Ffont.h"
class screen;
class game {
public:
	int state = 1, old_state = 1;
	int mode = 0;
	int width = 9, height = 9, mines = 10;
	Ffont *font = 0;
	SceCtrlData pad, oldpad;
	screen *gamescreen = 0, *pausescreen = 0, *settingscreen = 0;
	game();
	void run();
	~game();
};