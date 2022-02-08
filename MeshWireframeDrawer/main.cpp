#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
Model *model = NULL;
const int width  = 800;
const int height = 800;

// Write the line method here
// line (x0, y0, x1, y1, image, colour)

struct line
{
    int x0, y0;
    int x1, y1;
    line(int x, int y, int xx, int yy)
    {
        x0 = x;
        y0 = y;
        x1 = xx;
        y1 = yy;
    }

};


void drawlineboi(line inputline, TGAImage &image)
{
    float x;
    bool steep = false;
    if (abs(inputline.x0 - inputline.y0) < abs(inputline.y0 - inputline.y1))
    {
        float temp;
        temp = inputline.x0;
        inputline.x0 = inputline.y0;
        inputline.y0 = temp;

        temp = inputline.x1;
        inputline.x1 = inputline.y1;
        inputline.y1 = temp;

        steep = true;
    }
    if (inputline.x0 > inputline.x1)
    {
        float temp;
        temp = inputline.x0;
        inputline.x0 = inputline.x1;
        inputline.x1 = temp;

        temp = inputline.y0;
        inputline.y0 = inputline.y1;
        inputline.y1 = temp;
    }

    for (x = inputline.x0; x <= inputline.x1; x++)
    {
        float t = (x - inputline.x0) / (inputline.x1 - inputline.x0);
        float y = inputline.y0 * (1 - t) + (inputline.y1 * t);

        if (steep)
        {
            image.set(y, x, white);
        }
        else
        {
            image.set(x, y, white);
        }

    }

}

int main(int argc, char** argv) {
    line line1 = line(0, 0, 1000, 1000);
    line line2 = line(13, 20, 80, 40);
    line line3 = line(20, 13, 40, 80);
    line line4 = line(80, 40, 13, 20);

    TGAImage image(width, height, TGAImage::RGB);
    if (2 == argc)
    {
        model = new Model(argv[1]);
    }
    else
    {
        model = new Model("cc.obj");
    }
    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        for (int j = 0; j < 3; j++)
        {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j + 1) % 3]);
            int x0 = (v0.x + 1.) * width / 2.;
            int y0 = (v0.y + 1.) * height / 2.;
            int x1 = (v1.x + 1.) * width / 2;
            int y1 = (v1.y + 1.) * height / 2;
            line ccline = line(x0, y0, x1, y1);
            drawlineboi(ccline, image);
        }
    }

    float x, y;
    drawlineboi(line1, image);
    drawlineboi(line2, image);
    drawlineboi(line3, image);
    drawlineboi(line4, image);
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

