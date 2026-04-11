#include <TXlib.h>
#include "mandelbrots.h"
#include "painting.h"
#include "tests.h"
#include <stdlib.h>

int main(int argc, char* argv[])
{
    const char* test_filename = "benchmark_results.csv";
    if (argc == 2)
		test_filename = argv[1];

#if defined(_GRAPHICS_MODE)
    txCreateWindow(kWidth, kHeight);
    txTextCursor(false);
    RGBQUAD* videoBuf = txVideoMemory();
    if (!videoBuf)
        return -1;

    Win32::_fpreset();
    ProcessMandelbrot(videoBuf);
#else
    MakeTests(test_filename);
#endif

    return 0;
}
