@echo off
python .\extract_audio.py
echo.
python .\m4aTomp3.py
echo.
python .\DataGenerator.py
echo.
set compiler_flags=-I./SDL2/include -L./SDL2/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lSDL2_ttf
echo compiling...
g++ videoPlay_sdl_ttf.cpp -o ./run/videoPlay_sdl_ttf.exe %compiler_flags%
echo compilation completed!