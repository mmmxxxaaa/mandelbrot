#ifndef MANDELBROT_TESTS_H_
#define MANDELBROT_TESTS_H_

#include "mandelbrots.h"

// const char* kDefaultFilename = "benchmark_results.csv";
const int kTestIterationCount = 250;
const int kWarmupTestCount = 2;
const int kTestCount = 10;

static void MakeTestIteration(unsigned long long* total_iters);
void MakeTests(const char* filename);


#endif // MANDELBROT_TESTS_H_
