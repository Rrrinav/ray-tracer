#pragma once

#include "./shape.hpp"
#include "interval.hpp"
#include "ray.hpp"

class Sphere : public Shape
{
  ray center;
  double radius;
  shared_ptr<material> mat;

public:
  // Stationary Sphere
  Sphere(const point3 &static_center, double radius, shared_ptr<material> mat)
      : center(static_center, vec3(0, 0, 0)), radius(std::fmax(0, radius)), mat(mat)
  {
  }

  // Moving Sphere
  Sphere(const point3 &center1, const point3 &center2, double radius, shared_ptr<material> mat)
      : center(center1, center2 - center1), radius(std::fmax(0, radius)), mat(mat)
  {
  }

  bool hit(const ray &r, Interval interval, hit_record &record) const override
  {
    point3 current_center = center.at(r.time());
    vec3 oc = current_center - r.origin();
    auto a = r.direction().length_squared();
    auto h = dot(r.direction(), oc);
    auto c = oc.length_squared() - radius * radius;

    auto discriminant = h * h - a * c;
    if (discriminant < 0)
      return false;

    auto sqrtd = std::sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (h - sqrtd) / a;
    if (!interval.surrounds(root))
    {
      root = (h + sqrtd) / a;
      if (!interval.surrounds(root))
        return false;
    }

    record.t = root;
    record.point = r.at(record.t);
    vec3 outward_normal = (record.point - current_center) / radius;
    record.set_face_normal(r, outward_normal);
    record.mat = mat;

    return true;
  }
};
