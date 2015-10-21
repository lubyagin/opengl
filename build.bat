@REM with Code::Blocks, not Dev-Cpp
"D:\PROGS\CodeBlocks\MinGW\bin\gcc.exe" --std=c99 -Iinclude -c -o test.obj test.c
"D:\PROGS\CodeBlocks\MinGW\bin\gcc.exe" -o test.exe test.obj -L. -lopengl32 -lglu32 -lglut32
"D:\PROGS\CodeBlocks\MinGW\bin\strip.exe" --strip-all test.exe
"D:\UTILS\upx.exe" test.exe
