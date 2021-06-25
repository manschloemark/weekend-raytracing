#ifndef COLOR_H
#define COLOR_H

#include "common.h"

#include <iostream>
inline color normalize(vec3 &pixel_color, int samples_per_pixel) {
	auto scale = 1.0 / samples_per_pixel;
	pixel_color.e[0] = sqrt(pixel_color.e[0] * scale);
	pixel_color.e[1] = sqrt(pixel_color.e[1] * scale);
	pixel_color.e[2] = sqrt(pixel_color.e[2] * scale);
	return pixel_color;
}

void write_color(std::ostream &out, const color& pixel_color, int samples_per_pixel) {
	auto r = pixel_color.x();
	auto g = pixel_color.y();
	auto b = pixel_color.z();

	/* Moving this to it's own inline function so it can be done in multithreading
	auto scale = 1.0 / samples_per_pixel;
	r = sqrt(scale * r);
	g = sqrt(scale * g);
	b = sqrt(scale * b);
	*/

	out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << " "
	    << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << " "
	    << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << "\n";
}

#endif
