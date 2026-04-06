#include "painting.h"

RGBQUAD GetColor(int n_of_iter)
{
    RGBQUAD color = {0, 0, 0, 0};
    if (n_of_iter == kMaxIter)
    {
        color.rgbBlue = 0;
        color.rgbGreen = 0;
        color.rgbRed = 0;
    }
    else
    {
        int brightness = n_of_iter % 8;
        int gray = 70 + brightness * 20;
        color.rgbBlue  = (BYTE)gray;
        color.rgbGreen = (BYTE)gray;
        color.rgbRed   = (BYTE)gray;
    }
    return color;
}
