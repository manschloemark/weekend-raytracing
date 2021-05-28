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
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int samples_per_pixel = 100;
	const int max_depth = 50;

	// World / Scene
	timer t;
	t.start();
  //auto world = random_scene();
  hittable_list world;
  auto wave = make_shared<wave_texture>(color(1, 0, 0), color(0, 1, 0));
  auto mat = make_shared<lambertian>(wave);
  world.add(make_shared<sphere>(point3(0, 0, -1), 0.5, mat));
  t.stop();
  std::cerr << "It took " << t.duration_ms() << " milliseconds to load the scene.\n";
  t.start();
  bvh_node bvh(world, 0.0, 1.0);
  t.stop();
  std::cerr << "It took " << t.duration_ms() << " milliseconds to create the bounding volume hierarchy.\n";

	// Camera
  //point3 lookfrom(12, 2, 1);
  point3 lookfrom(1, 0, -1);
  point3 lookat(0, 0, -1);
  vec3 vup(0, 1, 0);
  double vfov = 60;
  auto dist_to_focus = 1.0;
  auto aperture = 0.1;
  camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

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
