#ifndef SCENES_H
#define SCENES_H

#include "common.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "moving_sphere.h"

hittable_list random_scene() {
	hittable_list world;

	auto ground_texture = make_shared<checker_texture>(color(1.0, 1.0, 1.0), color(0.2, 0.5, 0.34), 20.0);
	auto mat_ground = make_shared<lambertian>(ground_texture);

	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, mat_ground));

	for (int a = -11; a < 11; a++)
	{
	  for (int b = -11; b < 11; b++)
	  {
	    double radius = 0.4 * random_double(0.9, 0.99);
	    point3 center = point3(a + 0.9 * random_double(), radius, b + 0.9 * random_double());

	    if (random_double() > 0.66) continue;

	    double material = random_double();

	    if (material <= 0.7) // Diffuse (lambertian)
	    {
	      auto albedo = color::random() * color::random();
	      auto mat = make_shared<lambertian>(albedo);
	      auto center2 = center + vec3(0, random_double(0, 0.5), 0);
	      auto random_sphere = make_shared<moving_sphere>(center, center2, 0.0, 1.0, radius, mat);
	      world.add(random_sphere);
      }
	    else if (material <= 0.85) // Reflective (metal)
	    {
	      auto albedo = color::random();
	      auto fuzziness = random_double();
	      auto mat = make_shared<metal>(albedo, fuzziness);
	      auto random_sphere = make_shared<sphere>(center, radius, mat);
				world.add(random_sphere);
      }
	    else // Refractive (dialectric)
	    {
	      auto incidence_of_refraction = random_double(0.5, 1.9);
	      auto mat = make_shared<dialectric>(incidence_of_refraction);
	      auto random_sphere = make_shared<sphere>(center, radius, mat);
	      world.add(random_sphere);
	    }
    }
  }
	auto mat_one = make_shared<dialectric>(1.5);
	auto mat_two = make_shared<metal>(color(0.6, 0.7, 0.8), 0.0);
	auto mat_three = make_shared<lambertian>(color(0.25, 0.8, 0.55));

	world.add(make_shared<sphere>(point3(-2, 1, 0), 1.0, mat_one));
	world.add(make_shared<sphere>(point3(2, 1, -2), 1.0, mat_two));
	world.add(make_shared<sphere>(point3(-5, 1, -4), 1.0, mat_three));

	return world;
}

hittable_list material_demo_scene()
{

	// World / Scene
  auto world = random_scene();
	// Scene from book
  auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
  auto material_center = make_shared<lambertian>(color(0.7, 0.3, 0.3));
  auto material_left = make_shared<dialectric>(1.5);
  auto material_right = make_shared<metal>(color(0.8, 0.8, 0.8), 1.0);

  world.add(make_shared<sphere>(point3(0.0, -100.5, -1), 100, material_ground));
  world.add(make_shared<sphere>(point3(-1.0, 0.0, -1), 0.5, material_left));
  world.add(make_shared<sphere>(point3(-1.0, 0.0, -1), -0.45, material_left));
  world.add(make_shared<sphere>(point3(0.0, 0.0, -1), 0.5, material_center));
  world.add(make_shared<sphere>(point3(1.0, 0.0, -1), 0.5, material_right));

  auto charcoal = make_shared<lambertian>(color(0.2, 0.2, 0.2));
	auto stainless_steel = make_shared<metal>(color(0.7, 0.7, 0.7), 1.0);
	auto copper = make_shared<metal>(color(0.8, 0.55, 0.2), 0.55);
	auto glass = make_shared<dialectric>(1.5);
	auto earth = make_shared<lambertian>(make_shared<image_texture>("../resources/earth.jpg"));

	world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.4, glass));
	world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.4, stainless_steel));
	world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.4, copper));
	world.add(make_shared<sphere>(point3(0.0, 1.0, -1.0), 0.4, charcoal));
	world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, earth));

	return world;
}

hittable_list two_spheres() {
	hittable_list objects;

	auto checker1 =  make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9), 5.0);
	auto checker2 =  make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9), 20.0);
	objects.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker1)));
	objects.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker2)));

	return objects;
}

hittable_list two_perlin_spheres() {
	hittable_list objects;

	auto pertext =  make_shared<marbled_noise_texture>(4.0);
	objects.add(make_shared<sphere>(point3(0, -1000, -1), 1000, make_shared<lambertian>(pertext)));
	objects.add(make_shared<sphere>(point3(0, 2, -1), 2, make_shared<lambertian>(pertext)));

	return objects;
}

hittable_list two_turbulent_perlin_spheres() {
	hittable_list objects;

	auto pertext =  make_shared<turbulent_noise_texture>(4.0);
	objects.add(make_shared<sphere>(point3(0, -1000, -1), 1000, make_shared<lambertian>(pertext)));
	objects.add(make_shared<sphere>(point3(0, 2, -1), 2, make_shared<lambertian>(pertext)));

	return objects;
}

hittable_list earth() {
	auto earth_texture = make_shared<image_texture>("../resources/earth.jpg");
	auto earth_surface = make_shared<lambertian>(earth_texture);
	auto globe = make_shared<sphere>(point3(0, 0, -5), 1, earth_surface);

	return hittable_list(globe);
}

hittable_list my_textures() {
	hittable_list objects;

	auto wave = make_shared<wave_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
	auto coord = make_shared<coordinate_texture>(color(1.0, 1.0, 1.0));
	auto norm = make_shared<normal_texture>();

	objects.add(make_shared<sphere>(point3(-1, 0, -2), 0.5, make_shared<lambertian>(wave)));
	objects.add(make_shared<sphere>(point3(1, 0, -2), 0.5, make_shared<lambertian>(coord)));
	objects.add(make_shared<sphere>(point3(0, 0, -2), 0.5, make_shared<lambertian>(norm)));

	return objects;
}

hittable_list view_texture(shared_ptr<texture> t) {
	hittable_list objects;

	objects.add(make_shared<sphere>(point3(0, 0, -1011), 1000.5, make_shared<lambertian>(t)));

	return objects;


}
#endif
