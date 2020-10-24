#pragma once
#include <psp2/kernel/processmgr.h>
#include <psp2/audioout.h>
#include <psp2/ctrl.h>
#include <vita2d.h>
#include "Ffont.h"
#include "soloud.h"
#include "soloud_wav.h"
#include "soloud_speech.h"

class screen;

class game
{
public:
	int state = 1, old_state = 1;
	int mode = 0;
	int theme = -1, max_theme = 5;
	int width = 9, height = 9, mines = 10;
	int Backs[6] = {RGBA8(90, 105, 136, 255), RGBA8(48, 96, 130, 255), RGBA8(39, 39, 54, 255), RGBA8(50, 62, 79, 255), RGBA8(76, 69, 109, 255), RGBA8(104, 104, 104, 255)};
	SoLoud::Soloud gSoloud;
	SoLoud::Wav click;
	SoLoud::Wav jump;
	SoLoud::Wav boom;
	SoLoud::Wav tap;
	Ffont *font = 0;
	SceCtrlData pad, oldpad;
	screen *gamescreen = 0, *pausescreen = 0, *settingscreen = 0;

	game();
	void run();
	void play();
	void jumps();
	void booms();
	void taps();
	~game();
};