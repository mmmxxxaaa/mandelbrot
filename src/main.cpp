#include <TXlib.h>
#include "mandelbrots.h"
#include "painting.h"

int main()
{
#if defined(_GRAPHICS_MODE)
    txCreateWindow(kWidth, kHeight);
    txTextCursor(false);
#endif //_GRAPHICS_MODE

    Win32::_fpreset();

#if defined(_GRAPHICS_MODE)
    RGBQUAD* videoBuf = txVideoMemory();
    if (!videoBuf)
        return -1;
#else
    RGBQUAD* videoBuf = (RGBQUAD*)calloc(1, sizeof(RGBQUAD));
#endif //_GRAPHICS_MODE

    #if defined(_SIMPLE_VER)
        SimpleMandelbrot(videoBuf);
    #endif //_SIMPLE_VER
    #if defined(_QUAD_VER)
        QuadMandelbrot(videoBuf);
    #endif // _QUAD_VER
    #if defined(_VECTOR_VER)
        VectorMandelbrot(videoBuf);
    #endif // _VECTOR_VER

    return 0;
}
