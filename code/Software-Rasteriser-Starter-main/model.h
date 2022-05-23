#pragma once

#include <vector>
#include "geometry.h"
#include "SDL.h" 
#include "tgaimage.h"
#include "stb_image.h"
#include "Texture.h"

class Model {


	
public:
	void setColour(TGAColor col)
	{
		colour = col;
	}
	TGAColor getColour()
	{
		return colour;
	}
	int getColourR()
	{
		return colour.r;
	}
	int getColourG()
	{
		return colour.g;
	}
	int getColourB()
	{
		return colour.b;
	}
	/*unsigned char* getTexture()
	{
		return texture;
	}*/
	Texture* getTexture()
	{
		return texture;
	}
	void setTexture(const char* filename)
	{

		//unsigned char* image = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);
		texture = new Texture(filename);
		//texture = image;
	}
	int getTexWidth()
	{
		return width;
	}
	int getTexHeight()
	{
		return height;
	}
	int getTexChannels()
	{
		return channels;
	}
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	Vec2f vt(int i);
	Vec3f vn(int i);
	std::vector<int> face(int idx);
	std::vector<int> vns(int idx);
	std::vector<int> uvs(int idx);

	Texture* texture = NULL;

private:
	std::vector<Vec3f> verts_;              // Stores Vec3f for every model vertex world position
	std::vector<std::vector<int> > faces_;  // Stores a vector of vector<int> that represent indices in verts_ for vertices comprising a face
	std::vector<Vec2f> vts_;				// Stores Vec3f for every model vertex texture coordinate
	std::vector<Vec3f> vns_;
	std::vector<std::vector<int>> vnorm_;
	std::vector<std::vector<int>> uvs_;

	TGAColor colour;
	//unsigned char* texture;
	int width, height, channels;
};

