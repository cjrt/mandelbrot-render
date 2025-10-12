#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// window size
#define WIN_W 1920
#define WIN_H 1080

// maximum number of iterations, higher number = more detail[and less performance]
int const MAX_ITER = 600;

// viewport of the screen in the complex plane
typedef struct {
    double centerx, centery; // coordinates of the center of the screen
    double scale; // defines ratio between pixels and complex plane units
} viewPort;

// converts a pixel to a complex number, pixelx, pixely -> complex number (x), complex number (y)
static void pixel_to_complex(viewPort *v, int pixelx, int pixely, double *x, double *y) {

    // conversion formula
    // (WIN_W / 2.0, WIN_H / 2.0) =  the central pixel on the screen
    // (pixelx - WIN_W / 2.0, pixely - WIN_W / 2.0) = how far the pixel is from the center in the x,y
    // muliplied by v->scale to convert the pixel to complex
    // y increases downward
    *x = v->centerx + (pixelx - WIN_W / 2.0) * v->scale;
    *y = v->centery - (pixely - WIN_H / 2.0) * v->scale;
}

// cr, ci = constant complex real number, constant complex imaginary number
// these are the numbers being tested, representing "c" in the original formula
static int mandelbrot(double cr, double ci) {
    double zr = 0.0, zi = 0.0; // put both coordinates to the origin
    int i = 0;
    // iterating the Mandelbrot recurrence
    // zr, zi = current complex real number, current complex imaginary number
    while (zr * zr + zi * zi <= 4.0 && i < MAX_ITER) {
        double tmp = zr * zr - zi * zi + cr;
        // updated/next complex number representing Zn+1
        zi = 2.0 * zr * zi + ci;
        zr = tmp;
        // iterate
        i++;
    }
    return i;
}

static void draw_mandelbrot(SDL_Renderer *ren, viewPort *v) {
    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, WIN_W, WIN_H, 32, SDL_PIXELFORMAT_RGBA32);
    Uint32 *pixels = (Uint32*)surface->pixels;

    // openmp for parallel processing, iterations are independent anyway
    #pragma omp parallel for schedule(dynamic)

    for (int pixely = 0; pixely < WIN_H; ++pixely) {
        for (int pixelx = 0; pixelx < WIN_W; ++pixelx) {
            double x, y;
            pixel_to_complex(v, pixelx, pixely, &x, &y);
            int i = mandelbrot(x, y);

            // colouring the map
            double t = (double)i / MAX_ITER;
            // bernstein polynomial for good blending
            // Colour(t) = 255 x 9 x (1-t) x t^3
            Uint8 r = (Uint8)(9*(1-t)*t*t*t*255);
            Uint8 g = (Uint8)(15*(1-t)*(1-t)*t*t*255);
            Uint8 b = (Uint8)(8.5*(1-t)*(1-t)*(1-t)*t*255);
            pixels[pixely * WIN_W + pixelx] = SDL_MapRGBA(surface->format, r, g, b, 255);
        }
    }

    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surface);
    SDL_RenderCopy(ren, tex, NULL, NULL);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(surface);
}

/*  ############################################################################################################################################
//
//  Render Loop
//
//  ############################################################################################################################################ */

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Mandelbrot Render", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_W, WIN_H, SDL_WINDOW_SHOWN);
    SDL_Renderer *render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    viewPort view = { .centerx = -0.5, .centery = 0.0, .scale = 4.0 / WIN_W };

    int running = 1;
    int mouse_down = 0;
    int last_mx = 0, last_my = 0;

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    running = 0; break;
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_ESCAPE) running = 0;
                    else if (e.key.keysym.sym == SDLK_r) {
                        view.centerx = -0.5; view.centery = 0.0; view.scale = 4.0 / WIN_W;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (e.button.button == SDL_BUTTON_LEFT) {
                        mouse_down = 1;
                        last_mx = e.button.x;
                        last_my = e.button.y;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (e.button.button == SDL_BUTTON_LEFT) mouse_down = 0;
                    break;
                case SDL_MOUSEMOTION:
                    if (mouse_down) {
                        int dx = e.motion.x - last_mx;
                        int dy = e.motion.y - last_my;
                        view.centerx -= dx * view.scale;
                        view.centery += dy * view.scale;
                        last_mx = e.motion.x;
                        last_my = e.motion.y;
                    }
                    break;
                case SDL_MOUSEWHEEL: {
                    int mx, my;
                    SDL_GetMouseState(&mx, &my);
                    double bx, by;
                    pixel_to_complex(&view, mx, my, &bx, &by);
                    double factor = (e.wheel.y > 0) ? 0.8 : 1.25;
                    view.scale *= factor;
                    double ax, ay;
                    pixel_to_complex(&view, mx, my, &ax, &ay);
                    view.centerx += (bx - ax);
                    view.centery += (by - ay);
                } break;
            }
        }

        SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
        SDL_RenderClear(render);
        draw_mandelbrot(render, &view);
        SDL_RenderPresent(render);
    }

    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
