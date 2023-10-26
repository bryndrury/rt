// Created on iPad.

#include <iostream>
#include <fstream>
#include <thread>
#include <ctime>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include <omp.h>

#include "limits.h"
#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"

// Image Parameters
const auto aspect_ratio = 16.0 / 9;
const int image_width = 720;
const int image_height = static_cast<int>( image_width / aspect_ratio );
const int samples_per_pixel = 5000;
const int max_depth = 10;

const int defaultThreads = 4;


// int render_pixel(int i,int j, hittable_list world, camera cam);
int render_pixel(SDL_Renderer* renderer , int i,int j, hittable_list world, camera cam);

color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    // If ray bounce limit is exceeded, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    if ( world.hit(r, 0, infinity, rec) ) {
        point3 target = rec.p + rec.normal + random_in_unit_sphere();
        return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth-1);
    }
    vec3 unit_direction = unit_vector( r.direction() );
    auto t = 0.5 * ( unit_direction.y() + 1.0 );
    return ( 1.0 - t ) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

struct screen {
        // bool array of pixels that have been rendered or not
        // if renderer is true, then the pixel has been rendered
        bool s[image_width][image_height];
};

int main(int argc, char* argv[]) {
    omp_set_num_threads(defaultThreads);

    screen is_rendered;

    // Create Window
	SDL_Init( SDL_INIT_VIDEO );
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
    
    // Render Black Screen
	window = SDL_CreateWindow("RayTracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, image_width, image_height, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
	SDL_RenderClear( renderer );
    SDL_RenderPresent( renderer );

    // World
    hittable_list world;
    world.add( make_shared<sphere>( point3(0, 0, -1), 0.5 ) );
    world.add( make_shared<sphere>( point3(0, -100.5, -1), 100 ) );

    // Camera Parameters
    camera cam;

    SDL_Event event;
    Uint32 starting_tick;
	bool running = true;
    bool complete = false;
    int count = 0;

    while (running) {
		starting_tick = SDL_GetTicks();

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
				SDL_Quit();
				break;
			}
		}

        int i = rand() % image_width;
        int j = rand() % image_height;

        if (complete == false)
        {
            for (int j = 0; j < image_height; j++)
            {
                for (int i = 0; i < image_width; i++)
                {
                    render_pixel(renderer, i,image_height-j,world,cam);

                    if (i == image_width-1 && j == image_height-1)
                    {
                        complete = true;
                    }
                }
                std::cout << j << " ";
            }
            SDL_RenderPresent(renderer);
        }
    }
    return 0;
}


int render_pixel(SDL_Renderer* renderer , int i,int j, hittable_list world, camera cam) {
    color pixel_color(0, 0, 0);

    for (int s = 0; s < samples_per_pixel; s++) {
        auto u = (i + random_double()) / (image_width - 1);
        auto v = (j + random_double()) / (image_height - 1);
        ray r = cam.get_ray(u, v);
        pixel_color += ray_color(r, world, max_depth);
    }
    //write_color(std::cout, pixel_color, samples_per_pixel);

    // Save to screen struct
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Divide the color by the number of samples and gamma correct.
    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    r = static_cast<int>(255.999 * clamp(r, 0.0, 0.999));
    g = static_cast<int>(255.999 * clamp(g, 0.0, 0.999));
    b = static_cast<int>(255.999 * clamp(b, 0.0, 0.999));

    SDL_SetRenderDrawColor( renderer, r, g, b, 255 );
    SDL_RenderDrawPoint(renderer, i, image_height - j);
    return 0;
}

