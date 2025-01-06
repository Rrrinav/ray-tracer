#pragma once

#include "./aabb.hpp"
#include "./interval.hpp"
#include "./ray.hpp"

class material;

class hit_record
{
public:
  point3 point;
  vec3 normal;
  double t;
  bool is_front_facing;
  shared_ptr<material> mat;

  inline void set_face_normal(const ray &r, const vec3 &outward_normal)
  {
    is_front_facing = dot(r.direction(), outward_normal) < 0;
    normal = is_front_facing ? outward_normal : -outward_normal;
  }
};

class Shape
{
public:
  virtual ~Shape() = default;

  virtual bool hit(const ray &r, Interval interval, hit_record &rec) const = 0;
  virtual aabb bounding_box() const = 0;
};
