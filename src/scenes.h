#ifndef SCENES_H
#define SCENES_H

#include "common.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "aarect.h"
#include "box.h"
#include "constant_medium.h"
#include "bvh.h"

hittable_list book1_final() {
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


hittable_list earth() {
	auto earth_texture = make_shared<image_texture>("../resources/earth.jpg");
	auto earth_surface = make_shared<lambertian>(earth_texture);
	auto globe = make_shared<sphere>(point3(0, 0, -5), 1, earth_surface);

	return hittable_list(globe);
}

hittable_list cornell_box() {
    hittable_list objects;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(7, 7, 7));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(113, 442, 127, 432, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

	shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
	box1 = make_shared<rotate_y>(box1, 15);
	box1 = make_shared<translate>(box1, vec3(265, 0, 295));
	//objects.add(box1);

	shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
	box2 = make_shared<rotate_y>(box2, -18);
	box2 = make_shared<translate>(box2, vec3(130, 0, 65));
	//objects.add(box2);

	objects.add(make_shared<constant_medium>(box1, 0.01, color(0, 0, 0)));
	objects.add(make_shared<constant_medium>(box2, 0.01, color(1, 1, 1)));

	return objects;
}


hittable_list simple_light() {
	hittable_list objects;

	auto pertext = make_shared<marbled_noise_texture>(4);
	objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
	objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

	auto difflight = make_shared<diffuse_light>(color(4, 4, 4));
	objects.add(make_shared<xy_rect>(3, 5, 2, 4, -2, difflight));
	//objects.add(make_shared<sphere>(point3(3, 4, -2), 1, difflight));

	return objects;
}

hittable_list solar_system() {
	hittable_list objects;
	
	// LIGHTING
	auto scene_light = make_shared<diffuse_light>(color(2, 2, 2));
	auto light_pane = make_shared<xy_rect>(-500, 800, -500, 500, 510, scene_light);
	objects.add(light_pane);

	// SUN
  //auto sun_light = make_shared<diffuse_light>(color(5, 4.5, 0));
	auto sun_texture = make_shared<lambertian>(make_shared<gradient_noise_texture>(color(0.8, 0.6, 0.01), color(1.0, 0.1, 0.0), 0.1));
	auto sun = make_shared<sphere>(point3(-1050, 0, 0), 1000, sun_texture);
	auto sun_gas_sphere = make_shared<sphere>(point3(-1050, 0, 0), 1050, make_shared<metal>(color(1, 1, 1), 0.0));
	auto sun_gas = make_shared<constant_medium>(sun_gas_sphere, 0.0005, color(0.9, 0.8, 0.5));
	objects.add(sun);
	objects.add(sun_gas);

  // MERCURY
	auto mercury_texture = make_shared<lambertian>(make_shared<rocky_surface_texture>(color(0.7, 0.7, 0.7), color(0, 0, 0), 1.0));
	//auto mercury_texture = make_shared<lambertian>(make_shared<turbulent_noise_texture>(1.0));
	auto mercury = make_shared<sphere>(point3(200, -15, 0), 20, mercury_texture);
	objects.add(mercury);

  // VENUS
	auto venus_texture = make_shared<lambertian>(make_shared<dry_planet_texture>(color(0.8, 0.6, 0.1), color(0.7, 0.4, 0.1), 1.0));
	auto venus = make_shared<sphere>(point3(315, 0, 0), 25, venus_texture);
	objects.add(venus);

  // EARTH
	auto earth_texture = make_shared<image_texture>("../resources/earth.jpg");
	auto earth_surface = make_shared<lambertian>(earth_texture);
	auto terra = make_shared<sphere>(point3(400, 0, 0), 40, earth_surface);
	objects.add(terra);

  // Moon
	auto moon_texture = make_shared<rocky_surface_texture>(color(0.5, 0.5, 0.5), color(0.8, 0.8, 0.8), 3.0);
	auto moon_surface = make_shared<lambertian>(moon_texture);
	auto moon = make_shared<sphere>(point3(452, 60, -2), 6, moon_surface);
	objects.add(moon);

  // Mars
	auto mars_texture = make_shared<lambertian>(make_shared<dry_planet_texture>(color(0.5, 0.45, 0.0), color(0.8, 0.25, 0.0), 2.0));
	auto mars = make_shared<sphere>(point3(500, 12, 0), 30, mars_texture);
	objects.add(mars);
  // Asteroid belt
  // Jupiter
  // Saturn
  // Uranus
  // Neptune
  // Pluto



	return objects;
}

hittable_list book2_final() {
	hittable_list boxes1;

	auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));
	//auto ground = make_shared<lambertian>(make_shared<marbled_noise_texture>(0.1));

	const int boxes_per_side = 20;

	for(int i = 0; i < boxes_per_side; i++) {
		for(int j = 0; j < boxes_per_side; j++) {
			auto w = 100.0;
			auto x0 = -1000.0 + i * w;
			auto z0 = -1000.0 + j * w;
			auto y0 = 0.0;
			auto x1 = x0 + w;
			auto y1 = random_double(1, 101);
			auto z1 = z0 + w;

			boxes1.add(make_shared<box>(point3(x0, y0, z0), point3(x1, y1, z1), ground));
		}
	}

	hittable_list objects;

	objects.add(make_shared<bvh_node>(boxes1, 0, 1));

	auto light = make_shared<diffuse_light>(color(7, 7, 7));
	objects.add(make_shared<xz_rect>(123, 423, 147, 412, 554, light));

	auto center1 = point3(400, 400, 200);
	auto center2 = center1 + vec3(30, 0, 0);

	auto moving_sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
	objects.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50, moving_sphere_material));

	objects.add(make_shared<sphere>(point3(250, 150, 45), 50, make_shared<dialectric>(1.5)));
	objects.add(make_shared<sphere>(point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)));
	
	auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dialectric>(1.5));
	objects.add(boundary);
	objects.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
	// Remove this gas temporarily because it doesn't look great without really high
	// sample rate. (Talking 10000+)
	boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dialectric>(1.5));
	objects.add(make_shared<constant_medium>(boundary, .0001, color(1, 1, 1)));

	auto emat = make_shared<lambertian>(make_shared<image_texture>("../resources/earth.jpg"));
	objects.add(make_shared<sphere>(point3(400, 200, 400), 100, emat));
	auto pertext = make_shared<marbled_noise_texture>(0.1);
	objects.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(pertext)));

	hittable_list boxes2;
	auto white = make_shared<lambertian>(color(.73, .73, .73));
	int ns = 1000;
	for(int j = 0; j < ns; j++) {
		boxes2.add(make_shared<sphere>(point3::random(0, 165), 10, white));
	}

	objects.add(make_shared<translate>(
					make_shared<rotate_y>(
						make_shared<bvh_node>(boxes2, 0.0, 1.0), 15), vec3(-100, 270, 305)
						)
					);

	return objects;
}

hittable_list playground() {
	hittable_list objects;

	auto marble_mat = make_shared<lambertian>(make_shared<marbled_noise_texture>(12));
	auto ground = make_shared<sphere>(point3(0, -1000, 0), 1000, marble_mat);

	auto glass_mat = make_shared<dialectric>(1.5);
	auto glass_ball = make_shared<sphere>(point3(0, 1, 0), 1, glass_mat);

	auto light_mat = make_shared<diffuse_light>(color(3, 3, 3));
	auto light= make_shared<sphere>(point3(-1, 4, -5), 1.0, light_mat);
	//auto light_pane = make_shared<xy_rect>(1.5, 2.5, 1.5, 2.5, 0, light_mat);

	auto metal_mat = make_shared<metal>(color(0.67, 0.67, 0.67), 0.0);
	auto steel_ball = make_shared<sphere>(point3(-3, 0.5, 1), 0.5, metal_mat);

	objects.add(ground);
	objects.add(glass_ball);
	objects.add(light);
	objects.add(steel_ball);

	return objects;
}

#endif
