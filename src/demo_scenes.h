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
#include "triangle.h"
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
	auto norm = make_shared<sphere_normal_texture>();

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

hittable_list triangle_demo() {
	hittable_list objects;

	auto text1 = make_shared<triangle_normal_texture>();
	auto text2 = make_shared<rocky_surface_texture>(color(1, 0, 0), color(0, 0, 1), 0.1);
	auto mat1 = make_shared<lambertian>(text1);
	auto mat2 = make_shared<lambertian>(text2);
	auto ccw_tri = make_shared<triangle>(point3(0, 1, 0), point3(1, 0, 0), point3(-1, 0, 0), false, mat1);
	auto cw_tri = make_shared<triangle>(point3(-1, 0, 0), point3(5, 0, 0), point3(0, -1, 0), false, mat2);
	objects.add(ccw_tri);
	objects.add(cw_tri);

	auto text3 = make_shared<dotted>(color(0, 0, 0), color(0, 1.0, 0.3), 1.0, 1.0, .2);
	auto mat3 = make_shared<metal>(text3, 0.2);
	auto tri3 = make_shared<triangle>(point3(-1, -1, 3), point3(0, 2, -4), point3(2, 1, -5), false, mat3);
	
	objects.add(tri3);
	return objects;
}

hittable_list random_triangles(int num_triangles=50) {
	hittable_list objects;

	for(int i = 0; i < num_triangles; ++i) {
		double texture_value = random_double();
		double material_value = random_double();
		auto text = make_shared<solid_color>(color(0, 0, 0));
		if(texture_value < 0.33) {
			auto text = make_shared<solid_color>(vec3::random(0.0, 1.0));
			if(material_value < 0.25) {
				auto mat = make_shared<lambertian>(text);
				objects.add(make_shared<triangle>(vec3::random(-5.0, 5.0), vec3::random(-5.0, 5.0), vec3::random(-5.0, 5.0), false, mat));
			} else if(material_value < 0.5) {
				auto mat = make_shared<metal>(text, random_double());
				objects.add(make_shared<triangle>(vec3::random(-5.0, 5.0), vec3::random(-5.0, 5.0), vec3::random(-5.0, 5.0), false, mat));
			} else if(material_value < 0.75) {
				auto mat = make_shared<diffuse_light>(text);
				objects.add(make_shared<triangle>(vec3::random(-5.0, 5.0), vec3::random(-5.0, 5.0), vec3::random(-5.0, 5.0), false, mat));
			} else {
				auto mat = make_shared<dialectric>(random_double(0.5, 2.0));
				objects.add(make_shared<triangle>(vec3::random(-5.0, 5.0), vec3::random(-5.0, 5.0), vec3::random(-5.0, 5.0), false, mat));
			}

		} else if(texture_value < 0.66) {
			auto text = make_shared<gradient_noise_texture>(vec3::random(0.0, 1.0), vec3::random(0.0, 1.0), random_double(0.1, 5.0));
			if(material_value < 0.25) {
				auto mat = make_shared<lambertian>(text);
				objects.add(make_shared<triangle>(vec3::random(-5.0, 5.0), vec3::random(-5.0, 5.0), vec3::random(-5.0, 5.0), false, mat));
			} else if(material_value < 0.5) {
				auto mat = make_shared<metal>(text, random_double());
				objects.add(make_shared<triangle>(vec3::random(-5.0, 5.0), vec3::random(-5.0, 5.0), vec3::random(-5.0, 5.0), false, mat));
			} else if(material_value < 0.75) {
				auto mat = make_shared<diffuse_light>(text);
				objects.add(make_shared<triangle>(vec3::random(-5.0, 5.0), vec3::random(-5.0, 5.0), vec3::random(-5.0, 5.0), false, mat));
			} else {
				auto mat = make_shared<dialectric>(random_double(0.5, 2.0));
				objects.add(make_shared<triangle>(vec3::random(-5.0, 5.0), vec3::random(-5.0, 5.0), vec3::random(-5.0, 5.0), false, mat));
			}

		} else {
			auto text = make_shared<coordinate_texture>(vec3::random(0.0, 1.0));
			if(material_value < 0.25) {
				auto mat = make_shared<lambertian>(text);
				objects.add(make_shared<triangle>(vec3::random(-5.0, 5.0), vec3::random(-5.0, 5.0), vec3::random(-5.0, 5.0), false, mat));
			} else if(material_value < 0.5) {
				auto mat = make_shared<metal>(text, random_double());
				objects.add(make_shared<triangle>(vec3::random(-5.0, 5.0), vec3::random(-5.0, 5.0), vec3::random(-5.0, 5.0), false, mat));
			} else if(material_value < 0.75) {
				auto mat = make_shared<diffuse_light>(text);
				objects.add(make_shared<triangle>(vec3::random(-5.0, 5.0), vec3::random(-5.0, 5.0), vec3::random(-5.0, 5.0), false, mat));
			} else {
				auto mat = make_shared<dialectric>(random_double(0.5, 2.0));
				objects.add(make_shared<triangle>(vec3::random(-5.0, 5.0), vec3::random(-5.0, 5.0), vec3::random(-5.0, 5.0), false, mat));
			}
		}
	}
	return objects;
	}

