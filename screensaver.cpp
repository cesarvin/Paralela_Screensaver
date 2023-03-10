
#include "SDL.h"
#include "SDL2_gfxPrimitives.h"
#include "stdbool.h"
#include "iostream"

// variables globales
float circle_x = 320;
float circle_y = 240;
float circle_vx = 1;
float circle_vy = 1;
float circle_radius = 25;

// tamaño de pantalla
int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 720;

int NUM_STARS = 25;
int V_STARS = 1;

int NUM_PLAYERS = 5;
int V_PLAYERS = 2;

int NUM_BULLETS = 5;

// Estructuras para los objetos
struct Star {
    float x;  // pos x
    float y;  // pos y 
    float vx; // vel x
    float vy; // vel y
    float r;  // radius 
    float alpha; // color alpha de la estrella
};

struct Bullet {
    float x;  // pos x
    float y;  // pos y 
    float vx; // vel x
    float vy; // vel y
    float r;  // radius 
    bool used; // usada
};

struct Player {
    float x;  // pos x
    float y;  // pos y 
    float vx; // vel x
    float vy; // vel y
    float r;  // radius 
    Bullet bullets[5];
    bool dead;
};

int main(int argc, char *argv[]) {

    // iniciar SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error al inicializar SDL: %s\n", SDL_GetError());
        return 1;
    }

    // crea pantalla
    SDL_Window* window = SDL_CreateWindow("Screensaver", 
                                          SDL_WINDOWPOS_UNDEFINED, 
                                          SDL_WINDOWPOS_UNDEFINED, 
                                          SCREEN_WIDTH, 
                                          SCREEN_HEIGHT, 
                                          SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Error al crear ventana: %s\n", SDL_GetError());
        return 1;
    }

    // renderiza pantalla
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    
    //comprueba si se renderizo
    if (renderer == NULL) {
        printf("Error al renderizar: %s\n", SDL_GetError());
        return 1;
    }

    srand(time(NULL)); //inicializa la semilla aleatoria

    // se inician las estrellas
    Star obj_stars[NUM_STARS];
    for (int i = 0; i < NUM_STARS; i++) {
        
        obj_stars[i].x = rand() % SCREEN_WIDTH;
        obj_stars[i].y = rand() % SCREEN_HEIGHT;
        obj_stars[i].vx = V_STARS;
        obj_stars[i].vy = 0;
        obj_stars[i].r = rand() % 5 + 1;
        obj_stars[i].alpha = rand() % 100 + 155;
    }

    // se inicia el equipo A
    Player teamA[NUM_PLAYERS];
    for (int i = 0; i < NUM_PLAYERS; i++) {
        
        teamA[i].x = rand() % SCREEN_WIDTH;
        teamA[i].y = rand() % SCREEN_HEIGHT;
        teamA[i].vx = V_PLAYERS;
        teamA[i].vy = V_PLAYERS;
        teamA[i].r = rand() % 10 + 20;
        teamA[i].dead = false;
    }

    // se inicia el equipo B
    Player teamB[NUM_PLAYERS];
    for (int i = 0; i < NUM_PLAYERS; i++) {
        
        teamB[i].x = rand() % SCREEN_WIDTH;
        teamB[i].y = rand() % SCREEN_HEIGHT;
        teamB[i].vx = V_PLAYERS;
        teamB[i].vy = V_PLAYERS;
        teamB[i].r = rand() % 10 + 20;
        teamB[i].dead = false;
    }

    // disparos 
    for (int j = 0; j < NUM_BULLETS; j++){
        for (int i = 0; i < NUM_PLAYERS; i++) {
            teamA[i].bullets[i].x = teamA[i].x;
            teamA[i].bullets[i].y = teamA[i].y;
            teamA[i].bullets[i].vx = 2 * teamA[i].vx;
            teamA[i].bullets[i].vy = 2 * teamA[i].vy;
            teamA[i].bullets[i].r = 5;
            teamA[i].bullets[i].used = false;

            teamB[i].bullets[i].x = teamB[i].x;
            teamB[i].bullets[i].y = teamB[i].y;
            teamB[i].bullets[i].vx = 2 * teamB[i].vx;
            teamB[i].bullets[i].vy = 2 * teamB[i].vy;
            teamB[i].bullets[i].r = 5;
            teamB[i].bullets[i].used = false;
        }
    }


    // ciclo para hacer que el ciculo se mueva
    bool quit = false;
    SDL_Event e;

    while (!quit) {

        // Eventos de SDL
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                // Se cierra la ventana
                quit = true;
                break;
            }
        }
        
        
        // se mueven las estrellas del fondo
        for (int i = 0; i < NUM_STARS; i++) {
            obj_stars[i].x += obj_stars[i].vx;
            obj_stars[i].y += obj_stars[i].vy;

            // Cuando el ciculo toca un borde se desaparece y aparece una nueva estrella
            if (obj_stars[i].x > SCREEN_WIDTH - obj_stars[i].r) {
                obj_stars[i].x = 0;
                obj_stars[i].y = rand() % SCREEN_HEIGHT;
                obj_stars[i].vx = V_STARS;
                obj_stars[i].vy = 0;
                obj_stars[i].r = rand() % 5 + 1;
            }
        }


        // se mueven los players del equipo A
        for (int i = 0; i < NUM_PLAYERS; i++) {
            teamA[i].x += teamA[i].vx;
            teamA[i].y += teamA[i].vy;

            // Cuando el ciculo toca un borde invierte la dirección de la velocidad
            if (teamA[i].x < teamA[i].r || teamA[i].x > SCREEN_WIDTH - teamA[i].r) {
                teamA[i].vx = -teamA[i].vx;
            }
            if (teamA[i].y < teamA[i].r || teamA[i].y > SCREEN_HEIGHT - teamA[i].r) {
                teamA[i].vy = -teamA[i].vy;
            }
        }

        // se mueven los players del equipo B
        for (int i = 0; i < NUM_PLAYERS; i++) {
            teamB[i].x += teamB[i].vx;
            teamB[i].y += teamB[i].vy;

            // Cuando el ciculo toca un borde invierte la dirección de la velocidad
            if (teamB[i].x < teamB[i].r || teamB[i].x > SCREEN_WIDTH - teamB[i].r) {
                teamB[i].vx = -teamB[i].vx;
            }
            if (teamB[i].y < teamB[i].r || teamB[i].y > SCREEN_HEIGHT - teamB[i].r) {
                teamB[i].vy = -teamB[i].vy;
            }
        }

        // se mueven las balas
        for (int j = 0; j < NUM_BULLETS; j++){
            for (int i = 0; i < NUM_PLAYERS; i++) {
                teamA[i].bullets[i].x += teamA[i].bullets[i].vx;
                teamA[i].bullets[i].y += teamA[i].bullets[i].vy;

                // Cuando tocan los bordes hay un nuevo disparo
                if ((teamA[i].bullets[i].x < teamA[i].bullets[i].r || teamA[i].bullets[i].x > SCREEN_WIDTH - teamA[i].bullets[i].r) ||
                    (teamA[i].bullets[i].y < teamA[i].bullets[i].r || teamA[i].bullets[i].y > SCREEN_HEIGHT - teamA[i].bullets[i].r)) {
                    teamA[i].bullets[i].x = teamA[i].x;
                    teamA[i].bullets[i].y = teamA[i].y;
                    teamA[i].bullets[i].vx = teamA[i].vx;
                    teamA[i].bullets[i].vy = teamA[i].vy;
                    teamA[i].bullets[i].r = 5;
                    teamA[i].bullets[i].used = false;
                }
                
                teamB[i].bullets[i].x += teamB[i].bullets[i].vx;
                teamB[i].bullets[i].y += teamB[i].bullets[i].vy;

                if ((teamB[i].bullets[i].x < teamB[i].bullets[i].r || teamB[i].bullets[i].x > SCREEN_WIDTH - teamB[i].bullets[i].r) ||
                    (teamB[i].bullets[i].y < teamB[i].bullets[i].r || teamB[i].bullets[i].y > SCREEN_HEIGHT - teamB[i].bullets[i].r)) {
                    teamB[i].bullets[i].x = teamB[i].x;
                    teamB[i].bullets[i].y = teamB[i].y;
                    teamB[i].bullets[i].vx = teamB[i].vx;
                    teamB[i].bullets[i].vy = teamB[i].vy;
                    teamB[i].bullets[i].r = 5;
                    teamB[i].bullets[i].used = false;
                }

            }
        }
       

        // se mueve un circulo
        SDL_SetRenderDrawColor(renderer, // render
                            0,0,0,//133, 146, 158,//18, 24, 46,  // r,g,b
                            255); // alpha
        SDL_RenderClear(renderer);


        for (int i = 0; i < NUM_STARS; i++) {
            filledCircleRGBA(renderer, //render 
                        obj_stars[i].x, obj_stars[i].y, obj_stars[i].r, // circulo
                        253, 253, 0, // r,g,b
                        obj_stars[i].alpha); //alpha
        }

        for (int i = 0; i < NUM_PLAYERS; i++) {
            filledCircleRGBA(renderer, //render 
                        teamA[i].x, teamA[i].y, teamA[i].r, // circulo
                        255, 255, 255,//255, 0, 0, // r,g,b
                        255); //alpha
        }

        for (int i = 0; i < NUM_PLAYERS; i++) {
            filledCircleRGBA(renderer, //render 
                        teamB[i].x, teamB[i].y, teamB[i].r, // circulo
                        13, 141, 254,//21, 249, 52, // r,g,b
                        255); //alpha
        }
        
        for (int j = 0; j < NUM_BULLETS; j++){
            for (int i = 0; i < NUM_PLAYERS; i++) {
                filledCircleRGBA(renderer, //render 
                        teamA[i].bullets[i].x, teamA[i].bullets[i].y, teamA[i].bullets[i].r, // circulo
                        255, 255, 255, // r,g,b
                        255); //alpha

                filledCircleRGBA(renderer, //render 
                        teamB[i].bullets[i].x, teamB[i].bullets[i].y, teamB[i].bullets[i].r, // circulo
                        13, 141, 254, // r,g,b
                        255); //alpha
            }
        }
        
        // Actualizar la ventana
        SDL_RenderPresent(renderer);
        
        
    }

    // limpia la app para cerrarla
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
