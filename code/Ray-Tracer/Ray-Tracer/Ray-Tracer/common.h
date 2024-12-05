#pragma once
#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>
#include "Ray.h"
//usings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;

//constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.141592653589793285;

//utility functions
inline double degrees_to_radians(double degrees) { return degrees * pi / 180.0; }

inline double random_double()
{
	//returns a random real in [0,1].
	return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max)
{
	//returns a random real in [min, max].
	return min + (max - min) * random_double();
}
inline int random_int(int min, int max)
{
	return static_cast<int>(random_double(min, max + 1));
}


//common headers 
#include "ray.h"
#include "geometry.h"





