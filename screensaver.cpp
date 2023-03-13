#include <stdlib.h>
#include "SDL.h"
#include "SDL2_gfxPrimitives.h"
#include "stdbool.h"
#include "iostream"
#include <vector>
#include <time.h>

#define SWIDTH 1280
#define SHEIGHT 720
#define NUMSTARS 25
#define VSTARS 1
#define NUMPLAYERS 7
#define VPLAYERS 7
#define NUMBULLETS 5

// variables globales
SDL_Window* window;
SDL_Renderer* renderer;
bool quit;
int frm_cnt, last_frm, lst_count, tmr_fps, fps;
float circle_x = 320;
float circle_y = 240;
float circle_vx = 1;
float circle_vy = 1;
float circle_radius = 25;

// Estructura para las estrellas
class Star {
    private:
        float x, y, vx, vy;  // pos x, pos y, vel x, vel y
        float r, alpha;  // radius, color alpha
    public:
        Star(float x,float y,float vx,float vy,float r,float alpha) {
            this->x = x;
            this->y = y;
            this->vx = vx;
            this->vy = vy;
            this->r = r;
            this->alpha = alpha;
        }
        float GetX(void) { return this->x; }
        float GetY(void) { return this->y; }
        float GetVX(void) { return this->vx; }
        float GetVY(void) { return this->vy; }
        float GetR(void) { return this->r; }
        float GetAlpha(void) { return this->alpha; }
        void UpdateVel(void) {
            this->x += this->vx;
            this->y += this->vy;
        }
        void RestartPosition(void) {
            this->x = 0;
            this->y = rand() % SHEIGHT;
            this->r = rand() % 5 + 1;
        }
        ~Star() { }
};

// Estructura para las balas
struct Player {
    private:
        int id;
        float x, y, vx, vy;
        float r, timer;
        int health;
        time_t counter;
    public:
        Player(int id,float x,float y,float r) {
            this->id = id;
            this->x = x;
            this->y = y;
            this->r = r;
            this->health = 5;
            GenerateVelocities();
            ResetTimeParameters();
        }
        int GetId(void) { return this->id; }
        int GetX(void) { return this->x; }
        int GetY(void) { return this->y; }
        int GetVX(void) { return this->vx; }
        int GetVY(void) { return this->vy; }
        int GetR(void) { return this->r; }
        void TakeHit(void) { this->health -= 1; }
        bool IsDead(void) {
            bool result = this->health > 0 ? false : true;
            return result; 
        }
        void GenerateVelocities(void) {
            int num1 = rand() % 10;
            if(num1 >= 5) { this->vx = rand() % VPLAYERS; }
            else { this->vx = (rand() % VPLAYERS) * -1; }
            int num2 = rand() % 10;
            if(num2 >= 5) { this->vy = rand() % VPLAYERS; }
            else { this->vy = (rand() % VPLAYERS) * -1; }
        }
        void UpdatePosition(void) {
            if(time(0) >= this->counter + this->timer) {
                ResetTimeParameters();
                GenerateVelocities();
            }
            if(this->x-this->r < this->r+10 && this->vx < 0) { this->vx *= -1; }
            if(this->x+this->r > SWIDTH-(this->r+10) && this->vx > 0) { this->vx *= -1; }
            if(this->y-this->r < this->r+10 && this->vy < 0) { this->vy *= -1; }
            if(this->y+this->r > SHEIGHT-(this->r+10) && this->vy > 0) { this->vy *= -1; }
            this->x += this->vx;
            this->y += this->vy;
        }
        void ResetTimeParameters(void) {
            this->counter = time(0);
            this->timer = (float)(rand() % 2)+((float)(rand())/(float)(RAND_MAX));
        }
        ~Player() {}
};

std::vector<Star*> obj_stars;
std::vector<Player*> teamA;
std::vector<Player*> teamB;

