/* Stylized Textures
 * Mark Schloeman
 *
 * I find it fun to write textures and mess around with them but I don't want to pollute texture.h
 * with these classes. Most of these are coded hastily and without really thinking about the cost.
 * So I figured I'd make an extra file for them.
 *
 * I'll continue to use texture.h for simple textures, like solid_color or checkered_texture,
 * textures that are very general, like normal_texture, and textures from the book.
 *
 * This file will contain my personal textures that I make for fun.
 */
#ifndef STYLIZED_TEXTURE_H
#define STYLIZED_TEXTURE_H

#include "common.h"
#include "perlin.h"
#include "rt_stb_image.h"
#include "texture.h"

// Keeping this one as a happy little accident :)
class broken_marbled_noise_texture : public texture {
    public:
        broken_marbled_noise_texture() {}
        broken_marbled_noise_texture(double sc) : scale(sc) {} 

        virtual color value(double u, double v, const point3& p) const override {
            return color(1, 1, 1) * 0.5 * cos(p.length()) * (1.0 + sin(scale*p.y() + 10.0 * noise.turbulence(p)));
        }
    
    public:
        perlin noise;
        double scale;
};

class rocky_surface_texture : public texture {
	public:
		rocky_surface_texture() {}
		rocky_surface_texture(shared_ptr<texture> t1, shared_ptr<texture> t2, double sc) : col1(t1), col2(t2), scale(sc) {}
		rocky_surface_texture(color c1, color c2, double sc) : col1(make_shared<solid_color>(c1)), col2(make_shared<solid_color>(c2)), scale(sc) {}

		virtual color value(double u, double v, const point3& p) const override {
			double turb = sin(scale * 10.0 * noise.turbulence(p));
			turb *= cos(scale * noise.turbulence(point3(p.z(), p.x(), p.y())));
			color val(0, 0, 0);
			if (turb > 0)
				val = col1->value(u, v, p) * turb;
			else
				val =  col2->value(u, v, p) * fabs(turb);
			return val;
		}

	public:
		perlin noise;
		double scale;
		shared_ptr<texture> col1;
		shared_ptr<texture> col2;
};

class dry_planet_texture : public texture {
	public:
		dry_planet_texture() {}
		dry_planet_texture(shared_ptr<texture> t1, shared_ptr<texture> t2, double sc) : col1(t1), col2(t2), scale(sc) {}
		dry_planet_texture(color c1, color c2, double sc) : col1(make_shared<solid_color>(c1)), col2(make_shared<solid_color>(c2)), scale(sc) {}

		virtual color value(double u, double v, const point3& p) const override {
			double noise_in = u + v + noise.turbulence(scale * p);
			double n = 0.5 * (1.0 + sin(pi * noise_in));

			color val(0, 0, 0);
			// There's gotta be a better way to do this. I want it to dradually change from color A to color B when turb is ~0.
			return mix(col1->value(u, v, p), col2->value(u, v, p), n);
		}

	public:
		perlin noise;
		double scale;
		shared_ptr<texture> col1;
		shared_ptr<texture> col2;
};

class dotted : public texture {
	public:
		dotted() {}
		dotted(shared_ptr<texture> t1, shared_ptr<texture> t2, double u_pd, double v_pd, double r) : col1(t1), col2(t2), u_period(1.0 / u_pd), v_period(1.0 / v_pd) {
			radius = r * fmin(fabs(u_period), fabs(v_period));
		}
		dotted(color c1, color c2, double u_pd, double v_pd, double r) : col1(make_shared<solid_color>(c1)), col2(make_shared<solid_color>(c2)), u_period(1.0 / u_pd), v_period(1.0 / v_pd) {
			radius = r * fmin(fabs(u_period), fabs(v_period));
		}

		virtual color value(double u, double v, const point3& p) const override {
			u = (u_period <= 0.0) ? u : fmod(u, u_period);
			v = (v_period <= 0.0) ? v : fmod(v, v_period);
			u = 2.0 * u - u_period;
			v = 2.0 * v - v_period;
			double r = sqrt(u * u + v * v);

			return mix(col1->value(u, v, p), col2->value(u, v, p), smoothstep(radius * 0.99, radius * 1.01, r));
		}

