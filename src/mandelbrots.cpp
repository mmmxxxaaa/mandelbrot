#include "mandelbrots.h"
#include <math.h>
#include <immintrin.h>
#include <assert.h>
#include "painting.h"

#include <stdio.h>

static void ComputeSimple(RGBQUAD* video_buf, float xmin,  float xmax, float ymin, float ymax,
                          unsigned long long* total_iters)
{
    // assert(video_buf != NULL);
    float x_range = xmax - xmin;
    float y_range = ymax - ymin;

#ifndef _GRAPHICS_MODE
    unsigned long long local_counter = 0;
#endif
    for (int y = 0; y < kHeight; y++)
    {
        float p0_im = ymax - (float)y / kHeight * y_range;
        for (int x = 0; x < kWidth; x++)
        {
            float p0_re = xmin + (float)x / kWidth * x_range;
            float p_re = 0.0f, p_im = 0.0f;
            int n_of_iter = 0;
            while (n_of_iter < kMaxIter)
            {
                if (p_re * p_re + p_im * p_im > kLmax * kLmax)
                    break;
                float p_re_new = p_re * p_re - p_im * p_im + p0_re;
                float p_im_new = 2.0f * p_re * p_im + p0_im;
                p_re = p_re_new;
                p_im = p_im_new;
                n_of_iter++;
            }
#ifdef _GRAPHICS_MODE
            video_buf[y * kWidth + x] = GetColor(n_of_iter);
#else
            local_counter += n_of_iter;
#endif
        }
    }
#ifndef _GRAPHICS_MODE
    *total_iters += local_counter;
#endif
}

static void ComputeQuad(RGBQUAD* video_buf, float xmin, float xmax, float ymin, float ymax,
                 unsigned long long* total_iters)
{
    // assert(video_buf != NULL);

    float x_range = xmax - xmin;
    float y_range = ymax - ymin;
    float dx = x_range / kWidth;

#ifndef _GRAPHICS_MODE
    unsigned long long local = 0;
#endif
    for (int y = 0; y < kHeight; ++y)
    {
        float y0 = ymax - (float)y / kHeight * y_range;
        for (int x = 0; x < kWidth; x += kVecWidth)
        {
            float x0 = xmin + (float)x / kWidth * x_range;
            float x0_arr[kVecWidth] = {};
            for (int i = 0; i < kVecWidth; ++i) x0_arr[i] = x0 + i * dx;

            float y0_arr[kVecWidth] = {};
            for (int i = 0; i < kVecWidth; ++i) y0_arr[i] = y0;

            float x_arr[kVecWidth] = {}; for (int i = 0; i < kVecWidth; ++i) x_arr[i] = x0_arr[i];
            float y_arr[kVecWidth] = {}; for (int i = 0; i < kVecWidth; ++i) y_arr[i] = y0_arr[i];

            int n_of_iters[kVecWidth] = {};
            for (int n = 0; n < kMaxIter; ++n)
            {
                float x2[kVecWidth] = {}; for (int i = 0; i < kVecWidth; ++i) x2[i] = x_arr[i] * x_arr[i];
                float y2[kVecWidth] = {}; for (int i = 0; i < kVecWidth; ++i) y2[i] = y_arr[i] * y_arr[i];
                float xy[kVecWidth] = {}; for (int i = 0; i < kVecWidth; ++i) xy[i] = x_arr[i] * y_arr[i];
                float r2[kVecWidth] = {}; for (int i = 0; i < kVecWidth; ++i) r2[i] = x2[i] + y2[i];

                int cmp[kVecWidth] = {0};
                for (int i = 0; i < kVecWidth; ++i)
                    if (r2[i] <= kLmax * kLmax)
                        cmp[i] = 1;

                int mask = 0;
                for (int i = 0; i < kVecWidth; ++i) mask |= cmp[i] << i;
                if (!mask) break;
                for (int i = 0; i < kVecWidth; ++i) x_arr[i] = x2[i] - y2[i] + x0_arr[i];
                for (int i = 0; i < kVecWidth; ++i) y_arr[i] = xy[i] + xy[i] + y0_arr[i];
                for (int i = 0; i < kVecWidth; ++i) n_of_iters[i] += cmp[i];
            }
            for (int i = 0; i < kVecWidth; ++i)
            {
#ifdef _GRAPHICS_MODE
                video_buf[y * kWidth + (x + i)] = GetColor(n_of_iters[i]);
#else
                local += n_of_iters[i];
#endif
            }
        }
    }
#ifndef _GRAPHICS_MODE
    *total_iters += local;
#endif
}

