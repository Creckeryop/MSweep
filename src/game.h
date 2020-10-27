#pragma once
#include <psp2/kernel/processmgr.h>
#include <psp2/audioout.h>
#include <psp2/ctrl.h>
#include <vita2d.h>
#include <vector>
#include "Ffont.h"
#include "soloud.h"
#include "soloud_wav.h"
#include "soloud_speech.h"
#include "theme.h"

class screen;

class game
{
private:
	void load_themes();
public:
	int state = 1, old_state = 1;
	int mode = 0;
	int theme_id = -1, max_theme = -1;
	int width = 9, height = 9, mines = 10;
	std::vector<theme*> themes;
	theme *current_theme;
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