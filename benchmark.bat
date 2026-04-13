@echo off
REM =====================================================
REM Последовательный запуск тестов производительности
REM с высоким приоритетом и привязкой к ядру 3 (маска 8)
REM =====================================================

set AFFINITY_MASK=8
REM /affinity %AFFINITY_MASK%

REM call compile_perf_simple_o2.bat
REM start "" /b /wait /high /affinity %AFFINITY_MASK% perf_simple_o2.exe data/Simple_and_Vector/res_simple_o2.csv

REM call compile_perf_simple_o3.bat
REM start "" /b /wait /high /affinity %AFFINITY_MASK% perf_simple_o3.exe data/Simple_and_Vector/res_simple_o3.csv

REM call compile_perf_quad_o2.bat
start "" /b /wait /high /affinity %AFFINITY_MASK% perf_quad_o2.exe data/Quad_after_two_fixes/res_quad_o2.csv

REM call compile_perf_quad_o3.bat
start "" /b /wait /high /affinity %AFFINITY_MASK% perf_quad_o3.exe data/Quad_after_two_fixes/res_quad_o3.csv

REM call compile_perf_vector_o2.bat
REM start "" /b /wait /high /affinity %AFFINITY_MASK% perf_vector_o2.exe data/Simple_and_Vector/res_vector_o2.csv

REM call compile_perf_vector_o3.bat
REM start "" /b /wait /high /affinity %AFFINITY_MASK% perf_vector_o3.exe data/Simple_and_Vector/res_vector_o3.csv

pause
