#include <TXlib.h>
#include <math.h>


#define WIDTH  800
#define HEIGHT 600

#define X_MIN   -2.2
#define X_MAX    1
#define Y_MIN   -1.2
#define Y_MAX    1.2

#define MAX_ITER 256
#define L_MAX 2.0f

COLORREF GetColor(int n_of_iter)
{
    if (n_of_iter == MAX_ITER)
    {
        return RGB(0, 0, 0);
    }
    else
    {
        int brightness = n_of_iter % 8;
        int gray = 70 + brightness * 20;

        return RGB(gray, gray, gray);
    }
}

int main()
{
    txCreateWindow(WIDTH, HEIGHT);
    txTextCursor(false);
    txBegin();

    RGBQUAD* videoBuf = txVideoMemory();
    if (!videoBuf)
        return -1;

    float x_range = X_MAX - X_MIN;
    float y_range = Y_MAX - Y_MIN;

    for (int y = 0; y < HEIGHT; y++)
    {
        float p0_im = Y_MAX - (float)y / HEIGHT * y_range;

        for (int x = 0; x < WIDTH; x++)
        {
            float p0_re = X_MIN + (float)x / WIDTH  * x_range;
            float p_re = 0.0;
            float p_im = 0.0;
            int n_of_iter = 0;

            while (n_of_iter < MAX_ITER)
            {
                if (p_re * p_re + p_im * p_im > L_MAX * L_MAX)
                    break;

                float p_re_new = p_re * p_re - p_im * p_im + p0_re;
                float p_im_new = 2.0 * p_re * p_im + p0_im;
                p_re = p_re_new;
                p_im = p_im_new;
                n_of_iter++;
            }

            COLORREF color  = GetColor(n_of_iter);
            RGBQUAD* pixel  = &videoBuf[y * WIDTH + x];
            pixel->rgbRed   = GetRValue(color);
            pixel->rgbGreen = GetGValue(color);
            pixel->rgbBlue  = GetBValue(color);
            pixel->rgbReserved = 0;
        }
    }
    txEnd();

    return 0;
}
