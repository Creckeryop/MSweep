#pragma once
#include <vita2d.h>

class theme
{
public:
    unsigned int background_color;
    unsigned int undertile_color;
    unsigned int border_color;
    unsigned int shadow_color;
    int bomb_start_y;
    int mark_start_y;
    int tile_start_y;
    int flag_start_y;
    int frame_start_y;
    int Colors[10];
    theme(unsigned int bck_c, unsigned int und_c, unsigned int brd_c, unsigned int shd_c, int b_y, int m_y, int t_y, int f_y, int frame_y);
    void set_colors(int *colors);
};