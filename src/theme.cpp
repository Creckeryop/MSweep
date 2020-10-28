#include "theme.h"

void theme::set_colors(int *colors)
{
    for(int i = 0; i < 10; ++i)
    {
        Colors[i] = colors[i];
    }
}

theme::theme(std::string _name, unsigned int bck_c, unsigned int und_c, unsigned int brd_c, unsigned int shd_c, int b_y, int m_y, int t_y, int f_y, int frame_y)
{
    name = _name;
    background_color = bck_c;
    undertile_color = und_c;
    border_color = brd_c;
    shadow_color = shd_c;
    bomb_start_y = b_y;
    mark_start_y = m_y;
    tile_start_y = t_y; 
    flag_start_y = f_y;
    frame_start_y = frame_y;
    int _Colors[10] = {
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
    set_colors(_Colors);
}