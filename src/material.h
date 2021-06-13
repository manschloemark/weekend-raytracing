#ifndef MATERIAL_H
#define MATERIAL_H

#include "common.h"
#include "hittable.h"
#include "texture.h"
#include "stylized_texture.h"

struct hit_record;

class material {
	public:
		virtual bool scatter(
				const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered ) const = 0;

		virtual color emitted(
				double u, double v, const point3& p
				) const { return color(0, 0, 0); }
};

class lambertian : public material {
	public:
		lambertian(const color& a) : albedo(make_shared<solid_color>(a)) {}
		lambertian(shared_ptr<texture> a) : albedo(a) {}

		virtual bool scatter(
				const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
			) const override {

			auto scatter_direction = rec.normal + random_unit_vector();

			// Catch degenerate scatter direction
   	 	    if (scatter_direction.near_zero())
	  	 		scatter_direction = rec.normal;
					scattered = ray(rec.p, scatter_direction, r_in.time());
					attenuation = albedo->value(rec.u, rec.v, rec.p);
					return true;
		}

	public:
		shared_ptr<texture> albedo;
};

class metal : public material {
	public:
		metal (const color& a, double f) : albedo(make_shared<solid_color>(a)), fuzz(f < 1 ? f : 1) {}
		metal (shared_ptr<texture> a) : albedo(a) {}

		virtual bool scatter(
				const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
			) const override {
			vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
			scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere()), r_in.time();
			attenuation = albedo->value(rec.u, rec.v, rec.p);
			return (dot(scattered.direction(), rec.normal) > 0);
		}

	public:
		shared_ptr<texture> albedo;
		double fuzz;
};

/*
   I don't totally understand the math behind scatter here, but I would like to.
   I know it uses Snell's law, which is
   n * sin(theta) = n' * sin(theta')
   Where n is the index of refraction of the medium the ray is traveling through
   and n' is the index of refraction it is entering.
   theta is the angle the ray hits the point where it changes material.
   We use this formula to determine the angle the ray will travel at after the hit.
*/
class dialectric : public material {
	public:
		dialectric(double index_of_refraction) : ir(index_of_refraction) {}

		virtual bool scatter(
				const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
			) const override {
			attenuation = color(1.0, 1.0, 1.0); // Always 1 since glass absorbs no light.

			double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;
			vec3 unit_direction = unit_vector(r_in.direction());

			double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
			double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

			bool cannot_refract = (refraction_ratio * sin_theta > 1.0);
			vec3 direction;
			if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double()) {
				// Must reflect the ray when refraction ratio * angle is greater than 1
				direction = reflect(unit_direction, rec.normal);
			} else {
				direction = refract(unit_direction, rec.normal, refraction_ratio);
			}
			scattered = ray(rec.p, direction, r_in.time());
			return true;
		}
		
	public:
		double ir;

	private:
		static double reflectance(double cosine, double ref_idx) {
			// Shlick's approximation
      auto r0 = (1 - ref_idx) / (1 + ref_idx);
      r0 = r0 * r0;
      return r0 + (1 - r0) * pow((1 - cosine), 5);
		}
};

class diffuse_light : public material {
	public:
		diffuse_light(shared_ptr<texture> a) : emit(a) {}
		diffuse_light(color c) : emit(make_shared<solid_color>(c)) {}

		virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
			return false;
		}

		virtual color emitted(double u, double v, const point3& p) const override {
			return emit->value(u, v, p);
		}

	public:
		shared_ptr<texture> emit;
};

class diffuse_light_dim_edges : public material {
	public:
		diffuse_light_dim_edges(shared_ptr<texture> a) : emit(a) {}
		diffuse_light_dim_edges(color c) : emit(make_shared<solid_color>(c)) {}

		virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
			// So the dimming part of this works, the only real issue is that the current architecture is not built
			// for something like this.
			// Since this a scatter method, I'm supposed to redirect the light somewhere else.
			// But I don't actaully want to scatter, I just want to dim the light then the viewing ray is looking at a
			// angle.
			if (random_double() > 0.8)
				return false;
			vec3 viewer = r_in.origin() + r_in.direction();
			double angle = acos(dot(viewer, rec.normal) / viewer.length());
			double pi_over_two = pi / 2.0;
			angle = fmod(fabs(angle), pi_over_two) / pi_over_two;
			double dim = smoothstep(-0.1, 0.35, angle); 
			if (dim == 1.0)
				return false;
			attenuation = color(-1, -1, -1) * (1.0 - dim);
			scattered = r_in;
			return true;
		}

		virtual color emitted(double u, double v, const point3& p) const override {
			return emit->value(u, v, p);
		}

	public:
		shared_ptr<texture> emit;
};

class isotropic : public material {
	public:
		isotropic(color c) : albedo(make_shared<solid_color>(c)) {}
		isotropic(shared_ptr<texture> a) : albedo(a) {}

		virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
			const override {
				scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
				attenuation = albedo->value(rec.u, rec.v, rec.p);
				return true;
			}

	public:
		shared_ptr<texture> albedo;
};

#endif
