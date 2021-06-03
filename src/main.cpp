#include "common.h"

#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "camera.h"
#include "scenes.h"
#include "bvh.h"

#include "timer.h"

#include <iostream>

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


int main()
{
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
	switch(-1) {
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
	image_height = image_width * aspect_ratio;
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

	for (int j = image_height - 1; j >= 0; --j) {
		std::cerr << "\rScanlines remaining: " << j << " " << std::flush;
		for (int i = 0; i < image_width; ++i) {
#if 0
			double red = 0;
			double green = 0;
			double blue = 0;
			for(int s = 0; s < samples_per_pixel; ++s) {
				auto u = double(i + random_double()) / (image_width - 1);
				auto v = double(j + random_double()) / (image_height - 1);
				ray r = cam.get_ray(u, v);
				color c = ray_color(r, background, bvh, max_depth);
				red += c.x();
				green += c.y();
				blue += c.z();
			}
			write_color(std::cout, color(red, green, blue), samples_per_pixel);
#else
			color pixel_color(0, 0, 0);
			for(int s = 0; s < samples_per_pixel; ++s) {
				auto u = double(i + random_double()) / (image_width - 1);
				auto v = double(j + random_double()) / (image_height - 1);
				ray r = cam.get_ray(u, v);
				pixel_color += ray_color(r, background, bvh, max_depth);
			}
			//#pragma omp ordered
			write_color(std::cout, pixel_color, samples_per_pixel);
#endif
		}
	}
	std::cerr << "\nDone.\n";
	t.stop();
	std::cerr << "Rendered in " << t.duration_s() << " seconds.\n" << std::flush;
}
