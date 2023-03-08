# Paralela_Screensaver
Primer Proyecto


Para compilar ubuntu 22.04

```bash
gcc screensaver.c -fopenmp -o screensaver.o -I /usr/include/SDL2 -I /usr/include/SDL -lSDL2 -lm -lSDL2_gfx
```

Para ejecutar 

```bash
./screensaver.o
```

