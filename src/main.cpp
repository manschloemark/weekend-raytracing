#include <iostream>
#include <unistd.h>

#include "common.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "camera.h"
#include "scenes.h"
#include "bvh.h"

// My files
#include "timer.h"
#include "demo_scenes.h"
// I won't be checking for _OPENMP because I know I have it.
// But it's a good idea to think about it for the future.
#ifndef NUM_THREADS
#define NUM_THREADS 1
#endif

color ray_color(const ray& r, const color& background, const hittable& world, int depth)
{
	hit_record rec;

	if (depth <= 0)
	{
		return color(0, 0, 0);
	}

	if (!world.hit(r, 0.001, infinity, rec))
		return background;

	ray scattered;
	color attenuation;

	color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

	if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		return emitted;

	return emitted + attenuation * ray_color(scattered, background, world, depth - 1);
}


int main(int argc, char *argv[])
{
	nice(1);
	// Image
	auto aspect_ratio = 16.0 / 9.0;
	int image_width = 720;
	int image_height = static_cast<int>(image_width / aspect_ratio);

	int samples_per_pixel = 100;
	int max_depth = 50;

	// World / Scene / Camera
	point3 lookfrom(0, 0, 0);
	point3 lookat(0, 0, 1);
	vec3 vup(0, 1, 0);
	auto vfov = 90.0;
	auto aperture = 0.0;
	auto dist_to_focus = (lookat - lookfrom).length();

	hittable_list world;

	color background(0, 0, 0);

	timer t;
	t.start();
	switch(3) {
	case 1:
		samples_per_pixel = 700;

		world = book1_final();
		background = color(0.7, 0.8, 1.0);

		lookfrom = point3(13, 4, 2);
		lookat = point3(0, 0, -2);
		vfov = 40.0;
		break;
	case 2:
		samples_per_pixel = 1000;
		max_depth = 50;
		aspect_ratio = 1.0;
		image_width = 800;

		world = book2_final();
		background = color(0, 0, 0);

		lookfrom = point3(478, 278, -600);
		lookat = point3(278, 278, 0);
		vfov = 40.0;
		break;
	case 3:
		samples_per_pixel = 200;
		aspect_ratio = 32.0/9.0;
		image_width = 1920 * 2;

		world = solar_system();
		background = color(0, 0, 0);
		lookfrom = point3(100, 0, 100);
		lookat = point3(100, 0, 0);
		vfov = 45.0;
		aperture = 0.0;
		//dist_to_focus = (point3(25, 0, 25) - lookfrom).length();
		dist_to_focus = 100;
		break;
	case 4:
		samples_per_pixel = 300;

		world = colored_noise_demo();
		background = color(0.5, 0.8, 0.9);

		lookfrom = point3(0, 0, 10);
		lookat = point3(0, 0, 0);
		vfov = 45.0;
		break;
	case 5:
		samples_per_pixel = 50;
		aspect_ratio = 16.0 / 9.0;
		image_width = 720;

		world = texture_demo();
		background = color(0.7, 0.8, 0.9);

		lookfrom = point3(0, 0, 5);
		lookat = point3(0, 0, -15);
		dist_to_focus = (lookat - lookfrom).length();

		vfov = 30.0;

		break;
	case 6:
		samples_per_pixel = 250;
		aspect_ratio = 16.0 / 9.0;
		image_width = 1920;

		world = rocky_surface_texture_demo();
		background = color(1, 1, 1);
		lookfrom = point3(1920 / 2, 1080 / 2, -5);
		lookat = point3(1920 / 2, 1080 / 2, 0);
		dist_to_focus = 5;
		vfov = 60.0;
		break;
	default:
		samples_per_pixel = 400;
		aspect_ratio = 1.0;
		image_width = 400;

		world = cornell_box();
		background = color(0, 0, 0);

		lookfrom = point3(278, 278, -800);
		lookat = point3(278, 278, 0);
		vfov = 40.0;
		break;
	}
	image_height = static_cast<int>(image_width / aspect_ratio);

	// LAZY COMMAND LINE ARGUMENT CHECK
	// SINCE I ONLY HAVE ONE ARGUMENT RIGHT NOW JUST CHECK THE NUMBER OF ARGs
	// Test render flag, auto sets samples to very small amount.
	if (argc > 1)
		samples_per_pixel = 50;


	camera cam(lookfrom, lookat,
				vup, vfov,
				aspect_ratio,
				aperture, dist_to_focus,
				0.0, 1.0);
	t.stop();
	std::cerr << "It took " << t.duration_ms() << 
				 " milliseconds to load the scene and camera.\n";

	// Create bounding volume hierarchy to speed up collision detection
	t.start();
	bvh_node bvh(world, 0.0, 1.0);
	t.stop();
	std::cerr << "It took " << t.duration_ms() << 
				 " milliseconds to create the bounding volume hierarchy.\n";

	t.start();
	std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

	int chunk_width = static_cast<int>(image_width / 32);
	int chunk_height = static_cast<int>(image_height / 32);
	color *pixels = (color *)malloc((image_width * image_height) * sizeof(color));

	std::cerr << "Rendering...";
	#pragma omp parallel for collapse(2) num_threads(NUM_THREADS)
	for(int j = image_height - 1; j >= 0; j = j - chunk_height)
	{
		for(int i = 0; i < image_width; i = i + chunk_width)
		{
			for(int dj = 0; dj < chunk_height; ++dj)
			{
				int y = j - dj;
				if (y < 0)
					break;
				for (int di = 0; di < chunk_width; ++di)
				{
					int x = i + di;
					if(x >= image_width)
						break;
					color pixel_color(0, 0, 0);
					for (int s = 0; s < samples_per_pixel; ++s)
					{
						auto u = double(x + random_double()) / (image_width - 1);
						auto v = double(y + random_double()) / (image_height - 1);
						ray r = cam.get_ray(u, v);
						pixel_color += ray_color(r, background, bvh, max_depth);
					}
					pixels[(y * image_width) + x] = pixel_color;
				}
			}
		}
	}
	std::cerr << "Writing...";
	for(int j = image_height - 1; j >= 0; --j)
		for(int i = 0; i < image_width; ++i)
			write_color(std::cout, (color)(pixels[(j * image_width) + i]), samples_per_pixel);
	free(pixels);
	std::cerr << "\nDONE.\n";
}
