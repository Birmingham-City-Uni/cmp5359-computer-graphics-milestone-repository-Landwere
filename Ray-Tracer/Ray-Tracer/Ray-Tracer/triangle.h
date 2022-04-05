#pragma once
#include "hittable.h"
#include "geometry.h"

class triangle : public hittable {

public:
	triangle() {}
	triangle(Point3f vert0, Point3f vert1, Point3f vert2, shared_ptr<material> m) : v0(vert0), v1(vert1), v2(vert2), mat_ptr(m) {
		normal = (v1 - v0).crossProduct(v2 - v0);
	};
	triangle(Point3f vert0, Point3f vert1, Point3f vert2, Vec3f vn, shared_ptr<material> m) : v0(vert0), v1(vert1), v2(vert2), normal(vn), mat_ptr(m) {};

	virtual bool hit(const Ray& r, double t_min, double t_max, hit_record& rec) const override;

public:
	Point3f v0, v1, v2;
	Vec3f normal;
	shared_ptr<material> mat_ptr;
	
};

//ray-triangle intersection:
//from scratchpixel
bool triangle::hit(const Ray& r, double t_min, double t_max, hit_record& rec) const
{
	float thit, t, u, v;

	Vec3<float> v0v1 = v1 - v0;
	Vec3<float> v0v2 = v2 - v0;
	Vec3<float> pvec = r.direction().crossProduct(v0v2);

	float det = pvec.dotProduct(v0v1);
	float kEpsilon = 0.00001;

	//if the determinant is negative the triangle is backfacing 
	//if the determinant is close to 0 the ray misses the triangle
	if (det < kEpsilon) return false;

	float invDet = 1 / det;

	Vec3<float> tvec = r.origin() - v0;
	u = tvec.dotProduct(pvec) * invDet;

	if (u < 0 || u > 1) return false;

	Vec3<float> qvec = tvec.crossProduct(v0v1);
	v = r.direction().dotProduct(qvec) * invDet;
	if (v < 0 || u + v > 1) return false;

	t = v0v2.dotProduct(qvec) * invDet;

	if (t < 0) return false;; //intersection behind camera

	rec.p = r.at(t);
	rec.t = t;

	//TODO: fix normal calculation
	rec.normal = normal;
	rec.mat_ptr = mat_ptr;

	return true;
}