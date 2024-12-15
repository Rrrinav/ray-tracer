#pragma once

#include <cstddef>

#include "./color.hpp"
#include "./material.hpp"
#include "./ray.hpp"
#include "./shape.hpp"
#include "./utils.hpp"

const double min_interval = 0.00001;

class Camera
{
  int image_height;            // Rendered image height
  point3 center;               // Camera center
  point3 pixel00_loc;          // Location of pixel 0, 0
  vec3 pixel_delta_u;          // Offset to pixel to the right
  vec3 pixel_delta_v;          // Offset to pixel below
  double pixel_samples_scale;  // Color scale factor for a sum of pixel samples
  vec3 u, v, w;                // Camera frame basis vectors
  vec3 defocus_disk_u;         // Defocus disk horizontal radius
  vec3 defocus_disk_v;         // Defocus disk vertical radius

public:
  double aspect_ratio = 1.0;  // Ratio of image width over height
  int image_width = 100;      // Rendered image width in pixel count
  size_t samples_per_pixel = 10;
  size_t max_depth = 10;
  double vfov = 90.0;                 // Vertical field-of-view in
  point3 lookfrom = point3(0, 0, 0);  // Point camera is looking from
  point3 lookat = point3(0, 0, -1);   // Point camera is looking at
  vec3 vup = vec3(0, 1, 0);           // Camera-relative "up" direction
  double defocus_angle = 0;           // Variation angle of rays through each pixel
  double focus_dist = 10;             // Distance from camera lookfrom point to plane of perfect focus

  void render(const Shape &world)
  {
    initialize();

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++)
    {
      std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
      for (int i = 0; i < image_width; i++)
      {
        auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        auto ray_direction = pixel_center - center;
        ray r(center, ray_direction);

        color pixel_color(0, 0, 0);
        for (int sample = 0; sample < (int)samples_per_pixel; sample++)
        {
          ray r = get_aliasing_ray(i, j);
          pixel_color += ray_color(r, max_depth, world);
        }
        write_color(std::cout, pixel_samples_scale * pixel_color);
      }
    }

    std::clog << "\rDone.                 \n";
  }

private:
  color ray_color(const ray &r, size_t depth, const Shape &world) const
  {
    if (depth <= 0)
      return color(0, 0, 0);
    hit_record rec;

    if (world.hit(r, Interval(min_interval, infinity), rec))
    {
      ray scattered;
      color attenuation;
      if (rec.mat->scatter(r, rec, attenuation, scattered))
        return attenuation * ray_color(scattered, depth - 1, world);
      return color(0, 0, 0);
    }

    // Sky
    color base_white = color(1.0, 1.0, 1.0);
    color top_blue = color(0.5, 0.7, 1.0);
    vec3 unit_direction = unit_vector(r.direction());
    auto interpolator = 0.5 * (unit_direction.y() + 1.0);  // from -1 - 1 to 0 - 1
    return (1.0 - interpolator) * base_white + interpolator * top_blue;
  }

  point3 defocus_disk_sample() const
  {
    // Returns a random point in the camera defocus disk.
    auto p = random_in_unit_disk();
    return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
  }

  void initialize()
  {
    pixel_samples_scale = 1.0 / samples_per_pixel;
    image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    center = lookfrom;

    // Determine viewport dimensions.
    auto theta = degrees_to_radians(vfov);
    auto h = std::tan(theta / 2);
    auto viewport_height = 2 * h * focus_dist;
    auto viewport_width = viewport_height * (double(image_width) / image_height);

    // Unit vectors based on camera position
    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    vec3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
    vec3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel.
    auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Defocus disk vectors
    auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
    defocus_disk_u = u * defocus_radius;
    defocus_disk_v = v * defocus_radius;
  }
  ray get_aliasing_ray(int i, int j) const
  {
    // Construct a camera ray originating from the origin and directed at randomly sampled
    // point around the pixel location i, j.

    auto offset = sample_square();
    auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);

    auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
    auto ray_direction = pixel_sample - ray_origin;

    return ray(ray_origin, ray_direction);
  }

  vec3 sample_square() const
  {
    // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
    return vec3(random_double() - 0.5, random_double() - 0.5, 0);
  }
};
