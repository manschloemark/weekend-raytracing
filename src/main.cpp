#include <iostream>
#include <unistd.h>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

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

	// Default values
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
	switch(1) {
	case 1:
		samples_per_pixel = 10;

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
		samples_per_pixel = 600;
		aspect_ratio = 48.0/9.0;
		image_width = 1920 * 3;

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
	case 7:
		samples_per_pixel = 1000;
		background = color(0.6, 0.8, 0.9);
		world = random_triangles();
		lookfrom = point3(0, 0, 10);
		lookat = point3(0, 0, 0);
		dist_to_focus = 8.0;
		vfov = 60.0;
		break;
	case 8:
		samples_per_pixel = 50;
		background = color(0.6, 0.7, 0.9);
		world = triangle_test();
		lookfrom = point3(0, 0, 20);
		lookat = point3(0, 0, -5);
		dist_to_focus = 23.0;
		vfov = 30.0;
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

	// TODO : implement a better argument parser so I can create new arguments in the future.
	//        (ex specify image size)
	// Since I'm only implementing one command line flag it is triggered by an extra argument existing.
	if (argc > 1) // Quick test render
		samples_per_pixel = 30;

	camera cam(lookfrom, lookat,
				vup, vfov,
				aspect_ratio,
				aperture, dist_to_focus,
				0.0, 1.0);

	t.stop();
	std::cerr << "It took " << t.duration_ms() << 
				 " milliseconds to load the scene and camera.\n";

	// Create bounding volume hierarchy to speed up collision detection
	// Should I leave this here or should I let scene functions create the bvh?
	t.start();
	bvh_node bvh(world, 0.0, 1.0);
	t.stop();
	std::cerr << "It took " << t.duration_ms() << 
				 " milliseconds to create the bounding volume hierarchy.\n";

	t.start();
	std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
	color *pixels = (color *)malloc((image_width * image_height) * sizeof(color));

#if 1 // Render by lines
	std::cerr << "Rendering...\n";
	for(int y = image_height - 1; y >= 0; --y)
	{
		std::cerr << "\r" << y << " lines remaining." << std::flush;
		for(int x = 0; x < image_width; ++x)
		{
			color pixel_color(0, 0, 0);
			for(int s = 0; s < samples_per_pixel; ++s)
			{
				auto u = (x + random_double()) / (image_width - 1);
				auto v = (y + random_double()) / (image_height - 1);
				ray r = cam.get_ray(u, v);
				pixel_color += ray_color(r, background, bvh, max_depth);
			}
			pixel_color = normalize(pixel_color, samples_per_pixel);
			pixels[(y * image_width) + x] = pixel_color;
		}
	}
#else // Render by chunks
	int chunk_width = 32;
	int chunk_height = 32;
	// This is for the chunk progress counter
	int horizontal_chunk_count = (int)(ceil(image_width / (double)chunk_width));
	int vertical_chunk_count = (int)(ceil(image_height / (double)chunk_height));
	int chunks_remaining = horizontal_chunk_count * vertical_chunk_count;
	std::cerr << chunks_remaining << " chunks remaining." << std::flush;

	std::mutex mutex;
	for(int j = image_height - 1; j >= 0; j = j - chunk_height)
	{
		for(int i = 0; i < image_width; i = i + chunk_width)
		{
			for(int dj = 0; dj < std::min(j, chunk_height) ; ++dj)
			{
				int y = j - dj;
				for (int di = 0; di < std::min(image_width - i, chunk_width); ++di)
				{
					int x = i + di;
					color pixel_color(0, 0, 0);
					for (int s = 0; s < samples_per_pixel; ++s)
					{
						auto u = double(x + random_double()) / (image_width - 1);
						auto v = double(y + random_double()) / (image_height - 1);
						ray r = cam.get_ray(u, v);
						pixel_color += ray_color(r, background, bvh, max_depth);
					}
					pixel_color = normalize(pixel_color, samples_per_pixel);
					pixels[(y * image_width) + x] = pixel_color;
				}
			}
			--chunks_remaining;
			std::cerr << "\r" << chunks_remaining << " chunks remaining.";
		}
	}
#endif

	std::cerr << "Writing...";
	for(int j = image_height - 1; j >= 0; --j)
		for(int i = 0; i < image_width; ++i)
			write_color(std::cout, (color)(pixels[(j * image_width) + i]), samples_per_pixel);
	free(pixels);
	std::cerr << "\nDONE.\n";
}