static void ComputeVector(RGBQUAD* video_buf, float xmin, float xmax, float ymin, float ymax,
                          unsigned long long* total_iters)
{
    // assert(video_buf != NULL);

    float x_range = xmax - xmin;
    float y_range = ymax - ymin;
    float dx = x_range / kWidth;
    float dy = y_range / kHeight;
    float r2_limit = kLmax * kLmax;

    const __m256  r2max_vec   = _mm256_set1_ps(r2_limit);
    const __m256i zero_i      = _mm256_setzero_si256();
    const __m256  arr01234567 = _mm256_setr_ps(0, 1, 2, 3, 4, 5, 6, 7);
    const __m256  const2      = _mm256_set1_ps(2.0f);
    __m256 one_dx_arr = _mm256_mul_ps(arr01234567, _mm256_set1_ps(dx));
    __m256 eight_dx_arr = _mm256_set1_ps(8.0f * dx);
#ifndef _GRAPHICS_MODE
    unsigned long long local = 0;
#endif
    for (int y = 0; y < kHeight; y++)
    {
        float y0 = ymax - (float)y * dy; // или y / kHeight * y_range_local
        __m256 y0_vec = _mm256_set1_ps(y0);

        float x0_base = xmin;
        __m256 x0_vec = _mm256_add_ps(_mm256_set1_ps(x0_base), one_dx_arr);
        RGBQUAD* current_row = video_buf + y * kWidth;

        for (int x = 0; x < kWidth; x += 8)
        {
            __m256 x_vec = x0_vec;
            __m256 y_vec = y0_vec;
            __m256i iters = zero_i;

            for (int n = 0; n < kMaxIter; ++n)
            {
                __m256 x2 = _mm256_mul_ps(x_vec, x_vec);
                __m256 y2 = _mm256_mul_ps(y_vec, y_vec);
                __m256 xy = _mm256_mul_ps(x_vec, y_vec);
                __m256 r2 = _mm256_add_ps(x2, y2);

                __m256 cmp_f = _mm256_cmp_ps(r2, r2max_vec, _CMP_LE_OQ);
                int mask = _mm256_movemask_ps(cmp_f);
                if (mask == 0)
                    break;

                __m256 new_x = _mm256_add_ps(_mm256_sub_ps(x2, y2), x0_vec);
                __m256 new_y = _mm256_fmadd_ps(const2, xy, y0_vec);

                x_vec = _mm256_blendv_ps(x_vec, new_x, cmp_f);
                y_vec = _mm256_blendv_ps(y_vec, new_y, cmp_f);

                __m256i cmp_i = _mm256_castps_si256(cmp_f);
                iters = _mm256_sub_epi32(iters, cmp_i);
            }

            alignas(32) int iter_counts[8];
            _mm256_store_si256((__m256i*)iter_counts, iters);

            for (int i = 0; i < 8; ++i)
            {
#ifdef _GRAPHICS_MODE
            video_buf[y * kWidth + (x + i)] = GetColor(iter_counts[i]);
#else
            local += iter_counts[i];
#endif
            }
            x0_vec = _mm256_add_ps(x0_vec, eight_dx_arr);
        }
    }
#ifndef _GRAPHICS_MODE
    *total_iters += local;
#endif
}


