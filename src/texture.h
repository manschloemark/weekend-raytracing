#ifndef TEXTURE_H
#define TEXTURE_H

#include "common.h"
#include "perlin.h"
#include "rt_stb_image.h"
#include <iostream>

class texture {
    public:
        virtual color value(double u, double v, const point3& p) const = 0;
};


class solid_color : public texture {
    public:
        solid_color() {}
        solid_color(color c) : color_value(c) {}

        solid_color(double red, double green, double blue) : solid_color(color(red, green, blue)) {}

        virtual color value(double u, double v, const point3& p) const override {
            return color_value;
        }

    private:
        color color_value;
};


class image_texture : public texture {
    public:
        const static int bytes_per_pixel = 3;

        image_texture() : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

        image_texture(const char* filename) {
            auto components_per_pixel = bytes_per_pixel;

            data = stbi_load(filename, &width, &height, &components_per_pixel, components_per_pixel);

            if (!data) {
                std::cerr << "ERROR: could not load texture image file '" << filename << "'.\n";
                width = height = 0;
            }

            bytes_per_scanline = bytes_per_pixel * width;
        }

        ~image_texture() {
            delete data;
        }

        virtual color value(double u, double v, const point3& p) const override {
            if (data == nullptr)
                return color(1, 0.5, 1);

            u = clamp(u, 0.0, 1.0);
            v = 1.0 - clamp(v, 0.0, 1.0);

            auto i = static_cast<int>(u * width);
            auto j = static_cast<int>(v * height);

            if (i >= width)  i = width - 1;
            if (j >= height) j = height -1;

            const auto color_scale = 1.0 / 255.0;
            auto pixel = data + (j * bytes_per_scanline) + (i * bytes_per_pixel);

            return color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
        }
    
    private:
        unsigned char *data;
        int width, height;
        int bytes_per_scanline;
};


class checker_texture : public texture {
    public:
        checker_texture(shared_ptr<texture> _even, shared_ptr<texture> _odd, double freq = 10.0) : even(_even), odd(_odd), box_frequency(freq) {}

        checker_texture(color c1, color c2, double freq = 10.0) : even(make_shared<solid_color>(c1)), odd(make_shared<solid_color>(c2)), box_frequency(freq) {}

        virtual color value(double u, double v, const point3& p) const override {
            auto sines = sin(box_frequency * p.x()) * sin(box_frequency * p.y()) * sin(box_frequency * p.z());
            if (sines < 0)
            {
                return odd->value(u, v, p);
            }
            else
            {
                return even->value(u, v, p);
            } 
        }

    public:
        double box_frequency;
        shared_ptr<texture> odd;
        shared_ptr<texture> even;
};

// This is a copy of the checker texture from Peter Shirley's book
// but it uses the uv coordinates instead of xyz so that a flat surface will still have tiles.
class uv_checker_texture : public texture {
    public:
        uv_checker_texture(shared_ptr<texture> _even, shared_ptr<texture> _odd, double freq = 10.0) : even(_even), odd(_odd), box_frequency(freq) {}

        uv_checker_texture(color c1, color c2, double freq = 10.0) : even(make_shared<solid_color>(c1)), odd(make_shared<solid_color>(c2)), box_frequency(freq) {}

        virtual color value(double u, double v, const point3& p) const override {
            auto sines = sin(box_frequency * u) * sin(box_frequency * v);
            if (sines < 0)
            {
                return odd->value(u, v, p);
            }
            else
            {
                return even->value(u, v, p);
            } 
        }

    public:
        double box_frequency;
        shared_ptr<texture> odd;
        shared_ptr<texture> even;
};

class noise_texture : public texture {
    public:
        noise_texture() {}
        noise_texture(double sc) : scale(sc) {}

        virtual color value(double u, double v, const point3& p) const override {
            return color(1, 1, 1) * 0.5 * (1.0 + noise.noise(scale * p));
        }
    
    public:
        perlin noise;
        double scale;
};

class turbulent_noise_texture : public texture {
    public:
        turbulent_noise_texture() {}
        turbulent_noise_texture(double sc) : scale(sc) {}

