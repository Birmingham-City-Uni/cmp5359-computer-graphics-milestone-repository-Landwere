#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0, 255, 0, 255);

Model *model = NULL;
const int width  = 1080;
const int height = 1080;

// Write the line method here
// line (x0, y0, x1, y1, image, colour)

//struct lineS
//{
//    int x0, y0;
//    int x1, y1;
//    TGAColor colour;
//    lineS(int x, int y, int xx, int yy, TGAColor colourin)
//    {
//        x0 = x;
//        y0 = y;
//        x1 = xx;
//        y1 = yy;
//        colour = colourin;
//    }
//
//};

void line(Vec2i p0, Vec2i p1, TGAImage& image, TGAColor color) {
    bool steep = false;
    if (std::abs(p0.x - p1.x) < std::abs(p0.y - p1.y)) {
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
        steep = true;
    }
    if (p0.x > p1.x) {
        std::swap(p0, p1);
    }

    for (int x = p0.x; x <= p1.x; x++) {
        float t = (x - p0.x) / (float)(p1.x - p0.x);
        int y = p0.y * (1. - t) + p1.y * t;
        if (steep) {
            image.set(y, x, color);
        }
        else {
            image.set(x, y, color);
        }
    }
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color)
{


    if (t0.y == t1.y && t0.y == t2.y) return;

    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);

    float totalHeight = t2.y - t0.y;
    for (float step = t0.y; step < t1.y; step += 0.01f)
    {
        float segmentHeight = t1.y - t0.y + 1;
        float alpha = (step - t0.y) / totalHeight;
        float beta = (step - t0.y) / segmentHeight;
        Vec2i A = t0 + (t2 - t0) * alpha;
        Vec2i B = t0 + (t1 - t0) * beta;
       // image.set(A.x, step, red);
       // image.set(B.x, step, green);
        if (A.x > B.x) std::swap(A, B);
        for (int j = A.x; j < B.x; j++)
        {
            image.set(j, step, color);
        }
    }
    for (float step = t1.y; step < t2.y; step += 1.0f)
    {
        float segmentHeight = t2.y - t1.y + 1;
        float alpha = (step - t0.y) / totalHeight;
        float beta = (step - t1.y) / segmentHeight;
        Vec2i A = t0 + (t2 - t0) * alpha;
        Vec2i B = t1 + (t2 - t1) * beta;
        //image.set(A.x, step, red);
       // image.set(B.x, step, green);

        if (A.x > B.x) std::swap(A, B);
        for (int j = A.x; j < B.x; j++)
        {
            image.set(j, step, color);
        }
    }
       
    
}

float Barycentric(Vec3f q, Vec3f a, Vec3f b, Vec3f c)
{
    Vec3f BA = b - a;
    Vec3f CA = c - a;
    Vec3f BACACross = vectorCrossProduct(BA, CA);
    Vec3f CBQBCross = vectorCrossProduct(c - b, q - b);
    Vec3f ACQCCross = vectorCrossProduct(a - c, q - c);
    Vec3f BAQACross = vectorCrossProduct(b - a, q - a);
    Vec3f BACANormal = BACACross.normalize();

    float areaABC = vectorDotProduct(BACACross, BACANormal);
    float areaQBC = vectorDotProduct(CBQBCross, CBQBCross.normalize());
    float areaAQC = vectorDotProduct(ACQCCross, ACQCCross.normalize());
    float areaABQ = vectorDotProduct(BAQACross, BAQACross.normalize());

    float Alpha = areaQBC / areaABC;
    float Beta = areaAQC / areaABC;
    float Gamma = areaABQ / areaABC;

    float x = (Alpha * a.x); +(Beta * b.x) + (Gamma * c.x);
    float y = (Alpha * a.y); +(Beta * b.y) + (Gamma * c.y);;
    float z = (Alpha * a.z); +(Beta * b.z) + (Gamma * c.z);;

    float Q = x + y + z;
    return Q;
}

float vectorDotProduct(Vec3f a, Vec3f b)
{
    float i = (a.x * b.x);
    float j = (a.y * b.y);      
    float k = (a.z * b.z);
    return float( i + j + k);
}

Vec3f vectorCrossProduct(Vec3f a, Vec3f b )
{
        float i = ((a.y * b.z) - (a.z * b.y));
        float j = ((a.z * b.x) - (a.x - b.z));
        float k = ((a.x - b.y) - (a.y - b.x));
        return Vec3f(i, j, k);
}

