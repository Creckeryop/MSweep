#include "screen.h"
#include "game.h"
#include <math.h>
#include <vita2d.h>
#include "Ffont.h"

#define min(a, b) a < b ? a : b
#define max(a, b) a > b ? a : b
#define COLOR_BACKGROUND RGBA8(90, 105, 136, 255)
#define COLOR_UNDER_TILE RGBA8(192, 203, 220, 255)
#define COLOR_BWUNDER_TILE RGBA8(219, 219, 219, 255)
#define COLOR_BORDER RGBA8(17, 9, 26, 255)
#define COLOR_BWBORDER RGBA8(0, 0, 0, 255)
#define COLOR_UNDER_TILE_DELIMETER RGBA8(79, 103, 129, 255)
#define COLOR_TILE_SHADOW RGBA8(90, 105, 136, 255)
#define COLOR_BWTILE_SHADOW RGBA8(135, 135, 135, 255)
#define PI 3.141592

using namespace std;

int getCount(int num)
{
	return ((num /= 10) == 0) ? 1 : (1 + getCount(num));
}

string fillZero(int num, int max)
{
	string str = "";
	for (int i = getCount(num); i < max; ++i)
	{
		str += "0";
	}
	str += to_string(num);
	return str;
}

string GetTime(int t)
{
	return fillZero(t / 3600000, 3) + ":" + fillZero((t % 3600000) / 60000, 2) + ":" + fillZero((t % 60000) / 1000, 2) + "'" + fillZero(t % 1000, 3);
}

float EaseOutCubic(float t, float x, float dx, float d)
{
	if ((t /= d) >= 1)
	{
		return dx + x;
	}
	else
	{
		return ((--t) * t * t + 1) * dx + x;
	}
}

float EaseInCubic(float t, float x, float dx, float d)
{
	if ((t /= d) >= 1)
	{
		return dx + x;
	}
	else
	{
		return (t * t * t) * dx + x;
	}
}

float EaseOutBounce(float t, float x, float dx, float d)
{
	if ((t /= d) >= 1)
	{
		return dx + x;
	}
	else
	{
		return (1 - pow(2, -6 * t) * abs(cos(t * PI * 3.5))) * dx + x;
	}
}

void draw_rect_out(float x, float y, float width, float height, int thin, unsigned int color)
{
	vita2d_draw_rectangle(x, y, width, thin, color);
	vita2d_draw_rectangle(x, y + thin, thin, height - thin, color);
	vita2d_draw_rectangle(x + width - thin, y, thin, height, color);
	vita2d_draw_rectangle(x, y + height - thin, width, thin, color);
}

void change_color(vita2d_texture *texture, float x, float y, float width, float height, unsigned int color)
{
	int *data = (int *)vita2d_texture_get_datap(texture);
	int pitch = vita2d_texture_get_stride(texture) >> 2;
	for (int i = x; i < x + width; ++i)
	{
		for (int j = y; j < y + height; ++j)
		{
			data[i + j * pitch] = color;
		}
	}
}

vita2d_texture *getCopy(vita2d_texture *texture, float x, float y, float width, float height)
{
	vita2d_texture *copy = vita2d_create_empty_texture(width, height);
	int *dest = (int *)vita2d_texture_get_datap(copy), *src = (int *)vita2d_texture_get_datap(texture);
	int pitch_src = vita2d_texture_get_stride(texture) >> 2, pitch_dest = vita2d_texture_get_stride(copy) >> 2;
	for (int i = x, k = 0; i < x + width; ++i, ++k)
	{
		for (int j = y, n = 0; j < y + height; ++j, ++n)
		{
			dest[k + n * pitch_dest] = src[i + j * pitch_src];
		}
	}
	return copy;
}

vita2d_texture *make_screenshot()
{
	vita2d_texture *copy = vita2d_create_empty_texture(960, 544);
	unsigned int *buffer = (unsigned int *)vita2d_get_current_fb();
	int *data = (int *)vita2d_texture_get_datap(copy);
	for (int i = 0; i < 544 * 960; ++i)
	{
		data[i] = buffer[i];
	}
	return copy;
}

