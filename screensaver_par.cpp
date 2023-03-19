#include <stdlib.h>
#include "SDL.h"
#include "SDL2_gfxPrimitives.h"
#include "stdbool.h"
#include "iostream"
#include <vector>
#include <time.h>
#include <math.h>
#include <omp.h>

#define SWIDTH 1280
#define SHEIGHT 720
#define NUMSTARS 25
#define VSTARS 1
#define VPLAYERS 7
#define PI 3.14159265

class Star;
class Bullet;
class Player;

// variables globales
SDL_Window* window;
SDL_Renderer* renderer;
bool quit;
int frm_cnt, last_frm, lst_count, tmr_fps, fps;
std::vector<Star*> obj_stars;
std::vector<Bullet*> bullets;
std::vector<Player*> teamA;
std::vector<Player*> teamB;
int numplayers, numbullets, phealth, vbullet;
int num_thds;

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

// Estructura pra las balas
class Bullet {
    private:
        int group, parent_id;
        float x, y, vx, vy, r;
    public:
        Bullet(int group,int parent_id,float x,float y,float vx,float vy,float r) {
            this->group = group;
            this->parent_id = parent_id;
            this->x = x;
            this->y = y;
            this->vx = vx;
            this->vy = vy;
            this->r = r;
        }
        int GetGroupId(void) { return this->group; }
        int GetParentId(void) { return this->parent_id; }
        float GetX(void) { return this->x; }
        float GetY(void) { return this->y; }
        float GetVX(void) { return this->vx; }
        float GetVY(void) { return this->vy; }
        float GetR(void) { return this->r; }
        void UpdatePosition(void) {
            this->x += this->vx;
            this->y += this->vy;
        }
        ~Bullet() {}
};

// Estructura para los jugadores
class Player {
    private:
        int id, group, nbullets;
        float x, y, vx, vy;
        float r, timer;
        int health;
        time_t counter, atktime;
    public:
        Player(int id,int group,float x,float y,float r) {
            this->id = id;
            this->group = group;
            this->nbullets = numbullets;
            this->x = x;
            this->y = y;
            this->r = r;
            this->health = phealth;
            this->atktime = time(0) + 1.2;
            GenerateVelocities();
            ResetTimeParameters();
        }
        int GetId(void) { return this->id; }
        int GetGroup(void) { return this->group; }
        int GetBullets(void) { return this->nbullets; }
        int GetX(void) { return this->x; }
        int GetY(void) { return this->y; }
        int GetVX(void) { return this->vx; }
        int GetVY(void) { return this->vy; }
        int GetR(void) { return this->r; }
        void TakeHit(void) { 
            if(this->health > 0) { this->health -= 1; }
        }
        void Shoot(void) { 
            if(time(0) >= this->atktime && this->nbullets > 0) {
                // Disparo a miembro random del otro equipo
                Player* plyr = nullptr;
                if(this->group == 1) { 
                    plyr = teamB.at(rand() % numplayers);
                } else if(this->group == 2) { 
                    plyr = teamA.at(rand() % numplayers); 
                }
                float dx = plyr->GetX()-this->x;
                float dy = plyr->GetY()-this->y;
                float angle = atan(abs(dx/dy));
                float nvx = sin(angle) * vbullet;
                float nvy = cos(angle) * vbullet;
                if(dx < 0) { nvx *= -1; }
                if(dy < 0) { nvy *= -1; }
                this->atktime = time(0) + 1.2;
                Bullet* bullet = new Bullet(this->group,
                                            this->id,
                                            this->x,
                                            this->y,
                                            nvx,
                                            nvy,
                                            3);
                bullets.push_back(bullet);
                this->nbullets -= 1;
            }
        }
        void AddBullet(void) {
            if(this->nbullets < numbullets) { 
                this->nbullets += 1; 
            }
        }
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
        void RestartPlayer(void) {
            this->health = phealth;
            this->nbullets = numbullets;
            this->r = rand() % 10 + 20;
            this->x = rand() % (SWIDTH - (int)this->r*2) + (this->r*2);
            this->y = rand() % (SHEIGHT - (int)this->r*2) + (this->r*2);
        }
        ~Player() {}
};

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
    for(int i = 0;i < numplayers;i++) {
        int radius = rand() % 10 + 20;
        Player* plyr = new Player(i,1, // id, group
                                 rand() % (SWIDTH - radius*2) + (radius*2),
                                 rand() % (SHEIGHT - radius*2) + (radius*2),
                                 radius);
        teamA.push_back(plyr);
    }
    // Inicializar equipo B
    for(int i = 0;i < numplayers;i++) {
        int radius = rand() % 10 + 20;
        Player* plyr = new Player(i,2, // id, group
                                 rand() % (SWIDTH - radius*2) + (radius*2),
                                 rand() % (SHEIGHT - radius*2) + (radius*2),
                                 radius);
        teamB.push_back(plyr);
    }
    printf("Programa iniciado con exito\n");
};

