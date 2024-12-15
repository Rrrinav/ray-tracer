#pragma once

#include <memory>
#include <vector>

#include "./shape.hpp"
#include "interval.hpp"

using std::make_shared;
using std::shared_ptr;

class hittable_list : public Shape
{
public:
  std::vector<shared_ptr<Shape>> objects;

  hittable_list() {}
  hittable_list(shared_ptr<Shape> object) { add(object); }

  void clear() { objects.clear(); }

  void add(shared_ptr<Shape> object) { objects.push_back(object); }

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
};
