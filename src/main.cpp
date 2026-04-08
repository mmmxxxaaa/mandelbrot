#include <TXlib.h>
#include "mandelbrots.h"
#include "painting.h"

int main()
{
#if defined(_GRAPHICS_MODE)
    printf("Graphics mode ON\n");
    txCreateWindow(kWidth, kHeight);
    txTextCursor(false);
    txBegin();
#endif // _GRAPHICS_MODE
    Win32::_fpreset();
//FIXME тоже условной компиляцией убрать передачу этого аргумента в функции
    RGBQUAD* videoBuf = txVideoMemory();
    if (!videoBuf)
        return -1;

    #if defined(_SIMPLE_VER)
        SimpleMandelbrot(videoBuf);
    #endif //_SIMPLE_VER
    #if defined(_QUAD_VER)
        QuadMandelbrot(videoBuf);
    #endif // _QUAD_VER
    #if defined(_VECTOR_VER)
        VectorMandelbrot(videoBuf);
    #endif // _VECTOR_VER

#if defined(_GRAPHICS_MODE)
    txEnd();
#endif //_GRAPHICS_MODE

    return 0;
}
