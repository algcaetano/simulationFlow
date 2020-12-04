#pragma once

#include "RgbaPNG.h"
#include <list>
#include <iostream>
#include <limits>
using namespace std;

class FigureContour :public RgbaPNG {
public:
	//contructors and destructors
	FigureContour();
	FigureContour(string fileName);
	~FigureContour();
public:
	//members
	vector<Point> pointList; //lista de pontos => mudar para só ter várias retas
	vector<Color> pixelColor; //cores de cada ponto => é um´número inteiro
	vector<Point> limits; //vértices sup esquerdo e inf direito de um retagulo que engloba o domínio
private:
	//member functions
		//void findContour(vector<BYTE>& input, int width, int height, int stride, int bytesPerPixel);
	void getContour(vector<BYTE>& input, int width, int height, int stride, int bytesPerPixel, list<Point>& output);
	int pixelValue(vector<BYTE>& input, int x, int y, int width, int height, int stride, int bytesPerPixel);
	bool isBorder(vector<BYTE>& input, int x, int y, int width, int height, int stride, int bytesPerPixel);
	int maxValue(int x, int y);
	int minValue(int x, int y);
	double pointDistance(Point p1, Point p2);
	list<list <Point> ::iterator> findPath(list<Point>& input);
	list<list <Point> ::iterator> getOrder(list<Point>& pointList);
	bool compareDouble(double num1, double num2);
	void newPath(list<list <Point>::iterator>& pointList, Point& initialPoint, double& currentDistance,
		list<list<Point>::iterator>& currentPath, double& totalDistance, list<double>& distances,
		list<list<list<Point>::iterator>>& bifurcPointsAddress, list<list<list<Point>::iterator>::iterator>& bifurcsInPath,
		list<list<Point>::iterator>::iterator& currentPathIndex, list<list<double>::iterator>& bifurcsDistances,
		list<double>::iterator& currentDistanceIndex, double& currentMin);
	vector<Point> pathToPointList(list<list <Point> ::iterator> input);
	bool isExceed(double minPossible, double actualMin);
	vector<Color> getColor(vector<Point>& pointList, RgbaPNG& png);
	vector<Point> pointToPol(vector<Point>& input, vector<Color>& inputColor);
	bool isColinear(Point& p1, Point& p2, Point& p3, Color c1, Color c2, Color c3);
	vector<Point> getLimits(vector<Point>& pointList);
};