int StringToInt(string str)
{
	int num = 0;
	for (string::iterator i = str.begin(); i < str.end(); i++)
	{
		num *= 10;
		num += (*i - '0');
	}
	return num;
}

void game_screen::change_theme(int theme_id)
{
	if (theme_id > owner->max_theme)
	{
		theme_id = 0;
	}
	if (theme_id < 0)
	{
		theme_id = owner->max_theme;
	}
	vita2d_wait_rendering_done();
	vita2d_free_texture(tile1);
	vita2d_free_texture(tile2);
	vita2d_free_texture(flag.sheet);
	bomb.dy = 0;
	mark.dy = 0;
	flag.sheet = getCopy(flags, 0, 16 * theme_id, 176, 16);
	tile1 = getCopy(tiles, 0, 18 * theme_id, 18, 18);
	tile2 = getCopy(tiles, 18, 18 * theme_id, 18, 18);
	if (theme_id == 5)
	{
		bomb.dy = 16;
		mark.dy = 16;
	}
	owner->theme = theme_id;
}

void game_screen::save()
{
	sceIoRemove("ux0:data/save_msweep.txt");
	SceUID fs = sceIoOpen("ux0:data/save_msweep.txt", SCE_O_WRONLY | SCE_O_CREAT, 0777);
	if (fs >= 0)
	{
		string rec = to_string(record_e.GetTime()) + " " + to_string(record_m.GetTime()) + " " + to_string(record_h.GetTime()) + " " + to_string(record_c.GetTime()) + " " + to_string(owner->theme);
		sceIoWrite(fs, rec.c_str(), rec.length());
		sceIoClose(fs);
	}
}

game_screen::game_screen(game *owner) : screen(owner)
{
	frame_x = 0;
	frame_y = 0;
	win.Set(9999);
	y = 590;
	time = timer();
	record = timer();
	pause = timer();
	time.Pause();
	time.Reset();
	record.Pause();
	record.Reset();
	record_e.Pause();
	record_m.Pause();
	record_h.Pause();
	record_c.Pause();
	record_e.Reset();
	record_m.Reset();
	record_h.Reset();
	record_c.Reset();
	first = true;
	tiles = vita2d_load_PNG_file("app0:assets/tiles.png");
	flags = vita2d_load_PNG_file("app0:assets/flags.png");
	change_theme(0);
	frame = vita2d_load_PNG_file("app0:assets/frame.png");
	font = vita2d_load_PNG_file("app0:assets/font.png");
	mine = vita2d_load_PNG_file("app0:assets/mine.png");
	buttons = vita2d_load_PNG_file("app0:assets/buttons.png");
	SceUID fs = sceIoOpen("ux0:data/save_msweep.txt", SCE_O_RDONLY | SCE_O_CREAT, 0777);
	int size = sceIoLseek32(fs, 0, SCE_SEEK_END);
	sceIoLseek32(fs, 0, SCE_SEEK_SET);
	if (fs >= 0)
	{
		char *save = new char[256];
		sceIoRead(fs, save, size);
		records = string(save);
		sceIoClose(fs);
	}
	string now_rec = "";
	int n = 0;
	for (int i = 0; i < records.length(); ++i)
	{
		if (records[i] == ' ')
		{
			n++;
			if (n == 1)
			{
				record_e.Set(stoi(now_rec));
			}
			if (n == 2)
			{
				record_m.Set(stoi(now_rec));
			}
			if (n == 3)
			{
				record_h.Set(stoi(now_rec));
			}
			if (n == 4)
			{
				record_c.Set(stoi(now_rec));
			}
			if (n == 5)
			{
				break;
			}
			now_rec = "";
		}
		else
		{
			now_rec.push_back(records[i]);
		}
	}
	if (now_rec != "" && size != 0 && n >= 4)
	{
		change_theme(stoi(now_rec));
	}
	load(9, 9, 10);
}

