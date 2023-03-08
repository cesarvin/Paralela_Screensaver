#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <stdbool.h>

// variables globales
float circle_x = 320;
float circle_y = 240;
float circle_vx = 0.5;
float circle_vy = 0.5;
float circle_radius = 25;

// tamaño de pantalla
int width = 640;
int height = 480;


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
                                          width, 
                                          height, 
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
        printf("Error creating renderer: %s\n", SDL_GetError());
        return 1;
    }

    // int x = 320;
    // int y = 240;
    // int radio = 50;

    // dibuja circulo
    for (int i = 0; i < 360; i++) {
        float rad = i * (M_PI / 180);
        SDL_RenderDrawPoint(renderer, 
                            circle_x + cos(rad) * circle_radius, 
                            circle_y + sin(rad) * circle_radius);
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
        
        // se actualiza la posición
        circle_x += circle_vx;
        circle_y += circle_vy;

        // Cuando el ciculo toca un borde invierte la dirección de la velocidad
        if (circle_x < circle_radius || circle_x > width - circle_radius) {
            circle_vx = -circle_vx;
        }
        if (circle_y < circle_radius || circle_y > height - circle_radius) {
            circle_vy = -circle_vy;
        }

        SDL_SetRenderDrawColor(renderer, // render
                               215, 219, 221,  // r,g,b
                               255); // alpha
        SDL_RenderClear(renderer);
        filledCircleRGBA(renderer, //render 
                         circle_x, circle_y, circle_radius, // circulo
                         82, 190, 128, // r,g,b
                         255); //alpha

        // Actualizar la ventana
        SDL_RenderPresent(renderer);
    }

    // limpia la app para cerrarla
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