//void drawlineboi(lineS inputline, TGAImage &image)
//{
//    float x;
//    bool steep = false;
//    if (abs(inputline.x0 - inputline.y0) < abs(inputline.y0 - inputline.y1))
//    {
//        float temp;
//        temp = inputline.x0;
//        inputline.x0 = inputline.y0;
//        inputline.y0 = temp;
//
//        temp = inputline.x1;
//        inputline.x1 = inputline.y1;
//        inputline.y1 = temp;
//
//        steep = true;
//    }
//    if (inputline.x0 > inputline.x1)
//    {
//        float temp;
//        temp = inputline.x0;
//        inputline.x0 = inputline.x1;
//        inputline.x1 = temp;
//
//        temp = inputline.y0;
//        inputline.y0 = inputline.y1;
//        inputline.y1 = temp;
//    }
//
//    for (x = inputline.x0; x <= inputline.x1; x++)
//    {
//        float t = (x - inputline.x0) / (inputline.x1 - inputline.x0);
//        float y = inputline.y0 * (1 - t) + (inputline.y1 * t);
//
//        if (steep)
//        {
//            image.set(y, x, inputline.colour);
//        }
//        else
//        {
//            image.set(x, y, inputline.colour);
//        }
//
//    }
//
//}


int main(int argc, char** argv) {

   
    
    //line line1 = line(0, 0, 1000, 1000);
    //line line2 = line(13, 20, 80, 40);
    //line line3 = line(20, 13, 40, 80);
    //line line4 = line(80, 40, 13, 20);

   // Model* model = nullptr;


    TGAImage image(width, height, TGAImage::RGB);
    if (2 == argc)
    {
        model = new Model(argv[1]);
    }
    else
    {
        model = new Model("cc.obj");
    }

    Vec3f light_dir(0, 0, -1);
    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        Vec2i screenCoords[3];
        Vec3f worldCoords[3];
        for (int j = 0; j < 3; j++)
        {
            Vec3f v = model->vert(face[j]);
                screenCoords[j] = Vec2i((v.x + 1.) * width / 2., (v.y + 1) * height / 2.);
                worldCoords[j] = v;

            }
        Vec3f n = (worldCoords[2] - worldCoords[0])^
                    (worldCoords[1] - worldCoords[0]);
        n.normalize();
        float intensity = n * light_dir;
        if (intensity > 0) {
            //triangle(screenCoords[0], screenCoords[1], screenCoords[2], image, TGAColor(rand() % 255, rand() % 255, rand() % 255, 255));
            triangle(screenCoords[0], screenCoords[1], screenCoords[2], image, TGAColor(intensity * 11, intensity * 232, intensity * 129, 255));
        }

    }
    TGAColor random2 = TGAColor(0, 1, 0, 255);

    //for (int i = 0; i < model->nfaces(); i++)
    //{

    //    std::vector<int> face = model->face(i);
    //    for (int j = 0; j < 3; j++)
    //    {
    //        Vec3f v0 = model->vert(face[j]);
    //        Vec3f v1 = model->vert(face[(j + 1) % 3]);
    //        float x0 = (v0.x + 1.) * width / 3.;
    //        float y0 = (v0.y + 1.) * height / 3.;
    //        float x1 = (v1.x + 1.) * width / 3.;
    //        float y1 = (v1.y + 1.) * height / 3.;
    //        //TGAColor random = TGAColor(std::rand() % ((255 + 1) - 0), std::rand() % ((255 + 1) - 0), std::rand() % ((255 + 1) - 0), 255);
    //      
    //        
    //        

    //        line ccline = line(x0, y0, x1, y1, white);

    //        drawlineboi(ccline, image);
    //    }
    //}


    /*Vec2i t0[3] = { Vec2i(650, 1000),   Vec2i(530, 1066),  Vec2i(968, 901) };
    Vec2i t1[3] = { Vec2i(90, 680),  Vec2i(42, 750),   Vec2i(895, 627) };
    Vec2i t2[3] = { Vec2i(360, 380), Vec2i(1050, 160), Vec2i(1079, 256) };

    triangle(t0[0], t0[1], t0[2], image, red);
    triangle(t1[0], t1[1], t1[2], image, white);
    triangle(t2[0], t2[1], t2[2], image, green);*/



    //float x, y;

  /*  for (x = line1.x0; x <= line1.x1; x++)
    {
        int t = (x - line1.x0) / (line1.x1 - line1.x0);
            int y = line1.y0 * (1 - t) + (line1.y1 * t);
            image.set(x, y, white);
    }*/
  
    //for (float t = 0; t < 100; t+= 0.001)
    //{
    //    

    //    x = line1.x0 + (line1.x1 - line1.x0) * t;
    //    y = line1.y0 + (line1.y1 - line1.y0) * t;
    //    TGAColor blue = TGAColor(255, 255, 255, 255);
    //    image.set(x, y, white);
    //}

    image.flip_vertically(); // we want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    delete model;


    return 0;
}


