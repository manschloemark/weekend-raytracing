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


hittable_list earth_demo() {
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

hittable_list lambertian_cornell_box() {
    hittable_list objects;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

	shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
	box1 = make_shared<rotate_y>(box1, 15);
	box1 = make_shared<translate>(box1, vec3(265, 0, 295));
	objects.add(box1);

	shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
	box2 = make_shared<rotate_y>(box2, -18);
	box2 = make_shared<translate>(box2, vec3(130, 0, 65));
	objects.add(box2);

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

	// Radii of planets (and moon) divided by radius of mercury
	// With some extra shrinkage to the larger planets
	double r_sun     = 285.21767810026387;
	double r_mercury = 1.0 * 2.5;
	double r_venus   = 2.480211081794195 * 2.0;
	double r_earth   = 2.6108179419525066 * 2.0;
	double r_moon    = 0.7117414248021108 * 2.0;
	double r_mars    = 1.3891820580474934 * 2.0;
	double r_jupiter = 28.655013192612138 * 0.45;
	double r_saturn  = 23.868073878627968 * 0.45;
	double r_uranus  = 10.395118733509236 * 0.45;
	double r_neptune = 10.091688654353563 * 0.45;

	// x coordinate objects should lie on
	// These are pretty much random, not scaled from actual distances
	double x_sun = r_sun * -1.5;
	double x_mercury = -5.0;
	double x_venus = 22.0;
	double x_earth = 52.0;
	double x_mars = 78.0;

	double x_jupiter = 116.0;
	double x_saturn = 156.0;
	double x_uranus = 194.0;
	double x_neptune = 220.0;
	
	// LIGHTING
	auto scene_light = make_shared<diffuse_light>(color(0.4, 0.4, 0.4));
	auto light_pane = make_shared<xy_rect>(50, 150, -50, 50, 150, scene_light);
	objects.add(light_pane);

	auto star_bg_texture = make_shared<random_stars>(0.000003, 45.0);
	auto star_bg_mat = make_shared<diffuse_light>(star_bg_texture);
	auto star_bg = make_shared<xy_rect>(-600, 600, -300, 300, -100, star_bg_mat);
	objects.add(star_bg);

	// SUN
	auto sun_texture = make_shared<diffuse_light_dim_edges>(make_shared<gradient_noise_texture>(color(0.8, 0.8, 0.04), color(1.0, 0.1, 0.0), 1.0), 0.8);
	auto sun = make_shared<sphere>(point3(x_sun, 0, 0), r_sun, sun_texture);
	objects.add(sun);

  // MERCURY
	auto mercury_texture = make_shared<image_texture>("../resources/mercury.jpg");
	auto mercury_surface = make_shared<lambertian>(mercury_texture);
	auto mercury = make_shared<sphere>(point3(x_mercury, -2, 0), r_mercury, mercury_surface);
	objects.add(mercury);

  // VENUS
	auto venus_texture = make_shared<image_texture>("../resources/venus.jpg");
	auto venus_surface = make_shared<lambertian>(venus_texture);
	auto venus = make_shared<sphere>(point3(x_venus, 3, 0), r_venus, venus_surface);
	objects.add(venus);

  // EARTH
	auto earth_texture = make_shared<image_texture>("../resources/earth.jpg");
	auto earth_surface = make_shared<lambertian>(earth_texture);
	//auto earth = make_shared<rotate_z>(make_shared<sphere>(point3(x_earth, 0, 0), r_earth, earth_surface), 20);
	auto earth = make_shared<sphere>(point3(x_earth, -5, 0), r_earth, earth_surface);
	objects.add(earth);

  // Moon
	auto moon_texture = make_shared<image_texture>("../resources/moon.jpg");
	auto moon_surface = make_shared<lambertian>(moon_texture);
	auto moon = make_shared<sphere>(point3(x_earth - (r_earth * 0.8), -2, 1.2), r_moon, moon_surface);
	objects.add(moon);

  // Mars
	auto mars_texture = make_shared<image_texture>("../resources/mars.jpg");
	auto mars_surface = make_shared<lambertian>(mars_texture);
	auto mars = make_shared<sphere>(point3(x_mars, 5, 0), r_mars, mars_surface);
	objects.add(mars);
  // Asteroid belt
  // Jupiter
	auto jupiter_texture = make_shared<image_texture>("../resources/jupiter.jpg");
	auto jupiter_surface = make_shared<lambertian>(jupiter_texture);
	auto jupiter = make_shared<sphere>(point3(x_jupiter, 12, 0), r_jupiter, jupiter_surface);
	objects.add(jupiter);
  // Saturn
	auto saturn_texture = make_shared<image_texture>("../resources/saturn.jpg");
	auto saturn_surface = make_shared<lambertian>(saturn_texture);
	auto saturn = make_shared<sphere>(point3(x_saturn, -12, 0), r_saturn, saturn_surface);
	objects.add(saturn);
  // Uranus
	auto uranus_texture = make_shared<image_texture>("../resources/uranus.jpg");
	auto uranus_surface = make_shared<lambertian>(uranus_texture);
	auto uranus = make_shared<sphere>(point3(x_uranus, -4, 0), r_uranus, uranus_surface);
	objects.add(uranus);
  // Neptune
	auto neptune_texture = make_shared<image_texture>("../resources/neptune.jpg");
	auto neptune_surface = make_shared<lambertian>(neptune_texture);
	auto neptune = make_shared<sphere>(point3(x_neptune, 16, 0), r_neptune, neptune_surface);
	objects.add(neptune);
	//
  // Pluto
	// It's a small planet. It can't be seen.

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
	
	/*
	auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dialectric>(1.5));
	objects.add(boundary);
	objects.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
	// Remove this gas temporarily because it doesn't look great without really high
	// sample rate. (Talking 10000+)
	boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dialectric>(1.5));
	objects.add(make_shared<constant_medium>(boundary, .0001, color(1, 1, 1)));
	*/

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
