#ifndef COMMON_H
#define COMMON_H

#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>

// Using
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility
inline double degrees_to_radians(double degrees)
{
	return degrees * pi / 180.0;
}

inline double random_double()
{
	// returns a double in range [0, 1)
	return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max)
{
	// returns a double in range [min, max)
	return min + (max - min) * random_double();
}

inline double clamp(double x, double min, double max)
{
	if ( x < min ) return min;
	if ( x > max ) return max;
	return x;
}

inline double smoothstep(double e0, double e1, double x)
{
	x = clamp((x - e0) / (e1 - e0), 0.0f, 1.0f);
	return x * x * (3 - 2 * x);
}

inline int random_int(int min, int max)
{
	return static_cast<int>(random_double(min, max + 1));
}

//Common Headers
#include "ray.h"
#include "vec3.h"

#endif
