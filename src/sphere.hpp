#pragma once

#include "./shape.hpp"
#include "aabb.hpp"
#include "interval.hpp"
#include "ray.hpp"

class Sphere : public Shape
{
  ray center;
  double radius;
  shared_ptr<material> mat;
  aabb bbox;

public:
  // Stationary Sphere
  Sphere(const point3 &static_center, double radius, shared_ptr<material> mat)
      : center(static_center, vec3(0, 0, 0)), radius(std::fmax(0, radius)), mat(mat)
  {
    auto rvec = vec3(radius, radius, radius);
    bbox = aabb(static_center - rvec, static_center + rvec);
  }

  // Moving Sphere
  Sphere(const point3 &center1, const point3 &center2, double radius, shared_ptr<material> mat)
      : center(center1, center2 - center1), radius(std::fmax(0, radius)), mat(mat)
  {
    auto rvec = vec3(radius, radius, radius);
    aabb box1(center.at(0) - rvec, center.at(0) + rvec);
    aabb box2(center.at(1) - rvec, center.at(1) + rvec);
    bbox = aabb(box1, box2);
  }
  aabb bounding_box() const override { return bbox; }

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
    get_sphere_uv(outward_normal, record.u, record.v);
    record.mat = mat;

    return true;
  }
  // @param p: a given point on the sphere of radius one, centered at the origin.
  // @param u: returned value [0,1] of angle around the Y axis from X=-1.
  // @param v: returned value [0,1] of angle from Y=-1 to Y=+1.
  //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
  //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
  //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>
  static void get_sphere_uv(const point3 &p, double &u, double &v)
  {
    auto theta = std::acos(-p.y());
    auto phi = std::atan2(-p.z(), p.x()) + pi;

    u = phi / (2 * pi);
    v = theta / pi;
  }
};
