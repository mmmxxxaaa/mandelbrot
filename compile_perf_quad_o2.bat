g++ -o perf_quad_o2.exe src/painting.cpp src/mandelbrots.cpp src/main.cpp src/tests.cpp ^
    -I C:\TXlib -I./include -mavx2 -march=native -D_QUAD_VER  -g -fno-omit-frame-pointer -O2 -DNDEBUG
