#include <TXlib.h>
#include "mandelbrots.h"
#include "painting.h"

int main()
{
#if defined(_GRAPHICS_MODE)
    txCreateWindow(kWidth, kHeight);
    txTextCursor(false);
    RGBQUAD* videoBuf = txVideoMemory();
    if (!videoBuf)
        return -1;
#else
    RGBQUAD* videoBuf = (RGBQUAD*)calloc(kWidth * kHeight, sizeof(RGBQUAD));
    if (!videoBuf)
        return -1;
#endif //_GRAPHICS_MODE

    Win32::_fpreset();
    ProcessMandelbrot(videoBuf);

#if !defined(_GRAPHICS_MODE)
    free(videoBuf);
#endif

    return 0;
}