void game_screen::open()
{
	int state = scene->state;
	while (first && scene->minField[frame_x][frame_y])
		scene->Remake();
	if (first)
	{
		first = false;
		time.Reset();
		time.Resume();
	}
	bool check = false;
	if (scene->actField[frame_x][frame_y] != 2)
	{
		check = true;
		for (int i = 0; i < scene->width; ++i)
		{
			for (int j = 0; j < scene->height; ++j)
			{
				pole[i][j] = scene->actField[i][j] == 1;
			}
		}
	}
	scene->Open(frame_x, frame_y);
	if (state == 0)
	{
		if (scene->state == 1)
		{
			time.Pause();
			bomb.Reset();
			owner->booms();
		}
		if (scene->state == 2)
		{
			time.Pause();
			timer *record = owner->mode == 0 ? &record_e : owner->mode == 1 ? &record_m : owner->mode == 2 ? &record_h : &record_c;
			if (record->GetTime() == 0 || record->GetTime() > time.GetTime())
			{
				record->Set(time.GetTime());
				is_record = true;
				save();
			}
		}
	}
	if (check)
	{
		bool c = false;
		float width = 17 * size;
		int y_s = min((544 - p_y) / width + 1, scene->height);
		int x_s = min((960 - p_x) / width + 1, scene->width);
		int s_x = max((0 - p_x) / width, 0);
		int s_y = max((0 - p_y) / width, 0);
		for (int i = s_x; i < x_s; ++i)
		{
			for (int j = s_y; j < y_s; ++j)
			{
				if (pole[i][j] != (scene->actField[i][j] == 1) && !scene->minField[i][j])
				{
					particles.push_back(particle(i % 2 == j % 2 ? tile1 : tile2, 17 * size * i + 9 * size, 17 * size * j + 9 * size, 0, 0.3, rand() % 7 - 3, -(rand() % 4 + 4), (rand() % 11 - 5) * 0.01f, size));
					c = true;
				}
			}
		}
		if (c)
		{
			owner->jumps();
		}
	}
}

void game_screen::control(SceCtrlData &pad, SceCtrlData &oldpad)
{
	if (pad.buttons & SCE_CTRL_LEFT || pad.buttons & SCE_CTRL_RIGHT || pad.buttons & SCE_CTRL_UP || pad.buttons & SCE_CTRL_DOWN)
	{
		if (pause.GetTime() > key_pause || key_pause == pause_1)
		{
			pause.Reset();
			key_pause = key_pause == pause_1 ? pause_2 : key_pause == pause_2 ? pause_3 : pause_3;
			if (pad.buttons & SCE_CTRL_LEFT)
			{
				if (--frame_x < 0)
				{
					frame_x = scene->width - 1;
				}
			}
			if (pad.buttons & SCE_CTRL_RIGHT)
			{
				if (++frame_x > scene->width - 1)
				{
					frame_x = 0;
				}
			}
			if (pad.buttons & SCE_CTRL_UP)
			{
				if (--frame_y < 0)
				{
					frame_y = scene->height - 1;
				}
			}
			if (pad.buttons & SCE_CTRL_DOWN)
			{
				if (++frame_y > scene->height - 1)
				{
					frame_y = 0;
				}
			}
		}
	}
	else
	{
		key_pause = 300;
	}
	if (!(pad.buttons & SCE_CTRL_CROSS) && oldpad.buttons & SCE_CTRL_CROSS)
	{
		open();
	}
	if (!(oldpad.buttons & SCE_CTRL_CROSS) && pad.buttons & SCE_CTRL_CROSS)
	{
		owner->play();
	}

	if (pad.buttons & SCE_CTRL_SQUARE && !(oldpad.buttons & SCE_CTRL_SQUARE))
	{
		scene->Mark(frame_x, frame_y);
		owner->taps();
	}
	if (pad.buttons & SCE_CTRL_CIRCLE && !(oldpad.buttons & SCE_CTRL_CIRCLE))
	{
		scene->Flag(frame_x, frame_y);
		owner->taps();
	}
	if (pad.buttons & SCE_CTRL_CROSS)
	{
		hold_x = frame_x;
		hold_y = frame_y;
	}
	else
	{
		hold_x = -1;
		hold_y = -1;
	}
	if (!(pad.buttons & SCE_CTRL_TRIANGLE) && oldpad.buttons & SCE_CTRL_TRIANGLE)
	{
		load(owner->width, owner->height, owner->mines);
	}
	if (pad.buttons & SCE_CTRL_START && !(oldpad.buttons & SCE_CTRL_START) && owner->pausescreen->y == 0)
	{
		owner->state = 2;
		vita2d_wait_rendering_done();
		screenshot = make_screenshot();
		created = true;
	}
}

