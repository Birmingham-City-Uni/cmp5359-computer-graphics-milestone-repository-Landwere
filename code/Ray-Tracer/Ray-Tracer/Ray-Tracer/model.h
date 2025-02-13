#pragma once

#include <vector>
//#include "common.h"
#include "material.h"

class Model {



private:
	std::vector<Vec3f> verts_;              // Stores Vec3f for every model vertex world position
	std::vector<std::vector<int> > faces_;  // Stores a vector of vector<int> that represent indices in verts_ for vertices comprising a face
	std::vector<std::vector<int> > vnorms_;
	std::vector<Vec2f> vts_;				// Stores Vec3f for every model vertex texture coordinate
	std::vector<Vec3f> vns_;
	shared_ptr<material> mat;

public:

	void setMat(shared_ptr<material> material)
	{
		mat = material;
	}
	shared_ptr<material> getMat()
	{
		return mat;
	}

	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	Vec2f vt(int i);
	Vec3f vn(int i);
	std::vector<int> face(int idx);
	std::vector<int> vNorms(int idx);

};