	public:
		shared_ptr<texture> col1;
		shared_ptr<texture> col2;
		double u_period, v_period, radius;
};

// Since the ray tracer takes multiple samples of everything this
// texture will look different every time and also the 'stars' will appear
// faint because the samples are averaged.
// A workaround for that is to choose a really low fequency and a really high brightness.
class random_stars : public texture {
	public:
		random_stars() {}
		random_stars(double f, double b) : frequency(f), brightness(b) {}

		virtual color value(double u, double v, const point3& p) const override {
			// Determine color
			color c(0, 0, 0);
			if (random_double() < frequency)
				c = color::random(0.9, 1.0) * brightness;
			return c;
		}

	public:
		perlin noise;
		double frequency, brightness;
};

class warped_dots : public texture {
	public:
		warped_dots() {}
		warped_dots(shared_ptr<texture> t1, shared_ptr<texture> t2, double u_pd, double v_pd, double sc, double r) : col1(t1), col2(t2), u_period(1.0 / u_pd), v_period(1.0 / v_pd), scale(sc) {
			radius = r * fmin(fabs(u_period), fabs(v_period));
		}
		warped_dots(color c1, color c2, double u_pd, double v_pd, double sc, double r) : col1(make_shared<solid_color>(c1)), col2(make_shared<solid_color>(c2)), u_period(1.0 / u_pd), v_period(1.0 / v_pd), scale(sc) {
			radius = r * fmin(fabs(u_period), fabs(v_period));
		}

		virtual color value(double u, double v, const point3& p) const override {
			u += u_period * sin(pi * noise.noise(scale * p));
			v += v_period * sin(pi * noise.noise(scale * p));
			u = (u_period <= 0.0) ? u : fmod(u, u_period);
			v = (v_period <= 0.0) ? v : fmod(v, v_period);
			u = 2.0 * u - u_period;
			v = 2.0 * v - v_period;
			double r = sqrt(u * u + v * v);
			//double n = radius * noise.noise(scale * p);
			//r += n;

			return mix(col1->value(u, v, p), col2->value(u, v, p), smoothstep(radius * 0.9, radius * 1.1, r));
		}

	public:
		perlin noise;
		shared_ptr<texture> col1;
		shared_ptr<texture> col2;
		double u_period, v_period, radius, scale;
};

class craggly_line : public texture {
	public:
	 craggly_line() {}
	 craggly_line(shared_ptr<texture> t1, shared_ptr<texture> t2, double u_pd, double v_pd, double sc) : col1(t1), col2(t2), u_period(u_pd), v_period(v_pd), scale(sc) {}
	 craggly_line(color c1, color c2, double u_pd, double v_pd, double sc) : col1(make_shared<solid_color>(c1)), col2(make_shared<solid_color>(c2)), u_period(u_pd), v_period(v_pd), scale(sc) {}

		virtual color value(double u, double v, const point3& p) const override {
			double x = u + scale * noise.turbulence(p * u);
			double y = v + scale * noise.turbulence(p * v);

			return mix(col1->value(u, v, p), col2->value(u, v, p), fabs(sin(u_period * x * pi) + sin(v_period * y * pi)));

		}
	public:
		perlin noise;
		shared_ptr<texture> col1;
		shared_ptr<texture> col2;
		double u_period, v_period, scale;
};
class testing_texture : public texture {
	public:
		testing_texture() {}
		testing_texture(shared_ptr<texture> t1, shared_ptr<texture> t2, double u_pd, double v_pd, double sc) : col1(t1), col2(t2), u_period(u_pd), v_period(v_pd), scale(sc) {}
		testing_texture(color c1, color c2, double u_pd, double v_pd, double sc) : col1(make_shared<solid_color>(c1)), col2(make_shared<solid_color>(c2)), u_period(u_pd), v_period(v_pd), scale(sc) {}