bool game_screen::around(int x, int y)
{
	if (x < 0 && y < 0)
	{
		return false;
	}
	for (int k = x - 1; k < x + 2; ++k)
	{
		for (int l = y - 1; l < y + 2; ++l)
		{
			if (k >= 0 && l >= 0 && k < scene->width && l < scene->height)
			{
				if (k == hold_x && l == hold_y)
				{
					return true;
				}
			}
		}
	}
	return false;
}

void game_screen::draw()
{
	if (scene != NULL)
	{
		if (!created)
		{
			vita2d_draw_rectangle(0, 0, 960, 544, owner->Backs[owner->theme]);
			float width = 17 * size;
			int y_s = min((544 - p_y) / width + 1, scene->height);
			int x_s = min((960 - p_x) / width + 1, scene->width);
			int s_x = max((0 - p_x) / width, 0);
			int s_y = max((0 - p_y) / width, 0);
			vita2d_draw_rectangle(p_x + s_x * width - 5, p_y + s_y * width + 5 + size, width * (x_s - s_x), width * (y_s - s_y), RGBA8(0, 0, 0, 60));
			if (owner->theme == 5)
			{
				vita2d_draw_rectangle(p_x + s_x * width, p_y + s_y * width, width * (x_s - s_x), width * (y_s - s_y), COLOR_BWUNDER_TILE);
			}
			else
			{
				vita2d_draw_rectangle(p_x + s_x * width, p_y + s_y * width, width * (x_s - s_x), width * (y_s - s_y), COLOR_UNDER_TILE);
			}
			float X = p_x + size * 17 * x_s;
			for (int i = x_s - 1; i >= s_x; --i)
			{
				float Y = p_y + s_y * width;
				X -= width;
				for (int j = s_y; j < y_s; ++j)
				{
					if (scene->actField[i][j] == 1 || scene->actField[i][j] == 4)
					{
						if (i < scene->width - 1 && (scene->actField[i + 1][j] == 1 || scene->actField[i + 1][j] == 4))
						{
							if (owner->theme == 5)
							{
								vita2d_draw_rectangle(X + width, Y + size, size, width - size, COLOR_BWTILE_SHADOW);
							}
							else
							{
								vita2d_draw_rectangle(X + width, Y + size, size, width - size, COLOR_TILE_SHADOW);
							}
						}
						if (j > 0 && (scene->actField[i][j - 1] == 1 || scene->actField[i][j - 1] == 4))
						{
							if (owner->theme == 5)
							{
								vita2d_draw_rectangle(X, Y, width, size, COLOR_BWTILE_SHADOW);
							}
							else
							{
								vita2d_draw_rectangle(X, Y, width, size, COLOR_TILE_SHADOW);
							}
						}
						if (scene->numField[i][j])
						{
							unsigned int color = 0;
							if (owner->theme == 5)
							{
								color = BWColors[scene->numField[i][j]];
							}
							else
							{
								color = Colors[scene->numField[i][j]];
							}

							vita2d_draw_texture_part_tint_scale_rotate(font, X + (int)(width / 2), Y + (int)(width / 2), scene->numField[i][j] * 8, 0, 8, 9, size, size, 0, color);
						}
						if (scene->minField[i][j])
						{
							bomb.Play(X + width / 2, Y + width / 2, 0);
						}
						if (scene->actField[i][j] == 4)
						{
							if (owner->theme == 5)
							{
								vita2d_draw_texture_tint_part_scale(mine, X + 2, Y + 2, 0, 16, 16, 16, 2, 2, RGBA8(255, 255, 255, 180));
							}
							else
							{
								vita2d_draw_texture_tint_part_scale(mine, X + 2, Y + 2, 0, 0, 16, 16, 2, 2, RGBA8(255, 255, 255, 180));
							}
						}
					}
					else
					{
						vita2d_draw_texture_scale((i % 2 == j % 2) ? tile1 : tile2, X, Y, size, size);
						if (j < scene->height - 1 && (scene->actField[i][j + 1] == 1 || scene->actField[i][j + 1] == 4))
						{
							if (owner->theme == 5)
							{
								vita2d_draw_rectangle(X, Y + width + size, width, size, COLOR_BWTILE_SHADOW);
							}
							else
							{
								vita2d_draw_rectangle(X, Y + width + size, width, size, COLOR_TILE_SHADOW);
							}
						}
						if (i > 0 && (scene->actField[i - 1][j] == 1 || scene->actField[i - 1][j] == 4))
						{
							if (owner->theme == 5)
							{
								vita2d_draw_rectangle(X - size, Y + size, size, width + size - (j == scene->height - 1 ? size : 0), COLOR_BWTILE_SHADOW);
							}
							else
							{
								vita2d_draw_rectangle(X - size, Y + size, size, width + size - (j == scene->height - 1 ? size : 0), COLOR_TILE_SHADOW);
							}
						}
						if (scene->actField[i][j] == 2)
						{
							flag.Play(X + width / 2, Y + width / 2 + size / 2, 0);
						}
						else if (scene->actField[i][j] == 3)
						{
							mark.Play(X + width / 2, Y + width / 2, 0);
						}
						else if (scene->state == 0 && (hold_x >= 0 && hold_y >= 0 && scene->actField[hold_x][hold_y] == 1 && around(i, j) && scene->actField[i][j] == 0 || hold_x == i && hold_y == j))
						{
							vita2d_draw_rectangle(X, Y, width, width, RGBA8(0, 0, 0, 50));
						}
					}
					Y += width;
				}
			}
			if (owner->theme == 5)
			{
				draw_rect_out(p_x + s_x * width, p_y + s_y * width, width * (x_s - s_x) + size, width * (y_s - s_y) + size, size, COLOR_BWBORDER);
				vita2d_draw_texture_part_scale(frame, p_x + frame_x * width, p_y + frame_y * width, 0, 18, 18, 18, size, size);
			}
			else
			{
				draw_rect_out(p_x + s_x * width, p_y + s_y * width, width * (x_s - s_x) + size, width * (y_s - s_y) + size, size, COLOR_BORDER);
				vita2d_draw_texture_part_scale(frame, p_x + frame_x * width, p_y + frame_y * width, 0, 0, 18, 18, size, size);
			}
			for (vector<particle>::iterator i = particles.begin(); i < particles.end(); i++)
			{
				(*i).Draw(p_x - 5, p_y + 5, RGBA8(0, 0, 0, 60));
				(*i).Draw(p_x, p_y);
			}
			vita2d_draw_rectangle(8, 8, 300, 35, RGBA8(0, 0, 0, 255));
			if (is_record)
			{
				vita2d_draw_rectangle(8, 43, 180, 20, RGBA8(228, 59, 68, 255));
			}
			else
			{
				vita2d_draw_rectangle(8, 43, 180, 20, RGBA8(32, 32, 32, 255));
			}
			if (owner->theme == 5)
			{
				vita2d_draw_texture_part_scale(mine, 10, 72, 0, 16, 16, 16, 2, 2);
			}
			else
			{
				vita2d_draw_texture_part_scale(mine, 10, 72, 0, 0, 16, 16, 2, 2);
			}
			owner->font->write(45, 74, 2, to_string(scene->mines - scene->flags), RGBA8(0, 0, 0, 60));
			owner->font->write(48, 71, 2, to_string(scene->mines - scene->flags), RGBA8(255, 255, 255, 255));
			owner->font->write(12, 11, 2, "Time:" + GetTime(time.GetTime()), RGBA8(255, 255, 255, 255));
			timer record = owner->mode == 0 ? record_e : owner->mode == 1 ? record_m : owner->mode == 2 ? record_h : record_c;
			owner->font->write(18, 46, 1, "Record:" + GetTime(record.GetTime()), RGBA8(255, 255, 255, 255));
		}
	}
}

