#include "tests.h"
#include "mandelbrots.h"
#include "painting.h"
#include <stdio.h>
#include <stdlib.h>
#include <x86intrin.h>   // для __rdtsc

static void MakeTestIteration(unsigned long long* total_iters)
{
    #if defined(_SIMPLE_VER)
        ComputeSimple(NULL, kXmin, kXmax, kYmin, kYmax, total_iters);
    #elif defined(_QUAD_VER)
        ComputeQuad(NULL, kXmin, kXmax, kYmin, kYmax, total_iters);
    #elif defined(_VECTOR_VER)
        ComputeVector(NULL, kXmin, kXmax, kYmin, kYmax, total_iters);
    #endif
    return;
}

void MakeTests(const char* filename)
{
    unsigned long long cycles[kTestCount] = {};

    for (int warmup_iter = 0; warmup_iter < kWarmupTestCount; warmup_iter++)
    {
        unsigned long long dummy = 0;
        for (int i = 0; i < kTestIterationCount; i++)
            MakeTestIteration(&dummy);
    }

    for (int rep = 0; rep < kTestCount; rep++)
    {
        unsigned long long total_iters = 0;
        unsigned long long start_time = _rdtsc();
        for (int i = 0; i < kTestIterationCount; i++)
            MakeTestIteration(&total_iters);
        unsigned long long end_time = _rdtsc();
        cycles[rep] = end_time - start_time;
        fprintf(stderr, "Test %d: %llu cycles\n", rep, cycles[rep]);
    }

    FILE* file_output = fopen(filename, "w");
    if (!file_output)
    {
        fprintf(stderr, "Cannot open result file");
        return;
    }
    for (int i = 0; i < kTestCount; i++)
        fprintf(file_output, "%llu\n", cycles[i]);

    fclose(file_output);
    return;
}