void Init() {
    printf("Iniciando programa ...\n");
    srand(time(NULL));
    // se inician las estrellas
    for (int i = 0; i < NUMSTARS; i++) {
        Star* obj = new Star(rand() % SWIDTH,
                            rand() % SHEIGHT,
                            VSTARS,
                            0,
                            rand() % 5 + 1,
                            rand() % 100 + 155);
        obj_stars.push_back(obj);
    }
    // Inicializar equipo A
    for(int i = 0;i < NUMPLAYERS;i++) {
        int radius = rand() % 10 + 20;
        Player* plyr = new Player(i,
                                 rand() % (SWIDTH - radius*2) + (radius*2),
                                 rand() % (SHEIGHT - radius*2) + (radius*2),
                                 radius);
        teamA.push_back(plyr);
    }
    // Inicializar equipo B
    for(int i = 0;i < NUMPLAYERS;i++) {
        int radius = rand() % 10 + 20;
        Player* plyr = new Player(i,
                                 rand() % (SWIDTH - radius*2) + (radius*2),
                                 rand() % (SHEIGHT - radius*2) + (radius*2),
                                 radius);
        teamB.push_back(plyr);
    }
    // Inicializar equipo B 
    printf("Programa iniciado con exito\n");
};

void Render() {
    frm_cnt++;
    tmr_fps = SDL_GetTicks()-last_frm;
    if(tmr_fps<(1000/60)) {
        SDL_Delay((1000/60)-tmr_fps);
    }
    // Renderizar fondo
    SDL_SetRenderDrawColor(renderer, // render
                        0,0,0,//133, 146, 158,//18, 24, 46,  // r,g,b
                        255); // alpha
    SDL_RenderClear(renderer);
    // Renderizar estrellas
    for(Star* star : obj_stars) {
        filledCircleRGBA(renderer, //render 
                    star->GetX(),star->GetY(),star->GetR(), // circulo
                    253, 253, 0, // r,g,b
                    star->GetAlpha()); //alpha
    }
    // Renderizar Equipo A
    for(Player* plyr : teamA) {
        filledCircleRGBA(renderer, //render 
                    plyr->GetX(),plyr->GetY(),plyr->GetR(), // circulo
                    255, 255, 255, 255); // r,g,b,alpha
    }
    // Renderizar Equipo B
    for(Player* plyr : teamB) {
        filledCircleRGBA(renderer, //render 
                    plyr->GetX(),plyr->GetY(),plyr->GetR(), // circulo
                    13, 141, 254, 255); // r,g,b,alpha
    }
    SDL_RenderPresent(renderer);
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
    // Movimiento de estrellas
    for(Star* star : obj_stars) {
        star->UpdateVel();
        // Cuando el ciculo toca un borde se desaparece y aparece una nueva estrella
        if (star->GetX() > SWIDTH - star->GetR()) {
            star->RestartPosition();
        }
    }
    // Equipo A
    for(Player* plyr : teamA) {
        plyr->UpdatePosition();
    }
    // Equipo B
    for(Player* plyr : teamB) {
        plyr->UpdatePosition();
    }
};

void CleanEnvironment() {
    printf("Limpiando memoria ...\n");
    for(Star* star : obj_stars) {
        delete star;
    }
    obj_stars.clear();
    for(Player* plyr : teamA) {
        delete plyr;
    }
    teamA.clear();
    for(Player* plyr : teamB) {
        delete plyr;
    }
    teamB.clear();
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
    if (renderer == NULL) {
        printf("Error al renderizar: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    // Inicializar configuraciones del programa
    Init();
    // Loop principal
    while (!quit) {
        last_frm = SDL_GetTicks();
        if(last_frm >= (lst_time+1000)) {
            lst_time = last_frm;
            fps = frm_cnt;
            frm_cnt = 0;
            printf("FPS: %d\n",fps);
        }
        Input();
        Update();
        Render();
    }
    // limpia la app para cerrarla
    CleanEnvironment();
    return EXIT_SUCCESS;
}