void game_screen::update(double dt)
{
	if (!created)
	{
		int vec_x = 480 - p_x - frame_x * 17 * size - 17 * 0.5 * size,
			vec_y = 272 - p_y - frame_y * 17 * size - 17 * 0.5 * size;
		p_x += vec_x / 8;
		p_y += vec_y / 8;
	}
	if (owner->state != 1)
	{
		if (time.isPlaying())
		{
			time.Pause();
		}
	}
	else
	{
		if (!time.isPlaying() && scene->state == 0 && !first)
		{
			time.Resume();
		}
	}
	if (owner->state == 1)
	{
		vector<vector<particle>::iterator> to_delete;
		for (vector<particle>::iterator i = particles.begin(); i < particles.end(); ++i)
		{
			(*i).Update(dt);
			if ((*i).y + p_y > 600)
			{
				to_delete.push_back(i);
			}
		}
		while (!to_delete.empty())
		{
			particles.erase(to_delete.back());
			to_delete.pop_back();
		}
	}
	if (owner->pausescreen->y == 0 && owner->state == 1)
	{
		vita2d_wait_rendering_done();
		vita2d_free_texture(owner->pausescreen->screenshot);
		owner->pausescreen->screenshot = NULL;
		owner->pausescreen->created = false;
	}
}

