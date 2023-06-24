// This line **must** come **before** including <time.h> in order to
// bring in the POSIX functions such as `clock_gettime() from <time.h>`!
#define _POSIX_C_SOURCE 199309L

#include <errno.h>  // `errno`
#include <stdint.h> // `UINT64_MAX`
#include <stdio.h>  // `printf()`
#include <string.h> // `strerror(errno)`
#include <time.h>   // `clock_gettime()` and `timespec_get()`
#include <SDL2/SDL.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

double micro_sec_to_sec(double micro_sec)
{
    return micro_sec / 1000.00;
}

double ns_to_micro_sec(uint64_t ns)
{
    return ns / 1000.00;
}

double ns_to_millisecond(uint64_t ns)
{
    return (ns / 1e+6);
}

int sec_to_ns(uint64_t sec)
{
    return sec * 1000000000;
}

int now()
{
    uint64_t nanoseconds;
    struct timespec ts;

    int return_code = clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    if (return_code == -1)
    {
        printf("Failed to obtain timestamp. errno = %i: %s\n", errno,
               strerror(errno));
        nanoseconds = -1; // use this to indicate error
    }
    else
    {
        // `ts` now contains your timestamp in seconds and nanoseconds! To
        // convert the whole struct to nanoseconds, do this:
        nanoseconds = sec_to_ns((uint64_t)ts.tv_sec) + (uint64_t)ts.tv_nsec;
    }

    return nanoseconds;
}

SDL_Surface* get_surface(SDL_Window* window)
{
    SDL_Surface* screenSurface = NULL;

    screenSurface = SDL_GetWindowSurface(window);

    return screenSurface;
}

SDL_Window* create_window()
{
    SDL_Window* window = NULL;

    window = SDL_CreateWindow("MouseTestingTools", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (window == NULL)
    {
        printf("Error occured while creating window. Error: %s\n", SDL_GetError());
        return NULL;
    }

    return window;
}

void init()
{
    int success = 1;
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        success = 0;
        printf("SDL couldn't initialize. Error: %s\n", SDL_GetError());
    }

    return success;
}

int main()
{
    int running = 1;
    SDL_Window* window = create_window();

    if (!window)
    {
        return -1;
    }

    SDL_Surface* screenSurface = get_surface(window);
    SDL_Event e;
    uint32_t prev_ts;
    uint64_t t1 = now();
    uint64_t t2;
    double dt;

    int capture_success = SDL_CaptureMouse(SDL_TRUE);

    while(running)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                running = 0;
            }
            if (e.type == SDL_MOUSEMOTION)
            {
                SDL_MouseMotionEvent *motion_event = &e;
                if (motion_event->timestamp != prev_ts)
                {
                    t2 = now();
                    dt = t2 - t1;
                    double dt_ms = ns_to_millisecond(dt);
                    double pollingRate = (1 / dt_ms) * 1000.00;
                    printf("ms1: %ld ms2: %ld\n", t1, t2);
                    printf("Delta time: %f\n", dt_ms);
                    printf("Polling Rate: %f\n", pollingRate);
                    printf("Current TS - Prev TS: %d\n", motion_event->timestamp - prev_ts);
                    t1 = t2;
                    prev_ts = motion_event->timestamp;
                }
            }
        }
    }
    
    return 0;
}