void Render() {
    #pragma omp parallel
    {
        #pragma omp single
        {
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
        }
        // Renderizar estrellas
        #pragma omp for private(i) shared(obj_stars)
        for(int i = 0;i < obj_stars.size();i++) {
            Star* star = obj_stars[i];
            filledCircleRGBA(renderer, //render 
                        star->GetX(),star->GetY(),star->GetR(), // circulo
                        253, 253, 0, // r,g,b
                        star->GetAlpha()); //alpha
        }
        // Renderizar Equipo A
        #pragma omp for private(j) shared(teamA)
        for(int j = 0;j < teamA.size();j++) {
            Player* plyr = teamA[j];
            filledCircleRGBA(renderer, //render 
                        plyr->GetX(),plyr->GetY(),plyr->GetR(), // circulo
                        255, 255, 255, 255); // r,g,b,alpha
        }
        // Renderizar Equipo B
        #pragma omp for private(k) shared(teamB)
        for(int k = 0;k < teamB.size();k++) {
            Player* plyr = teamB[k];
            filledCircleRGBA(renderer, //render 
                        plyr->GetX(),plyr->GetY(),plyr->GetR(), // circulo
                        13, 141, 254, 255); // r,g,b,alpha
        }
        #pragma omp for private(l) shared(bullets)
        for(int l = 0;l < bullets.size();l++) {
            Bullet* bullet = bullets[l];
            if(bullet->GetGroupId() == 1) {
                filledCircleRGBA(renderer, //render 
                            bullet->GetX(),bullet->GetY(),bullet->GetR(), // circulo
                            255, 0, 0, 255); // r,g,b,alpha
            } else {
                filledCircleRGBA(renderer, //render 
                            bullet->GetX(),bullet->GetY(),bullet->GetR(), // circulo
                            0, 255, 0, 255); // r,g,b,alpha
            }
        }
        #pragma omp single
        {
            SDL_RenderPresent(renderer);
        }
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
    #pragma omp parallel
    {
        // Movimiento de estrellas
        #pragma omp for private(i) shared(obj_stars)
        for(int i =0;i < obj_stars.size();i++) {
            Star* star = obj_stars[i];
            star->UpdateVel();
            // Cuando el ciculo toca un borde se desaparece y aparece una nueva estrella
            if (star->GetX() > SWIDTH - star->GetR()) {
                star->RestartPosition();
            }
        }
        // Equipo A
        #pragma omp for private(j) shared(teamA)
        for(int j = 0;j < teamA.size();j++) {
            Player* plyr = teamA[j];
            if(plyr->IsDead()) {
                plyr->RestartPlayer();
            }
            plyr->UpdatePosition();
            plyr->Shoot();
        }
        // Equipo B
        #pragma omp for private(k) shared(teamB)
        for(int k =0;k < teamB.size();k++) {
            Player* plyr = teamB[k];
            if(plyr->IsDead()) {
                plyr->RestartPlayer();
            }
            plyr->UpdatePosition();
            plyr->Shoot();
        }
        // Balas
        #pragma omp for private(i) shared(bullets,teamB,teamA)
        for(int i = 0;i < bullets.size();i++) {
            Bullet* bullet = bullets.at(i);
            Player* bplyr = nullptr;
            bool cond = false;
            bullet->UpdatePosition();
            if(bullet->GetGroupId() == 1) {
                for(Player* plyr : teamA) {
                    if(plyr->GetId() == bullet->GetParentId()) {
                        bplyr = plyr;
                        break;
                    }
                }
                for(Player* plyr : teamB) {
                    float lradius = sqrt(pow(bullet->GetX()-plyr->GetX(),2.0)+
                                        pow(bullet->GetY()-plyr->GetY(),2.0));
                    if(lradius <= plyr->GetR()) {
                        bplyr->AddBullet();
                        plyr->TakeHit();
                        bullets.erase(bullets.begin()+i);
                        delete bullet;
                        cond = true;
                        break;
                    }
                }
                if(!cond) {
                    if(bullet->GetX() > SWIDTH || bullet->GetX() < 0 ||
                        bullet->GetY() > SHEIGHT || bullet->GetY() < 0) {
                        bplyr->AddBullet();
                        bullets.erase(bullets.begin()+i);
                        delete bullet;
                        break;
                    }
                }
            } else {
                for(Player* plyr : teamB) {
                    if(plyr->GetId() == bullet->GetParentId()) {
                        bplyr = plyr;
                        break;
                    }
                }
                for(Player* plyr : teamA) {
                    float lradius = sqrt(pow(bullet->GetX()-plyr->GetX(),2.0)+
                                        pow(bullet->GetY()-plyr->GetY(),2.0));
                    if(lradius <= plyr->GetR()) {
                        bplyr->AddBullet();
                        plyr->TakeHit();
                        bullets.erase(bullets.begin()+i);
                        delete bullet;
                        cond = true;
                        break;
                    }
                }
                if(!cond) {
                    if(bullet->GetX() > SWIDTH || bullet->GetX() < 0 ||
                        bullet->GetY() > SHEIGHT || bullet->GetY() < 0) {
                        bplyr->AddBullet();
                        bullets.erase(bullets.begin()+i);
                        delete bullet;
                        break;
                    }
                }
            }
        }
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
    for(Bullet* bullet : bullets) {
        delete bullet;
    }
    bullets.clear();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    printf("Programa cerrado con exito\n");
};

int main(int argc, char *argv[]) {
    quit = false;
    static int lst_time = 0;
    numplayers = 1;
    numbullets = 5;
    vbullet = 3;
    phealth = 5;
    num_thds = 1;
    if(argc > 5) {
        numplayers = std::stoi(argv[1],NULL,10);
        if(numplayers < 1) { numplayers = 1; }
        numbullets = std::stoi(argv[2],NULL,10);
        if(numbullets < 1) { numbullets = 1; }
        vbullet = std::stoi(argv[3],NULL,10);
        if(vbullet < 1) { vbullet = 1; }
        phealth = std::stoi(argv[4],NULL,10);
        if(phealth < 1) { phealth = 1; }
        num_thds = std::stoi(argv[5],NULL,10);
        if(num_thds == 0) { num_thds = 1; }
    }
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
        #pragma omp parallel num_threads(num_thds)
        Update();
        #pragma omp_parallel num_threads(num_thds)
        Render();
    }
    // limpia la app para cerrarla
    CleanEnvironment();
    return EXIT_SUCCESS;
}