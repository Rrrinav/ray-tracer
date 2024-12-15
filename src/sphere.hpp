#pragma once

#include "./shape.hpp"
#include "interval.hpp"

class Sphere : public Shape
{
  point3 center;
  double radius;
  shared_ptr<material> mat;

public:
  Sphere(const point3 &center, double radius, shared_ptr<material> mat) : center(center), radius(std::fmax(0, radius)), mat(mat) {}

  bool hit(const ray &r, Interval interval, hit_record &record) const override
  {
    vec3 oc = center - r.origin();
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
    vec3 outward_normal = (record.point - center) / radius;
    record.set_face_normal(r, outward_normal);
    record.mat = mat;

    return true;
  }
};