void game_screen::load(int width, int height, int mines)
{
	if (scene != NULL)
	{
		for (int i = 0; i < scene->width; ++i)
		{
			delete[] pole[i];
		}
		delete[] pole;
		delete scene;
	}
	scene = new minesweeper(width, height, mines);
	pole = new bool *[scene->width];
	for (int i = 0; i < scene->width; ++i)
	{
		pole[i] = new bool[scene->height];
		for (int j = 0; j < scene->height; ++j)
		{
			pole[i][j] = false;
		}
	}
	first = true;
	is_record = false;
	time.Pause();
	time.Reset();
	frame_x = min(frame_x, scene->width - 1);
	frame_y = min(frame_y, scene->height - 1);
	particles.clear();
}

void game_screen::load(int mode)
{
	switch (mode)
	{
	case 0:
		load(9, 9, 10);
		break;
	case 1:
		load(16, 16, 40);
		break;
	case 2:
		load(30, 16, 99);
		break;
	case 3:
		load(30, 24, 200);
		break;
	default:
		break;
	}
}

game_screen::~game_screen()
{
	vita2d_free_texture(tile1);
	vita2d_free_texture(tile2);
	vita2d_free_texture(tiles);
	vita2d_free_texture(flags);
	vita2d_free_texture(frame);
	vita2d_free_texture(mine);
	vita2d_free_texture(font);
	vita2d_free_texture(buttons);
	if (scene != NULL)
	{
		for (int i = 0; i < scene->width; ++i)
		{
			delete[] pole[i];
		}
		delete[] pole;
		delete scene;
	}
}

pause_screen::pause_screen(game *owner) : screen(owner)
{
	x = 0;
	y = 426;
	t.Set(99999);
	state_change.Set(99999);
}

void pause_screen::draw()
{
	if (y != 0 || owner->state == 2)
	{
		if (owner->gamescreen->created)
		{
			owner->font->write(480, 80, 5, "MSweep", RGBA8(255, 255, 255, 255), FONT_CENTERED);
			for (int i = 0, y = 32; i < 4; ++i, y += 56)
			{
				owner->font->write(480 + owner->settingscreen->x / 3, 272 + y, 3, states[i], state == i ? RGBA8(255, 255, 255, 255) : RGBA8(100, 100, 100, 255), FONT_CENTERED);
			}
			owner->font->write(10, 522, 1, "v 1.2", RGBA8(255, 255, 255, 255), FONT_LEFTED);
			owner->font->write(950, 522, 1, "creckeryop 2019 - 2020", RGBA8(255, 255, 255, 255), FONT_RIGHTED);
			vita2d_draw_texture_part(owner->gamescreen->screenshot, 0, -y, 0, 0, 960, 272);
			vita2d_draw_texture_part(owner->gamescreen->screenshot, 0, 272 + y, 0, 272, 960, 272);
		}
		if (created)
		{
			vita2d_draw_texture_part(screenshot, 0, y - 272, 0, 0, 960, 272);
			vita2d_draw_texture_part(screenshot, 0, 544 - y, 0, 272, 960, 272);
		}
	}
}

