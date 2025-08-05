@echo off
echo Start to extract audio and convert to mp3...
python .\extract_audio.py
echo.
echo Start to convert m4a to mp3...(if needed)
python .\m4aTomp3.py
echo.
echo Start to generate framesdata...
python .\DataGenerator.py
echo.
set compiler_flags=-I./SDL2/include -L./SDL2/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer
echo compiling...
g++ videoPlay_sdl.cpp -o ./run/videoPlay_sdl %compiler_flags%
echo compilation completed!