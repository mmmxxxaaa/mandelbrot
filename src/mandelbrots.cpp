#include "mandelbrots.h"
#include <math.h>
#include <immintrin.h>
#include <assert.h>
#include "painting.h"

void SimpleMandelbrot(RGBQUAD* videoBuf)
{
    assert(videoBuf != NULL);

    float x_range = kXmax - kXmin;
    float y_range = kYmax - kYmin;
    for (int y = 0; y < kHeight; y++)
    {
        float p0_im = kYmax - (float)y / kHeight * y_range;

        for (int x = 0; x < kWidth; x++)
        {
            float p0_re = kXmin + (float)x / kWidth  * x_range;
            float p_re = 0.0;
            float p_im = 0.0;
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
#if defined(_GRAPHICS_MODE)
            videoBuf[y * kWidth + x] = GetColor(n_of_iter);
#endif //_GRAPHICS_MODE
        }
    }
    return ;
}

void QuadMandelbrot(RGBQUAD* videoBuf)
{
    assert(videoBuf != NULL);

    float x_range = kXmax - kXmin;
    float y_range = kYmax - kYmin;

    float dx = x_range / kWidth;

    for (int y = 0; y < kHeight; y++)
    {
        float y0 = kYmax - (float)y / kHeight * y_range;

        for (int x = 0; x < kWidth; x += kVecWidth)
        {
            float x0 = kXmin + (float)x / kWidth * x_range;

            float x0_arr[kVecWidth] = {};
            for (int i = 0; i < kVecWidth; i++) x0_arr[i] = x0 + i * dx;

            float y0_arr[kVecWidth] = {};
            for (int i = 0; i < kVecWidth; i++) y0_arr[i] = y0;

            float x_arr[kVecWidth] = {};
                for (int i = 0; i < kVecWidth; i++) x_arr[i] = x0_arr[i];

            float y_arr[kVecWidth] = {};
                for (int i = 0; i < kVecWidth; i++) y_arr[i] = y0_arr[i];

            int n_of_iters[kVecWidth] = {};      // массив счетчиков итераций, пока хуй знает как назвать

            for (int n = 0; n < kMaxIter; n++)
            {
                float x2[kVecWidth] = {}; for (int i = 0; i < kVecWidth; i++) x2[i] = x_arr[i] * x_arr[i];
                float y2[kVecWidth] = {}; for (int i = 0; i < kVecWidth; i++) y2[i] = y_arr[i] * y_arr[i];
                float xy[kVecWidth] = {}; for (int i = 0; i < kVecWidth; i++) xy[i] = x_arr[i] * y_arr[i];

                float r2[kVecWidth] = {}; for (int i = 0; i < kVecWidth; i++) r2[i] = x2[i] + y2[i];

                int cmp[kVecWidth] = {};
                for (int i = 0; i < kVecWidth; i++)
                    if (r2[i] <= kLmax  * kLmax )
                        cmp[i] = 1;

                int mask = 0;       //битовый флаг активности четырёх точек

                for (int i = 0; i < kVecWidth; i++)
                    mask |= cmp[i] << i;
                if (!mask)
                    break;

                for (int i = 0; i < kVecWidth; i++)
                    x_arr[i] = x2[i] - y2[i] + x0_arr[i];
                for (int i = 0; i < kVecWidth; i++)
                    y_arr[i] = xy[i] + xy[i] + y0_arr[i];

                for (int i = 0; i < kVecWidth; i++)
                    n_of_iters[i] += cmp[i];
            }

#if defined(_GRAPHICS_MODE)
            for (int i = 0; i < kVecWidth; i++)
            {
                int videoMemoryOffset = (kHeight - 1 - y) * kWidth + (x + i);
                videoBuf[videoMemoryOffset] = GetColor(n_of_iters[i]);
            }
#endif // _GRAPHICS_MODE
        }
    }
    return;
}

void VectorMandelbrot(RGBQUAD* videoBuf)
{
    assert(videoBuf != NULL);

    const float x_range = kXmax - kXmin;
    const float y_range = kYmax - kYmin;
    const float dx = x_range / kWidth;
    const float dy = y_range / kHeight;
    const float r2_limit = kLmax * kLmax;

    const __m256  r2max_vec   = _mm256_set1_ps(r2_limit);
    const __m256i zero_i      = _mm256_setzero_si256();
    const __m256  arr01234567 = _mm256_setr_ps(0, 1, 2, 3, 4, 5, 6, 7);
    const __m256  const2      = _mm256_set1_ps(2.0f);

    __m256 one_dx_arr   = _mm256_mul_ps(arr01234567, _mm256_set1_ps(dx));
    __m256 eight_dx_arr = _mm256_set1_ps(8.0f * dx);

    for (int y = 0; y < kHeight; y++)
    {
        float y0 = kYmax - (float)y / kHeight * y_range;
        __m256 y0_vec = _mm256_set1_ps(y0);

        float x0_base = kXmin;
        __m256 x0_vec = _mm256_add_ps(_mm256_set1_ps(x0_base), one_dx_arr);
#if defined(_GRAPHICS_MODE)
        RGBQUAD* current_row = videoBuf + (kHeight - 1 - y) * kWidth;
#endif // _GRAPHICS_MODE
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

                __m256 new_x = _mm256_add_ps(_mm256_sub_ps(x2, y2), x0_vec); // new_x = x*x - y*y + x0
                __m256 new_y = _mm256_fmadd_ps(const2, xy, y0_vec);          // new_y = 2*x*y + y0
                //FIXME МБ КАПИТАЛЬНО ПОХУЙ
                x_vec = _mm256_blendv_ps(x_vec, new_x, cmp_f); // если знаковый бит cmp_f[i] = 1 (т.е. cmp_f[i] < 0), берем new_x[i], иначе оставляем x_vec[i].
                y_vec = _mm256_blendv_ps(y_vec, new_y, cmp_f);

                __m256i cmp_i = _mm256_castps_si256(cmp_f); // чтобы потом использовать _mm256_sub_epi32
                iters = _mm256_sub_epi32(iters, cmp_i);
            }

            // мб это на интринсиках(вроде нет)
            alignas(32) int iter_counts[8] = {};
            _mm256_store_si256((__m256i*)iter_counts, iters);   // цвет будем обрабатывать скалярно

#if defined(_GRAPHICS_MODE)
            for (int i = 0; i < 8; ++i)
                current_row[x + i] = GetColor(iter_counts[i]);
#endif // _GRAPHICS_MODE

            x0_vec = _mm256_add_ps(x0_vec, eight_dx_arr);
        }
    }
    return;
}