void pause_screen::update(double dt)
{
	if (owner->old_state < 2 && owner->state == 2)
	{
		t.Reset();
	}
	if (owner->old_state == 2 && owner->state < 2)
	{
		t.Reset();
	}
	if (owner->state >= 2)
	{
		y = EaseOutCubic(t.GetTime(), 0, 272, 400);
	}
	else
	{
		y = EaseInCubic(t.GetTime(), 272, -272, 400);
	}
}

void pause_screen::control(SceCtrlData &pad, SceCtrlData &oldpad)
{
	if (pad.buttons & SCE_CTRL_UP && !(oldpad.buttons & SCE_CTRL_UP))
	{
		if (--state < 0)
		{
			state = 3;
		}
	}
	if (pad.buttons & SCE_CTRL_DOWN && !(oldpad.buttons & SCE_CTRL_DOWN))
	{
		if (++state >= 4)
		{
			state = 0;
		}
	}
	if (owner->settingscreen->x == 0 && y == 272)
	{
		if (!(pad.buttons & SCE_CTRL_CROSS) && oldpad.buttons & SCE_CTRL_CROSS)
		{
			switch (state)
			{
			case 0:
				owner->state = 1;
				break;
			case 1:
				((game_screen *)owner->gamescreen)->load(owner->width, owner->height, owner->mines);
				owner->state = 1;
				break;
			case 2:
				owner->state = 3;
				break;
			case 3:
				owner->state = 0;
				break;
			}
		}
		if (pad.buttons & SCE_CTRL_START && !(oldpad.buttons & SCE_CTRL_START) || pad.buttons & SCE_CTRL_CIRCLE && !(oldpad.buttons & SCE_CTRL_CIRCLE))
		{
			owner->state = 1;
		}
		if (owner->state < 2)
		{
			owner->gamescreen->created = false;
			vita2d_wait_rendering_done();
			vita2d_free_texture(owner->gamescreen->screenshot);
			created = true;
			screenshot = make_screenshot();
		}
	}
}

settings_screen::settings_screen(game *owner) : screen(owner)
{
	x = 0;
	y = 426;
	t.Set(99999);
	state_change.Set(99999);
};

void settings_screen::draw()
{
	if (x != 0)
	{
		if (owner->gamescreen->created)
		{
			for (int i = 0, y = 0; i < 6; ++i, y += 46)
			{
				string now = "<" + modes[owner->mode] + ">";
				if (i == 1)
				{
					now = states[i] + "<" + fillZero(owner->width, 2) + ">";
				}
				else if (i == 2)
				{
					now = states[i] + "<" + fillZero(owner->height, 2) + ">";
				}
				else if (i == 3)
				{
					now = states[i] + "<" + fillZero(owner->mines, 4) + ">";
				}
				else if (i == 4)
				{
					now = states[i] + "<" + to_string(owner->theme) + ">";
				}
				else if (i == 5)
				{
					now = states[i];
				}
				owner->font->write(1200 + x, 544 / 2 - 50 + y, 2, now, state == i ? RGBA8(255, 255, 255, 255) : RGBA8(100, 100, 100, 255), FONT_CENTERED);
			}
		}
		vita2d_draw_rectangle(1400 + x, 544 / 2 - 50, 120, 100, owner->Backs[owner->theme]);
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				vita2d_draw_texture((i % 2 == j % 2) ? ((game_screen *)owner->gamescreen)->tile1 : ((game_screen *)owner->gamescreen)->tile2, 1425 + x + i * 17, 544 / 2 - 55 + 20 + 17 * j);
			}
		}
		((game_screen *)owner->gamescreen)->flag.Play(1430 + x, 544 / 2 - 25, 0);
	}
}

void settings_screen::update(double dt)
{
	if (owner->old_state != 3 && owner->state == 3)
	{
		t.Reset();
	}
	if (owner->old_state == 3 && owner->state != 3)
	{
		t.Reset();
	}
	if (owner->state == 3)
	{
		x = EaseOutCubic(t.GetTime(), 0, -560, 350);
	}
	else
	{
		x = EaseInCubic(t.GetTime(), -560, 560, 350);
	}
}

