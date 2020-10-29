/*this code was written by creckeryop*/
#include "game.h"
#include "screen.h"

void game::load_themes()
{
	themes.push_back(new theme("Classic", RGBA8(90, 105, 136, 255), RGBA8(192, 203, 220, 255), RGBA8(17, 9, 26, 255), RGBA8(90, 105, 136, 255), 0, 0, 0, 0, 0));
	themes.push_back(new theme("Dark Classic", RGBA8(48, 96, 130, 255), RGBA8(192, 203, 220, 255), RGBA8(17, 9, 26, 255), RGBA8(90, 105, 136, 255), 0, 0, 18, 16, 0));
	themes.push_back(new theme("Chocolate", RGBA8(39, 39, 54, 255), RGBA8(192, 203, 220, 255), RGBA8(17, 9, 26, 255), RGBA8(90, 105, 136, 255), 0, 0, 36, 32, 0));
	themes.push_back(new theme("Pinky", RGBA8(50, 62, 79, 255), RGBA8(192, 203, 220, 255), RGBA8(17, 9, 26, 255), RGBA8(90, 105, 136, 255), 0, 0, 54, 48, 0));
	themes.push_back(new theme("Motroskin", RGBA8(76, 69, 109, 255), RGBA8(192, 203, 220, 255), RGBA8(17, 9, 26, 255), RGBA8(90, 105, 136, 255), 0, 0, 72, 64, 0));

	theme *bw_theme = new theme("Black & White", RGBA8(104, 104, 104, 255), RGBA8(219, 219, 219, 255), RGBA8(0, 0, 0, 255), RGBA8(135, 135, 135, 255), 16, 16, 90, 80, 18);
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
		RGBA8(25, 25, 25, 255)};
	bw_theme->set_colors(BWColors);

	themes.push_back(bw_theme);
	themes.push_back(new theme("Realistic", RGBA8(90, 105, 136, 255), RGBA8(192, 203, 220, 255), RGBA8(17, 9, 26, 255), RGBA8(90, 105, 136, 255), 32, 0, 108, 96, 36));
	themes.push_back(new theme("OldSchool", RGBA8(0, 128, 129, 255), RGBA8(192, 192, 192, 255), RGBA8(17, 9, 26, 255), RGBA8(128, 128, 128, 255), 48, 32, 126, 112, 0));
	for (auto _ : themes)
	{
		max_theme++;
	}
}

game::game() : state(1)
{
	gSoloud.init();
	jump.load("app0:/sounds/jump.wav");
	click.load("app0:/sounds/click.wav");
	boom.load("app0:/sounds/boom.wav");
	tap.load("app0:/sounds/tap.wav");
	vita2d_init();
	memset(&pad, 0, sizeof(pad));
	font = new Ffont();
	load_themes();
	gamescreen = new game_screen(this);
	pausescreen = new pause_screen(this);
	settingscreen = new settings_screen(this);
}

void game::run()
{
	vita2d_set_clear_color(RGBA8(0, 0, 0, 0xFF));
	double delta = 1;
	timer dtimer;
	while (state)
	{
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
		switch (state)
		{
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

void game::play()
{
	gSoloud.play(click);
}

void game::jumps()
{
	gSoloud.play(jump);
}

void game::booms()
{
	gSoloud.play(boom);
}

void game::taps()
{
	gSoloud.play(tap);
}

game::~game()
{
	for (auto t : themes)
	{
		delete t;
	}
	vita2d_fini();
	delete font;
	delete gamescreen;
	delete pausescreen;
	delete settingscreen;
}