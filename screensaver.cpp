#include <stdlib.h>
#include "SDL.h"
#include "SDL2_gfxPrimitives.h"
#include "stdbool.h"
#include "iostream"

#define SWIDTH 1280
#define SHEIGHT 720
#define NUMSTARS 25
#define VSTARS 1

SDL_Window* window;
SDL_Renderer* renderer;
bool quit;
int frm_cnt, last_frm, lst_count, tmr_fps, fps;

// variables globales
float circle_x = 320;
float circle_y = 240;
float circle_vx = 1;
float circle_vy = 1;
float circle_radius = 25;

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

Star obj_stars[NUMSTARS];

void Init() {
    printf("Iniciando programa ...\n");
    srand(time(NULL));
    // se inician las estrellas
    for (int i = 0; i < NUMSTARS; i++) {
        obj_stars[i].x = rand() % SWIDTH;
        obj_stars[i].y = rand() % SHEIGHT;
        obj_stars[i].vx = VSTARS;
        obj_stars[i].vy = 0;
        obj_stars[i].r = rand() % 5 + 1;
        obj_stars[i].alpha = rand() % 100 + 155;
    }
    printf("Programa iniciado con exito\n");
};

void Render() {
    frm_cnt++;
    tmr_fps = SDL_GetTicks()-last_frm;
    if(tmr_fps<(1000/60)) {
        SDL_Delay((1000/60)-tmr_fps);
    }
};

void Input() {
    SDL_Event event;
    const Uint8* keystts = SDL_GetKeyboardState(NULL);
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            // Se cierra la ventana
            quit = true;
        }
    }
};

void Update() {

};

void CleanEnvironment() {
    printf("Limpiando memoria ...\n");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    printf("Programa cerrado con exito\n");
};

int main(int argc, char *argv[]) {
    quit = false;
    static int lst_time = 0;
    // iniciar SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("Error al inicializar SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    // crea pantalla
    window = SDL_CreateWindow("Screensaver", 
                                          SDL_WINDOWPOS_UNDEFINED, 
                                          SDL_WINDOWPOS_UNDEFINED, 
                                          SWIDTH, 
                                          SHEIGHT, 
                                          SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Error al crear ventana: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    // renderiza pantalla
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    if (renderer == NULL) {
        printf("Error al renderizar: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    // Inicializar configuraciones del programa
    Init();
    // se inicia el equipo A
    Player teamA[NUM_PLAYERS];
    for (int i = 0; i < NUM_PLAYERS; i++) {
        
        teamA[i].x = rand() % SWIDTH;
        teamA[i].y = rand() % SHEIGHT;
        teamA[i].vx = V_PLAYERS;
        teamA[i].vy = V_PLAYERS;
        teamA[i].r = rand() % 10 + 20;
        teamA[i].dead = false;
    }

    // se inicia el equipo B
    Player teamB[NUM_PLAYERS];
    for (int i = 0; i < NUM_PLAYERS; i++) {
        
        teamB[i].x = rand() % SWIDTH;
        teamB[i].y = rand() % SHEIGHT;
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
    while (!quit) {
        last_frm = SDL_GetTicks();
        if(last_frm >= (lst_time+1000)) {
            lst_time = last_frm;
            fps = frm_cnt;
            frm_cnt = 0;
            printf("FPS: %d\n",fps);
        }
        // se mueven las estrellas del fondo
        for (int i = 0; i < NUMSTARS; i++) {
            obj_stars[i].x += obj_stars[i].vx;
            obj_stars[i].y += obj_stars[i].vy;

            // Cuando el ciculo toca un borde se desaparece y aparece una nueva estrella
            if (obj_stars[i].x > SWIDTH - obj_stars[i].r) {
                obj_stars[i].x = 0;
                obj_stars[i].y = rand() % SHEIGHT;
                obj_stars[i].vx = VSTARS;
                obj_stars[i].vy = 0;
                obj_stars[i].r = rand() % 5 + 1;
            }
        }


        // se mueven los players del equipo A
        for (int i = 0; i < NUM_PLAYERS; i++) {
            teamA[i].x += teamA[i].vx;
            teamA[i].y += teamA[i].vy;

            // Cuando el ciculo toca un borde invierte la dirección de la velocidad
            if (teamA[i].x < teamA[i].r || teamA[i].x > SWIDTH - teamA[i].r) {
                teamA[i].vx = -teamA[i].vx;
            }
            if (teamA[i].y < teamA[i].r || teamA[i].y > SHEIGHT - teamA[i].r) {
                teamA[i].vy = -teamA[i].vy;
            }
        }

        // se mueven los players del equipo B
        for (int i = 0; i < NUM_PLAYERS; i++) {
            teamB[i].x += teamB[i].vx;
            teamB[i].y += teamB[i].vy;

            // Cuando el ciculo toca un borde invierte la dirección de la velocidad
            if (teamB[i].x < teamB[i].r || teamB[i].x > SWIDTH - teamB[i].r) {
                teamB[i].vx = -teamB[i].vx;
            }
            if (teamB[i].y < teamB[i].r || teamB[i].y > SHEIGHT - teamB[i].r) {
                teamB[i].vy = -teamB[i].vy;
            }
        }

        // se mueven las balas
        for (int j = 0; j < NUM_BULLETS; j++){
            for (int i = 0; i < NUM_PLAYERS; i++) {
                teamA[i].bullets[i].x += teamA[i].bullets[i].vx;
                teamA[i].bullets[i].y += teamA[i].bullets[i].vy;

                // Cuando tocan los bordes hay un nuevo disparo
                if ((teamA[i].bullets[i].x < teamA[i].bullets[i].r || teamA[i].bullets[i].x > SWIDTH - teamA[i].bullets[i].r) ||
                    (teamA[i].bullets[i].y < teamA[i].bullets[i].r || teamA[i].bullets[i].y > SHEIGHT - teamA[i].bullets[i].r)) {
                    teamA[i].bullets[i].x = teamA[i].x;
                    teamA[i].bullets[i].y = teamA[i].y;
                    teamA[i].bullets[i].vx = teamA[i].vx;
                    teamA[i].bullets[i].vy = teamA[i].vy;
                    teamA[i].bullets[i].r = 5;
                    teamA[i].bullets[i].used = false;
                }
                
                teamB[i].bullets[i].x += teamB[i].bullets[i].vx;
                teamB[i].bullets[i].y += teamB[i].bullets[i].vy;

                if ((teamB[i].bullets[i].x < teamB[i].bullets[i].r || teamB[i].bullets[i].x > SWIDTH - teamB[i].bullets[i].r) ||
                    (teamB[i].bullets[i].y < teamB[i].bullets[i].r || teamB[i].bullets[i].y > SHEIGHT - teamB[i].bullets[i].r)) {
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


        for (int i = 0; i < NUMSTARS; i++) {
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
        Input();
        Update();
        Render();
    }
    // limpia la app para cerrarla
    CleanEnvironment();

    return EXIT_SUCCESS;
}