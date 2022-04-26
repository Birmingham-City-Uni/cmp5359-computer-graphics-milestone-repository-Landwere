// Ray-Tracer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <cmath>
#include <chrono>
#include <filesystem>
#include <vector>
#include <string>
#include "SDL.h"
#include "hittable_list.h"
#include "common.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "threadpool.h"
#include "model.h"
#include "triangle.h"

#define M_PI 3.14159265359

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Surface* screen;

namespace fs = std::filesystem;


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

    SDL_Window* window = SDL_CreateWindow("Cory's Software Ray Tracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 360, 0);

    screen = SDL_GetWindowSurface(window);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
}
hittable_list random_scene() {
    hittable_list world;
    auto ground_material = make_shared<lambertian>(Colour(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(Point3f(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            auto choose_mat = random_double();
            Point3f centre(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
            if ((centre - Point3f(4, 0.2, 0)).length() > 0.9)
            {
                shared_ptr<material> sphere_material;
                if (choose_mat < 0.8)
                {
                    //diffuse
                    auto albedo = Colour::random() * Colour::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(centre, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95)
                {
                    //metal
                    auto albedo = Colour::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(centre, 0.2, sphere_material));
                }
                else
                {
                    //glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(centre, 0.2, sphere_material));
                }
            }
        }
    }
    auto mat1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(Point3f(0, 1, 0), 1.0, mat1));
    auto mat2 = make_shared<lambertian>(Colour(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(Point3f(-4, 1, 0), 1.0, mat2));
    auto mat3 = make_shared<metal>(Colour(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(Point3f(4, 1, 0), 1.0, mat3));
    return world;
}

hittable_list test_scene()
{
    hittable_list world;

    Model* model = new Model("__Box001__Box001Shape.obj");
    if (model == NULL)
    {
        std::cout << "Model failed to load!" << std::endl;
    }
    Vec3f transform(0, 0.8, 0);
    auto glass = make_shared<dielectric>(1.5);

   /* for (uint32_t i = 0; i < model->nfaces(); ++i)
    {
        std::cout << "face!" << std::endl;
        const Vec3f& v0 = model->vert(model->face(i)[0]);
        const Vec3f& v1 = model->vert(model->face(i)[1]);
        const Vec3f& v2 = model->vert(model->face(i)[2]);
        world.add(make_shared<triangle>(v0 + transform, v1 + transform, v2 + transform, glass));
    }*/

    transform = Vec3f(0, 0, 0);
    auto mat_diffuse = make_shared<lambertian>(Colour(1, 0, 0));
    for (uint32_t i = 0; i < model->nfaces(); ++i)
    {
        const Vec3f& v0 = model->vert(model->face(i)[0]);
        const Vec3f& v1 = model->vert(model->face(i)[1]);
        const Vec3f& v2 = model->vert(model->face(i)[2]);
        world.add(make_shared<triangle>(v0 + transform, v1 + transform, v2 + transform ,mat_diffuse));
    }
    transform = Vec3f(-1.2, 0.8, 0);
    auto mat_metal = make_shared<metal>(Colour(0.7, 0.6, 0.5), 0.0);
   /* for (uint32_t i = 0; i < model->nfaces(); ++i)
    {
        const Vec3f& v0 = model->vert(model->face(i)[0]);
        const Vec3f& v1 = model->vert(model->face(i)[1]);
        const Vec3f& v2 = model->vert(model->face(i)[2]);
        world.add(make_shared<triangle>(v0 + transform, v1 + transform, v2 + transform, mat_metal));
    }*/

    auto ground_material = make_shared<lambertian>(Colour(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(Point3f(0, -1000, 0), 1000, ground_material));
    return world;
}

const int modelArraySize = 34;
Model* modelArray[modelArraySize];
hittable_list maya_scene()
{
    //materials
    auto purple_mat_diffuse = make_shared<lambertian>(Colour(0.140f, 0.122f, 0.230f));
    auto brown_mat_diffuse = make_shared<lambertian>(Colour(0.150f, 0.75f, 0));
    auto glass = make_shared<dielectric>(1.5);
    auto mat_metal = make_shared<metal>(Colour(0.7, 0.6, 0.5), 0.0);

    hittable_list world;

    std::string path = "\Models";

    int iteration = 0;
    for (const auto & entry : fs::directory_iterator(path))
    {
        if (iteration == 31)
        {
            std::cout << entry.path() << std::endl;
            std::string s = entry.path().u8string();

            modelArray[iteration] = new Model(s.c_str());
            //Model* model = new Model(s.c_str());
            if (modelArray[iteration] == NULL)
            {
                std::cout << "Model failed to load!" << std::endl;
            }
            modelArray[iteration]->setMat(glass);
        }
        if (iteration == 0)
        {
            std::cout << entry.path() << std::endl;
            std::string s = entry.path().u8string();

            modelArray[iteration] = new Model(s.c_str());
            //Model* model = new Model(s.c_str());
            if (modelArray[iteration] == NULL)
            {
                std::cout << "Model failed to load!" << std::endl;
            }
            modelArray[iteration]->setMat(purple_mat_diffuse);
        }
        if (iteration == 26)
        {
            std::cout << entry.path() << std::endl;
            std::string s = entry.path().u8string();

            modelArray[iteration] = new Model(s.c_str());
            //Model* model = new Model(s.c_str());
            if (modelArray[iteration] == NULL)
            {
                std::cout << "Model failed to load!" << std::endl;
            }
            modelArray[iteration]->setMat(mat_metal);
        }
        iteration++;
    }


    //Change textures
   /* modelArray[33]->setMat(brown_mat_diffuse);
    modelArray[26]->setMat(mat_metal);
    modelArray[29]->setMat(glass);
    modelArray[30]->setMat(glass);
    modelArray[31]->setMat(glass);*/

    //add to world
    Vec3f transform = Vec3f(0, 0, 0);
    iteration = 0;
    for (int i = 0; i < modelArraySize; ++i)
    {
        if (modelArray[i] != NULL)
        {
            for (uint32_t i = 0; i < modelArray[iteration]->nfaces(); ++i)
            {
                const Vec3f& v0 = modelArray[iteration]->vert(modelArray[iteration]->face(i)[0]);
                const Vec3f& v1 = modelArray[iteration]->vert(modelArray[iteration]->face(i)[1]);
                const Vec3f& v2 = modelArray[iteration]->vert(modelArray[iteration]->face(i)[2]);

                const Vec3f& vn0 = modelArray[iteration]->vn(modelArray[iteration]->vNorms(i)[0]);
                const Vec3f& vn1 = modelArray[iteration]->vn(modelArray[iteration]->vNorms(i)[1]);
                const Vec3f& vn2 = modelArray[iteration]->vn(modelArray[iteration]->vNorms(i)[2]);

                world.add(make_shared<triangle>(v0 + transform, v1 + transform, v2 + transform,vn0, vn1, vn2 ,modelArray[iteration]->getMat()));
            }
        }
        else std::cout << "Empty model in array" << std::endl;
            iteration++;
    }

    auto ground_material = make_shared<lambertian>(Colour(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(Point3f(0, -1000, 0), 1000, ground_material));
    return world;
}

void LineRender(SDL_Surface* screen, hittable_list world, int y, int spp, int max_depth, camera* cam)
{
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = screen->w;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const Colour black(0, 0, 0);
    Vec3f pix_col(black);
    for (int x = 0; x < screen->w; ++x)
    {
        pix_col = black;
        for (int s = 0; s < spp; s++)
        {
            auto u = double(x + random_double()) / (image_width - 1);
            auto v = double(y + random_double()) / (image_height - 1);
            Ray ray = cam->get_ray(u, v);
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



int main(int argc, char **argv)
{
    if (__cplusplus == 201703L)
        std::cout << "C++17" << std::endl;
    else if (__cplusplus == 201402L)
        std::cout << "C++14" << std::endl;
    else if (__cplusplus == 201103L)
        std::cout << "C++11" << std::endl;
    else if (__cplusplus == 199711L)
        std::cout << "C++98" << std::endl;
    else
        std::cout << "pre-standard C++" << std::endl;

    init();

    //image 
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = screen->w;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int max_depth = 50;

    const int H = 640;
    const int W = 480;

    const Vec3f white(255, 255, 255);
    const Vec3f black(0, 0, 0);
    const Vec3f red(255, 0, 0);
    const Vec3f green(0, 255, 0);

    const int spp = 5;
    //image 
    const float scale = 1.f / spp;
    auto R = cos(pi / 4);
    //camera
    Point3f lookfrom(-17.678873, 84.315444, 74.007110);
    Point3f lookat(-2.243758, 84.315444, 39.339274);
   // Point3f lookfrom(2, 13, 0);
   // Point3f lookat(0, 0, 0);
    Vec3f vup(0, 1, 0);
    auto dist_to_focus = 100;
    auto apeture = 0.05;
    camera cam(lookfrom, lookat, vup,25 /*54.43*/, aspect_ratio, apeture, dist_to_focus);

    //world
    auto world = maya_scene();
    //const Sphere sphere(Vec3f(W * 0.5f, H * 0.5f, 50), 100);
    //const Sphere light(Vec3f(1000, 0, -50), 1);
   /* hittable_list world;
    auto material_ground = make_shared <lambertian>(Colour(0.8, 0.8, 0.0));
    auto material_center = make_shared <lambertian>(Colour(0.1, 0.2, 0.5));
    auto material_left = make_shared <dielectric>(1.5);
    auto material_right = make_shared <metal>(Colour(0.8, 0.6, 0.2), 0.0);
    auto material_blue = make_shared <lambertian>(Colour(0, 0, 1));
    auto material_red = make_shared <lambertian>(Colour(1, 0, 0));

    world.add(make_shared<sphere>(Point3f(0, 0, -1), 0.5, material_center));
    world.add(make_shared<sphere>(Point3f(0, -100.5, -1), 100, material_ground));
    world.add(make_shared<sphere>(Point3f(-1, 0, -1), 0.5, material_left));
    world.add(make_shared<sphere>(Point3f(-1.0, 0.0, -1.0), -0.4, material_left));
    world.add(make_shared<sphere>(Point3f(1, 0, -1), 0.5, material_right));*/
   /* world.add(make_shared<sphere>(Point3f(-R, 0, -1), R, material_blue));
    world.add(make_shared<sphere>(Point3f(R, 0, -1), R, material_red));*/



    double t;


    SDL_Event e;
    bool running = true;
    while (running) {

        auto t_start = std::chrono::high_resolution_clock::now();

        // clear back buffer, pixel data on surface and depth buffer (as movement)
        SDL_FillRect(screen, nullptr, SDL_MapRGB(screen->format, 0, 0, 0));
        SDL_RenderClear(renderer);


        {
            t_start = std::chrono::high_resolution_clock::now();
            ThreadPool pool(std::thread::hardware_concurrency());

            int start = screen->h - 1;
            int step = screen->h / std::thread::hardware_concurrency();
            for (int y = 0; y < screen->h - 1; y++)
            {
                pool.Enqueue(std::bind(LineRender, screen, world, y, spp, max_depth, &cam));
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


    std::ofstream out("out.ppm");
    out << "P3\n" << W << ' ' << H << ' ' << "255\n";

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
