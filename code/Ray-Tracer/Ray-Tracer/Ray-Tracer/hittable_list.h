#pragma once
#include "hittable.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class hittable_list : public hittable
{
public:
	hittable_list() {}
	hittable_list(shared_ptr<hittable> object) { add(object); }

	void clear() { objects.clear(); }
	void add(shared_ptr<hittable> object) { objects.push_back(object); }
	virtual bool hit(const Ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool bounding_box(aabb& output_box) const override;
public:
	std::vector<shared_ptr<hittable>> objects;
};

bool hittable_list::hit(const Ray& r, double t_min, double t_max, hit_record& rec) const
{
	hit_record temp_rec;
	bool hit_anything = false;
	auto closest_so_far = t_max;

	for (const auto& object : objects)
	{
		if (object->hit(r, t_min, closest_so_far, temp_rec))
		{
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
	}
	return hit_anything;
}

inline bool hittable_list::bounding_box(aabb& output_box) const
{
	if (objects.empty()) return false;

	aabb temp_box;
	bool first_box = true;

	for (const auto& object : objects)
	{
		if (!object->bounding_box(temp_box)) return false;
		output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
	}
	return true;
}

