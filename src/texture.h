#ifndef TEXTURE_H
#define TEXTURE_H

#include "common.h"
#include "perlin.h"

class texture {
    public:
        virtual color value(double u, double v, const point3& p) const = 0;
};

class solid_color : public texture {
    public:
        solid_color() {}
        solid_color(color c) : color_value(c) {}

        solid_color(double red, double green, double blue) : solid_color(color(red, green, blue)) {}

        virtual color value(double u, double v, const vec3& p) const override {
            return color_value;
        }

    private:
        color color_value;
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

class noise_texture : public texture {
    public:
        noise_texture() {}
        noise_texture(double sc) : scale(sc) {}

        virtual color value(double u, double v, const point3& p) const override {
            return color(1, 1, 1) * noise.noise(p);
        }
    
    public:
        perlin noise;
        double scale;
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
class normal_texture : public texture {
    public:
        normal_texture() {}

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

#endif