void settings_screen::control(SceCtrlData &pad, SceCtrlData &oldpad)
{
	if (pad.buttons & SCE_CTRL_UP && !(oldpad.buttons & SCE_CTRL_UP))
	{
		if (--state < 0)
		{
			state = 5;
		}
	}
	if (pad.buttons & SCE_CTRL_DOWN && !(oldpad.buttons & SCE_CTRL_DOWN))
	{
		if (++state > 5)
		{
			state = 0;
		}
	}
	int tmp_mode = owner->mode;
	int tmp_w = owner->width, tmp_h = owner->height, tmp_m = owner->mines;
	if (pad.buttons & SCE_CTRL_LEFT || pad.buttons & SCE_CTRL_RIGHT)
	{
		if (pause.GetTime() > key_pause || key_pause == pause_1)
		{
			pause.Reset();
			if (state == 0)
			{
				key_pause = pause_2;
			}
			else
			{
				key_pause = key_pause == pause_1 ? pause_2 : key_pause == pause_2 ? pause_3 : pause_3;
			}

			if (pad.buttons & SCE_CTRL_RIGHT)
			{
				switch (state)
				{
				case 0:
					if (++owner->mode >= 4)
					{
						owner->mode = 0;
					}
					break;
				case 1:
					if (++owner->width > 40)
					{
						owner->width = 40;
					}
					break;
				case 2:
					if (++owner->height > 40)
					{
						owner->height = 40;
					}
					break;
				case 3:
					if (++owner->mines > 9999)
					{
						owner->mines = 9999;
					}
					break;
				case 4:
					((game_screen *)owner->gamescreen)->change_theme(owner->theme + 1);
					((game_screen *)owner->gamescreen)->save();
					break;
				default:
					break;
				}
			}
			if (pad.buttons & SCE_CTRL_LEFT)
			{
				switch (state)
				{
				case 0:
					if (--owner->mode < 0)
					{
						owner->mode = 3;
					}
					break;
				case 1:
					if (--owner->width < 5)
					{
						owner->width = 5;
					}
					break;
				case 2:
					if (--owner->height < 5)
					{
						owner->height = 5;
					}
					break;
				case 3:
					if (--owner->mines < 5)
					{
						owner->mines = 5;
					}
					break;
				case 4:
					((game_screen *)owner->gamescreen)->change_theme(owner->theme - 1);
					((game_screen *)owner->gamescreen)->save();
					break;
				default:
					break;
				}
			}
		}
	}
	else
	{
		key_pause = 300;
	}
	if (pad.buttons & SCE_CTRL_CROSS && !(oldpad.buttons & SCE_CTRL_CROSS))
	{
		if (state == 5)
		{
			((game_screen *)owner->gamescreen)->record_e.Reset();
			((game_screen *)owner->gamescreen)->record_m.Reset();
			((game_screen *)owner->gamescreen)->record_h.Reset();
			((game_screen *)owner->gamescreen)->record_c.Reset();
			((game_screen *)owner->gamescreen)->save();
		}
	}
	if (tmp_w != owner->width || tmp_h != owner->height || tmp_m != owner->mines)
	{
		owner->mode = 3;
		if (owner->mines >= owner->width * owner->height - 2)
		{
			owner->mines = owner->width * owner->height - 2;
		}
	}
	if (tmp_mode != owner->mode)
	{
		if (owner->mode == 0)
		{
			owner->width = 9;
			owner->height = 9;
			owner->mines = 10;
		}
		if (owner->mode == 1)
		{
			owner->width = 16;
			owner->height = 16;
			owner->mines = 40;
		}
		if (owner->mode == 2)
		{
			owner->width = 30;
			owner->height = 16;
			owner->mines = 99;
		}
	}
	if (pad.buttons & SCE_CTRL_START && !(oldpad.buttons & SCE_CTRL_START) || pad.buttons & SCE_CTRL_CIRCLE && !(oldpad.buttons & SCE_CTRL_CIRCLE))
	{
		owner->state = 2;
	}
}