		virtual color value(double u, double v, const point3& p) const override {
			// I'm looking for a funciton that is very squiggly, less uniform than trig functions, and has branches. Hm.
			double u_value = u_period * (u * u * (3 - 2 * u));
			double v_value = v_period * (v * (2 + v));
			//double noise_in =  u_value + v_value + scale * noise.turbulence(p);
			//double n = 0.5 * (1.0 + sin(pi * noise_in));
			double line_noise = fabs(sin(pi * u_value) * cos(pi * v_value) * tan(pi * scale * noise.turbulence(p)));

			double n = noise.turbulence(p);

			color val = mix(col1->value(u, v, p), col2->value(u, v, p), n);
			return val * line_noise;
			//return col1->value(u, v, p) * line_noise;
		}

	public:
		perlin noise;
		double u_period, v_period, scale;
		shared_ptr<texture> col1;
		shared_ptr<texture> col2;
};

// This texture produced a neat circle that looks like it could be the big red dot (saturn? Or is that jupiter lol?)
class potential_saturn_texture : public texture {
	public:
		potential_saturn_texture() {}
		potential_saturn_texture(shared_ptr<texture> t1, shared_ptr<texture> t2, double sc) : col1(t1), col2(t2), scale(sc) {}
		potential_saturn_texture(color c1, color c2, double sc) : col1(make_shared<solid_color>(c1)), col2(make_shared<solid_color>(c2)), scale(sc) {}

		virtual color value(double u, double v, const point3& p) const override {
			double turb = sin(scale * p.y() + 10.0 * noise.turbulence(p));
			turb *= tan(scale * p.z()) * noise.turbulence(point3(p.z(), p.x(), p.y()));
			color val(0, 0, 0);
			if (turb > 0)
				val = col1->value(u, v, p) * smoothstep(0.4, 0.8, turb);
			else
				val =  col2->value(u, v, p) * smoothstep(0.4, 0.8, fabs(turb));
			return val;
		}

	public:
		perlin noise;
		double scale;
		shared_ptr<texture> col1;
		shared_ptr<texture> col2;
};

class bullseye_texture : public texture {
	public:
		bullseye_texture() {}
		bullseye_texture(shared_ptr<texture> t1, shared_ptr<texture> t2, double sc) : col1(t1), col2(t2), scale(sc) {}
		bullseye_texture(color c1, color c2, double sc) : col1(make_shared<solid_color>(c1)), col2(make_shared<solid_color>(c2)), scale(sc) {}

		virtual color value(double u, double v, const point3& p) const override {
			double turb = sin(scale * p.y() + 10.0 * noise.turbulence(p));
			turb *= tan(p.z());
			color val(0, 0, 0);
			if (turb > 0)
				val = col1->value(u, v, p) * turb;
			else
				val =  col2->value(u, v, p) * fabs(turb);
			return val;
		}

	public:
		perlin noise;
		double scale;
		shared_ptr<texture> col1;
		shared_ptr<texture> col2;
};

// Below are textures I have made myself
class wave_texture : public texture {
    public:
        wave_texture(color c1, color c2, double x_freq = 10.0, double y_freq = 10.0)
        : even(make_shared<solid_color>(c1)), odd(make_shared<solid_color>(c2)), x_frequency(x_freq), y_frequency(y_freq) {}
        wave_texture(shared_ptr<texture> c1, shared_ptr<texture> c2, double x_freq = 10.0, double y_freq = 10.0)
        : even(c1), odd(c2), x_frequency(x_freq), y_frequency(y_freq) {}

        virtual color value(double u, double v, const point3& p) const override {
            auto wave_height = (sin(u * pi * x_frequency) + 1.0) * 0.5;
            //auto wave_height = sin(u * pi) * 0.5;
            //v *= (1.0 / y_freq);
            
            if (fmod(v, 1.0 / y_frequency) > wave_height * (1.0 / 2.0 * y_frequency))
            {
                return odd->value(u, v, p);
            }
            else
            {
                return even->value(u, v, p);
            }

        }

    public:
        double x_frequency, y_frequency;
        shared_ptr<texture> odd;
        shared_ptr<texture> even;
};

#endif
