g++ -Isrc/include -c .\main.cpp
g++ main.o -o compiled_with_bat -Lsrc/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
main.exe