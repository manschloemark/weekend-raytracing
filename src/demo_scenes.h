/* Demo Scenes
 * Mark Schloeman
 *
 * Making this file to keep scenes meant to demonstrate specific features from other scenes.
 * Scenes in this file are mainly meant to show off specific textures or materials
 * so I can save them and reference them later.
 */
#ifndef DEMO_SCENES_H
#define DEMO_SCENES_H

#include "common.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "aarect.h"
#include "box.h"
#include "constant_medium.h"
#include "bvh.h"
#include "scenes.h"

hittable_list material_demo_scene()
{

	// World / Scene
  auto world = book1_final();
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

hittable_list colored_noise_demo() {
	hittable_list objects;

	auto pertext_one =  make_shared<colored_noise_texture>(color(1.0, 0.0, 0.0), 4.0);
	auto pertext_two =  make_shared<colored_noise_texture>(color(0.0, 1.0, 0.0), 2.0);
	auto gradient_pertext = make_shared<gradient_noise_texture>(color(0.8, 0.55, 0.01), color(1.0, 0.1, 0.0), 4.0);
	objects.add(make_shared<sphere>(point3(0, -1000, -1), 1000, make_shared<lambertian>(pertext_one)));
	objects.add(make_shared<sphere>(point3(0, 0, -50), 50, make_shared<lambertian>(gradient_pertext)));
	objects.add(make_shared<sphere>(point3(5, 4, -10), 2, make_shared<lambertian>(pertext_two)));

	return objects;
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

hittable_list light_test() {
	hittable_list objects;

	double sphere_size = 0.35;

	auto light_matr = make_shared<diffuse_light>(color(3, 1, 1));
	auto lightr = make_shared<sphere>(point3(0, 0, 1), sphere_size, light_matr);
	auto light_matg = make_shared<diffuse_light>(color(1, 3, 1));
	auto lightg = make_shared<sphere>(point3(0.866, 0, -0.499), sphere_size, light_matg);
	auto light_matb = make_shared<diffuse_light>(color(1, 1, 3));
	auto lightb = make_shared<sphere>(point3(-0.866, 0, -0.499), sphere_size, light_matb);

	auto mat1 = make_shared<metal>(color(0.8, 0.8, 0.8), 0.0);
	auto ball1 = make_shared<sphere>(point3(0, 0, -1), sphere_size, mat1);
	auto mat2 = make_shared<metal>(color(0.8, 0.8, 0.8), 0.5);
	auto ball2 = make_shared<sphere>(point3(0.866, 0, 0.499), sphere_size, mat2);
	auto mat3 = make_shared<metal>(color(0.8, 0.8, 0.8), 1.0);
	auto ball3 = make_shared<sphere>(point3(0.866, 0, 0.499), sphere_size, mat3);

	auto glass_mat = make_shared<dialectric>(1.5);
	auto glass_ball = make_shared<sphere>(point3(0, 0, 0), sphere_size, glass_mat);
	//auto glass_ball2 = make_shared<sphere>(point3(0, 3, 0), sphere_size, glass_mat);

	auto bg_reflection = make_shared<metal>(color(1.0, 1.0, 1.0), 0.0);
	objects.add(make_shared<sphere>(point3(-4, -1000, -10), 999, bg_reflection));

	objects.add(lightr);
	objects.add(lightg);
	objects.add(lightb);
	objects.add(ball1);
	objects.add(ball2);
	objects.add(ball3);
	objects.add(glass_ball);
	//objects.add(glass_ball2);
	return objects;
}

hittable_list noise_texture_demo() {
	hittable_list objects;

	auto noise1 = make_shared<noise_texture>(1.0);
	auto noise3 = make_shared<noise_texture>(4.0);
	auto noise5 = make_shared<noise_texture>(16.0);
	auto noise7 = make_shared<noise_texture>(64.0);

	auto width  = 1920 / 2;
	auto height = 1080 / 2;

	auto start_x = 0;
	auto start_y = 0;

	auto rect1 = make_shared<xy_rect>(start_x, start_x + width, start_y, start_y + height, 0, make_shared<lambertian>(noise1));
	start_x += width;
	auto rect3 = make_shared<xy_rect>(start_x, start_x + width, start_y, start_y + height, 0, make_shared<lambertian>(noise3));
	start_x = 0;
	start_y += height;
	auto rect5 = make_shared<xy_rect>(start_x, start_x + width, start_y, start_y + height, 0, make_shared<lambertian>(noise5));
	start_x += width;
	auto rect7 = make_shared<xy_rect>(start_x, start_x + width, start_y, start_y + height, 0, make_shared<lambertian>(noise7));

	objects.add(rect1);
	objects.add(rect3);
	objects.add(rect5);
	objects.add(rect7);


	return objects;
}

hittable_list turbulent_noise_texture_demo() {
	hittable_list objects;

	auto noise1 = make_shared<turbulent_noise_texture>(1.0);
	auto noise3 = make_shared<turbulent_noise_texture>(4.0);
	auto noise5 = make_shared<turbulent_noise_texture>(16.0);
	auto noise7 = make_shared<turbulent_noise_texture>(64.0);

	auto width  = 1920 / 2;
	auto height = 1080 / 2;

	auto start_x = 0;
	auto start_y = 0;

	auto rect1 = make_shared<xy_rect>(start_x, start_x + width, start_y, start_y + height, 0, make_shared<lambertian>(noise1));
	start_x += width;
	auto rect3 = make_shared<xy_rect>(start_x, start_x + width, start_y, start_y + height, 0, make_shared<lambertian>(noise3));
	start_x = 0;
	start_y += height;
	auto rect5 = make_shared<xy_rect>(start_x, start_x + width, start_y, start_y + height, 0, make_shared<lambertian>(noise5));
	start_x += width;
	auto rect7 = make_shared<xy_rect>(start_x, start_x + width, start_y, start_y + height, 0, make_shared<lambertian>(noise7));

	objects.add(rect1);
	objects.add(rect3);
	objects.add(rect5);
	objects.add(rect7);


	return objects;
}

hittable_list rocky_surface_texture_demo() {
	hittable_list objects;

	color colA = color(1.0, 1.0, 1.0);
	color colB = color(0, 0, 0);
	auto noise1 = make_shared<rocky_surface_texture>(colA, colB, 1.0);
	auto noise3 = make_shared<rocky_surface_texture>(colA, colB, 4.0);
	auto noise5 = make_shared<rocky_surface_texture>(colA, colB, 16.0);
	auto noise7 = make_shared<rocky_surface_texture>(colA, colB, 64.0);

	auto width  = 1920 / 2;
	auto height = 1080 / 2;

	auto start_x = 0;
	auto start_y = 0;

	auto rect1 = make_shared<xy_rect>(start_x, start_x + width, start_y, start_y + height, 0, make_shared<lambertian>(noise1));
	start_x += width;
	auto rect3 = make_shared<xy_rect>(start_x, start_x + width, start_y, start_y + height, 0, make_shared<lambertian>(noise3));
	start_x = 0;
	start_y += height;
	auto rect5 = make_shared<xy_rect>(start_x, start_x + width, start_y, start_y + height, 0, make_shared<lambertian>(noise5));
	start_x += width;
	auto rect7 = make_shared<xy_rect>(start_x, start_x + width, start_y, start_y + height, 0, make_shared<lambertian>(noise7));

	objects.add(rect1);
	objects.add(rect3);
	objects.add(rect5);
	objects.add(rect7);


	return objects;
}

hittable_list texture_demo(){
	hittable_list objects;

	auto text = make_shared<gradient_noise_texture>(color(0.8, 0.8, 0.4), color(1.0, 0.1, 0.0), 2.0);
	auto mat = make_shared<diffuse_light>(text);
	auto test_mat = make_shared<diffuse_light_dim_edges>(text);
	auto sph = make_shared<sphere>(point3(-2.55, 0, -15), 2.5, mat);
	auto test_sph = make_shared<sphere>(point3(2.55, 0, -15), 2.5, test_mat);
	objects.add(sph);
	objects.add(test_sph);

	return objects;
}
#endif

