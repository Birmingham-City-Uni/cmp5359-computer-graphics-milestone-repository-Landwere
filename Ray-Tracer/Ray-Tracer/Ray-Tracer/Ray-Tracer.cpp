// Ray-Tracer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <cmath>
#include "SDL.h"
#include <chrono>
#include "hittable_list.h"
#include "common.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#define M_PI 3.14159265359

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Surface* screen;




double hit_sphere(const Point3f& center, double radius, const Ray& r)
{
    Vec3f oc = r.origin() - center;
    auto a = r.direction().dotProduct(r.direction());
    auto b = 2.0 * oc.dotProduct(r.direction());
    auto c = oc.dotProduct(oc) - radius * radius;
    auto discriminant = b * b - 4 * a * c;
    if(discriminant < 0)
    {
        return -1.0;
    }
    else
    {
        return (-b - sqrt(discriminant)) / (2.0 * a);
    }
}

Colour ray_colour(const Ray& r, const hittable& world, int depth)
{
    hit_record rec;
    //if we exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0) return Colour(0, 0, 0);
    if (world.hit(r, 0.001, infinity, rec)) //bounded t from 0 --> inf
    {
        Ray scattered;
        Colour attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_colour(scattered, world, depth - 1);
        return Colour(0, 0, 0);
        
       // Point3f target = rec.p + rec.normal + Vec3f().random_in_unlit_sphere();
       // //next line for normal visualization debugging only
       //// return (rec.normal + Colour(1, 1, 1)) * 255 * 0.5;
       // return 0.5 * ray_colour(Ray(rec.p, target - rec.p), world, depth-1);
    }
    Vec3f unit_direction = r.direction().normalize();
    auto t = 0.5 * (unit_direction.y + 1.0);
    return (1.0 - t) * Colour(1.0, 1.0, 1.0) + t * Colour(0.5, 0.7, 1.0) * 255;
   
}

//struct Vec3f 
//{
//    double x, y, z;
//    Vec3f(double x, double y, double z) : x(x), y(y), z(z) {}
//    Vec3f operator + (const Vec3f& v) const { return Vec3f(x + v.x, y + v.y, z + v.z); }
//    Vec3f operator - (const Vec3f& v) const { return Vec3f(x - v.x, y - v.y, z - v.z); }
//    Vec3f operator * (double d) const { return Vec3f(x * d, y * d, z * d); }
//    Vec3f operator / (double d) const { return Vec3f(x / d, y / d, z / d); }
//    Vec3f normailize() const
//    {
//        double mg = sqrt(x * x + y * y + z * z);
//        return Vec3f(x / mg, y / mg, z / mg);
//    }
//};

void putPixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16*)p = pixel;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        }
        else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32*)p = pixel;
        break;
    }
}

inline double dot(const Vec3f& a, const Vec3f& b)
{
    return(a.x * b.x + a.y * b.y + a.z * b.z);
}

//struct Ray
//{
//    Vec3f o, d;
//    Ray(const Vec3f& o, const Vec3f& d) : o(o), d(d) {}
//};



struct Sphere
{
    Vec3f c;
    double r;
    Sphere(const Vec3f& c, double r) : c(c), r(r) {}
    Vec3f getNormal(const Vec3f& pi) const { return (pi - c) / r; }
    //Solve t^2*d.d+2*t*(o-p).d+(o-p).(o-p)-R^2=0
    bool intersect(const Ray& ray, double& t) const
    {
        const Vec3f o = ray.orig;
        const Vec3f d = ray.dir;
        const Vec3f oc = o - c;
        const double b = 2 * dot(oc, d);
        const double c = dot(oc, oc) - r * r;
        double disc = b * b - 4 * c; // a=1 as ray is normalised
        if (disc < 1e-4) return false; //ray misses sphere
        disc = sqrt(disc);
        const double t0 = -b - disc;
        const double t1 = -b + disc;
        t = (t0 < t1) ? t0 : t1; // two intersections on sphere, set t to shortest
        return true;
    }

    

    
};

struct Plane
{
    Vec3f a, b, c, d;
    Plane(const Vec3f& a, const Vec3f& b, const Vec3f& c, const Vec3f& d) : a(a), b(b), c(c), d(d) {}

    bool intersect(const Ray& ray, double& t) const
    {
        
        float x = ray.orig.x + t * ray.dir.x;
        float y = ray.orig.y + t * ray.dir.y;

    }
};

