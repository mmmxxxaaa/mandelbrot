#ifndef MANDELBROTS_H_
#define MANDELBROTS_H_

#include <TXLib.h>

const int   kPerformanceIters = 2000;

void ComputeSimple(RGBQUAD* video_buf, float xmin, float xmax,
                   float ymin, float ymax, unsigned long long* total_iters);
void ComputeQuad  (RGBQUAD* video_buf, float xmin, float xmax,
                   float ymin, float ymax, unsigned long long* total_iters);
void ComputeVector(RGBQUAD* video_buf, float xmin, float xmax,
                   float ymin, float ymax, unsigned long long* total_iters);

void ProcessMandelbrot(RGBQUAD* videoBuf);


#endif // MANDELBROTS_H_
