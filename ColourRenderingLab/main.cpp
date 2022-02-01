#include <iostream>
#include <fstream>
#include "tgaimage.h"

using namespace std;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

int main(int argc, char** argv) {

	const int low_image_width = 256;
	const int low_image_height = 256;
	// Test writing TGA image
	TGAImage image(low_image_width, low_image_height, TGAImage::RGB);
	for (int j = low_image_height - 1; j >= 0; --j) {
		for (int i = 0; i < low_image_width; ++i) {
			auto r = double(i) / (low_image_width - 1);
			auto g = double(j) / (low_image_height - 1);
			auto b = 0.25;
			int ir = static_cast<int>(255.999 * r);
			int ig = static_cast<int>(255.999 * g);
			int ib = static_cast<int>(255.999 * b);
			const TGAColor blue = TGAColor(ir, ig, ib, 255);
			image.set(i, j, blue);
		}
	}

	//image.set(52, 41, red);
	image.flip_vertically(); // I want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");

	// Test writing PPM image
	const int image_width = 256;
    const int image_height = 256;
	ofstream outPPM;
	outPPM.open ("output.ppm");	
	outPPM << "P3\n" << image_width << ' ' << image_height << "\n255\n";
	for (int j = image_height-1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            auto r = double(i) / (image_width-1);
            auto g = double(j) / (image_height-1);
            auto b = 0.25;
            int ir = static_cast<int>(255.999 * r);
            int ig = static_cast<int>(255.999 * g);
            int ib = static_cast<int>(255.999 * b);
            outPPM << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }
	outPPM.close();
	return 0;
}

