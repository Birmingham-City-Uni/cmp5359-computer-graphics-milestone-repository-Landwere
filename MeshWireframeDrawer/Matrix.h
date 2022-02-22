#pragma once
#include <vector>
#include <cmath>
#include <iostream>
#include "geometry.h"


using namespace std;

	class Matrix
	{

		vector<vector<float>> m;
		int row, cols;

	public:
		//matrix of 0s with specified (or defualt) row and column
		Matrix(int _rows = 4, int _cols = 4);
		Matrix(Vec3i v); // store a vector in a matrix object with w
		int num_rows(); // getter for rown
		int num_cols(); // getter for cols
		static Matrix identity(int dimensions); // return the identiy
		vector<float>& operator[](const int i); // return the row of a matrix
		vector<float>& operator*(const Matrix& a); //return a martix multiplication result
		Matrix transpose(); //return the transpose of the matrix 
		Matrix inverse(); // calculate the inverse of the matrix 
		friend ostream& operator<<(ostream& s, Matrix& m); //print matrix 
	};