static void UpdateView(float* cur_xmin, float* cur_xmax, float* cur_ymin, float* cur_ymax,
                       float* center_x, float* center_y, float* x_range,  float* y_range)
{
    float step_x = *x_range * 0.05f;
    float step_y = *y_range * 0.05f;
    if (GetAsyncKeyState(VK_RIGHT)) { *cur_xmin += step_x; *cur_xmax += step_x; }
    if (GetAsyncKeyState(VK_LEFT))  { *cur_xmin -= step_x; *cur_xmax -= step_x; }
    if (GetAsyncKeyState(VK_DOWN))  { *cur_ymin += step_y; *cur_ymax += step_y; }
    if (GetAsyncKeyState(VK_UP))    { *cur_ymin -= step_y; *cur_ymax -= step_y; }

    const float zoom_factor = 0.9f;
    if (GetAsyncKeyState('W'))
    {
        float new_x_range = *x_range * zoom_factor;
        float new_y_range = new_x_range * ((float)kHeight / kWidth);
        *cur_xmin = *center_x - new_x_range * 0.5f;
        *cur_xmax = *center_x + new_x_range * 0.5f;
        *cur_ymin = *center_y - new_y_range * 0.5f;
        *cur_ymax = *center_y + new_y_range * 0.5f;
        *x_range = *cur_xmax - *cur_xmin;
        *y_range = *cur_ymax - *cur_ymin;
    }
    if (GetAsyncKeyState('S'))
    {
        float new_x_range = *x_range / zoom_factor;
        float new_y_range = new_x_range * ((float)kHeight / kWidth);
        *cur_xmin = *center_x - new_x_range * 0.5f;
        *cur_xmax = *center_x + new_x_range * 0.5f;
        *cur_ymin = *center_y - new_y_range * 0.5f;
        *cur_ymax = *center_y + new_y_range * 0.5f;
        *x_range = *cur_xmax - *cur_xmin;
        *y_range = *cur_ymax - *cur_ymin;
    }

    *center_x = (*cur_xmin + *cur_xmax) * 0.5f;
    *center_y = (*cur_ymin + *cur_ymax) * 0.5f;

    return;
}

void ProcessMandelbrot(RGBQUAD* video_buf)
{
    assert(video_buf != NULL);

#if defined(_GRAPHICS_MODE)
    float cur_xmin = kXmin, cur_xmax = kXmax;
    float cur_ymin = kYmin, cur_ymax = kYmax;
    float center_x = (cur_xmin + cur_xmax) * 0.5f;
    float center_y = (cur_ymin + cur_ymax) * 0.5f;
    float x_range = cur_xmax - cur_xmin;
    float y_range = cur_ymax - cur_ymin;

    while (!GetAsyncKeyState(VK_ESCAPE))
    {
        UpdateView(&cur_xmin, &cur_xmax, &cur_ymin, &cur_ymax,
                   &center_x, &center_y, &x_range,  &y_range);
    #if defined(_SIMPLE_VER)
        ComputeSimple(video_buf, cur_xmin, cur_xmax, cur_ymin, cur_ymax, NULL);
    #endif //
    #if defined(_QUAD_VER)
        ComputeQuad(video_buf, cur_xmin, cur_xmax, cur_ymin, cur_ymax, NULL);
    #endif //
    #if defined(_VECTOR_VER)
        ComputeVector(video_buf, cur_xmin, cur_xmax, cur_ymin, cur_ymax, NULL);
    #endif //
    }
#else
    unsigned long long total_iters = 0;
    for (int perf = 0; perf < kPerformanceIters; perf++)
    {
        float cur_xmin = kXmin, cur_xmax = kXmax;
        float cur_ymin = kYmin, cur_ymax = kYmax;
#if defined(_SIMPLE_VER)
        ComputeSimple(NULL, kXmin, kXmax, kYmin, kYmax, &total_iters);
#elif defined(_QUAD_VER)
        ComputeQuad(NULL, kXmin, kXmax, kYmin, kYmax, &total_iters);
#elif defined(_VECTOR_VER)
        ComputeVector(NULL, kXmin, kXmax, kYmin, kYmax, &total_iters);
#endif
    }
    volatile unsigned long long dummy = total_iters;
    (void)dummy;
#endif // _GRAPHICS_MODE
}

