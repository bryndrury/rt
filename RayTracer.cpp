// Created on iPad.

#include <iostream>
#include <fstream>
#include <thread>
#include <ctime>

#include "limits.h"
#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"


// Image Parameters
const auto aspect_ratio = 16.0 / 9;
const int image_width = 250;
const int image_height = static_cast<int>( image_width / aspect_ratio );
const int samples_per_pixel = 5000;
const int max_depth = 5;

// Screen
struct screen{
    int s[image_width][image_height][3] = { 0 };
} screen;

int render_pixel(int i,int j, hittable_list world, camera cam);

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

int main() {

    // World
    hittable_list world;
    world.add( make_shared<sphere>( point3(0, 0, -1), 0.5 ) );
    world.add( make_shared<sphere>( point3(0, -100.5, -1), 100 ) );


    // Camera Parameters
    camera cam;

    // Renderer
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    time_t start = time(NULL);
    for (int j = image_height - 1; j >= 0 ; j--)
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;

        for (int i = 0; i < image_width; i++)
        {

            std::thread one(render_pixel,i,j,world,cam);

            one.join();

            //render_pixel(i,j,world,cam);


            /*
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

            // Write the translated [0, 255] value of each clor component.
            screen.s[i][j][0] = static_cast<int>(255.999 * clamp(r, 0.0, 0.999));
            screen.s[i][j][1] = static_cast<int>(255.999 * clamp(g, 0.0, 0.999));
            screen.s[i][j][2] = static_cast<int>(255.999 * clamp(b, 0.0, 0.999));
            */
        }
    }

    for (int j = image_height - 1; j >= 0 ; j--) {
        for (int i = 0; i < image_width; i++) {
            std::cout << screen.s[i][j][0] << ' '
                      << screen.s[i][j][1] << ' '
                      << screen.s[i][j][2] << '\n';
        };
    };

    std::cerr << "\nDone." << std::endl;
    time_t finish = time(NULL);
    std::cerr << "Time Taken: " << (finish-start) << " Seconds" << std::endl;
    return 0;
}


int render_pixel(int i,int j, hittable_list world, camera cam) {
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

    // Write the translated [0, 255] value of each clor component.
    screen.s[i][j][0] = static_cast<int>(255.999 * clamp(r, 0.0, 0.999));
    screen.s[i][j][1] = static_cast<int>(255.999 * clamp(g, 0.0, 0.999));
    screen.s[i][j][2] = static_cast<int>(255.999 * clamp(b, 0.0, 0.999));

    return 0;
}
