#pragma once
#include "geometry.h"
class Ray
{
public:
	Ray() {}
	Ray(const Point3f& origin, const Vec3f& direction) : orig(origin), dir(direction) {}

	Point3f origin() const { return orig; }
	Vec3f direction() const { return dir; }

	Point3f at(double t) const { return orig + t * dir; }

public:
	Point3f orig;
	Vec3f dir;
};

