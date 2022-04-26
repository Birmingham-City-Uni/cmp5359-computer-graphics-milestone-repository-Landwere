#pragma once
#include "hittable.h"
#include "geometry.h"

class sphere : public hittable
{
public:
	sphere() {}
	sphere(Point3f cen, double r, shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {};

	virtual bool hit(const Ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool bounding_box(aabb& output_box) const override;
public:
	Point3f center;
	double radius;
	shared_ptr<material> mat_ptr;
};

bool sphere::hit(const Ray& r, double t_min, double t_max, hit_record& rec) const
{
	Vec3f oc = r.origin() - center;
	auto a = r.direction().norm();
	auto half_b = oc.dotProduct(r.direction());
	auto c = oc.norm() - radius * radius;
	auto discriminant = half_b * half_b - a * c;
	if (discriminant < 0) return false;
	auto sqrtd = sqrt(discriminant);

	//find the nearest root that lies in the acceptable range 
	auto root = (-half_b - sqrtd) / a;
	if (root < t_min || t_max < root)
	{
		root = (-half_b + sqrtd) / a;
		if (root < t_min || t_max < root)
			return false;
	}
	//update hit record data accordingly
	rec.t = root;
	rec.p = r.at(rec.t);
	Vec3f outward_normal = (rec.p - center) / radius;
	rec.set_face_normal(r, outward_normal);
	rec.mat_ptr = mat_ptr;

	return true;
}

inline bool sphere::bounding_box(aabb& output_box) const
{
	output_box = aabb(center - Vec3f(radius, radius, radius), center + Vec3f(radius, radius, radius));
	return true;
}

