#ifndef PAINTING_H_
#define PAINTING_H_
#include <TXLib.h>

const int   kWidth   = 800;
const int   kHeight  = 600;
const float kXmin = -2.2f;
const float kXmax = 1.0f;
const float kYmin = -1.2f;
const float kYmax =  1.2f;

const int   kMaxIter = 256;
const float kLmax = 2.0f;

RGBQUAD GetColor(int n_of_iter);

#endif // PAINTING_H_
