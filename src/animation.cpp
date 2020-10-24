#include "animation.h"

animation::animation(vita2d_texture *_sheet, int _width, int w, int h, int dt, int _size, int _dx, int _dy, bool LOOP)
{
    sheet = _sheet;
    width = w;
    height = h;
    delta = dt;
    size = _size;
    loop = LOOP;
    images = _width / w;
    dx = _dx;
    dy = _dy;
    time.Reset();
}

void animation::Reset()
{
    time.Reset();
}

void animation::Play(float x, float y, float rot)
{
    int card = time.GetTime() / delta;
    if (card >= images || card < 0)
    {
        if (loop)
        {
            card = 0;
            Reset();
        }
        else
        {
            card = images - 1;
        }
    }
    if (sheet != NULL)
    {
        vita2d_draw_texture_part_scale_rotate(sheet, x, y, dx + card * width, dy, width, height, size, size, rot);
    }
}

animation::~animation()
{
    delete sheet;
}