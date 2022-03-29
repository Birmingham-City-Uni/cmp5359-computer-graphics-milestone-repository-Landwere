#pragma once
#include "common.h"
#include "geometry.h"
#include "hittable.h"

struct hit_record;
class material {
public: 
	virtual bool scatter(const Ray& r_in, const hit_record& rec, Colour& attenuation, Ray& scattered) const = 0;
};

class lambertian : public material{
public:
	lambertian(const Colour& a) : albedo(a) {}

	virtual bool scatter(const Ray& r_in, const hit_record& rec, Colour& attenuation, Ray& scattered) const override
	{
		auto scatter_direction = rec.normal + Vec3f().random_in_unlit_sphere();
		//catch degenerate scatter direction
		if (scatter_direction.near_zero())
			scatter_direction = rec.normal;
		scattered = Ray(rec.p, scatter_direction);
		attenuation = albedo;
		return true;
	}

public:
	Colour albedo;
};

Vec3f reflect(const Vec3f& v, const Vec3f& n)
{
	return v - 2 * v.dotProduct(n) * n;
}

class metal : public material {
public:
	metal(const Colour& a) : albedo(a) {}

	virtual bool scatter(const Ray& r_in, const hit_record& rec, Colour& attenuation, Ray& scattered) const override
	{
		Vec3f reflected = reflect(r_in.direction().normalize(), rec.normal);
		scattered = Ray(rec.p, reflected);
		attenuation = albedo;
		return (scattered.direction().dotProduct(rec.normal) > 0);
	}
public: 
	Colour albedo;
};