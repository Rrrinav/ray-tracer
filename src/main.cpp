#include "./bvh_node.hpp"
#include "./camera.hpp"
#include "./material.hpp"
#include "./sphere.hpp"
#include "./texture.hpp"
#include "./utils.hpp"
#include "./vec3.hpp"
#include "./world.hpp"

void bouncing_spheres()
{
  hittable_list world;
  auto checker = make_shared<Checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
  world.add(make_shared<Sphere>(point3(0, -1000, 0), 1000, make_shared<Lambertian>(checker)));

  for (int a = -11; a < 11; a++)
  {
    for (int b = -11; b < 11; b++)
    {
      auto choose_mat = random_double();
      point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
      if ((center - point3(4, 0.2, 0)).length() > 0.9)
      {
        shared_ptr<material> Sphere_material;
        if (choose_mat < 0.8)
        {
          // diffuse
          auto albedo = color::random() * color::random();
          Sphere_material = make_shared<Lambertian>(albedo);
          auto center2 = center + vec3(0, random_double(0, .5), 0);
          world.add(make_shared<Sphere>(center, center2, 0.2, Sphere_material));
        }
        else if (choose_mat < 0.95)
        {
          // metal
          auto albedo = color::random(0.5, 1);
          auto fuzz = random_double(0, 0.5);
          Sphere_material = make_shared<metal>(albedo, fuzz);
          world.add(make_shared<Sphere>(center, 0.2, Sphere_material));
        }
        else
        {
          // glass
          Sphere_material = make_shared<dielectric>(1.5);
          world.add(make_shared<Sphere>(center, 0.2, Sphere_material));
        }
      }
    }
  }

  auto material1 = make_shared<dielectric>(1.5);
  world.add(make_shared<Sphere>(point3(0, 1, 0), 1.0, material1));
  auto material2 = make_shared<Lambertian>(color(0.4, 0.2, 0.1));
  world.add(make_shared<Sphere>(point3(-4, 1, 0), 1.0, material2));
  auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
  world.add(make_shared<Sphere>(point3(4, 1, 0), 1.0, material3));

  world = hittable_list(make_shared<bvh_node>(world));

  Camera cam;
  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 1200;
  cam.samples_per_pixel = 100;
  cam.max_depth = 150;
  cam.vfov = 20;
  cam.lookfrom = point3(13, 10, 3);
  cam.lookat = point3(0, 0, 0);
  cam.vup = vec3(0, 1, 0);
  cam.defocus_angle = 0.6;
  cam.focus_dist = 13.0;
  cam.render(world);
}

void wood()
{
  auto earth_texture = make_shared<Image_texture>("nebular.jpg");
  auto earth_surface = make_shared<Lambertian>(earth_texture);
  auto globe = make_shared<Sphere>(point3(0, 0, 0), 2, earth_surface);

  Camera cam;

  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 400;
  cam.samples_per_pixel = 100;
  cam.max_depth = 50;

  cam.vfov = 20;
  cam.lookfrom = point3(0, 0, 12);
  cam.lookat = point3(0, 0, 0);
  cam.vup = vec3(0, 1, 0);

  cam.defocus_angle = 0;

  cam.render(hittable_list(globe));
}

void perlin_spheres()
{
  hittable_list world;

  auto pertext = make_shared<Noise_texture>();
  world.add(make_shared<Sphere>(point3(0, -1000, 0), 1000, make_shared<Lambertian>(pertext)));
  world.add(make_shared<Sphere>(point3(0, 2, 0), 2, make_shared<Lambertian>(pertext)));

  Camera cam;

  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 400;
  cam.samples_per_pixel = 100;
  cam.max_depth = 50;

  cam.vfov = 20;
  cam.lookfrom = point3(13, 2, 3);
  cam.lookat = point3(0, 0, 0);
  cam.vup = vec3(0, 1, 0);

  cam.defocus_angle = 0;

  cam.render(world);
}
int main()
{
  switch (3)
  {
    case 1:
      bouncing_spheres();
      break;
    case 2:
      wood();
      break;
    case 3:
      perlin_spheres();
      break;
    default:
      wood();
      break;
  }
}