//method to make sure colours dont go out of the 8 bit range
void clamp255(Vec3f& col)
{
    col.x = (col.x > 255) ? 255 : (col.x < 0) ? 0 : col.x;
    col.y = (col.y > 255) ? 255 : (col.y < 0) ? 0 : col.y;
    col.z = (col.z > 255) ? 255 : (col.z < 0) ? 0 : col.z;
}

void init()
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Cory's Software Ray Tracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);

    screen = SDL_GetWindowSurface(window);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
}

int main(int argc, char **argv)
{
    init();

    //image 
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = screen->w;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int max_depth = 50;

    const int H = 500;
    const int W = 500;

    const Vec3f white(255, 255, 255);
    const Vec3f black(0, 0, 0);
    const Vec3f red(255, 0, 0);
    const Vec3f green(0, 255, 0);

    const int spp = 8;
    //image 
    const float scale = 1.f / spp;
    //camera
    camera cam;

    //world
    //const Sphere sphere(Vec3f(W * 0.5f, H * 0.5f, 50), 100);
    //const Sphere light(Vec3f(1000, 0, -50), 1);
    hittable_list world;
    auto material_ground = make_shared <lambertian>(Colour(0.8, 0.8, 0.0));
    auto material_center = make_shared <lambertian>(Colour(0.7, 0.3, 0.3));
    auto material_left = make_shared <metal>(Colour(0.8, 0.8, 0.8));
    auto material_right = make_shared <metal>(Colour(0.8, 0.6, 0.2));


    world.add(make_shared<sphere>(Point3f(0, 0, -1), 0.5, material_center));
    world.add(make_shared<sphere>(Point3f(0, -100.5, -1), 100, material_ground));
    world.add(make_shared<sphere>(Point3f(-1, 0, -1), 0.5, material_left));
    world.add(make_shared<sphere>(Point3f(1, 0, -1), 0.5, material_right));

    std::ofstream out("out.ppm");
    out << "P3\n" << W << ' ' << H << ' ' << "255\n";

    double t;
    Vec3f pix_col(black);

    SDL_Event e;
    bool running = true;
    while (running) {

        auto t_start = std::chrono::high_resolution_clock::now();

        // clear back buffer, pixel data on surface and depth buffer (as movement)
        SDL_FillRect(screen, nullptr, SDL_MapRGB(screen->format, 0, 0, 0));
        SDL_RenderClear(renderer);

    for (int y = screen->h-1; y >= 0; --y )// start at the top left
    {
        std::cerr << "\rScanlines remaining: " << y << std::flush;
        for (int x = 0; x < screen->w; ++x)
        {
            pix_col = black;
            for (int s = 0; s < spp; s++)
            {
                auto u = double(x + random_double()) / (image_width - 1);
                auto v = double(y + random_double()) / (image_height - 1);
                Ray ray = cam.get_ray(u, v);
                pix_col = pix_col + ray_colour(ray, world, max_depth);
            }
           //scale cumalativew colour values accordingly to spp and gamme-correct for gamma = 2.0
            pix_col /= 255.f * spp;
            pix_col.x = sqrt(pix_col.x);
            pix_col.y = sqrt(pix_col.y);
            pix_col.z = sqrt(pix_col.z);
            pix_col *= 255;
            Uint32 colour = SDL_MapRGB(screen->format, pix_col.x, pix_col.y, pix_col.z);
            putPixel(screen, x, y, colour);

           

        }
    }

    auto t_end = std::chrono::high_resolution_clock::now();
    auto passedTime = std::chrono::duration<double, std::milli>(t_end - t_start).count();
    std::cerr << "Frame render time:  " << passedTime << " ms" << std::endl;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, screen);
    if (texture == NULL) {
        fprintf(stderr, "CreateTextureFromSurface failed: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_FreeSurface(screen);

    SDL_RenderCopyEx(renderer, texture, NULL, NULL, 0, 0, SDL_FLIP_VERTICAL);
    SDL_RenderPresent(renderer);

    SDL_DestroyTexture(texture);

    if (SDL_PollEvent(&e))
    {
        switch (e.type) {
        case SDL_KEYDOWN:
            switch (e.key.keysym.sym) {
            case SDLK_ESCAPE:
                running = false;
                break;
            }
            break;
        }
    }
    }

    std::cout << "Hello World!\n";
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