hittable_list triangle_test() {
	hittable_list objects;

	objects.add(make_shared<xz_rect>(-100, 100, -400, 500, 20, make_shared<lambertian>(make_shared<image_texture>("../resources/earth.jpg"))));

	objects.add(make_shared<triangle>(point3(-5, -5, -8), point3(0, 5, -3), point3(5, -5, 2), false, make_shared<metal>(color(0.8, 0.8, 0.8), 1.0)));

	objects.add(make_shared<sphere>(point3(-20, 0, 3), 10, make_shared<lambertian>(color(1, 0, 0))));
	//objects.add(make_shared<triangle>(point3(-20, -20, -40), point3(20, -20, -40), point3(0, 20, -40), false, make_shared<lambertian>(make_shared<checker_texture>(color(0, 1, 0), color(1, 0, 1), 2.0))));

	return objects;
}

hittable_list texture_demo(){
	hittable_list objects;

	auto text = make_shared<rocky_surface_texture>(color(0.8, 0.8, 0.4), color(1.0, 0.1, 0.0), 2.0);
	auto mat = make_shared<diffuse_light>(text);
	auto test_mat = make_shared<diffuse_light_dim_edges>(text, 0.4);
	auto sph = make_shared<sphere>(point3(-2.55, 0, -15), 2.5, mat);
	auto test_sph = make_shared<sphere>(point3(2.55, 0, -15), 2.5, test_mat);
	objects.add(sph);
	objects.add(test_sph);

	return objects;
}

hittable_list glass_and_shadows() {
	hittable_list objects;

	auto light = make_shared<diffuse_light>(color(4, 4, 4));
	//auto light_triangle = make_shared<triangle>(point3(-4, -2, 2), point3(4, -2, 2), point3(0, 2, 2), false, light);
	auto light_wall = make_shared<xy_rect>(-20, 20, -20, 20, 30, light);

	//auto box_material = make_shared<metal>(color(0.9, 0.9, 0.9), 1.0);
	auto box_material = make_shared<lambertian>(color(0.4, 0.4, 0.4));
	auto back_material = make_shared<lambertian>(make_shared<rocky_surface_texture>(color(0.2, 0.2, 0.6), color(0, 0, 0), 4.0));
	//auto back_material = make_shared<lambertian>(make_shared<image_texture>("../resources/earth.jpg"));
	//auto walls = make_shared<box>(point3(-50, -8, 50), point3(50, 25, -30), box_material);
	auto left_wall = make_shared<yz_rect>(-50, 50, -50, 50, -50, box_material);
	auto right_wall = make_shared<yz_rect>(-50, 50, -50, 50, 50, box_material);
	auto back_wall = make_shared<xy_rect>(-50, 50, -50, 50, -25, back_material);

	auto metal_sphere = make_shared<sphere>(point3(-10, 0, -10), 2, make_shared<metal>(color(0.55, 0.55, 0.55), 0.0));

	auto glass_ball = make_shared<sphere>(point3(0, 0, -10), 2, make_shared<dialectric>(1.52));

	auto water_ball = make_shared<sphere>(point3(10, 0, -10), 2, make_shared<dialectric>(1.333));

	//objects.add(light_triangle);
	//objects.add(walls);
	//objects.add(light_sphere);
	objects.add(light_wall);
	objects.add(left_wall);
	objects.add(right_wall);
	objects.add(back_wall);
	objects.add(metal_sphere);
	objects.add(glass_ball);
	objects.add(water_ball);

	return objects;
}

