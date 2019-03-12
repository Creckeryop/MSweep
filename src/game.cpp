#include "game.h"
#include "screen.h"
game::game() : state(1) {
	gSoloud.init();
	jump.load("app0:/sounds/jump.wav");
	click.load("app0:/sounds/click.wav");
	boom.load("app0:/sounds/boom.wav");
	tap.load("app0:/sounds/tap.wav");
	vita2d_init();
	memset(&pad, 0, sizeof(pad));
	font = new Ffont();
	gamescreen = new game_screen(this);
	pausescreen = new pause_screen(this);
	settingscreen = new settings_screen(this);
}

void game::run() {
	vita2d_set_clear_color(RGBA8(0, 0, 0, 0xFF));
	double delta = 1;
	timer dtimer;
	while (state) {
		dtimer.Reset();
		vita2d_start_drawing();
		vita2d_clear_screen();
		gamescreen->update(delta);
		gamescreen->draw();
		settingscreen->update(delta);
		settingscreen->draw();
		pausescreen->update(delta);
		pausescreen->draw();
		vita2d_end_drawing();
		sceCtrlPeekBufferPositive(0, &pad, 1);
		old_state = state;
		switch (state) {
		case 1:
			gamescreen->control(pad, oldpad);
			break;
		case 2:
			pausescreen->control(pad, oldpad);
			break;
		case 3:
			settingscreen->control(pad, oldpad);
			break;
		default:
			4 + 4;
			break;
		}
		vita2d_swap_buffers();
		oldpad = pad;
		delta = dtimer.GetTime() / 15;
	}
}

game::~game() {
	vita2d_fini();
	delete font;
	delete gamescreen;
	delete pausescreen;
	delete settingscreen;
}