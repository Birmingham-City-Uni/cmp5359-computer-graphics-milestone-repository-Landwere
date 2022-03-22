// Ray-Tracer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <cmath>



struct Vec3 
{
    double x, y, z;
    Vec3(double x, double y, double z) : x(x), y(y), z(z) {}
    Vec3 operator + (const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator - (const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator * (double d) const { return Vec3(x * d, y * d, z * d); }
    Vec3 operator / (double d) const { return Vec3(x / d, y / d, z / d); }
    Vec3 normailize() const
    {
        double mg = sqrt(x * x + y * y + z * z);
        return Vec3(x / mg, y / mg, z / mg);
    }
};

inline double dot(const Vec3& a, const Vec3& b)
{
    return(a.x * b.x + a.y * b.y + a.z * b.z);
}

struct Ray
{
    Vec3 o, d;
    Ray(const Vec3& o, const Vec3& d) : o(o), d(d) {}
};

struct Sphere
{
    Vec3 c;
    double r;
    Sphere(const Vec3& c, double r) : c(c), r(r) {}
    Vec3 getNormal(const Vec3& pi) const { return (pi - c) / r; }
    //Solve t^2*d.d+2*t*(o-p).d+(o-p).(o-p)-R^2=0
    bool intersect(const Ray& ray, double& t) const
    {
        const Vec3 o = ray.o;
        const Vec3 d = ray.d;
        const Vec3 oc = o - c;
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

//method to make sure colours dont go out of the 8 bit range
void clamp255(Vec3& col)
{
    col.x = (col.x > 255) ? 255 : (col.x < 0) ? 0 : col.x;
    col.y = (col.y > 255) ? 255 : (col.y < 0) ? 0 : col.y;
    col.z = (col.z > 255) ? 255 : (col.z < 0) ? 0 : col.z;
}

int main()
{

    const int H = 500;
    const int W = 500;

    const Vec3 white(255, 255, 255);
    const Vec3 black(0, 0, 0);
    const Vec3 red(255, 0, 0);

    const Sphere sphere(Vec3(W * 0.5f, H * 0.5f, 50), 50);
    const Sphere light(Vec3(0, 0, 50), 1);

    std::ofstream out("out.ppm");
    out << "P3\n" << W << ' ' << H << ' ' << "255\n";

    double t;
    Vec3 pix_col(black);

    for (int y = 0; y < H; ++y)
    {
        for (int x = 0; x < W; ++x)
        {
            pix_col = black;

            const Ray ray(Vec3(x, y, 0), Vec3(0, 0, 1));
            if (sphere.intersect(ray, t))
            {
                const Vec3 pi = ray.o + ray.d * t;
                const Vec3 L = light.c - pi;
                const Vec3 N = sphere.getNormal(pi);
                const double dt = dot(L.normailize(), N.normailize());

                pix_col = (red + white * dt) * 0.5;
                clamp255(pix_col);
            }
            out << (int)pix_col.x << ' ' << (int)pix_col.y << ' ' << (int)pix_col.z << '\n';
        }
    }

    std::cout << "Hello World!\n";
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