hittable_list ultimate_demo() {
	// This is going to be a 4x4 grid showing off the 4 main hittable shapes
	// and different materials.
	// [Lambertian, Metal, Dialectric, Isotropic(maybe not if it takes too long)
	// |  S  S  S  S |
	// |  T  T  T  T |
	// |  R  R  R  R |
	// |  C  C  C  C |
	hittable_list objects;

	// Light
	auto light = make_shared<triangle>(point3(50, 0, 150), point3(150, 0, 200), point3(110, 100, 150), false, make_shared<diffuse_light>(color(5, 5, 5)));
	objects.add(light);
	// Ground
	auto ground = make_shared<xz_rect>(-65, 65, -65, 65, 0, make_shared<metal>(make_shared<uv_checker_texture>(make_shared<colored_turbulent_noise>(color(0.33, 0.54, 0.85), 0.5), make_shared<colored_turbulent_noise>(color(1.0, 1.0, 1.0), 0.5), 50.0), 0.5));
	objects.add(ground);

	auto left_wall = make_shared<yz_rect>(0, 50, -50, 50, -50, make_shared<metal>(color(0.8, 0.8, 0.8), 0.0));
	objects.add(left_wall);
	auto right_wall = make_shared<xy_rect>(-50, 50, 0, 50, -50, make_shared<metal>(color(0.8, 0.8, 0.8), 0.0));
	objects.add(right_wall);

	double x = -45.0;
	double z = -45.0;
	double y = 10.0;
	double r = 10.0;
	// Spheres
	auto lambertian_sphere = make_shared<sphere>(point3(x, y, z), r, make_shared<lambertian>(color(0.8, 0.6, 0.6)));
	objects.add(lambertian_sphere);
	x += 30.0;
	auto metal_sphere = make_shared<sphere>(point3(x, y, z), r, make_shared<metal>(color(0.6, 0.8, 0.8), 0.1));
	objects.add(metal_sphere);
	x+= 30.0;
	auto glass_sphere = make_shared<sphere>(point3(x, y, z), r, make_shared<dialectric>(1.67));
	objects.add(glass_sphere);
	x+= 30.0;
	auto gas_sphere = make_shared<constant_medium>(make_shared<sphere>(point3(x, y, z), r, make_shared<isotropic>(color(1.0, 1.0, 1.0))), 0.01, color(0.2, 1, 0.2));
	objects.add(gas_sphere);

	x = -45.0;
	z += 30.0;

	// Triangles
	// Gas triangle doesn't really appear visible so I will use different textures instead
	//auto gas_triangle = make_shared<constant_medium>(make_shared<triangle>(point3(-r, -r, 0), point3(r,-r, 0), point3(0, r, 0), false, make_shared<lambertian>(color(1, 1, 1))), 0.0001, color(0.55, 0.66, 0.11));
	auto noise_triangle = make_shared<triangle>(point3(-r, -r, 0), point3(r,-r, 0), point3(0, r, 0), false, make_shared<metal>(make_shared<gradient_noise_texture>(color::random(), color::random(), 8.0), 0.5));
	objects.add(make_shared<translate>(make_shared<rotate_y>(noise_triangle, 45.0), vec3(x, y, z)));
	x += 30.0;
	auto lambertian_triangle = make_shared<triangle>(point3(-r, -r, 0), point3(r, -r, 0), point3(0, r, 0), false, make_shared<lambertian>(color(0.2, 0.55, 0.72)));
	objects.add(make_shared<translate>(make_shared<rotate_y>(lambertian_triangle, 45.0), vec3(x, y, z)));
	x += 30.0;
	auto metal_triangle = make_shared<triangle>(point3(-r, -r, 0), point3(r, -r, 0), point3(0, r, 0), false, make_shared<metal>(color(0.6, 0.42, 0.32), 0.1));
	objects.add(make_shared<translate>(make_shared<rotate_y>(metal_triangle, 45.0), vec3(x, y, z)));
	x += 30.0;
	auto glass_triangle = make_shared<triangle>(point3(-r, -r, 0), point3(r, -r, 0), point3(0, r, 0), false, make_shared<dialectric>(2.0));
	objects.add(make_shared<translate>(make_shared<rotate_y>(glass_triangle, 45.0), vec3(x, y, z)));

	x = -45.0;
	z += 30.0;
	// Rectangles
	auto glass_rectangle = make_shared<xy_rect>(-r, r, -r, r, 0, make_shared<dialectric>(1.2));
	objects.add(make_shared<translate>(make_shared<rotate_y>(glass_rectangle, 45.0), vec3(x, y, z)));
	x += 30.0;
	//auto gas_rectangle = make_shared<constant_medium>(make_shared<xy_rect>(-r, r, -r, r, 0, make_shared<isotropic>(color(1, 1, 1))), 0.001, color(0.9, 0.66, 0.11));
	auto noise_rectangle = make_shared<xy_rect>(-r, r, -r, r, 0, make_shared<lambertian>(make_shared<colored_turbulent_noise>(color::random(), 16.0)));
	objects.add(make_shared<translate>(make_shared<rotate_y>(noise_rectangle, 45.0), vec3(x, y ,z)));
	x += 30.0;
	auto lambertian_rectangle = make_shared<xy_rect>(-r, r, -r, r, 0, make_shared<lambertian>(color(0.2, 0.55, 0.32)));
	objects.add(make_shared<translate>(make_shared<rotate_y>(lambertian_rectangle, 45.0), vec3(x, y ,z)));
	x += 30.0;
	auto metal_rectangle = make_shared<xy_rect>(-r, r, -r, r, 0, make_shared<metal>(color(0.6, 0.62, 0.32), 0.1));
	objects.add(make_shared<translate>(make_shared<rotate_y>(metal_rectangle, 45.0), vec3(x, y, z)));

	x = -45.0;
	z += 30.0;
	// Boxes
	auto metal_box = make_shared<box>(point3(x - r, y - r, z - r), point3(x + r, y + r, z + r), make_shared<metal>(color(0.6, 0.62, 0.62), 0.1));
	objects.add(metal_box);
	x += 30.0;
	auto glass_box = make_shared<box>(point3(x - r, y - r, z - r), point3(x + r, y + r, z + r), make_shared<dialectric>(1.2));
	objects.add(glass_box);
	x += 30.0;
	auto gas_box = make_shared<constant_medium>(make_shared<box>(point3(x - r, y - r, z - r), point3(x + r, y + r, z + r), make_shared<isotropic>(color(1, 1, 1))), 0.1, color(0.2, 0.46, 0.11));
	objects.add(gas_box);
	x += 30.0;
	auto lambertian_box = make_shared<box>(point3(x - r, y - r, z - r), point3(x + r, y + r, z + r), make_shared<lambertian>(color(0.2, 0.55, 0.32)));
	objects.add(lambertian_box);
	return objects;
}

#endif

