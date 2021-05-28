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

color ray_color(const ray& r, const hittable& world, int depth)
{
	hit_record rec;
	if (depth <= 0)
	{
	  return color(0, 0, 0);
  }

	if (world.hit(r, 0.001, infinity, rec))
	{
	  ray scattered;
	  color attenuation;
	  if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		  return attenuation * ray_color(scattered, world, depth - 1);
	  return color(0, 0, 0);
  }

	vec3 unit_direction = unit_vector(r.direction());
	auto t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}


int main()
{
	// Image

	const auto aspect_ratio = 16.0 / 9.0;
	const int image_width = 800;
	const int samples_per_pixel = 100;
	const int max_depth = 50;

	// World / Scene / Camera
	hittable_list world;
	point3 lookfrom;
	point3 lookat;
	auto vfov = 40;
	auto aperture = 0.0;
	vec3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	const int image_height = static_cast<int>(image_width / aspect_ratio);

	timer t;
	t.start();
	// Switch statement to pick different scene setups.
	switch(3) {
		case 1:
			world = random_scene();
			lookfrom = point3(13, 5, 3);
			lookat = point3(0, 0, 0);
			vfov = 30;
			aperture = 0.1;
			break;
		case 2:
			world = two_spheres();
			lookfrom = point3(13, 2, 3);
			lookat = point3(0, 0, 0);
			vfov = 20.0;
			break;
		case 3:
			world = two_perlin_spheres();
			lookfrom = point3(13, 3, 3);
			lookat = point3(0, 0, 0);
			vfov = 40.0;
			break;
		case 4:
			world = my_textures();
			lookfrom = point3(0, 0, 1);
			lookat = point3(0, 0, -1);
			vfov = 60;
			aperture = 0.0;
			dist_to_focus = 2.0;
			break;
		case 5:
			world = view_texture(make_shared<noise_texture>());
			lookfrom = point3(0, 0, 0);
			lookat = point3(0, 0, -1);
			vfov = 90;
			aperture = 0.0;
			dist_to_focus = 1.0;
			break;
		case 6:
			world = two_turbulent_perlin_spheres();
			lookfrom = point3(13, 3, 3);
			lookat = point3(0, 0, 0);
			vfov = 40.0;
			break;
		default:
			world = material_demo_scene();
			lookfrom = point3(0, 0, 0);
			lookat = point3(0, 0, -1);
			vfov = 90;
			aperture = 0.0;
			dist_to_focus = 5.0;
			break;
	}
	camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);
	t.stop();
	std::cerr << "It took " << t.duration_ms() << " milliseconds to load the scene and camera.\n";

	// Create bounding volume hierarchy to speed up collision detection
	t.start();
	bvh_node bvh(world, 0.0, 1.0);
	t.stop();
	std::cerr << "It took " << t.duration_ms() << " milliseconds to create the bounding volume hierarchy.\n";

	t.start();
	std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

	for (int j = image_height - 1; j >= 0; --j) {
		std::cerr << "\rScanlines remaining: " << j << " " << std::flush;
		for (int i = 0; i < image_width; ++i) {
			color pixel_color = color(0, 0, 0);
			for(int s = 0; s < samples_per_pixel; ++s) {
				auto u = double(i + random_double()) / (image_width - 1);
				auto v = double(j + random_double()) / (image_height - 1);
				ray r = cam.get_ray(u, v);
				pixel_color += ray_color(r, bvh, max_depth);
			}
			write_color(std::cout, pixel_color, samples_per_pixel);
		}
	}
	std::cerr << "\nDone.\n";
	t.stop();
	std::cerr << "Rendered in " << t.duration_s() << " seconds.\n" << std::flush;
}
