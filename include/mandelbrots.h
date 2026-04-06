#ifndef MANDELBROTS_H_
#define MANDELBROTS_H_

#include <TXLib.h>

const int   kVecWidth = 4;

void SimpleMandelbrot(RGBQUAD* videoBuf);
void QuadMandelbrot  (RGBQUAD* videoBuf);
void VectorMandelbrot(RGBQUAD* videoBuf);


#endif // MANDELBROTS_H_
