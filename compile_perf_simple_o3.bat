g++ -o perf_simple_o3.exe src/painting.cpp src/mandelbrots.cpp src/main.cpp -I C:\TXlib -I./include -mavx2 -march=native -D_SIMPLE_VER  -g -fno-omit-frame-pointer -O3 -DNDEBUG