        virtual color value(double u, double v, const point3& p) const override {
            return color(1, 1, 1) * noise.turbulence(scale * p);
        }

    public:
        perlin noise;
        double scale;
};

class marbled_noise_texture : public texture {
    public:
        marbled_noise_texture() {}
        marbled_noise_texture(double sc) : scale(sc) {} 

        virtual color value(double u, double v, const point3& p) const override {
            return color(1, 1, 1) * 0.5 * (1.0 + sin(scale * p.y() + 10.0 * noise.turbulence(p)));
        }
    
    public:
        perlin noise;
        double scale;
};

// @NOTE make this take a specific noise texture (or any texture, really)
// so I can easily use marbled noise or turbulent etc.
// Should I make a noise file that has functions for all sorts of noise?
class colored_noise_texture : public texture {
    public:
				colored_noise_texture() {}
				colored_noise_texture(shared_ptr<texture> t, double sc) : color_value(t), scale(sc) {}
				colored_noise_texture(color c, double sc) : color_value(make_shared<solid_color>(c)), scale(sc) {}

        virtual color value(double u, double v, const point3& p) const override {
					auto n = 0.5 * (1.0 + noise.noise(scale * p));
					return color_value->value(u, v, p) * (n * n);
				}

		public:
			perlin noise;
			double scale;
			shared_ptr<texture> color_value;
};

class colored_turbulent_noise : public texture {
	public:
				colored_turbulent_noise() {}
				colored_turbulent_noise(shared_ptr<texture> t, double sc) : color_value(t), scale(sc) {}
				colored_turbulent_noise(color c, double sc) : color_value(make_shared<solid_color>(c)), scale(sc) {}

				virtual color value(double u, double v, const point3& p) const override {
					return color_value->value(u, v, p) * (noise.turbulence(scale * p));
				}

	public:
		perlin noise;
		double scale;
		shared_ptr<texture> color_value;
};

class gradient_noise_texture : public texture {
	public:
		gradient_noise_texture() {}
		gradient_noise_texture(shared_ptr<texture> t1, shared_ptr<texture> t2, double sc) : col1(t1), col2(t2), scale(sc) {}
		gradient_noise_texture(color c1, color c2, double sc) : col1(make_shared<solid_color>(c1)), col2(make_shared<solid_color>(c2)), scale(sc) {}

		virtual color value(double u, double v, const point3& p) const override {
			return mix(col1->value(u, v, p), col2->value(u, v, p),
                 noise.turbulence(scale * p));
                        //(noise.turbulence(scale * p) > 0.5) ? 1.0 : 0.0);
		}

	public:
		perlin noise;
		double scale;
		shared_ptr<texture> col1;
		shared_ptr<texture> col2;
};

class coordinate_texture : public texture {
    public:
        coordinate_texture(color c) : color_value(c) {}

        virtual color value(double u, double v, const point3& p) const override {
            return color(color_value.x() * u, color_value.y() * (u + v / 2.0), color_value.z() * v);
        }

    public:
        color color_value;
};

// Note I'm not sure this is quite right but it was fun to work on.
// This only (sort of) works for spheres. How could I make this work on any shape?
// I mean, the easiest way would be to give textures more data... but that would be a lot of work.
// So what else? Make a different texture for different shapes?
class sphere_normal_texture : public texture {
    public:
        sphere_normal_texture() {}

        virtual color value(double u, double v, const point3& p) const override {
            double y_n = fabs(v * 2.0 - 1.0);
            double x_n, z_n;
            double* intpart;
            double frac = modf(u / 0.25, intpart);

            // NOTE: not sure if 0 is x or z.
            if (static_cast<long>(*intpart) % 2 == 0)
            {
                x_n = 1.0 - frac;
                z_n = frac;
            }
            else
            {
                z_n = 1.0 - frac;
                x_n = frac;
            }
            x_n *= 1.0 - y_n;
            z_n *= 1.0 - y_n;

            return color(x_n, y_n, z_n);
        }
};

class triangle_normal_texture : public texture {
	public:
		triangle_normal_texture() {}

		virtual color value(double u, double v, const point3& p) const override {
			color c(u, v, 1 - u - v);
			return c;
		}
};

#endif
