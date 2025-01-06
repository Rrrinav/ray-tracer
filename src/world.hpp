#pragma once

#include <memory>
#include <vector>

#include "./aabb.hpp"
#include "./interval.hpp"
#include "./shape.hpp"

using std::make_shared;
using std::shared_ptr;

class hittable_list : public Shape
{
  aabb bbox;

public:
  std::vector<shared_ptr<Shape>> objects;

  hittable_list() {}
  hittable_list(shared_ptr<Shape> object) { add(object); }

  ~hittable_list() {}

  void clear() { objects.clear(); }

  void add(shared_ptr<Shape> object)
  {
    objects.push_back(object);
    bbox = aabb(bbox, object->bounding_box());
  }

  bool hit(const ray &r, Interval interval, hit_record &rec) const override
  {
    hit_record temp_rec;
    bool hit_anything = false;
    auto closest_so_far = interval.max;

    for (const auto &object : objects)
    {
      if (object->hit(r, Interval(interval.min, closest_so_far), temp_rec))
      {
        hit_anything = true;
        closest_so_far = temp_rec.t;
        rec = temp_rec;
      }
    }

    return hit_anything;
  }

  aabb bounding_box() const override { return bbox; }
};
