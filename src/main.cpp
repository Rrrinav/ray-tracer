#include "./bvh_node.hpp"
#include "./camera.hpp"
#include "./material.hpp"
#include "./sphere.hpp"
#include "./utils.hpp"
#include "./vec3.hpp"
#include "./world.hpp"

int main()
{
  hittable_list world;

  // Ground - made larger for better scene scale
  auto ground_material = make_shared<Lambertian>(color(0.2, 0.2, 0.3));
  world.add(make_shared<Sphere>(point3(0, -1000, 0), 1000, ground_material));

  // Create a spiral of spheres
  int circles = 5;
  double height_step = 0.4;
  double radius_step = 0.8;

  for (int circle = 0; circle < circles; circle++)
  {
    double base_radius = 6.0 - circle * radius_step;
    double y = circle * height_step;
    int spheres_in_circle = 12 - circle * 2;

    for (int i = 0; i < spheres_in_circle; i++)
    {
      double angle = 2 * 3.14159 * i / spheres_in_circle;
      point3 center(base_radius * cos(angle), y + 0.2, base_radius * sin(angle));

      auto choose_mat = random_double();
      if (choose_mat < 0.6)
      {
        // Colored diffuse spheres with warmer colors
        auto albedo = color(random_double(0.6, 1.0),  // More red
                            random_double(0.3, 0.7),  // Medium green
                            random_double(0.2, 0.4)   // Less blue
        );
        auto sphere_material = make_shared<Lambertian>(albedo);
        auto center2 = center + vec3(0, random_double(0, .5), 0);
        world.add(make_shared<Sphere>(center, center2, 0.2, sphere_material));
      }
      else if (choose_mat < 0.85)
      {
        // Metallic spheres with low fuzz
        auto albedo = color::random(0.7, 0.95);
        auto fuzz = random_double(0, 0.3);
        auto sphere_material = make_shared<metal>(albedo, fuzz);
        world.add(make_shared<Sphere>(center, 0.2, sphere_material));
      }
      else
      {
        // Glass spheres
        auto sphere_material = make_shared<dielectric>(1.5);
        world.add(make_shared<Sphere>(center, 0.2, sphere_material));
      }
    }
  }

  // Three large distinctive spheres in the center
  auto material1 = make_shared<dielectric>(1.5);
  world.add(make_shared<Sphere>(point3(0, 1, 0), 1.0, material1));

  auto material2 = make_shared<metal>(color(0.9, 0.5, 0.2), 0.1);  // Golden metal
  world.add(make_shared<Sphere>(point3(-2.5, 1, 0), 1.0, material2));

  auto material3 = make_shared<Lambertian>(color(0.8, 0.2, 0.3));  // Reddish
  world.add(make_shared<Sphere>(point3(2.5, 1, 0), 1.0, material3));

  /*world = hittable_list(make_shared<bvh_node>(world));*/
  // Camera settings
  Camera cam;
  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 800;        // Higher resolution
  cam.samples_per_pixel = 200;  // More samples
  cam.max_depth = 50;
  cam.vfov = 25;                    // Slightly wider angle
  cam.lookfrom = point3(12, 5, 9);  // Better angle
  cam.lookat = point3(0, 0, 0);
  cam.vup = vec3(0, 1, 0);
  cam.defocus_angle = 0.4;
  cam.focus_dist = 10.0;

  cam.render(world);
}
