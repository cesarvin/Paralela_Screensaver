# Proyecto 1 Paralela


Este repositorio contiene archivos importantes para la elaboración del proyecto 1 de la clase de Programación Paralela y Distribuida 2023.

Integrantes:

- Cesar Vinicio Rodas Alvarado
- Osmin Josue Sagastume Orellana
- Javier Alejandro Ramírez Cospin

Se recomienda usar Linux ubuntu 20.04 en adelante

## Programa Secuencial

Para compilar el programa secuencial (screensaver.cpp):

```bash
g++ screensaver.cpp -o screensaver.o -I /usr/include/SDL2 -I /usr/include/SDL -lSDL2 -lm -lSDL2_gfx
```

Para ejecutar el programa secuencial:

```bash
./screensaver.o <numplayers> <numbullets> <bulletspeed> <playerlives>
```

- *numplayers:* Numero de jugadores.
- *numbullets:* Numero de balas por jugador.
- *bulletspeed:* Velocidad de cada bala.
- *playerlives:* Vida maxima de cada jugador. 

## Programa Paralelo

Para compilar el programa paralelo (screensaver_par.cpp:

```bash
g++ screensaver_par.cpp -fopenmp -o screensaver_par.o -I /usr/include/SDL2 -I /usr/include/SDL -lSDL2 -lm -lSDL2_gfx
```

Para ejecutar el programa paralelo:

```bash
./screensaver_par.o <numplayers> <numbullets> <bulletspeed> <playerlives> <numthreads>
```

- *numplayers:* Numero de jugadores.
- *numbullets:* Numero de balas por jugador.
- *bulletspeed:* Velocidad de cada bala.
- *playerlives:* Vida maxima de cada jugador. 
- *numthreads:* Numero de threads para ejecutar.
